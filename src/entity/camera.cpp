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

#include "positionable.h"

namespace ramen
{
	const uint32_t Camera::id = 0x00000002;

	Camera::Camera()
		: m_dirty(true)
		, m_fFar(100.f)
		, m_fFov(60.f)
		, m_fNear(0.1f)
		, m_fRatio(640.f / 480.f)
	{
	}

	void Camera::setFov(const float fov)
	{ 
		m_fFov = fov; 
		m_dirty = true;
	}

	void Camera::setNearFar(const float near, const float far)
	{
		m_fNear = near;
		m_fFar = far;
		m_dirty = true;
	}

	void Camera::update()
	{
		if (m_dirty) {
			m_proj = glm::perspective(m_fFov, m_fRatio, m_fNear, m_fFar);
			m_dirty = false;
		}
	}

} // namespace ramen