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


// CVIPPropertiesDlg dialog

class CVIPPropertiesDlg : public CDialog
{
	DECLARE_DYNAMIC(CVIPPropertiesDlg)
	class VIPMenuCollection *m_pMenuCollection;

public:
	CVIPPropertiesDlg(class VIPMenuCollection *pMenuCollection);   // standard constructor
	virtual ~CVIPPropertiesDlg();

// Dialog Data
	enum { IDD = IDD_PROPERTIES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_Width;
	CString m_Height;
	afx_msg void OnBnClickedOk();
	BOOL OnInitDialog();
	CComboBox m_StartingMenu;
	CString m_CollectionID;
};
