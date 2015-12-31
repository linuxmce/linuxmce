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


// CVIPListDlg dialog

class CVIPListDlg : public CDialog
{
	DECLARE_DYNAMIC(CVIPListDlg)
	class VIPMenuElement_List *m_pList;
	class CVIPDesignDoc *m_pDoc;

public:
	CVIPListDlg(class VIPMenuElement_List *pList,class CVIPDesignDoc *pDoc);   // standard constructor
	virtual ~CVIPListDlg();

// Dialog Data
	enum { IDD = IDD_VIPLIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	void DynamicChange();
public:
	BOOL m_IsAlpha;
	CString m_MaxLines;
	CString m_TabStops;
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
	afx_msg void OnEnUpdateListR();
	afx_msg void OnEnUpdateListG();
	afx_msg void OnEnUpdateListB();
	afx_msg void OnEnUpdateListX();
	afx_msg void OnEnUpdateListY();
	afx_msg void OnEnUpdateListWidth();
	afx_msg void OnEnUpdateListHeight();
	afx_msg void OnEnUpdateListSize();
	afx_msg void OnEnUpdateListText();
	afx_msg void OnEnUpdateListTabstops();
	CString m_BasketOption;
	CString m_Variable;
};
