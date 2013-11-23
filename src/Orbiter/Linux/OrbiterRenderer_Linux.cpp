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
#include "OrbiterLinux.h"

#include <stdlib.h>

#include "DCE/Logger.h"
#include "pluto_main/Define_DesignObj.h"

#ifdef MAEMO_NOKIA770
	#include "../pluto_main/Define_Button.h"
	#include "../pluto_main/Define_Direction.h"
#endif

#include <iomanip>
#include <sstream>
#include <sys/time.h>

#include <X11/Xutil.h>
#include <X11/Xproto.h>
#include <X11/keysymdef.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>

#include <SDL/SDL_syswm.h>
#include "../CallBackTypes.h"
#include "../dialog_types.h"

#include "OrbiterRenderer_Linux.h"
#include "../ScreenHistory.h"

#ifdef ORBITER_OPENGL
	#define BASE_CLASS OrbiterRenderer_OpenGL
	#include "../OpenGL/OpenGL3DEngine.h"
#else
	#define BASE_CLASS OrbiterRenderer_SDL
#endif

#ifdef ORBITER_OPENGL
#ifndef USE_SDL_GL_PATCH
	#if !defined(VIA_OVERLAY)
		#include "utilities/linux/CompositeHelper.h"
	#endif
#endif
#endif

using namespace DCE;

#include "../ProgressBarConfig.h"

OrbiterRenderer_Linux::OrbiterRenderer_Linux(Orbiter *pOrbiter) : BASE_CLASS(pOrbiter), 
#ifndef DIRECTFB
	m_screenMaskObjects(None), m_screenMaskPopups(None), m_screenMaskCurrent(None), 
#endif
	m_bHasPopups(false), m_bScreenRendered(false)
{
}

OrbiterRenderer_Linux::~OrbiterRenderer_Linux()
{
#ifndef DIRECTFB
	OrbiterLinux *pOrbiterLinux = dynamic_cast<OrbiterLinux *>(OrbiterLogic());
	
	if (pOrbiterLinux != NULL)
	{
		pOrbiterLinux->m_pX11->Delete_Pixmap(m_screenMaskObjects);
		pOrbiterLinux->m_pX11->Delete_Pixmap(m_screenMaskPopups);
		pOrbiterLinux->m_pX11->Delete_Pixmap(m_screenMaskCurrent);
	}
#endif
}

bool OrbiterRenderer_Linux::HandleShowPopup(PlutoPopup* Popup, PlutoPoint Position, int EffectID)
{
#ifndef DIRECTFB
	OrbiterLinux *pOrbiterLinux = dynamic_cast<OrbiterLinux *>(OrbiterLogic());
	
	if (pOrbiterLinux != NULL && pOrbiterLinux->m_bUseMask)
	{
		m_bHasPopups = true;

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "qqq Shape_PixmapMask_Rectangle %d,%d,%d,%d opaque",
			Popup->m_Position.X,
			Popup->m_Position.Y,
			Popup->m_pObj->m_rPosition.Width,
			Popup->m_pObj->m_rPosition.Height);

		//modify mask
		pOrbiterLinux->m_pX11->Shape_PixmapMask_Rectangle(
			m_screenMaskPopups,
			Popup->m_Position.X,
			Popup->m_Position.Y,
			Popup->m_pObj->m_rPosition.Width,
			Popup->m_pObj->m_rPosition.Height,
			true );
		
		ApplyMasks();
	}
	else if (pOrbiterLinux != NULL && pOrbiterLinux->IsYieldInput() && Popup->m_sName == "popup_alert")
	{
		PlutoRectangle rectTotal(0, 0, Popup->m_pObj->m_rPosition.Width, Popup->m_pObj->m_rPosition.Height);
		pOrbiterLinux->ApplyMask(rectTotal, Popup->m_Position, mtShowPopupMask);
	}
#endif
	return BASE_CLASS::HandleShowPopup(Popup, Position, EffectID);
}

bool OrbiterRenderer_Linux::HandleHidePopup(PlutoPopup* Popup)
{
#ifndef DIRECTFB
	OrbiterLinux *pOrbiterLinux = dynamic_cast<OrbiterLinux *>(OrbiterLogic());
	
	if (pOrbiterLinux != NULL && pOrbiterLinux->m_bUseMask)
	{
		m_bHasPopups = true;

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "qqq Shape_PixmapMask_Rectangle %d,%d,%d,%d transparent",
			Popup->m_Position.X,
			Popup->m_Position.Y,
			Popup->m_pObj->m_rPosition.Width,
			Popup->m_pObj->m_rPosition.Height);

		//modify mask
		pOrbiterLinux->m_pX11->Shape_PixmapMask_Rectangle(
			m_screenMaskPopups,
			Popup->m_Position.X,
			Popup->m_Position.Y,
			Popup->m_pObj->m_rPosition.Width,
			Popup->m_pObj->m_rPosition.Height,
			false );
		
		ApplyMasks();
	}
	else if (pOrbiterLinux != NULL && pOrbiterLinux->IsYieldInput() && Popup->m_sName == "popup_alert")
	{
		PlutoRectangle rectTotal(0, 0, Popup->m_pObj->m_rPosition.Width, Popup->m_pObj->m_rPosition.Height);
		pOrbiterLinux->ApplyMask(rectTotal, Popup->m_Position, mtHidePopupMask);
	}
