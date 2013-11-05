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

#include "fbxUtility.h"

#include <sstream>
#include <boost/format.hpp>
#include <glm/glm.hpp>
#include "../log.h"

namespace ramen
{
    namespace fbxUtility
    {
        std::string getAttributeStr(FbxNodeAttribute::EType type)
        {
            switch(type) {
            case FbxNodeAttribute::eUnknown: return "Unidentified";
            case FbxNodeAttribute::eNull: return "Null";
            case FbxNodeAttribute::eMarker: return "Marker";
            case FbxNodeAttribute::eSkeleton: return "Skeleton";
            case FbxNodeAttribute::eMesh: return "Mesh";
            case FbxNodeAttribute::eNurbs: return "Nurbs";
            case FbxNodeAttribute::ePatch: return "Patch";
            case FbxNodeAttribute::eCamera: return "Camera";
            case FbxNodeAttribute::eCameraStereo: return "Stereo";
            case FbxNodeAttribute::eCameraSwitcher: return "Camera switcher";
            case FbxNodeAttribute::eLight: return "Light";
            case FbxNodeAttribute::eOpticalReference: return "Optical reference";
            case FbxNodeAttribute::eOpticalMarker: return "Marker";
            case FbxNodeAttribute::eNurbsCurve: return "Nurbs curve";
            case FbxNodeAttribute::eTrimNurbsSurface: return "Trim nurbs surface";
            case FbxNodeAttribute::eBoundary: return "Boundary";
            case FbxNodeAttribute::eNurbsSurface: return "Nurbs surface";
            case FbxNodeAttribute::eShape: return "Shape";
            case FbxNodeAttribute::eLODGroup: return "Lodgroup";
            case FbxNodeAttribute::eSubDiv: return "Subdiv";
            default: return "unknown";
            }
        }

        static void printAttribute(FbxNodeAttribute* pAttribute, int numTabs)
        {
            std::stringstream tabStream;

            if(!pAttribute) {
                return;
            }

            for(int i = 0; i < numTabs; i++)
                tabStream << "\t";

            boost::format fmt("<attribute type='%1%' name='%2%'/>");

            LOGI << tabStream.str() << fmt % getAttributeStr(pAttribute->GetAttributeType()) % pAttribute->GetName();
        }

        void printNode(FbxNode* pNode, const int numTabs)
        {
            std::stringstream tabStream;
            const char* nodeName = pNode->GetName();
            FbxDouble3 translation = pNode->LclTranslation.Get();
            FbxDouble3 rotation = pNode->LclRotation.Get();
            FbxDouble3 scaling = pNode->LclScaling.Get();

            for(int i = 0; i < numTabs; i++)
                tabStream << "\t";

            // Print the contents of the node.
            boost::format fmt("<node name=%1%, translation=(%2%, %3%, %4%), rotation=(%5%, %6%, %7%), scale=(%8%, %9%, %10%)>");

            LOGI << tabStream.str() << fmt	% nodeName
                % translation[0] % translation[1] % translation[2]
            % rotation[0] % rotation[1] % rotation[2]
            % scaling[0] % scaling[1] % scaling[2];

            // Print the node's attributes.
            for(int i = 0; i < pNode->GetNodeAttributeCount(); i++)
                printAttribute(pNode->GetNodeAttributeByIndex(i), numTabs + 1);

            // Recursively print the children.
            for(int j = 0; j < pNode->GetChildCount(); j++)
                printNode(pNode->GetChild(j), numTabs + 1);

            LOGI << tabStream.str() << "</node>\n";
        }
    } // namespace fbsUtility
} // namespace ramen