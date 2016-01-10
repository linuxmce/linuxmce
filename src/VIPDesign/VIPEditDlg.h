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


// CVIPEditDlg dialog

class CVIPEditDlg : public CDialog
{
	DECLARE_DYNAMIC(CVIPEditDlg)
	
	class CVIPDesignDoc *m_pDoc;

	map<long, class VIPMenuElement *> *m_mapText;
	map<long, class VIPMenuElement *> *m_mapList;
	map<long, class VIPMenuElement *> *m_mapGraphic;
	map<long, class VIPMenuInput *> *m_mapInput;
	map<long, class VIPMenuZone *> *m_mapZone;
    map<long, class VIPMenuResolution *> *m_mapResolution;
	map<long, class VIPMenu *> *m_mapMenu;
	map<long,class VIPVariable *> *m_mapVariables;

public:
	CVIPEditDlg(map<long, class VIPMenuElement *> *mapText,map<long, class VIPMenuElement *> *mapList,map<long, class VIPMenuElement *> *mapGraphic,
		map<long, class VIPMenuInput *> *mapInput,map<long, class VIPMenuZone *> *mapZone,
		map<long, class VIPMenuResolution *> *mapResolution,map<long, class VIPMenu *> *mapMenu,
		map<long,class VIPVariable *> *mapVariables,class CVIPDesignDoc *pDoc);   // standard constructor
	virtual ~CVIPEditDlg();

// Dialog Data
	enum { IDD = IDD_VIPEDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedEditUp();
	afx_msg void OnBnClickedEditEdit();
	afx_msg void OnBnClickedEditDelete();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedEditDown();
	CListBox m_lbItems;

	void PopulateWithText();
	void PopulateWithList();
	void PopulateWithGraphic();
	void PopulateWithInput();
	void PopulateWithZone();
	void PopulateWithResolution();
	void PopulateWithMenu();
	void PopulateWithVariables();

	BOOL OnInitDialog();
	void PopulateListBox();
};
