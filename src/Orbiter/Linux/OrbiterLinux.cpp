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
  OrbiterLinux

  Copyright (C) 2004 Pluto, Inc., a Florida Corporation

  www.plutohome.com

  Phone: +1 (877) 758-8648

  This program is distributed according to the terms of the Pluto Public License, available at:
  http://plutohome.com/index.php?section=public_license

  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

*/

#include <stdlib.h>

#include "DCE/Logger.h"
#include "OrbiterLinux.h"
#include "pluto_main/Define_DesignObj.h"

#include <iomanip>
#include <sstream>
#include <sys/time.h>

#ifndef DIRECTFB
#include <X11/Xutil.h>
#include <X11/Xproto.h>
#include <X11/keysymdef.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>
#include <X11/extensions/shape.h>
#endif

#include <SDL/SDL_syswm.h>

#include "pluto_main/Define_Button.h"
#include "pluto_main/Define_Screen.h"
#ifndef DIRECTFB
#include "PlutoUtils/PlutoButtonsToX.h"
#endif
#include "../ScreenHistory.h"

#if !defined(DIRECTFB) && !defined(PADORBITER)
#include "OSDScreenHandler.h"
#endif

#if !defined(PADORBITER) 
#include "MouseBehavior_Linux.h"
#endif

#include "pluto_main/Define_DesignObjParameter.h"

#if !defined(DIRECTFB) && defined(VIA_OVERLAY) && defined(ORBITER_OPENGL)
#include "OpenGL/VIA/ViaOverlay.h"
#endif

#include "../CallBackTypes.h"

#include "../dialog_types.h"

using namespace std;

OrbiterLinux::OrbiterLinux(int DeviceID, int PK_DeviceTemplate,
                           string ServerAddress, string sLocalDirectory,
                           bool bLocalMode,
                           int nImageWidth, int nImageHeight,
						   bool bFullScreen)

        : Orbiter(DeviceID, PK_DeviceTemplate, ServerAddress, sLocalDirectory, bLocalMode,
                     nImageWidth, nImageHeight, NULL)
        , m_strWindowName("Orbiter")
#if !defined(MAEMO_NOKIA770) && !defined(DIRECTFB)		
        , m_pRecordHandler(NULL)
#endif	
        , m_nDesktopWidth(0)
        , m_nDesktopHeight(0)
        , m_nProgressWidth(0)
        , m_nProgressHeight(0)
#ifndef DIRECTFB
        , m_pWaitGrid(NULL)
        , m_bButtonPressed_WaitGrid(false)
        , m_pWaitList(NULL)
        , m_bButtonPressed_WaitList(false)
        , m_pWaitUser(NULL)
#endif
	, m_bMaskApplied(false)
        , m_bOrbiterReady(false)
        , m_bIsExclusiveMode(false)
#ifndef DIRECTFB
	, m_pDisplay_SDL(NULL)
        , m_pWinListManager(NULL)
        , m_pX11(NULL)
#endif
{
#ifndef DIRECTFB
    const char *pDisplayName = getenv("DISPLAY");
    if(NULL != pDisplayName)
        m_strDisplayName = pDisplayName;
    else
    {
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "getenv returned NULL for variable DISPLAY! Assuming it's :0");
		m_strDisplayName = ":0";
    }
	
    m_nProgressWidth = 400;
    m_nProgressHeight = 200;
    m_pWinListManager = new WinListManager(m_strWindowName);
#endif
}

void *HackThread(void *p)
{
    // For some reason X can fail to properly die????  TODO - HACK
    cout << "Inside Hackthread" << endl;
    Sleep(30000);
    cout << "Big problem -- this app should have died and didn't.  Send ourselves a seg fault so we log a coredump and can fix this" << endl;
    kill(getpid(), SIGSEGV);
    return NULL;
}

OrbiterLinux::~OrbiterLinux()
{
    pthread_t hackthread;
    pthread_create(&hackthread, NULL, HackThread, (void*)this);
    pthread_detach(hackthread);

    KillMaintThread();

    Destroy();
#ifndef DIRECTFB
    //re-activate repeated keys
    system("/usr/bin/X11/xset r");  
#endif
}

void OrbiterLinux::HideOtherWindows()
{
#ifndef DIRECTFB
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterLinux::HideOtherWindows: Hidding other windows...");

	list<WinInfo> listWinInfo;
	m_pWinListManager->GetWindows(listWinInfo);

	for(list<WinInfo>::iterator it = listWinInfo.begin(); it != listWinInfo.end(); ++it)
	{
		string sClassName = it->sClassName;
        // skip the pointer-constrain window
        if (
            (sClassName == "pointer_constrain_name") ||
            (sClassName == "pointer_constrain_class") ||
            (sClassName == "pointer_constrain_name.pointer_constrain_class")
            )
        {
            continue;
        }
		if(sClassName != "" && string::npos == sClassName.find("Orbiter"))
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterLinux::HideOtherWindows, hidding %s", sClassName.c_str());
			m_pWinListManager->HideWindow(sClassName);
		}
	}
