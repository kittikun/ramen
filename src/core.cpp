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
#include <glm/glm.hpp>

#include "coreComponents.h"
#include "database.h"
#include "log.h"
#include "settings.h"
#include "builder\builder.h"
#include "builder/meshbuilder.h"
#include "graphic/graphic.h"
#include "io/filesystem.h"
#include "io/fbx.h"
#include "perfmon/profiler.h"
#include "perfmon/resmon.h"

#include "graphic/mesh.h"

#include <boost/scoped_ptr.hpp>

namespace ramen
{
    Core::Core()
        : m_bState(false)
        , m_pDatabase(new Database())
        , m_pFbxManager(new FBXManager())
        , m_pBuilder(new Builder())
        , m_pFilesystem(new Filesystem())
        , m_pGraphic(new Graphic())
        , m_pResmon(new Resmon())
        , m_pSettings(new Settings())
    {
        Log::initialize();
        LOGC << "Creating core...";
    }

    Core::~Core()
    {
        LOGC << "Destroying core...";
        Profiler::dump();
    }

    void Core::fillCoreComponents(CoreComponents* out)
    {
        out->builder = m_pBuilder;
        out->core = this;
        out->database = m_pDatabase;
        out->fbxManager = m_pFbxManager;
        out->filesystem = m_pFilesystem;
        out->graphic = m_pGraphic;
        out->settings = m_pSettings;
    }

    const bool Core::initialize()
    {
        PROFILE;
        CoreComponents components;

        fillCoreComponents(&components);

        LOGC << "Initializing SDL..";
        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            LOGE << SDL_GetError();
            return false;
        }

        if (!m_pFilesystem->initialize()) {
            return false;
        }

        if (!m_pSettings->initialize(&components)) {
            return false;
        }

        if (!m_pGraphic->initialize(&components)) {
            return false;
        }

        m_pResmon->initialize(&components);

        m_pFbxManager->initialialize(&components);
        boost::shared_ptr<FBXScene> scene = m_pFbxManager->loadScene("humanoid.fbx");
        m_pBuilder->addJob(boost::dynamic_pointer_cast<Job>(scene->createJobMesh()));

        // connect signals
        m_sigState.connect(SigState::slot_type(&Graphic::slotState, m_pGraphic.get(), _1).track(m_pGraphic));
        m_sigState.connect(SigState::slot_type(&Builder::slotState, m_pBuilder.get(), _1).track(m_pBuilder));
        m_sigState.connect(SigState::slot_type(&Resmon::slotState, m_pResmon.get(), _1).track(m_pResmon));

        return true;
    }

    void Core::run()
    {
        SDL_Event e;

        m_threads.create_thread(boost::bind(&Graphic::run, m_pGraphic.get()));
        m_threads.create_thread(boost::bind(&Builder::run, m_pBuilder.get()));
        m_threads.create_thread(boost::bind(&Resmon::run, m_pResmon.get()));

        LOGC << "Starting main loop..";
        m_bState.store(true);

        while (m_bState.load()) {
            while (SDL_PollEvent(&e)) {
                switch (e.type) {
                case SDL_WINDOWEVENT:
                    {
                        if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
                            LOGC << "ID " << e.window.windowID << " width " << e.window.data1 << " height " << e.window.data2;
                        }
                    }
                    break;

                case SDL_KEYDOWN:
                    {
                        stop();
                    }
                    break;

                case SDL_QUIT:
                    {
                        stop();
                    }
                    break;
                }
            }
        }
        LOGC << "Exiting main loop..";
        m_threads.join_all();
    }

    void Core::slotError()
    {
        LOGC << "Error signal received";
        stop();
    }

    void Core::stop()
    {
        LOGC << "Requesting core shutdown..";
        m_bState.store(false);
        m_sigState(false);
    }
} // namespace ramen