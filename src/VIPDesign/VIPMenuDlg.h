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


// CVIPMenuDlg dialog

class CVIPMenuDlg : public CDialog
{
	DECLARE_DYNAMIC(CVIPMenuDlg)
	class VIPMenu *m_pMenu;

public:
	CVIPMenuDlg(class VIPMenu *pMenu);   // standard constructor
	virtual ~CVIPMenuDlg();

// Dialog Data
	enum { IDD = IDD_VIPMENU };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CString m_Description;
	afx_msg void OnBnClickedOk();
};