#endif
}

void OrbiterLinux::HideWindow(string sWindowName)
{
#ifndef DIRECTFB
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterLinux::HideWindow %s", sWindowName.c_str());
	m_pWinListManager->HideWindow(sWindowName);
#endif
}

void OrbiterLinux::reinitGraphics()
{
#ifndef DIRECTFB
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterLinux::reinitGraphics()");
    m_pWinListManager->ShowSdlWindow(m_bIsExclusiveMode, m_bYieldInput);
#endif
}

void OrbiterLinux::setWindowName(string strWindowName)
{
    m_strWindowName = strWindowName;
}

void OrbiterLinux::setDisplayName(string strDisplayName)
{
    m_strDisplayName = strDisplayName;
}

#ifndef DIRECTFB
bool OrbiterLinux::X11_Init()
{
    if ((m_pX11 != NULL) && m_pX11->IsInitialized())
    {
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "OrbiterLinux::X11_Init(): already initialized");
        return true;
    }
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterLinux::X11_Init()");

	// we need to use the same Display and Window with SDL
    SDL_SysWMinfo info;
    SDL_VERSION(&info.version); // this is important!
    bool bResult = SDL_GetWMInfo(&info);
    if (bResult == false)
    {
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "OrbiterLinux::X11_Init() : error in SDL_GetWMInfo()");
    }
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "SDL uses pDisplay=%p, Window=%d", info.info.x11.display, info.info.x11.window);
    // save the SDL display
    m_pDisplay_SDL = info.info.x11.display;
    // initialize the X11wrapper
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterLinux::X11_Init() : X11wrapper");
    m_pX11 = new X11wrapper();
    m_pX11->Assign_MainWindow(info.info.x11.window);
    m_pX11->Assign_WmWindow(info.info.x11.wmwindow);
    m_pX11->Display_Open();
    m_pX11->GetDisplaySize(m_nDesktopWidth, m_nDesktopHeight);
    // initialize other classes
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterLinux::X11_Init() : done");
    return true;
}
bool OrbiterLinux::X11_Exit()
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterLinux::X11_Exit()");
	
	if (m_pX11 != NULL)
    {
		// display was opened by SDL, it should be closed by SDL
		delete m_pX11;
		m_pX11 = NULL;
		m_pDisplay_SDL = NULL;
    }

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterLinux::X11_Exit() : done");

    return true;
}

Display * OrbiterLinux::GetDisplay()
{
    //m_pX11 should be created by now
	if(NULL == m_pX11)
		return NULL;

    return m_pX11->GetDisplay();
}

Window OrbiterLinux::GetMainWindow()
{
	if(NULL == m_pX11)
		return 0;

    //m_pX11 should be created by now
    return m_pX11->GetMainWindow();
}

Display * OrbiterLinux::GetDisplay_MainWindow()
{
    return m_pDisplay_SDL;
}
#endif

bool OrbiterLinux::RenderDesktop( class DesignObj_Orbiter *pObj, PlutoRectangle rectTotal, PlutoPoint point )
{
#ifndef DIRECTFB
    m_bOrbiterReady = true;
    m_bIsExclusiveMode = false;

    //Orbiter::RenderDesktop(pObj, rectTotal, point);
    vector<int> vectButtonMaps;
    GetButtonsInObject(pObj,vectButtonMaps);

	// If bApplicationInBackground==true, that's for UI2 with orbiter always being on top
	bool bApplicationInBackground = pObj->m_mapObjParms_Find(DESIGNOBJPARAMETER_In_Background_CONST) == "1";
	
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterLinux::RenderDesktop() : ptr=%p coord=[%d,%d,%d,%d], in background %d",
                          pObj,
                          rectTotal.X, rectTotal.Y, rectTotal.Width, rectTotal.Height,
						  bApplicationInBackground
                          );
