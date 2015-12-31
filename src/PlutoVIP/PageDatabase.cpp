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
// PageDatabase.cpp : implementation file
//

#include "stdafx.h"
#include "PlutoVIP.h"
#include "PageDatabase.h"
#include "Registry.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPageDatabase property page

IMPLEMENT_DYNCREATE(CPageDatabase, CPropertyPage)

CPageDatabase::CPageDatabase() : CPropertyPage(CPageDatabase::IDD)
{
	//{{AFX_DATA_INIT(CPageDatabase)
	m_Copy = FALSE;
	m_Switch = FALSE;
	//}}AFX_DATA_INIT
}

CPageDatabase::~CPageDatabase()
{
}

void CPageDatabase::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageDatabase)
	DDX_Control(pDX, IDC_WHICH_ONE, m_WhichOne);
	DDX_Control(pDX, IDC_WHAT_DO, m_Whatdo);
	DDX_Control(pDX, IDC_INFORMATTION, m_Information);
	DDX_Check(pDX, IDC_COPY, m_Copy);
	DDX_Check(pDX, IDC_SWITCH, m_Switch);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageDatabase, CPropertyPage)
	//{{AFX_MSG_MAP(CPageDatabase)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_RENAME, OnRename)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_COPY, OnCopy)
	ON_BN_CLICKED(IDC_SWITCH, OnSwitch)
	ON_CBN_SELCHANGE(IDC_WHICH_ONE, OnSelChangeWhichOne)
	ON_CBN_SELCHANGE(IDC_WHAT_DO, OnSelChangeWhatDo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageDatabase message handlers

void CPageDatabase::OnRemove() 
{
	// TODO: Add your control notification handler code here
	
}

void CPageDatabase::OnRename() 
{
	// TODO: Add your control notification handler code here
	
}

void CPageDatabase::OnAdd() 
{
	// TODO: Add your control notification handler code here
	
}

BOOL CPageDatabase::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	//read from registry
	SetGetVariables(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CPageDatabase::OnApply() 
{
	SetGetVariables(TRUE);
	
	return CPropertyPage::OnApply();
}

void CPageDatabase::OnOK() 
{
	SetGetVariables(TRUE);
	
	CPropertyPage::OnOK();
}

void CPageDatabase::SetGetVariables(BOOL bSet)
{
	CRegistry registry;

	if (bSet)	//Save variables in registry
	{
		UpdateData(TRUE);
		registry.OpenKey(HKEY_CURRENT_USER, REGISTRY_KEY_VIPSERVICE_FULL);
		registry.SetValueDWORD(REGISTRY_PAGE_DATABASE_COPY, m_Copy);
		registry.SetValueDWORD(REGISTRY_PAGE_DATABASE_SWITCH, m_Switch);
		registry.SetValueDWORD(REGISTRY_PAGE_DATABASE_WHAT_DO, m_Whatdo.GetCurSel());
		registry.SetValueDWORD(REGISTRY_PAGE_DATABASE_WHICH_ONE, m_WhichOne.GetCurSel());
		registry.CloseKey();		
	}
	else		//Retrieve variables value from registry
	{
		DWORD Item=0;

		registry.OpenKey(HKEY_CURRENT_USER, REGISTRY_KEY_VIPSERVICE_FULL);
		m_Copy=registry.GetValueDWORD(REGISTRY_PAGE_DATABASE_COPY);
		m_Switch=registry.GetValueDWORD(REGISTRY_PAGE_DATABASE_SWITCH);
		
		Item=registry.GetValueDWORD(REGISTRY_PAGE_DATABASE_WHAT_DO);
		m_Whatdo.SetCurSel(Item);

		Item=registry.GetValueDWORD(REGISTRY_PAGE_DATABASE_WHICH_ONE);
		m_WhichOne.SetCurSel(Item);

		registry.CloseKey();		
		UpdateData(FALSE);
	}
}


void CPageDatabase::OnCopy() 
{
	SetModified();
}

void CPageDatabase::OnSwitch() 
{
	SetModified();
}

void CPageDatabase::OnSelChangeWhichOne() 
{
	SetModified();
}

void CPageDatabase::OnSelChangeWhatDo() 
{
	SetModified();
}
