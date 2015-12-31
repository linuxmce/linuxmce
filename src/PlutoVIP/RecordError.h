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
#if !defined(AFX_RECORDERROR_H__2FCEF671_E428_4CBA_8A49_D0EE6B813E40__INCLUDED_)
#define AFX_RECORDERROR_H__2FCEF671_E428_4CBA_8A49_D0EE6B813E40__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RecordError.h : header file
//

#include "PPHtmlStatic.h"
/////////////////////////////////////////////////////////////////////////////
// CRecordError dialog

class CRecordError : public CDialog
{
// Construction
public:
	CString szMACAddress;
	CRecordError(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRecordError)
	enum { IDD = IDD_RECORD_ERROR };
	CPPHtmlStatic	m_MACAddress;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRecordError)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRecordError)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RECORDERROR_H__2FCEF671_E428_4CBA_8A49_D0EE6B813E40__INCLUDED_)
