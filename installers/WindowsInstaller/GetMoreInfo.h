#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CGetMoreInfo dialog

class CGetMoreInfo : public CDialog
{
	DECLARE_DYNAMIC(CGetMoreInfo)

	CDialog *m_pPrevDialog;

public:
	CGetMoreInfo(CDialog *pPrevDialog, CWnd* pParent = NULL);   // standard constructor
	virtual ~CGetMoreInfo();

// Dialog Data
	enum { IDD = IDD_DIALOG_MOREINFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	void InstallPackage(int iIndex);
	void LogInstallAction(CString str);
	void AddMySqlServerIPToHosts();
	void ParseAndExecuteCommands(CString sServer, CString sURL, CString sCommands, CString sFileName, 
		CString sSourceImplementationPath);
	bool UnzipFileToFolder(CString sSourceFile, CString sDestinationFolder);
	bool LaunchProgram(CString sFilePath, bool bWaitProcessToTerminate = true);

	bool DownloadFileFromHttp(CString sServer, CString sURL, CString sDestinationFile);
	bool DownloadFileFromFtp(CString sServer, CString sURL, CString sDestinationFile);
	bool DownloadFileFromSvn(CString sServer, CString sRepository, CString sSourceImplementationPath, 
		CString sDestinationFile);

	bool IsSVNInstalled();
	CString GetInstallationDrive();
	CString GetLogFilePath();
	bool InstallSVN();
	void ProcessMessages(DWORD dwMs = 100);
	bool NeedToInstallSVN();

	bool m_bAboutToCancel;

	vector<CString> m_vectPostInstall;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedMoreinfoPrev();
	afx_msg void OnBnClickedMoreinfoCancel();
	void CloseAll();
	CEdit m_InstallEdit;
	CProgressCtrl m_Progress;
	CEdit m_ServerIPAddressEdit;
	CButton m_RadioBoxOption1;
	CButton m_RadioBoxOption2;
	CButton m_RadioBoxOption3;
	CButton m_PrevButton;
	CButton m_InstallButton;
	afx_msg void OnBnClickedMoreinfoInstall();
	afx_msg void OnBnClickedOption1();
	afx_msg void OnBnClickedOption2();
	afx_msg void OnBnClickedOption3();
	CButton m_CancelFinishButton;
	CComboBox m_PlutoDriveComboBox;
};
