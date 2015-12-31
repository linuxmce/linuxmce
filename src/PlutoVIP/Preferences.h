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
#ifndef _PREFERENCES_H_
#define _PREFERENCES_H_

#include "PageIntroduction.h"	// Added by ClassView
#include "PageDatabase.h"	// Added by ClassView
#include "PageInternet.h"	// Added by ClassView
#include "PagePanelPosition.h"	// Added by ClassView
#include "PageTerminals.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Preferences.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPreferences

class CPreferences : public CPropertySheet
{
	DECLARE_DYNAMIC(CPreferences)

// Construction
public:
	CPreferences(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CPreferences(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPreferences)
	//}}AFX_VIRTUAL

// Implementation
public:
	CPageTerminals m_Terminals;
	CPagePanelPosition m_PanelPosition;
	CPageIntroduction m_Introduction;
	CPageInternet m_Internet;
	CPageDatabase m_Database;
	virtual ~CPreferences();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPreferences)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // _PREFERENCES_H_