#endif

	//if an application like xine has to be in background, we'll move orbiter to be on layer above.
	m_bIsExclusiveMode = bApplicationInBackground;

	m_pWinListManager->ShowSdlWindow(bApplicationInBackground, m_bYieldInput);
    if (pObj->m_ObjectType == DESIGNOBJTYPE_App_Desktop_CONST)
    {
		m_pWinListManager->SetExternApplicationPosition(rectTotal);

		string sWindowName = m_pWinListManager->GetExternApplicationName();
		bool bIsWindowAvailable = m_pWinListManager->IsWindowAvailable(sWindowName);
		if (bIsWindowAvailable)
		{
			if ( (rectTotal.Width == -1) && (rectTotal.Height == -1) )
			{
#ifdef DEBUG
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "RenderDesktop : maximize sWindowName='%s'", sWindowName.c_str());
#endif
				m_pWinListManager->PositionWindow(sWindowName, 0, 0, -1, -1);
				m_pWinListManager->MaximizeWindow(sWindowName);
			}
			else
			{
#ifdef DEBUG
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "RenderDesktop : position sWindowName='%s'", sWindowName.c_str());
#endif
				m_pWinListManager->PositionWindow(sWindowName, rectTotal.X, rectTotal.Y, rectTotal.Width, rectTotal.Height);
			}
			m_pWinListManager->ActivateWindow(m_pWinListManager->GetExternApplicationName());
		}
		else
			ActivateExternalWindowAsync(NULL);

		//the keyboard is released to OS, so this is firefox, dvd menu or maybe mythtv-setup 
		//we'll apply a mask with xshape
		if (IsYieldInput())
			ApplyMask(rectTotal, point, mtNormalMask);
    }

#ifdef DEBUG
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterLinux::RenderDesktop() : done");
#endif
	return true;
#endif
}

/*virtual*/ void OrbiterLinux::ActivateExternalWindowAsync(void *)
{
#ifndef DIRECTFB
    string sWindowName = m_pWinListManager->GetExternApplicationName();
    PlutoRectangle rectTotal;
    m_pWinListManager->GetExternApplicationPosition(rectTotal);

	bool bIsWindowAvailable = m_pWinListManager->IsWindowAvailable(sWindowName);
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterLinux::ActivateExternalWindowAsync() : IsWindowAvailable(%s) ==> %s", sWindowName.c_str(), bIsWindowAvailable ? "Yes, it is!" : "No, it's NOT!");
#endif
	if (bIsWindowAvailable)
    {
        if ( (rectTotal.Width == -1) && (rectTotal.Height == -1) )
        {
#ifdef DEBUG
            LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterLinux::ActivateExternalWindowAsync() : maximize sWindowName='%s'", sWindowName.c_str());
#endif
			m_pWinListManager->MaximizeWindow(sWindowName);
        }
        else
        {
#ifdef DEBUG
            LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterLinux::ActivateExternalWindowAsync() : position sWindowName='%s'", sWindowName.c_str());
#endif
            m_pWinListManager->PositionWindow(sWindowName, rectTotal.X, rectTotal.Y, rectTotal.Width, rectTotal.Height);
        }
        m_pWinListManager->ActivateWindow(m_pWinListManager->GetExternApplicationName());
		m_pWinListManager->ApplyContext(m_pWinListManager->GetExternApplicationName());
    }
    else
        CallMaintenanceInMiliseconds( bIsWindowAvailable ? 1000 : 200, (OrbiterCallBack)&OrbiterLinux::ActivateExternalWindowAsync, NULL, pe_ALL );
#endif
}

// public interface implementations below

void OrbiterLinux::Initialize(GraphicType Type, int iPK_Room, int iPK_EntertainArea)
{
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterLinux::Initialize()");
	Orbiter::Initialize(Type, iPK_Room, iPK_EntertainArea);

    //we know here the ui version!
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Orbiter UI Version is %d", m_iUiVersion);
#if !defined(DIRECTFB) && !defined(PADORBITER)
    if(UsesUIVersion2())
	{
		m_pMouseBehavior = new MouseBehavior_Linux(this);
		m_pWinListManager->KeepSdlWindowActive(true);
	}
#endif
#if !defined(MAEMO_NOKIA770) && !defined(DIRECTFB)	
    m_pRecordHandler = new XRecordExtensionHandler(this, m_strDisplayName);
    m_pRecordHandler->enableRecording(true);
#endif

    reinitGraphics();

	// Now see if there are any scancode entries that conflict with the hard keys
	for(map< pair<int,char>,string>::iterator it=m_mapScanCodeToRemoteButton.begin();it!=m_mapScanCodeToRemoteButton.end();++it)
	{
		int iScanCode = it->first.first;

		// Store a NULL entry in m_mapHardKeys so we don't also process this as a hard key
		Orbiter::Event event;
		event.type = Orbiter::Event::BUTTON_DOWN;
		event.data.button.m_iKeycode = iScanCode;
		event.data.button.m_iPK_Button = 0;
		PreprocessEvent(event);
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"OrbiterLinux::Initialize scancode %d is button %d", iScanCode, event.data.button.m_iPK_Button);
#endif
		if( event.data.button.m_iPK_Button )
		{
			if( m_mapHardKeys.find( event.data.button.m_iPK_Button )!=m_mapHardKeys.end() )
			{
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"OrbiterLinux::Initialize removing hardkey %d", event.data.button.m_iPK_Button);
#endif
				Message *pMessage = m_mapHardKeys[event.data.button.m_iPK_Button];
				m_mapHardKeys.erase(event.data.button.m_iPK_Button);
				delete pMessage;
			}
		}
	}

