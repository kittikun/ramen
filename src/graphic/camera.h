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

namespace ramen
{
	class Camera
	{
	public:
		Camera();

		void lookAt(glm::vec3 at);
	private:
		glm::vec3 m_position;
		glm::mat4x4 m_view;
	};
} // namespace ramen

#endif // CAMERA_H
