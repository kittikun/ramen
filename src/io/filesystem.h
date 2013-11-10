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

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <boost/filesystem.hpp>
#include <boost/shared_array.hpp>

namespace ramen
{
    class Filesystem
    {
    public:
        enum class ResourceType
        {
            Default,
            Fbx,
            Font,
            Shader
        };

        const bool initialize();

        const std::string resourcePathAbs(ResourceType type, const std::string& filename) const;
        const std::string resourcePathRel(ResourceType type, const std::string& filename) const;

        // User must manually free data after usage
        boost::shared_array<char> resource(ResourceType type, const std::string& filename) const;

    private:
        void findWorkingDir();
        void findresourcePathAbs();
        const boost::filesystem::path fsresourcePathAbs(ResourceType type, const std::string& filename) const;

    private:
        boost::filesystem::path m_workingDir;
        boost::filesystem::path m_resourcePath;
    };
} // namespace ramen

#endif // FILESYSTEM_H
