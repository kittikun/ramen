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

#include "../log.h"
#include "filesystem.h"
#include "fbxUtility.h"

namespace ramen
{
	Fbx::Fbx()
		: m_pFbxManager(nullptr)
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

		m_pFbxManager = FbxManager::Create();
		if(!m_pFbxManager)
		{
			LOGE << "Unable to create FBX Manager";
			return false;
		} else {
			LOGI << "Autodesk FBX SDK version " << m_pFbxManager->GetVersion();
		}

		FbxIOSettings *ioSettings = FbxIOSettings::Create(m_pFbxManager, IOSROOT);
		m_pFbxManager->SetIOSettings(ioSettings);

		return true;
	}

	const bool Fbx::loadfile(const std::string& filename)
	{
		FbxImporter* importer  = nullptr;
		FbxScene* scene = nullptr;
		std::string pathAbs;
		std::string pathRel;

		pathRel = m_pFilesystem->resourcePathRel(Filesystem::TYPE_FBX, filename);
		pathAbs = m_pFilesystem->resourcePathAbs(Filesystem::TYPE_FBX, filename);
		if (pathAbs.empty()) {
			return false;
		}

		LOGI << "Loading FBX file '" << pathRel << "'..";

		importer = FbxImporter::Create(m_pFbxManager,"");
		if(!importer->Initialize(pathAbs.c_str(), -1, m_pFbxManager->GetIOSettings())) { 
			LOGE << "FbxImporter initialization for failed with " << importer->GetStatus().GetErrorString(); 
			return false;
		}

		scene = FbxScene::Create(m_pFbxManager, filename.c_str());
		if(!scene)
		{
			LOGE << "Unable to create FBX scene";
			return false;
		}

		if (!importer->Import(scene)) {
			LOGE << "Failed to import scene '" << filename << "'";
		}

		// Convert Axis System if needed
		FbxAxisSystem sceneAxisSystem = scene->GetGlobalSettings().GetAxisSystem();
		FbxAxisSystem axisSystem(FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eRightHanded);
		if(sceneAxisSystem != axisSystem)
		{
			LOGW << "Scene needed axis convertion";
			axisSystem.ConvertScene(scene);
		}

		//// Convert Unit System if needed
		//FbxSystemUnit sceneSystemUnit = scene->GetGlobalSettings().GetSystemUnit();
		//if(sceneSystemUnit.GetScaleFactor() != 1.0)
		//{
		//	LOGW << "Scene needed unit convertion";
		//	FbxSystemUnit::cm.ConvertScene(scene);
		//}

		// Convert mesh, NURBS and patch into triangle mesh
		FbxGeometryConverter geomConverter(m_pFbxManager);
		geomConverter.Triangulate(scene, true);

		// Split meshes per material, so that we only have one material per mesh (for VBO support)
		geomConverter.SplitMeshesPerMaterial(scene, true);


		FbxNode* root = scene->GetRootNode();

		fbxUtility::printNode(root);

		importer->Destroy();

		return true;
	}

} // namespace ramen