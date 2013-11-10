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

#include <sstream>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/thread.hpp>
#include <boost/unordered_map.hpp>

#include "../log.h"
#include "../utility.h"

namespace ramen
{
    namespace profiler
    {
        struct ProfileData
        {
            unsigned int iCount;
            std::string strName;
            boost::chrono::duration<long long, boost::micro> totalTime;
            boost::chrono::duration<long long, boost::micro> min;
            boost::chrono::duration<long long, boost::micro> max;
        };

        static boost::mutex m_mutex;
        static boost::unordered_map<std::string, ProfileData> m_profiles;

        static const std::string readableDuration(const boost::chrono::duration<long long, boost::micro>& duration)
        {
            std::stringstream stream;
            int digits = utility::calcNumDigits(duration.count(), false);

            stream << boost::chrono::duration_short;

            if (digits <= 3) {
                stream << duration;
            } else if ((digits > 3) && (digits <= 6)) {
                stream << boost::chrono::duration_cast<boost::chrono::milliseconds>(duration);
            } else if ((digits > 6) && (digits <= 9)) {
                stream << boost::chrono::duration_cast<boost::chrono::seconds>(duration);
            } else if (digits > 9)
            {
                stream << boost::chrono::duration_cast<boost::chrono::minutes>(duration);
            }

            return stream.str();
        }

        void dump()
        {
            boost::lock_guard<boost::mutex> guard(m_mutex);

            LOGP << "Dumping profiling information";

            BOOST_FOREACH(auto profile, m_profiles | boost::adaptors::map_values ) {
                ProfileData& data = profile;

                // trying to print chrono duration with log or format will not Job with short duration so fallback to standard streams
                LOGP << boost::format("%1% avg:%2% min:%3% max:%4% cnt:%5% tot:%6%")
                    % data.strName
                    % readableDuration(data.totalTime / data.iCount)
                    % readableDuration(data.min) % readableDuration(data.max)
                    % data.iCount % readableDuration(data.totalTime);
            }
        }

        static void update(const std::string& name, boost::chrono::duration<long long, boost::micro> duration)
        {
            boost::lock_guard<boost::mutex> guard(m_mutex);

            auto iter = m_profiles.find(name);

            if (iter != m_profiles.end()) {
                ProfileData& data = iter->second;

                data.totalTime += duration;

                if (duration > data.max) {
                    data.max = duration;
                } else if(duration < data.min) {
                    data.min = duration;
                }

                ++data.iCount;
            } else {
                ProfileData data;

                data.totalTime = duration;
                data.min = duration;
                data.max = duration;
                data.strName = name;
                data.iCount = 1;

                m_profiles.insert(std::make_pair(name, data));
            }
        }

        //-------------------------------------------------------------------------------------
        // AUTOPROFILE
        //-------------------------------------------------------------------------------------
        AutoProfile::AutoProfile(const std::string& name)
            : m_start(boost::chrono::system_clock::now())
            , m_strName(name)
        {
        }

        AutoProfile::~AutoProfile()
        {
            update(m_strName, boost::chrono::duration_cast<boost::chrono::microseconds>(boost::chrono::system_clock::now() - m_start));
        }
    } // namespace profiler
} // namespace ramen