#ifndef DIRECTFB
    //temporary workaround for xfwm window manager
    //this will set orbiter above when its window is created
    //then orbiter will set it to the right layer
    if(UsesUIVersion2() && NULL != m_pWinListManager && !m_pWinListManager->ResetOrbiterWindow())
		OnQuit();    
#endif

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterLinux::Initialize() : done");
}

void OrbiterLinux::Destroy()
{
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterLinux::Destroy()");
#if !defined(MAEMO) && !defined(DIRECTFB)
    delete m_pRecordHandler;
    m_pRecordHandler = NULL;
#endif
#if !defined(PADORBITER) && !defined(DIRECTFB)    
    if (m_pMouseBehavior)
    {
	    LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterLinux::Destroy() killing mouse behavior");
        delete m_pMouseBehavior;
        m_pMouseBehavior = NULL;
    }
#endif
#ifndef DIRECTFB
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterLinux::Destroy() calling X11 Exit");
    X11_Exit();
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterLinux::Destroy() deleting Window manager");
    delete m_pWinListManager;
    m_pWinListManager = NULL;
#endif
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterLinux::Destroy() : done");
}

void OrbiterLinux::CMD_Activate_PC_Desktop(bool bTrueFalse,string &sCMD_Result,Message *pMessage)
{
#ifdef MAEMO_NOKIA770
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Orbiter::CMD_Activate_PC_Desktop - exit for Maemo Nokia");
#endif

	OnQuit();
#else
#ifndef DIRECTFB	
	PLUTO_SAFETY_LOCK(sm, m_ScreenMutex);

	static bool bInternalState_DesktopActivated = false;

	if(bTrueFalse == bInternalState_DesktopActivated)
		return;

	bInternalState_DesktopActivated = bTrueFalse;

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "SSS CMD_Activate_PC_Desktop %d",(int) bTrueFalse);

	if( bTrueFalse )
	{
		if(NULL != m_pWinListManager)
			m_pWinListManager->HandleOffCommand();	

#if defined(VIA_OVERLAY) && defined(ORBITER_OPENGL)
		ViaOverlay::Instance().SuspendOverlay();
#endif

		Orbiter::CMD_Surrender_to_OS("1", true, true);

		//check if kde is running and if not, start it
		ConfirmPcDesktop();

		//re-activate repeated keys
		system("/usr/bin/X11/xset r"); 
	}
	else
	{
#if defined(VIA_OVERLAY) && defined(ORBITER_OPENGL)
		ViaOverlay::Instance().ResumeOverlay();
#endif
		
		if(NULL != m_pWinListManager)
			m_pWinListManager->HandleOnCommand();

		Orbiter::CMD_Surrender_to_OS("0", false, false);

		//disable repeated keys
		system("/usr/bin/X11/xset r off"); 
	}
#endif
#endif	
}

void OrbiterLinux::CMD_Activate_Window(string sWindowName,string &sCMD_Result,Message *pMessage)
{
#ifndef DIRECTFB
	m_sApplicationName = sWindowName;
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterLinux::CMD_Activate_Window(%s)", sWindowName.c_str());
	m_pWinListManager->SetExternApplicationName(sWindowName);

	if(UsesUIVersion2() && (sWindowName.find("Screen_Saver") != string::npos || sWindowName.empty()) )
	{
		if( sWindowName.empty()==false )
			m_pWinListManager->ShowWindow(sWindowName);
		m_pWinListManager->ApplyContext(sWindowName);
	}
#endif
}

void OrbiterLinux::CMD_Simulate_Keypress(string sPK_Button,int iStreamID,string sName,string &sCMD_Result,Message *pMessage)
{
#ifndef DIRECTFB
#ifdef DEBUG
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterLinux::CMD_Simulate_Keypress() : m_bYieldInput==%d", m_bYieldInput);
#endif
    if( m_bYieldInput )
    {
        pair<bool,int> XKeySym = PlutoButtonsToX(atoi(sPK_Button.c_str()));
#ifdef DEBUG
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Need to forward pluto key %s to X key %d (shift %d)", sPK_Button.c_str(),XKeySym.second,XKeySym.first);
#endif
        // TODO: clean this code
        Display *dpy = GetDisplay();
        X11_Locker lock(dpy);

		if( XKeySym.first )
            XTestFakeKeyEvent( dpy, XKeysymToKeycode(dpy, XK_Shift_L), True, 0 );

        XTestFakeKeyEvent( dpy, XKeysymToKeycode(dpy, XKeySym.second), True, 0 );
        XTestFakeKeyEvent( dpy, XKeysymToKeycode(dpy, XKeySym.second), False, 0 );

        if( XKeySym.first )
            XTestFakeKeyEvent( dpy, XKeysymToKeycode(dpy, XK_Shift_L), False, 0 );
    }
#endif
    Orbiter::CMD_Simulate_Keypress(sPK_Button,0,sName,sCMD_Result,pMessage);
}

