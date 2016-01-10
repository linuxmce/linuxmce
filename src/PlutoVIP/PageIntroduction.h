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
#if !defined(AFX_PAGEINTRODUCTION_H__01A6E8E3_1700_4235_9916_0CD0B309E5A2__INCLUDED_)
#define AFX_PAGEINTRODUCTION_H__01A6E8E3_1700_4235_9916_0CD0B309E5A2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageIntroduction.h : header file
//

#include "PPHtmlStatic.h"
/////////////////////////////////////////////////////////////////////////////
// CPageIntroduction dialog

class CPageIntroduction : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageIntroduction)

// Construction
public:
	void SetGetVariables(BOOL bSet);
	CPageIntroduction();
	~CPageIntroduction();

// Dialog Data
	//{{AFX_DATA(CPageIntroduction)
	enum { IDD = IDD_PAGE_INTRODUCTION };
	CPPHtmlStatic m_Credit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageIntroduction)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPageIntroduction)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEINTRODUCTION_H__01A6E8E3_1700_4235_9916_0CD0B309E5A2__INCLUDED_)
