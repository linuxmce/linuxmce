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
#ifndef __MouseBehavior_Win32_H__
#define __MouseBehavior_Win32_H__
//-----------------------------------------------------------------------------------------------------
#include <list>
#include <string>
#include <map>
using namespace std;

#include "MouseBehavior.h"
using namespace DCE;

namespace DCE
{
	//-----------------------------------------------------------------------------------------------------
	/**
	* @brief Handles special mouse behavior in a Win32 environment
	*/
	//-----------------------------------------------------------------------------------------------------
	class MouseBehavior_Win32 : public MouseBehavior
	{
	public:
		MouseBehavior_Win32(Orbiter *pOrbiter);
		virtual void SetMousePosition(int X,int Y);
		virtual void GetMousePosition(PlutoPoint *p) { POINT pt; GetCursorPos(&pt); p->X=pt.x; p->Y=pt.y; }
		virtual void ShowMouse(bool bShow, SetMouseBehaviorRemote setMouseBehaviorRemote);

		bool ConstrainMouse(const PlutoRectangle &rect);
		void SetMouseCursorStyle(MouseCursorStyle mouseCursorStyle);

	private:
		void GetWindowPosition(int &x, int &y);
	};
}
//-----------------------------------------------------------------------------------------------------
#endif
