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
// PlutoVIPMain.h : header file
//

#ifndef __TASKBARDLG_H__
#define __TASKBARDLG_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ControlsDlg.h"
//#include "PhoneDetection_VIP.h"
/////////////////////////////////////////////////////////////////////////////

//alias

// CPlutoVIPMain dialog
class CPlutoVIPMain : public CDialog
{
public:
	void RefreshAppBars();
	void MinimizeParent();
	BOOL CheckInternetConnection();
	void ProgressUpdate ( LPCTSTR szIEMsg, LPCTSTR szCustomMsg, const int nPercentDone );
	void DetectPhones();
	void InitializeTrayIcon();
	void UnregisterAppBars();
	void RegisterAppBars();
	CPlutoVIPMain(CWnd* pParent = NULL);	// standard constructor
	virtual ~CPlutoVIPMain();
	//{{AFX_DATA(CPlutoVIPMain)
	enum { IDD = IDD_PLUTOVIP_MAIN_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlutoVIPMain)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	/*CTrayIcon*/ NOTIFYICONDATA m_TrayIcon;
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CPlutoVIPMain)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHelpAbout();
	afx_msg void OnHelpContents();
	afx_msg void OnHelpPlutoVIPFAQ();
	afx_msg void OnHelpPlutoVIPFreeStuff();
	afx_msg void OnHelpPlutoVIPOnlineSupport();
	afx_msg void OnHelpPlutoviponthewebPlutoVIPHomepage();
	afx_msg void OnHelpPlutoVIPProductNews();
	afx_msg void OnHelpTipOfTheDay();
	afx_msg void OnExit();
	afx_msg void OnPreferences();
	afx_msg void OnTrayAbout();
	afx_msg void OnTrayExit();
	afx_msg void OnTrayPreferences();
	afx_msg void OnTrayRestore();
	afx_msg void OnCheckUpdateVersion();
	afx_msg void OnMinimized();
	//}}AFX_MSG
	virtual LRESULT OnTrayNotification(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
        
private:
	PhoneDetection_VIP* m_pPhoneDetection_VIP;

	//ITaskbarList* pTaskbar;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TASKBARDLG_H__9089251F_A8C8_11D1_B257_006097960BB7__INCLUDED_)
