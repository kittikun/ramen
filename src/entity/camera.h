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

#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

#include "entity.h"

namespace ramen
{
	class Camera final : public Component
	{
	public:
		static const uint32_t id;

		Camera();

		void lookAt(glm::vec3 at);

		const glm::mat4x4& view() const { return m_view; }

	private:
		glm::vec3 m_target;
		glm::mat4x4 m_view;
	};
} // namespace ramen

#endif // CAMERA_H
