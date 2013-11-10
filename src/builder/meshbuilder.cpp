//  Copyright (C) 2013  kittikun
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along
//  with this program; if not, write to the Free Software Foundation, Inc.,
//  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

#include "meshbuilder.h"

#include "../database.h"
#include "../log.h"
#include "../perfmon/profiler.h"

namespace ramen
{
    MeshBuilder::MeshBuilder(const boost::shared_ptr<Database>& database, FbxMesh* fbxMesh)
        : Job(database)
        , m_strUVName(nullptr)
        ,  m_pFbxMesh(fbxMesh)
        , m_materialIndice(nullptr)
        , m_materialMappingMode(FbxGeometryElement::eNone)
    {
    }

    void MeshBuilder::allocateArrays(boost::shared_ptr<Mesh>& mesh)
    {
        FbxStringList uvNames;

        // Allocate the array memory, by control point or by polygon vertex.
        mesh->m_iPolygonVertexCount = m_pFbxMesh->GetControlPointsCount();

        if (!mesh->m_bByControlPoint) {
            mesh->m_iPolygonVertexCount = mesh->m_iPolygonCount * 3;
        }

        mesh->m_vertices.resize(mesh->m_iPolygonVertexCount * 4);
        mesh->m_indices.resize(mesh->m_iPolygonCount * 3);

        if (mesh->m_bHasNormal)	{
            mesh->m_normals.resize(mesh->m_iPolygonVertexCount * 3);
        }

        m_pFbxMesh->GetUVSetNames(uvNames);

        if (mesh->m_bHasUV && uvNames.GetCount()) {
            mesh->m_UVs.resize(mesh->m_iPolygonVertexCount * 2);
            m_strUVName = uvNames[0];
        }
    }

    void MeshBuilder::getMeshAttributes(boost::shared_ptr<Mesh>& mesh)
    {
        // If normal or UV is by polygon vertex, record all vertex attributes by polygon vertex.
        mesh->m_bHasNormal = m_pFbxMesh->GetElementNormalCount() > 0;
        mesh->m_bHasUV = m_pFbxMesh->GetElementUVCount() > 0;

        FbxGeometryElement::EMappingMode normalMappingMode = FbxGeometryElement::eNone;
        FbxGeometryElement::EMappingMode uvMappingMode = FbxGeometryElement::eNone;

        if (mesh->m_bHasNormal) {
            normalMappingMode = m_pFbxMesh->GetElementNormal(0)->GetMappingMode();

            if (normalMappingMode == FbxGeometryElement::eNone) {
                mesh->m_bHasNormal = false;
            }

            if (mesh->m_bHasNormal && normalMappingMode != FbxGeometryElement::eByControlPoint) {
                mesh->m_bByControlPoint = false;
            }
        }

        if (mesh->m_bHasUV) {
            uvMappingMode = m_pFbxMesh->GetElementUV(0)->GetMappingMode();

            if (uvMappingMode == FbxGeometryElement::eNone) {
                mesh->m_bHasUV = false;
            }

            if (mesh->m_bHasUV && uvMappingMode != FbxGeometryElement::eByControlPoint) {
                mesh->m_bByControlPoint = false;
            }
        }
    }

    void MeshBuilder::calcPolygonCountPerMaterial(boost::shared_ptr<Mesh>& mesh)
    {
        if (m_pFbxMesh->GetElementMaterial()) {
            FbxLayerElementArrayTemplate<int>* m_materialIndice = &m_pFbxMesh->GetElementMaterial()->GetIndexArray();
            m_materialMappingMode = m_pFbxMesh->GetElementMaterial()->GetMappingMode();

            if (m_materialIndice && m_materialMappingMode == FbxGeometryElement::eByPolygon) {
                FBX_ASSERT(m_materialIndice->GetCount() == mesh->m_iPolygonCount);
                if (m_materialIndice->GetCount() == mesh->m_iPolygonCount)
                {
                    // Count the faces of each material
                    for (uint32_t polygonIndex = 0; polygonIndex < mesh->m_iPolygonCount; ++polygonIndex) {
                        const uint32_t materiaindex = m_materialIndice->GetAt(polygonIndex);

                        if (mesh->m_subMeshes.size() < materiaindex + 1) {
                            mesh->m_subMeshes.resize(materiaindex + 1);
                        }

                        mesh->m_subMeshes[materiaindex].triangleCount += 1;
                    }

                    // Record the offset (how many vertex)
                    const int lMaterialCount = mesh->m_subMeshes.size();
                    int offset = 0;

                    for (int index = 0; index < lMaterialCount; ++index) {
                        mesh->m_subMeshes[index].indexOffset = offset;
                        offset += mesh->m_subMeshes[index].triangleCount * 3;
                        // This will be used as counter in the following procedures, reset to zero
                        mesh->m_subMeshes[index].triangleCount = 0;
                    }
                    FBX_ASSERT(offset == mesh->m_iPolygonCount * 3);
                }
            }
        }
    }