#endif
	return BASE_CLASS::HandleHidePopup(Popup);
}

void OrbiterRenderer_Linux::ObjectRendered(DesignObj_Orbiter *pObj, PlutoPoint point)
{
	BASE_CLASS::ObjectRendered(pObj, point);

	if(point.X != 0 || point.Y != 0)
	{
		//don't handle the objects from popups
		return;
	}

	if(m_bScreenRendered)
	{
		//the screen was rendered; we might want to refresh an object, but we don't need to 
		//modify the mask; only the popups can modify the mask after the screen was rendered
		return;
	}

	OrbiterLinux *pOrbiterLinux = dynamic_cast<OrbiterLinux *>(OrbiterLogic());
#ifndef DIRECTFB
	if(pOrbiterLinux != NULL && pOrbiterLinux->m_bUseMask)
	{
		if(pObj->m_vectGraphic.size() > 0 && NULL != pObj->m_vectGraphic[0])
		{
			string sMaskPath = OrbiterLogic()->GetLocalDirectory() + pObj->m_vectGraphic[0]->m_Filename + ".mask.xbm";

			LoggerWrapper::GetInstance()->Write(LV_STATUS, "qqq Shape_PixmapMask_Copy object %s -  position %d,%d,%d,%d / popup offset %d,%d",
				pObj->m_ObjectID.c_str(), pObj->m_rPosition.X, pObj->m_rPosition.Y,
				pObj->m_rPosition.Width, pObj->m_rPosition.Height, point.X, point.Y);

			if(!pOrbiterLinux->m_pX11->Shape_PixmapMask_Copy(
				pOrbiterLinux->m_pX11->GetWmWindow(), sMaskPath, m_screenMaskObjects,
				pObj->m_rPosition.X, pObj->m_rPosition.Y))
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "qqq Shape_PixmapMask_Copy ERROR!");
			}
		}
		else
		{
			if(pObj->m_ObjectType == DESIGNOBJTYPE_Datagrid_CONST)
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "qqq Shape_PixmapMask_Rectangle %d,%d,%d,%d opaque",
					pObj->m_rPosition.X, pObj->m_rPosition.Y, pObj->m_rPosition.Width, pObj->m_rPosition.Height);
#ifndef DIRECTFB
				//modify mask
				pOrbiterLinux->m_pX11->Shape_PixmapMask_Rectangle(
					m_screenMaskObjects, pObj->m_rPosition.X, pObj->m_rPosition.Y,
					pObj->m_rPosition.Width, pObj->m_rPosition.Height, true);
#endif
			}
		}
	}
#endif
}

void OrbiterRenderer_Linux::RenderScreen( bool bRenderGraphicsOnly )
{
	m_bScreenRendered = false;

	OrbiterLinux *pOrbiterLinux = dynamic_cast<OrbiterLinux *>(OrbiterLogic());
	if(NULL != pOrbiterLinux)
	{
#ifndef DIRECTFB
		if(pOrbiterLinux->m_bUseMask)
		{
			if(m_screenMaskCurrent == None)
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "qqq Pixmap_Create %d,%d,%d,%d transparent (reset), wmwindow %p",
					0, 0, pOrbiterLinux->m_iImageWidth, pOrbiterLinux->m_iImageHeight, pOrbiterLinux->m_pX11->GetWmWindow());

				m_screenMaskObjects = pOrbiterLinux->m_pX11->Pixmap_Create(pOrbiterLinux->m_pX11->GetWmWindow(),
					pOrbiterLinux->m_iImageWidth, pOrbiterLinux->m_iImageHeight, 1);
				m_screenMaskPopups = pOrbiterLinux->m_pX11->Pixmap_Create(pOrbiterLinux->m_pX11->GetWmWindow(),
					pOrbiterLinux->m_iImageWidth, pOrbiterLinux->m_iImageHeight, 1);	
				m_screenMaskCurrent = pOrbiterLinux->m_pX11->Pixmap_Create(pOrbiterLinux->m_pX11->GetWmWindow(),
					pOrbiterLinux->m_iImageWidth, pOrbiterLinux->m_iImageHeight, 1);
			}

			m_bHasPopups = false;

			//reset masks
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "qqq Shape_PixmapMask_Rectangle %d,%d,%d,%d transparent (reset), wmwindow %p",
				0, 0, pOrbiterLinux->m_iImageWidth, pOrbiterLinux->m_iImageHeight, pOrbiterLinux->m_pX11->GetWmWindow());
			pOrbiterLinux->m_pX11->Shape_PixmapMask_Rectangle(
				m_screenMaskObjects, 0, 0, pOrbiterLinux->m_iImageWidth, pOrbiterLinux->m_iImageHeight, false);
			pOrbiterLinux->m_pX11->Shape_PixmapMask_Rectangle(
				m_screenMaskPopups, 0, 0, pOrbiterLinux->m_iImageWidth, pOrbiterLinux->m_iImageHeight, false);
		}
#endif
		if( bRenderGraphicsOnly )
		{
			BASE_CLASS::RenderScreen( bRenderGraphicsOnly );
#ifndef DIRECTFB	
                        if(pOrbiterLinux->m_bUseMask)
                                ApplyMasks();
#endif
	                m_bScreenRendered = true;
			return;
		}		

