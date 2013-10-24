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

#include "profiler.h"

#include <boost/format.hpp>

#include "log.h"

namespace ramen
{
	boost::mutex Profiler::m_mutex;
	std::map<std::string, ProfileData> Profiler::m_profiles;

	Profile::Profile(const std::string& name)
		: m_start(boost::chrono::system_clock::now())
		, m_strName(name)
	{
		
	}

	Profile::~Profile()
	{
		Profiler::update(m_strName, boost::chrono::duration_cast<boost::chrono::milliseconds>(boost::chrono::system_clock::now() - m_start));
	}

	void Profiler::dump()
	{
		boost::lock_guard<boost::mutex> guard(m_mutex);
		auto end = m_profiles.end();

		LOG << "Dumping profiling information";

		for (auto iter = m_profiles.begin(); iter != end; ++iter) {
			ProfileData& data = iter->second;

			LOG << boost::format("%1%\navg:%2%\nmin:%3%\nmax:%4%\ncount:%5%\ntotal:%6%") % data.strName % (data.totalTime / data.iCount) % data.min % data.max % data.iCount % data.totalTime;
		}
	}

	void Profiler::update(const std::string& name, boost::chrono::duration<long long, boost::milli> duration)
	{
		boost::lock_guard<boost::mutex> guard(m_mutex);

		auto iter = m_profiles.find(name);

		if (iter != m_profiles.end()) {
			ProfileData& data = m_profiles[name];

			data.totalTime += duration;

			if (duration > data.max) {
				data.max = duration;
			} else if(duration < data.min) {
				data.min = duration;
			}

			++data.iCount;

		} else {
			ProfileData data;

			data.totalTime += duration;
			data.min = duration;
			data.max = duration;
			data.strName = name;
			data.iCount = 1;

			m_profiles.insert(std::make_pair(name, data));
		}
	}

} // namespace ramen