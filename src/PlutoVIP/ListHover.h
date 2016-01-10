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
#ifndef _LIST_HOVER_H_
#define _LIST_HOVER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListHover.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CListHover window

class CListHover : public CListCtrl
{
// Construction
public:
	CListHover();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListHover)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetParent(CWnd* pParent);
	virtual ~CListHover();

	// Generated message map functions
protected:
	CWnd* m_pParent;
	//{{AFX_MSG(CListHover)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	int Treat;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // _LIST_HOVER_H_
