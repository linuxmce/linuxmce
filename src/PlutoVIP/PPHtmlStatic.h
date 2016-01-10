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
#ifndef _PPHTMLSTATIC_H
#define _PPHTMLSTATIC_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PPHtmlStatic.h : header file
//
#include "PPHtmlDrawer.h"

#define UDM_TOOLTIP_REPAINT		   (WM_APP + 100)

#define PPHTMLSTATIC_ALIGN_DEFAULT		0x00
#define PPHTMLSTATIC_ALIGN_LEFT    	    0x01
#define PPHTMLSTATIC_ALIGN_RIGHT  		0x02
#define PPHTMLSTATIC_ALIGN_CENTER 		0x03
#define PPHTMLSTATIC_ALIGN_TOP 		    0x04
#define PPHTMLSTATIC_ALIGN_BOTTOM  		0x08
#define PPHTMLSTATIC_ALIGN_VCENTER 		0x0C

/////////////////////////////////////////////////////////////////////////////
// CPPHtmlStatic window

class CPPHtmlStatic : public CStatic
{
// Construction
public:
	void SetCallbackHyperlink(UINT nMessage, LPARAM lParam = 0);
	void SetHyperlink(LPCTSTR lpszText, LPCTSTR lpszUrl, BOOL bParentNotify = FALSE);

	void SetIcon(DWORD dwIdIcon, LPSIZE lpSizeItem = NULL);
	void SetIcon(HICON hIcon);
	void SetBitmap(DWORD dwIdBitmap, COLORREF clrMask = RGB(255, 0, 255));
	void SetBitmap(HBITMAP hBitmap, COLORREF clrMask = RGB(255, 0, 255));
	void SetAnimation(DWORD dwIdBitmap, LPSIZE lpSizeItem, COLORREF clrMask = RGB(255, 0, 255));
	void SetAnimation(HBITMAP hBitmap, LPSIZE lpSizeItem, COLORREF clrMask = RGB(255, 0, 255));
	void SetImageList(DWORD dwIdBitmap, DWORD dwIndex, LPSIZE lpSizeItem, COLORREF clrMask = RGB(255, 0, 255));
	void SetImageList(HBITMAP hBitmap, DWORD dwIndex, LPSIZE lpSizeItem, COLORREF clrMask = RGB(255, 0, 255));

	void  SetAlign(DWORD dwAlign = PPHTMLSTATIC_ALIGN_DEFAULT);
	DWORD GetAlign();

	void SetImageZoom(int nWidthPercent = 100, int nHeightPercent = 100);
	void SetImageZoom(int nPercent = 100);
	
	void SetFrameRate(DWORD dwFrameRate = 500);
	void ClearFrameRate();

	void SetHyperlinkStyle(COLORREF clrOrg, BOOL bOrgUnderline, COLORREF clrHover, BOOL bHoverUnderline);

	void SetTextColor(COLORREF clrText);
	void SetBkColor(COLORREF clrBk);
	void SetBkMode(int nBkMode = TRANSPARENT);

	static short GetVersionI()		{return 0x11;}
	static LPCTSTR GetVersionC()	{return (LPCTSTR)_T("1.1");}

	CPPHtmlStatic();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPPHtmlStatic)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPPHtmlStatic();

	// Generated message map functions
protected:
	void SetDefaultFont(HFONT hFont);
	void SetText(LPCTSTR lpszText);
	void UpdateSurface();
	void RepaintWindow(HDC hDC = NULL);

	HBITMAP m_hBitmapBk;

	CRect m_rcArea;
	CSize m_szOutput;

	CPPHtmlDrawer m_drawer;
	DWORD	m_dwAlign;
	CString m_sText;
	CString m_sOrgText; //Original
	int m_nWidthPercent;
	int m_nHeightPercent;
	
	BOOL m_bIsEnabled;

	//Timer
	BOOL m_bIsRunning;
	DWORD m_dwFrameRate;

	BOOL m_bIsImage;
	CString m_sTemplate;

	HFONT m_hFont;

	COLORREF m_clrText;
	COLORREF m_clrBk;
	int m_nBkMode;

	//{{AFX_MSG(CPPHtmlStatic)
	afx_msg void OnPaint();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnEnable(BOOL bEnable);
	//}}AFX_MSG
 	afx_msg LRESULT OnGetText(WPARAM, LPARAM);
	afx_msg LRESULT OnSetText(WPARAM, LPARAM);
	afx_msg LRESULT OnSetFont(WPARAM, LPARAM);
	afx_msg LRESULT OnGetFont(WPARAM, LPARAM);
    afx_msg LRESULT OnRepaintWindow(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
