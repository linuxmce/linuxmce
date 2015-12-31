// OrbiterCEInstaller.h : main header file for the ORBITERCEINSTALLER application
//

#if !defined(AFX_ORBITERCEINSTALLER_H__9415986D_3FA1_40A6_AA7C_8F86DC0CA1CC__INCLUDED_)
#define AFX_ORBITERCEINSTALLER_H__9415986D_3FA1_40A6_AA7C_8F86DC0CA1CC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// COrbiterCEInstallerApp:
// See OrbiterCEInstaller.cpp for the implementation of this class
//

class COrbiterCEInstallerApp : public CWinApp
{
public:
	COrbiterCEInstallerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COrbiterCEInstallerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(COrbiterCEInstallerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ORBITERCEINSTALLER_H__9415986D_3FA1_40A6_AA7C_8F86DC0CA1CC__INCLUDED_)
