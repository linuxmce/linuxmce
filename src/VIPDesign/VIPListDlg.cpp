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
// VIPListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VIPShared/VIPIncludes.h"
#include "VIPShared/VIPMenu.h"
#include "VIPShared/PlutoConfig.h"
#include "PlutoUtils/StringUtils.h"

#include "VIPDesign.h"
#include "VIPListDlg.h"
#include "VIPDesignDoc.h"


// CVIPListDlg dialog

IMPLEMENT_DYNAMIC(CVIPListDlg, CDialog)
CVIPListDlg::CVIPListDlg(class VIPMenuElement_List *pList,class CVIPDesignDoc *pDoc)
	: CDialog(CVIPListDlg::IDD, NULL)
	, m_IsAlpha(FALSE)
	, m_MaxLines(_T(""))
	, m_TabStops(_T(""))
	, m_sFont(_T(""))
	, m_sText(_T(""))
	, m_Height(_T(""))
	, m_W(_T(""))
	, m_Y(_T(""))
	, m_X(_T(""))
	, m_Size(_T(""))
	, m_R(_T(""))
	, m_G(_T(""))
	, m_B(_T(""))
	, m_BasketOption(_T(""))
	, m_Variable(_T(""))
{
	m_pDoc=pDoc;
	m_pList=pList;
}

CVIPListDlg::~CVIPListDlg()
{
}

void CVIPListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_LIST_ISALPHA, m_IsAlpha);
	DDX_Text(pDX, IDC_LIST_MAXLINES, m_MaxLines);
	DDX_Text(pDX, IDC_LIST_TABSTOPS, m_TabStops);
	DDX_Text(pDX, IDC_LIST_FONT, m_sFont);
	DDX_Text(pDX, IDC_LIST_TEXT, m_sText);
	DDX_Text(pDX, IDC_LIST_HEIGHT, m_Height);
	DDX_Text(pDX, IDC_LIST_WIDTH, m_W);
	DDX_Text(pDX, IDC_LIST_Y, m_Y);
	DDX_Text(pDX, IDC_LIST_X, m_X);
	DDX_Text(pDX, IDC_LIST_SIZE, m_Size);
	DDX_Text(pDX, IDC_LIST_R, m_R);
	DDX_Text(pDX, IDC_LIST_G, m_G);
	DDX_Text(pDX, IDC_LIST_B, m_B);
	DDX_Text(pDX, IDC_LIST_BASKETOPTION, m_BasketOption);
	DDX_Text(pDX, IDC_LIST_VARIABLE, m_Variable);
}


BEGIN_MESSAGE_MAP(CVIPListDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_TEXT_COLOR, OnBnClickedTextColor)
	ON_EN_UPDATE(IDC_LIST_R, OnEnUpdateListR)
	ON_EN_UPDATE(IDC_LIST_G, OnEnUpdateListG)
	ON_EN_UPDATE(IDC_LIST_B, OnEnUpdateListB)
	ON_EN_UPDATE(IDC_LIST_X, OnEnUpdateListX)
	ON_EN_UPDATE(IDC_LIST_Y, OnEnUpdateListY)
	ON_EN_UPDATE(IDC_LIST_WIDTH, OnEnUpdateListWidth)
	ON_EN_UPDATE(IDC_LIST_HEIGHT, OnEnUpdateListHeight)
	ON_EN_UPDATE(IDC_LIST_SIZE, OnEnUpdateListSize)
	ON_EN_UPDATE(IDC_LIST_TEXT, OnEnUpdateListText)
	ON_EN_UPDATE(IDC_LIST_TABSTOPS, OnEnUpdateListTabstops)
END_MESSAGE_MAP()


