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


// CVIPTextDlg dialog

class CVIPTextDlg : public CDialog
{
	DECLARE_DYNAMIC(CVIPTextDlg)

	class VIPMenuElement_Text *m_pText;
	class CVIPDesignDoc *m_pDoc;

public:
	CVIPTextDlg(class VIPMenuElement_Text *pText,class CVIPDesignDoc *pDoc);   // standard constructor
	virtual ~CVIPTextDlg();

// Dialog Data
	enum { IDD = IDD_VIPTEXT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeTextFont();
	CString m_sFont;
	CString m_sText;
	afx_msg void OnBnClickedOk();
	CString m_Height;
	CString m_W;
	CString m_Y;
	CString m_X;
	CString m_Size;

	CString m_R;
	CString m_G;
	CString m_B;
	afx_msg void OnBnClickedTextColor();
	afx_msg void OnEnUpdateTextX();
	afx_msg void OnEnUpdateTextY();
	afx_msg void OnEnUpdateTextWidth();
	afx_msg void OnEnUpdateTextHeight();
	afx_msg void OnEnUpdateTextR();
	afx_msg void OnEnUpdateTextG();
	afx_msg void OnEnUpdateTextB();
	afx_msg void OnCbnSelchangeTextFont();
	afx_msg void OnEnUpdateTextSize();

	void DynamicChange();
	CString m_Variable;
	CString m_BasketOption;
};
