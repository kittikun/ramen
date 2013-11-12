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

#include "database.h"

#include "log.h"
#include "entity/entity.h"

namespace ramen
{
	void Database::remove(const std::string& key)
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
		auto found = m_data.find(key);

		if (found == m_data.end()) {
			LOGE << "Database erase key \"" << key << "\" doesn't exist";
		} else {
			m_data.erase(found);
		}
	}
} // namespace ramen