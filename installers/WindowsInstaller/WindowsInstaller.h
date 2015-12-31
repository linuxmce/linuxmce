// WindowsInstaller.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include <vector>
#include "afxtempl.h"
using namespace std;

bool HandleCancel(CDialog *pDialog);

class PackageInfos
{
public:
	CString sPackageID;
	CString sPackageName;
	CString sURL;
	CString sComments;
	CString sRepository;
	CString sRepositoryType;
	CString sVersion;
	CString sBinaryExecutiblesPathPath;
	CString sSourceIncludesPath;
	CString sSourceImplementationPath;
	CString sBinaryLibraryPath;
	CString sConfiguration;
	CString sRepositorySourceUsername;
	CString sRepositorySourcePassword;
	CString sParams;
	CString sPackageSourceDescription;
};

extern vector<PackageInfos> vectPackageInfos;
extern CArray<int,int> aryListBoxSel;
extern const string csNotSpecified;

// CWindowsInstallerApp:
// See WindowsInstaller.cpp for the implementation of this class
//

class CWindowsInstallerApp : public CWinApp
{
public:
	CWindowsInstallerApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CWindowsInstallerApp theApp;