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
// PagePanelPosition.cpp : implementation file
//

#include "stdafx.h"
#include "PlutoVIP.h"
#include "PagePanelPosition.h"
#include "Registry.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CPagePanelPosition property page

IMPLEMENT_DYNCREATE(CPagePanelPosition, CPropertyPage)

CPagePanelPosition::CPagePanelPosition() : CPropertyPage(CPagePanelPosition::IDD)
{
	//{{AFX_DATA_INIT(CPagePanelPosition)
	m_NewTime = 5;
	m_RecentTime = 5;
	m_NewAppearance = 0;
	m_ActiveAppearance = 0;
	m_RecentAppearance = 0;
	//}}AFX_DATA_INIT
}

CPagePanelPosition::~CPagePanelPosition()
{
}

void CPagePanelPosition::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPagePanelPosition)
	DDX_Control(pDX, IDC_RECENT_CLIENTS_SIDE, m_RecentSide);
	DDX_Control(pDX, IDC_NEW_CLIENTS_SIDE, m_NewSide);
	DDX_Control(pDX, IDC_ACTIVE_CLIENTS_SIDE, m_ActiveSide);
	DDX_Text(pDX, IDC_CLIENT_NEW_TIME, m_NewTime);
	DDV_MinMaxInt(pDX, m_NewTime, 0, 60);
	DDX_Text(pDX, IDC_CLIENT_RECENT_TIME, m_RecentTime);
	DDV_MinMaxInt(pDX, m_RecentTime, 1, 60);
	DDX_Radio(pDX, IDC_NEW_APPEARANCE_VISIBLE, m_NewAppearance);
	DDX_Radio(pDX, IDC_ACTIVE_APPEARANCE_VISIBLE, m_ActiveAppearance);
	DDX_Radio(pDX, IDC_RECENT_APPEARANCE_VISIBLE, m_RecentAppearance);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPagePanelPosition, CPropertyPage)
	//{{AFX_MSG_MAP(CPagePanelPosition)
	ON_BN_CLICKED(IDC_NEW_APPEARANCE_VISIBLE, OnNewAppearanceVisible)
	ON_BN_CLICKED(IDC_NEW_APPEARANCE_INVISIBLE, OnNewAppearanceInvisible)
	ON_BN_CLICKED(IDC_NEW_APPEARANCE_AUTOHIDE, OnNewAppearanceAutohide)
	ON_BN_CLICKED(IDC_ACTIVE_APPEARANCE_VISIBLE, OnActiveAppearanceVisible)
	ON_BN_CLICKED(IDC_ACTIVE_APPEARANCE_AUTOHIDE, OnActiveAppearanceAutohide)
	ON_BN_CLICKED(IDC_ACTIVE_APPEARANCE_INVISIBLE, OnActiveAppearanceInvisible)
	ON_BN_CLICKED(IDC_RECENT_APPEARANCE_VISIBLE, OnRecentAppearanceVisible)
	ON_BN_CLICKED(IDC_RECENT_APPEARANCE_INVISIBLE, OnRecentAppearanceInvisible)
	ON_BN_CLICKED(IDC_RECENT_APPEARANCE_AUTOHIDE, OnRecentAppearanceAutohide)
	ON_CBN_SELCHANGE(IDC_NEW_CLIENTS_SIDE, OnSelChangeNewClientsSide)
	ON_CBN_SELCHANGE(IDC_RECENT_CLIENTS_SIDE, OnSelChangeRecentClientsSide)
	ON_EN_CHANGE(IDC_CLIENT_NEW_TIME, OnChangeClientNewTime)
	ON_EN_CHANGE(IDC_CLIENT_RECENT_TIME, OnChangeClientRecentTime)
	ON_CBN_SELCHANGE(IDC_ACTIVE_CLIENTS_SIDE, OnSelChangeActiveClientsSide)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPagePanelPosition message handlers

BOOL CPagePanelPosition::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	//read from registry
	SetGetVariables(FALSE);


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CPagePanelPosition::OnNewAppearanceVisible() 
{
	m_NewAppearance=0;
	UpdateData(FALSE);

	SetModified();
}

void CPagePanelPosition::OnNewAppearanceInvisible() 
{
	m_NewAppearance=2;
	UpdateData(FALSE);

	SetModified();
}

void CPagePanelPosition::OnNewAppearanceAutohide() 
{
	m_NewAppearance=1;
	UpdateData(FALSE);

	SetModified();
}

void CPagePanelPosition::OnActiveAppearanceVisible() 
{
	m_ActiveAppearance=0;
	UpdateData(FALSE);

	SetModified();
}

void CPagePanelPosition::OnActiveAppearanceAutohide() 
{
	m_ActiveAppearance=1;
	UpdateData(FALSE);

	SetModified();
}

