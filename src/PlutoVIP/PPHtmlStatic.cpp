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
/////////////////////////////////////////////////////////////////////
// PPHtmlStatic.cpp : implementation file
//-----------------------------------------
// Author:			Eugene Pustovoyt
// Created:			8 May 2004
// Last Modified:	25 May 2004
// Current Version: 1.1
//
//--- History ------------------------------ 
// 2004/05/09 *** Releases version 1.0 ***
//------------------------------------------
// 2004/05/18 [ADD] Added a SetBkColor method and a SetBkMode method
// 2004/05/19 [ADD] Added a ClearFrameRate method (thanks to Steve Mayfield)
//			  [FIX] Fixed a minor errors with animation (thanks to Steve Mayfield)
// 2004/05/21 [FIX] Fixed a dirty control when redrawing of the text
// 2004/05/25 [FIX] Enable state of the control wasn't initialized
/////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PPHtmlStatic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TIMER_ANIMATION		0x104

/////////////////////////////////////////////////////////////////////////////
// CPPHtmlStatic

CPPHtmlStatic::CPPHtmlStatic()
{
	m_hBitmapBk = NULL;

	m_bIsRunning = FALSE;
	m_dwFrameRate = 0;

	m_bIsImage = FALSE;
	SetText(_T(""));

	m_rcArea.SetRectEmpty();
	
	SetAlign(PPHTMLSTATIC_ALIGN_DEFAULT);
	SetHyperlinkStyle(RGB(0, 0, 255), FALSE, RGB(0, 0, 255), TRUE);
	SetImageZoom(100, 100);
	SetTextColor(-1); //Use a color from HTML string
	SetBkColor(RGB(255, 255, 255));
	SetBkMode();
}

CPPHtmlStatic::~CPPHtmlStatic()
{
	ClearFrameRate();
	if (NULL != m_hBitmapBk)
		::DeleteObject(m_hBitmapBk);
} //End of ~CPPHtmlStatic

BEGIN_MESSAGE_MAP(CPPHtmlStatic, CStatic)
	//{{AFX_MSG_MAP(CPPHtmlStatic)
	ON_WM_PAINT()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_ENABLE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SETTEXT, OnSetText)
	ON_MESSAGE(WM_GETTEXT, OnGetText)
	ON_MESSAGE(WM_SETFONT, OnSetFont)
	ON_MESSAGE(WM_GETFONT, OnGetFont)
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(UDM_TOOLTIP_REPAINT, OnRepaintWindow)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPPHtmlStatic message handlers
void CPPHtmlStatic::PreSubclassWindow() 
{
	//Enable the Static to send the Window Messages To its parent
	DWORD dwStyle = GetStyle();
	SetWindowLong(GetSafeHwnd(), GWL_STYLE, dwStyle | SS_NOTIFY);

	m_bIsEnabled = (WS_DISABLED & dwStyle) ? FALSE : TRUE;
	m_drawer.EnableOutput(m_bIsEnabled);
	
	m_drawer.SetCallbackRepaint(GetSafeHwnd(), UDM_TOOLTIP_REPAINT);

	CFont* pFont = GetFont();
	HFONT hFont = NULL;
	//ENG: Gets a default system font
	//RUS: Получаем системный шрифт по-умолчанию
	if (NULL == hFont)
	{
		hFont = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
		if (NULL == hFont)
			hFont = (HFONT)::GetStockObject(ANSI_VAR_FONT);
	}
	else hFont = (HFONT)pFont->GetSafeHandle();

	SetDefaultFont(hFont);
	
	CStatic::PreSubclassWindow();
} //End of PreSubclassWindow

afx_msg BOOL CPPHtmlStatic::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
} //End of OnEraseBkgnd

afx_msg void CPPHtmlStatic::OnTimer(UINT nIDEvent) 
{
	if ((TIMER_ANIMATION == nIDEvent) && m_bIsEnabled)
	{
		m_drawer.OnTimer();
		RepaintWindow();
	}
	
	CStatic::OnTimer(nIDEvent);
} //End of OnTimer

afx_msg void CPPHtmlStatic::OnEnable(BOOL bEnable) 
{
	CStatic::OnEnable(bEnable);
	
	m_bIsEnabled = bEnable;
	m_drawer.EnableOutput(bEnable);
//	Invalidate();
//	UpdateWindow();
	UpdateSurface();
} //End of OnEnable

