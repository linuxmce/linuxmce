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
// VIPMenuDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VIPShared/VIPIncludes.h"
#include "VIPShared/VIPMenu.h"

#include "VIPDesign.h"
#include "VIPMenuDlg.h"


// CVIPMenuDlg dialog

IMPLEMENT_DYNAMIC(CVIPMenuDlg, CDialog)
CVIPMenuDlg::CVIPMenuDlg(class VIPMenu *pMenu)
	: CDialog(CVIPMenuDlg::IDD, NULL)
	, m_Description(_T(""))
{
	m_pMenu=pMenu;
}

CVIPMenuDlg::~CVIPMenuDlg()
{
}

void CVIPMenuDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_MENU_DESCRIPTION, m_Description);
}


BEGIN_MESSAGE_MAP(CVIPMenuDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

BOOL CVIPMenuDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_Description = m_pMenu->m_sDescription.c_str();
	UpdateData(FALSE);

	return TRUE;
}


// CVIPMenuDlg message handlers

void CVIPMenuDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();

	m_pMenu->m_sDescription=m_Description;
}
