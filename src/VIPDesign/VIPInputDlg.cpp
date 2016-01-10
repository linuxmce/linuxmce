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
// VIPInputDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VIPShared/VIPIncludes.h"
#include "VIPShared/VIPMenu.h"
#include "PlutoUtils/StringUtils.h"
#include "VIPDesign.h"
#include "VIPInputDlg.h"


// CVIPInputDlg dialog

IMPLEMENT_DYNAMIC(CVIPInputDlg, CDialog)
CVIPInputDlg::CVIPInputDlg(class VIPMenuInput *pInput)
	: CDialog(CVIPInputDlg::IDD, NULL)
	, m_NumberOfCharacters(_T(""))
	, m_AutoTerminate(FALSE)
	, m_ClearOnLoad(FALSE)
{
	m_pInput=pInput;
}

CVIPInputDlg::~CVIPInputDlg()
{
}

void CVIPInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_INPUT_NUMCHARS, m_NumberOfCharacters);
	DDX_Check(pDX, IDC_INPUT_AUTOTERM, m_AutoTerminate);
	DDX_Check(pDX, IDC_INPUT_CLEARVAR, m_ClearOnLoad);
	DDX_Control(pDX, IDC_INPUT_VARIABLE, m_cbVariable);
	DDX_Control(pDX, IDC_CB_TEXT, m_TextElement);
}


BEGIN_MESSAGE_MAP(CVIPInputDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

BOOL CVIPInputDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_NumberOfCharacters = StringUtils::itos(m_pInput->m_iNumberOfChars).c_str();
	m_AutoTerminate = m_pInput->m_iTerminateWhenNumberOfChars;
	m_ClearOnLoad = m_pInput->m_iClearVariable;

	VIPMenu *pMenu = m_pInput->m_pMenu;
	VIPMenuCollection *pMenuColletion = pMenu->m_pCollection;

	MYSTL_ITERATE_LONG_MAP(pMenuColletion->m_mapVariables,VIPVariable,pVariable,itv)
	{
		int Value=m_cbVariable.AddString(pVariable->m_sDescription.c_str());
		m_cbVariable.SetItemData(Value,pVariable->m_iVariableID);
		if( m_pInput->m_iVariableID == pVariable->m_iVariableID )
			m_cbVariable.SetCurSel(Value);
	}

	MYSTL_ITERATE_LONG_MAP(pMenu->m_mapMenuElements,VIPMenuElement,pElement,ie)
	{
		if( pElement->MenuElementID()!=MENU_ELEMENT_TEXT )
			continue;
		VIPMenuElement_Text *pText = (VIPMenuElement_Text *)pElement;
		int Value=m_TextElement.AddString(pText->m_sText.c_str());
		m_TextElement.SetItemData(Value,pElement->m_iElementNumber);
		if( m_pInput->m_pTextElement == pText )
			m_TextElement.SetCurSel(Value);
	}
	UpdateData(FALSE);
   return TRUE;
}



// CVIPInputDlg message handlers

void CVIPInputDlg::OnBnClickedOk()
{
	OnOK();
	m_pInput->m_iNumberOfChars = atoi(m_NumberOfCharacters);
	m_pInput->m_iTerminateWhenNumberOfChars = m_AutoTerminate;
	m_pInput->m_iClearVariable = m_ClearOnLoad;

	m_pInput->m_iVariableID = m_cbVariable.GetItemData(m_cbVariable.GetCurSel());
	int TextSel = m_TextElement.GetCurSel();

    m_pInput->m_pTextElement = NULL;

    long lElementID = m_TextElement.GetItemData(TextSel);
	if( TextSel>=0 )
	{
        VIPMenuElement *pElement = m_pInput->m_pMenu->m_mapMenuElements_Find(lElementID);
        if(pElement->MenuElementID() == MENU_ELEMENT_TEXT)
            m_pInput->m_pTextElement = (VIPMenuElement_Text *)pElement;
	}
}
