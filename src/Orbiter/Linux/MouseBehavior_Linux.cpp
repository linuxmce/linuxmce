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
#include "MouseBehavior_Linux.h"

#include "SDL.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#ifndef DIRECTFB
#include <X11/Xutil.h>
#include <X11/Xproto.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>
#endif

#ifndef DIRECTFB
#include "utilities/linux/transparency/transparency.h"
#endif

#ifdef HID_REMOTE
#include "HIDInterface.h"
#endif

using namespace DCE;

//-----------------------------------------------------------------------------------------------------

MouseBehavior_Linux::MouseBehavior_Linux(Orbiter *pOrbiter)
        : MouseBehavior(pOrbiter), m_LastCursorStyle(mcs_Normal)
{
}

MouseBehavior_Linux::~MouseBehavior_Linux()
{
#ifndef DIRECTFB
    OrbiterLinux *pOrbiterLinux = ptrOrbiterLinux();
    // ptr should not be null
    // if it is, then this class was destroyed in the wrong place
    // now fixed, but leave the check anyway, only here
    if (NULL != pOrbiterLinux && NULL != pOrbiterLinux->m_pX11 && pOrbiterLinux->m_pX11->Mouse_IsConstrainActive())
		pOrbiterLinux->m_pX11->Mouse_Constrain_Release();
#endif
}

OrbiterLinux * MouseBehavior_Linux::ptrOrbiterLinux()
{
#ifndef DIRECTFB
    OrbiterLinux * pOrbiterLinux = dynamic_cast<OrbiterLinux *>(m_pOrbiter);
    if (pOrbiterLinux == NULL)
    {
        LoggerWrapper::GetInstance()->Write(
            LV_CRITICAL, "MouseBehavior_Linux::ptrOrbiterLinux() : NULL dynamic_cast<OrbiterLinux *>(%p)",
            m_pOrbiter
            );
        return NULL;
    }
    return pOrbiterLinux;
#endif
}

void MouseBehavior_Linux::SetMousePosition(int X,int Y)
{
#ifndef DIRECTFB
	if(0 == ptrOrbiterLinux()->GetMainWindow())
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "MouseBehavior_Linux::SetMousePosition error: main window not created yet!");
		return;
	}

	MouseBehavior::SetMousePosition(X,Y);
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "MouseBehavior_Linux::SetMousePosition() : Moving mouse (relative %d,%d)",X,Y);
    ptrOrbiterLinux()->m_pX11->Mouse_SetPosition(X, Y);

	// Now purge any pending mouse move events
	PLUTO_SAFETY_LOCK( mt, m_pOrbiter->m_MaintThreadMutex );
	for(map<int,PendingCallBackInfo *>::iterator it=m_pOrbiter->m_mapPendingCallbacks.begin();it!=m_pOrbiter->m_mapPendingCallbacks.end();++it)
	{
		PendingCallBackInfo *pCallBackInfo = (*it).second;
		Orbiter::Event *pEvent_cb = (Orbiter::Event *) pCallBackInfo->m_pData;
		if( pCallBackInfo->m_fnCallBack==&Orbiter::QueueEventForProcessing &&
			pEvent_cb && pEvent_cb->type==Orbiter::Event::MOUSE_MOVE )
				pCallBackInfo->m_bStop=true;
	}
	mt.Release();
#endif
}

void MouseBehavior_Linux::ShowMouse(bool bShow, SetMouseBehaviorRemote setMouseBehaviorRemote)
{
#ifndef DIRECTFB
    // TODO: delete these lines
    //       SDL cannot restore a custom cursor set by the X11 code
    //X11_Locker lock(ptrOrbiterLinux()->GetDisplay());
	//SDL_ShowCursor(bShow ? SDL_ENABLE : SDL_DISABLE);

	// if there is no mouse plugged in, never show the cursor
	// (HID remote also registers a mouse device so no special code needed for it)
	if (!FileUtils::FileExists("/dev/input/mice"))
		bShow = false;
	if (FileUtils::FileExists("/etc/pluto/orbiter.disable.mouse"))
		bShow = false;
    // at show, we want to show the standard mouse cursor
#ifdef HID_REMOTE
	if( m_pOrbiter->m_pHIDInterface )
	{
		if( bShow && (setMouseBehaviorRemote==smb_Default || setMouseBehaviorRemote==smb_TurnOnRemote) )
			m_pOrbiter->m_pHIDInterface->StartMouse();
		else if( !bShow && (setMouseBehaviorRemote==smb_Default || setMouseBehaviorRemote==smb_TurnOffRemote) )
			m_pOrbiter->m_pHIDInterface->StopMouse();
	}
#endif
	m_bMouseVisible=bShow;
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "MouseBehavior_Linux::ShowMouse %d",(int) bShow);
#endif
    if (bShow)
	{
        ptrOrbiterLinux()->m_pX11->Mouse_ShowStandardCursor(ptrOrbiterLinux()->GetMainWindow());
		SetMouseCursorStyle(m_LastCursorStyle);
	}
    else
        ptrOrbiterLinux()->m_pX11->Mouse_HideCursor(ptrOrbiterLinux()->GetMainWindow());
