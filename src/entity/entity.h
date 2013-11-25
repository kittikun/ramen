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

#include <boost/shared_ptr.hpp>
#include <boost/type_traits.hpp>
#include <boost/unordered_map.hpp>
#include <boost/utility.hpp>

namespace ramen
{
	class Entity;

	class Component : boost::noncopyable
	{
	public:
		virtual ~Component() {};

		virtual void draw() {};
		virtual void update() {};

	protected:
		Entity* m_entity;

		friend class Entity;
	};

	class Entity : boost::noncopyable
	{
	public:
		Entity(const std::string& name);

		void draw();
		void update();

		template <typename T>
		void addComponent(const boost::shared_ptr<T>& component)
		{
			BOOST_STATIC_ASSERT(boost::is_base_of<Component, T>::value);

			m_components[T::id] = boost::dynamic_pointer_cast<Component>(component);
			component->m_entity = this;
		}

		template <typename T>
		boost::shared_ptr<T> getComponent()
		{
			BOOST_STATIC_ASSERT(boost::is_base_of<Component, T>::value);
			return boost::dynamic_pointer_cast<T>(m_components[T::id]);
		}

		const std::string& name() { return m_strName; }

	private:
		boost::unordered_map<uint32_t, boost::shared_ptr<Component>> m_components;
		std::string m_strName;
	};
} // namespace ramen

#endif // ENTITY_H
