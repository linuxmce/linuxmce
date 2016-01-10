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
// RedeemVisit.cpp : implementation file
//

#include "stdafx.h"
#include "PlutoVIP.h"
#include "RedeemVisit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRedeemVisit dialog


CRedeemVisit::CRedeemVisit(CWnd* pParent /*=NULL*/)
	: CDialog(CRedeemVisit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRedeemVisit)
	m_Redeem = 0;
	m_Description = _T("");
	//}}AFX_DATA_INIT
}


void CRedeemVisit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRedeemVisit)
	DDX_Control(pDX, IDC_REDEEM_VISIT, m_Visit);
	DDX_Text(pDX, IDC_VISIT, m_Redeem);
	DDX_Text(pDX, IDC_REDEEM_VISIT_DESCRIPTION, m_Description);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRedeemVisit, CDialog)
	//{{AFX_MSG_MAP(CRedeemVisit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRedeemVisit message handlers

void CRedeemVisit::SetVisitStatistics(long Total, long Available)
{
	TotalVisit=Total;
	AvailableVisit=Available;
}

BOOL CRedeemVisit::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CString szVisit;
	szVisit.Format(_T("Total Visit: <b>%d</b><br>Available Visit: <b>%d</b>"), 
		TotalVisit, AvailableVisit);
	m_Visit.SetWindowText(szVisit);
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
