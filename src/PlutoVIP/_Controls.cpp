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
// Controls.cpp : implementation file
//

#include "stdafx.h"
#include "TaskBar.h"
#include "Controls.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CControlsDlg dialog


CControlsDlg::CControlsDlg(CWnd* pParent /*=NULL*/)
	: CAppBarDlg(CControlsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CControlsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CControlsDlg::DoDataExchange(CDataExchange* pDX)
{
	CAppBarDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CControlsDlg)
	DDX_Control(pDX, IDC_CUSTOMER, m_Customer);
	//}}AFX_DATA_MAP
	//DDX_Control(pDX, IDC_START, m_btnStart);
}


BEGIN_MESSAGE_MAP(CControlsDlg, CAppBarDlg)
	//{{AFX_MSG_MAP(CControlsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CControlsDlg message handlers
/*
void CControlsDlg::OnStart() 
{
    CRect rect;
    m_btnStart.GetWindowRect (&rect);
//    ShowMenu (rect, IDR_EPIC_BUTTON_MENU, -1);
}
*/


BOOL CControlsDlg::OnInitDialog() 
{
	CAppBarDlg::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CControlsDlg::SetupButtons()
{
	int Position=0;
	map<unsigned long,class Customer *>::iterator itCustomers;
	for (itCustomers=m_pDetection->m_mapCustomers.begin();
		 itCustomers!=m_pDetection->m_mapCustomers.end();
		 ++itCustomers)
	{
		Customer *pCustomer = (*itCustomers).second;
		if( !pCustomer || pCustomer->m_cAppBar!=m_cAppBar )
			continue;
		
			/* ARMEN: Something like this
			CButton *pButton = new CButton();
			pButton->SetPosition(m_iXOffset*Position,m_iYOffset*Position);
			pButton->SetID(pCustomer->m_iPlutoId);  // Some way to retrieve an unsigned long when the mouse is over it
			pButton->AddToAppBar(this);
			
		*/
	}
}
