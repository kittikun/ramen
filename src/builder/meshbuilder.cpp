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
    MeshBuilder::MeshBuilder(const boost::shared_ptr<Database>& database, const std::string& name, FbxMesh* fbxMesh)
        : Job(database)
		, m_strName(name)
        , m_strUVName(nullptr)
        , m_pFbxMesh(fbxMesh)
        , m_materialIndice(nullptr)
        , m_materialMappingMode(FbxGeometryElement::eNone)
    {
    }

    void MeshBuilder::resizeArrays(boost::shared_ptr<Mesh>& m_pMesh)
    {
        FbxStringList uvNames;

        // Allocate the array memory, by control point or by polygon vertex.
        m_pMesh->m_iPolygonVertexCount = m_pFbxMesh->GetControlPointsCount();

        if (!m_pMesh->m_bByControlPoint) {
            m_pMesh->m_iPolygonVertexCount = m_pMesh->m_iPolygonCount * 3;
        }

        m_pMesh->m_vertices.resize(m_pMesh->m_iPolygonVertexCount * Mesh::StrideVertex);
        m_pMesh->m_indices.resize(m_pMesh->m_iPolygonCount * 3);

        if (m_pMesh->m_bHasNormal)	{
            m_pMesh->m_normals.resize(m_pMesh->m_iPolygonVertexCount * Mesh::StrideNormal);
        }

        m_pFbxMesh->GetUVSetNames(uvNames);

        if (m_pMesh->m_bHasUV && uvNames.GetCount()) {
            m_pMesh->m_UVs.resize(m_pMesh->m_iPolygonVertexCount * Mesh::StrideUV);
            m_strUVName = uvNames[0];
        }
    }

    void MeshBuilder::calcPolygonCountPerMaterial(boost::shared_ptr<Mesh>& m_pMesh)
    {
        if (m_pFbxMesh->GetElementMaterial()) {
            FbxLayerElementArrayTemplate<int>* m_materialIndice = &m_pFbxMesh->GetElementMaterial()->GetIndexArray();
            m_materialMappingMode = m_pFbxMesh->GetElementMaterial()->GetMappingMode();

            if (m_materialIndice && m_materialMappingMode == FbxGeometryElement::eByPolygon) {
                FBX_ASSERT(m_materialIndice->GetCount() == m_pMesh->m_iPolygonCount);
                if (m_materialIndice->GetCount() == m_pMesh->m_iPolygonCount)
                {
                    // Count the faces of each material
                    for (int32_t polygonIndex = 0; polygonIndex < m_pMesh->m_iPolygonCount; ++polygonIndex) {
                        const uint32_t materiaindex = m_materialIndice->GetAt(polygonIndex);

                        if (m_pMesh->m_subMeshes.size() < materiaindex + 1) {
                            m_pMesh->m_subMeshes.resize(materiaindex + 1);
                        }

                        m_pMesh->m_subMeshes[materiaindex].triangleCount += 1;
                    }

                    // Record the offset (how many vertex)
                    const int lMaterialCount = m_pMesh->m_subMeshes.size();
                    int offset = 0;

                    for (int index = 0; index < lMaterialCount; ++index) {
                        m_pMesh->m_subMeshes[index].indexOffset = offset;
                        offset += m_pMesh->m_subMeshes[index].triangleCount * 3;
                        // This will be used as counter in the following procedures, reset to zero
                        m_pMesh->m_subMeshes[index].triangleCount = 0;
                    }
                    FBX_ASSERT(offset == m_pMesh->m_iPolygonCount * 3);
                }
            }
        }
    }

    void MeshBuilder::getMeshAttributes(boost::shared_ptr<Mesh>& m_pMesh)
    {
        // If normal or UV is by polygon vertex, record all vertex attributes by polygon vertex.
        m_pMesh->m_bHasNormal = m_pFbxMesh->GetElementNormalCount() > 0;
        m_pMesh->m_bHasUV = m_pFbxMesh->GetElementUVCount() > 0;

        FbxGeometryElement::EMappingMode normalMappingMode = FbxGeometryElement::eNone;
        FbxGeometryElement::EMappingMode uvMappingMode = FbxGeometryElement::eNone;

        if (m_pMesh->m_bHasNormal) {
            normalMappingMode = m_pFbxMesh->GetElementNormal(0)->GetMappingMode();

            if (normalMappingMode == FbxGeometryElement::eNone) {
                m_pMesh->m_bHasNormal = false;
            }

            if (m_pMesh->m_bHasNormal && normalMappingMode != FbxGeometryElement::eByControlPoint) {
                m_pMesh->m_bByControlPoint = false;
            }
        }

        if (m_pMesh->m_bHasUV) {
            uvMappingMode = m_pFbxMesh->GetElementUV(0)->GetMappingMode();

            if (uvMappingMode == FbxGeometryElement::eNone) {
                m_pMesh->m_bHasUV = false;
            }

            if (m_pMesh->m_bHasUV && uvMappingMode != FbxGeometryElement::eByControlPoint) {
                m_pMesh->m_bByControlPoint = false;
            }
        }
    }

	const bool MeshBuilder::prepare()
	{
		if (m_pFbxMesh == nullptr) {
			LOGE << "Mesh builder: invalid FbxMesh provided";
			return false;
		}

		m_pMesh.reset(new Mesh());
		m_pDatabase->set<boost::shared_ptr<Mesh>>(m_strName, m_pMesh);

		return true;
	}

    void MeshBuilder::process()
    {
        PROFILE;
        LOGB << "Mesh builder processing \"" << m_pFbxMesh->GetName() << "\"";

        m_pMesh->m_iPolygonCount = m_pFbxMesh->GetPolygonCount();
        LOGB << "Mesh polygon count: " << m_pMesh->m_iPolygonCount;

        calcPolygonCountPerMaterial(m_pMesh);

        if (m_pMesh->m_subMeshes.size() == 0) {
            // All faces will use the same material.
            m_pMesh->m_subMeshes.resize(1);
        }

        // Congregate all the data of a m_pMesh to be cached in VBOs.
        getMeshAttributes(m_pMesh);
        resizeArrays(m_pMesh);

        // Populate the array with vertex attribute, if by control point.
        const FbxVector4 * controlPoints = m_pFbxMesh->GetControlPoints();
        FbxVector4 currentVertex;
        FbxVector4 currentNormal;
        FbxVector2 currentUV;

        if (m_pMesh->m_bByControlPoint) {
            const FbxGeometryElementNormal * normalElement = NULL;
            const FbxGeometryElementUV * uvElement = NULL;

            if (m_pMesh->m_bHasNormal) {
                normalElement = m_pFbxMesh->GetElementNormal(0);
            }

            if (m_pMesh->m_bHasUV) {
                uvElement = m_pFbxMesh->GetElementUV(0);
            }

            for (uint32_t index = 0; index < m_pMesh->m_iPolygonVertexCount; ++index) {
                // Save the vertex position.
                currentVertex = controlPoints[index];
                m_pMesh->m_vertices[index * Mesh::StrideVertex] = static_cast<float>(currentVertex[0]);
                m_pMesh->m_vertices[index * Mesh::StrideVertex + 1] = static_cast<float>(currentVertex[1]);
                m_pMesh->m_vertices[index * Mesh::StrideVertex + 2] = static_cast<float>(currentVertex[2]);
                m_pMesh->m_vertices[index * Mesh::StrideVertex + 3] = 1;

                // Save the normal.
                if (m_pMesh->m_bHasNormal)	{
                    int lNormaindex = index;

                    if (normalElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect) {
                        lNormaindex = normalElement->GetIndexArray().GetAt(index);
                    }

                    currentNormal = normalElement->GetDirectArray().GetAt(lNormaindex);
                    m_pMesh->m_normals[index * Mesh::StrideNormal] = static_cast<float>(currentNormal[0]);
                    m_pMesh->m_normals[index * Mesh::StrideNormal + 1] = static_cast<float>(currentNormal[1]);
                    m_pMesh->m_normals[index * Mesh::StrideNormal + 2] = static_cast<float>(currentNormal[2]);
                }

                // Save the UV.
                if (m_pMesh->m_bHasUV) {
                    int lUVIndex = index;
                    if (uvElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect) {
                        lUVIndex = uvElement->GetIndexArray().GetAt(index);
                    }
                    currentUV = uvElement->GetDirectArray().GetAt(lUVIndex);
                    m_pMesh->m_UVs[index * Mesh::StrideUV] = static_cast<float>(currentUV[0]);
                    m_pMesh->m_UVs[index * Mesh::StrideUV + 1] = static_cast<float>(currentUV[1]);
                }
            }
        }

        int vertexCount = 0;
        for (int32_t polygonIndex = 0; polygonIndex < m_pMesh->m_iPolygonCount; ++polygonIndex)	{
            // The material for current face.
            int materiaindex = 0;
            if (m_materialIndice && m_materialMappingMode == FbxGeometryElement::eByPolygon) {
                materiaindex = m_materialIndice->GetAt(polygonIndex);
            }

            // Where should I save the vertex attribute index, according to the material
            const int indexOffset = m_pMesh->m_subMeshes[materiaindex].indexOffset + m_pMesh->m_subMeshes[materiaindex].triangleCount * 3;
			
            for (int verticeIndex = 0; verticeIndex < (Mesh::StrideVertex - 1); ++verticeIndex)	{
                const int controlPointIndex = m_pFbxMesh->GetPolygonVertex(polygonIndex, verticeIndex);

                if (m_pMesh->m_bByControlPoint)	{
                    m_pMesh->m_indices[indexOffset + verticeIndex] = static_cast<uint16_t>(controlPointIndex);
                } else {
                    // Populate the array with vertex attribute, if by polygon vertex.
                    m_pMesh->m_indices[indexOffset + verticeIndex] = static_cast<uint16_t>(vertexCount);

                    currentVertex = controlPoints[controlPointIndex];
                    m_pMesh->m_vertices[vertexCount * Mesh::StrideVertex] = static_cast<float>(currentVertex[0]);
                    m_pMesh->m_vertices[vertexCount * Mesh::StrideVertex + 1] = static_cast<float>(currentVertex[1]);
                    m_pMesh->m_vertices[vertexCount * Mesh::StrideVertex + 2] = static_cast<float>(currentVertex[2]);
                    m_pMesh->m_vertices[vertexCount * Mesh::StrideVertex + 3] = 1;

                    if (m_pMesh->m_bHasNormal)	{
                        m_pFbxMesh->GetPolygonVertexNormal(polygonIndex, verticeIndex, currentNormal);
                        m_pMesh->m_normals[vertexCount * Mesh::StrideNormal] = static_cast<float>(currentNormal[0]);
                        m_pMesh->m_normals[vertexCount * Mesh::StrideNormal + 1] = static_cast<float>(currentNormal[1]);
                        m_pMesh->m_normals[vertexCount * Mesh::StrideNormal + 2] = static_cast<float>(currentNormal[2]);
                    }

                    if (m_pMesh->m_bHasUV)	{
                        bool lUnmappedUV;
                        m_pFbxMesh->GetPolygonVertexUV(polygonIndex, verticeIndex, m_strUVName, currentUV, lUnmappedUV);
                        m_pMesh->m_UVs[vertexCount * Mesh::StrideUV] = static_cast<float>(currentUV[0]);
                        m_pMesh->m_UVs[vertexCount * Mesh::StrideUV + 1] = static_cast<float>(currentUV[1]);
                    }
                }
                ++vertexCount;
            }
            ++m_pMesh->m_subMeshes[materiaindex].triangleCount;
        }
    }
} // namespace ramen