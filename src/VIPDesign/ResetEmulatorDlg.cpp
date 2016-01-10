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
// ResetEmulatorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VIPShared/VIPIncludes.h"
#include "VIPShared/VIPMenu.h"

#include "VIPDesign.h"
#include "ResetEmulatorDlg.h"


// CResetEmulatorDlg dialog

IMPLEMENT_DYNAMIC(CResetEmulatorDlg, CDialog)
CResetEmulatorDlg::CResetEmulatorDlg(class VIPMenuCollection *pCollection,bool bReportVariablesOnly)
	: CDialog(CResetEmulatorDlg::IDD, NULL)
{
	m_pCollection=pCollection;
	m_bReportVariablesOnly=bReportVariablesOnly;
}

CResetEmulatorDlg::~CResetEmulatorDlg()
{
	delete[] m_EditPointers;
}

void CResetEmulatorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CResetEmulatorDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CResetEmulatorDlg message handlers

// CVIPPropertiesDlg message handlers
BOOL CResetEmulatorDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_EditPointers = new CEditPtr[m_pCollection->m_mapVariables.size()];

	int Count=0;

	MYSTL_ITERATE_LONG_MAP(m_pCollection->m_mapVariables,VIPVariable,pVariable,itVariables)
	{
		CEdit *e = new CEdit();
		m_EditPointers[Count]=e;
		CRect r(200,Count*22+15,500,Count*22+35);
		if( m_bReportVariablesOnly )
		{
			e->Create(WS_CHILD | WS_VISIBLE,r,this,60000+Count);
			e->SetWindowText(pVariable->m_sCurrentValue.c_str());
		}
		else
		{
			e->Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,r,this,60000+Count);
			e->SetWindowText(pVariable->m_sInitialValue.c_str());
		}
		
		CStatic *s = new CStatic();
//		s->SetWindowText(pVariable->m_sDescription.c_str());
		CRect r2(5,Count*22+15,185,Count*22+35);
		s->Create(pVariable->m_sDescription.c_str(),SS_RIGHT | WS_CHILD | WS_VISIBLE | WS_TABSTOP,r2,this,61000+Count);

		Count++;
	}

	UpdateData(FALSE);
	return TRUE;
}
void CResetEmulatorDlg::OnBnClickedOk()
{
	OnOK();
	int Count=0;

	MYSTL_ITERATE_LONG_MAP(m_pCollection->m_mapVariables,VIPVariable,pVariable,itVariables)
	{
		CEdit *e = m_EditPointers[Count];
		CString str;
		e->GetWindowText(str);
		pVariable->m_sCurrentValue=str;

		Count++;
	}
}
