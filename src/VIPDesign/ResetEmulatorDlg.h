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
#pragma once


// CResetEmulatorDlg dialog

typedef CEdit * CEditPtr;

class CResetEmulatorDlg : public CDialog
{
	DECLARE_DYNAMIC(CResetEmulatorDlg)
	CEditPtr *m_EditPointers;
	class VIPMenuCollection *m_pCollection;
	bool m_bReportVariablesOnly;

public:
	CResetEmulatorDlg(class VIPMenuCollection *pCollection,bool bReportVariablesOnly=false);   // standard constructor
	virtual ~CResetEmulatorDlg();

// Dialog Data
	enum { IDD = IDD_RESET_EMULATOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	BOOL OnInitDialog();

public:
	afx_msg void OnBnClickedOk();
};
