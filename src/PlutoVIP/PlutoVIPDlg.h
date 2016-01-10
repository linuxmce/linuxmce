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
// TaskBarDlg.h : header file
//

#ifndef __TASKBARDLG_H__
#define __TASKBARDLG_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Controls.h"

/////////////////////////////////////////////////////////////////////////////
// CTaskBarDlg dialog
class CTaskBarDlg : public CDialog
{
public:
	CTaskBarDlg(CWnd* pParent = NULL);	// standard constructor

	//{{AFX_DATA(CTaskBarDlg)
	enum { IDD = IDD_TASKBAR_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTaskBarDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTaskBarDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnRegister();
	afx_msg void OnDestroy();
	afx_msg void OnUnregister();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
        
private:
    CControlsDlg *m_pControlsDlg;
	//CControlsDlg* m_a;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TASKBARDLG_H__9089251F_A8C8_11D1_B257_006097960BB7__INCLUDED_)
