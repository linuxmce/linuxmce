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
#if !defined(AFX_PAGEINTERNET_H__03111A14_EE08_465D_97DE_65EE98876E0A__INCLUDED_)
#define AFX_PAGEINTERNET_H__03111A14_EE08_465D_97DE_65EE98876E0A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageInternet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPageInternet dialog

class CPageInternet : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageInternet)

// Construction
public:
	void FillDialupConnectionsComboBox();
	void SetGetVariables(BOOL bSet);
	CPageInternet();
	~CPageInternet();

// Dialog Data
	//{{AFX_DATA(CPageInternet)
	enum { IDD = IDD_PAGE_INTERNET };
	CComboBox	m_DialupConnections;
	int		m_InternetConnection;
	int		m_InternetWhen;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageInternet)
	public:
	virtual BOOL OnApply();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPageInternet)
	afx_msg void OnInternetConnection();
	afx_msg void OnInternetWhen();
	virtual BOOL OnInitDialog();
	afx_msg void OnInternetNoAccess();
	afx_msg void OnInternetDialup();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEINTERNET_H__03111A14_EE08_465D_97DE_65EE98876E0A__INCLUDED_)
