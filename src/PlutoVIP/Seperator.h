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
#ifndef _SEPERATOR_H_
#define _SEPERATOR_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Seperator.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSeperator window

class CSeperator : public CStatic
{
// Construction
public:
	CSeperator();

// Attributes
public:

protected:
	// text color
	COLORREF m_clrText;
	// color of top/left line of the separator
	COLORREF m_clrTopLeft;
	// color of bottom/right line of the separator
	COLORREF m_clrBottomRight;
	// flag that specifies whether the separator should be drawn 
	// vertically (the control is horizontally oriented by default)
	BOOL m_bVertOriented;

private:
	// window's text
	CString m_sText;


// Operations
public:
	inline BOOL SetTextColor(COLORREF clrText=::GetSysColor(COLOR_WINDOWTEXT), 
		BOOL bRedraw=FALSE) 
	{ 
		if(bRedraw && m_clrText==clrText)
			bRedraw=FALSE;
		m_clrText=clrText;
		if(bRedraw)
			RedrawWindow();
		return TRUE;
	}

	inline BOOL GetTextColor(COLORREF& clrText) const 
	{ 
		clrText=m_clrText; 
		return TRUE;
	}

	inline BOOL SetSeparatorColors(COLORREF clrTopLeft=::GetSysColor(COLOR_BTNSHADOW), 
		COLORREF clrBottomRight=::GetSysColor(COLOR_BTNHILIGHT), BOOL bRedraw=FALSE) 
	{ 
		if(bRedraw && m_clrTopLeft==clrTopLeft && 
			m_clrBottomRight==clrBottomRight)
			bRedraw=FALSE;
		m_clrTopLeft=clrTopLeft; 
		m_clrBottomRight=clrBottomRight;
		if(bRedraw)
			RedrawWindow();
		return TRUE;
	}

	inline BOOL GetSeparatorColors(COLORREF& clrTopLeft, COLORREF& clrBottomRight) const 
	{ 
		clrTopLeft=m_clrTopLeft; 
		clrBottomRight=m_clrBottomRight;
		return TRUE;
	}

	inline void SetVertOriented(BOOL bVertOriented=TRUE, BOOL bRedraw=FALSE)
	{
		if(bRedraw && m_bVertOriented==bVertOriented)
			bRedraw=FALSE;
		m_bVertOriented=bVertOriented;
		if(bRedraw)
			RedrawWindow();
	}

	inline BOOL IsVertOriented() const
	{
		return m_bVertOriented;
	}


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSeperator)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSeperator();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSeperator)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg LONG OnSetText(UINT wParam, LONG lParam);
	afx_msg LONG OnGetText(UINT wParam, LONG lParam);
	afx_msg LONG OnGetTextLength(UINT wParam, LONG lParam);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // _SEPERATOR_H_