    void MeshBuilder::process()
    {
        PROFILE;
        LOGB << "Meshbuilder processing \"" << m_pFbxMesh->GetName() << "\"";
        boost::shared_ptr<Mesh> mesh(new Mesh());

        if (m_pFbxMesh == nullptr) {
            LOGE << "Could invalid FbxMesh provided";
            return;
        }

        mesh->m_iPolygonCount = m_pFbxMesh->GetPolygonCount();
        LOGB << "Mesh polygon count: " << mesh->m_iPolygonCount;

        calcPolygonCountPerMaterial(mesh);

        if (mesh->m_subMeshes.size() == 0) {
            // All faces will use the same material.
            mesh->m_subMeshes.resize(1);
        }

        // Congregate all the data of a mesh to be cached in VBOs.
        getMeshAttributes(mesh);
        allocateArrays(mesh);

        // Populate the array with vertex attribute, if by control point.
        const FbxVector4 * controlPoints = m_pFbxMesh->GetControlPoints();
        FbxVector4 currentVertex;
        FbxVector4 currentNormal;
        FbxVector2 currentUV;

        if (mesh->m_bByControlPoint) {
            const FbxGeometryElementNormal * normalElement = NULL;
            const FbxGeometryElementUV * uvElement = NULL;

            if (mesh->m_bHasNormal) {
                normalElement = m_pFbxMesh->GetElementNormal(0);
            }

            if (mesh->m_bHasUV) {
                uvElement = m_pFbxMesh->GetElementUV(0);
            }

            for (uint32_t index = 0; index < mesh->m_iPolygonVertexCount; ++index) {
                // Save the vertex position.
                currentVertex = controlPoints[index];
                mesh->m_vertices[index * 4] = static_cast<float>(currentVertex[0]);
                mesh->m_vertices[index * 4 + 1] = static_cast<float>(currentVertex[1]);
                mesh->m_vertices[index * 4 + 2] = static_cast<float>(currentVertex[2]);
                mesh->m_vertices[index * 4 + 3] = 1;

                // Save the normal.
                if (mesh->m_bHasNormal)	{
                    int lNormaindex = index;

                    if (normalElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect) {
                        lNormaindex = normalElement->GetIndexArray().GetAt(index);
                    }

                    currentNormal = normalElement->GetDirectArray().GetAt(lNormaindex);
                    mesh->m_normals[index * 3] = static_cast<float>(currentNormal[0]);
                    mesh->m_normals[index * 3 + 1] = static_cast<float>(currentNormal[1]);
                    mesh->m_normals[index * 3 + 2] = static_cast<float>(currentNormal[2]);
                }

                // Save the UV.
                if (mesh->m_bHasUV) {
                    int lUVIndex = index;
                    if (uvElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect) {
                        lUVIndex = uvElement->GetIndexArray().GetAt(index);
                    }
                    currentUV = uvElement->GetDirectArray().GetAt(lUVIndex);
                    mesh->m_UVs[index * 2] = static_cast<float>(currentUV[0]);
                    mesh->m_UVs[index * 2 + 1] = static_cast<float>(currentUV[1]);
                }
            }
        }

        int vertexCount = 0;
        for (uint32_t polygonIndex = 0; polygonIndex < mesh->m_iPolygonCount; ++polygonIndex)	{
            // The material for current face.
            int materiaindex = 0;
            if (m_materialIndice && m_materialMappingMode == FbxGeometryElement::eByPolygon) {
                materiaindex = m_materialIndice->GetAt(polygonIndex);
            }

            // Where should I save the vertex attribute index, according to the material
            const int indexOffset = mesh->m_subMeshes[materiaindex].indexOffset + mesh->m_subMeshes[materiaindex].triangleCount * 3;

            for (int verticeIndex = 0; verticeIndex < 3; ++verticeIndex)	{
                const int controlPointIndex = m_pFbxMesh->GetPolygonVertex(polygonIndex, verticeIndex);

                if (mesh->m_bByControlPoint)	{
                    mesh->m_indices[indexOffset + verticeIndex] = static_cast<unsigned int>(controlPointIndex);
                } else {
                    // Populate the array with vertex attribute, if by polygon vertex.
                    mesh->m_indices[indexOffset + verticeIndex] = static_cast<unsigned int>(vertexCount);

                    currentVertex = controlPoints[controlPointIndex];
                    mesh->m_vertices[vertexCount * 4] = static_cast<float>(currentVertex[0]);
                    mesh->m_vertices[vertexCount * 4 + 1] = static_cast<float>(currentVertex[1]);
                    mesh->m_vertices[vertexCount * 4 + 2] = static_cast<float>(currentVertex[2]);
                    mesh->m_vertices[vertexCount * 4 + 3] = 1;

                    if (mesh->m_bHasNormal)	{
                        m_pFbxMesh->GetPolygonVertexNormal(polygonIndex, verticeIndex, currentNormal);
                        mesh->m_normals[vertexCount * 3] = static_cast<float>(currentNormal[0]);
                        mesh->m_normals[vertexCount * 3 + 1] = static_cast<float>(currentNormal[1]);
                        mesh->m_normals[vertexCount * 3 + 2] = static_cast<float>(currentNormal[2]);
                    }

                    if (mesh->m_bHasUV)	{
                        bool lUnmappedUV;
                        m_pFbxMesh->GetPolygonVertexUV(polygonIndex, verticeIndex, m_strUVName, currentUV, lUnmappedUV);
                        mesh->m_UVs[vertexCount * 2] = static_cast<float>(currentUV[0]);
                        mesh->m_UVs[vertexCount * 2 + 1] = static_cast<float>(currentUV[1]);
                    }
                }
                ++vertexCount;
            }
            mesh->m_subMeshes[materiaindex].triangleCount += 1;
        }

        m_pDatabase->set<boost::shared_ptr<Mesh>>("mesh", mesh);
    }
} // namespace ramen