// CVIPListDlg message handlers
BOOL CVIPListDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_IsAlpha = m_pList->m_iIsAlpha;
	m_MaxLines = StringUtils::itos(m_pList->m_iMaxItemsPerScreen).c_str();
	m_TabStops = m_pList->m_sTabStops.c_str();

	m_sFont = m_pList->m_sFont.c_str();

	string Text = m_pList->m_sText;
	string::size_type pos=-1;
	while( (pos=Text.find('\t'))!=string::npos )
	{
		Text.replace(pos,1,"|");
	}

	m_sText = Text.c_str();
	m_Size = StringUtils::itos(m_pList->m_iSize).c_str();

	m_X = StringUtils::itos(m_pList->m_iX).c_str();
	m_Y = StringUtils::itos(m_pList->m_iY).c_str();
	m_W = StringUtils::itos(m_pList->m_iWidth).c_str();
	m_Height = StringUtils::itos(m_pList->m_iHeight).c_str();
	m_R = StringUtils::itos(m_pList->m_R).c_str();
	m_G = StringUtils::itos(m_pList->m_G).c_str();
	m_B = StringUtils::itos(m_pList->m_B).c_str();
	m_BasketOption = m_pList->m_sBasketOption.c_str();
	m_Variable = StringUtils::itos(m_pList->m_iVariableIDHidden).c_str();

	UpdateData(FALSE);
	return TRUE;
}

void CVIPListDlg::OnBnClickedOk()
{
	OnOK();
	m_pList->m_iIsAlpha = m_IsAlpha;
	m_pList->m_iMaxItemsPerScreen = atoi(m_MaxLines);
	m_pList->m_sTabStops = m_TabStops;
	m_pList->m_sFont = m_sFont;
	m_pList->m_sText = m_sText;

	string::size_type pos=-1;
	while( (pos=m_pList->m_sText .find('|'))!=string::npos )
	{
		m_pList->m_sText.replace(pos,1,"\t");
	}

	m_pList->m_iSize = atoi(m_Size);
	m_pList->m_iX = atoi(m_X);
	m_pList->m_iY = atoi(m_Y);
	m_pList->m_iWidth = atoi(m_W);
	m_pList->m_iHeight = atoi(m_Height);
	m_pList->m_R = atoi(m_R);
	m_pList->m_G = atoi(m_G);
	m_pList->m_B = atoi(m_B);
	m_pList->m_sBasketOption = m_BasketOption;
	m_pList->m_iVariableIDHidden = atoi(m_Variable);
}

void CVIPListDlg::OnBnClickedTextColor()
{
	UpdateData(true);
	CColorDialog c(RGB(atoi(m_X),atoi(m_Y),atoi(m_B)));
	if( c.DoModal()==IDOK )
	{
		m_R = StringUtils::itos(GetRValue(c.GetColor())).c_str();
		m_G = StringUtils::itos(GetGValue(c.GetColor())).c_str();
		m_B = StringUtils::itos(GetBValue(c.GetColor())).c_str();
		UpdateData(false);
	}
}

void CVIPListDlg::OnEnUpdateListR()
{
	DynamicChange();
}

void CVIPListDlg::OnEnUpdateListG()
{
	DynamicChange();
}

void CVIPListDlg::OnEnUpdateListB()
{
	DynamicChange();
}

void CVIPListDlg::OnEnUpdateListX()
{
	DynamicChange();
}

void CVIPListDlg::OnEnUpdateListY()
{
	DynamicChange();
}

void CVIPListDlg::OnEnUpdateListWidth()
{
	DynamicChange();
}

void CVIPListDlg::OnEnUpdateListHeight()
{
	DynamicChange();
}

void CVIPListDlg::OnEnUpdateListSize()
{
	DynamicChange();
}

void CVIPListDlg::OnEnUpdateListText()
{
	DynamicChange();
}

void CVIPListDlg::OnEnUpdateListTabstops()
{
	DynamicChange();
}

void CVIPListDlg::DynamicChange()
{
	UpdateData(true);
	m_pList->m_iIsAlpha = m_IsAlpha;
	m_pList->m_iMaxItemsPerScreen = atoi(m_MaxLines);
	m_pList->m_sTabStops = m_TabStops;
	m_pList->m_sFont = m_sFont;
	m_pList->m_sText = m_sText;
	m_pList->m_iSize = atoi(m_Size);
	m_pList->m_iX = atoi(m_X);
	m_pList->m_iY = atoi(m_Y);
	m_pList->m_iWidth = atoi(m_W);
	m_pList->m_iHeight = atoi(m_Height);
	m_pList->m_R = atoi(m_R);
	m_pList->m_G = atoi(m_G);
	m_pList->m_B = atoi(m_B);	
	m_pDoc->UpdateAllViews(NULL);
}

