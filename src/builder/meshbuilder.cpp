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

namespace ramen
{
    MeshBuilder::MeshBuilder(const boost::shared_ptr<Database>& database, FbxMesh* fbxMesh)
        : Job(database)
        ,  m_pFbxMesh(fbxMesh)
    {
    }

    void MeshBuilder::process()
    {
        LOGB << "Meshbuilder " << m_pFbxMesh->GetInitialName();
        boost::shared_ptr<Mesh> mesh(new Mesh());
        int polygonCount;

        if (m_pFbxMesh == nullptr) {
            LOGE << "Could invalid FbxMesh provided";
            return;
        }

        polygonCount = m_pFbxMesh->GetPolygonCount();
        LOGI << "Mesh polygon count: " << polygonCount;

        // Count the polygon count of each material
        FbxLayerElementArrayTemplate<int>* materialIndice = nullptr;
        FbxGeometryElement::EMappingMode materialMappingMode = FbxGeometryElement::eNone;

        if (m_pFbxMesh->GetElementMaterial()) {
            materialIndice = &m_pFbxMesh->GetElementMaterial()->GetIndexArray();
            materialMappingMode = m_pFbxMesh->GetElementMaterial()->GetMappingMode();

            if (materialIndice && materialMappingMode == FbxGeometryElement::eByPolygon) {
                FBX_ASSERT(materialIndice->GetCount() == polygonCount);
                if (materialIndice->GetCount() == polygonCount)
                {
                    // Count the faces of each material
                    for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex) {
                        const uint32_t materialIndex = materialIndice->GetAt(polygonIndex);

                        if (mesh->m_subMeshes.size() < materialIndex + 1) {
                            mesh->m_subMeshes.resize(materialIndex + 1);
                        }

                        mesh->m_subMeshes[materialIndex].triangleCount += 1;
                    }

                    // Record the offset (how many vertex)
                    const int lMaterialCount = mesh->m_subMeshes.size();
                    int lOffset = 0;

                    for (int lIndex = 0; lIndex < lMaterialCount; ++lIndex) {
                        mesh->m_subMeshes[lIndex].indexOffset = lOffset;
                        lOffset += mesh->m_subMeshes[lIndex].triangleCount * 3;
                        // This will be used as counter in the following procedures, reset to zero
                        mesh->m_subMeshes[lIndex].triangleCount = 0;
                    }
                    FBX_ASSERT(lOffset == polygonCount * 3);
                }
            }
        }

        // All faces will use the same material.
        if (mesh->m_subMeshes.size() == 0) {
            mesh->m_subMeshes.resize(1);
        }

        // Congregate all the data of a mesh to be cached in VBOs.
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

        // Allocate the array memory, by control point or by polygon vertex.
        int polygonVertexCount = m_pFbxMesh->GetControlPointsCount();
        if (!mesh->m_bByControlPoint) {
            polygonVertexCount = polygonCount * 3;
        }

        float * vertices = new float[polygonVertexCount * 4];
        unsigned int * indices = new unsigned int[polygonCount * 3];
        float * normals = NULL;

        if (mesh->m_bHasNormal)	{
            normals = new float[polygonVertexCount * 3];
        }

        float * lUVs = NULL;
        FbxStringList uvNames;
        m_pFbxMesh->GetUVSetNames(uvNames);
        const char * lUVName = NULL;
        if (mesh->m_bHasUV && uvNames.GetCount())
        {
            lUVs = new float[polygonVertexCount * 2];
            lUVName = uvNames[0];
        }

        // Populate the array with vertex attribute, if by control point.
        const FbxVector4 * controlPoints = m_pFbxMesh->GetControlPoints();
        FbxVector4 lCurrentVertex;
        FbxVector4 lCurrentNormal;
        FbxVector2 lCurrentUV;

