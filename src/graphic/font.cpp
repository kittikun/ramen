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

#include <algorithm>
#include <vector>
#include <glm/glm.hpp>
#include <boost/shared_ptr.hpp>

#include "../log.h"
#include "../profiler.h"
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
	"  gl_FragColor = vec4(texpos, 0, 1);\n"
	//"  gl_FragColor = vec4(texture2D(tex, texpos).a, texture2D(tex, texpos).a, texture2D(tex, texpos).a, 1) * color;\n"
	//"  gl_FragColor = vec4(1, 1, 1, texture2D(tex, texpos).a) * color;\n"
	"}\n"
};

namespace ramen
{

//-------------------------------------------------------------------------------------
// FONTATLAS
//-------------------------------------------------------------------------------------

	FontAtlas::FontAtlas(FT_Face face, const int fontSize)
	{
		static const int MaxWidth = 1024;
		int rowW = 0;
		int rowH = 0;
		int w = 0;
		int h = 0;
		FT_GlyphSlot glyph = face->glyph;

		m_characters.resize(128);
		// Make a simple texture storing all glyph on width
		FT_Set_Pixel_Sizes(face, 0, fontSize);

		// Find minimum size for a texture holding all visible ASCII characters
		for (int i = 32; i < 128; i++) {
			if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
				LOGE << "Loading character " << i << "failed";
				continue;
			}

			if (rowW + glyph->bitmap.width + 1 >= MaxWidth) {
				w = std::max(w, rowW);
				h += rowH;
				rowW = 0;
				rowH = 0;
			}
			rowW += glyph->bitmap.width + 1;
			rowH = std::max(rowH, glyph->bitmap.rows);
		}

		w = findNearestPowerofTwo(std::max(w, rowW));
		h = findNearestPowerofTwo(h + rowH);
		m_iTexWidth = w;
		m_iTexHeight = h;

		// Create a texture that will be used to hold all ASCII glyphs
		glGenTextures(1, &m_iTexture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_iTexture);
		VERIFYGL();

		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, w, h, 0, GL_ALPHA, GL_UNSIGNED_BYTE, 0);
		VERIFYGL();

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		VERIFYGL();

		/* Paste all glyph bitmaps into the texture, remembering the offset */
		int ox = 0;
		int oy = 0;

		rowH = 0;

		for (int i = 32; i < 128; i++) {
			FontAtlasCharacter& ch = m_characters[i];

			if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
				LOGE << "Loading character " << i << " failed!";
				continue;
			}

			if (ox + glyph->bitmap.width + 1 >= MaxWidth) {
				oy += rowH;
				rowH = 0;
				ox = 0;
			}

			glTexSubImage2D(GL_TEXTURE_2D, 0, ox, oy, glyph->bitmap.width, glyph->bitmap.rows, GL_ALPHA, GL_UNSIGNED_BYTE, glyph->bitmap.buffer);
			VERIFYGL();

			ch.ax = float(glyph->advance.x >> 6);
			ch.ay = float(glyph->advance.y >> 6);

			ch.bw = float(glyph->bitmap.width);
			ch.bh = float(glyph->bitmap.rows);

			ch.bl = float(glyph->bitmap_left);
			ch.bt = float(glyph->bitmap_top);

			ch.tx = ox / (float)w;
			ch.ty = oy / (float)h;

			rowH = std::max(rowH, glyph->bitmap.rows);
			ox += glyph->bitmap.width + 1;
		}

		LOGGFX << boost::format("Generated FontAtlas with %1%x%2% (%3% kb) texture") % w % h % (w * h / 1024);
	}

	FontAtlas::~FontAtlas()
	{
		glDeleteTextures(1, &m_iTexture);
	}

