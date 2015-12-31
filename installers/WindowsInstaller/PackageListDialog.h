#pragma once
#include "afxwin.h"


// CPackageListDialog dialog

class CPackageListDialog : public CDialog
{
	DECLARE_DYNAMIC(CPackageListDialog)

	CDialog *m_pNextDialog;
	CDialog *m_pPrevDialog;
	CString m_sPlutoHomeURL;

public:
	CPackageListDialog(CDialog *pPrevDialog, CString sPlutoHomeURL, CWnd* pParent = NULL);   // standard constructor
	virtual ~CPackageListDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG_PACKAGESLIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedPrevPack();
	afx_msg void OnBnClickedNextPack();
	afx_msg void OnBnClickedExitPack();
	afx_msg void OnBnClickedPackagesPack();
	afx_msg void OnLbnSelchangeList();
	CStatic m_Status;

	void CloseAll();
	CListBox m_list;
	CStatic m_Header;

protected:
	void ParseRawData(const CString &sHttpRawData);
	void ParseRawLine(const CString &sHttpRawLine);
	void PopulateList();
};
