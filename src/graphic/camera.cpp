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

#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace ramen
{
	Camera::Camera()
		: m_position(0.f, 10.f, -10.f)
	{
	}

	void Camera::lookAt(glm::vec3 at)
	{
		glm::vec3 up(0.f, 1.f, 0.f);

		m_view = glm::lookAt(m_position, at, up);
	}

} // namespace ramen