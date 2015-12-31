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
// PageInternet.cpp : implementation file
//

#include "stdafx.h"
#include "PlutoVIP.h"
#include "PageInternet.h"
#include "Registry.h"
#include "Ras.h"
#include "RasError.h"

//we used RAS API to enumerate internet connections
#pragma comment(lib, "Rasapi32.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPageInternet property page

IMPLEMENT_DYNCREATE(CPageInternet, CPropertyPage)

CPageInternet::CPageInternet() : CPropertyPage(CPageInternet::IDD)
{
	//{{AFX_DATA_INIT(CPageInternet)
	m_InternetConnection = 0;
	m_InternetWhen = 0;
	//}}AFX_DATA_INIT
}

CPageInternet::~CPageInternet()
{
}

void CPageInternet::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageInternet)
	DDX_Control(pDX, IDC_CONNECTION, m_DialupConnections);
	DDX_Radio(pDX, IDC_INTERNET_CONNECTION, m_InternetConnection);
	DDX_Radio(pDX, IDC_INTERNET_WHEN, m_InternetWhen);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageInternet, CPropertyPage)
	//{{AFX_MSG_MAP(CPageInternet)
	ON_BN_CLICKED(IDC_INTERNET_CONNECTION, OnInternetConnection)
	ON_BN_CLICKED(IDC_INTERNET_WHEN, OnInternetWhen)
	ON_BN_CLICKED(IDC_INTERNET_NO_ACCESS, OnInternetNoAccess)
	ON_BN_CLICKED(IDC_INTERNET_DIALUP, OnInternetDialup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageInternet message handlers

void CPageInternet::OnInternetConnection() 
{
	GetDlgItem(IDC_CONNECTION)->EnableWindow(FALSE);

	m_InternetConnection=0;
	UpdateData(FALSE);

	SetModified();
}

void CPageInternet::OnInternetWhen() 
{
	SetModified();
}

BOOL CPageInternet::OnApply() 
{
	SetGetVariables(TRUE);
	
	return CPropertyPage::OnApply();
}

void CPageInternet::OnOK() 
{
	SetGetVariables(TRUE);
	
	CPropertyPage::OnOK();
}

BOOL CPageInternet::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	SetGetVariables(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPageInternet::SetGetVariables(BOOL bSet)
{
	CRegistry registry;

	if (bSet)	//Save variables in registry
	{
		UpdateData(TRUE);
		registry.OpenKey(HKEY_CURRENT_USER, REGISTRY_KEY_VIPSERVICE_FULL);
		registry.SetValueDWORD(REGISTRY_PAGE_INTERNET_DIALUP_CONNECTIONS, m_DialupConnections.GetCurSel());
		CString ConnectionName;
		m_DialupConnections.GetWindowText(ConnectionName);
		registry.SetValueString(REGISTRY_PAGE_INTERNET_DIALUP_CONNECTIONS_NAME, ConnectionName);
		registry.SetValueDWORD(REGISTRY_PAGE_INTERNET_INTERNET_CONNECTION, m_InternetConnection);
		registry.SetValueDWORD(REGISTRY_PAGE_INTERNET_INTERNET_WHEN, m_InternetWhen);
		registry.CloseKey();		
	}
	else		//Retrieve variables value from registry
	{
		DWORD Item=0;
		CString ConnectionName;

		registry.OpenKey(HKEY_CURRENT_USER, REGISTRY_KEY_VIPSERVICE_FULL);
		Item=registry.GetValueDWORD(REGISTRY_PAGE_INTERNET_DIALUP_CONNECTIONS);
		ConnectionName=registry.GetValueString(REGISTRY_PAGE_INTERNET_DIALUP_CONNECTIONS_NAME);

		FillDialupConnectionsComboBox();
		if (Item<(DWORD) m_DialupConnections.GetCount())
		{
			CString ListName;
			m_DialupConnections.GetLBText(Item, ListName);
			if (ConnectionName.CompareNoCase(ListName)==0)
				m_DialupConnections.SetCurSel(Item);
		}
			

		m_InternetConnection=registry.GetValueDWORD(REGISTRY_PAGE_INTERNET_INTERNET_CONNECTION);
		
		if (m_InternetConnection==1)	//dialup connections
			OnInternetDialup();

		m_InternetWhen=registry.GetValueDWORD(REGISTRY_PAGE_INTERNET_INTERNET_WHEN);
		
		registry.CloseKey();		
		UpdateData(FALSE);
	}
}

void CPageInternet::FillDialupConnectionsComboBox()
{
	m_DialupConnections.ResetContent();

	//loop for adding all of dial up connections
	DWORD dwCb = sizeof(RASENTRYNAME);
    DWORD dwErr = ERROR_SUCCESS;
    DWORD dwRetries = 5;
    DWORD dwEntries = 0;
    RASENTRYNAME* lpRasEntryName = NULL;
	
    //
    // Loop through in case the information from RAS changes between calls.
    //
    while (dwRetries--)
    {
        //
        // If the memory is allocated, free it.
        //
        if (NULL != lpRasEntryName)
        {
            HeapFree(GetProcessHeap(), 0, lpRasEntryName);
            lpRasEntryName = NULL;
        }
        //
        // Allocate the size need for the RAS structure.
        //
        lpRasEntryName = (RASENTRYNAME*) HeapAlloc(GetProcessHeap(), 0, dwCb);
        if (NULL == lpRasEntryName)
        {
            dwErr = ERROR_NOT_ENOUGH_MEMORY;
            break;
        }
        //
        // Set the structure size for version checking purposes.
        //
        lpRasEntryName->dwSize = sizeof(RASENTRYNAME);
        //
        // Call the RAS API, bail on the loop if we are successful or an unknown
        // error occurs.
        //
        dwErr = RasEnumEntries(
			NULL,
			NULL,
			lpRasEntryName,
			&dwCb,
			&dwEntries);
        if (ERROR_BUFFER_TOO_SMALL != dwErr)
        {
            break;
        }
    }
    //
    // In the success case print the names of the entries.
    //
    if (ERROR_SUCCESS == dwErr)
    {
        DWORD i;
		
        for (i = 0; i < dwEntries; i++)
        {
            m_DialupConnections.AddString(lpRasEntryName[i].szEntryName);
        }
    }
    
    //
    // Free the memory if necessary.
    //
    if (NULL != lpRasEntryName)
    {
        HeapFree(GetProcessHeap(), 0, lpRasEntryName);
        lpRasEntryName = NULL;
    }
}

void CPageInternet::OnInternetNoAccess() 
{
	GetDlgItem(IDC_CONNECTION)->EnableWindow(FALSE);

	m_InternetConnection=2;
	UpdateData(FALSE);

	SetModified();
}

void CPageInternet::OnInternetDialup() 
{
	GetDlgItem(IDC_CONNECTION)->EnableWindow(TRUE);	

	m_InternetConnection=1;
	UpdateData(FALSE);

	SetModified();
}
