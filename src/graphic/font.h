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

#ifndef FONT_H
#define FONT_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include <vector>
#include <boost/unordered_map.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <GLES2/gl2.h>
#include <glm/glm.hpp>


namespace ramen
{
    struct CoreComponents;
	class Program;
	class Filesystem;
    class Settings;

	class FontAtlas : boost::noncopyable
	{
	public:
		struct Character
		{
			glm::vec2 advance;
			glm::vec2 bmpSize;
			glm::vec2 bmpTopLeft;
			glm::vec2 texOffset;
		};

		FontAtlas();
		~FontAtlas();

		void initialize(const FT_Face fontFamilly, const unsigned char fontSize, const int maxTexWidth);

		const GLuint texID() const { return m_iTexID; }
		const Character& charAt(const unsigned int index) const { return m_characters[index]; }
		const glm::ivec2 texSize() const { return m_texSize; }

	private:
		void calcRequiredTexSize(const FT_Face fontFamilly, const int maxTexWidth);
		void createTexture(const FT_Face fontFamilly, const int maxTexWidth);

	private:
		GLuint m_iTexID;
		glm::ivec2 m_texSize;
		std::vector<Character> m_characters;
	};

	class FontManager : boost::noncopyable
	{
	public:
		FontManager();
		~FontManager();

		const bool initialize(const CoreComponents* components);
        const bool initializeGL();

		const bool loadFontFamillyFromFile(const std::string& name, const std::string& filename);
		const bool loadFontFamillyFromMemory(const std::string& name, const unsigned char* data, const unsigned int size);
		const bool createFont(const std::string& name, const std::string& fontFamilly, const int fontSize);

		void drawText(const std::string& text, const glm::vec2& pos) const;

		const bool setActiveFont(const std::string& name);
		const void setFontColor(const glm::vec4& color) { m_color = color; }
		const void setFontColor(const float r, const float g, const float b, const float a) { m_color = glm::vec4(r, g, b, a); }

	private:
		boost::shared_ptr<Filesystem> m_pFilesystem;
		boost::shared_ptr<Program> m_pProgram;
        boost::shared_ptr<Settings> m_pSettings;
        FontAtlas const* m_pActiveFont;
		FT_Library m_FTLibrary;
        glm::vec4 m_color;
		glm::vec2 m_scaleFactor;
        GLuint m_vbo;
		boost::unordered_map<std::string, FT_Face> m_fonts;
		boost::unordered_map<std::string, boost::shared_ptr<FontAtlas>> m_fontAtlases;
	};

} // namespace ramen

#endif // FONT_H

