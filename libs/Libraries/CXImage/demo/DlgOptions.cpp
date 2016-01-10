// DlgOptions.cpp : implementation file
//

#include "stdafx.h"
#include "demo.h"
#include "DlgOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DlgOptions dialog


DlgOptions::DlgOptions(CWnd* pParent /*=NULL*/)
	: CDialog(DlgOptions::IDD, pParent)
{
	//{{AFX_DATA_INIT(DlgOptions)
	m_quality = 0;
	m_xres = 0;
	m_yres = 0;
	//}}AFX_DATA_INIT
}


void DlgOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DlgOptions)
	DDX_Control(pDX, IDOK, m_ok);
	DDX_Control(pDX, IDCANCEL, m_canc);
	DDX_Text(pDX, IDC_EDIT1, m_quality);
	DDV_MinMaxByte(pDX, m_quality, 0, 100);
	DDX_Text(pDX, IDC_EDIT2, m_xres);
	DDV_MinMaxLong(pDX, m_xres, 0, 10000);
	DDX_Text(pDX, IDC_EDIT3, m_yres);
	DDV_MinMaxDWord(pDX, m_yres, 0, 10000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DlgOptions, CDialog)
	//{{AFX_MSG_MAP(DlgOptions)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgOptions message handlers

BOOL DlgOptions::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ok.SetIcon(IDI_G,BS_LEFT);
	m_canc.SetIcon(IDI_R,BS_LEFT);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
