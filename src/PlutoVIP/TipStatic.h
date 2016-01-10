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
#if !defined(AFX_TIPSTATIC_H__9BCE0CEE_9E83_4B71_A7DB_84A83C206AE8__INCLUDED_)
#define AFX_TIPSTATIC_H__9BCE0CEE_9E83_4B71_A7DB_84A83C206AE8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TipStatic.h : header file
//

#define IDC_TIP_STATIC	33334

/////////////////////////////////////////////////////////////////////////////
// CTipStatic window

class CTipStatic : public CStatic
{
// Construction
public:
	CTipStatic();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTipStatic)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetSliderMode(BOOL bSliderMode);
	void SetSliderColor(COLORREF colSlider);
	void AddTip(CString strTip);
	void ShowPrevTip();
	void ShowNextTip();
	virtual ~CTipStatic();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTipStatic)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

// Attributes
protected:
	COLORREF m_colSlider;
	CString m_strTipList[10];
	int m_nCurrentTip, m_nTotalTip;
	BOOL m_bSliderMode;


};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIPSTATIC_H__9BCE0CEE_9E83_4B71_A7DB_84A83C206AE8__INCLUDED_)