void OrbiterLinux::CMD_Set_Mouse_Position_Relative(int iPosition_X,int iPosition_Y,string &sCMD_Result,Message *pMessage)
{
#ifndef DIRECTFB
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Moving mouse (relative %d,%d)", iPosition_X, iPosition_Y);

    int nAbsolute_X = 0, nAbsolute_Y = 0;

    m_pX11->Mouse_GetPosition(nAbsolute_X, nAbsolute_Y);
    m_pX11->Mouse_SetPosition(nAbsolute_X + iPosition_X, nAbsolute_Y + iPosition_Y);
#endif
}

void OrbiterLinux::CMD_Simulate_Mouse_Click_At_Present_Pos(string sType,string &sCMD_Result,Message *pMessage)
{
#ifndef DIRECTFB
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Clicking mouse %s",sType.c_str());

    {
        Display *pDisplay = GetDisplay();
        X11_Locker lock(pDisplay);
        XTestFakeButtonEvent(pDisplay, 1, true, 0);
        XTestFakeButtonEvent(pDisplay, 1, false, 0);
    }
#endif
}

void OrbiterLinux::CMD_Surrender_to_OS(string sOnOff,bool bFully_release_keyboard,bool bAlways, string &sCMD_Result, Message *pMessage)
{
#ifndef DIRECTFB
    // when xine is full-screen, on DVD menu
    // and navigation in the dvd-menu with keyboard is wanted
    // then
    // sOnOff should be 1
    // and bFully_release_keyboard should be true
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterLinux::CMD_Surrender_to_OS(%s, %d)", sOnOff.c_str(), bFully_release_keyboard);
    GrabPointer(sOnOff != "1");
    GrabKeyboard(sOnOff != "1");
    Orbiter::CMD_Surrender_to_OS(sOnOff, bFully_release_keyboard, bAlways, sCMD_Result, pMessage);
    if (m_bYieldInput)
    {
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterLinux::CMD_Surrender_to_OS() : ActivateWindow('%s')", m_pWinListManager->GetExternApplicationName().c_str());
        m_pWinListManager->ActivateWindow(m_pWinListManager->GetExternApplicationName());
		m_pWinListManager->ApplyContext();
    }

	if(sOnOff == "0" && !m_bUseMask)
	{
		if(MaskApplied())
			ResetAppliedMask();
	}

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterLinux::CMD_Surrender_to_OS(%s, %d) : done", sOnOff.c_str(), bFully_release_keyboard);
#endif
}

#if !defined(DIRECTFB) && !defined(PADORBITER)
/*virtual*/ ScreenHandler *OrbiterLinux::CreateScreenHandler()
{
	return new OSDScreenHandler(this, &m_mapDesignObj);
}
#endif

void OrbiterLinux::X_LockDisplay()
{
#ifndef DIRECTFB
	if(NULL != m_pX11)
		m_pX11->Lock();
#endif
}

void OrbiterLinux::X_UnlockDisplay()
{
#ifndef DIRECTFB
	if(NULL != m_pX11)
		m_pX11->Unlock();	
#endif
}

void OrbiterLinux::GrabPointer(bool bEnable)
{
#ifndef DIRECTFB
#ifndef MAEMO_NOKIA770
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterLinux::GrabPointer(%d)", bEnable);
    if (bEnable)
    {
        m_pX11->Mouse_Grab(GetMainWindow());
    }
    else
    {
        m_pX11->Mouse_Ungrab();
    }
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterLinux::GrabPointer(%d) : done", bEnable);
#endif
#endif
}

void OrbiterLinux::GrabKeyboard(bool bEnable)
{
#ifndef DIRECTFB
#ifndef MAEMO_NOKIA770
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterLinux::GrabKeyboard(%d)", bEnable);
    if (bEnable)
    {
        m_pX11->Keyboard_Grab(GetMainWindow());
    }
    else
    {
        m_pX11->Keyboard_Ungrab();
    }
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterLinux::GrabKeyboard(%d) : done", bEnable);
#endif
#endif
}

bool OrbiterLinux::MaskApplied()
{
#ifndef DIRECTFB
	return m_bMaskApplied;
#endif
}

void OrbiterLinux::ResetAppliedMask()
{
#ifndef DIRECTFB
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Reseting mask - xshape");

	XRectangle rect = {0, 0, m_iImageWidth, m_iImageHeight};
	
	XShapeCombineRectangles (
		m_pX11->GetDisplay(),
		m_pX11->GetWmWindow(),
		ShapeBounding,
		0, 0,
		&rect,
		1, ShapeSet, Unsorted);
	
	m_bMaskApplied = false;
#endif
}

