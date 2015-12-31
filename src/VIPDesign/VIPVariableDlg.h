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


// CVIPVariableDlg dialog

class CVIPVariableDlg : public CDialog
{
	DECLARE_DYNAMIC(CVIPVariableDlg)
	class VIPVariable *m_pVariable;

public:
	CVIPVariableDlg(class VIPVariable *pVariable);   // standard constructor
	virtual ~CVIPVariableDlg();

// Dialog Data
	enum { IDD = IDD_VIPVARIABLE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_cbPredefinedVariables;
	CString m_CustomVariableDescription;
	BOOL m_Encrypt;
	BOOL m_PhoneSetsValue;
	CComboBox m_Format;
	CString m_sValue;

	void AddVariableIfNotThere(string Description,int VariableID);
	void AddFormat(string Description,int FormatID);

	afx_msg void OnEnChangeVarCustomVariable();
	afx_msg void OnCbnSelchangeVarPredefined();
	afx_msg void OnBnClickedOk();
};
