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

#ifndef PROFILER_H
#define PROFILER_H

#include <boost/chrono.hpp>
#include <boost/utility.hpp>

#define PROFILE profiler::AutoProfile _profile(__FUNCTION__)

namespace ramen
{
    namespace profiler
    {
        class AutoProfile : boost::noncopyable
        {
        public:
            AutoProfile(const std::string& name);
            ~AutoProfile();

        private:
            boost::chrono::system_clock::time_point m_start;
            std::string m_strName;
        };

        static void dump();
    } // namespace profiler
} // namespace ramen

#endif // PROFILER