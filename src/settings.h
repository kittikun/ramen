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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <boost/property_tree/ptree.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

namespace ramen
{
    struct CoreComponents;
    class Filesystem;

    class Settings : boost::noncopyable
    {
    public:
        const bool initialize(const CoreComponents* components);

        template <typename T>
        const T get(const std::string& name) const { return m_properties.get<T>(name); }

    private:
        boost::property_tree::ptree m_properties;
        boost::shared_ptr<Filesystem> m_pFileSystem;
    };
} // namespace ramen

#endif // SETTINGS_H
