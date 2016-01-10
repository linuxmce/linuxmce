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
#if !defined(AFX_ADDPURCHASE_H__06F935A5_9C5B_40BE_8363_DAC19BBC13E4__INCLUDED_)
#define AFX_ADDPURCHASE_H__06F935A5_9C5B_40BE_8363_DAC19BBC13E4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddPurchase.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAddPurchase dialog

class CAddPurchase : public CDialog
{
// Construction
public:
	CAddPurchase(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAddPurchase)
	enum { IDD = IDD_ADD_PURCHASE };
	double	m_Purchase;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddPurchase)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddPurchase)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDPURCHASE_H__06F935A5_9C5B_40BE_8363_DAC19BBC13E4__INCLUDED_)
