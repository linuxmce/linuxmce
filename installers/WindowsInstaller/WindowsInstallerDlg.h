// WindowsInstallerDlg.h : header file
//

#pragma once
#include "afxwin.h"

// CWindowsInstallerDlg dialog
class CWindowsInstallerDlg : public CDialog
{
	CDialog *m_pNextDialog;

// Construction
public:
	CWindowsInstallerDlg(CWnd* pParent = NULL);	// standard constructor
	~CWindowsInstallerDlg();

// Dialog Data
	enum { IDD = IDD_WINDOWSINSTALLER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedNext();
	afx_msg void OnBnClickedExit();

	void CloseAll();
public:
	CEdit m_DeviceID;
	CEdit m_ActivationCode;
	CStatic m_Status;
};
