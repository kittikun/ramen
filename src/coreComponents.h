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

#ifndef CORE_COMPONENTS_H
#define CORE_COMPONENTS_H

namespace ramen
{
    class Builder;
    class Core;
    class Database;
    class FBXManager;
    class Filesystem;
    class Graphic;
    class Settings;

    // Limit functions parameters when initalizing components
    struct CoreComponents
    {
        CoreComponents() : core(nullptr) {}
        boost::shared_ptr<FBXManager> fbxManager;
        boost::shared_ptr<Database> database;
        boost::shared_ptr<Builder> builder;
        boost::shared_ptr<Filesystem> filesystem;
        boost::shared_ptr<Graphic> graphic;
        boost::shared_ptr<Settings> settings;
        Core* core;
    };
} // namespace ramen

#endif // CORE_COMPONENTS_H
