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

		// component
		void update();

		const glm::mat4x4& proj() const { return m_proj; }

		void setFov(const float fov);
		void setNearFar(const float near, const float far);

	private:
		bool m_dirty;
		float m_fFar;
		float m_fFov;
		float m_fNear;
		float m_fRatio;
		glm::mat4x4 m_proj;
	};
} // namespace ramen

#endif // CAMERA_H
