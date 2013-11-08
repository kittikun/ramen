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

#include "mesh.h"

#include "../log.h"
#include "../utility.h"
#include "../perfmon/profiler.h"

namespace ramen
{
    Mesh::Mesh()
        : m_bByControlPoint(true)
        , m_bGLInialized(false)
        , m_bHasNormal(false)
        , m_bHasUV(false)
        , m_iPolygonCount(0)
        , m_iPolygonVertexCount(0)
    {
    }

    void Mesh::initializeGL()
    {
        PROFILE;
        uint32_t size;

        // Create VBOs
        glGenBuffers(VBOCount, m_vbos);

        // Save VBOVertex attributes into GPU
        glBindBuffer(GL_ARRAY_BUFFER, m_vbos[VBOVertex]);
        size = m_vertices.size() * sizeof(float);
        LOGGFX << "glBufferData Mesh vertices " << utility::readableSizeByte(size);
        glBufferData(GL_ARRAY_BUFFER, size, &m_vertices.front(), GL_STATIC_DRAW);

        if (m_bHasNormal) {
            glBindBuffer(GL_ARRAY_BUFFER, m_vbos[VBONormal]);
            size = m_normals.size() * sizeof(float);
            LOGGFX << "glBufferData Mesh normals " << utility::readableSizeByte(size);
            glBufferData(GL_ARRAY_BUFFER, size, &m_normals.front(), GL_STATIC_DRAW);
        }

        if (m_bHasUV) {
            glBindBuffer(GL_ARRAY_BUFFER, m_vbos[VBOUV]);
            size = m_UVs.size() * sizeof(float);
            LOGGFX << "glBufferData Mesh normals " << utility::readableSizeByte(size);
            glBufferData(GL_ARRAY_BUFFER, size, &m_UVs.front(), GL_STATIC_DRAW);
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbos[VBOIndex]);
        size = m_indices.size() * sizeof(float);
        LOGGFX << "glBufferData Mesh normals " << utility::readableSizeByte(size);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.front() * sizeof(unsigned int), &m_indices.front(), GL_STATIC_DRAW);
    }
} // namespace ramen