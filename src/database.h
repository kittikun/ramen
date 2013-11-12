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

#ifndef DATABASE_H
#define DATABASE_H

#include <atomic>
#include <boost/any.hpp>
#include <boost/unordered_map.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/utility.hpp>

#include "log.h"
#include "perfmon/profiler.h"

namespace ramen
{
    class Entity;
    class Mesh;

    class Database : boost::noncopyable
    {
    public:
        template <typename T>
        const T& get(const std::string& key)
        {
            PROFILE;
            boost::lock_guard<boost::mutex> lock(m_mutex);
            auto found = m_data.find(key);

            if (found == m_data.end()) {
                LOGE << "Database get key \"" << key << "\" doesn't exist";
            }

            assert(found != m_data.end());
			return boost::any_cast<const T&>(found->second);
        }

		void remove(const std::string& key);

        template <typename T>
        void set(const std::string& key, const T& value)
        {
            PROFILE;
            boost::lock_guard<boost::mutex> lock(m_mutex);
            auto found = m_data.find(key);

            if (found == m_data.end()) {
                LOGD << "Adding key \"" << key << "\" with type " << typeid(T).name();

                m_data.insert(std::make_pair(key, boost::any(value)));
            } else {
                found->second = value;
            }
        }

    private:
        boost::mutex m_mutex;
        std::vector<boost::shared_ptr<Entity>> m_entities;
        boost::unordered_map<std::string, boost::any> m_data;
    };
} // namespace ramen

#endif // DATABASE_H
