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

#ifndef SHADER_H
#define SHADER_H

#include <boost/shared_ptr.hpp>
#include <GLES2/gl2.h>
#include <map>

namespace ramen
{
	class Filesystem;

	class Shader
	{
	public:
		Shader(const GLenum type);
		Shader(const GLenum type, const std::string& filename);
		~Shader();

		static void setFilesystem(const boost::shared_ptr<Filesystem>& filesystem);

		const bool loadFromMemory(const GLchar** data) const;
		const bool loadFromFile(const std::string& filename) const;
		const bool compile();

		const bool isFragment() const { return m_eType == GL_FRAGMENT_SHADER; }

		const GLuint shaderID() const { return m_iShaderID; }

	private:
		static boost::shared_ptr<Filesystem> m_pFilesystem;
		GLenum m_eType;
		GLuint m_iShaderID;
	};

	class Program
	{
	public:
		Program();
		Program(const boost::shared_ptr<Shader>& vert, const boost::shared_ptr<Shader>& frag);
		~Program();

		const bool attachShader(const boost::shared_ptr<Shader>& shader);
		void bindAttribLocation(GLuint index, const std::string& name);
		const bool link();
		const bool use();

		const GLint attribLocation(const std::string& name) const;
		const GLint uniformLocation(const std::string& name) const;

	private:
		GLuint m_iProgram;
		std::map<GLuint, boost::shared_ptr<Shader>> m_shaders;
	};
} // namespace ramen

#endif // SHADER_H