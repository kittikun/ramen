//  Copyright (count) 2013  kittikun
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
#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
#include <glm/ext.hpp>

#include "graphic.h"
#include "shader.h"
#include "graphicUtility.h"
#include "../coreComponents.h"
#include "../log.h"
#include "../settings.h"
#include "../utility.h"
#include "../io/filesystem.h"
#include "../perfmon/profiler.h"

namespace ramen
{
	//-------------------------------------------------------------------------------------
	// FONTATLAS
	//-------------------------------------------------------------------------------------
	FontAtlas::FontAtlas()
		: m_iTexID(0)
	{
		m_characters.resize(128);
	}

	FontAtlas::~FontAtlas()
	{
		if (m_iTexID != 0) {
			glDeleteTextures(1, &m_iTexID);
		}
	}

	void FontAtlas::calcRequiredTexSize(const FT_Face fontFamilly, const int maxTexWidth)
	{
		FT_GlyphSlot glyph = fontFamilly->glyph;
		glm::ivec2 pos;
		glm::ivec2 rowSize;

		// Find minimum size for a texture holding all visible ASCII characters
		for (int i = 32; i < 128; i++) {
			if (FT_Load_Char(fontFamilly, i, FT_LOAD_RENDER)) {
				LOGE << "FontAltas loading of character " << i << "failed";
				continue;
			}

			if (rowSize.x + glyph->bitmap.width + 1 >= maxTexWidth) {
				pos.x = std::max(pos.x, rowSize.x);
				pos.y += rowSize.y;
				rowSize.x = 0;
				rowSize.y = 0;
			}
			rowSize.x += glyph->bitmap.width + 1;
			rowSize.y = std::max(rowSize.y, glyph->bitmap.rows);
		}

		m_texSize.x = utility::calcNearestPowerofTwo(std::max(pos.x, rowSize.x));
		m_texSize.y = utility::calcNearestPowerofTwo(pos.y + rowSize.y);
	}

