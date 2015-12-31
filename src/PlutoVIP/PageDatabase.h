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
#if !defined(AFX_PAGEDATABASE_H__CB1CEF70_BC9D_419A_A77E_3968DC4C2C81__INCLUDED_)
#define AFX_PAGEDATABASE_H__CB1CEF70_BC9D_419A_A77E_3968DC4C2C81__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageDatabase.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPageDatabase dialog

class CPageDatabase : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageDatabase)

// Construction
public:
	void SetGetVariables(BOOL bSet);
	CPageDatabase();
	~CPageDatabase();

// Dialog Data
	//{{AFX_DATA(CPageDatabase)
	enum { IDD = IDD_PAGE_DATABASE };
	CComboBox	m_WhichOne;
	CComboBox	m_Whatdo;
	CListBox	m_Information;
	BOOL	m_Copy;
	BOOL	m_Switch;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageDatabase)
	public:
	virtual BOOL OnApply();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPageDatabase)
	afx_msg void OnRemove();
	afx_msg void OnRename();
	afx_msg void OnAdd();
	virtual BOOL OnInitDialog();
	afx_msg void OnCopy();
	afx_msg void OnSwitch();
	afx_msg void OnSelChangeWhichOne();
	afx_msg void OnSelChangeWhatDo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEDATABASE_H__CB1CEF70_BC9D_419A_A77E_3968DC4C2C81__INCLUDED_)
