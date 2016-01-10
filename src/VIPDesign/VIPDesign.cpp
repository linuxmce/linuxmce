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
// VIPDesign.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "VIPShared/VIPIncludes.h"
#include "VIPShared/VIPMenu.h"
#include "VIPShared/PlutoConfig.h"
#include "PlutoUtils/StringUtils.h"

#include "VIPDesign.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "VIPDesignDoc.h"
#include "VIPDesignView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class PlutoConfig *g_pPlutoConfig = new PlutoConfig();

// CVIPDesignApp

BEGIN_MESSAGE_MAP(CVIPDesignApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()


// CVIPDesignApp construction

CVIPDesignApp::CVIPDesignApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CVIPDesignApp object

CVIPDesignApp theApp;
void* EmulatorThread(void* param); // Defined in VIPEmulator.cpp

// CVIPDesignApp initialization

BOOL CVIPDesignApp::InitInstance()
{
	g_pPlutoConfig = new PlutoConfig();

	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_VIPDesignTYPE,
		RUNTIME_CLASS(CVIPDesignDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CVIPDesignView));
	AddDocTemplate(pDocTemplate);
	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;
	// call DragAcceptFiles only if there's a suffix
	//  In an MDI app, this should occur immediately after setting m_pMainWnd
	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();
	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);
	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

/*
	// Get a handle to the DLL module.
    m_hinstPlutoVIPLib = LoadLibrary("C:\\ha\\level4\\VIP_POS\\Debug\\VIP_POS.dll"); 
 
    // If the handle is valid, try to get the function address.
    if (m_hinstPlutoVIPLib != NULL) 
    { 
        m_fnPlutoMemberIdentified = (PLUTO_MEMBER_IDENTIFIED) GetProcAddress(hinstLib, "fnPlutoMemberIdentified"); 
 
        // If the function address is valid, call the function.
 
        if (fRunTimeLinkSuccess = (RegisterCallbackAdd != NULL)) 
           foo = (RegisterCallbackAdd) (282,12,0,
		   MemberIdentified,0,
		   PaymentRequestCompleted,0,
		   NULL,0); 

        // Free the DLL module.
 
        fFreeResult = FreeLibrary(hinstLib); 
    } 
 
 */

	// The main window has been initialized, so show and update it
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	g_pPlutoConfig->m_hWnd_Emulator=pMainFrame->m_hWnd;
	g_pPlutoConfig->m_pDocTemplate=pDocTemplate;
	g_pPlutoConfig->m_pMainFrame=pMainFrame;
#ifdef EMULATOR
	if(pthread_create(&m_pthread_emulator, NULL, EmulatorThread, (void*)this))
	{
		return FALSE;
	}
#endif

	return TRUE;
}



// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// App command to run the dialog
void CVIPDesignApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

