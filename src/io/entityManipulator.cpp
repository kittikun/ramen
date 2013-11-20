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

#include "../coreComponents.h"
#include "../database.h"
#include "../entity/entity.h"
#include "../entity/positionable.h"

namespace ramen
{
	EntityManipulator::EntityManipulator()
		: m_iCurrent(0)
	{
	}

	void EntityManipulator::initialize(const CoreComponents& components)
	{
		m_pDatabase = components.database;
	}

	void EntityManipulator::processInput(const SDL_Event& event)
	{
		switch (event.type)
		{
		case SDL_KEYDOWN:
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_UP:
					{
						boost::shared_ptr<Positionable> positionable = m_pDatabase->entities()[m_iCurrent]->getComponent<Positionable>(Positionable::CompoID_Positionable);

						const glm::vec3& pos = positionable->translation();
						positionable->setTranslation(pos + glm::vec3(0.f, 0.01f, 0.f));
					}
				case SDLK_DOWN:
					{
						boost::shared_ptr<Positionable> positionable = m_pDatabase->entities()[m_iCurrent]->getComponent<Positionable>(Positionable::CompoID_Positionable);

						const glm::vec3& pos = positionable->translation();
						positionable->setTranslation(pos + glm::vec3(0.f, -0.01f, 0.f));
					}
				}
			}
			break;
		}
	}
} // namespace ramen