	void FontAtlas::createTexture(const FT_Face fontFamilly, const int maxTexWidth)
	{
		FT_GlyphSlot glyph = fontFamilly->glyph;
		glm::ivec2 offset;
		glm::ivec2 pos;
		glm::ivec2 rowSize;

		// Create texture
		glGenTextures(1, &m_iTexID);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_iTexID);
		VERIFYGL();

		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, m_texSize.x, m_texSize.y, 0, GL_ALPHA, GL_UNSIGNED_BYTE, 0);
		VERIFYGL();

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		VERIFYGL();

		LOGGFX << boost::format("Generated FontAtlas with %1%x%2% %3% (%4%)  texture")
			% m_texSize.x
			% m_texSize.y
			% graphicUtility::glEnumtoString(GL_ALPHA)
			% utility::readableSizeByte(m_texSize.x * m_texSize.y);

		// Fill texture with data from freetype
		for (int i = 32; i < 128; i++) {
			FontAtlas::Character& ch = m_characters[i];

			if (FT_Load_Char(fontFamilly, i, FT_LOAD_RENDER)) {
				LOGE << "FontAltas loading of character " << i << "failed";
				continue;
			}

			if (offset.x + glyph->bitmap.width + 1 >= maxTexWidth) {
				offset.x = 0;
				offset.y += rowSize.y;
				rowSize.y = 0;
			}

			glTexSubImage2D(GL_TEXTURE_2D, 0, offset.x, offset.y, glyph->bitmap.width, glyph->bitmap.rows, GL_ALPHA, GL_UNSIGNED_BYTE, glyph->bitmap.buffer);
			VERIFYGL();

			// Fill char info
			ch.advance.x = static_cast<float>(glyph->advance.x >> 6);
			ch.advance.y = static_cast<float>(glyph->advance.y >> 6);
			ch.bmpSize.x = static_cast<float>(glyph->bitmap.width);
			ch.bmpSize.y = static_cast<float>(glyph->bitmap.rows);
			ch.bmpTopLeft.x = static_cast<float>(glyph->bitmap_left);
			ch.bmpTopLeft.y = static_cast<float>(glyph->bitmap_top);
			ch.texOffset.s = offset.x / static_cast<float>(m_texSize.x);
			ch.texOffset.t = offset.y / static_cast<float>(m_texSize.y);

			rowSize.y = std::max(rowSize.y, glyph->bitmap.rows);
			offset.x += glyph->bitmap.width + 1;
		}
	}

	void FontAtlas::initialize(const FT_Face fontFamilly, const unsigned char size, const int maxTexWidth)
	{
		FT_Set_Pixel_Sizes(fontFamilly, 0, size);

		calcRequiredTexSize(fontFamilly, maxTexWidth);
		createTexture(fontFamilly, maxTexWidth);
	}

	//-------------------------------------------------------------------------------------
	// FONTMANAGER
	//-------------------------------------------------------------------------------------
	FontManager::FontManager()
		: m_pFilesystem(nullptr)
		, m_FTLibrary(nullptr)
		, m_vbo(0)
	{
	}

	FontManager::~FontManager()
	{
		if (m_FTLibrary) {
			FT_Done_FreeType(m_FTLibrary);
		}

		if (m_vbo != 0) {
			glDeleteBuffers(1, &m_vbo);
		}
	}

	void FontManager::addText(const std::string& text, const std::string& font, const glm::vec4& color, const glm::vec2& pos)
	{
		PROFILE;
		boost::lock_guard<boost::mutex> lock(m_mutex);

		m_cache[font][color].push_back(TextCache(text, pos));
	}

	const bool FontManager::createFont(const std::string& name, const std::string& fontFamilly, const int fontSize)
	{
		PROFILE;
		auto foundFamilly = m_fonts.find(fontFamilly);

		LOGGFX << boost::format("Generating '%1%' from '%2%' at %3%pts..") % name % fontFamilly % fontSize;

		if (foundFamilly == m_fonts.end()) {
			LOGE << "Font familly '" << fontFamilly << "' doesn't exist";
			return false;
		}

		// Make sure name isn't already used
		auto foundDup = m_fontAtlases.find(name);

		if (foundDup != m_fontAtlases.end()) {
			LOGW << "A font named '" << name << "' already exists, aborting";
			return false;
		}

		boost::shared_ptr<FontAtlas> atlas(new FontAtlas());

		atlas->initialize(foundFamilly->second, fontSize, m_pSettings->get<int>("fontTextureWidth"));
		m_fontAtlases.insert(std::make_pair(name, atlas));

		return true;
	}

	void FontManager::draw()
	{
		PROFILE;
		boost::lock_guard<boost::mutex> lock(m_mutex);
		size_t charCount;

		m_pProgram->use();
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(m_pProgram->uniformLocation("tex"), 0);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glEnableVertexAttribArray(m_pProgram->attribLocation("coord"));
		glVertexAttribPointer(m_pProgram->attribLocation("coord"), 4, GL_FLOAT, GL_FALSE, 0, 0);
		VERIFYGL();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		VERIFYGL();

		BOOST_FOREACH(auto& fontIter, m_cache) {
			const boost::shared_ptr<FontAtlas>& font = m_fontAtlases[fontIter.first];

			glBindTexture(GL_TEXTURE_2D, font->texID());

			BOOST_FOREACH(auto& colorIter, fontIter.second) {
				charCount = 0;
				m_texcoords.clear();

				// first pass to count number of characters
				BOOST_FOREACH(auto& textData, colorIter.second) {
					charCount += textData.text.size();
				}

				if (charCount > 0) {
					m_texcoords.resize(6 * charCount);
					glUniform4fv(m_pProgram->uniformLocation("color"), 1, glm::value_ptr(colorIter.first));
					VERIFYGL();

					// second pass to fill data
					int count = 0;

					BOOST_FOREACH(auto& textData, colorIter.second) {
						glm::vec2 posScaled = glm::vec2(-1 + textData.position.x * m_scaleFactor.x, 1 - textData.position.y * m_scaleFactor.y);

						for (size_t i = 0; i < textData.text.size(); ++i) {
							const FontAtlas::Character& ch = font->charAt(textData.text[i]);
							glm::vec2 pos2;
							glm::vec2 size;

							// Calculate the vertex and texture coordinates
							pos2.x = posScaled.x + ch.bmpTopLeft.x * m_scaleFactor.x;
							pos2.y = -posScaled.y - ch.bmpTopLeft.y * m_scaleFactor.y;
							float w = ch.bmpSize.x * m_scaleFactor.x;
							float h = ch.bmpSize.y * m_scaleFactor.y;

							// Advance the cursor to the start of the next character
							posScaled.x += ch.advance.x * m_scaleFactor.x;
							posScaled.y += ch.advance.y * m_scaleFactor.y;

							// Skip glyphs that have no pixels
							if (!w || !h)
								continue;

							m_texcoords[count++] = glm::vec4(pos2.x, -pos2.y, ch.texOffset.s, ch.texOffset.t);
							m_texcoords[count++] = glm::vec4(pos2.x + w, -pos2.y, ch.texOffset.s + ch.bmpSize.x / font->texSize().x, ch.texOffset.t);
							m_texcoords[count++] = glm::vec4(pos2.x, -pos2.y - h, ch.texOffset.s, ch.texOffset.t + ch.bmpSize.y / font->texSize().y);
							m_texcoords[count++] = glm::vec4(pos2.x + w, -pos2.y, ch.texOffset.s + ch.bmpSize.x / font->texSize().x, ch.texOffset.t);
							m_texcoords[count++] = glm::vec4(pos2.x, -pos2.y - h, ch.texOffset.s, ch.texOffset.t + ch.bmpSize.y / font->texSize().y);
							m_texcoords[count++] = glm::vec4(pos2.x + w, -pos2.y - h, ch.texOffset.s + ch.bmpSize.x / font->texSize().x, ch.texOffset.t + ch.bmpSize.y / font->texSize().y);
						}
					}
					// Draw all the character on the screen in one go
					glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * m_texcoords.size(), &m_texcoords[0], GL_DYNAMIC_DRAW);
					glDrawArrays(GL_TRIANGLES, 0, count);
					VERIFYGL();
					colorIter.second.clear();
				}
			}
		}

		glDisableVertexAttribArray(m_pProgram->attribLocation("coord"));
		glDisable(GL_BLEND);
	}

	const bool FontManager::initialize(const CoreComponents& components)
	{
		PROFILE;
		glm::ivec2 windowSize(components.settings->get<int>("windowWidth"), components.settings->get<int>("windowHeight"));

		LOGGFX << "Initializing font manager..";

		m_pFilesystem = components.filesystem;
		m_pSettings = components.settings;

		if (FT_Init_FreeType(&m_FTLibrary)) {
			LOGE << "Could not initialize Freetype library";
			return false;
		}

		m_scaleFactor = glm::vec2(2.0 / static_cast<float>(windowSize.x), 2.0 / static_cast<float>(windowSize.y));

		return true;
	}

	const bool FontManager::initializeGL()
	{
		boost::shared_ptr<Shader> vtx(new Shader(GL_VERTEX_SHADER, "font.v"));
		boost::shared_ptr<Shader> frg(new Shader(GL_FRAGMENT_SHADER, "font.f"));

		LOGGFX << "Initializing GL for font renderer..";

		// Shaders/Program to be used for all font drawing
		if (!vtx->compile() || !frg->compile()) {
			return false;
		}

		m_pProgram.reset(new Program(vtx, frg));

		if (!m_pProgram->link()) {
			return false;
		}

		// Buffer that will be used for all text drawing
		glGenBuffers(1, &m_vbo);
		VERIFYGL_RET();

		return true;
	}

	const bool FontManager::loadFontFamillyFromFile(const std::string& name, const std::string& filename)
	{
		PROFILE;
		FT_Face face;
		std::string pathAbs;
		std::string pathRel;

		pathRel = m_pFilesystem->resourcePathRel(Filesystem::ResourceType::Font, filename);
		pathAbs = m_pFilesystem->resourcePathAbs(Filesystem::ResourceType::Font, filename);

		LOGGFX << boost::format("Loading font familly '%1%' from '%2%'..") % name % pathRel;

		if (pathAbs.empty()) {
			return false;
		}

		if (FT_New_Face(m_FTLibrary, pathAbs.c_str(), 0, &face)) {
			LOGE << boost::format("Could not open font familly '%1%' from '%2%'") % name % pathRel;
			return false;
		}

		m_fonts.insert(std::make_pair(name, face));

		return true;
	}

	const bool FontManager::loadFontFamillyFromMemory(const std::string& name, const unsigned char* data, const uint32_t size)
	{
		FT_Face face;

		LOGGFX << "Loading font familly '" << name << "'..";

		if (FT_New_Memory_Face(m_FTLibrary, data, size, 0, &face)) {
			LOGE << "Could not open font familly '" << name << "' from memory";
			return false;
		}

		m_fonts.insert(std::make_pair(name, face));

		return true;
	}
} // namespace ramen