        if (mesh->m_bByControlPoint) {
            const FbxGeometryElementNormal * lNormalElement = NULL;
            const FbxGeometryElementUV * lUVElement = NULL;

            if (mesh->m_bHasNormal) {
                lNormalElement = m_pFbxMesh->GetElementNormal(0);
            }

            if (mesh->m_bHasUV) {
                lUVElement = m_pFbxMesh->GetElementUV(0);
            }

            for (int lIndex = 0; lIndex < polygonVertexCount; ++lIndex) {
                // Save the vertex position.
                lCurrentVertex = controlPoints[lIndex];
                vertices[lIndex * 4] = static_cast<float>(lCurrentVertex[0]);
                vertices[lIndex * 4 + 1] = static_cast<float>(lCurrentVertex[1]);
                vertices[lIndex * 4 + 2] = static_cast<float>(lCurrentVertex[2]);
                vertices[lIndex * 4 + 3] = 1;

                // Save the normal.
                if (mesh->m_bHasNormal)	{
                    int lNormalIndex = lIndex;

                    if (lNormalElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect) {
                        lNormalIndex = lNormalElement->GetIndexArray().GetAt(lIndex);
                    }

                    lCurrentNormal = lNormalElement->GetDirectArray().GetAt(lNormalIndex);
                    normals[lIndex * 3] = static_cast<float>(lCurrentNormal[0]);
                    normals[lIndex * 3 + 1] = static_cast<float>(lCurrentNormal[1]);
                    normals[lIndex * 3 + 2] = static_cast<float>(lCurrentNormal[2]);
                }

                // Save the UV.
                if (mesh->m_bHasUV) {
                    int lUVIndex = lIndex;
                    if (lUVElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect) {
                        lUVIndex = lUVElement->GetIndexArray().GetAt(lIndex);
                    }
                    lCurrentUV = lUVElement->GetDirectArray().GetAt(lUVIndex);
                    lUVs[lIndex * 2] = static_cast<float>(lCurrentUV[0]);
                    lUVs[lIndex * 2 + 1] = static_cast<float>(lCurrentUV[1]);
                }
            }
        }

        int vertexCount = 0;
        for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex)	{
            // The material for current face.
            int materialIndex = 0;
            if (materialIndice && materialMappingMode == FbxGeometryElement::eByPolygon) {
                materialIndex = materialIndice->GetAt(polygonIndex);
            }

            // Where should I save the vertex attribute index, according to the material
            const int indexOffset = mesh->m_subMeshes[materialIndex].indexOffset + mesh->m_subMeshes[materialIndex].triangleCount * 3;

            for (int verticeIndex = 0; verticeIndex < 3; ++verticeIndex)	{
                const int controlPointIndex = m_pFbxMesh->GetPolygonVertex(polygonIndex, verticeIndex);

                if (mesh->m_bByControlPoint)	{
                    indices[indexOffset + verticeIndex] = static_cast<unsigned int>(controlPointIndex);
                } else {
                    // Populate the array with vertex attribute, if by polygon vertex.
                    indices[indexOffset + verticeIndex] = static_cast<unsigned int>(vertexCount);

                    lCurrentVertex = controlPoints[controlPointIndex];
                    vertices[vertexCount * 4] = static_cast<float>(lCurrentVertex[0]);
                    vertices[vertexCount * 4 + 1] = static_cast<float>(lCurrentVertex[1]);
                    vertices[vertexCount * 4 + 2] = static_cast<float>(lCurrentVertex[2]);
                    vertices[vertexCount * 4 + 3] = 1;

                    if (mesh->m_bHasNormal)	{
                        m_pFbxMesh->GetPolygonVertexNormal(polygonIndex, verticeIndex, lCurrentNormal);
                        normals[vertexCount * 3] = static_cast<float>(lCurrentNormal[0]);
                        normals[vertexCount * 3 + 1] = static_cast<float>(lCurrentNormal[1]);
                        normals[vertexCount * 3 + 2] = static_cast<float>(lCurrentNormal[2]);
                    }

                    if (mesh->m_bHasUV)	{
                        bool lUnmappedUV;
                        m_pFbxMesh->GetPolygonVertexUV(polygonIndex, verticeIndex, lUVName, lCurrentUV, lUnmappedUV);
                        lUVs[vertexCount * 2] = static_cast<float>(lCurrentUV[0]);
                        lUVs[vertexCount * 2 + 1] = static_cast<float>(lCurrentUV[1]);
                    }
                }
                ++vertexCount;
            }
            mesh->m_subMeshes[materialIndex].triangleCount += 1;
        }
    }
} // namespace ramen