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
// Picture_DlgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Picture_DlgDlg.h"

#include "PlutoUtils/CommonIncludes.h"	
#include "PlutoUtils/MyStl.h"
#include "Customer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHighlightedCustomer dialog

CHighlightedCustomer::CHighlightedCustomer(class Customer *pCustomer)
	: CDialog(CHighlightedCustomer::IDD, 0), m_pCustomer(pCustomer)
{
	//{{AFX_DATA_INIT(CHighlightedCustomer)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
}

void CHighlightedCustomer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHighlightedCustomer)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CHighlightedCustomer, CDialog)
	//{{AFX_MSG_MAP(CHighlightedCustomer)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHighlightedCustomer message handlers

BOOL CHighlightedCustomer::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Set big icon

    SetDlgItemTextA( IDC_NICKNAME, m_pCustomer->m_sNickname.c_str());

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CHighlightedCustomer::OnPaint() 
{
	HWND hWnd = AfxGetApp()->GetMainWnd()->m_hWnd;
	CPaintDC dc(this);

	CRect ClientArea;
	::GetClientRect(hWnd, &ClientArea);
	
//	m_Picture.UpdateSizeOnDC(&dc); // Get Picture Dimentions In Pixels

//	m_Picture.Show(&dc, CRect(0,30,ClientArea.Width(),ClientArea.Height())); // Change Original Dimentions
	 m_pCustomer->m_pPicture->Draw(&dc, CPoint(0,30),CSize(ClientArea.Width(),ClientArea.Height())); // Change Original Dimentions
}	

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CHighlightedCustomer::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CHighlightedCustomer::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	
	Invalidate(FALSE);
}
