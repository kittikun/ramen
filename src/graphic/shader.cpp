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

#include "shader.h"

#include "../log.h"
#include "../io/filesystem.h"
#include "graphicUtility.h"

namespace ramen
{
    //-------------------------------------------------------------------------------------
    // SHADER
    //-------------------------------------------------------------------------------------
	boost::shared_ptr<Filesystem> Shader::m_pFilesystem;

    Shader::Shader(const GLenum type)
        : m_eType(type)
    {
        m_iShaderID = glCreateShader(m_eType);
        VERIFYGL();
    }

	Shader::Shader(const GLenum type, const std::string& filename)
		: m_eType(type)
	{
		m_iShaderID = glCreateShader(m_eType);
		VERIFYGL();

		loadFromFile(filename);
	}

    Shader::~Shader()
    {
		glDeleteShader(m_iShaderID);
    }

    const bool Shader::compile()
    {
        GLint compiled;

        if (m_iShaderID == 0) {
            LOGE << "Cannot compile shader with an ID of 0";
            return false;
        }

        glCompileShader(m_iShaderID);
        VERIFYGL();

        // Check the compile status
        glGetShaderiv(m_iShaderID, GL_COMPILE_STATUS, &compiled);

        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(m_iShaderID, GL_INFO_LOG_LENGTH, &infoLen);

            if (infoLen > 1) {
                std::vector<char> infoLog(infoLen);
                GLsizei srcLen;

                glGetShaderInfoLog(m_iShaderID, infoLen, NULL, &infoLog[0]);
                LOGE << "Error compiling shader:\n" << std::string(infoLog.begin(), infoLog.end());

                glGetShaderiv(m_iShaderID, GL_SHADER_SOURCE_LENGTH, &infoLen);
                infoLog.resize(infoLen);

                glGetShaderSource(m_iShaderID, infoLen, &srcLen, &infoLog[0]);
                LOGE << "Shader source:\n" << std::string(infoLog.begin(), infoLog.end());
            }

            return false;
        }

        return true;
    }

    const bool Shader::loadFromMemory(const GLchar** data) const
    {
        glShaderSource(m_iShaderID, 1, data, nullptr);
        VERIFYGL_RET();

        return true;
    }

	const bool Shader::loadFromFile(const std::string& filename) const
	{
		boost::shared_array<char> raw = m_pFilesystem->resource(Filesystem::ResourceType::Shader, filename);;
		const GLchar* data = nullptr;

		if (raw == nullptr) {
			return false;
		}

		data = raw.get();
		return loadFromMemory(&data);
	}

	void Shader::setFilesystem(const boost::shared_ptr<Filesystem>& filesystem)
	{
		assert(m_pFilesystem == nullptr);
		m_pFilesystem = filesystem;
	}

    //-------------------------------------------------------------------------------------
    // PROGRAM
    //-------------------------------------------------------------------------------------
    Program::Program()
    {
        m_iProgram = glCreateProgram();
    }

	Program::Program(const boost::shared_ptr<Shader>& vert, const boost::shared_ptr<Shader>& frag)
	{
		m_iProgram = glCreateProgram();
		assert(attachShader(vert));
		assert(attachShader(frag));
	}


    Program::~Program()
    {
        glDeleteProgram(m_iProgram);
    }

    const bool Program::attachShader(const boost::shared_ptr<Shader>& shader)
    {
        // Check if the shader wasn't already attached
        GLuint id = shader->shaderID();
        auto iter = m_shaders.find(id);

        if (iter != m_shaders.end()) {
            LOGE << "Shader " << id << " was already attached";
            return false;
        }

        glAttachShader(m_iProgram, id);
        VERIFYGL_RET();

        m_shaders.insert(std::make_pair(id, shader));

        return true;
    }

    const GLint Program::attribLocation(const std::string& name) const
    {
        GLint ret = glGetAttribLocation(m_iProgram, name.c_str());
        VERIFYGL();

        return ret;    }

	void Program::bindAttribLocation(GLuint index, const std::string& name)
	{
		glBindAttribLocation(m_iProgram, index, name.c_str());
	}

    const GLint Program::uniformLocation(const std::string& name) const
    {
        GLint ret = glGetUniformLocation(m_iProgram, name.c_str());
        VERIFYGL();

        return ret;
    }

    const bool Program::link()
    {
        GLint linked;

        glLinkProgram(m_iProgram);
        glGetProgramiv(m_iProgram, GL_LINK_STATUS, &linked);

        if (!linked) {
            GLint infoLen = 0;
            glGetProgramiv(m_iProgram, GL_INFO_LOG_LENGTH, &infoLen);

            if (infoLen > 1) {
                char* infoLog = new char[infoLen];

                glGetProgramInfoLog(m_iProgram, infoLen, NULL, infoLog);
                LOGE << "Error linking program " << infoLog;

                delete(infoLog);
            }

            return false;
        }

        return true;
    }

    const bool Program::use()
    {
        glUseProgram(m_iProgram);
        VERIFYGL_RET();

        return true;
    }
} // namespace ramen