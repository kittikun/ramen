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

#include "core.h"

#include <SDL.h>

#include "graphic.h"
#include "log.h"

namespace ramen
{

    Core::Core()
       : m_graphic(new Graphic())
    {
        Log::initialize();
        LOGC << "Creating core...";
    }

    Core::~Core()
    {
        LOGC << "Destroying core...";
    }

    bool Core::initialize(const int width, const int height)
    {
        LOGC << "Initializing SDL..";
        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            LOGE << SDL_GetError();
            return false;
        }

        if (!m_graphic->initialize(width, height))
            return false;

        return true;
    }

    void Core::run()
    {
        SDL_Event e;
        bool quit = false; 

        m_threads.create_thread(boost::bind(&Graphic::run, m_graphic));
        LOGC << "Starting main loop..";
        while (!quit) {
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT)
                    quit = true;
                if (e.type == SDL_KEYDOWN)
                    quit = true;
                if (e.type == SDL_MOUSEBUTTONDOWN)
                    quit = true;
            }
        }
        LOGC << "Exiting main loop..";
    }

} // namespace ramen

