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
// VIPDesignDoc.h : interface of the CVIPDesignDoc class
//


#pragma once

class CVIPDesignDoc : public CDocument
{
protected: // create from serialization only
	CVIPDesignDoc();
	DECLARE_DYNCREATE(CVIPDesignDoc)

// Attributes
public:
	class VIPMenuCollection *m_pMenuCollection;
	class VIPMenu *m_pMenu;
	class VIPMenuInput *m_pMenuInput;

	unsigned char m_pImageStatic_Type;
	unsigned long m_pImageStatic_Size;
	const char *m_pImageStatic_Data;

//	HINSTANCE m_hinstPlutoVIPLib;
//	PLUTO_MEMBER_IDENTIFIED m_fnPlutoMemberIdentified;
  //  BOOL m_bRunTimeLinkSuccess;

// Operations
public:

// Overrides
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra,AFX_CMDHANDLERINFO* pHandlerInfo);
	void UpdateListOfMenus();
	void SwitchToMenu(VIPMenu *pMenu,bool bUpdateViews=true);
	void InvalidateAllViews();

// Implementation
public:
	virtual ~CVIPDesignDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileSave();
	afx_msg void OnAddText();
	afx_msg void OnAddGraphic();
	afx_msg void OnEditText();
	afx_msg void OnEditGraphic();
	afx_msg void OnEditInput();
	afx_msg void OnEditResolution();
	afx_msg void OnEditMenu();
	afx_msg void OnEditVariable();
	afx_msg void OnAddInput();
	afx_msg void OnAddResolution();
	afx_msg void OnAddMenu();
	afx_msg void OnAddVariable();
	afx_msg void OnViewGotomenu();
	afx_msg void OnGotoListmenus();
	afx_msg void OnModifyProperties();
	afx_msg void OnAddList();
	afx_msg void OnModifyList();
	afx_msg void OnPosMemberidentified();
};


