//
// Author : Cristian Miron
//
// Changed by : Remus C.
//

#include "WMController.h"

#include "../../wrapper/wrapper_x11.h"
#include "../../../defines/define_logger.h"
#include "DCE/Logger.h"
using namespace DCE;

//-------------------------------------------------------------------------------------------------------------
WMControllerImpl::WMControllerImpl()
{
	m_ulUserCurrentDesktop = 0;

	const char *pPrimaryDesktopName = getenv("ORBITER_PRIMARY_DESKTOP");
    if(NULL != pPrimaryDesktopName)
        m_sPrimaryDesktop = pPrimaryDesktopName;
    else
 		m_sPrimaryDesktop = "0";

    const char *pSecondaryDesktopName = getenv("ORBITER_SECONDARY_DESKTOP");
    if(NULL != pSecondaryDesktopName)
        m_sSecondaryDesktop = pSecondaryDesktopName;
    else
 		m_sSecondaryDesktop = "1";

#if defined(ORBITER)
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "WMControllerImpl: primary desktop %s", m_sPrimaryDesktop.c_str());
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "WMControllerImpl: secondary desktop %s", m_sSecondaryDesktop.c_str());

	SaveCurrentUserDesktop();
	SwitchToPrimaryDesktop();
#endif
}
//-------------------------------------------------------------------------------------------------------------
WMControllerImpl::~WMControllerImpl()
{
}

//-------------------------------------------------------------------------------------------------------------
bool WMControllerImpl::SetVisible(string sWindow, bool bVisible, bool bUseWindowId)
{
	TranslateWindowId(sWindow, bUseWindowId);

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "WMControllerImpl::SetVisible() : window: %s/%s, visible: %d\n", sWindow.c_str(), bVisible ? m_sPrimaryDesktop.c_str() : m_sSecondaryDesktop.c_str(), bVisible);
#endif
    return wmctrl.ActionCommand('t', sWindow.c_str(), bVisible ? m_sPrimaryDesktop.c_str() : m_sSecondaryDesktop.c_str(), bUseWindowId);
}

//-------------------------------------------------------------------------------------------------------------
bool WMControllerImpl::SetLayer(string sWindow, WindowLayer aLayer, bool bUseWindowId)
{
	TranslateWindowId(sWindow, bUseWindowId);

    // TODO: activate the lines with ActionCommand and 'O', after the demo!
    // these should remove the need for the bogus_xterm
    // ActionCommand 'O' :  clear/set recursive the override_redirect property for a window
    // SetLayer() was broken by the alpha-patch, when xcompmgr is used
    // not activated by default, to not change anything before the demo

#ifdef DEBUG
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "WMControllerImpl::SetLayer() : window: %s, aLayer: %d\n", sWindow.c_str(), aLayer);
#endif

    bool bResult = true;
	switch(aLayer)
	{
		case LayerNormal:
            bResult = bResult && wmctrl.ActionCommand('b', sWindow.c_str(), "remove,above", bUseWindowId);
            bResult = bResult && wmctrl.ActionCommand('b', sWindow.c_str(), "remove,below", bUseWindowId);
			break;
		case LayerAbove:
            bResult = bResult && wmctrl.ActionCommand('b', sWindow.c_str(), "remove,below", bUseWindowId);
            bResult = bResult && wmctrl.ActionCommand('b', sWindow.c_str(), "remove,above", bUseWindowId);
            bResult = bResult && wmctrl.ActionCommand('b', sWindow.c_str(), "add,above", bUseWindowId);
			break;
		case LayerBelow:
            bResult = bResult && wmctrl.ActionCommand('b', sWindow.c_str(), "remove,above", bUseWindowId);
//            bResult = bResult && wmctrl.ActionCommand('b', sWindow.c_str(), "remove,below", bUseWindowId);
            bResult = bResult && wmctrl.ActionCommand('b', sWindow.c_str(), "add,below", bUseWindowId);
			break;
		default:
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "WMControllerImpl::SetLayer() : Unknown layer!");
			return false;
	}
	return bResult;
}

//-------------------------------------------------------------------------------------------------------------
bool WMControllerImpl::SetPosition(string sWindow, int x, int y, int w, int h, bool bUseWindowId)
{
	TranslateWindowId(sWindow, bUseWindowId);

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "WMControllerImpl::SetPosition() : window: %s, position: %d %d %d %d\n", sWindow.c_str(), x, y, w, h);
#endif

	char sParam[128];
	sprintf(sParam, "0,%d,%d,%d,%d", x, y, w, h);
    return wmctrl.ActionCommand('e', sWindow.c_str(), sParam, bUseWindowId);
}

//-------------------------------------------------------------------------------------------------------------
bool WMControllerImpl::SetFullScreen(string sWindow, bool bFullScreen, bool bUseWindowId)
{
	TranslateWindowId(sWindow, bUseWindowId);

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "WMControllerImpl::SetFullScreen() : window: %s, fullscreen: %d\n", sWindow.c_str(), bFullScreen);
#endif

    return wmctrl.ActionCommand('b', sWindow.c_str(), bFullScreen ? "add,fullscreen" : "remove,fullscreen", bUseWindowId);
}

