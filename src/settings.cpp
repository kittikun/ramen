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

#include "settings.h"

#include <boost/property_tree/ini_parser.hpp>

#include "coreComponents.h"
#include "log.h"
#include "io/filesystem.h"

namespace ramen
{
    const bool Settings::initialize(const CoreComponents* components)
    {
        std::string configPath;

        m_pFileSystem = components->filesystem;

        configPath = m_pFileSystem->resourcePathAbs(Filesystem::ResourceType::Default, "settings.ini");
        if (configPath.empty()) {
            return false;
        }

        LOGI << "Loading settings..";

        boost::property_tree::read_ini(configPath, m_properties);

        return true;
    }
} // namespace ramen