void CPagePanelPosition::OnActiveAppearanceInvisible() 
{
	m_ActiveAppearance=2;
	UpdateData(FALSE);

	SetModified();
}

void CPagePanelPosition::OnRecentAppearanceVisible() 
{
	m_RecentAppearance=0;
	UpdateData(FALSE);

	SetModified();
}

void CPagePanelPosition::OnRecentAppearanceInvisible() 
{
	m_RecentAppearance=2;
	UpdateData(FALSE);

	SetModified();
}

void CPagePanelPosition::OnRecentAppearanceAutohide() 
{
	m_RecentAppearance=1;
	UpdateData(FALSE);

	SetModified();
}

void CPagePanelPosition::OnOK() 
{
	SetGetVariables(TRUE)	;

	CPropertyPage::OnOK();
}

BOOL CPagePanelPosition::OnApply() 
{
	SetGetVariables(TRUE);
	
	return CPropertyPage::OnApply();
}

void CPagePanelPosition::SetGetVariables(BOOL bSet)
{
	CRegistry registry;

	if (bSet)	//Save variables in registry
	{
		UpdateData(TRUE);
		registry.OpenKey(HKEY_CURRENT_USER, REGISTRY_KEY_VIPSERVICE_FULL);
		registry.SetValueDWORD(REGISTRY_PAGE_PANEL_POSITION_ACTIVE_APPEARANCE, m_ActiveAppearance);
		registry.SetValueDWORD(REGISTRY_PAGE_PANEL_POSITION_RECENT_APPEARANCE, m_RecentAppearance);
		registry.SetValueDWORD(REGISTRY_PAGE_PANEL_POSITION_NEW_APPEARANCE, m_NewAppearance);
		registry.SetValueDWORD(REGISTRY_PAGE_PANEL_POSITION_NEW_TIME, m_NewTime);
		registry.SetValueDWORD(REGISTRY_PAGE_PANEL_POSITION_RECENT_TIME, m_RecentTime);
		registry.SetValueDWORD(REGISTRY_PAGE_PANEL_POSITION_NEW_SIDE, m_NewSide.GetCurSel());
		registry.SetValueDWORD(REGISTRY_PAGE_PANEL_POSITION_ACTIVE_SIDE, m_ActiveSide.GetCurSel());
		registry.SetValueDWORD(REGISTRY_PAGE_PANEL_POSITION_RECENT_SIDE, m_RecentSide.GetCurSel());
		registry.CloseKey();		
	}
	else		//Retrieve variables value from registry
	{
		DWORD Side=0;

		registry.OpenKey(HKEY_CURRENT_USER, REGISTRY_KEY_VIPSERVICE_FULL);
		m_ActiveAppearance=registry.GetValueDWORD(REGISTRY_PAGE_PANEL_POSITION_ACTIVE_APPEARANCE);
		m_RecentAppearance=registry.GetValueDWORD(REGISTRY_PAGE_PANEL_POSITION_RECENT_APPEARANCE);
		m_NewAppearance=registry.GetValueDWORD(REGISTRY_PAGE_PANEL_POSITION_NEW_APPEARANCE);
		m_NewTime=registry.GetValueDWORD(REGISTRY_PAGE_PANEL_POSITION_NEW_TIME);
		m_RecentTime=registry.GetValueDWORD(REGISTRY_PAGE_PANEL_POSITION_RECENT_TIME);
		
		Side=registry.GetValueDWORD(REGISTRY_PAGE_PANEL_POSITION_NEW_SIDE);
		//if ((Side<4) && (Side>-1))
			m_NewSide.SetCurSel(Side);

		Side=registry.GetValueDWORD(REGISTRY_PAGE_PANEL_POSITION_ACTIVE_SIDE);
		//if ((Side<4 && Side>-1))
			m_ActiveSide.SetCurSel(Side);

		Side=registry.GetValueDWORD(REGISTRY_PAGE_PANEL_POSITION_RECENT_SIDE);
		//if ((Side<4) && (Side>-1))
			m_RecentSide.SetCurSel(Side);

		registry.CloseKey();		
		UpdateData(FALSE);
	}
}

void CPagePanelPosition::OnSelChangeNewClientsSide() 
{
	SetModified();
}

void CPagePanelPosition::OnSelChangeRecentClientsSide() 
{
	SetModified();	
}

void CPagePanelPosition::OnChangeClientNewTime() 
{
	SetModified();
}

void CPagePanelPosition::OnChangeClientRecentTime() 
{
	SetModified();
}

void CPagePanelPosition::OnSelChangeActiveClientsSide() 
{
	SetModified();
}
