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

#ifndef FBX_UTILITY_H
#define FBX_UTILITY_H

#include <fbxsdk.h>
#include <string>

namespace ramen
{
	namespace fbxUtility
	{
		extern void printNode(FbxNode* pNode, const int numTabs = 0);
		extern std::string getAttributeStr(FbxNodeAttribute::EType type);

	} // namespace fbsUtility

} // namespace ramen

#endif // FBX_UTILITY_H