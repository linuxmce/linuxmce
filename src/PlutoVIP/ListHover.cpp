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
// ListHover.cpp : implementation file
//

#include "stdafx.h"
#include "TaskBar.h"
#include "ListHover.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListHover

CListHover::CListHover()
{
	Treat=0;
}

CListHover::~CListHover()
{
}


BEGIN_MESSAGE_MAP(CListHover, CListCtrl)
	//{{AFX_MSG_MAP(CListHover)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListHover message handlers

void CListHover::SetParent(CWnd *pParent)
{
	ASSERT(pParent!=NULL);
	m_pParent=pParent;
}


//DEL void CListHover::OnMouseHover(WPARAM wparam, LPARAM lparam) 
//DEL {
//DEL 	if (Treat==0)
//DEL 	{
//DEL 		::SendMessage(m_hWnd, WM_LBUTTONDBLCLK, MK_LBUTTON, 0);
//DEL 		Treat=1;
//DEL 	}	
//DEL }


//DEL LRESULT CListHover::OnMouseLeave(WPARAM wparam, LPARAM lparam)
//DEL {
//DEL 	//if mouse is in dialog return without doing anything
//DEL 	//but if mouse is outside of dialog set timer to kill dialog
//DEL 	POINT point;
//DEL 	GetCursorPos(&point);
//DEL 	
//DEL 	CRect rect;
//DEL 	GetClientRect(&rect);
//DEL 	ClientToScreen(&rect);
//DEL 	
//DEL 	Treat=0;
//DEL 
//DEL 	if (rect.PtInRect(point))
//DEL 	{
//DEL 		TRACKMOUSEEVENT tme;
//DEL 		tme.cbSize = sizeof(tme);
//DEL 		tme.hwndTrack = m_hWnd;
//DEL 		tme.dwFlags = /*TME_LEAVE|*/TME_HOVER;
//DEL 		tme.dwHoverTime = 1;
//DEL 		_TrackMouseEvent(&tme);
//DEL 		return 0;
//DEL 	}
//DEL 	
//DEL 	return 0;
//DEL }

//DEL int CListHover::OnCreate(LPCREATESTRUCT lpCreateStruct) 
//DEL {
//DEL 	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
//DEL 		return -1;
//DEL 	
//DEL 	TRACKMOUSEEVENT tme;
//DEL 	tme.cbSize = sizeof(tme);
//DEL 	tme.hwndTrack = m_hWnd;
//DEL 	tme.dwFlags = TME_LEAVE|TME_HOVER;
//DEL 	tme.dwHoverTime = 10;
//DEL 	_TrackMouseEvent(&tme);
//DEL 	
//DEL 	::SendMessage(m_hWnd, WM_LBUTTONDBLCLK, MK_LBUTTON, 0);
//DEL 	
//DEL 	return 0;
//DEL }
