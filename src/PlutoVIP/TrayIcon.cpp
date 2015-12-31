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
// TrayIcon.cpp : implementation file
//

#include "stdafx.h"
#include "TrayIcon.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


const UINT WM_TASKBAR_CREATED = ::RegisterWindowMessage(_T("TaskbarCreated"));

/////////////////////////////////////////////////////////////////////////////
// CTrayIcon

CTrayIcon::CTrayIcon()
{
	SetDefaultValues();
}

CTrayIcon::~CTrayIcon()
{
    RemoveIcon();
    DestroyWindow();
}

BEGIN_MESSAGE_MAP(CTrayIcon, CWnd)
	//{{AFX_MSG_MAP(CTrayIcon)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	ON_WM_SETTINGCHANGE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_TASKBAR_CREATED, OnTaskbarCreated)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrayIcon message handlers
void CTrayIcon::SetDefaultValues()
{
    ::ZeroMemory(&m_NIData, sizeof(NOTIFYICONDATA));
    
	// Default initialization for the NOTIFYICONDATA base struct.
	
    m_NIData.cbSize           = sizeof(NOTIFYICONDATA);
    m_NIData.hWnd             = NULL;
    m_NIData.uID              = 0;
    m_NIData.uFlags           = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    m_NIData.uCallbackMessage = WM_TRAYICON;
    m_NIData.hIcon            = NULL;
    m_NIData.szTip[0]         = 0;
	
    // Initialize the remaining member data.
	
	m_iMaxTipSize		= 64;
    m_nIconID			= 0;
    m_strToolTip		= _T("");
    m_nIDEvent			= 1001;
    m_nCounter			= 0;
    m_uDefMenuItemID    = 0;
    m_bDefMenuItemByPos = true;
    m_bHidden           = true;
    m_bRemoved          = true;
    m_bShowPending      = false;
    m_hWndNotify        = NULL;
	m_hIcon             = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CTrayIcon notification window initialization

bool CTrayIcon::SetNotificationWnd(CWnd* pWndNotify)
{
	ASSERT_VALID(pWndNotify);
	
    // Make sure Notification window is valid
    if (!pWndNotify || !::IsWindow(pWndNotify->GetSafeHwnd())) 
		return false;
	
	// assign values
    m_NIData.hWnd   = pWndNotify->GetSafeHwnd();
    m_NIData.uFlags = 0;
	
	if (!::Shell_NotifyIcon(NIM_MODIFY, &m_NIData)) 
		return false;
	
	return true;
}

CWnd* CTrayIcon::GetNotificationWnd()
{
	return CWnd::FromHandle(m_NIData.hWnd);
}

bool CTrayIcon::SetIcon(HICON hIcon)
{
	// check to see if this icon was already set.
	if ( m_NIData.hIcon == hIcon )
		return true;
	
    m_NIData.uFlags = NIF_ICON;
    m_NIData.hIcon  = hIcon;
	
    if ( m_bHidden )
        return true;
	
    if (!::Shell_NotifyIcon( NIM_MODIFY, &m_NIData))
		return false;
	
	return true;
}

bool CTrayIcon::SetIcon(UINT nIDResource)
{
	m_hIcon=LoadIcon(GetModuleHandle(NULL), 
		MAKEINTRESOURCE(nIDResource));
    
	return SetIcon(m_hIcon);
}

HICON CTrayIcon::GetIcon() const
{
    return m_NIData.hIcon;
}

bool CTrayIcon::SetTooltipText(LPCTSTR lpszTipText)
{
    ASSERT(AfxIsValidString(lpszTipText));
    ASSERT(_tcslen(lpszTipText) < m_iMaxTipSize);
	
    m_NIData.uFlags = NIF_TIP;
    _tcsncpy(m_NIData.szTip, lpszTipText, m_iMaxTipSize-1);
	
    if (m_bHidden) 
		return true;
    	
    if (!::Shell_NotifyIcon(NIM_MODIFY, &m_NIData)) 
		return false;
    
	return true;
}

bool CTrayIcon::SetTooltipText(UINT nTipText)
{
	CString strTipText;
	if (!strTipText.LoadString(nTipText)) 
		return false;
	
	
	return SetTooltipText(strTipText);
}

CString CTrayIcon::GetTooltipText() const
{
    return CString(m_NIData.szTip);
}

bool CTrayIcon::AddIcon()
{
    if (!m_bRemoved) 
		RemoveIcon();
    	
    m_NIData.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    if (!::Shell_NotifyIcon(NIM_ADD, &m_NIData))
    {
        m_bShowPending = true;
		return false;
    }
	
    m_bRemoved = m_bHidden = false;
	
    return true;
}

bool CTrayIcon::RemoveIcon()
{
    m_bShowPending = false;
	
    if (m_bRemoved) 
		return true;
    	
    m_NIData.uFlags = 0;
    if (!::Shell_NotifyIcon(NIM_DELETE, &m_NIData))
		return false;
    	
    m_bRemoved = m_bHidden = true;
	
    return true;
}

bool CTrayIcon::SetDefaultMenuItem(UINT uItem, bool bByPos)
{
	// if the values already exist, return true.
    if ((m_uDefMenuItemID == uItem) && (m_bDefMenuItemByPos == bByPos))  
		return true;
	
    m_uDefMenuItemID    = uItem;
    m_bDefMenuItemByPos = bByPos;
	
	// verify that we can load the menu defined by uID.
    CMenu menu;
    if (!menu.LoadMenu(m_NIData.uID))
        return false;
    	
	// see if we can access the submenu
    CMenu* pSubMenu = menu.GetSubMenu(0);
    if (!pSubMenu) 
		return false;
    
	
	// check to see if we can set the submenu for the popup.  This is just a check to ensure
	// that everything has been correctly set.
    if (!::SetMenuDefaultItem(pSubMenu->m_hMenu, m_uDefMenuItemID, 
								m_bDefMenuItemByPos)) 
	{
		return false;
    }
	
	return true;
}

void CTrayIcon::GetDefaultMenuItem(UINT& uItem, bool& bByPos)
{
    uItem  = m_uDefMenuItemID;
    bByPos = m_bDefMenuItemByPos;
}

bool CTrayIcon::Create(
						 LPCTSTR lpszCaption,
						 CWnd* pParentWnd,
						 UINT nIconID,
						 UINT uMenuID/*=0*/,
						 UINT uDefMenuItemID/*=0*/,
						 bool bDefMenuItemByPos/*=false*/)
{
	ASSERT(pParentWnd); // must be valid.
	
	m_nIconID     = nIconID;
	m_strToolTip  = lpszCaption;
	m_hWndNotify  = pParentWnd->GetSafeHwnd();
    m_iMaxTipSize = CountOf(m_NIData.szTip);
	
	// Set the tray icon and tooltip text
	SetIcon(m_nIconID);
	SetTooltipText(m_strToolTip);
	
    // Create an invisible window
    CWnd::CreateEx(0, AfxRegisterWndClass(0), _T(""), WS_POPUP, 0,0,0,0, NULL, 0);
	
	m_NIData.hWnd		= m_hWnd;
	m_NIData.uID		= uMenuID;
	m_uDefMenuItemID    = uDefMenuItemID;
	m_bDefMenuItemByPos = bDefMenuItemByPos;
	
	m_uFlags = m_NIData.uFlags;
	
	return AddIcon();
}

LRESULT CTrayIcon::OnTrayNotification(WPARAM wParam, LPARAM lParam) 
{
    // Return quickly if its not for this tray icon
    if (wParam != m_NIData.uID) 
        return 0L;
    

    if (m_hWndNotify == NULL) 
        return 0L;
    

	// Check to see if our notification window has already handled this
	// message, if so then return success.
	if (::SendMessage(m_hWndNotify, WM_TRAYICON, wParam, lParam))
		return 1;

	switch (LOWORD(lParam))
	{

		case WM_RBUTTONUP:
		{
			CMenu menu;
			if (!menu.LoadMenu(m_NIData.uID)) 
				return 0;
			
			CMenu* pSubMenu = menu.GetSubMenu(0);
			if (pSubMenu == NULL) 
				return 0;
						
			// Make chosen menu item the default (bold font)
			::SetMenuDefaultItem(pSubMenu->m_hMenu,
				m_uDefMenuItemID, m_bDefMenuItemByPos);
			
			CPoint pos;
			GetCursorPos(&pos);
			::SetForegroundWindow(m_hWndNotify); 
			
			::TrackPopupMenu(pSubMenu->m_hMenu, 0, pos.x, pos.y,
							 0, m_hWndNotify, NULL);
			
			::PostMessage(m_hWndNotify, WM_NULL, 0, 0);
			
			menu.DestroyMenu();
		}
		break;

		case WM_LBUTTONDBLCLK:
		{
			// double click received, the default action is to execute default menu item
			::SetForegroundWindow(m_hWndNotify);  
			
			UINT uItem;
			if (m_bDefMenuItemByPos)
			{
				CMenu menu;
				if (!menu.LoadMenu(m_NIData.uID)) 
					return 0;
				
				
				CMenu* pSubMenu = menu.GetSubMenu(0);
				if (pSubMenu == NULL) 
					return 0;
				
				
				uItem = pSubMenu->GetMenuItemID(m_uDefMenuItemID);
				
				menu.DestroyMenu();
			}
			else 
				uItem = m_uDefMenuItemID;
						
			::SendMessage(m_hWndNotify, WM_COMMAND, uItem, 0);
		}
	}

    return 1;
}

LRESULT CTrayIcon::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
    if (message == m_NIData.uCallbackMessage)
		return OnTrayNotification(wParam, lParam);
		
	return CWnd::WindowProc(message, wParam, lParam);
}

