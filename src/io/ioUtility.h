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

    // Return path when appended to a_From will resolve to same as a_To
    boost::filesystem::path makeRelativePath(boost::filesystem::path a_From, boost::filesystem::path a_To)
    {
        boost::filesystem::path ret;
        boost::filesystem::path::const_iterator itrFrom(a_From.begin()), itrTo(a_To.begin());

		a_From = boost::filesystem::absolute(a_From); a_To = boost::filesystem::absolute(a_To);

        // Find common base
		for(boost::filesystem::path::const_iterator toEnd(a_To.end()), fromEnd(a_From.end()) ; itrFrom != fromEnd && itrTo != toEnd && *itrFrom == *itrTo; ++itrFrom, ++itrTo);

		// Navigate backwards in directory to reach previously found base
		for(boost::filesystem::path::const_iterator fromEnd(a_From.end()); itrFrom != fromEnd; ++itrFrom)
		{
			if((*itrFrom) != ".")
				ret /= "..";
		}
		// Now navigate down the directory branch
		ret.append(itrTo, a_To.end());
		return ret;
	}

	} // namespace ioUtility
} // namespace ramen

#endif IO_UTILITY_H