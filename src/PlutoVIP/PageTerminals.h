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
#if !defined(AFX_PAGETERMINALS_H__24EB67E9_F7F1_4008_9011_DFFF4B1A34D4__INCLUDED_)
#define AFX_PAGETERMINALS_H__24EB67E9_F7F1_4008_9011_DFFF4B1A34D4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageTerminals.h : header file
//


#define ANSWER_NO		0
#define ANSWER_YES		1
/////////////////////////////////////////////////////////////////////////////
// CPageTerminals dialog

class CPageTerminals : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageTerminals)

// Construction
public:
	void SetGetVariables(BOOL bSet);
	CPageTerminals();
	~CPageTerminals();

// Dialog Data
	//{{AFX_DATA(CPageTerminals)
	enum { IDD = IDD_PAGE_TERMINALS };
	BOOL	m_Client;
	CString	m_Directory;
	CString	m_Server;
	int		m_OtherTerminals;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageTerminals)
	public:
	virtual BOOL OnApply();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPageTerminals)
	afx_msg void OnOtherTerminals();
	afx_msg void OnBrowse();
	afx_msg void OnFind();
	afx_msg void OnOtherTerminals2();
	virtual BOOL OnInitDialog();
	afx_msg void OnClient();
	afx_msg void OnChangeDirectory();
	afx_msg void OnChangeServer();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void EnableControls(BOOL bEnable);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGETERMINALS_H__24EB67E9_F7F1_4008_9011_DFFF4B1A34D4__INCLUDED_)
