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
// VIPGraphicDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VIPShared/VIPIncludes.h"
#include "VIPShared/VIPMenu.h"
#include "VIPShared/PlutoConfig.h"
#include "PlutoUtils/StringUtils.h"
#include "Picture.h"

#include "VIPDesign.h"
#include "VIPGraphicDlg.h"
#include "VIPDesignDoc.h"

// CVIPGraphicDlg dialog

IMPLEMENT_DYNAMIC(CVIPGraphicDlg, CDialog)
CVIPGraphicDlg::CVIPGraphicDlg(class VIPMenuElement_Graphic *pGraphic,class CVIPDesignDoc *pDoc)
	: CDialog(CVIPGraphicDlg::IDD, NULL)
	, m_X(_T(""))
	, m_Y(_T(""))
	, m_W(_T(""))
	, m_H(_T(""))
	, m_sFile(_T(""))
	, m_BasketOption(_T(""))
	, m_Variable(_T(""))
{
	m_pGraphic=pGraphic;
	m_pDoc=pDoc;
}

CVIPGraphicDlg::~CVIPGraphicDlg()
{
}

void CVIPGraphicDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_GRAPHIC_X, m_X);
	DDX_Text(pDX, IDC_GRAPHIC_Y, m_Y);
	DDX_Text(pDX, IDC_GRAPHIC_WIDTH, m_W);
	DDX_Text(pDX, IDC_GRAPHIC_HEIGHT, m_H);
	DDX_Text(pDX, IDC_GRAPHIC_FILENAME, m_sFile);
	DDX_Text(pDX, IDC_GRAPHIC_BASKETOPTION, m_BasketOption);
	DDX_Text(pDX, IDC_GRAPHIC_VARIABLE, m_Variable);
}


BEGIN_MESSAGE_MAP(CVIPGraphicDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_GRAPHIC_FIND, OnBnClickedGraphicFind)
	ON_EN_UPDATE(IDC_GRAPHIC_FILENAME, OnEnUpdateGraphicFilename)
	ON_EN_UPDATE(IDC_GRAPHIC_X, OnEnUpdateGraphicX)
	ON_EN_UPDATE(IDC_GRAPHIC_Y, OnEnUpdateGraphicY)
	ON_EN_UPDATE(IDC_GRAPHIC_WIDTH, OnEnUpdateGraphicWidth)
	ON_EN_UPDATE(IDC_GRAPHIC_HEIGHT, OnEnUpdateGraphicHeight)
END_MESSAGE_MAP()

BOOL CVIPGraphicDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_X = StringUtils::itos(m_pGraphic->m_iX).c_str();
	m_Y = StringUtils::itos(m_pGraphic->m_iY).c_str();
	m_W = StringUtils::itos(m_pGraphic->m_iWidth).c_str();
	m_H = StringUtils::itos(m_pGraphic->m_iHeight).c_str();
	m_sFile = m_pGraphic->m_sFileName.c_str();
	m_BasketOption = m_pGraphic->m_sBasketOption.c_str();
	m_Variable = StringUtils::itos(m_pGraphic->m_iVariableIDHidden).c_str();

	UpdateData(FALSE);

	return TRUE;
}


// CVIPGraphicDlg message handlers

void CVIPGraphicDlg::OnBnClickedOk()
{
	OnOK();
	DynamicChange();
/*
	m_pGraphic->m_iX = atoi(m_X);
	m_pGraphic->m_iY = atoi(m_Y);
	m_pGraphic->m_iWidth = atoi(m_W);
	m_pGraphic->m_iHeight = atoi(m_H);
	m_pGraphic->m_sFileName = m_sFile;
	m_pGraphic->m_sBasketOption = m_BasketOption;
	m_pGraphic->m_iVariableIDHidden = atoi(m_Variable);

	// If x,y,w,h are 0, leave it and we'll use the menu defaults
	// but if either x or y are not 0, this is a child object, and if the
	// width or height is 0 we should calculate the size automatically
	if( (m_pGraphic->m_iWidth==0 || m_pGraphic->m_iHeight==0) &&
		(!m_pGraphic->m_iX==0 || !m_pGraphic->m_iY==0) )
	{

		CPicture pic;
		if( pic.Load(m_pGraphic->m_sFileName.c_str()) )
		{
			if( m_pGraphic->m_iWidth==0 )
				m_pGraphic->m_iWidth=pic.GetOriginalWidth();
			if( m_pGraphic->m_iHeight==0 )
				m_pGraphic->m_iHeight=pic.GetOriginalHeight();
		}
	}
*/
}

void CVIPGraphicDlg::OnBnClickedGraphicFind()
{
	UpdateData(true);
	CFileDialog dlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST, "Graphic Files (*.jpg,*.bmp,*.png)|*.jpg;*.BMP;*.PNG");
	int result=dlg.DoModal();
	if( result==IDOK) {
		m_sFile = dlg.GetPathName();
		UpdateData(false);
	}
}

void CVIPGraphicDlg::DynamicChange()
{
	UpdateData(true);
	m_pGraphic->m_iX = atoi(m_X);
	m_pGraphic->m_iY = atoi(m_Y);
	m_pGraphic->m_iWidth = atoi(m_W);
	m_pGraphic->m_iHeight = atoi(m_H);
	m_pGraphic->m_sFileName = m_sFile;
	m_pGraphic->m_sBasketOption = m_BasketOption;
	m_pGraphic->m_iVariableIDHidden = atoi(m_Variable);

	// If x,y,w,h are 0, leave it and we'll use the menu defaults
	// but if either x or y are not 0, this is a child object, and if the
	// width or height is 0 we should calculate the size automatically
	//if( (m_pGraphic->m_iWidth==0 || m_pGraphic->m_iHeight==0) &&
	//	(!m_pGraphic->m_iX==0 || !m_pGraphic->m_iY==0) )
	{
		CPicture pic;
		if( pic.Load(m_pGraphic->m_sFileName.c_str()) )
		{
			if( m_pGraphic->m_iWidth==0 )
				m_pGraphic->m_iWidth=pic.GetOriginalWidth();
			if( m_pGraphic->m_iHeight==0 )
				m_pGraphic->m_iHeight=pic.GetOriginalHeight();

			size_t iFileSize = 0;
			char *pFileData = FileUtils::ReadFileIntoBuffer(m_pGraphic->m_sFileName, iFileSize);

			delete m_pGraphic->m_pGraphicData;
			m_pGraphic->m_pGraphicData = pFileData;
			m_pGraphic->m_iGraphicSize = iFileSize;
		}
	}
	m_pDoc->UpdateAllViews(NULL);
}

void CVIPGraphicDlg::OnEnUpdateGraphicFilename()
{
	DynamicChange();
}

void CVIPGraphicDlg::OnEnUpdateGraphicX()
{
	DynamicChange();
}

void CVIPGraphicDlg::OnEnUpdateGraphicY()
{
	DynamicChange();
}

void CVIPGraphicDlg::OnEnUpdateGraphicWidth()
{
	DynamicChange();
}

void CVIPGraphicDlg::OnEnUpdateGraphicHeight()
{
	DynamicChange();
}
