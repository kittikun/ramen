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

#include "graphic.h"

#include <SDL.h>
#include <GLES2/gl2.h>

#include "log.h"

namespace ramen
{
    Graphic::Graphic()
       : m_pContext(nullptr)
       , m_pWindow(nullptr)
    {
    }

    Graphic::~Graphic()
    {
        LOGGFX << "Destroying graphics...";
        SDL_GL_DeleteContext(m_pContext);
        SDL_DestroyWindow(m_pWindow);
    }

    bool Graphic::initialize(const int width, const int height)
    {
        LOGGFX << "Initializing window...";
        m_pWindow = SDL_CreateWindow("ramen", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

        if (m_pWindow == nullptr) {
            LOGE << "Failed to initialize window...";
            return false;
        }

        return true;
    }

    void Graphic::run()
    {
        LOGGFX << "Creating GL context...";
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_EGL, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
        SDL_GL_CreateContext(m_pWindow);

        LOGC << "Starting graphic loop..";
        while (1) {
            glClearColor(0.0f, 0.0f, 0.3f, 1);
            glClear(GL_COLOR_BUFFER_BIT);
            SDL_GL_SwapWindow(m_pWindow);
        }
        LOGC << "Exiting graphic loop..";
    }

} // namespace ramen