afx_msg LRESULT CPPHtmlStatic::OnSetText(WPARAM wParam, LPARAM lParam)
{
	m_bIsImage = FALSE;
	SetText((LPCTSTR)lParam);
	return 0;
} //End of OnSetFont

afx_msg LRESULT CPPHtmlStatic::OnGetText(WPARAM wParam, LPARAM lParam)
{
	return (LRESULT)(LPCTSTR)m_sText;
} //End of OnSetFont

afx_msg LRESULT CPPHtmlStatic::OnSetFont (WPARAM wParam, LPARAM lParam)
{
	SetDefaultFont((HFONT)wParam);
	return 0;
} //End of OnSetFont

afx_msg LRESULT CPPHtmlStatic::OnGetFont (WPARAM, LPARAM)
{
	return (LRESULT)m_hFont;
} //End of OnSetFont

afx_msg void CPPHtmlStatic::OnMouseMove(UINT nFlags, CPoint point) 
{
	// Create TRACKMOUSEEVENT structure
	TRACKMOUSEEVENT tmeMouseEvent;		
	
	// Initialize members of structure
	tmeMouseEvent.cbSize = sizeof(TRACKMOUSEEVENT);
	tmeMouseEvent.dwFlags = TME_LEAVE;
	tmeMouseEvent.hwndTrack = m_hWnd;
	
	// Track mouse leave event
	_TrackMouseEvent(&tmeMouseEvent);
	
	CStatic::OnMouseMove(nFlags, point);
} //End of OnMouseMove

// Handler for WM_MOUSELEAVE
afx_msg LRESULT CPPHtmlStatic::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	RepaintWindow();

	return 0;
} // End of OnMouseLeave

void CPPHtmlStatic::UpdateSurface()
{
	if (::IsWindow(GetSafeHwnd()))
	{
		Invalidate();
		UpdateWindow();
//		RedrawWindow(NULL);
//		RepaintWindow();
/*
		CRect (rc);
		GetWindowRect(rc);
		RedrawWindow();
		
		GetParent()->ScreenToClient(rc);
		GetParent()->InvalidateRect(rc,TRUE);
		GetParent()->UpdateWindow();
*/
	} //if
} //End of UpdateSurface

void CPPHtmlStatic::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	HDC hMemDC = ::CreateCompatibleDC(dc.GetSafeHdc());
	
	CRect rcClient;
	GetClientRect(&rcClient);

	//Copies a background of a static control to a bitmap
