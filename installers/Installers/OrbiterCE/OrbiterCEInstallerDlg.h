// OrbiterCEInstallerDlg.h : header file
//

#if !defined(AFX_ORBITERCEINSTALLERDLG_H__B527F95C_F73A_4374_9B1D_D8D257E2A30B__INCLUDED_)
#define AFX_ORBITERCEINSTALLERDLG_H__B527F95C_F73A_4374_9B1D_D8D257E2A30B__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// COrbiterCEInstallerDlg dialog

class COrbiterCEInstallerDlg : public CDialog
{
// Construction
public:
	COrbiterCEInstallerDlg(CWnd* pParent = NULL);	// standard constructor

	bool DownloadFile(CString sFile, CString sDestinationFile);

// Dialog Data
	//{{AFX_DATA(COrbiterCEInstallerDlg)
	enum { IDD = IDD_ORBITERCEINSTALLER_DIALOG };
	CStatic	m_Static;
	CEdit	m_Path;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COrbiterCEInstallerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(COrbiterCEInstallerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnInstall();
	afx_msg void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ORBITERCEINSTALLERDLG_H__B527F95C_F73A_4374_9B1D_D8D257E2A30B__INCLUDED_)
