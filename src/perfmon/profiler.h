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

#include <boost/unordered_map.hpp>
#include <boost/chrono.hpp>
#include <boost/thread.hpp>
#include <boost/utility.hpp>

#define PROFILE Profile profile(__FUNCTION__)

namespace ramen
{
	struct ProfileData
	{
		unsigned int iCount;
		std::string strName;
		boost::chrono::duration<long long, boost::micro> totalTime;
		boost::chrono::duration<long long, boost::micro> min;
		boost::chrono::duration<long long, boost::micro> max;
	};

	class Profile : boost::noncopyable
	{
	public:
		Profile(const std::string& name);
		~Profile();

	private:
		boost::chrono::system_clock::time_point m_start;
		std::string m_strName;

	};

    class Profiler : boost::noncopyable
    {
    public:

        static const std::string readableDuration(const boost::chrono::duration<long long, boost::micro>& duration);
        static void update(const std::string& name, boost::chrono::duration<long long, boost::micro> duration);
        static void dump();

    private:
        static boost::mutex m_mutex;
        static boost::unordered_map<std::string, ProfileData> m_profiles;
    };

} // namespace ramen

#endif // PROFILER