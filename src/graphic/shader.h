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
	class Shader
	{
	public:
		Shader(const GLenum type);
		~Shader();

		bool compile(const GLchar** data);
		bool isFragment() { return m_eType == GL_FRAGMENT_SHADER; }

		GLuint getShaderID() { return m_iShader; }
		GLenum getType() { return m_eType; }

	private:
		GLenum m_eType;
		GLuint m_iShader;
	};

	class Program
	{
	public:
		Program();
		~Program();

		bool attachShader(boost::shared_ptr<Shader> shader);
		bool link();
		bool use();

		GLint getAttribLocation(const char* name);
		GLint getUniformLocation(const char* name);

	private:
		typedef std::map<GLuint, boost::shared_ptr<Shader> > ShaderMap;

		GLuint m_iProgram;
		ShaderMap m_shaders;
	};

} // namespace ramen

#endif // SHADER_H