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
#if !defined(AFX_UPDATEDLG_H__00EEA299_B84A_4182_BCBF_3E7790B3E5C7__INCLUDED_)
#define AFX_UPDATEDLG_H__00EEA299_B84A_4182_BCBF_3E7790B3E5C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UpdateDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUpdateDlg dialog

class CUpdateDlg : public CDialog
{
// Construction
public:
	void StartDownloading();
	void ProgressUpdate ( LPCTSTR szIEMsg, LPCTSTR szCustomMsg, const int nPercentDone );
	void SetURL(CString szURL);
	CUpdateDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CUpdateDlg)
	enum { IDD = IDD_UPDATE };
	CProgressCtrl	m_Progress;
	CStatic	m_stCustomMsg;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUpdateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString m_URL;

	// Generated message map functions
	//{{AFX_MSG(CUpdateDlg)
	afx_msg void OnStop();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UPDATEDLG_H__00EEA299_B84A_4182_BCBF_3E7790B3E5C7__INCLUDED_)