//-------------------------------------------------------------------------------------------------------------
bool WMControllerImpl::SetMaximized(string sWindow, bool bMaximized, bool bUseWindowId)
{
	TranslateWindowId(sWindow, bUseWindowId);

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "WMControllerImpl::SetMaximized() : window: %s, maximized: %d\n", sWindow.c_str(), bMaximized);
#endif

    return wmctrl.ActionCommand('b', sWindow.c_str(), bMaximized ? "add,maximized_horz,maximized_vert" : "remove,maximized_horz,maximized_vert", bUseWindowId);
}

//-------------------------------------------------------------------------------------------------------------
bool WMControllerImpl::ActivateWindow(string sWindow, bool bUseWindowId)
{
	TranslateWindowId(sWindow, bUseWindowId);

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "WMControllerImpl::ActivateWindow() : window: %s\n", sWindow.c_str());
#endif

    if (sWindow == "")
        return false;
    return wmctrl.ActionCommand('R', sWindow.c_str(), "", bUseWindowId);
}
//-------------------------------------------------------------------------------------------------------------
bool WMControllerImpl::ListWindows(list<WinInfo> &listWinInfo)
{
#ifdef DEBUG
//	LoggerWrapper::GetInstance()->Write(LV_STATUS, "WMControllerImpl::ListWindows()\n");
#endif

	bool bResult = wmctrl.ActionCommand('l', NULL, NULL, true, &listWinInfo);

	int nPrimaryDesktop = atoi(m_sPrimaryDesktop.c_str());
	int nSecondaryDesktop = atoi(m_sSecondaryDesktop.c_str());

	//filter the windows; we need only those from our desktops
	for(list<WinInfo>::iterator it = listWinInfo.begin(); it != listWinInfo.end();)
	{
#ifdef DEBUG
//		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Window %s, id 0x%x desktop %d", it->sClassName.c_str(), it->ulWindowId, it->lDesktop); 
#endif

		if(static_cast<int>(it->lDesktop) != nPrimaryDesktop && static_cast<int>(it->lDesktop) != nSecondaryDesktop)
		{
#ifdef DEBUG
//			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Skipping %s id 0x%x (it's not on our desktops)", it->sClassName.c_str(), it->ulWindowId); 
#endif
			listWinInfo.erase(it++);
		}
		else
			++it;
	}

    return bResult;
}
//-------------------------------------------------------------------------------------------------------------
string WMControllerImpl::WindowId(string sWindowClass)
{
	list<WinInfo> listWinInfo;
	wmctrl.ActionCommand('l', NULL, NULL, true, &listWinInfo);

	int nPrimaryDesktop = atoi(m_sPrimaryDesktop.c_str());
	int nSecondaryDesktop = atoi(m_sSecondaryDesktop.c_str());

	//LoggerWrapper::GetInstance()->Write(LV_STATUS, "WMControllerImpl::WindowId window class %s", sWindowClass.c_str()); 

	//filter the windows; we need only those from our desktops
	for(list<WinInfo>::iterator it = listWinInfo.begin(); it != listWinInfo.end();++it)
	{
		//LoggerWrapper::GetInstance()->Write(LV_STATUS, "Window %s, id 0x%x desktop %d", it->sClassName.c_str(), it->ulWindowId, it->lDesktop); 

		if(
			(
				static_cast<int>(it->lDesktop) == nPrimaryDesktop || 
				static_cast<int>(it->lDesktop) == nSecondaryDesktop ||
				static_cast<int>(it->lDesktop) == -1
				//it->sClassName.find("kicker") != string::npos

			)
			&&
			it->sClassName.find(sWindowClass) != string::npos
		)
		{
			char buf[64];
			sprintf(buf, "0x%x", static_cast<unsigned int>(it->ulWindowId));
			return string(buf);
		}
	}

    return "";
}
//-------------------------------------------------------------------------------------------------------------
void WMControllerImpl::SaveCurrentUserDesktop()
{
	unsigned long ulUserCurrentDesktop = 0;
	if(wmctrl.CurrentDesktop(ulUserCurrentDesktop) && static_cast<int>(ulUserCurrentDesktop) != atoi(m_sPrimaryDesktop.c_str()))
		m_ulUserCurrentDesktop = ulUserCurrentDesktop;

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "WMControllerImpl: user's current desktop: %d", m_ulUserCurrentDesktop);
}
//-------------------------------------------------------------------------------------------------------------
void WMControllerImpl::SwitchToPrimaryDesktop()
{
	//Switch to primary desktop
    wmctrl.ActionCommand('s', NULL, m_sPrimaryDesktop.c_str());

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "WMControllerImpl: switching to primary desktop %s", m_sPrimaryDesktop.c_str());
}
//-------------------------------------------------------------------------------------------------------------
void WMControllerImpl::SwitchToUserDesktop()
{
	//Switch to user's desktop
	wmctrl.ActionCommand('s', NULL, StringUtils::ltos(m_ulUserCurrentDesktop).c_str());

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "WMControllerImpl: switching back to user's desktop %d", m_ulUserCurrentDesktop);
}
//-------------------------------------------------------------------------------------------------------------
void WMControllerImpl::TranslateWindowId(string& sWindow, bool& bUseWindowId)
{
	if(!bUseWindowId)
	{
		string sWindowID = WindowId(sWindow);
		if(!sWindowID.empty())
		{
			sWindow = sWindowID;
			bUseWindowId = true;
		}
	}
}
//-------------------------------------------------------------------------------------------------------------
