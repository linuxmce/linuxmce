//
// Author : Cristian Miron
//
// Changed by : Remus C.
//

// Notes : use a single instance

#ifndef __WINDOW_MANAGER_H__
#define __WINDOW_MANAGER_H__

#include <X11/Xlib.h>

#include "../wmctrl/wmctrl.h"
#include "WindowLayer.h"

#include <list>
#include <string>
using namespace std;

//-------------------------------------------------------------------------------------------------------------
class WMControllerImpl
{
public:
    WMControllerImpl();
    ~WMControllerImpl();

	bool SetVisible(string sWindow, bool bVisible, bool bUseWindowId = false);
    bool SetLayer(string sWindow, WindowLayer aLayer, bool bUseWindowId = false);
    bool SetPosition(string sWindow, int x, int y, int w, int h, bool bUseWindowId = false);
    bool SetFullScreen(string sWindow, bool bFullScreen, bool bUseWindowId = false);
    bool SetMaximized(string sWindow, bool bMaximized, bool bUseWindowId = false);
    bool ActivateWindow(string sWindow, bool bUseWindowId = false);

    bool ListWindows(list<WinInfo> &listWinInfo);

	void SaveCurrentUserDesktop();
	void SwitchToPrimaryDesktop();
	void SwitchToUserDesktop();

	string WindowId(string sWindowClass);

protected:
    WmCtrl wmctrl;

	unsigned long m_ulUserCurrentDesktop;
	string m_sPrimaryDesktop;
	string m_sSecondaryDesktop;

	void TranslateWindowId(string& sWindow, bool& bUseWindowId);
};

//-------------------------------------------------------------------------------------------------------------
#endif
