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
// PageIntroduction.cpp : implementation file
//

#include "stdafx.h"
#include "PlutoVIP.h"
#include "PageIntroduction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPageIntroduction property page

IMPLEMENT_DYNCREATE(CPageIntroduction, CPropertyPage)

CPageIntroduction::CPageIntroduction() : CPropertyPage(CPageIntroduction::IDD)
{
	//{{AFX_DATA_INIT(CPageIntroduction)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CPageIntroduction::~CPageIntroduction()
{
}

void CPageIntroduction::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageIntroduction)
	DDX_Control(pDX, IDC_CREDIT, m_Credit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageIntroduction, CPropertyPage)
	//{{AFX_MSG_MAP(CPageIntroduction)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageIntroduction message handlers

BOOL CPageIntroduction::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	CString szCredits=_T(
		"<h2>PlutoVIP</h2><br><hr color=blue><br><br>"
		"<h3>Programming by:</h3>"		//Subtitle
		"<br>Cool guys<br><br>"
		"<h3>Special thanks to:</h3>"	//Subtitle
		"<br>A. Riazi<br>"
		"A. Riazi<br>"
		"A. Riazi<br>"
		"A. Riazi<br>"
		"A. Riazi<br>"
		"A. Riazi<br><br>"
		"<br>Copyright © 2004 PlutoVIP<br>"
		"<br>All right reserved.<br><br>"
		"<a href=www.plutovip.com>PlutoVIP.com</a>");
	
	m_Credit.SetWindowText(szCredits);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



void CPageIntroduction::SetGetVariables(BOOL bSet)
{

}
