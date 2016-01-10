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
// VIPEdit.cpp : implementation file
//

#include "stdafx.h"
#include "VIPShared/VIPIncludes.h"
#include "VIPShared/VIPMenu.h"
#include "VIPShared/PlutoConfig.h"
#include "PlutoUtils/StringUtils.h"
#include "VIPDesign.h"
#include "VIPEditDlg.h"
#include "VIPDesignDoc.h"

#include "VIPTextDlg.h"
#include "VIPListDlg.h"
#include "VIPGraphicDlg.h"
#include "VIPInputDlg.h"
#include "VIPResolutionDlg.h"
#include "VIPMenuDlg.h"
#include "VIPVariableDlg.h"

// CVIPEditDlg dialog

IMPLEMENT_DYNAMIC(CVIPEditDlg, CDialog)
CVIPEditDlg::CVIPEditDlg(map<long, class VIPMenuElement *> *mapText,map<long, class VIPMenuElement *> *mapList,map<long, class VIPMenuElement *> *mapGraphic,
		map<long, class VIPMenuInput *> *mapInput,map<long, class VIPMenuZone *> *mapZone,
		map<long, class VIPMenuResolution *> *mapResolution,map<long, class VIPMenu *> *mapMenu,
		map<long,class VIPVariable *> *mapVariables,class CVIPDesignDoc *pDoc)
	: CDialog(CVIPEditDlg::IDD, NULL)
{
	m_mapText=mapText;
	m_mapList=mapList;
	m_mapGraphic=mapGraphic;
	m_mapInput=mapInput;
	m_mapZone=mapZone;
    m_mapResolution=mapResolution;
	m_mapMenu=mapMenu;
	m_mapVariables=mapVariables;
	m_pDoc = pDoc;
}

CVIPEditDlg::~CVIPEditDlg()
{
}

void CVIPEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ITEMS, m_lbItems);
}


BEGIN_MESSAGE_MAP(CVIPEditDlg, CDialog)
	ON_BN_CLICKED(IDC_EDIT_UP, OnBnClickedEditUp)
	ON_BN_CLICKED(IDC_EDIT_EDIT, OnBnClickedEditEdit)
	ON_BN_CLICKED(IDC_EDIT_DELETE, OnBnClickedEditDelete)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_EDIT_DOWN, OnBnClickedEditDown)
END_MESSAGE_MAP()


// CVIPEditDlg message handlers

void CVIPEditDlg::OnBnClickedEditUp()
{
	// TODO: Add your control notification handler code here
}

void CVIPEditDlg::OnBnClickedEditEdit()
{
	int Selected = m_lbItems.GetCurSel();
	if( Selected>=0 )
	{
		if( m_mapText )
		{
			CVIPTextDlg v((VIPMenuElement_Text *) (*m_mapText)[(long) m_lbItems.GetItemData(Selected)],m_pDoc);
			v.DoModal();
		}
		if( m_mapList )
		{
			CVIPListDlg v((VIPMenuElement_List *) (*m_mapList)[(long) m_lbItems.GetItemData(Selected)],m_pDoc);
			v.DoModal();
		}
		if( m_mapGraphic )
		{
			CVIPGraphicDlg v((VIPMenuElement_Graphic *) (*m_mapGraphic)[(long) m_lbItems.GetItemData(Selected)],m_pDoc);
			v.DoModal();
		}
		if( m_mapInput )
		{
			CVIPInputDlg v((*m_mapInput)[(long) m_lbItems.GetItemData(Selected)]);
			v.DoModal();
		}
		if( m_mapZone )
		{
//			CVIPTextDlg v((VIPMenuElement_Text *) (*m_mapZone)[Selected]);
//			v.DoModal();
		}

		if( m_mapResolution )
		{
			CVIPResolutionDlg v((*m_mapResolution)[(long) m_lbItems.GetItemData(Selected)]);
			v.DoModal();
		}

		if( m_mapMenu )
		{
			CVIPMenuDlg v((*m_mapMenu)[(long) m_lbItems.GetItemData(Selected)]);
			v.DoModal();
		}

		if( m_mapVariables )
		{
			CVIPVariableDlg v((*m_mapVariables)[(long) m_lbItems.GetItemData(Selected)]);
			v.DoModal();
		}
	}
	PopulateListBox();
}

void CVIPEditDlg::OnBnClickedEditDelete()
{
	/*
	int Selected = m_lbItems.GetCurSel();
	if( Selected>=0 )
	{
		if( m_mapResolution )
		{
			VIPMenuResolution *pResolution = (*m_mapResolution)[(long) m_lbItems.GetItemData(Selected)];
			pResolution->
		}


		PopulateListBox();
	}
	*/
}

void CVIPEditDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CVIPEditDlg::OnBnClickedEditDown()
{
	// TODO: Add your control notification handler code here
}

BOOL CVIPEditDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();
   PopulateListBox();

   return TRUE;
}

void CVIPEditDlg::PopulateListBox()
{
	m_lbItems.ResetContent();

	if( m_mapText )
		PopulateWithText();

	if( m_mapList )
		PopulateWithList();

	if( m_mapGraphic )
		PopulateWithGraphic();

	if( m_mapInput )
		PopulateWithInput();

	if( m_mapZone )
		PopulateWithZone();

	if( m_mapResolution )
		PopulateWithResolution();

	if( m_mapMenu )
		PopulateWithMenu();

	if( m_mapVariables )
		PopulateWithVariables();
}

void CVIPEditDlg::PopulateWithText()
{
	MYSTL_ITERATE_LONG_MAP((*m_mapText),VIPMenuElement,pElement,i) 
	{
		if( pElement->MenuElementID()!=MENU_ELEMENT_TEXT )
			continue;

		VIPMenuElement_Text *pText = (VIPMenuElement_Text *) pElement;
		int Item=m_lbItems.AddString(pText->m_sText.c_str());
		m_lbItems.SetItemData(Item,pElement->m_iElementNumber);
	}
}
void CVIPEditDlg::PopulateWithList()
{
	MYSTL_ITERATE_LONG_MAP((*m_mapList),VIPMenuElement,pElement,i) 
	{
		if( pElement->MenuElementID()!=MENU_ELEMENT_LIST )
			continue;

		VIPMenuElement_List *pList = (VIPMenuElement_List *) pElement;
		int Item=m_lbItems.AddString(pList->m_sText.c_str());
		m_lbItems.SetItemData(Item,pElement->m_iElementNumber);
	}
}
void CVIPEditDlg::PopulateWithGraphic()
{
	MYSTL_ITERATE_LONG_MAP((*m_mapGraphic),VIPMenuElement,pElement,i) 
	{
		if( pElement->MenuElementID()!=MENU_ELEMENT_GRAPHIC )
			continue;

		VIPMenuElement_Graphic *pGraphic = (VIPMenuElement_Graphic *) pElement;
		int Item=m_lbItems.AddString(
			(pGraphic->m_sFileName + " X:" + StringUtils::itos(pGraphic->m_iX) + " Y:" + StringUtils::itos(pGraphic->m_iY)).c_str());
		m_lbItems.SetItemData(Item,pElement->m_iElementNumber);
	}
}
void CVIPEditDlg::PopulateWithInput()
{
	MYSTL_ITERATE_LONG_MAP((*m_mapInput),VIPMenuInput,pInput,i) 
	{
		string Description = pInput->m_sDescription;
		if( Description.length()==0 )
		{
			VIPVariable *pVariable = pInput->m_pMenu->m_pCollection->m_mapVariables_Find(pInput->m_iVariableID);
			if( pVariable )
				Description = pVariable->m_sDescription;
			else
				Description = "Variable: " + StringUtils::itos(pInput->m_iVariableID);
		}
		int Item=m_lbItems.AddString(Description.c_str());
		m_lbItems.SetItemData(Item,pInput->m_iInputNumber);
	}
}
void CVIPEditDlg::PopulateWithZone()
{
}
void CVIPEditDlg::PopulateWithResolution()
{
	while(m_lbItems.GetCount())
		m_lbItems.DeleteString(0);

	MYSTL_ITERATE_LONG_MAP((*m_mapResolution),VIPMenuResolution,pResolution,i) 
	{
		int Item=m_lbItems.AddString(pResolution->m_sTerminatingKey.c_str());
		m_lbItems.SetItemData(Item,pResolution->m_iResolutionNumber);
	}
}
void CVIPEditDlg::PopulateWithMenu()
{
	MYSTL_ITERATE_LONG_MAP((*m_mapMenu),VIPMenu,pMenu,i) 
	{
		int Item=m_lbItems.AddString(pMenu->m_sDescription.c_str());
		//m_lbItems.SetItemData(Item,pMenu->m_iMenuID);
		
		//temp hack
		m_lbItems.SetItemData(Item,Item);
	}
}
void CVIPEditDlg::PopulateWithVariables()
{
	MYSTL_ITERATE_LONG_MAP((*m_mapVariables),VIPVariable,pVariable,i) 
	{
        if(!pVariable)
            continue;

		int Item = m_lbItems.AddString(
			(pVariable->m_sDescription + " " + StringUtils::itos(pVariable->m_iVariableID)).c_str());
		m_lbItems.SetItemData(Item,pVariable->m_iVariableID);
	}
}
