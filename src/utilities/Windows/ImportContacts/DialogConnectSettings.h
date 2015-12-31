#pragma once


// CDialogConnectSettings dialog

class CDialogConnectSettings : public CDialog
{
	DECLARE_DYNAMIC(CDialogConnectSettings)

public:
	CDialogConnectSettings(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogConnectSettings();

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_ServerName;
	CString m_DatabaseName;
	CString m_User;
	CString m_Password;
	CString m_ConnectString;
};
