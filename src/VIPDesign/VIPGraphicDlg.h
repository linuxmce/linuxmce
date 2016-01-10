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


// CVIPGraphicDlg dialog

class CVIPGraphicDlg : public CDialog
{
	DECLARE_DYNAMIC(CVIPGraphicDlg)
	class VIPMenuElement_Graphic *m_pGraphic;
	class CVIPDesignDoc *m_pDoc;

public:
	CVIPGraphicDlg(class VIPMenuElement_Graphic *pGraphic,class CVIPDesignDoc *pDoc);   // standard constructor
	virtual ~CVIPGraphicDlg();

// Dialog Data
	enum { IDD = IDD_VIPGRAPHIC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CString m_X;
	CString m_Y;
	CString m_W;
	CString m_H;
	CString m_sFile;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedGraphicFind();
	CString m_BasketOption;
	CString m_Variable;
	afx_msg void OnEnUpdateGraphicFilename();
	afx_msg void OnEnUpdateGraphicX();
	afx_msg void OnEnUpdateGraphicY();
	afx_msg void OnEnUpdateGraphicWidth();
	afx_msg void OnEnUpdateGraphicHeight();

	void DynamicChange();
};
