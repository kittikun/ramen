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
#include "builder/builder.h"
#include "graphic/graphic.h"
#include "graphic/font.h"
#include "io/entityManipulator.h"
#include "io/filesystem.h"
#include "io/fbx.h"
#include "perfmon/profiler.h"
#include "perfmon/resmon.h"

#include "entity/camera.h"
#include "entity/entity.h"
#include "entity/meshrender.h"
#include "entity/positionable.h"
#include "graphic/material.h"

namespace ramen
{
	Core::Core()
		: m_bState(false)
		, m_pBuilder(new Builder())
		, m_pDatabase(new Database())
        , m_pManipulator(new EntityManipulator())
		, m_pFbxManager(new FBXManager())
		, m_pFilesystem(new Filesystem())
		, m_pFontManager(new FontManager())
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
		profiler::dump();
	}

	void Core::fillCoreComponents(CoreComponents& out)
	{
		out.builder = m_pBuilder;
		out.core = this;
		out.database = m_pDatabase;
		out.fbxManager = m_pFbxManager;
		out.filesystem = m_pFilesystem;
		out.fontManager = m_pFontManager;
		out.graphic = m_pGraphic;
		out.settings = m_pSettings;
	}

	const bool Core::initialize()
	{
		PROFILE;
		CoreComponents components;

		fillCoreComponents(components);

		LOGC << "Initializing SDL..";
		if (SDL_Init(SDL_INIT_VIDEO) != 0) {
			LOGE << SDL_GetError();
			return false;
		}

		if (!m_pFilesystem->initialize()) {
			return false;
		}

		if (!m_pSettings->initialize(components)) {
			return false;
		}

		if (!m_pFontManager->initialize(components)) {
			return false;
		}

		if (!m_pGraphic->initialize(components)) {
			return false;
		}

		m_pManipulator->initialize(components);
		m_pResmon->initialize(components);
		m_pFbxManager->initialialize(components);

		m_pDatabase->set<boost::shared_ptr<EntityManipulator>>("activeManipulator", m_pManipulator);

		// fbx scene
		boost::shared_ptr<FBXScene> scene = m_pFbxManager->loadScene("teapot.fbx");
		m_pBuilder->addJob(scene->createJobMesh());

		// model
		{
			boost::shared_ptr<Entity> entity(new Entity("model"));
			boost::shared_ptr<MeshRender> meshRender(new MeshRender(m_pDatabase->get<boost::shared_ptr<Mesh>>("mesh")));
			boost::shared_ptr<Positionable> positionable(new Positionable());

			entity->addComponent(meshRender);
			entity->addComponent(positionable);
			m_pDatabase->addEntity(entity);
		}

		// camera
		{
			boost::shared_ptr<Entity> entity(new Entity("camera"));
			boost::shared_ptr<Camera> camera(new Camera());
			boost::shared_ptr<Positionable> positionable(new Positionable());

			positionable->setTranslation(glm::vec3(0.f, 0.f, -50.f));

			entity->addComponent(positionable);
			entity->addComponent(camera);
			m_pDatabase->addEntity(entity);
			m_pDatabase->set<boost::shared_ptr<Entity>>("activeCamera", entity);
		}

		// material
		boost::shared_ptr<Material> material(new Material());
		m_pDatabase->set<boost::shared_ptr<Material>>("material", material);

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
				m_pManipulator->processInput(e);

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
						if (e.key.keysym.sym == SDLK_ESCAPE) {
							stop();
						}
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