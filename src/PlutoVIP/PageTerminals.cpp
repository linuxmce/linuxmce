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
// PageTerminals.cpp : implementation file
//

#include "stdafx.h"
#include "PlutoVIP.h"
#include "PageTerminals.h"
#include "Registry.h"
#include "SBDestination.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "XSBrowseFolder.h"
#include "shlobj.h"
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CPageTerminals property page

IMPLEMENT_DYNCREATE(CPageTerminals, CPropertyPage)

CPageTerminals::CPageTerminals() : CPropertyPage(CPageTerminals::IDD)
{
	//{{AFX_DATA_INIT(CPageTerminals)
	m_Client = FALSE;
	m_Directory = _T("");
	m_Server = _T("");
	m_OtherTerminals = 0;
	//}}AFX_DATA_INIT
}

CPageTerminals::~CPageTerminals()
{
}

void CPageTerminals::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageTerminals)
	DDX_Check(pDX, IDC_CLIENT, m_Client);
	DDX_Text(pDX, IDC_DIRECTORY, m_Directory);
	DDX_Text(pDX, IDC_SERVER, m_Server);
	DDX_Radio(pDX, IDC_OTHER_TERMINALS, m_OtherTerminals);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageTerminals, CPropertyPage)
	//{{AFX_MSG_MAP(CPageTerminals)
	ON_BN_CLICKED(IDC_OTHER_TERMINALS, OnOtherTerminals)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_FIND, OnFind)
	ON_BN_CLICKED(IDC_OTHER_TERMINALS2, OnOtherTerminals2)
	ON_BN_CLICKED(IDC_CLIENT, OnClient)
	ON_EN_CHANGE(IDC_DIRECTORY, OnChangeDirectory)
	ON_EN_CHANGE(IDC_SERVER, OnChangeServer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageTerminals message handlers

void CPageTerminals::OnOtherTerminals() 
{
	m_OtherTerminals=0;
	UpdateData(FALSE);

	EnableControls(FALSE);

	SetModified();
}

void CPageTerminals::EnableControls(BOOL bEnable)
{
	GetDlgItem(IDC_SERVER)->EnableWindow(bEnable);
	GetDlgItem(IDC_FIND)->EnableWindow(bEnable);
	GetDlgItem(IDC_DIRECTORY)->EnableWindow(bEnable);
	GetDlgItem(IDC_BROWSE)->EnableWindow(bEnable);
	GetDlgItem(IDC_CLIENT)->EnableWindow(bEnable);
}


void CPageTerminals::OnBrowse() 
{
	CXSBrowseFolder bf;
	bf.SetTitle(_T("Select desired folder:"));
	//bf.ModifyStyle(BIF_NEWDIALOGSTYLE, 0);
	
	UpdateData(TRUE);
	
	TCHAR path[MAX_PATH];
	_tcscpy(path, m_Directory.GetBuffer(0));
	
	// Display the dalog and check the return code
	if (bf.Show(GetSafeHwnd(), path)==CXSBrowseFolder::RET_OK) 
	{
		m_Directory=path;
		UpdateData(FALSE);
	}	
}

void CPageTerminals::OnFind() 
{
	CSBDestination Computers;
	Computers.SetFlags(BIF_BROWSEFORCOMPUTER);
	Computers.SetTitle("Select desired computer:");
	Computers.SetOwner(m_hWnd);
	LPITEMIDLIST ItemID=NULL;
	SHGetSpecialFolderLocation(m_hWnd,CSIDL_NETWORK,&ItemID);
	Computers.SetRoot(ItemID);
	
	if (Computers.SelectNetworkComputer())
	{
		CString ComputerName=Computers.GetComputerName();
		ComputerName.Replace("\\\\","");
		m_Server=ComputerName;
		UpdateData(FALSE);		
	}
}

void CPageTerminals::OnOtherTerminals2() 
{
	m_OtherTerminals=1;
	UpdateData(FALSE);

	EnableControls(TRUE);

	SetModified();
}

BOOL CPageTerminals::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	SetGetVariables(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CPageTerminals::OnApply() 
{
	SetGetVariables(TRUE);
	
	return CPropertyPage::OnApply();
}

void CPageTerminals::OnOK() 
{
	SetGetVariables(TRUE);
	
	CPropertyPage::OnOK();
}

void CPageTerminals::SetGetVariables(BOOL bSet)
{
	CRegistry registry;

	if (bSet)	//Save variables in registry
	{
		UpdateData(TRUE);
		registry.OpenKey(HKEY_CURRENT_USER, REGISTRY_KEY_VIPSERVICE_FULL);
		registry.SetValueDWORD(REGISTRY_PAGE_TERMINALS_TERMINALS, m_OtherTerminals);
		registry.SetValueString(REGISTRY_PAGE_TERMINALS_DIRECTORY, m_Directory.GetBuffer(0));
		registry.SetValueString(REGISTRY_PAGE_TERMINALS_SERVER, m_Server.GetBuffer(0));
		registry.SetValueDWORD(REGISTRY_PAGE_TERMINALS_CLIENT, m_Client);
		registry.CloseKey();		
	}
	else		//Retrieve variables value from registry
	{
		registry.OpenKey(HKEY_CURRENT_USER, REGISTRY_KEY_VIPSERVICE_FULL);
		m_OtherTerminals=registry.GetValueDWORD(REGISTRY_PAGE_TERMINALS_TERMINALS);
		m_Directory=registry.GetValueString(REGISTRY_PAGE_TERMINALS_DIRECTORY);
		m_Server=registry.GetValueString(REGISTRY_PAGE_TERMINALS_SERVER);
		m_Client=registry.GetValueDWORD(REGISTRY_PAGE_TERMINALS_CLIENT);
		registry.CloseKey();		

		if (m_OtherTerminals)
			OnOtherTerminals2();
		
		UpdateData(FALSE);
	}
}



void CPageTerminals::OnClient() 
{
	SetModified();	
}

void CPageTerminals::OnChangeDirectory() 
{
	SetModified();
}

void CPageTerminals::OnChangeServer() 
{
	SetModified();
}
