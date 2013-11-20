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

#ifndef MESHRENDER_H
#define MESHRENDER_H

#include <boost/shared_ptr.hpp>

#include "entity.h"

namespace ramen
{
	class Mesh;
	class Program;

	class MeshRender final : public Component
	{
	public:
		MeshRender(const boost::shared_ptr<Mesh>& mesh);

		void draw();
		const bool setupGL();

	public:
		static const uint32_t CompoID_MeshRender;

	private:
		boost::shared_ptr<Mesh> m_pMesh;
	};
} // namespace ramen

#endif // MESHRENDER_H
