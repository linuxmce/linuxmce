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
// VIPPropertiesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VIPShared/VIPIncludes.h"
#include "VIPShared/VIPMenu.h"
#include "PlutoUtils/StringUtils.h"

#include "VIPDesign.h"
#include "VIPPropertiesDlg.h"


// CVIPPropertiesDlg dialog

IMPLEMENT_DYNAMIC(CVIPPropertiesDlg, CDialog)
CVIPPropertiesDlg::CVIPPropertiesDlg(class VIPMenuCollection *pMenuCollection)
	: CDialog(CVIPPropertiesDlg::IDD, NULL)
	, m_Width(_T(""))
	, m_Height(_T(""))
	, m_CollectionID(_T(""))
{
	m_pMenuCollection=pMenuCollection;
}

CVIPPropertiesDlg::~CVIPPropertiesDlg()
{
}

void CVIPPropertiesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PROP_WIDTH, m_Width);
	DDX_Text(pDX, IDC_PROP_HEIGHT, m_Height);
	DDX_Control(pDX, IDC_PROP_STARTINGMENU, m_StartingMenu);
	DDX_Text(pDX, IDC_PROP_COLLECTIONID, m_CollectionID);
}


BEGIN_MESSAGE_MAP(CVIPPropertiesDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CVIPPropertiesDlg message handlers
BOOL CVIPPropertiesDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_Width=StringUtils::itos(m_pMenuCollection->m_iWidth).c_str();
	m_Height=StringUtils::itos(m_pMenuCollection->m_iHeight).c_str();
	m_CollectionID=StringUtils::itos(m_pMenuCollection->m_iMenuCollectionID).c_str();

	MYSTL_ITERATE_LONG_MAP(m_pMenuCollection->m_mapMenus,VIPMenu,pMenu,i)
	{
		int Value=m_StartingMenu.AddString(pMenu->m_sDescription.c_str());
		m_StartingMenu.SetItemData(Value,pMenu->m_iMenuNumber);
		if( pMenu==m_pMenuCollection->m_pMenu_Starting )
			m_StartingMenu.SetCurSel(Value);
	}

	//default values
	m_StartingMenu.SelectString(-1, "Default");
	m_Width = "208";
	m_Height = "176";

	UpdateData(FALSE);
	return TRUE;
}

void CVIPPropertiesDlg::OnBnClickedOk()
{
	OnOK();
	UpdateData(TRUE);
	m_pMenuCollection->m_iWidth = atoi(m_Width);
	m_pMenuCollection->m_iHeight = atoi(m_Height);
	m_pMenuCollection->m_iMenuCollectionID = atoi(m_CollectionID);
	MYSTL_GET_AT_LONG_MAP(m_pMenuCollection->m_mapMenus,VIPMenu,pMenu,m_StartingMenu.GetItemData(m_StartingMenu.GetCurSel()));
	m_pMenuCollection->m_pMenu_Starting=pMenu;
}
