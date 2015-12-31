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
// Picture_DlgDlg.h : header file
//

#if !defined(AFX_PICTURE_DLGDLG_H__BFEF8E6F_1E56_422B_AD56_FEF31CFD8F68__INCLUDED_)
#define AFX_PICTURE_DLGDLG_H__BFEF8E6F_1E56_422B_AD56_FEF31CFD8F68__INCLUDED_

#include "Common/Picture.h"	// Added by ClassView
#include "resource.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CHighlightedCustomer dialog
class Customer;

class CHighlightedCustomer : public CDialog
{
// Construction
public:
	CPicture m_Picture;
	CHighlightedCustomer(Customer *pCustomer);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CHighlightedCustomer)
	enum { IDD = IDD_HIGHLIGHT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHighlightedCustomer)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
    Customer *m_pCustomer;
	// Generated message map functions
	//{{AFX_MSG(CHighlightedCustomer)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PICTURE_DLGDLG_H__BFEF8E6F_1E56_422B_AD56_FEF31CFD8F68__INCLUDED_)
