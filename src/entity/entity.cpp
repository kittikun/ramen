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

#include "entity.h"

#include <boost/foreach.hpp>

namespace ramen
{
    void Entity::addComponent(const boost::shared_ptr<Component>& component)
    {
        m_components.push_back(component);
    }

    void Entity::draw()
    {
        BOOST_FOREACH(auto component, m_components) {
            component->draw();
        }
    }
} // namespace ramen