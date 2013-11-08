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

#ifndef IO_UTILITY_H
#define IO_UTILITY_H

#include <boost/filesystem.hpp>

template < >
boost::filesystem::path& boost::filesystem::path::append< typename boost::filesystem::path::iterator >(typename boost::filesystem::path::iterator begin, typename boost::filesystem::path::iterator end, const boost::filesystem::path::codecvt_type& cvt)
{
    for(; begin != end ; ++begin)
        *this /= *begin;
    return *this;
}

namespace ramen {
    namespace ioUtility
    {
        // Return path when appended to from will resolve to same as to
        boost::filesystem::path makeRelativePath(boost::filesystem::path from, boost::filesystem::path to)
        {
            boost::filesystem::path ret;
            boost::filesystem::path::const_iterator itrFrom(from.begin()), itrTo(to.begin());

            from = boost::filesystem::absolute(from); to = boost::filesystem::absolute(to);

            // Find common base
            for(boost::filesystem::path::const_iterator toEnd(to.end()), fromEnd(from.end()) ; itrFrom != fromEnd && itrTo != toEnd && *itrFrom == *itrTo; ++itrFrom, ++itrTo);

            // Navigate backwards in directory to reach previously found base
            for(boost::filesystem::path::const_iterator fromEnd(from.end()); itrFrom != fromEnd; ++itrFrom)
            {
                if((*itrFrom) != ".")
                    ret /= "..";
            }
            // Now navigate down the directory branch
            ret.append(itrTo, to.end());
            return ret;
        }
    } // namespace ioUtility
} // namespace ramen

#endif IO_UTILITY_H