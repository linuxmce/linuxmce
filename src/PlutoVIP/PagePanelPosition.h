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
#if !defined(AFX_PAGEPANELPOSITION_H__970177F4_F922_47A8_AF88_544F336B438E__INCLUDED_)
#define AFX_PAGEPANELPOSITION_H__970177F4_F922_47A8_AF88_544F336B438E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PagePanelPosition.h : header file
//


#define SIDE_LEFT		0
#define SIDE_TOP		1
#define SIDE_RIGHT		2
#define SIDE_BOTTOM		3
/////////////////////////////////////////////////////////////////////////////
// CPagePanelPosition dialog

class CPagePanelPosition : public CPropertyPage
{
	DECLARE_DYNCREATE(CPagePanelPosition)

// Construction
public:
	void SetGetVariables(BOOL bSet);
	CPagePanelPosition();
	~CPagePanelPosition();

// Dialog Data
	//{{AFX_DATA(CPagePanelPosition)
	enum { IDD = IDD_PAGE_PANEL_POSITION };
	CComboBox	m_RecentSide;
	CComboBox	m_NewSide;
	CComboBox	m_ActiveSide;
	int		m_NewTime;
	int		m_RecentTime;
	int		m_NewAppearance;
	int		m_ActiveAppearance;
	int		m_RecentAppearance;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPagePanelPosition)
	public:
	virtual void OnOK();
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPagePanelPosition)
	virtual BOOL OnInitDialog();
	afx_msg void OnNewAppearanceVisible();
	afx_msg void OnNewAppearanceInvisible();
	afx_msg void OnNewAppearanceAutohide();
	afx_msg void OnActiveAppearanceVisible();
	afx_msg void OnActiveAppearanceAutohide();
	afx_msg void OnActiveAppearanceInvisible();
	afx_msg void OnRecentAppearanceVisible();
	afx_msg void OnRecentAppearanceInvisible();
	afx_msg void OnRecentAppearanceAutohide();
	afx_msg void OnSelChangeNewClientsSide();
	afx_msg void OnSelChangeRecentClientsSide();
	afx_msg void OnChangeClientNewTime();
	afx_msg void OnChangeClientRecentTime();
	afx_msg void OnSelChangeActiveClientsSide();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEPANELPOSITION_H__970177F4_F922_47A8_AF88_544F336B438E__INCLUDED_)
