// cximagedll.h : main header file for the CXIMAGEDLL DLL
//

#if !defined(AFX_CXIMAGEDLL_H__E98F71A6_B361_11D6_BB83_CAEE2E1CB77B__INCLUDED_)
#define AFX_CXIMAGEDLL_H__E98F71A6_B361_11D6_BB83_CAEE2E1CB77B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CCximagedllApp
// See cximagedll.cpp for the implementation of this class
//

class CCximagedllApp : public CWinApp
{
public:
	CCximagedllApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCximagedllApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CCximagedllApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CXIMAGEDLL_H__E98F71A6_B361_11D6_BB83_CAEE2E1CB77B__INCLUDED_)
