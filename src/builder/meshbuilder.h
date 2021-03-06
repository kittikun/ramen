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

#ifndef MESH_BUILDER_H
#define MESH_BUILDER_H

#include <fbxsdk.h>
#include <boost/shared_ptr.hpp>

#include "builder.h"
#include "../graphic/mesh.h"

namespace ramen
{
	class Mesh;
    class Database;

    class MeshBuilder final : public Job
    {
    public:
        MeshBuilder(const boost::shared_ptr<Database>& database, const std::string& name, FbxMesh* m_pFbxMesh);

		const bool prepare() final;
        void process() final;

    private:
        void resizeArrays(boost::shared_ptr<Mesh>& mesh);
        void getMeshAttributes(boost::shared_ptr<Mesh>& mesh);
        void calcPolygonCountPerMaterial(boost::shared_ptr<Mesh>& mesh);

    private:
		boost::shared_ptr<Mesh> m_pMesh;
		std::string m_strName;
        const char* m_strUVName;
        FbxGeometryElement::EMappingMode m_materialMappingMode;
        FbxMesh* m_pFbxMesh;
        FbxLayerElementArrayTemplate<int>* m_materialIndice;
    };
} // namespace ramen

#endif // MESH_BUILDER_H
