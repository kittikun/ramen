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

#include <boost/foreach.hpp>

#include "../coreComponents.h"
#include "../database.h"
#include "../log.h"
#include "../builder/meshbuilder.h"
#include "../graphic/mesh.h"
#include "../perfmon/profiler.h"
#include "filesystem.h"
#include "fbxUtility.h"

namespace ramen
{
    //-------------------------------------------------------------------------------------
    // FBXMANAGER
    //-------------------------------------------------------------------------------------
    FBXManager::FBXManager()
        : m_pFbxManager(nullptr)
    {
    }

    FBXManager::~FBXManager()
    {
        LOGI << "Destroying FBX manager..";

        if (m_pFbxManager) {
            m_pFbxManager->Destroy();
        }
    }

    const bool FBXManager::initialialize(const CoreComponents* components)
    {
        LOGI << "Initializing FBX manager..";
        m_pFilesystem = components->filesystem;
        m_pDatabase = components->database;

        m_pFbxManager = FbxManager::Create();
        if(!m_pFbxManager)
        {
            LOGE << "Unable to create Fbx manager";
            return false;
        }

        LOGI << "Autodesk FBX SDK version " << m_pFbxManager->GetVersion();

        FbxIOSettings *ioSettings = FbxIOSettings::Create(m_pFbxManager, IOSROOT);
        m_pFbxManager->SetIOSettings(ioSettings);

        return true;
    }

    boost::shared_ptr<FBXScene> FBXManager::loadScene(const std::string& filename)
    {
        PROFILE;
        FbxImporter* importer  = nullptr;
        FbxScene* scene = nullptr;
        std::string pathAbs;
        std::string pathRel;

        pathRel = m_pFilesystem->resourcePathRel(Filesystem::ResourceType::Fbx, filename);
        pathAbs = m_pFilesystem->resourcePathAbs(Filesystem::ResourceType::Fbx, filename);
        if (pathAbs.empty()) {
            return boost::shared_ptr<FBXScene>();
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
            LOGE << "Unable to create a FbxScene";
            return boost::shared_ptr<FBXScene>();
        }

        if (!importer->Import(scene)) {
            LOGE << "Failed to import scene '" << filename << "'";
            return boost::shared_ptr<FBXScene>();
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

        // Convert fbxMesh, NURBS and patch into triangle fbxMesh
        FbxGeometryConverter geomConverter(m_pFbxManager);
        geomConverter.Triangulate(scene, true);

        // Split meshes per material, so that we only have one material per fbxMesh (for VBO support)
        geomConverter.SplitMeshesPerMaterial(scene, true);

        importer->Destroy();

        return boost::shared_ptr<FBXScene>(new FBXScene(scene, m_pDatabase));
    }

    //-------------------------------------------------------------------------------------
    // FBXSCENE
    //-------------------------------------------------------------------------------------
    FBXScene::FBXScene(FbxScene* scene, const boost::shared_ptr<Database>& database)
        : m_pDatabase(database)
        , m_pScene(scene)
    {
    }

    FbxNode* FBXScene::findNode(FbxNodeAttribute::EType type)
    {
        FbxNode* node = m_pScene->GetRootNode();
        std::vector<FbxNode*> toExplore;

        for (int i = 0; i < node->GetChildCount(); ++i) {
            toExplore.push_back(node->GetChild(i));
        }

        while (!toExplore.empty()) {
            node = toExplore.back();
            toExplore.pop_back();

            if (node->GetNodeAttribute()->GetAttributeType() == type) {
                return node;
            }

            for (int i = 0; i < node->GetChildCount(); ++i) {
                toExplore.push_back(node->GetChild(i));
            }
        }

        LOGE << "Couldn't find any node of type " << fbxUtility::getAttributeStr(type);

        return nullptr;
    }

    boost::shared_ptr<Job> FBXScene::createJobMesh()
    {
        boost::shared_ptr<MeshBuilder> job;
        FbxNode* node = findNode(FbxNodeAttribute::eMesh);

        if (node != nullptr) {
            job.reset(new MeshBuilder(m_pDatabase, "mesh", node->GetMesh()));
        }

        return boost::dynamic_pointer_cast<Job>(job);
    }
} // namespace ramen