#ifndef DIRECTFB		
		pOrbiterLinux->StopActivateExternalWindowTask();
		pOrbiterLinux->m_pWinListManager->HideAllWindows();
		pOrbiterLinux->m_bIsExclusiveMode = true; // This is set to false if there's an application desktop

		{
			if(NULL == pOrbiterLinux->GetDisplay())
			{
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "The display is not available");
				return;
			}

			X11_Locker lock(pOrbiterLinux->GetDisplay());
#endif
			BASE_CLASS::RenderScreen(bRenderGraphicsOnly);
#ifndef DIRECTFB
	                if(pOrbiterLinux->m_bUseMask)
        	                ApplyMasks();
		}
		if(pOrbiterLinux->m_bOrbiterReady)
			pOrbiterLinux->m_pWinListManager->ShowSdlWindow(pOrbiterLinux->m_bIsExclusiveMode, pOrbiterLinux->m_bYieldScreen);
		m_bScreenRendered = true;
		pOrbiterLinux->m_pWinListManager->ApplyContext();
#endif
	}
}

void OrbiterRenderer_Linux::ApplyMasks()
{
	OrbiterLinux *pOrbiterLinux = dynamic_cast<OrbiterLinux *>(OrbiterLogic());
	if(NULL != pOrbiterLinux)
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "qqq Shape_Window_Apply!");
#ifndef DIRECTFB
		if(m_bHasPopups)
		{
			//mirror mask objects with mask current
			pOrbiterLinux->m_pX11->Shape_PixmapMask_Copy(m_screenMaskObjects, m_screenMaskCurrent, 0, 0, 
				pOrbiterLinux->m_iImageWidth, pOrbiterLinux->m_iImageHeight, 0, 0);
			//merge mask popup with mask current
			pOrbiterLinux->m_pX11->Shape_PixmapMask_Copy(m_screenMaskPopups, m_screenMaskCurrent, 0, 0, 
				pOrbiterLinux->m_iImageWidth, pOrbiterLinux->m_iImageHeight, 0, 0, GXor);

			if( !pOrbiterLinux->m_pX11->Shape_Window_Apply(pOrbiterLinux->m_pX11->GetWmWindow(), m_screenMaskCurrent))
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "qqq Shape_Window_Apply ERROR!");
			}
		}
		else
		{
			if( !pOrbiterLinux->m_pX11->Shape_Window_Apply(pOrbiterLinux->m_pX11->GetWmWindow(), m_screenMaskObjects) )
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "qqq Shape_Window_Apply ERROR!");
			}
		}
#endif
	}

}

void OrbiterRenderer_Linux::InitializeAfterSetVideoMode()
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterLinux::InitializeAfterSetVideoMode() START");

	OrbiterLinux *pOrbiterLinux = dynamic_cast<OrbiterLinux *>(OrbiterLogic());
	if(NULL != pOrbiterLinux)
	{
#ifndef DIRECTFB
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterLinux::InitializeAfterSetVideoMode()");
		pOrbiterLinux->X11_Init();
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterLinux::InitializeAfterSetVideoMode() : HideOtherWindows");
		pOrbiterLinux->HideOtherWindows();
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterLinux::InitializeAfterSetVideoMode() : done");
#endif
	}
}

void OrbiterRenderer_Linux::InitializeAfterRelatives()
{
	OrbiterLinux *pOrbiterLinux = dynamic_cast<OrbiterLinux *>(OrbiterLogic());
	if(NULL != pOrbiterLinux)
	{
#ifndef DIRECTFB
		// allow initial "extern" dialogs to receive clicks until this point
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterLinux::InitializeAfterRelatives()");
		pOrbiterLinux->GrabPointer(true);
		pOrbiterLinux->GrabKeyboard(true);
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterLinux::InitializeAfterRelatives() : done");
#endif
	}
}

bool OrbiterRenderer_Linux::DisplayProgress(string sMessage, const map<string, bool> &mapChildDevices, int nProgress)
{
    std::cout << "== DisplayProgress( " << sMessage << ", ChildDevices, " << nProgress << " );" << std::endl;
#ifndef DIRECTFB
	OrbiterLinux *pOrbiterLinux = dynamic_cast<OrbiterLinux *>(OrbiterLogic());
	if(NULL ==	pOrbiterLinux)
		return false;

    sMessage += ": ";
    for(map<string, bool>::const_iterator it = mapChildDevices.begin(); it != mapChildDevices.end(); ++it)
        if(!it->second)
            sMessage += StringUtils::Replace(it->first, "|", "# ") + ", ";
    std::cout << "== DisplayProgress( " << sMessage << ", ChildDevices, " << nProgress << " );" << std::endl;

    DisplayProgress(sMessage, nProgress);

    return false;
#else
    return true;
#endif
}

