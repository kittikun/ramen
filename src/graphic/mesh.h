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
        Mesh();

        void initializeGL();
        const bool isGLInitialized() const { return m_bGLInialized; }

    private:
        enum VBOIndices : unsigned int
        {
            VBOVertex,
            VBONormal,
            VBOUV,
            VBOIndex,
            VBOCount,
        };

        // For every material, record the offsets in every VBO and triangle counts
        struct SubMesh
        {
            SubMesh() : indexOffset(0), triangleCount(0) {}

            int indexOffset;
            int triangleCount;
        };

        bool m_bByControlPoint;
        bool m_bGLInialized;
        bool m_bHasNormal;
        bool m_bHasUV;
        int32_t m_iPolygonCount;
        uint32_t m_iPolygonVertexCount;
        std::vector<float> m_normals;
        std::vector<float> m_vertices;
        std::vector<float> m_UVs;
        std::vector<SubMesh> m_subMeshes;
        std::vector<uint32_t> m_indices;
        GLuint m_vbos[VBOIndices::VBOCount];

        friend class MeshBuilder;
    };
} // namespace ramen

#endif // MESH_H
