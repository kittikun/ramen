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

#include "font.h"

#include <boost/shared_ptr.hpp>

#include "../log.h"
#include "font_DIN.h"
#include "shader.h"
#include "utility.h"

static const char* g_strVertexFont[] = {
	"attribute vec4 coord;\n"
	"varying vec2 texpos;\n"

	"void main() {\n"
	"  gl_Position = vec4(coord.xy, 0, 1);\n"
	"  texpos = coord.zw;\n"
	"}\n"
};

static const char* g_strFragFont[] = {
	"precision mediump float;\n"
	"varying vec2 texpos;\n"
	"uniform sampler2D tex;\n"
	"uniform vec4 color;\n"

	"void main(void) {\n"
	"  gl_FragColor = vec4(1, 1, 1, texture2D(tex, texpos).a) * color;\n"
	"}\n"
};

namespace ramen
{
    TextRenderer::TextRenderer()
    : m_pFTLibrary(nullptr)
	, m_vbo(-1)
    {

    }

    TextRenderer::~TextRenderer()
    {
        LOGGFX << "Destroying text renderer..";

        if (m_pFTLibrary) {
            FT_Done_FreeType(m_pFTLibrary);
        }
    }

    bool TextRenderer::initialize()
    {
        LOGGFX << "Initializing text renderer..";
        if (FT_Init_FreeType(&m_pFTLibrary)) {
            LOGE << "Could not init freetype library";
            return false;
        }

        FT_Face face;

        if (FT_New_Memory_Face(m_pFTLibrary, DIN_Light_ttf, DIN_Light_ttf_len, 0, &face)) {
            LOGE << "Could not open font DIN";
            return false;
        }

		boost::shared_ptr<Shader> vtx(new Shader(GL_VERTEX_SHADER));
		boost::shared_ptr<Shader> frg(new Shader(GL_FRAGMENT_SHADER));

		if (!vtx->compile(g_strVertexFont) || !frg->compile(g_strFragFont)) {
			return false;
		}

		m_program.reset(new Program());
		m_program->attachShader(vtx);
		m_program->attachShader(frg);

		if (!m_program->link()) {
			return false;
		}

		glGenBuffers(1, &m_vbo);

        return true;
    }

} // namespace ramen
