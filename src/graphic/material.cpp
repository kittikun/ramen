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

#include "material.h"

#include <glm/ext.hpp>

#include "mesh.h"
#include "shader.h"

namespace ramen {
	const bool Material::setupGL()
	{
		boost::shared_ptr<Shader> vtx(new Shader(GL_VERTEX_SHADER, "mesh.v"));
		boost::shared_ptr<Shader> frg(new Shader(GL_FRAGMENT_SHADER, "mesh.f"));

		// Create shaders
		if (!vtx->compile() || !frg->compile()) {
			return false;
		}

		m_pProgram.reset(new Program(vtx, frg));
		m_pProgram->bindAttribLocation(Mesh::VBOVertex, "pos");
		m_pProgram->bindAttribLocation(Mesh::VBONormal, "normal");
		m_pProgram->bindAttribLocation(Mesh::VBOUV, "uv");

		if (!m_pProgram->link()) {
			return false;
		}
	}

	void Material::mvp(const glm::mat4x4& mvp) const
	{
		glUniformMatrix4fv(m_pProgram->uniformLocation("mvp"), 1, false, glm::value_ptr(mvp));
	}

	void Material::use() const
	{
		m_pProgram->use();
	}
} // namespace ramen