void OrbiterLinux::ApplyMask(PlutoRectangle rectTotal, PlutoPoint point, MaskTypes mask_type)
{
#ifndef DIRECTFB
	if(mask_type == mtShowPopupMask)
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Applying xshape to show popup: rectangle %d,%d,%d,%d",
			point.X + rectTotal.X, point.Y + rectTotal.Y, rectTotal.Width, rectTotal.Height);

		XRectangle rects[1] = 
		{
			{point.X + rectTotal.X, point.Y + rectTotal.Y, rectTotal.Width, rectTotal.Height}
		};

		XShapeCombineRectangles (
			m_pX11->GetDisplay(),
			m_pX11->GetWmWindow(),
			ShapeBounding,
			0, 0,
			rects,
			1, ShapeUnion, Unsorted);
	}
	else if(mask_type == mtHidePopupMask)
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Applying xshape to hide popup: rectangle %d,%d,%d,%d",
			point.X + rectTotal.X, point.Y + rectTotal.Y, rectTotal.Width, rectTotal.Height);

		XRectangle rects[1] = 
		{
			{point.X + rectTotal.X, point.Y + rectTotal.Y, rectTotal.Width, rectTotal.Height}
		};

		XShapeCombineRectangles (
			m_pX11->GetDisplay(),
			m_pX11->GetWmWindow(),
			ShapeBounding,
			0, 0,
			rects,
			1, ShapeSubtract, Unsorted);		
	}
	else if(mask_type == mtNormalMask)
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Applying xshape: rectangle %d,%d,%d,%d",
			point.X + rectTotal.X, point.Y + rectTotal.Y, rectTotal.Width, rectTotal.Height);

		enum rectIndex
		{
			riTop,
			riBottom,
			riLeft,
			riRight
		};

		XRectangle rects[4] = 
		{
			{0, 0, 0, 0},
			{0, 0, 0, 0},
			{0, 0, 0, 0},
			{0, 0, 0, 0}
		};

		if(point.Y + rectTotal.Y > 0)
		{
			//top
			//rects[riTop].x = 0;
			//rects[riTop].y = 0;
			rects[riTop].width = m_iImageWidth;
			rects[riTop].height = point.Y + rectTotal.Y;
		}

		if(m_iImageHeight - point.Y - rectTotal.Y - rectTotal.Height > 0)
		{
			//bottom
			//rects[riBottom].x = 0;
			rects[riBottom].y = point.Y + rectTotal.Y + rectTotal.Height;
			rects[riBottom].width = m_iImageWidth;
			rects[riBottom].height = m_iImageHeight - point.Y - rectTotal.Y - rectTotal.Height;
		}
		
		if(point.X + rectTotal.X > 0)
		{
			//left
			//rects[riLeft].x = 0;
			rects[riLeft].y = point.Y + rectTotal.Y;
			rects[riLeft].width = point.X + rectTotal.X;
			rects[riLeft].height = rectTotal.Height;
		}

		if(m_iImageWidth - point.X - rectTotal.X - rectTotal.Width > 0)
		{
			//right
			rects[riRight].x = point.X + rectTotal.X + rectTotal.Width;
			rects[riRight].y = point.Y + rectTotal.Y;
			rects[riRight].width = m_iImageWidth - point.X - rectTotal.X - rectTotal.Width;
			rects[riRight].height = rectTotal.Height;
		}
				
		XShapeCombineRectangles (
			m_pX11->GetDisplay(),
			m_pX11->GetWmWindow(),
			ShapeBounding,
			0, 0,
			rects,
			4, ShapeSet, Unsorted);

		m_bMaskApplied = true;
	}

	m_pX11->Sync();

#if defined(VIA_OVERLAY) && defined(ORBITER_OPENGL)
	//for VIA, if we are in dvd menu, we won't restart the alpha mask
	//since alpha overlay is the only way of seeing xine
	if(NULL != m_pScreenHistory_Current && m_pScreenHistory_Current->PK_Screen() != SCREEN_DVDMenuFullScreen_CONST)
	{
		ViaOverlay::Instance().FillRectangleInAlphaMask(point.X + rectTotal.X, point.Y + rectTotal.Y, 
			rectTotal.Width, rectTotal.Height, 0x00);
	}
#endif
#endif
}

bool OrbiterLinux::PreprocessEvent(Orbiter::Event &event)
{
#ifndef DIRECTFB
	if( event.type != Orbiter::Event::BUTTON_DOWN && event.type != Orbiter::Event::BUTTON_UP )
		return false;

	event.data.button.m_iPK_Button = TranslateXKeyCodeToPlutoButton(event.data.button.m_iKeycode,event.type);
	return false;
#endif
}