/*
	if (NULL != m_hBitmapBk)
		::DeleteObject(m_hBitmapBk);
	m_hBitmapBk = ::CreateCompatibleBitmap(dc.GetSafeHdc(), rcClient.Width(), rcClient.Height());
	HBITMAP hOldBmp = (HBITMAP)::SelectObject(hMemDC, m_hBitmapBk);
	if (TRANSPARENT == m_nBkMode)
	{
		::BitBlt(hMemDC, 0, 0, rcClient.Width(), rcClient.Height(), dc.GetSafeHdc(), rcClient.left, rcClient.top, SRCCOPY);
	}
	else
	{
//		::BitBlt(hMemDC, 0, 0, rcClient.Width(), rcClient.Height(), NULL, 0, 0, WHITENESS);
		HBRUSH brush = ::CreateSolidBrush(m_clrBk);
		::FillRect(hMemDC, rcClient, brush);
		::DeleteObject(brush);
	} //if
	::SelectObject(hMemDC, hOldBmp);
	::DeleteDC(hMemDC);
*/
	if (NULL == m_hBitmapBk)
	{
		m_hBitmapBk = ::CreateCompatibleBitmap(dc.GetSafeHdc(), rcClient.Width(), rcClient.Height());
		HBITMAP hOldBmp = (HBITMAP)::SelectObject(hMemDC, m_hBitmapBk);
		if (TRANSPARENT == m_nBkMode)
		{
			::BitBlt(hMemDC, 0, 0, rcClient.Width(), rcClient.Height(), dc.GetSafeHdc(), rcClient.left, rcClient.top, SRCCOPY);
		}
		else
		{
			//		::BitBlt(hMemDC, 0, 0, rcClient.Width(), rcClient.Height(), NULL, 0, 0, WHITENESS);
			HBRUSH brush = ::CreateSolidBrush(m_clrBk);
			::FillRect(hMemDC, rcClient, brush);
			::DeleteObject(brush);
		} //if
		::SelectObject(hMemDC, hOldBmp);
		::DeleteDC(hMemDC);
	} //if

	CString str = m_sText;
	if (!m_bIsImage && (m_clrText != -1))
	{
		//If text mode was setted a color was specified
		CString sColor;
		sColor.Format(_T("<font color=0x%06X>"), m_clrText);
		str = sColor + str + _T("</font>");
	} //if
	m_drawer.PrepareOutput(dc.GetSafeHdc(), str, &m_szOutput);

	SIZE size;
	size.cx = min(m_szOutput.cx, rcClient.Width());
	size.cy = min(m_szOutput.cy, rcClient.Height());

	DWORD dwStyle = GetStyle();
	DWORD dwAlign = m_dwAlign;
	//Horizontal align
	if (PPHTMLSTATIC_ALIGN_DEFAULT == (dwAlign & 0x03))
	{
		switch (dwStyle & 0x03)
		{
		case SS_LEFT:
			dwAlign |= PPHTMLSTATIC_ALIGN_LEFT;
			break;
		case SS_RIGHT:
			dwAlign |= PPHTMLSTATIC_ALIGN_RIGHT;
			break;
		case SS_CENTER:
			dwAlign |= PPHTMLSTATIC_ALIGN_CENTER;
			break;
		} //switch
	} //if
	//Vertical align
	if (PPHTMLSTATIC_ALIGN_DEFAULT == (dwAlign & 0x0C))
	{
		if (SS_CENTERIMAGE & dwStyle)
			dwAlign |= PPHTMLSTATIC_ALIGN_VCENTER;
		else
			dwAlign |= PPHTMLSTATIC_ALIGN_TOP;
	} //if
	
	CPoint pt = rcClient.TopLeft();
	switch (dwAlign & 0x03)
	{
	case PPHTMLSTATIC_ALIGN_RIGHT:
		pt.x += rcClient.Width() - size.cx;
		break;
	case PPHTMLSTATIC_ALIGN_CENTER:
		pt.x += (rcClient.Width() - size.cx) / 2;
		break;
	} //switch
	
	switch (dwAlign & 0x0C)
	{
	case PPHTMLSTATIC_ALIGN_BOTTOM:
		pt.y += rcClient.Height() - size.cy;
		break;
	case PPHTMLSTATIC_ALIGN_VCENTER:
		pt.y += (rcClient.Height() - size.cy) / 2;
		break;
	} //switch

	//ENG: A placement of the output area
	//RUS: Положение области вывода по отношению к клиентским координатам
	m_rcArea.SetRect(pt.x, pt.y, pt.x + size.cx, pt.y + size.cy);

	RepaintWindow(dc.GetSafeHdc());
} //End of OnPaint

BOOL CPPHtmlStatic::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (m_bIsEnabled)
	{
		CPoint ptClient;
		::GetCursorPos(&ptClient);
		ScreenToClient(&ptClient);
//		ptClient.x -= m_rcArea.left;
//		ptClient.y -= m_rcArea.top;
		if (m_drawer.OnSetCursor(&ptClient))
			return TRUE; //The cursor over the hyperlink
	} //if
	
	return CStatic::OnSetCursor(pWnd, nHitTest, message);
} //End of OnSetCursor

afx_msg void CPPHtmlStatic::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_rcArea.PtInRect(point) && m_bIsEnabled)
	{
		CPoint ptClient = point;
		ptClient.x -= m_rcArea.left;
		ptClient.y -= m_rcArea.top;
		m_drawer.OnLButtonDown(&ptClient);
	} //if
	
	CStatic::OnLButtonDown(nFlags, point);
} //End of OnLButtonDown

void CPPHtmlStatic::SetText(LPCTSTR lpszText)
{
	m_sText = (CString)lpszText;
	UpdateSurface();
} //End of SetText

afx_msg LRESULT CPPHtmlStatic::OnRepaintWindow(WPARAM wParam, LPARAM lParam)
{
	TRACE("CPPHtmlStatic::OnRepaintWindow()\n");
	RepaintWindow();
    return TRUE;
} //End of the UDM_TOOLTIP_REPAINT handler

