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
// VIPTextDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VIPShared/VIPIncludes.h"
#include "VIPShared/VIPMenu.h"
#include "VIPShared/PlutoConfig.h"
#include "PlutoUtils/StringUtils.h"

#include "VIPDesign.h"
#include "VIPTextDlg.h"
#include "VIPDesignDoc.h"

// CVIPTextDlg dialog

IMPLEMENT_DYNAMIC(CVIPTextDlg, CDialog)

CVIPTextDlg::CVIPTextDlg(class VIPMenuElement_Text *pText,class CVIPDesignDoc *pDoc)
	: CDialog(CVIPTextDlg::IDD, NULL)
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
	, m_Variable(_T(""))
	, m_BasketOption(_T(""))
{
	m_pText=pText;
	m_pDoc = pDoc;
}

CVIPTextDlg::~CVIPTextDlg()
{
}

void CVIPTextDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TEXT_FONT, m_sFont);
	DDX_Text(pDX, IDC_TEXT, m_sText);
	DDX_Text(pDX, IDC_TEXT_HEIGHT, m_Height);
	DDX_Text(pDX, IDC_TEXT_WIDTH, m_W);
	DDX_Text(pDX, IDC_TEXT_Y, m_Y);
	DDX_Text(pDX, IDC_TEXT_X, m_X);
	DDX_Text(pDX, IDC_TEXT_SIZE, m_Size);
	DDX_Text(pDX, IDC_TEXT_R, m_R);
	DDX_Text(pDX, IDC_TEXT_G, m_G);
	DDX_Text(pDX, IDC_TEXT_B, m_B);
	DDX_Text(pDX, IDC_TEXT_VARIABLE, m_Variable);
	DDX_Text(pDX, IDC_TEXT_BASKETOPTION2, m_BasketOption);
}


BEGIN_MESSAGE_MAP(CVIPTextDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_TEXT_COLOR, OnBnClickedTextColor)
	ON_EN_UPDATE(IDC_TEXT_X, OnEnUpdateTextX)
	ON_EN_UPDATE(IDC_TEXT_Y, OnEnUpdateTextY)
	ON_EN_UPDATE(IDC_TEXT_WIDTH, OnEnUpdateTextWidth)
	ON_EN_UPDATE(IDC_TEXT_HEIGHT, OnEnUpdateTextHeight)
	ON_EN_UPDATE(IDC_TEXT_R, OnEnUpdateTextR)
	ON_EN_UPDATE(IDC_TEXT_G, OnEnUpdateTextG)
	ON_EN_UPDATE(IDC_TEXT_B, OnEnUpdateTextB)
	ON_CBN_SELCHANGE(IDC_TEXT_FONT, OnCbnSelchangeTextFont)
	ON_EN_UPDATE(IDC_TEXT_SIZE, OnEnUpdateTextSize)
END_MESSAGE_MAP()

BOOL CVIPTextDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_sFont = m_pText->m_sFont.c_str();
	m_sText = m_pText->m_sText.c_str();
	m_Size = StringUtils::itos(m_pText->m_iSize).c_str();

	m_X = StringUtils::itos(m_pText->m_iX).c_str();
	m_Y = StringUtils::itos(m_pText->m_iY).c_str();
	m_W = StringUtils::itos(m_pText->m_iWidth).c_str();
	m_Height = StringUtils::itos(m_pText->m_iHeight).c_str();
	m_R = StringUtils::itos(m_pText->m_R).c_str();
	m_G = StringUtils::itos(m_pText->m_G).c_str();
	m_B = StringUtils::itos(m_pText->m_B).c_str();
	m_BasketOption = m_pText->m_sBasketOption.c_str();
	m_Variable = StringUtils::itos(m_pText->m_iVariableIDHidden).c_str();

	UpdateData(FALSE);
	return TRUE;
}

void CVIPTextDlg::OnBnClickedOk()
{
	OnOK();
	m_pText->m_sFont = m_sFont;
	m_pText->m_sText = m_sText;
	m_pText->m_iSize = atoi(m_Size);
	m_pText->m_iX = atoi(m_X);
	m_pText->m_iY = atoi(m_Y);
	m_pText->m_iWidth = atoi(m_W);
	m_pText->m_iHeight = atoi(m_Height);
	m_pText->m_R = atoi(m_R);
	m_pText->m_G = atoi(m_G);
	m_pText->m_B = atoi(m_B);
	m_pText->m_sBasketOption = m_BasketOption;
	m_pText->m_iVariableIDHidden = atoi(m_Variable);
}

void CVIPTextDlg::OnBnClickedTextColor()
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

void CVIPTextDlg::DynamicChange()
{
	UpdateData(true);
	m_pText->m_sFont = m_sFont;
	m_pText->m_sText = m_sText;
	m_pText->m_iSize = atoi(m_Size);
	m_pText->m_iX = atoi(m_X);
	m_pText->m_iY = atoi(m_Y);
	m_pText->m_iWidth = atoi(m_W);
	m_pText->m_iHeight = atoi(m_Height);
	m_pText->m_R = atoi(m_R);
	m_pText->m_G = atoi(m_G);
	m_pText->m_B = atoi(m_B);
	m_pDoc->UpdateAllViews(NULL);
}

void CVIPTextDlg::OnEnUpdateTextX()
{
	DynamicChange();
}

void CVIPTextDlg::OnEnUpdateTextY()
{
	DynamicChange();
}


void CVIPTextDlg::OnEnUpdateTextWidth()
{
	DynamicChange();
}

void CVIPTextDlg::OnEnUpdateTextHeight()
{
	DynamicChange();
}

void CVIPTextDlg::OnEnUpdateTextR()
{
	DynamicChange();
}

void CVIPTextDlg::OnEnUpdateTextG()
{
	DynamicChange();
}

void CVIPTextDlg::OnEnUpdateTextB()
{
	DynamicChange();
}

void CVIPTextDlg::OnCbnSelchangeTextFont()
{
	DynamicChange();
}

void CVIPTextDlg::OnEnUpdateTextSize()
{
	DynamicChange();
}
