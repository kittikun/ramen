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

#include "meshrender.h"

#include <GLES2/gl2.h>

#include "../log.h"
#include "../graphic/mesh.h"
#include "../perfmon/profiler.h"

namespace ramen
{
    MeshRender::MeshRender(const boost::shared_ptr<Mesh>& mesh)
        : m_pMesh(mesh)
    {
    }

    void MeshRender::draw()
    {
        glBindBuffer(GL_ARRAY_BUFFER, mesh->vbos()[Mesh::VBOVertex]);
        glVertexAttribPointer(Mesh::VBOVertex, 4, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(Mesh::VBOVertex);
    }

    void MeshRender::setupGL()
    {
        PROFILE;
        uint32_t size;

        // Create VBOs
        glGenBuffers(Mesh::VBOCount, &m_vbos.front());

        // Save VBOVertex attributes into GPU
        glBindBuffer(GL_ARRAY_BUFFER, mesh->vbos()[Mesh::VBOVertex]);
        size = m_vertices.size() * sizeof(float);
        LOGGFX << "glBufferData Mesh vertices " << utility::readableSizeByte(size);
        glBufferData(GL_ARRAY_BUFFER, size, &m_vertices.front(), GL_STATIC_DRAW);

        if (mesh->hasNormal()) {
            glBindBuffer(GL_ARRAY_BUFFER,mesh->vbos()[Mesh::VBONormal]);
            size = m_normals.size() * sizeof(float);
            LOGGFX << "glBufferData Mesh normals " << utility::readableSizeByte(size);
            glBufferData(GL_ARRAY_BUFFER, size, &m_normals.front(), GL_STATIC_DRAW);
        }

        if (mesh->hasUV()) {
            glBindBuffer(GL_ARRAY_BUFFER, mesh->vbos()[Mesh::VBOUV]);
            size = m_UVs.size() * sizeof(float);
            LOGGFX << "glBufferData Mesh normals " << utility::readableSizeByte(size);
            glBufferData(GL_ARRAY_BUFFER, size, &m_UVs.front(), GL_STATIC_DRAW);
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->vbos()[Mesh::VBOIndex]);
        size = m_indices.size() * sizeof(float);
        LOGGFX << "glBufferData Mesh normals " << utility::readableSizeByte(size);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.front() * sizeof(unsigned int), &m_indices.front(), GL_STATIC_DRAW);
    }
} // namespace ramen