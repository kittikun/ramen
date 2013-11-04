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

#include <boost/program_options.hpp>
#include <boost/scoped_ptr.hpp>
#include <glm/glm.hpp>

#include "core.h"

namespace po = boost::program_options;

int main(int ac, char** av)
{
    boost::scoped_ptr<ramen::Core> core(new ramen::Core());

    if (!core->initialize()) {
        std::cout << "Failed to initialize Core, exiting..." << std::endl;
        return 1;
    }

    core->run();

    return 0;
}
