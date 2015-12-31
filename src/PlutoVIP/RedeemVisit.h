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
#if !defined(AFX_REDEEMVISIT_H__6D2DA844_D0AD_4A59_B5F5_BA323A47A8BB__INCLUDED_)
#define AFX_REDEEMVISIT_H__6D2DA844_D0AD_4A59_B5F5_BA323A47A8BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RedeemVisit.h : header file
//

#include "PPHtmlStatic.h"
/////////////////////////////////////////////////////////////////////////////
// CRedeemVisit dialog

class CRedeemVisit : public CDialog
{
// Construction
public:
	void SetVisitStatistics(long Total, long Available);
	CRedeemVisit(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRedeemVisit)
	enum { IDD = IDD_REDEEM_VISIT };
	CPPHtmlStatic	m_Visit;
	int		m_Redeem;
	CString	m_Description;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRedeemVisit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	long AvailableVisit;
	long TotalVisit;

	// Generated message map functions
	//{{AFX_MSG(CRedeemVisit)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REDEEMVISIT_H__6D2DA844_D0AD_4A59_B5F5_BA323A47A8BB__INCLUDED_)
