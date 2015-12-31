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
// Window List Manager

#ifndef __WM_LIST_MANAGER__
#define __WM_LIST_MANAGER__

#include "SerializeClass/ShapesColors.h"
#include "utilities/linux/window_manager/WMController/WMController.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include <string>
#include <list>
using namespace std;
//-----------------------------------------------------------------------------------------------------
class WindowContext;
//-----------------------------------------------------------------------------------------------------
typedef map<string, WindowContext> WindowsContext;
//-----------------------------------------------------------------------------------------------------
class WinListManager
{
public:
    WinListManager(const string &sSdlWindowName);
    ~WinListManager();

    void ActivateWindow(const string& sWindowsName);
    void ActivateSdlWindow();

    void ShowSdlWindow(bool bExclusive, bool bYieldInput);

    void ShowWindow(const string &sWindowName);
    void MaximizeWindow(const string &sWindowName);
    void PositionWindow(const string &sWindowName, int x, int y, int w, int h);
    void HideAllWindows();

    string GetExternApplicationName();
    void SetExternApplicationName(const string &sName);

    void GetExternApplicationPosition(PlutoRectangle &coord);
    void SetExternApplicationPosition(const PlutoRectangle &coord);

    bool IsEmpty();
    bool IsWindowAvailable(const string &sClassName);
	bool HideWindow(const string &sClassName);

	void GetWindows(list<WinInfo>& listWinInfo);
	void SetSdlWindowVisibility(bool bValue);
	bool ResetOrbiterWindow();

	void SetLayer(const string &sClassName, WindowLayer layer);

	void ApplyContext(string sExternalWindowName = ""); //if empty, apply to all

	void KeepSdlWindowActive( bool bKeepSdlWindowActive ) { m_bKeepSdlWindowActive=bKeepSdlWindowActive; }

	void HandleOnCommand();
	void HandleOffCommand();
	
private:

	// With myth tv it happens that if we activate Orbiter and then activate Myth
	// then even though Orbiter is topmost (ie add,above) it is not visible.  So we
	// can set the sdl window to keep active and whenever another window is activated,
	// this one will also be activated right after (m_bKeepSdlWindowActive)
	bool m_bKeepSdlWindowActive;

	WindowsContext m_CurrentContext;
	WindowsContext m_PendingContext;

	WindowContext& PendingContext(string sWindowName);

    pthread_mutexattr_t m_WindowsMutexAttr;
	pluto_pthread_mutex_t m_WindowsMutex;

	string m_sExternApplicationName;
    PlutoRectangle m_coordExternalApplication;
    unsigned long m_pidExternalApplication;

	// Every time we run GetWindows() we will update m_mapKnownWindows with the known windows
	// and check if that list has changed.  If it has we will set m_bExternalChange to true
	// so the next ApplyContext will cleanly apply all changes.  This should resolve an issue
	// where an app may die and restart itself (like xine) but it's window doesn't get correctly
	// positioned because ApplyContext doesn't see that we're trying to change something
	// we didn't already change
	map<unsigned long,string> m_mapKnownWindows;  // PID,WindowName
	bool m_bExternalChange;

	bool m_bHideSdlWindow;
	bool m_bExclusive;

	//internal helpers methods
	void Internal_ActivateWindow(const string& sWindowsName);
	void Internal_ActivateSdlWindow();
	void Internal_ShowSdlWindow(bool bExclusive, bool bYieldInput);
	void Internal_ShowWindow(const string &sWindowName);
	void Internal_MaximizeWindow(const string &sWindowName);
	void Internal_PositionWindow(const string &sWindowName, int x, int y, int w, int h);
	bool Internal_HideWindow(const string &sClassName);
	void Internal_SetSdlWindowVisibility(bool bValue);
	void Internal_ResetOrbiterWindow();
	void RemoveWindowDecoration(string sWindowId);

	//variables
	string m_sSdlWindowName;
	WMControllerImpl *m_pWMController;
	bool m_bEnabled;
};

#endif
// __WM_LIST_MANAGER__
