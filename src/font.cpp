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

#include "log.h"

namespace ramen
{
    TextRenderer::TextRenderer()
    : m_freetype(nullptr)
    {

    }

    TextRenderer::~TextRenderer()
    {
        LOGGFX << "Destroying text renderer..";

        if (m_freetype) {
            FT_Done_FreeType(m_freetype);
        }
    }

    bool TextRenderer::initialize()
    {
        LOGGFX << "Initializing text renderer..";
        if (FT_Init_FreeType(&m_freetype)) {
            LOGE << "Could not init freetype library";
            return false;
        }

        return true;
    }

} // namespace ramen
