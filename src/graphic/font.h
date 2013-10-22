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

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <GLES2/gl2.h>

namespace ramen
{
	class Program;
	class Graphic;

    class TextRenderer : boost::noncopyable
    {
    public:
        TextRenderer(Graphic* pGraphic);
        ~TextRenderer();

        bool initialize();
		void display();

		void render_text(const char *text, float x, float y, float sx, float sy);

    private:
		Graphic* m_pGraphic;
        FT_Library m_pFTLibrary;
		FT_Face m_pFTFace;
		boost::shared_ptr<Program> m_pProgram;
		GLuint m_vbo;
    };

} // namespace ramen

#endif // FONT_H
