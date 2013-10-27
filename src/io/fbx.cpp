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

#include "fbx.h"

#include "filesystem.h"

#include "../log.h"

namespace ramen
{
	Fbx::Fbx()
		: m_pFbxManager(FbxManager::Create())
		, m_pFilesystem(nullptr)
	{
	}

	Fbx::~Fbx()
	{
		m_pFbxManager->Destroy();
	}

	const bool Fbx::initialialize(Filesystem const* filesystem)
	{
		m_pFilesystem = filesystem;

		FbxIOSettings *ioSettings = FbxIOSettings::Create(m_pFbxManager, IOSROOT);
		m_pFbxManager->SetIOSettings(ioSettings);

		FbxImporter* lImporter = FbxImporter::Create(m_pFbxManager,"");
		std::string path;

		path = m_pFilesystem->resourcePath(Filesystem::TYPE_FBX, "teapot.fbx");
		if (path.empty()) {
			return false;
		}

		if(!lImporter->Initialize(path.c_str(), -1, m_pFbxManager->GetIOSettings())) { 
			LOGE << "Call to FbxImporter::Initialize() failed with " << lImporter->GetStatus().GetErrorString(); 
			return false;
		}

		FbxScene* lScene = FbxScene::Create(m_pFbxManager,"myScene");

		lImporter->Import(lScene);
		lImporter->Destroy();

		return true;
	}

} // namespace ramen