bool OrbiterRenderer_Linux::DisplayProgress(string sMessage, int nProgress)
{
#ifndef DIRECTFB
    std::cout << "== DisplayProgress waitlist( " << sMessage << ", " << nProgress << " );" << std::endl;

	OrbiterLinux *pOrbiterLinux = dynamic_cast<OrbiterLinux *>(OrbiterLogic());
	if(NULL ==	pOrbiterLinux)
		return false;

#ifdef ORBITER_OPENGL
	TextureManager::Instance()->SafeToReleaseTextures(true);

	Popups->Reset();
	Engine->NewScreen("Progress");
	TextureManager::Instance()->EmptyCache();

	TextureManager::Instance()->SafeToReleaseTextures(false);

	m_pLastHighlightedObject = NULL;
	Engine->BeginModifyGeometry();
#endif /* ORBITER_OPENGL */
	RenderProgressBar(nProgress);

	BatchedTextRendering(false);

	DesignObjText text;
	text.m_rPosition = PlutoRectangle(TEXT_TOP_X, TEXT_TOP_Y, OrbiterLogic()->m_iImageWidth - TEXT_TOP_X, OrbiterLogic()->m_iImageHeight - PROGRESS_BOTTOM_DISTANCE);

#ifndef ARMV4I	
	RenderText(sMessage, &text, m_spTextStyle.get());
#endif

	BatchedTextRendering(true);

#ifdef ORBITER_OPENGL
	Engine->EndModifyGeometry();
#endif /* ORBITER_OPENGL */
	UpdateScreen();

    return false;
#else
    return true;
#endif
}

int OrbiterRenderer_Linux::PromptUser(string sPrompt, int iTimeoutSeconds, map<int,string> *p_mapPrompts)
{
#ifndef DIRECTFB
    map<int,string> mapPrompts;
    mapPrompts[PROMPT_CANCEL]    = "Ok";
    if (p_mapPrompts == NULL) {
        p_mapPrompts = &mapPrompts;
    }
    std::cout << "== PromptUser( " << sPrompt << ", " << iTimeoutSeconds << ", " << p_mapPrompts << " );" << std::endl;

	OrbiterLinux *pOrbiterLinux = dynamic_cast<OrbiterLinux *>(OrbiterLogic());
	if(NULL ==	pOrbiterLinux)
		return false;

	// TODO: replace these with SDL code
	pOrbiterLinux->m_pWinListManager->SetSdlWindowVisibility(false);
#ifdef USE_GTK
    GTKPromptUser promptDlg(sPrompt, iTimeoutSeconds, p_mapPrompts);
#else
    XPromptUser promptDlg(sPrompt, iTimeoutSeconds, p_mapPrompts);
    promptDlg.SetButtonPlacement(XPromptUser::BTN_VERT);
#endif
    promptDlg.Init();
	pOrbiterLinux->m_pWinListManager->PositionWindow(promptDlg.m_wndName, 0, 0, pOrbiterLinux->m_iImageWidth, pOrbiterLinux->m_iImageHeight);
	pOrbiterLinux->m_pWinListManager->ApplyContext();
    int nUserAnswer = promptDlg.RunModal();
    promptDlg.DeInit();
	pOrbiterLinux->m_pWinListManager->SetSdlWindowVisibility(true);
	pOrbiterLinux->m_pWinListManager->ApplyContext();
    return nUserAnswer;
#else
    return 0;
#endif
}

void OrbiterRenderer_Linux::LockDisplay()
{
#ifndef DIRECTFB
	OrbiterLinux *pOrbiterLinux = dynamic_cast<OrbiterLinux *>(OrbiterLogic());
    if (pOrbiterLinux)
        pOrbiterLinux->X_LockDisplay();
#endif
}

void OrbiterRenderer_Linux::UnlockDisplay()
{
#ifndef DIRECTFB
	OrbiterLinux *pOrbiterLinux = dynamic_cast<OrbiterLinux *>(OrbiterLogic());
    if (pOrbiterLinux)
        pOrbiterLinux->X_UnlockDisplay();
#endif
}

