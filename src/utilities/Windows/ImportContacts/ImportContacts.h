// ImportContacts.h : main header file for the ImportContacts application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols


// CImportContactsApp:
// See ImportContacts.cpp for the implementation of this class
//

class CImportContactsApp : public CWinApp
{
public:
	CImportContactsApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CImportContactsApp theApp;