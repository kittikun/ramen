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

#include "filesystem.h"

#if defined(_WIN32)
#include "windows.h"
#endif

#include "../log.h"

namespace ramen
{

	void Filesystem::findResourcePath()
	{
		boost::filesystem::directory_iterator end;
		boost::filesystem::path path = m_workingDir;
		bool found = false;

		while (!found) {
			for(boost::filesystem::directory_iterator iter(path) ; iter != end ; ++iter) {
				if (boost::filesystem::is_directory(*iter)) {
					if (iter->path().leaf() == "resources") {
						m_resourcePath = iter->path();
						found = true;
						break;
					}
				}
			}

			if (!found) {
				path = path.parent_path();
			}
		}

		LOGI << "Resource directory: " << m_resourcePath;
	}

	void Filesystem::findWorkingDir()
	{
#if defined(_WIN32)
		std::vector<char> exePath(64);
		DWORD res = ::GetModuleFileNameA(nullptr, &exePath[0], static_cast<DWORD>(exePath.size()));

		// buffer might be too short
		while(res == exePath.size()) {
			exePath.resize(exePath.size() * 2);
			res = ::GetModuleFileNameA(nullptr, &exePath[0], static_cast<DWORD>(exePath.size()));
		}

		m_workingDir = std::string(exePath.begin(), exePath.begin() + res);
#endif

		m_workingDir = m_workingDir.remove_filename();
		LOGI << "Working directory: " << m_workingDir;
	}

	const bool Filesystem::initialize()
	{
		findWorkingDir();
		findResourcePath();

		return true;
	}

	const std::string Filesystem::resourcePathStr(uint32_t type, const std::string& filename) const
	{
		std::string pathStr;
		boost::filesystem::path pathFS;

		if (type == TYPE_FONT) {
			pathFS = m_resourcePath / "font" / filename;

			if (!boost::filesystem::exists(pathFS)) {
				LOGE << pathFS << " does not exists";
			}

			pathStr = pathFS.string();
		}

		return pathStr;
	}

} // namespace ramen