#endif
}

bool MouseBehavior_Linux::ConstrainMouse(const PlutoRectangle &rect)
{
#ifndef DIRECTFB
	if(0 == ptrOrbiterLinux()->GetMainWindow())
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "MouseBehavior_Linux::ConstrainMouse error: main window not created yet!");
		return false;
	}

	m_rMouseConstrained = rect;

	bool bResult = true;
	if(rect.Width == ptrOrbiterLinux()->m_iImageWidth && rect.Height == ptrOrbiterLinux()->m_iImageHeight)
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "MouseBehavior_Linux::ConstrainMouse: releasing constrain mouse ");
#endif
		return ptrOrbiterLinux()->m_pX11->Mouse_Constrain_Release();
	}
	else
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "MouseBehavior_Linux::ConstrainMouse(%d, %d, %d, %d)", rect.X, rect.Y, rect.Width, rect.Height);
#endif
		bResult = ptrOrbiterLinux()->m_pX11->Mouse_Constrain(rect.X, rect.Y, rect.Width, rect.Height, ptrOrbiterLinux()->GetMainWindow());
	}

	if (bResult)
	{
		ptrOrbiterLinux()->m_pWinListManager->ActivateSdlWindow();
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "MouseBehavior_Linux::ConstrainMouse(%d, %d, %d, %d) : done",
			rect.X, rect.Y, rect.Width, rect.Height);
#endif
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "MouseBehavior_Linux::ConstrainMouse(%d, %d, %d, %d) : error",
			rect.X, rect.Y, rect.Width, rect.Height);
	}
	return bResult;
#endif
}

void MouseBehavior_Linux::SetMouseCursorStyle(MouseCursorStyle mouseCursorStyle)
{
#ifndef DIRECTFB
#ifdef DEBUG
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "MouseBehavior_Linux::SetMousePointerStyle(%d)",(int) mouseCursorStyle);
#endif

	m_LastCursorStyle = mouseCursorStyle;

    // convert enum values
    std::string sErr;
    std::string sDir = "/usr/pluto/orbiter/skins/Basic/cursors/pointers_bw/";
    std::string sName;
    int nShape = 0;
    switch (mouseCursorStyle)
    {
        case mcs_Normal:
            nShape = XC_left_ptr;
            if (! ptrOrbiterLinux()->m_pX11->Mouse_SetCursor_Font(ptrOrbiterLinux()->GetMainWindow(), nShape))
            {
                LoggerWrapper::GetInstance()->Write(
                    LV_CRITICAL, "MouseBehavior_Linux::SetMousePointerStyle(%d) : shape==%d",
                    mouseCursorStyle, nShape
                    );
                return;
            }
            return;
        case mcs_LeftRight:
            //nShape = XC_sb_h_double_arrow;
            sName = "left_right.xbm";
            break;
        case mcs_UpDown:
            //nShape = XC_sb_v_double_arrow;
            sName = "up_down.xbm";
            break;
        case mcs_AnyDirection:
            //nShape = XC_cross_reverse;
            sName = "horiz_vert.xbm";
            break;
        case mcs_LeftRightUpDown:
            //nShape = XC_fleur;
            sName = "horiz_vert.xbm";
            break;
        default:
            LoggerWrapper::GetInstance()->Write(
                LV_CRITICAL, "MouseBehavior_Linux::SetMousePointerStyle(%d) : bad value",
                mouseCursorStyle
                );
            return;
    }
    std::string sPath = sDir + sName;
    std::string sPathMask = sPath + ".msk";
    // try to change the cursor
    SetMouseCursorImage(sPath, sPathMask);
#endif
}

bool MouseBehavior_Linux::SetMouseCursorImage(const string &sPath, const string &sPathMask)
{
#ifndef DIRECTFB
    if (! ptrOrbiterLinux()->m_pX11->Mouse_SetCursor_Image(ptrOrbiterLinux()->GetMainWindow(), sPath, sPathMask))
    {
        LoggerWrapper::GetInstance()->Write(
            LV_CRITICAL, "MouseBehavior_Linux::SetMouseCursorImage() : path==%s, pathmask==%s",
            sPath.c_str(), sPathMask.c_str()
            );
        return false;
    }
    return true;
#endif
}
