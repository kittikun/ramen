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
#include "graphic.h"
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

struct point {
	GLfloat x;
	GLfloat y;
	GLfloat s;
	GLfloat t;
};

namespace ramen
{
	TextRenderer::TextRenderer(Graphic* pGraphic)
		: m_pGraphic(pGraphic)
		, m_pFTLibrary(nullptr)
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

		if (FT_New_Memory_Face(m_pFTLibrary, DIN_Light_ttf, DIN_Light_ttf_len, 0, &m_pFTFace)) {
			LOGE << "Could not open font DIN";
			return false;
		}

		boost::shared_ptr<Shader> vtx(new Shader(GL_VERTEX_SHADER));
		boost::shared_ptr<Shader> frg(new Shader(GL_FRAGMENT_SHADER));

		if (!vtx->compile(g_strVertexFont) || !frg->compile(g_strFragFont)) {
			return false;
		}

		m_pProgram.reset(new Program());
		m_pProgram->attachShader(vtx);
		m_pProgram->attachShader(frg);

		if (!m_pProgram->link()) {
			return false;
		}

		glGenBuffers(1, &m_vbo);

		return true;
	}

	void TextRenderer::render_text(const char *text, float x, float y, float sx, float sy)
	{
		const char *p;
		FT_GlyphSlot glyph = m_pFTFace->glyph;

		/* Create a texture that will be used to hold one "glyph" */
		GLuint tex;

		glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glUniform1i(m_pProgram->getAttribLocation("tex"), 0);

		/* We require 1 byte alignment when uploading texture data */
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glEnableVertexAttribArray(m_pProgram->getAttribLocation("coord"));
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glVertexAttribPointer(m_pProgram->getAttribLocation("coord"), 4, GL_FLOAT, GL_FALSE, 0, 0);

		/* Loop through all characters */
		for (p = text; *p; p++) {
			/* Try to load and render the character */
			if (FT_Load_Char(m_pFTFace, *p, FT_LOAD_RENDER))
				continue;

			/* Upload the "bitmap", which contains an 8-bit grayscale image, as an alpha texture */
			glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, glyph->bitmap.width, glyph->bitmap.rows, 0, GL_ALPHA, GL_UNSIGNED_BYTE, glyph->bitmap.buffer);

			/* Calculate the vertex and texture coordinates */
			float x2 = x + glyph->bitmap_left * sx;
			float y2 = -y - glyph->bitmap_top * sy;
			float w = glyph->bitmap.width * sx;
			float h = glyph->bitmap.rows * sy;

			point box[4] = {
				{x2, -y2, 0, 0},
				{x2 + w, -y2, 1, 0},
				{x2, -y2 - h, 0, 1},
				{x2 + w, -y2 - h, 1, 1},
			};

			/* Draw the character on the screen */
			glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

			/* Advance the cursor to the start of the next character */
			x += (glyph->advance.x >> 6) * sx;
			y += (glyph->advance.y >> 6) * sy;
		}

		glDisableVertexAttribArray(m_pProgram->getAttribLocation("coord"));
		glDeleteTextures(1, &tex);
	}

	void TextRenderer::display()
	{
		glm::ivec2 windowSize = m_pGraphic->getWindowSize();
		float sx = 2.0 / float(windowSize.x);
		float sy = 2.0 / float(windowSize.y);

		m_pProgram->use();

		VERIFYGL();

		/* Enable blending, necessary for our alpha texture */
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		VERIFYGL();

		GLfloat black[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		GLfloat red[4] = { 1, 0, 0, 1 };
		GLfloat transparent_green[4] = { 0, 1, 0, 0.5 };

		/* Set font size to 48 pixels, color to black */
		FT_Set_Pixel_Sizes(m_pFTFace, 0, 48);
		glUniform4fv(m_pProgram->getUniformLocation("color"), 1, black);
		VERIFYGL();

		/* Effects of alignment */
		render_text("The Quick Brown Fox Jumps Over The Lazy Dog", -1 + 8 * sx, 1 - 50 * sy, sx, sy);
		render_text("The Misaligned Fox Jumps Over The Lazy Dog", -1 + 8.5 * sx, 1 - 100.5 * sy, sx, sy);

		/* Scaling the texture versus changing the font size */
		render_text("The Small Texture Scaled Fox Jumps Over The Lazy Dog", -1 + 8 * sx, 1 - 175 * sy, sx * 0.5, sy * 0.5);
		FT_Set_Pixel_Sizes(m_pFTFace, 0, 24);
		render_text("The Small Font Sized Fox Jumps Over The Lazy Dog", -1 + 8 * sx, 1 - 200 * sy, sx, sy);
		FT_Set_Pixel_Sizes(m_pFTFace, 0, 48);
		render_text("The Tiny Texture Scaled Fox Jumps Over The Lazy Dog", -1 + 8 * sx, 1 - 235 * sy, sx * 0.25, sy * 0.25);
		FT_Set_Pixel_Sizes(m_pFTFace, 0, 12);
		render_text("The Tiny Font Sized Fox Jumps Over The Lazy Dog", -1 + 8 * sx, 1 - 250 * sy, sx, sy);
		FT_Set_Pixel_Sizes(m_pFTFace, 0, 48);

		/* Colors and transparency */
		render_text("The Solid Black Fox Jumps Over The Lazy Dog", -1 + 8 * sx, 1 - 430 * sy, sx, sy);

		glUniform4fv(m_pProgram->getUniformLocation("color"), 1, red);
		render_text("The Solid Red Fox Jumps Over The Lazy Dog", -1 + 8 * sx, 1 - 330 * sy, sx, sy);
		render_text("The Solid Red Fox Jumps Over The Lazy Dog", -1 + 28 * sx, 1 - 450 * sy, sx, sy);

		glUniform4fv(m_pProgram->getUniformLocation("color"), 1, transparent_green);
		render_text("The Transparent Green Fox Jumps Over The Lazy Dog", -1 + 8 * sx, 1 - 380 * sy, sx, sy);
		render_text("The Transparent Green Fox Jumps Over The Lazy Dog", -1 + 18 * sx, 1 - 440 * sy, sx, sy);
	}

} // namespace ramen
