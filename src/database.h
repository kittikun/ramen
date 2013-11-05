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
#include <boost/unordered_map.hpp>
#include <boost/thread.hpp>
#include <boost/utility.hpp>

namespace ramen
{
    class Mesh;

    class Database : boost::noncopyable
    {
    public:
        const uint32_t uint(const std::string& key) const;
        void uint(const std::string& key, const uint32_t value);

    private:
        boost::mutex m_mutex;
        boost::unordered_map<std::string, Mesh> m_meshes;
        boost::unordered_map<std::string, uint32_t> m_uint;
    };
} // namespace ramen

#endif // DATABASE_H
