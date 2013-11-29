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

#ifndef ENTITY_MANIPULATOR_H
#define ENTITY_MANIPULATOR_H

#include <SDL.h>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

namespace ramen
{
	class Entity;
	class Database;
	class FontManager;
	struct CoreComponents;

	class EntityManipulator : boost::noncopyable
	{
	public:
		EntityManipulator();
		~EntityManipulator();

		void draw();
		void initialize(const CoreComponents& components);
		void processInput(const SDL_Event& event);

	private:
		boost::shared_ptr<Database> m_pDatabase;
		boost::shared_ptr<FontManager> m_pFontManager;
		size_t m_iCurrent;
	};
} // namespace ramen

#endif // ENTITY_MANIPULATOR_H
