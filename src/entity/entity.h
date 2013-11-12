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

#ifndef ENTITY_H
#define ENTITY_H

#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

namespace ramen
{
    class Component : boost::noncopyable
    {
    public:
        virtual ~Component() {};

        virtual void draw() {};
    };

    class Entity : boost::noncopyable
    {
    public:
        void addComponent(const boost::shared_ptr<Component>& component);
        void draw();

    private:
        std::vector<boost::shared_ptr<Component>> m_components;
    };
} // namespace ramen

#endif // ENTITY_H
