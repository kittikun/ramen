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
#include <windows.h>
#endif

#include <fstream>

#include "../log.h"
#include "../utility.h"
#include "ioUtility.h"

namespace ramen
{

    void Filesystem::findresourcePathAbs()
    {
        const boost::filesystem::directory_iterator end;
        boost::filesystem::path path = m_workingDir;
        bool found = false;

        while (!found) {
            for (boost::filesystem::directory_iterator iter(path); iter != end; ++iter) {
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
        std::vector<char> exePath(64);

#if defined(_WIN32)
        DWORD res = ::GetModuleFileNameA(nullptr, &exePath[0], static_cast<DWORD>(exePath.size()));

        while (res == exePath.size()) {
            exePath.resize(exePath.size() * 2);
            res = ::GetModuleFileNameA(nullptr, &exePath[0], static_cast<DWORD>(exePath.size()));
        }
#elif defined(__linux__)
        std::string cmd = boost::str(boost::format("/proc/%1%/exe") % getpid());
        size_t res = readlink(cmd.c_str(), &exePath[0], exePath.size());

        while (res == exePath.size()) {
            exePath.resize(exePath.size() * 2);
            res = readlink(cmd.c_str(), &exePath[0], exePath.size());
        }
#endif
        m_workingDir = std::string(exePath.begin(), exePath.begin() + res); 
        m_workingDir = m_workingDir.remove_filename();
        LOGI << "Working directory: " << m_workingDir;
    }

	const boost::filesystem::path Filesystem::fsresourcePathAbs(ResourceType type, const std::string& filename) const
	{
		boost::filesystem::path pathFS;

		switch (type) {
		case TYPE_FBX:
			pathFS = m_resourcePath / "fbx" / filename;
			break;

		case TYPE_FONT:
			pathFS = m_resourcePath / "font" / filename;
			break;
		case TYPE_SHADER:
			pathFS = m_resourcePath / "shader" / filename;
			break;

		default:
			LOGE << "Unrecognized resource type";
			break;
		}

		if (!boost::filesystem::exists(pathFS)) {
			LOGE << pathFS << " does not exists";
		}

		return pathFS;
	}

    const bool Filesystem::initialize()
    {
        findWorkingDir();
        findresourcePathAbs();

        return true;
    }

	char const* Filesystem::resource(ResourceType type, const std::string& filename) const
	{
		const boost::filesystem::path absPath = fsresourcePathAbs(type, filename);
		std::ifstream stream(absPath.string());
		uint32_t size;
		char* data = nullptr;

		if (absPath.empty()) {
			return nullptr;
		}

		if (stream.is_open()) {
			if (type == TYPE_SHADER) {
				std::string str((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());

				data = new char[str.size() +1];
				std::copy(str.begin(), str.end(), data);
				size = str.size();
				data[size] = '\0';
			} else {
				size = static_cast<uint32_t>(boost::filesystem::file_size(absPath));
				data = new char[size];
				stream.read(data, size);
			}

			LOGI << boost::format("Read %1% from %2%..") % utility::readableSizeByte(size) % ioUtility::makeRelativePath(m_resourcePath, absPath);
		}

		return data;
	}

    const std::string Filesystem::resourcePathAbs(ResourceType type, const std::string& filename) const
    {
		return fsresourcePathAbs(type, filename).string();
    }

	const std::string Filesystem::resourcePathRel(ResourceType type, const std::string& filename) const
	{
		boost::filesystem::path absPath = fsresourcePathAbs(type, filename).string();
		return ioUtility::makeRelativePath(m_resourcePath, absPath).string();
	}

} // namespace ramen
