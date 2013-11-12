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

#ifndef MESH_H
#define MESH_H

#include <boost/cstdint.hpp>
#include <GLES2/gl2.h>

#include <vector>

namespace ramen
{
    class Mesh
    {
    public:
        // For every material, record the offsets in every VBO and triangle counts
        struct SubMesh
        {
            SubMesh() : indexOffset(0), triangleCount(0) {}

            int indexOffset;
            int triangleCount;
        };

        enum VBOIndex : unsigned int
        {
            VBOVertex,
            VBONormal,
            VBOUV,
            VBOIndices,
            VBOIndexCount
        };

        enum AttributeStride : unsigned int
        {
            StrideVertex = 4,
            StrideNormal = 3,
            StrideUV = 2
        };

        Mesh();
        ~Mesh() { int i = 0; }

        const bool hasNormal() const { return m_bHasNormal; }
        const bool hasUV() const { return m_bHasUV; }

        const bool isGLSetup() const { return m_vbos.size() != 0; }
        const bool isDataSetup() const { return m_vertices.size() != 0; }

        const GLuint vboAt(VBOIndex index) const { return m_vbos[index]; }

        const std::vector<uint16_t>& indices() const { return m_indices; }
        const std::vector<float>& vertices() const { return m_vertices; }
        const std::vector<float>& normals() const { return m_normals; }
        const std::vector<float>& UVs() const { return m_UVs; }
        const std::vector<SubMesh>& submeshes() const { return m_subMeshes; }
        std::vector<GLuint>& vbos() { return m_vbos; }

    private:
        bool m_bByControlPoint;
        bool m_bHasNormal;
        bool m_bHasUV;
        int32_t m_iPolygonCount;
        uint32_t m_iPolygonVertexCount;
        std::vector<float> m_normals;
        std::vector<float> m_vertices;
        std::vector<float> m_UVs;
        std::vector<GLuint> m_vbos;
        std::vector<SubMesh> m_subMeshes;
        std::vector<uint16_t> m_indices;

        friend class MeshBuilder;
    };
} // namespace ramen

#endif // MESH_H
