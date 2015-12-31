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
// VIPDesignView.h : interface of the CVIPDesignView class
//


#pragma once

#include "VIPShared/RenderMenu.h"

class CVIPDesignView : public CView, public RenderMenu
{
protected: // create from serialization only
	CVIPDesignView();
	DECLARE_DYNCREATE(CVIPDesignView)

	CPaintDC *m_dc;
	HGDIOBJ m_hOriginalFont;
// Attributes
public:
	CVIPDesignDoc* GetDocument() const;

// Operations
public:

// Overrides
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CVIPDesignView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void Redraw();
	virtual void MessageBox(const char *Message,const char *Title);
	virtual bool DrawImage(const char *Filename,int X,int Y,int Width,int Height);
	virtual bool DrawImage(unsigned char ImageType,void *pGraphic,int GraphicSize,int X,int Y,int Width,int Height);
	virtual bool DrawText(const char *Text, MyRect &r);
	virtual void SetTextProperties(int Size,const char *Font,int R, int G, int B);
	virtual void SwitchToMenu(VIPMenu *pMenu);
	virtual bool Draw3dRect(MyRect &r);

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	char NumericToText(char Key,const char *Value);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
};

#ifndef _DEBUG  // debug version in VIPDesignView.cpp
inline CVIPDesignDoc* CVIPDesignView::GetDocument() const
   { return reinterpret_cast<CVIPDesignDoc*>(m_pDocument); }
#endif

