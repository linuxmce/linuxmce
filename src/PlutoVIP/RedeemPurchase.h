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
#if !defined(AFX_REDEEMPURCHASE_H__871EC16C_6B25_469F_98C7_3F68EC16628A__INCLUDED_)
#define AFX_REDEEMPURCHASE_H__871EC16C_6B25_469F_98C7_3F68EC16628A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RedeemPurchase.h : header file
//

#include "PPHtmlStatic.h"
/////////////////////////////////////////////////////////////////////////////
// CRedeemPurchase dialog

class CRedeemPurchase : public CDialog
{
// Construction
public:
	void SetPurchaseStatistics(double Total, double Available);
	CRedeemPurchase(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRedeemPurchase)
	enum { IDD = IDD_REDEEM_PURCHASE };
	CPPHtmlStatic	m_Purchase;
	double	m_Redeem;
	CString	m_Description;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRedeemPurchase)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	double AvailablePurchase;
	double TotalPurchase;

	// Generated message map functions
	//{{AFX_MSG(CRedeemPurchase)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REDEEMPURCHASE_H__871EC16C_6B25_469F_98C7_3F68EC16628A__INCLUDED_)
