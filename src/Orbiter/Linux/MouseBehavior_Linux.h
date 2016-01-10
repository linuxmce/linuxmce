/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#ifndef __MouseBehavior_Linux_H__
#define __MouseBehavior_Linux_H__
//-----------------------------------------------------------------------------------------------------
#include <list>
#include <string>
#include <map>
using namespace std;

#include "../MouseBehavior.h"
using namespace DCE;
#include "OrbiterLinux.h"

namespace DCE
{
    //-----------------------------------------------------------------------------------------------------
	/**
	* @brief Handles special mouse behavior in a Linux environment
	*/
	//-----------------------------------------------------------------------------------------------------
	class MouseBehavior_Linux : public MouseBehavior
	{
	public:
		MouseBehavior_Linux(Orbiter *pOrbiter);
		~MouseBehavior_Linux();
		virtual void SetMousePosition(int X,int Y);
		virtual void ShowMouse(bool bShow, SetMouseBehaviorRemote setMouseBehaviorRemote);
        virtual bool ConstrainMouse(const PlutoRectangle &rect);
		virtual void SetMouseCursorStyle(MouseCursorStyle mouseCursorStyle);
		virtual bool SetMouseCursorImage(const string &sPath, const string &sPathMask);
    protected:
        OrbiterLinux * ptrOrbiterLinux();

		MouseCursorStyle m_LastCursorStyle;
	};
}

//-----------------------------------------------------------------------------------------------------

#endif
// __MouseBehavior_Linux_H__
