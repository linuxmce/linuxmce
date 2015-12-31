// DialogConnectSettings.cpp : implementation file
//

#include "stdafx.h"
#include "ImportContacts.h"
#include "DialogConnectSettings.h"


// CDialogConnectSettings dialog

IMPLEMENT_DYNAMIC(CDialogConnectSettings, CDialog)
CDialogConnectSettings::CDialogConnectSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogConnectSettings::IDD, pParent)
	, m_ServerName(_T(""))
	, m_DatabaseName(_T(""))
	, m_User(_T(""))
	, m_Password(_T(""))
	, m_ConnectString(_T(""))
{
}

CDialogConnectSettings::~CDialogConnectSettings()
{
}

void CDialogConnectSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_ServerName);
	DDX_Text(pDX, IDC_EDIT2, m_DatabaseName);
	DDX_Text(pDX, IDC_EDIT3, m_User);
	DDX_Text(pDX, IDC_EDIT4, m_Password);
	DDX_Text(pDX, IDC_EDIT5, m_ConnectString);
}


BEGIN_MESSAGE_MAP(CDialogConnectSettings, CDialog)
END_MESSAGE_MAP()


// CDialogConnectSettings message handlers
