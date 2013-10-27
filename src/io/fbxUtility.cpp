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

		static std::string getAttributeStr(FbxNodeAttribute::EType type) { 
			switch(type) { 
			case FbxNodeAttribute::eUnknown: return "unidentified"; 
			case FbxNodeAttribute::eNull: return "null"; 
			case FbxNodeAttribute::eMarker: return "marker"; 
			case FbxNodeAttribute::eSkeleton: return "skeleton"; 
			case FbxNodeAttribute::eMesh: return "mesh"; 
			case FbxNodeAttribute::eNurbs: return "nurbs"; 
			case FbxNodeAttribute::ePatch: return "patch"; 
			case FbxNodeAttribute::eCamera: return "camera"; 
			case FbxNodeAttribute::eCameraStereo: return "stereo"; 
			case FbxNodeAttribute::eCameraSwitcher: return "camera switcher"; 
			case FbxNodeAttribute::eLight: return "light"; 
			case FbxNodeAttribute::eOpticalReference: return "optical reference"; 
			case FbxNodeAttribute::eOpticalMarker: return "marker"; 
			case FbxNodeAttribute::eNurbsCurve: return "nurbs curve"; 
			case FbxNodeAttribute::eTrimNurbsSurface: return "trim nurbs surface"; 
			case FbxNodeAttribute::eBoundary: return "boundary"; 
			case FbxNodeAttribute::eNurbsSurface: return "nurbs surface"; 
			case FbxNodeAttribute::eShape: return "shape"; 
			case FbxNodeAttribute::eLODGroup: return "lodgroup"; 
			case FbxNodeAttribute::eSubDiv: return "subdiv"; 
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
