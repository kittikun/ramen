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

#include "entityManipulator.h"

#include <glm/glm.hpp>

#include "../coreComponents.h"
#include "../database.h"
#include "../entity/entity.h"
#include "../entity/positionable.h"
#include "../graphic/color.h"
#include "../graphic/font.h"

namespace ramen
{
	EntityManipulator::EntityManipulator()
		: m_iCurrent(0)
	{
	}

	EntityManipulator::~EntityManipulator()
	{
		LOGI << "Destroying EntityManipulator..";
	}

	void EntityManipulator::draw()
	{
		const glm::vec2 offset(10, 47);

		m_pFontManager->addText(m_pDatabase->entities()[m_iCurrent]->name(), "vera16", color::yellow, offset);

		boost::shared_ptr<Positionable> positionable = m_pDatabase->entities()[m_iCurrent]->getComponent<Positionable>();

		if (positionable) {
			boost::format fmt("trans (%1%, %2%, %3%)");
			std::string toto = boost::str(fmt % positionable->translation().x
				% positionable->translation().y
				% positionable->translation().z);

			m_pFontManager->addText(toto, "vera16", color::yellow, offset + glm::vec2(0.f, 21.f));


		}


		m_pFontManager->addText(m_pDatabase->entities()[m_iCurrent]->name(), "vera16", color::yellow, offset);
	}

	void EntityManipulator::initialize(const CoreComponents& components)
	{
		m_pDatabase = components.database;
		m_pFontManager = components.fontManager;
	}

	void EntityManipulator::processInput(const SDL_Event& event)
	{
		boost::shared_ptr<Positionable> positionable = m_pDatabase->entities()[m_iCurrent]->getComponent<Positionable>();
		const glm::vec3& pos = positionable->translation();

		switch (event.type)
		{
		case SDL_KEYDOWN:
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_UP:
					{
						positionable->setTranslation(pos + glm::vec3(0.f, 1.f, 0.f));
					}
					break;

				case SDLK_DOWN:
					{
						positionable->setTranslation(pos + glm::vec3(0.f, -1.f, 0.f));
					}
					break;

				case SDLK_LEFT:
					{
						positionable->setTranslation(pos + glm::vec3(1.f, 0.f, 0.f));
					}
					break;

				case SDLK_RIGHT:
					{
						positionable->setTranslation(pos + glm::vec3(-1.f, 0.f, 0.f));
					}
					break;

				case SDLK_TAB:
					{
						m_iCurrent = (m_iCurrent + 1) % m_pDatabase->entities().size();
					}
					break;
				}
			}
			break;
		}
	}
} // namespace ramen