LRESULT CTrayIcon::OnTaskbarCreated(WPARAM wParam, LPARAM lParam) 
{
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
	
    InstallIconPending();
	return 0L;
}

void CTrayIcon::OnSettingChange(UINT uFlags, LPCTSTR lpszSection) 
{
	CWnd::OnSettingChange(uFlags, lpszSection);
	
    if (uFlags == SPI_SETWORKAREA) 
        InstallIconPending();	
    
}

void CTrayIcon::InstallIconPending()
{
    // Is the icon display pending, and it's not been set as "hidden"?
    if (!m_bShowPending || m_bHidden) 
        return;
    	
	// Reset the flags to what was used at creation
	m_NIData.uFlags = m_uFlags;
	
    // Try and recreate the icon
    m_bHidden = !::Shell_NotifyIcon(NIM_ADD, &m_NIData);
	
    // If it's STILL hidden, then have another go next time...
    m_bShowPending = !m_bHidden;
	
    ASSERT(m_bHidden == FALSE);
}

bool CTrayIcon::CreateMinimizeWnd(CWnd* pWndApp)
{
    // Create the minimize window
    if (!::IsWindow(m_wndMinimize.m_hWnd))
    {
		if (!m_wndMinimize.CreateEx(0, AfxRegisterWndClass(0), _T(""), WS_POPUP,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, 0))
		{
			return false;
		}
    }
	
    pWndApp->SetParent(&m_wndMinimize);
    return true;
}

void CTrayIcon::MinimizeToTray(CWnd* pWndApp)
{
    CreateMinimizeWnd(pWndApp);
    pWndApp->ShowWindow(SW_HIDE);
}

void CTrayIcon::MaximizeFromTray(CWnd* pWndApp)
{
    pWndApp->SetParent(NULL);
		
	pWndApp->ShowWindow(SW_SHOW);
	pWndApp->RedrawWindow(NULL, NULL, RDW_UPDATENOW | RDW_ALLCHILDREN | RDW_FRAME |
		RDW_INVALIDATE | RDW_ERASE);
	
    // Move focus away and back again to ensure taskbar icon is recreated
    if (::IsWindow(m_wndMinimize.m_hWnd)) {
        m_wndMinimize.SetActiveWindow();
    }
	
    pWndApp->SetActiveWindow();
    pWndApp->SetForegroundWindow();
}
