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
// TipStatic.cpp : implementation file
//

#include "stdafx.h"
#include "StaticTip.h"
#include "TipStatic.h"
#include "MemDC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTipStatic

CTipStatic::CTipStatic()
{
	m_nCurrentTip = 0;
	m_nTotalTip = 0;
	m_colSlider = GetSysColor(COLOR_3DFACE);
	m_bSliderMode = FALSE;

}

CTipStatic::~CTipStatic()
{
}


BEGIN_MESSAGE_MAP(CTipStatic, CStatic)
	//{{AFX_MSG_MAP(CTipStatic)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTipStatic message handlers


void CTipStatic::ShowNextTip()
{
	CClientDC dc(this);

	CRect rc;
	GetClientRect(rc);
	
	m_nCurrentTip++;
	if (m_nCurrentTip == m_nTotalTip)
		m_nCurrentTip = 0;

	SetWindowText(m_strTipList[m_nCurrentTip]);

	////////////////////////////////////////////////
	// Get Capture Image.
	////////////////////////////////////////////////
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());
	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc);
	CBitmap *pOldBitmap = dcMem.SelectObject(&bitmap);
	dcMem.BitBlt(0,0,rc.Width(), rc.Height(), &dc, 0,0,SRCCOPY);
	/////////////////////////////////////////////////

	if (m_nCurrentTip == 0)
		SetWindowText(m_strTipList[m_nTotalTip-1]);
	else
		SetWindowText(m_strTipList[m_nCurrentTip-1]);

	int x = 0;

	CPen pen(PS_SOLID, 2, m_colSlider);
	CPen *oldPen;
	oldPen = dc.SelectObject(&pen);

	// Left to Right
	if (m_bSliderMode == TRUE)
	{
		while( x <= rc.Width() )
		{
			dc.MoveTo(x, 0);
			dc.LineTo(x, rc.Height()-1);
			x += 2;
			::Sleep(1);
		} // end of while
	}

	x = rc.Width();

	while ( x >= 0 )
	{
		dc.BitBlt(x,0,2, rc.Height(), &dcMem,x,0,SRCCOPY);
		x -= 2;
		::Sleep(1);
	} // end of while

	dc.BitBlt(0,0,1, rc.Height(), &dcMem,0,0,SRCCOPY);

	dc.SelectObject(oldPen);
	dcMem.SelectObject(pOldBitmap);
}

void CTipStatic::ShowPrevTip()
{
	CClientDC dc(this);

	CRect rc;
	GetClientRect(rc);
	
	m_nCurrentTip--;
	if (m_nCurrentTip == -1)
		m_nCurrentTip = m_nTotalTip -1;

	SetWindowText(m_strTipList[m_nCurrentTip]);

	////////////////////////////////////////////////
	// Get Capture Image.
	////////////////////////////////////////////////
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());
	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc);
	CBitmap *pOldBitmap = dcMem.SelectObject(&bitmap);
	dcMem.BitBlt(0,0,rc.Width(), rc.Height(), &dc, 0,0,SRCCOPY);
	/////////////////////////////////////////////////

	if (m_nCurrentTip == m_nTotalTip -1)
		SetWindowText(m_strTipList[0]);
	else
		SetWindowText(m_strTipList[m_nCurrentTip+1]);

	int x = rc.Width()-1;

	CPen pen(PS_SOLID, 2, m_colSlider);
	CPen *oldPen;
	oldPen = dc.SelectObject(&pen);

	// Left to Right

	if (m_bSliderMode == TRUE)
	{
		while( x >= 0 )
		{
			dc.MoveTo(x, 0);
			dc.LineTo(x, rc.Height()-1);
			x -= 2;
			::Sleep(1);
		} // end of while
	}

	x = 0;

	while ( x <= rc.Width()+1 )
	{
		dc.BitBlt(x,0,2, rc.Height(), &dcMem,x,0,SRCCOPY);
		x += 2;
		::Sleep(1);
	} // end of while
//	dc.BitBlt(rc.Width(),0,1, rc.Height(), &dcMem,0,0,SRCCOPY);

	dc.SelectObject(oldPen);
	dcMem.SelectObject(pOldBitmap);
}


void CTipStatic::AddTip(CString strTip)
{
	m_strTipList[m_nTotalTip] = strTip;

	if (m_nTotalTip == 0)
		SetWindowText(strTip);

	m_nTotalTip++;
}

void CTipStatic::SetSliderColor(COLORREF colSlider)
{
	m_colSlider = colSlider;
}


void CTipStatic::SetSliderMode(BOOL bSliderMode)
{
	m_bSliderMode = bSliderMode;

}
