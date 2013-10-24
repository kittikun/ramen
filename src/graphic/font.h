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
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <GLES2/gl2.h>

namespace ramen
{
	class Program;
	class Graphic;

	struct FontAtlasCharacter
	{
		float ax;	// advance.x
		float ay;	// advance.y
		float bw;	// bitmap.width;
		float bh;	// bitmap.height;
		float bl;	// bitmap_left;
		float bt;	// bitmap_top;
		float tx;	// x offset of glyph in texture coordinates
		float ty;	// y offset of glyph in texture coordinates
	};

	class FontAtlas : boost::noncopyable
	{
	public:
		FontAtlas(FT_Face face, const int fontSize);
		~FontAtlas();

		const GLuint getTexture() const { return m_iTexture; }
		const FontAtlasCharacter& getChar(const unsigned int index) const { return m_characters[index]; }
		const int getTexWidth() const { return m_iTexWidth; }
		const int getTexHeight() const { return m_iTexHeight; }

	private:
		GLuint m_iTexture;
		int m_iTexWidth;
		int m_iTexHeight;
		std::vector<FontAtlasCharacter> m_characters;
	};

    class TextRenderer : boost::noncopyable
    {
    public:
        TextRenderer(Graphic* pGraphic);
        ~TextRenderer();

        const bool initialize();
		void display() const;

		void render_text(const std::string& text, float x, float y, float sx, float sy) const;

    private:
		Graphic* m_pGraphic;
        FT_Library m_pFTLibrary;
		FT_Face m_pFTFace;
		boost::shared_ptr<FontAtlas> m_pAtlas;
		boost::shared_ptr<Program> m_pProgram;
		GLuint m_vbo;
    };

} // namespace ramen

#endif // FONT_H

