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
#if !defined(AFX_CONTROLS_H__F1365403_AC7E_11D1_B25A_006097960BB7__INCLUDED_)
#define AFX_CONTROLS_H__F1365403_AC7E_11D1_B25A_006097960BB7__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Controls.h : header file
//
#include "afxmt.h"

#include "AppBar.h"
#include "resource.h"
#include "PhoneDetection_VIP.h"
#include "ImageDefs.h"
#include "HighlightedCustomer.h"	// Added by ClassView
/////////////////////////////////////////////////////////////////////////////
// CControlsDlg dialog
/////////////////////////////////////////////////////////////////////////////

class CControlsDlg : public CAppBar
{
// Construction
public:
	void FireHighlightedCustomerDialog();
	int GetImageSize();
	void PrepareImageList();
	void AddCustomerImageToImageList(ULONG Id, CString FileName, int iThumbnailSize);
	CImageList CustomerImageList;
	void PrepareAppBar();
	void SetAppBarName(CString szName);
	void SetDesktopSide(int Side);
	int m_Minute;
	void SetParent(CDialog* pParent);
	void SetPhoneDetection(PhoneDetection_VIP* phoneDetection);
	void RunTimer(int iMinute);
	void SetWindowVisible();
	void SetWindowInvisible();
	void SetImageType(int iImage);
	void SetupButtons();
	CControlsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CControlsDlg();
	CSize m_szMinTracking;  // The minimum size of the client area

// Dialog Data
	//{{AFX_DATA(CControlsDlg)
	enum { IDD = IDD_APPBAR };
	//}}AFX_DATA

protected:  // Internal implementation functions
	void HideFloatAdornments (BOOL fHide);
	void OnAppBarStateChange (BOOL fProposed, UINT uStateProposed);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CControlsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString m_AppBarName;
	CDialog* m_pParent;
	int m_ImageType;
	PhoneDetection_VIP* m_pDetection;
	CHighlightedCustomer* HighlighDlg;
	CMutex m_Mutex;

	void RefreshTaskBar(CDC *pDC);
	void DrawItem(CDC *pDC, int Index, int CustomerImageIndex, string sNickName);
	Customer* SelectedCustomer(CPoint point);

	void OnCancel();
	// Generated message map functions
	//{{AFX_MSG(CControlsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnAppbarAutohide();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClickCustomer(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnOK();
	afx_msg void OnDestroy();
	afx_msg void OnActivateCustomer(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnMouseHover(WPARAM wparam, LPARAM lparam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
};


/////////////////////////////////////////////////////////////////////////////
class TaskBarsContainer
{
public:
    CControlsDlg *m_NewCustomer;
	CControlsDlg *m_RecentCustomer;
	CControlsDlg *m_ActiveCustomer;

	void Init()
	{
	    m_NewCustomer = NULL;
		m_ActiveCustomer = NULL;
		m_RecentCustomer = NULL;
	}

	void RefreshAppBars()
	{
		m_NewCustomer->SetupButtons();
		m_ActiveCustomer->SetupButtons();
		m_RecentCustomer->SetupButtons();
	}

	void Destroy()
	{
		if (m_ActiveCustomer)
		{
			delete m_ActiveCustomer;
			m_ActiveCustomer = NULL;
		}

		if (m_RecentCustomer)
		{
			delete m_RecentCustomer;
			m_RecentCustomer = NULL;
		}

		if (m_NewCustomer)
		{
			delete m_NewCustomer;
			m_NewCustomer = NULL;
		}
	}
};
/////////////////////////////////////////////////////////////////////////////
extern TaskBarsContainer g_TaskBars;
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTROLS_H__F1365403_AC7E_11D1_B25A_006097960BB7__INCLUDED_)