void OrbiterRenderer_Linux::EventLoop()
{
#if (defined(USE_SDL_PATCH) || !defined(ORBITER_OPENGL)) && !defined(VIA_OVERLAY)

	int SDL_Event_Pending = 0;

	SDL_Event Event;
	SDL_EventState(SDL_VIDEOEXPOSE, SDL_ENABLE);
	SDL_EventState(SDL_VIDEORESIZE, SDL_ENABLE);

	// For now I'll assume that shift + arrows scrolls a grid
	while (!OrbiterLogic()->m_bQuit_get()&& !OrbiterLogic()->m_bReload)
	{
		LockDisplay();
		SDL_Event_Pending = SDL_PollEvent(&Event);
		UnlockDisplay();

		if (SDL_Event_Pending)
		{
			Orbiter::Event orbiterEvent;
			orbiterEvent.type = Orbiter::Event::NOT_PROCESSED;

			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Event type %d", Event.type);

			if (Event.type == SDL_QUIT)
			{
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "Received sdl event SDL_QUIT");
				break;
			}
#ifdef ORBITER_OPENGL
			else if(Event.type == SDL_VIDEOEXPOSE && Event.type == SDL_VIDEORESIZE)
			{
				Engine->RefreshScreen();
			}
#endif //ORBITER_OPENGL

#ifdef MAEMO_NOKIA770
			else if(Event.type == SDL_MOUSEMOTION)
			{
				if (m_bRelativeMode)
				{
					orbiterEvent.type = Orbiter::Event::MOUSE_RELATIVE_MOVE;
					orbiterEvent.data.region.m_iX = Event.motion.xrel;
					orbiterEvent.data.region.m_iY = Event.motion.yrel;
				}
				else
				{
					orbiterEvent.type = Orbiter::Event::MOUSE_MOVE;
					orbiterEvent.data.region.m_iX = Event.button.x;
					orbiterEvent.data.region.m_iY = Event.button.y;
				}
				orbiterEvent.data.region.m_iButton = Event.button.button;
				OrbiterLogic()->ProcessEvent(orbiterEvent);
			} 
			else if (Event.type == SDL_MOUSEBUTTONDOWN)
			{
				orbiterEvent.type = Orbiter::Event::REGION_DOWN;
				orbiterEvent.data.region.m_iX = Event.button.x;
				orbiterEvent.data.region.m_iY = Event.button.y;
				orbiterEvent.data.region.m_iButton = Event.button.button;
				OrbiterLogic()->ProcessEvent(orbiterEvent);

#if defined(WIN32) && !defined(PROXY_ORBITER) && !defined(BLUETOOTH_DONGLE)
				RecordMouseAction(Event.button.x, Event.button.y);
#endif
			}
			else if (Event.type == SDL_MOUSEBUTTONUP)
			{
				orbiterEvent.type = Orbiter::Event::REGION_UP;
				orbiterEvent.data.region.m_iX = Event.button.x;
				orbiterEvent.data.region.m_iY = Event.button.y;
				orbiterEvent.data.region.m_iButton = Event.button.button;
				OrbiterLogic()->ProcessEvent(orbiterEvent);
			}
#endif

#ifndef MAEMO_NOKIA770			
			else if(Event.type == SDL_ACTIVEEVENT)
                        {
                            /* See what happened */
                            string sAppState = string("Orbiter app ") + (Event.active.gain ? "gained" : "lost");
                            if ( Event.active.state & SDL_APPACTIVE )
                            {
                                LoggerWrapper::GetInstance()->Write(LV_STATUS, "%s active state", sAppState.c_str() );
                                if(Event.active.gain)
                                        OrbiterLogic()->CMD_Activate_PC_Desktop(0);
                            }
                        }
#endif			
		}
		else
		{
			OnIdle();
		}
	}  // while

#else
#ifndef DIRECTFB
	Display *dpy = CompositeHelper::GetInstance().GetDisplay();
    if (!dpy) 
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Unable to open display!");
		return;
    }

	XSetWindowAttributes attr;
    XWindowAttributes wattr;

    /* select for all events */
    attr.event_mask = KeyPressMask | KeyReleaseMask | ButtonPressMask |
			   ButtonReleaseMask | EnterWindowMask |
			   LeaveWindowMask | PointerMotionMask | 
			   Button1MotionMask |
			   Button2MotionMask | Button3MotionMask |
			   Button4MotionMask | Button5MotionMask |
			   ButtonMotionMask | KeymapStateMask |
			   ExposureMask | VisibilityChangeMask | 
			   StructureNotifyMask | /* ResizeRedirectMask | */
			   SubstructureNotifyMask | SubstructureRedirectMask |
			   FocusChangeMask | PropertyChangeMask |
			   ColormapChangeMask | OwnerGrabButtonMask;

	OrbiterLinux *pOrbiterLinux = dynamic_cast<OrbiterLinux *>(OrbiterLogic());
	if(NULL != pOrbiterLinux->m_pX11)
	{
		XGetWindowAttributes(dpy, pOrbiterLinux->m_pX11->GetMainWindow(), &wattr);
		if (wattr.all_event_masks & ButtonPressMask)
			attr.event_mask &= ~ButtonPressMask;
		attr.event_mask &= ~SubstructureRedirectMask;
		XSelectInput(dpy, pOrbiterLinux->m_pX11->GetMainWindow(), attr.event_mask);
	}
	
	XEvent report;   // event union reference
	SDL_Event Event;
	int SDL_Event_Pending = 0;

	while (!OrbiterLogic()->m_bQuit_get()&& !OrbiterLogic()->m_bReload)
	{	
		SDL_Event_Pending = SDL_PollEvent(&Event);
		if (SDL_Event_Pending && Event.type == SDL_QUIT)
		{
			//it's all over now...
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Received sdl event SDL_QUIT");
			break;
		}

		if(!XCheckMaskEvent(dpy, attr.event_mask, &report))
		{
			//no event in the queue
			OnIdle();
			continue;
		}

		// event loop
		switch (report.type)
		{
			case EnterNotify: 
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "EnterNotify!");
				OrbiterLogic()->CMD_Activate_PC_Desktop(0);
				break;

			case LeaveNotify: 
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "LeaveNotify!");
				break;

			case VisibilityNotify:
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "VisibilityNotify!");
				break;

			case FocusIn:
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "FocusIn!");
				break;

			case FocusOut:
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "FocusOut!");
				break;

			default:
				//LoggerWrapper::GetInstance()->Write(LV_WARNING, "got an event %d -- ignore it!", report.type);
			break;
		}            
	}       
#endif
#endif //USE_SDL_PATCH
}

