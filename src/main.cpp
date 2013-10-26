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
    po::options_description desc("Allowed options");
    po::positional_options_description p;
    po::variables_map vm;

    try {

        desc.add_options()
           ("help,h", "output help message")
           ("width,w", po::value<uint32_t>()->default_value(1024)->required(), "window width")
           ("height,g", po::value<uint32_t>()->default_value(768)->required(), "window height")
        ;

        p.add("width", 1);
        p.add("height", 2);

        po::store(po::command_line_parser(ac, av).
                  options(desc).positional(p).run(), vm);

        if ((ac == 0) || (vm.count("help"))) {
            std::cout << "USAGE: " << desc <<  std::endl;
            return 0;
        }

        po::notify(vm);

    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cout << "Exception of unknown type!" << std::endl;
    }

    boost::scoped_ptr<ramen::Core> core(new ramen::Core());
	glm::ivec2 winSize(vm["width"].as<uint32_t>(), vm["height"].as<uint32_t>());

    if (!core->initialize(winSize)) {
        std::cout << "Failed to initialize Core, exiting..." << std::endl;
        return 1;
    }

    core->run();

    return 0;
}