//-------------------------------------------------------------------------------------
// TEXTRENDER
//-------------------------------------------------------------------------------------

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

	const bool TextRenderer::initialize()
	{
		PROFILE
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

		m_pAtlas.reset(new FontAtlas(m_pFTFace, 48));
		VERIFYGL();

		return true;
	}

	void TextRenderer::render_text(const std::string& text, float x, float y, float sx, float sy) const
	{
		PROFILE
		const uint8_t *p;

		/* Use the texture containing the atlas */
		glBindTexture(GL_TEXTURE_2D, m_pAtlas->getTexture());
		glUniform1i(m_pProgram->getUniformLocation("tex"), 0);
		VERIFYGL();

		/* Set up the VBO for our vertex data */
		glEnableVertexAttribArray(m_pProgram->getAttribLocation("coord"));
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glVertexAttribPointer(m_pProgram->getAttribLocation("coord"), 4, GL_FLOAT, GL_FALSE, 0, 0);
		VERIFYGL();

		std::vector<glm::vec4> coords(6 * text.size());
		int c = 0;

		/* Loop through all characters */
		for (p = (const uint8_t *)text.c_str(); *p; p++) {
			const FontAtlasCharacter& ch = m_pAtlas->getChar(*p);
			/* Calculate the vertex and texture coordinates */
			float x2 = x + ch.bl * sx;
			float y2 = -y - ch.bt * sy;
			float w = ch.bw * sx;
			float h = ch.bh * sy;

			/* Advance the cursor to the start of the next character */
			x += ch.ax * sx;
			y += ch.ay * sy;

			/* Skip glyphs that have no pixels */
			if (!w || !h)
				continue;

			coords[c++] = glm::vec4(x2, -y2, ch.tx, ch.ty);
			coords[c++] = glm::vec4(x2 + w, -y2, (ch.tx + ch.bw) / m_pAtlas->getTexWidth(), ch.ty);
			coords[c++] = glm::vec4(x2, -y2 - h, ch.tx, (ch.ty + ch.bh) / m_pAtlas->getTexHeight());
			coords[c++] = glm::vec4(x2 + w, -y2, (ch.tx + ch.bw) / m_pAtlas->getTexWidth(), ch.ty);
			coords[c++] = glm::vec4(x2, -y2 - h, ch.tx, (ch.ty + ch.bh) / m_pAtlas->getTexHeight());
			coords[c++] = glm::vec4(x2 + w, -y2 - h, (ch.tx + ch.bw) / m_pAtlas->getTexWidth(), ch.ty + ch.bh / m_pAtlas->getTexHeight());
		}

		/* Draw all the character on the screen in one go */
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * coords.size(), &coords.front(), GL_DYNAMIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, c);
		VERIFYGL();

		glDisableVertexAttribArray(m_pProgram->getAttribLocation("coord"));
	}

	void TextRenderer::display() const
	{
		PROFILE
		glm::ivec2 windowSize = m_pGraphic->getWindowSize();
		float sx = 2.0f / float(windowSize.x);
		float sy = 2.0f / float(windowSize.y);

		m_pProgram->use();

		/* Enable blending, necessary for our alpha texture */
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		GLfloat black[4] = { 0, 0, 0, 1 };
		GLfloat red[4] = { 1, 0, 0, 1 };
		GLfloat transparent_green[4] = { 0, 1, 0, 0.5 };

		/* Set color to black */
		glUniform4fv(m_pProgram->getUniformLocation("color"), 1, black);

		/* Effects of alignment */
		render_text("The Quick Brown Fox Jumps Over The Lazy Dog", -1 + 8 * sx, 1 - 50 * sy, sx, sy);
		render_text("The Misaligned Fox Jumps Over The Lazy Dog", -1 + 8.5 * sx, 1 - 100.5 * sy, sx, sy);

		/* Scaling the texture versus changing the font size */
		render_text("The Small Texture Scaled Fox Jumps Over The Lazy Dog", -1 + 8 * sx, 1 - 175 * sy, sx * 0.5, sy * 0.5);
		render_text("The Small Font Sized Fox Jumps Over The Lazy Dog", -1 + 8 * sx, 1 - 200 * sy, sx, sy);
		render_text("The Tiny Texture Scaled Fox Jumps Over The Lazy Dog", -1 + 8 * sx, 1 - 235 * sy, sx * 0.25, sy * 0.25);
		render_text("The Tiny Font Sized Fox Jumps Over The Lazy Dog", -1 + 8 * sx, 1 - 250 * sy, sx, sy);

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