#ifdef MAEMO_NOKIA770
void translateSDLEventToOrbiterEvent(SDL_Event &sdlEvent, Orbiter::Event *orbiterEvent, struct keyboardState *kbdState, bool bUsingUIVersion2)
{
    orbiterEvent->type = Orbiter::Event::NOT_PROCESSED;

    switch ( sdlEvent.type )
    {
        case SDL_QUIT: orbiterEvent->type = Orbiter::Event::QUIT;	break;

        case SDL_KEYDOWN:
        case SDL_KEYUP:

            orbiterEvent->type = SDL_KEYDOWN != sdlEvent.type ? Orbiter::Event::BUTTON_UP :
                Orbiter::Event::BUTTON_DOWN;

			orbiterEvent->data.button.m_bSimulated = false;

            switch (sdlEvent.key.keysym.sym)
            {
                case SDLK_LSHIFT:   case SDLK_RSHIFT:       kbdState->bShiftDown = SDL_KEYDOWN == sdlEvent.type;      break;
                case SDLK_LCTRL:    case SDLK_RCTRL:		kbdState->bControlDown = SDL_KEYDOWN == sdlEvent.type;				break;
                case SDLK_LALT:     case SDLK_RALT:			kbdState->bAltDown = SDL_KEYDOWN == sdlEvent.type;					break;
                default:
                    break; //nothing here
            }

            if(SDL_KEYDOWN == sdlEvent.type && SDLK_CAPSLOCK == sdlEvent.key.keysym.sym)
                kbdState->bCapsLock = !kbdState->bCapsLock;

#ifdef DEBUG
            LoggerWrapper::GetInstance()->Write(LV_STATUS, "key %s %d shif: %d ctrl: %d alt: %d caps: %d",
                                  SDL_KEYDOWN == sdlEvent.type ? "down" : "up",
                                  (int) sdlEvent.key.keysym.sym,
                                  (int) kbdState->bShiftDown,
                                  (int) kbdState->bControlDown,
                                  (int) kbdState->bAltDown,
                                  (int) kbdState->bCapsLock);
#endif

#ifndef PHONEKEYS
            if ( SDLK_a <= sdlEvent.key.keysym.sym && sdlEvent.key.keysym.sym <= SDLK_z )
            {
                if( ( ! kbdState->bCapsLock && ! kbdState->bShiftDown ) ||
                    (   kbdState->bCapsLock &&   kbdState->bShiftDown ) )
                    orbiterEvent->data.button.m_iPK_Button = BUTTON_a_CONST + sdlEvent.key.keysym.sym - SDLK_a;
                else
                    orbiterEvent->data.button.m_iPK_Button = BUTTON_A_CONST + sdlEvent.key.keysym.sym - SDLK_a;
            }
            else
#endif
/*
  if( sdlEvent.key.keysym.sym==SDLK_LSHIFT || sdlEvent.key.keysym.sym==SDLK_RSHIFT )
  {
  orbiterEvent->type = Orbiter::Event::NOT_PROCESSED;
  kbdState->bShiftDown=false;
  }
  else if( sdlEvent.key.keysym.sym==SDLK_LCTRL || sdlEvent.key.keysym.sym==SDLK_RCTRL )
  {
  orbiterEvent->type = Orbiter::Event::NOT_PROCESSED;
  kbdState->bControlDown=false;
  }
  else if( sdlEvent.key.keysym.sym==SDLK_LALT || sdlEvent.key.keysym.sym==SDLK_RALT )
  {
  orbiterEvent->type = Orbiter::Event::NOT_PROCESSED;
  kbdState->bAltDown=false;
  }

  else*/

                if( ! kbdState->bShiftDown && ! kbdState->bControlDown && ! kbdState->bAltDown )
                {
                    // No Modifiers were down
                    switch (sdlEvent.key.keysym.sym )
                    {
                        case SDLK_0: case SDLK_KP0: 	orbiterEvent->data.button.m_iPK_Button = BUTTON_0_CONST; break;
                        case SDLK_1: case SDLK_KP1: 	orbiterEvent->data.button.m_iPK_Button = BUTTON_1_CONST; break;
                        case SDLK_2: case SDLK_KP2: 	orbiterEvent->data.button.m_iPK_Button = BUTTON_2_CONST; break;
                        case SDLK_3: case SDLK_KP3: 	orbiterEvent->data.button.m_iPK_Button = BUTTON_3_CONST; break;
                        case SDLK_4: case SDLK_KP4: 	orbiterEvent->data.button.m_iPK_Button = BUTTON_4_CONST; break;
                        case SDLK_5: case SDLK_KP5: 	orbiterEvent->data.button.m_iPK_Button = BUTTON_5_CONST; break;
                        case SDLK_6: case SDLK_KP6: 	orbiterEvent->data.button.m_iPK_Button = BUTTON_6_CONST; break;
                        case SDLK_7: case SDLK_KP7: 	orbiterEvent->data.button.m_iPK_Button = BUTTON_7_CONST; break;
                        case SDLK_8: case SDLK_KP8: 	orbiterEvent->data.button.m_iPK_Button = BUTTON_8_CONST; break;
                        case SDLK_9: case SDLK_KP9: 	orbiterEvent->data.button.m_iPK_Button = BUTTON_9_CONST; break;
#ifdef PHONEKEYS
                        case SDLK_c:		orbiterEvent->data.button.m_iPK_Button = BUTTON_Phone_C_CONST; break;
                        case SDLK_p:		orbiterEvent->data.button.m_iPK_Button = BUTTON_Phone_Pencil_CONST; break;
                        case SDLK_t:		orbiterEvent->data.button.m_iPK_Button = BUTTON_Phone_Talk_CONST; break;
                        case SDLK_e:		orbiterEvent->data.button.m_iPK_Button = BUTTON_Phone_End_CONST; break;
                        case SDLK_l:		orbiterEvent->data.button.m_iPK_Button = BUTTON_Phone_Soft_left_CONST; break;
                        case SDLK_r:		orbiterEvent->data.button.m_iPK_Button = BUTTON_Phone_Soft_right_CONST; break;
                        case SDLK_ASTERISK: orbiterEvent->data.button.m_iPK_Button = BUTTON_Asterisk_CONST; break;
                        case SDLK_HASH:     orbiterEvent->data.button.m_iPK_Button = BUTTON_Pound_CONST; break;
#endif
                        case SDLK_F1:		orbiterEvent->data.button.m_iPK_Button = BUTTON_F1_CONST; break;
                        case SDLK_F2:		orbiterEvent->data.button.m_iPK_Button = BUTTON_F2_CONST; break;
                        case SDLK_F3:		orbiterEvent->data.button.m_iPK_Button = BUTTON_F3_CONST; break;
                        case SDLK_F4:		orbiterEvent->data.button.m_iPK_Button = BUTTON_F4_CONST; break;
                        case SDLK_F5:		orbiterEvent->data.button.m_iPK_Button = BUTTON_F5_CONST; break;
                        case SDLK_F6:		orbiterEvent->data.button.m_iPK_Button = BUTTON_F6_CONST; break;
                        case SDLK_F7:		orbiterEvent->data.button.m_iPK_Button = BUTTON_F7_CONST; break;
                        case SDLK_F8:		orbiterEvent->data.button.m_iPK_Button = BUTTON_F8_CONST; break;
                        case SDLK_F9:		orbiterEvent->data.button.m_iPK_Button = BUTTON_F9_CONST; break;
                        case SDLK_F10:		orbiterEvent->data.button.m_iPK_Button = BUTTON_F10_CONST; break;

                        case SDLK_F12:
                        	{
                                // TODO: remove when NewUI will be ready
                                // wx tests for new dialogs
                                break;
                            }

                        case SDLK_UP:		orbiterEvent->data.button.m_iPK_Button = BUTTON_Up_Arrow_CONST; break;
                        case SDLK_DOWN:		orbiterEvent->data.button.m_iPK_Button = BUTTON_Down_Arrow_CONST; break;
                        case SDLK_LEFT:		orbiterEvent->data.button.m_iPK_Button = BUTTON_Left_Arrow_CONST; break;

                        case SDLK_KP_ENTER: case SDLK_RETURN: 	orbiterEvent->data.button.m_iPK_Button = BUTTON_Enter_CONST;  break;
                        case SDLK_SPACE:		orbiterEvent->data.button.m_iPK_Button = BUTTON_space_CONST; break;

                        case SDLK_BACKSPACE:	orbiterEvent->data.button.m_iPK_Button = BUTTON_Back_CONST;  break;

                        case SDLK_UNDERSCORE:   orbiterEvent->data.button.m_iPK_Button = BUTTON_underscore_CONST;  break;
                        case SDLK_MINUS:	case SDLK_KP_MINUS:   orbiterEvent->data.button.m_iPK_Button = BUTTON_dash_CONST;  break;
                        case SDLK_PLUS:		case SDLK_KP_PLUS:		orbiterEvent->data.button.m_iPK_Button = BUTTON_plus_CONST;  break;
                        case SDLK_EQUALS:	case SDLK_KP_EQUALS:   orbiterEvent->data.button.m_iPK_Button = BUTTON_equals_sign_CONST;  break;

                        case SDLK_PERIOD:	case SDLK_KP_PERIOD:	orbiterEvent->data.button.m_iPK_Button = BUTTON_dot_CONST;  break;
                        case SDLK_COMMA:	orbiterEvent->data.button.m_iPK_Button = BUTTON_comma_CONST; break;
                        case SDLK_COLON:	 orbiterEvent->data.button.m_iPK_Button = BUTTON_colon_CONST; break;
                        case SDLK_SEMICOLON:	orbiterEvent->data.button.m_iPK_Button = BUTTON_semicolumn_CONST; break;
                        case SDLK_QUOTE:     orbiterEvent->data.button.m_iPK_Button = BUTTON_single_quote_CONST; break;
                        case SDLK_QUOTEDBL:    orbiterEvent->data.button.m_iPK_Button = BUTTON_double_quote_CONST; break;
			default:
                            orbiterEvent->type = Orbiter::Event::NOT_PROCESSED;
                            LoggerWrapper::GetInstance()->Write(LV_STATUS, "Unknown key: %d", (int) sdlEvent.key.keysym.sym);
                    };
                } // else if( !bShiftDown && !bControlDown && !bAltDown && !bRepeat )
                else if ( kbdState->bShiftDown && ! kbdState->bControlDown && ! kbdState->bAltDown )
                {
                    // The Shift key was pressed
                    switch (sdlEvent.key.keysym.sym)
                    {
                        case SDLK_UP:		orbiterEvent->data.button.m_iPK_Button = BUTTON_Shift_Up_Arrow_CONST; break;
                        case SDLK_DOWN:		orbiterEvent->data.button.m_iPK_Button = BUTTON_Shift_Down_Arrow_CONST; break;
                        case SDLK_LEFT:		orbiterEvent->data.button.m_iPK_Button = BUTTON_Shift_Left_Arrow_CONST; break;
                        case SDLK_RIGHT:	orbiterEvent->data.button.m_iPK_Button = BUTTON_Shift_Right_Arrow_CONST; break;

                        case SDLK_MINUS: case SDLK_KP_MINUS:  orbiterEvent->data.button.m_iPK_Button = BUTTON_underscore_CONST;  break;
                        case SDLK_EQUALS:     case SDLK_KP_EQUALS:   orbiterEvent->data.button.m_iPK_Button = BUTTON_plus_CONST;  break;

                        case SDLK_0: case SDLK_KP0:   orbiterEvent->data.button.m_iPK_Button = BUTTON_right_parenthesis_CONST; break;
                        case SDLK_1: case SDLK_KP1:   orbiterEvent->data.button.m_iPK_Button = BUTTON_exclamation_point_CONST; break;
                        case SDLK_2: case SDLK_KP2:   orbiterEvent->data.button.m_iPK_Button = BUTTON_at_sign_CONST; break;
                        case SDLK_3: case SDLK_KP3:   orbiterEvent->data.button.m_iPK_Button = BUTTON_Pound_CONST; break;
                        case SDLK_4: case SDLK_KP4:   orbiterEvent->data.button.m_iPK_Button = BUTTON_dollar_CONST; break;
                        case SDLK_5: case SDLK_KP5:   orbiterEvent->data.button.m_iPK_Button = BUTTON_percent_CONST; break;
                        case SDLK_6: case SDLK_KP6:   orbiterEvent->data.button.m_iPK_Button = BUTTON_caret_CONST; break;
                        case SDLK_7: case SDLK_KP7:   orbiterEvent->data.button.m_iPK_Button = BUTTON_ampersand_CONST; break;
                        case SDLK_8: case SDLK_KP8:   orbiterEvent->data.button.m_iPK_Button = BUTTON_Asterisk_CONST; break;
                        case SDLK_9: case SDLK_KP9:   orbiterEvent->data.button.m_iPK_Button = BUTTON_left_parenthesis_CONST; break;

                        case SDLK_SEMICOLON:    orbiterEvent->data.button.m_iPK_Button = BUTTON_colon_CONST; break;
                        case SDLK_QUOTE:     orbiterEvent->data.button.m_iPK_Button = BUTTON_double_quote_CONST; break;

                        default:
                            orbiterEvent->type = Orbiter::Event::NOT_PROCESSED;
                    };
                }
            break;

        case SDL_MOUSEBUTTONDOWN:
            LoggerWrapper::GetInstance()->Write(LV_STATUS, "SDL_MOUSEBUTTONDOWN(b=%d, x=%d, y=%d)", sdlEvent.button.button, sdlEvent.button.x, sdlEvent.button.y);
#ifdef WIN32
            RecordMouseAction(sdlEvent.button.x, sdlEvent.button.y);
#endif
        case SDL_MOUSEBUTTONUP:
            LoggerWrapper::GetInstance()->Write(LV_STATUS, "SDL_MOUSEBUTTONUP(b=%d, x=%d, y=%d)", sdlEvent.button.button, sdlEvent.button.x, sdlEvent.button.y);

#ifdef AUDIDEMO
            orbiterEvent->type = (sdlEvent.type == SDL_MOUSEBUTTONDOWN) ? Orbiter::Event::BUTTON_DOWN : Orbiter::Event::BUTTON_UP;
            switch ( sdlEvent.button.button )
            {
                case 1:	orbiterEvent->data.button.m_iPK_Button = BUTTON_4_CONST; 			break;
                case 2: orbiterEvent->data.button.m_iPK_Button = BUTTON_Enter_CONST; 		break;
                case 3:	orbiterEvent->data.button.m_iPK_Button = BUTTON_5_CONST; 			break;
                case 4:	orbiterEvent->data.button.m_iPK_Button = BUTTON_Up_Arrow_CONST; 	break;
                case 5:	orbiterEvent->data.button.m_iPK_Button = BUTTON_Down_Arrow_CONST; 	break;
                case 6:	orbiterEvent->data.button.m_iPK_Button = BUTTON_2_CONST; 			break;
                case 7:	orbiterEvent->data.button.m_iPK_Button = BUTTON_1_CONST; 			break;

                default:
                    orbiterEvent->type = Orbiter::Event::NOT_PROCESSED;
            }
#else
            orbiterEvent->type = (sdlEvent.type == SDL_MOUSEBUTTONDOWN) ? Orbiter::Event::REGION_DOWN : Orbiter::Event::REGION_UP;
            orbiterEvent->data.region.m_iButton = sdlEvent.button.button;
            orbiterEvent->data.region.m_iX = sdlEvent.button.x;
            orbiterEvent->data.region.m_iY = sdlEvent.button.y;
#endif
            break;

        case SDL_MOUSEMOTION: // not handled
            //LoggerWrapper::GetInstance()->Write(LV_WARNING, "SDL_MOUSEMOTION(b=%d, x=%d, y=%d)", sdlEvent.button.button, sdlEvent.button.x, sdlEvent.button.y);
            orbiterEvent->type = Orbiter::Event::MOUSE_MOVE;
            orbiterEvent->data.region.m_iX = sdlEvent.button.x;
            orbiterEvent->data.region.m_iY = sdlEvent.button.y;
			break;

        default:
            orbiterEvent->type = Orbiter::Event::NOT_PROCESSED;
            break;
    }
}

#endif
