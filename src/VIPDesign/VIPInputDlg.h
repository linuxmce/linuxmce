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
#include "afxwin.h"


// CVIPInputDlg dialog

class CVIPInputDlg : public CDialog
{
	class VIPMenuInput *m_pInput;
	DECLARE_DYNAMIC(CVIPInputDlg)

public:
	CVIPInputDlg(class VIPMenuInput *pInput);   // standard constructor
	virtual ~CVIPInputDlg();

// Dialog Data
	enum { IDD = IDD_VIPINPUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CString m_NumberOfCharacters;
	BOOL m_AutoTerminate;
	BOOL m_ClearOnLoad;
	CComboBox m_cbVariable;
	CComboBox m_TextElement;
	afx_msg void OnBnClickedOk();
};
