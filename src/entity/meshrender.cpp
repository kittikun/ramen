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

#include <boost/foreach.hpp>
#include <GLES2/gl2.h>

#include "../log.h"
#include "../utility.h"
#include "../graphic/mesh.h"
#include "../graphic/shader.h"
#include "../graphic/graphicUtility.h"
#include "../perfmon/profiler.h"

namespace ramen
{
	MeshRender::MeshRender(const boost::shared_ptr<Mesh>& m_pMesh)
		: m_pMesh(m_pMesh)
	{
	}

	void MeshRender::draw()
	{
		if (!m_pMesh->isDataSetup()) {
			return;
		}

		if (!m_pMesh->isGLSetup()) {
			if (!setupGL()) {
				return;
			}
		}

		glBindBuffer(GL_ARRAY_BUFFER, m_pMesh->vboAt(Mesh::VBOVertex));
		glVertexAttribPointer(Mesh::VBOVertex, Mesh::StrideVertex, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(Mesh::VBOVertex);

		if (m_pMesh->hasNormal()) {
			glBindBuffer(GL_ARRAY_BUFFER, m_pMesh->vboAt(Mesh::VBONormal));
			glVertexAttribPointer(Mesh::VBONormal, Mesh::StrideNormal, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(Mesh::VBONormal);
		}

		if (m_pMesh->hasUV()) {
			glBindBuffer(GL_ARRAY_BUFFER, m_pMesh->vboAt(Mesh::VBOUV));
			glVertexAttribPointer(Mesh::VBOUV, Mesh::StrideUV, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(Mesh::VBOUV);
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_pMesh->vboAt(Mesh::VBOIndices));

		BOOST_FOREACH(auto sub, m_pMesh->submeshes()) {
			const GLsizei offset = sub.indexOffset * sizeof(uint16_t);
			const GLsizei elementCount = sub.triangleCount * 3;

			glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_SHORT, reinterpret_cast<const GLvoid *>(offset));
		}

		for (int i = 0; i < Mesh::VBOIndexCount; ++i) {
			glDisableVertexAttribArray(i);
		}
	}

	const bool MeshRender::setupGL()
	{
		PROFILE;
		size_t size;

		// Create VBOs
		m_pMesh->vbos().resize(Mesh::VBOIndexCount);
		glGenBuffers(Mesh::VBOIndexCount, &m_pMesh->vbos().front());

		// Save VBOVertex attributes into GPU
		glBindBuffer(GL_ARRAY_BUFFER, m_pMesh->vboAt(Mesh::VBOVertex));
		size = m_pMesh->vertices().size() * sizeof(float);
		LOGGFX << "glBufferData Mesh vertices " << utility::readableSizeByte(size);
		glBufferData(GL_ARRAY_BUFFER, size, &m_pMesh->vertices().front(), GL_STATIC_DRAW);
		VERIFYGL();

		if (m_pMesh->hasNormal()) {
			glBindBuffer(GL_ARRAY_BUFFER, m_pMesh->vboAt(Mesh::VBONormal));
			size = m_pMesh->normals().size() * sizeof(float);
			LOGGFX << "glBufferData Mesh normals " << utility::readableSizeByte(size);
			glBufferData(GL_ARRAY_BUFFER, size, &m_pMesh->normals().front(), GL_STATIC_DRAW);
			VERIFYGL();
		}

		if (m_pMesh->hasUV()) {
			glBindBuffer(GL_ARRAY_BUFFER, m_pMesh->vboAt(Mesh::VBOUV));
			size = m_pMesh->UVs().size() * sizeof(float);
			LOGGFX << "glBufferData Mesh UVs " << utility::readableSizeByte(size);
			glBufferData(GL_ARRAY_BUFFER, size, &m_pMesh->UVs().front(), GL_STATIC_DRAW);
			VERIFYGL();
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_pMesh->vboAt(Mesh::VBOIndices));
		size = m_pMesh->indices().size() * sizeof(uint16_t);
		LOGGFX << "glBufferData Mesh indices " << utility::readableSizeByte(size);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, &m_pMesh->indices().front(), GL_STATIC_DRAW);
		VERIFYGL();

		return true;
	}
} // namespace ramen