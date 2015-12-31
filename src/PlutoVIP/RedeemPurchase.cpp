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
// RedeemPurchase.cpp : implementation file
//

#include "stdafx.h"
#include "PlutoVIP.h"
#include "RedeemPurchase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRedeemPurchase dialog


CRedeemPurchase::CRedeemPurchase(CWnd* pParent /*=NULL*/)
	: CDialog(CRedeemPurchase::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRedeemPurchase)
	m_Redeem = 0.0;
	m_Description = _T("");
	//}}AFX_DATA_INIT
}


void CRedeemPurchase::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRedeemPurchase)
	DDX_Control(pDX, IDC_REDEEM_PURCHASE, m_Purchase);
	DDX_Text(pDX, IDC_PURCHASE, m_Redeem);
	DDX_Text(pDX, IDC_REDEEM_PURCHASE_DESCRIPTION, m_Description);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRedeemPurchase, CDialog)
	//{{AFX_MSG_MAP(CRedeemPurchase)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRedeemPurchase message handlers

void CRedeemPurchase::SetPurchaseStatistics(double Total, double Available)
{
	TotalPurchase=Total;
	AvailablePurchase=Available;
}

BOOL CRedeemPurchase::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CString szPurchase;
	szPurchase.Format(_T("Total Purchase: <b>%.2f</b><br>Available Purchase: <b>%.2f</b>"), 
		TotalPurchase, AvailablePurchase);
	m_Purchase.SetWindowText(szPurchase);
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