void CPPHtmlStatic::RepaintWindow(HDC hDC /* = NULL */)
{
	TRACE("CPPHtmlStatic::RepaintWindow()\n");
	//ENG: If device context not passed
	//RUS: Если контекст устройства не передавался, то получаем его и устанавливаем признак автоматического удаления
	BOOL bAutoReleaseDC = FALSE;
	if (NULL == hDC)
	{
		hDC = ::GetDC(this->GetSafeHwnd());
		bAutoReleaseDC = TRUE;
	} //if

	CRect rcClient;
	GetClientRect(&rcClient);

	//ENG: Creates memory context
	//RUS: Создаем контекст устройства в памяти
	HDC hMemDC = ::CreateCompatibleDC(hDC);
	HDC hBkDC = ::CreateCompatibleDC(hDC);
	HBITMAP hOldBkBitmap = (HBITMAP)::SelectObject(hBkDC, m_hBitmapBk);
	HBITMAP hBitmap = ::CreateCompatibleBitmap(hDC, rcClient.Width(), rcClient.Height());
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, hBitmap);

	//ENG: Copy background to the temporary bitmap
	//RUS: Копируем фон под тултипом в память
	::BitBlt(hMemDC, 0, 0, rcClient.Width(), rcClient.Height(), hBkDC, 0, 0, SRCCOPY);

	CRect rcOutput = m_rcArea;
	if (!m_bIsEnabled)
	{
		rcOutput.OffsetRect(1, 1);
		m_drawer.SetDisabledColor(::GetSysColor(COLOR_BTNHILIGHT));
		m_drawer.DrawPreparedOutput(hMemDC, &rcOutput);
		rcOutput.OffsetRect(-1, -1);
		m_drawer.SetDisabledColor(::GetSysColor(COLOR_BTNSHADOW));
	}
	//ENG: Draw HTML string
	//RUS: Отображаем HTML строку
	m_drawer.DrawPreparedOutput(hMemDC, &rcOutput);

	//ENG: Output a tooltip to the screen
	//RUS: Выводим тултип на экран
	::BitBlt(hDC, 0, 0, rcClient.Width(), rcClient.Height(), hMemDC, 0, 0, SRCCOPY);
	
	//ENG: Free resources
	//RUS: Освобождаем задействованные ресурсы
	::SelectObject(hBkDC, hOldBkBitmap);
	::SelectObject(hMemDC, hOldBitmap);
	::DeleteObject(hBitmap);
	::DeleteDC(hBkDC);
	::DeleteDC(hMemDC);

	//ENG: Releases device context if needed
	//RUS: Освобождаем контекст устройства если это необходимо
	if (bAutoReleaseDC)
		::ReleaseDC(this->GetSafeHwnd(), hDC);
}

///////////////////////////////////////////////////////////////////////////////
// public methods

void CPPHtmlStatic::SetDefaultFont(HFONT hFont)
{
	m_hFont = hFont;

	if (NULL != hFont)
	{
		LOGFONT lf;
		CFont::FromHandle(hFont)->GetLogFont(&lf);
		CString str;
		str.Format(_T("font-size:%dpt; color:0x%06X; font-family:%s; font-weight:%d"), lf.lfHeight, ::GetSysColor(COLOR_WINDOWTEXT), lf.lfFaceName, lf.lfWeight);
		if (lf.lfItalic)
			str += _T("; font-style:italic");
		m_drawer.SetTextStyle(_T("body"), str);
		UpdateSurface();
	} //if
} //End of SetDefaultFont

void CPPHtmlStatic::SetImageZoom(int nPercent /* = 100 */)
{
	SetImageZoom(nPercent, nPercent);
}  //End of SetImageZoom

void CPPHtmlStatic::SetImageZoom(int nWidthPercent /* = 100 */, int nHeightPercent /* = 100 */)
{
	m_nHeightPercent = nHeightPercent;
	m_nWidthPercent = nWidthPercent;
	if (m_bIsImage)
	{
		CRect rect;
		GetClientRect(&rect);
		CString str = m_sTemplate.Left(m_sTemplate.GetLength() - 1);
		CString str1, str2;
		if (0 == nWidthPercent)
			str1.Format(_T(" width=%d"), rect.Width());
		else
			str1.Format(_T(" width=%d%%"), nWidthPercent);

		if (0 == nHeightPercent)
			str2.Format(_T(" height=%d"), rect.Height());
		else
			str2.Format(_T(" height=%d%%"), nHeightPercent);

		str += str1 + str2 + _T(">");
		SetText(str);
	} //if
} //End of SetImageZoom

void CPPHtmlStatic::SetAlign(DWORD dwAlign /* = PPHTMLSTATIC_ALIGN_DEFAULT */)
{
	TRACE(_T("CPPHtmlStatic::SetTextAlign()\n"));
	m_dwAlign = dwAlign;
	UpdateSurface();
} //End of SetAlign