int OrbiterLinux::TranslateXKeyCodeToPlutoButton(int Keycode,int Type)
{
#ifndef DIRECTFB
	/* Type == BUTTON_DOWN or BUTTON_UP */

	int iPK_Button=0;
    XKeyEvent  kevent;
    KeySym   keysym;
    char   buf[1];

	// First check for some media playback keys that don't seem to be in the X11 headers
	switch( Keycode )
	{
	case 153:
		iPK_Button = BUTTON_Skip_Next_CONST;
		break;
	case 144:
		iPK_Button =  BUTTON_Skip_Prior_CONST;
		break;
	case 180:
		iPK_Button =  BUTTON_Fast_Forward_CONST;
		break;
	case 152:
		iPK_Button =  BUTTON_Rewind_CONST;
		break;
	case 162:
		iPK_Button =  BUTTON_Pause_CONST;
		break;
	case 164:
		iPK_Button =  BUTTON_Stop_CONST;
		break;
	case 100130:
		iPK_Button =  BUTTON_Power_CONST;
		break;
	};

	if( iPK_Button )
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"OrbiterLinux::TranslateXKeyCodeToPlutoButton Keycode %d = media button %d", Keycode, iPK_Button);
#endif
		return iPK_Button;
	}

    kevent.type = KeyPress;
    kevent.display = GetDisplay();
    kevent.state = 0;
    kevent.keycode = Keycode;
    XLookupString(&kevent, buf, sizeof(buf), &keysym, 0);

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"OrbiterLinux::TranslateXKeyCodeToPlutoButton Keycode %d = keysym %d/%d", Keycode, (int)keysym, (unsigned int)keysym);
#endif

    switch ( keysym )
    {
        case XK_Shift_L:   case XK_Shift_R:
			iPK_Button = BUTTON_left_shift_CONST;
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "shift %s", Type == Orbiter::Event::BUTTON_DOWN ? "down" : "up");
            m_bShiftDown = ( Type == Orbiter::Event::BUTTON_DOWN );
			m_bShiftDownOnScreenKeyboard = false;
            break;
        case XK_Control_L:    case XK_Control_R:
            m_bControlDown = ( Type == Orbiter::Event::BUTTON_DOWN );
            break;
        case XK_Alt_L:    case XK_Alt_R:
            m_bAltDown = ( Type == Orbiter::Event::BUTTON_DOWN );
            break;
	case XK_Caps_Lock:
            if ( Type == Orbiter::Event::BUTTON_UP )
                m_bCapsLock = ! m_bCapsLock;
            break;

        case XK_F1:     iPK_Button = BUTTON_F1_CONST; break;
        case XK_F2:     iPK_Button = BUTTON_F2_CONST; break;
        case XK_F3:     iPK_Button = BUTTON_F3_CONST; break;
        case XK_F4:     iPK_Button = BUTTON_F4_CONST; break;
        case XK_F5:     iPK_Button = BUTTON_F5_CONST; break;
		case XK_F6:     iPK_Button = BUTTON_F6_CONST; LoggerWrapper::GetInstance()->Write(LV_STATUS, "Key F6 %s", Type == Orbiter::Event::BUTTON_DOWN ? "down" : "up"); break;
		case XK_F7:     iPK_Button = BUTTON_F7_CONST; LoggerWrapper::GetInstance()->Write(LV_STATUS, "Key F7 %s", Type == Orbiter::Event::BUTTON_DOWN ? "down" : "up"); break;
		case XK_F8:     iPK_Button = BUTTON_F8_CONST; LoggerWrapper::GetInstance()->Write(LV_STATUS, "Key F8 %s", Type == Orbiter::Event::BUTTON_DOWN ? "down" : "up"); break;

        case XK_0: case XK_KP_0:
            if(m_bShiftDown)
                iPK_Button = BUTTON_right_parenthesis_CONST;
            else
                iPK_Button = BUTTON_0_CONST;
            break;
        case XK_1: case XK_KP_1:
            if(m_bShiftDown)
                iPK_Button = BUTTON_exclamation_point_CONST;
            else
                iPK_Button = BUTTON_1_CONST;
            break;
        case XK_2: case XK_KP_2:
            if(m_bShiftDown)
                iPK_Button = BUTTON_at_sign_CONST;
            else
                iPK_Button = BUTTON_2_CONST;
            break;
        case XK_3: case XK_KP_3:
           if(m_bShiftDown)
                iPK_Button = BUTTON_Pound_CONST;
            else
                iPK_Button = BUTTON_3_CONST;
            break;
        case XK_4: case XK_KP_4:
            if(m_bShiftDown)
                iPK_Button = BUTTON_dollar_CONST;
            else
                iPK_Button = BUTTON_4_CONST;
            break;
        case XK_5: case XK_KP_5:
            if(m_bShiftDown)
                iPK_Button = BUTTON_percent_CONST;
            else
                iPK_Button = BUTTON_5_CONST;
            break;
        case XK_6: case XK_KP_6:
            if(m_bShiftDown)
                iPK_Button = BUTTON_caret_CONST;
            else
                iPK_Button = BUTTON_6_CONST;
            break;
        case XK_7: case XK_KP_7:
            if(m_bShiftDown)
                iPK_Button = BUTTON_ampersand_CONST;
            else
                iPK_Button = BUTTON_7_CONST;
            break;
        case XK_8: case XK_KP_8:
            if(m_bShiftDown)
                iPK_Button = BUTTON_Asterisk_CONST;
            else
                iPK_Button = BUTTON_8_CONST;
            break;
        case XK_9: case XK_KP_9:
            if(m_bShiftDown)
                iPK_Button = BUTTON_left_parenthesis_CONST;
            else
                iPK_Button = BUTTON_9_CONST;
            break;

        case XK_Up:
			if(m_bShiftDown)
				iPK_Button = BUTTON_Shift_Up_Arrow_CONST;
			else
				iPK_Button = BUTTON_Up_Arrow_CONST;
			break;
        case XK_Down:
			if(m_bShiftDown)
				iPK_Button = BUTTON_Shift_Down_Arrow_CONST;
			else
				iPK_Button = BUTTON_Down_Arrow_CONST;
			break;
        case XK_Left:
			if(m_bShiftDown)
				iPK_Button = BUTTON_Shift_Left_Arrow_CONST;
			else
		      	iPK_Button = BUTTON_Left_Arrow_CONST;
            break;
        case XK_Right:
			if(m_bShiftDown)
				iPK_Button = BUTTON_Shift_Right_Arrow_CONST;
			else
				iPK_Button = BUTTON_Right_Arrow_CONST;
			break;

        case XK_Page_Down:		iPK_Button = BUTTON_Scroll_Down_CONST; break;
        case XK_Page_Up:		iPK_Button = BUTTON_Scroll_Up_CONST; break;

