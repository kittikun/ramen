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

#ifndef POSITIONABLE_H
#define POSITIONABLE_H

#include <boost/shared_ptr.hpp>
#include <glm/glm.hpp>

#include "entity.h"

namespace ramen
{
	class Positionable final : public Component
	{
	public:
		Positionable();

		void update();

		const glm::mat4x4& model() const { return m_model; }

		const glm::vec3& translation() const { return m_translation; }
		void setTranslation(const glm::vec3& translation);

	public:
		static const uint32_t CompoID_Positionable;

	private:
		bool m_dirty;
		glm::mat4x4 m_model;
		glm::vec3 m_translation;
		glm::vec3 m_rotation; // euler angles
		glm::vec3 m_scale;
	};
} // namespace ramen

#endif // POSITIONABLE_H