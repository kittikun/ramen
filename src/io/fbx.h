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

#ifndef FBX_H
#define FBX_H

#include <fbxsdk.h>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

namespace ramen
{
	struct CoreComponents;
	class Database;
	class Filesystem;
	class Job;

	class FBXScene
	{
	public:
		FBXScene(FbxScene* scene, const boost::shared_ptr<Database>& database);

		boost::shared_ptr<Job> createJobMesh();

	private:
		FbxNode* findNode(FbxNodeAttribute::EType type);

	private:
		boost::shared_ptr<Database> m_pDatabase;
		FbxScene* m_pScene;
	};

	class FBXManager
	{
	public:
		FBXManager();
		~FBXManager();

		const bool initialialize(const CoreComponents& components);

		boost::shared_ptr<FBXScene> loadScene(const std::string& filename);

	private:
		FbxManager* m_pFbxManager;
		boost::shared_ptr<Database> m_pDatabase;
		boost::shared_ptr<Filesystem> m_pFilesystem;
	};
} // namespace ramen

#endif // FBX_H
