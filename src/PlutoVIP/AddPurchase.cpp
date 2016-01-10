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
// AddPurchase.cpp : implementation file
//

#include "stdafx.h"
#include "PlutoVIP.h"
#include "AddPurchase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddPurchase dialog


CAddPurchase::CAddPurchase(CWnd* pParent /*=NULL*/)
	: CDialog(CAddPurchase::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddPurchase)
	m_Purchase = 0.0;
	//}}AFX_DATA_INIT
}


void CAddPurchase::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddPurchase)
	DDX_Text(pDX, IDC_PURCHASE, m_Purchase);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddPurchase, CDialog)
	//{{AFX_MSG_MAP(CAddPurchase)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddPurchase message handlers
