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
#include "MouseBehavior_Win32.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
using namespace DCE;


//-----------------------------------------------------------------------------------------------------
MouseBehavior_Win32::MouseBehavior_Win32(Orbiter *pOrbiter)
	: MouseBehavior(pOrbiter)
{
}

void MouseBehavior_Win32::SetMousePosition(int X,int Y)
{
	int nWindowX = 0, nWindowY = 0;
	GetWindowPosition(nWindowX, nWindowY);

	MouseBehavior::SetMousePosition(nWindowX + X, nWindowY + Y);
	
	::SetCursorPos(nWindowX + X, nWindowY + Y);
}

void MouseBehavior_Win32::ShowMouse(bool bShow, SetMouseBehaviorRemote setMouseBehaviorRemote)
{
	m_bMouseVisible=bShow;
	ShowCursor(bShow);
}

bool MouseBehavior_Win32::ConstrainMouse(const PlutoRectangle &rect)
{
	m_rMouseConstrained = rect;
	int nWindowX = 0, nWindowY = 0;
	GetWindowPosition(nWindowX, nWindowY);

	m_bMouseConstrained = rect.X!=0 || rect.Y!=0 || rect.Width!=0 || rect.Height!=0;
	if( rect.Width==0 || rect.Height==0 )
	{
		ClipCursor(NULL);
		return true;
	}

	RECT rcClip;           // new area for ClipCursor
	rcClip.left=nWindowX + rect.X;
	rcClip.right=nWindowX + rect.X + rect.Width;
	rcClip.top=nWindowY + rect.Y;
	rcClip.bottom=nWindowY + rect.Y + rect.Height;
	ClipCursor(&rcClip); 
	return true;
}

void MouseBehavior_Win32::SetMouseCursorStyle(MouseCursorStyle mouseCursorStyle)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "SetMouseCursorStyle %d", mouseCursorStyle);

	LPCTSTR lpCursor;

	switch(mouseCursorStyle)
	{
	case mcs_Normal:
		lpCursor =	IDC_ARROW;
		break;
	case mcs_LeftRight:
		lpCursor =	IDC_SIZEWE;
		break;
	case mcs_UpDown:
		lpCursor =	IDC_SIZENS;
		break;
	case mcs_AnyDirection:
		lpCursor =	IDC_SIZEALL;
		break;
	case mcs_LeftRightUpDown:
		lpCursor =	IDC_CROSS;
		break;
	};

	HCURSOR hCursor = LoadCursor(NULL, lpCursor);
	SetCursor(hCursor);
}

void MouseBehavior_Win32::GetWindowPosition(int &x, int &y)
{
	//we'll need to window's position in order to apply the constrain relatively to the window's position
	HWND hWnd = ::FindWindow("SDL_app", "OrbiterGL");
	RECT rectWin;
	::GetWindowRect(hWnd, &rectWin);

	int nTitleHeigth = ::GetSystemMetrics(SM_CYSIZE);
	int nBorderWidth = ::GetSystemMetrics(SM_CXBORDER);

	x = rectWin.left + nBorderWidth;
	y = rectWin.top + nTitleHeigth + nBorderWidth;
}
