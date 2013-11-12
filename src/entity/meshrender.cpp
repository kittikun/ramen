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
#include "../utility.h"
#include "../graphic/m_pMesh.h"
#include "../perfmon/profiler.h"

namespace ramen
{
    MeshRender::MeshRender(const boost::shared_ptr<Mesh>& m_pMesh)
        : m_pMesh(m_pMesh)
    {
    }

    void MeshRender::draw()
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_pMesh->vbo(Mesh::VBOVertex);
        glVertexAttribPointer(Mesh::VBOVertex, Mesh::StrideVertex, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(Mesh::VBOVertex);
        glBindBuffer(GL_ARRAY_BUFFER, m_pMesh->vbo(Mesh::StrideNormal);
        glVertexAttribPointer(Mesh::VBONormal, Mesh::StrideNormal, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(Mesh::StrideNormal);
        glBindBuffer(GL_ARRAY_BUFFER, m_pMesh->vbo(Mesh::StrideUV);
        glVertexAttribPointer(Mesh::VBOUV, Mesh::StrideUV, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(Mesh::StrideUV);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_pMesh->vbo(Mesh::VBOIndices);
    }

    void MeshRender::setupGL()
    {
        PROFILE;
        uint32_t size;

        // Create VBOs
        glGenBuffers(Mesh::VBOCount, &m_pMesh->vbos();

        // Save VBOVertex attributes into GPU
        glBindBuffer(GL_ARRAY_BUFFER, m_pMesh->vbo(Mesh::VBOVertex);
        size = m_pMesh->vertices().size() * sizeof(float);
        LOGGFX << "glBufferData Mesh vertices " << utility::readableSizeByte(size);
        glBufferData(GL_ARRAY_BUFFER, size, &m_pMesh->vertices().front(), GL_STATIC_DRAW);

        if (m_pMesh->hasNormal()) {
            glBindBuffer(GL_ARRAY_BUFFER,m_pMesh->vbo(Mesh::VBONormal);
            size = m_pMesh->normals().size() * sizeof(float);
            LOGGFX << "glBufferData Mesh normals " << utility::readableSizeByte(size);
            glBufferData(GL_ARRAY_BUFFER, size, &m_pMesh->normals().front(), GL_STATIC_DRAW);
        }

        if (m_pMesh->hasUV()) {
            glBindBuffer(GL_ARRAY_BUFFER, m_pMesh->vbo(Mesh::VBOUV);
            size = m_pMesh->m_UVs().size() * sizeof(float);
            LOGGFX << "glBufferData Mesh normals " << utility::readableSizeByte(size);
            glBufferData(GL_ARRAY_BUFFER, size, &m_pMesh->m_UVs().front(), GL_STATIC_DRAW);
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_pMesh->vbo(Mesh::VBOIndex);
        size = m_pMesh->indices().size() * sizeof(uint32_t);
        LOGGFX << "glBufferData Mesh normals " << utility::readableSizeByte(size);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, &m_pMesh->indices().front(), GL_STATIC_DRAW);
    }
} // namespace ramen