/* modificari */
        case XK_F9:     iPK_Button = BUTTON_F9_CONST; break;
        case XK_F10:    iPK_Button = BUTTON_F10_CONST; break;
        case XK_Escape: iPK_Button = BUTTON_escape_CONST; break;
        case XK_Tab:    iPK_Button = BUTTON_tab_CONST; break;
        case XK_KP_Enter: case XK_Return:       iPK_Button = BUTTON_Enter_CONST;  break;
		case XK_KP_Space: case XK_space:        iPK_Button = BUTTON_space_CONST; break;
        case XK_BackSpace:      iPK_Button = BUTTON_Back_CONST;  break;
        case XK_underscore:     iPK_Button = BUTTON_underscore_CONST;  break;
        case XK_minus:  iPK_Button = BUTTON_dash_CONST;  break;
        case XK_equal:  case XK_KP_Equal:
			iPK_Button = BUTTON_equals_sign_CONST;
			break;
	case XK_slash:	iPK_Button = BUTTON_slash_CONST;  break;
        case XK_period: iPK_Button = BUTTON_dot_CONST;  break;
        case XK_comma:  case XK_KP_Separator:   iPK_Button =  BUTTON_comma_CONST; break;
        case XK_colon:  iPK_Button = BUTTON_colon_CONST; break;
        case XK_semicolon:      iPK_Button = BUTTON_semicolumn_CONST; break;
        case XK_quotedbl:       iPK_Button = BUTTON_double_quote_CONST; break;
			
/* end modificari */

        default:
            if ( XK_a <= keysym && keysym <= XK_z )
			{
				iPK_Button = BUTTON_a_CONST + keysym - XK_a;
			}
    }

#ifdef DEBUG
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "The keysym was %d, the final Type %d, button %d", keysym, Type, iPK_Button);
#endif

	return iPK_Button;
#endif
}

void OrbiterLinux::StopActivateExternalWindowTask()
{
#ifndef DIRECTFB
	PLUTO_SAFETY_LOCK( cm, m_MaintThreadMutex );
	for(map<int,PendingCallBackInfo *>::iterator it=m_mapPendingCallbacks.begin();it!=m_mapPendingCallbacks.end();++it)
	{
		PendingCallBackInfo *pCallBackInfo = (*it).second;
		if(pCallBackInfo->m_fnCallBack== (OrbiterCallBack)&OrbiterLinux::ActivateExternalWindowAsync)
		{
			m_mapPendingCallbacks.erase(it);
			return;
		}
	}
#endif
}

void OrbiterLinux::ConfirmPcDesktop()
{
#if !defined(PADORBITER) && !defined(DIRECTFB)
#if defined(VIA_OVERLAY) && defined(ORBITER_OPENGL)
	if(!ProcessUtils::SpawnDaemon("/usr/pluto/bin/Fiire_KDE.sh", "", true))
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed to start KDE (with Fiire_KDE.sh)");
#else
	if(!ProcessUtils::SpawnDaemon("/usr/pluto/bin/Start_KDE.sh", "", true))
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed to start KDE");
#endif
#endif
}

