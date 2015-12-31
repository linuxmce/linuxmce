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
#ifndef _TRAY_ICON_H_
#define _TRAY_ICON_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TrayIcon.h : header file
//

//Tray Icon Message
#define  WM_TRAYICON		(WM_USER+1)

#define CountOf(array) (sizeof(array)/sizeof(array[0]))
/////////////////////////////////////////////////////////////////////////////
// CTrayIcon window

class CTrayIcon : public CWnd
{
public:

    CTrayIcon();
    virtual ~CTrayIcon();

protected:

	UINT			m_uFlags;			 // Style settings for icon restore.
	UINT			m_nIconID;			 // Resource ID for the default icon.
	UINT			m_nIDEvent;			 // Timer event ID.
	UINT			m_nCounter;			 // Holds the current position in the timer loop.
	UINT			m_uDefMenuItemID;	 // Default popup menu item ID.
	bool			m_bDefMenuItemByPos; // Determines if the default menu item is a command or index.
	bool			m_bHidden;			 // State of the icon. true to indicate the icon is hidden.
	bool			m_bRemoved;			 // true if the icon has been removed from the system tray.
	bool			m_bShowPending;		 // true if the icon display is pending.
	HWND			m_hWndNotify;		 // Handle to the window that receives command notification.
	CWnd			m_wndMinimize;		 // Hidden window used during minimize and restore functions.
	size_t			m_iMaxTipSize;		 // Maximum size for tooltip string.
	CString			m_strToolTip;		 // Tooltip for the default icon.
	HICON			m_hIcon;			 // Default icon.
	NOTIFYICONDATA	m_NIData;			 // Tray icon structure.

public:
	//Create the tray icon
    virtual bool Create(LPCTSTR lpszCaption,
						CWnd* pParentWnd, UINT nIconID, 
						UINT uMenuID=0, UINT uDefMenuItemID=0,
						bool bDefMenuItemByPos=false);

	//set tooltip text
    bool SetTooltipText(LPCTSTR lpszTipText);
	bool SetTooltipText(UINT nTipText);

	//get tooltip text
	CString GetTooltipText() const;
	
	//restore default values
	void SetDefaultValues();

	//Set & Get callback message
	//callback message is responsible for mouse click of user
	bool SetCallbackMessage(UINT uNewCallbackMessage);
	UINT GetCallbackMessage();

	//Set default menu item when user right clicks on tray icon
	//the defualt menu is bold
	bool SetDefaultMenuItem(UINT uItem,bool bByPos);
	void GetDefaultMenuItem(UINT& uItem,bool& bByPos);

	//Set notification window - the message will route to this window
	bool SetNotificationWnd(CWnd* pWndNotify);
	CWnd* GetNotificationWnd();

	//set desired icon by icon handle
	bool SetIcon(HICON hIcon);

	//set desired icon in resource by it's resource id
	bool SetIcon(UINT nIDResource);

	HICON GetIcon() const;

	//add icon to tray
	bool AddIcon();
	
	//remove icon from tray
	bool RemoveIcon();

	//minimize selected window = minimize to tray
	void MinimizeToTray(CWnd* pWnd);

	//maximize the window from tray
	void MaximizeFromTray(CWnd* pWnd);

	//{{AFX_VIRTUAL(CTrayIcon)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
    virtual LRESULT OnTrayNotification(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

    virtual void InstallIconPending();
	virtual bool CreateMinimizeWnd(CWnd* pWndApp);

	//{{AFX_MSG(CTrayIcon)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	//}}AFX_MSG
    afx_msg LRESULT OnTaskbarCreated(WPARAM wParam, LPARAM lParam);

    DECLARE_MESSAGE_MAP()

public:

	virtual bool Create(LPCTSTR lpszCaption, DWORD dwStyle, CWnd* pParentWnd, UINT nIconID);
    virtual void SetTooltip(CString strToolTip);
    virtual void SetTrayIcon(UINT nIcon, DWORD dwMessage=NIM_ADD);
    
};

//////////////////////////////////////////////////////////////////////

AFX_INLINE bool CTrayIcon::Create(LPCTSTR lpszCaption, DWORD dwStyle, 
								  CWnd* pParentWnd, UINT nIconID) 
{
	UNREFERENCED_PARAMETER(dwStyle); return Create(lpszCaption, pParentWnd, nIconID);
}

AFX_INLINE void CTrayIcon::SetTooltip(CString strToolTip) 
{
	SetTooltipText(strToolTip);
}

AFX_INLINE void CTrayIcon::SetTrayIcon(UINT nIcon, DWORD dwMessage) 
{
	UNREFERENCED_PARAMETER(dwMessage); 
	SetIcon(nIcon);
}

//////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // _TRAY_ICON_H_
