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
       : m_bState(false)
       , m_pGraphic(new Graphic())
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

        if (!m_pGraphic->initialize(width, height))
            return false;

        // connect signals
        m_sigState.connect(SigState::slot_type(&Graphic::slotState, m_pGraphic.get(), _1).track(m_pGraphic));

        return true;
    }

    void Core::run()
    {
        SDL_Event e;

        m_threads.create_thread(boost::bind(&Graphic::run, m_pGraphic.get()));

        LOGC << "Starting main loop..";
        m_bState = true;

        while (m_bState) {
            while (SDL_PollEvent(&e)) {
                switch (e.type) {
                case SDL_QUIT:
                    {
                        stop();
                    }
                    break;

                case SDL_KEYDOWN:
                    {
                        stop();
                    }
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    {
                        stop();
                    }
                    break;
                }
            }
        }
        LOGC << "Exiting main loop..";
    }

    void Core::stop()
    {
        LOGC << "Requesting core shutdown..";
        m_bState = false;
        m_sigState(false);
        m_threads.join_all();
    }

} // namespace ramen