DWORD CPPHtmlStatic::GetAlign()
{
	TRACE(_T("CPPHtmlStatic::GetTextAlign()\n"));
	return m_dwAlign;
} //End of GetAlign

void CPPHtmlStatic::SetHyperlink(LPCTSTR lpszText, LPCTSTR lpszUrl, BOOL bParentNotify /* = FALSE */)
{
	ASSERT(lpszText);
	ASSERT(lpszUrl);
	
	ClearFrameRate();
	CString str;
	if (bParentNotify)
		str.Format(_T("<a msg=\"%s\">%s</a>"), lpszUrl, lpszText);
	else
		str.Format(_T("<a href=\"%s\">%s</a>"), lpszUrl, lpszText);
	SetText(str);
} //End of SetHyperlink

void CPPHtmlStatic::SetIcon(DWORD dwIdIcon, LPSIZE lpSize /* = NULL */)
{
	ASSERT(dwIdIcon);
	ClearFrameRate();
	if (NULL != lpSize)
		m_sTemplate.Format(_T("<icon idres=%d cx=%d cy=%d>"), dwIdIcon, lpSize->cx, lpSize->cy);
	else
		m_sTemplate.Format(_T("<icon idres=%d cx=%d cy=%d>"), dwIdIcon, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
	m_bIsImage = TRUE;
	SetImageZoom(m_nWidthPercent, m_nHeightPercent);
} //End of SetIcon

void CPPHtmlStatic::SetIcon(HICON hIcon)
{
	ASSERT(hIcon);
	ClearFrameRate();
	m_sTemplate.Format(_T("<icon handle=0x%X>"), hIcon);
	m_bIsImage = TRUE;
	SetImageZoom(m_nWidthPercent, m_nHeightPercent);
} //End of SetIcon

void CPPHtmlStatic::SetBitmap(DWORD dwIdBitmap, COLORREF clrMask /* = RGB(255, 0, 255) */)
{
	ASSERT(dwIdBitmap);
	ClearFrameRate();
	if (-1 == clrMask)
		m_sTemplate.Format(_T("<bmp idres=%d>"), dwIdBitmap);
	else
		m_sTemplate.Format(_T("<bmp idres=%d mask=0x%06X>"), dwIdBitmap, clrMask);
	m_bIsImage = TRUE;
	SetImageZoom(m_nWidthPercent, m_nHeightPercent);
} //End of SetBitmap

void CPPHtmlStatic::SetBitmap(HBITMAP hBitmap, COLORREF clrMask /* = RGB(255, 0, 255) */)
{
	ASSERT(hBitmap);
	ClearFrameRate();
	if (-1 == clrMask)
		m_sTemplate.Format(_T("<bmp handle=0x%X>"), hBitmap);
	else
		m_sTemplate.Format(_T("<bmp handle=0x%X mask=0x%06X>"), hBitmap, clrMask);
	m_bIsImage = TRUE;
	SetImageZoom(m_nWidthPercent, m_nHeightPercent);
} //End of SetBitmap

void CPPHtmlStatic::SetAnimation(DWORD dwIdBitmap, LPSIZE lpSizeItem, COLORREF clrMask /* = RGB(255, 0, 255) */)
{
	ASSERT(dwIdBitmap);
	ASSERT(lpSizeItem->cx);
	ASSERT(lpSizeItem->cy);
	SetFrameRate(m_dwFrameRate);
	m_sTemplate.Format(_T("<ilst index=0 idres=%d mask=0x%06X cx=%d, cy=%d speed=1>"), dwIdBitmap, clrMask, lpSizeItem->cx, lpSizeItem->cy);
	m_bIsImage = TRUE;
	SetImageZoom(m_nWidthPercent, m_nHeightPercent);
}

void CPPHtmlStatic::SetAnimation(HBITMAP hBitmap, LPSIZE lpSizeItem, COLORREF clrMask /* = RGB(255, 0, 255) */)
{
	ASSERT(hBitmap);
	SetFrameRate(m_dwFrameRate);
	m_sTemplate.Format(_T("<ilst index=0 handle=0x%X mask=0x%06X cx=%d, cy=%d speed=1>"), hBitmap, clrMask, lpSizeItem->cx, lpSizeItem->cy);
	m_bIsImage = TRUE;
	SetImageZoom(m_nWidthPercent, m_nHeightPercent);
}

void CPPHtmlStatic::SetImageList(DWORD dwIdBitmap, DWORD dwIndex, LPSIZE lpSizeItem, COLORREF clrMask /* = RGB(255, 0, 255) */)
{
	ClearFrameRate();
	m_sTemplate.Format(_T("<ilst index=%d idres=%d mask=0x%06X cx=%d, cy=%d>"), dwIndex, dwIdBitmap, clrMask, lpSizeItem->cx, lpSizeItem->cy);
	m_bIsImage = TRUE;
	SetImageZoom(m_nWidthPercent, m_nHeightPercent);
}

void CPPHtmlStatic::SetImageList(HBITMAP hBitmap, DWORD dwIndex, LPSIZE lpSizeItem, COLORREF clrMask /* = RGB(255, 0, 255) */)
{
	ClearFrameRate();
	m_sTemplate.Format(_T("<ilst index=%d handle=0x%X mask=0x%06X cx=%d, cy=%d>"), dwIndex, hBitmap, clrMask, lpSizeItem->cx, lpSizeItem->cy);
	m_bIsImage = TRUE;
	SetImageZoom(m_nWidthPercent, m_nHeightPercent);
}

//Method was added by Steve Mayfield
void CPPHtmlStatic::ClearFrameRate()
{
  if(m_bIsRunning)
  {
    KillTimer(TIMER_ANIMATION);
    m_bIsRunning = FALSE;
  }
} //End of ClearFrameRate

//--------------------------------------------------------------------------
// CPPHtmlStatic::SetFrameRate()
//		Sets the new frame rate for the animation
//--------------------------------------------------------------------------
// Parameters:
//		dwFrameRate		 - Specified a time-out value between current and next 
//						   displayed bitmap
//--------------------------------------------------------------------------
// Sets the new frame rate for the animation. For the CPPHtmlStatic class the 
// frame rate means a time-out period in milliseconds between current and next 
// bitmap instead of number of frames per second. 
// The default value is 500 milliseconds.
//--------------------------------------------------------------------------
void CPPHtmlStatic::SetFrameRate(DWORD dwFrameRate /* = 500 */)
{
	if ((m_dwFrameRate == dwFrameRate) && m_bIsRunning)
		return;

	ClearFrameRate();
	m_dwFrameRate = dwFrameRate;
	if (dwFrameRate)
		m_bIsRunning = SetTimer(TIMER_ANIMATION, dwFrameRate, NULL);
} //End of SetAnimationSpeed

/////////////////////////////////////////////////////////////////////
// CPPHtmlStatic::SetCallbackHyperlink
// This function sets or removes the notification messages from the control before display.
//
// Parameters:
///////////////////////////////////////////////////////////////////////
void CPPHtmlStatic::SetCallbackHyperlink(UINT nMessage, LPARAM lParam /* = 0 */)
{
	TRACE(_T("CPPHtmlDrawer::SetCallbackHyperlink()\n"));
	m_drawer.SetCallbackHyperlink(GetParent()->GetSafeHwnd(), nMessage, lParam);
} //End SetCallbackHyperlink

//--------------------------------------------------------------------------
// CPPHtmlStatic::SetHyperlinkStyle
//--------------------------------------------------------------------------
void CPPHtmlStatic::SetHyperlinkStyle(COLORREF clrOrg, BOOL bOrgUnderline, COLORREF clrHover, BOOL bHoverUnderline)
{
	CString str;
	str.Format(_T("color:0x%06X; text-decoration:"), clrOrg);
	if (bOrgUnderline) str += _T("underline");
	else str += _T("none");
	m_drawer.SetTextStyle(_T("a:link"), str);
	str.Format(_T("color:0x%06X; text-decoration:"), clrHover);
	if (bHoverUnderline) str += _T("underline");
	else str += _T("none");
	m_drawer.SetTextStyle(_T("a:hover"), str);
} //End of SetHyperlinkStyle

void CPPHtmlStatic::SetBkColor(COLORREF clrBk)
{
	m_clrBk = clrBk;
	UpdateSurface();
} //End of SetBkColor

void CPPHtmlStatic::SetBkMode(int nBkMode /*= TRANSPARENT*/)
{
	m_nBkMode = nBkMode;
	UpdateSurface();
} //End of SetBkMode

void CPPHtmlStatic::SetTextColor(COLORREF clrText /*= -1*/)
{
	m_clrText = clrText;
	UpdateSurface();
} //End of SetTextColor
