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
// TaskBar.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "PlutoUtils/CommonIncludes.h"	
#include "VIPShared/VIPIncludes.h"
#include "VIPShared/PlutoConfig.h"
#include "DCE/Logger.h"
#include "PlutoVIP.h"
#include "PlutoVIPMain.h"
#include "Registry.h"
#include <vector>
#include "ImageDefs.h"
#include "ADO2.h"

using namespace std;
using namespace DCE;
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTaskBarApp

BEGIN_MESSAGE_MAP(CTaskBarApp, CWinApp)
	//{{AFX_MSG_MAP(CTaskBarApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTaskBarApp construction

CTaskBarApp::CTaskBarApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// CTaskBarApp destruction

CTaskBarApp::~CTaskBarApp()
{
	delete g_pPlutoConfig;
	g_pPlutoConfig = NULL;

	
	
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CTaskBarApp object

CTaskBarApp  theApp;

PlutoConfig* g_pPlutoConfig=new PlutoConfig();

//image list
//CImageList		PlutoVIPImageList;
map<unsigned long  /*PlutoID*/, PlutoCustomer*> mapPlutoImageList;
/////////////////////////////////////////////////////////////////////////////
// CTaskBarApp initialization

BOOL CTaskBarApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Initialize OLE
	AfxOleInit();
	
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	
	InitCommonControls();

	CWinApp::InitInstance();
	AfxEnableControlContainer();
	

	//prepare the registry
	PrepareRegistry();
	
	//Create Image List
	//PlutoVIPImageList.Create(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT, ILC_COLOR24, 100, 10);

	long err;
	WSADATA wsaData;
	err = WSAStartup(MAKEWORD( 1, 1 ),(LPWSADATA)  &wsaData);
	
	g_pPlutoConfig->m_iEstablishmentID=32880;
	g_pPlutoConfig->m_iSoftwareVersion=1;

	CPlutoVIPMain dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

void CTaskBarApp::PrepareRegistry()
{
	CRegistry registry;
	if (registry.OpenKey(HKEY_CURRENT_USER, REGISTRY_KEY_VIPSERVICE_FULL))
	{
		//set the application version
		registry.SetValueString(PLUTOVIP_VERSION_FULL, PLUTOVIP_VERSION_FULL_NUMBER);
		registry.SetValueString(PLUTOVIP_VERSION_VERSION, PLUTOVIP_VERSION_VERSION_NUMBER);
		registry.SetValueString(PLUTOVIP_VERSION_MAJOR, PLUTOVIP_VERSION_MAJOR_NUMBER);
		registry.SetValueString(PLUTOVIP_VERSION_MINOR, PLUTOVIP_VERSION_MINOR_NUMBER);
		registry.SetValueString(PLUTOVIP_VERSION_BUILD, PLUTOVIP_VERSION_BUILD_NUMBER);
	}
	else
	{
		DWORD dummy=0;
		registry.CreateNewKey(HKEY_CURRENT_USER, REGISTRY_KEY_VIPSERVICE_FULL, &dummy);
		registry.SetValueString(PLUTOVIP_VERSION_FULL, PLUTOVIP_VERSION_FULL_NUMBER);
		registry.SetValueString(PLUTOVIP_VERSION_VERSION, PLUTOVIP_VERSION_VERSION_NUMBER);
		registry.SetValueString(PLUTOVIP_VERSION_MAJOR, PLUTOVIP_VERSION_MAJOR_NUMBER);
		registry.SetValueString(PLUTOVIP_VERSION_MINOR, PLUTOVIP_VERSION_MINOR_NUMBER);
		registry.SetValueString(PLUTOVIP_VERSION_BUILD, PLUTOVIP_VERSION_BUILD_NUMBER);
		registry.SetValueDWORD(REGISTRY_PAGE_PANEL_POSITION_RECENT_TIME, 5 /*5 minutes*/);
		registry.SetValueDWORD(REGISTRY_PAGE_PANEL_POSITION_NEW_TIME, 5 /*5 minutes*/);
	}

	if (registry.QueryValue(REGISTRY_PAGE_PANEL_POSITION_NEW_SIDE,
							NULL, NULL, NULL)!=ERROR_SUCCESS)
	{
		registry.SetValueDWORD(REGISTRY_PAGE_PANEL_POSITION_NEW_SIDE, 
			APPBAR_POSITION_LEFT);
	}

	if (registry.QueryValue(REGISTRY_PAGE_PANEL_POSITION_ACTIVE_SIDE,
							NULL, NULL, NULL)!=ERROR_SUCCESS)
	{
		registry.SetValueDWORD(REGISTRY_PAGE_PANEL_POSITION_ACTIVE_SIDE, 
			APPBAR_POSITION_TOP);
	}

	if (registry.QueryValue(REGISTRY_PAGE_PANEL_POSITION_RECENT_SIDE,
							NULL, NULL, NULL)!=ERROR_SUCCESS)
	{
		registry.SetValueDWORD(REGISTRY_PAGE_PANEL_POSITION_RECENT_SIDE, 
			APPBAR_POSITION_RIGHT);
	}

	if (registry.QueryValue(REGISTRY_PAGE_TERMINALS_DIRECTORY,
							NULL, NULL, NULL)!=ERROR_SUCCESS)
	{
		registry.SetValueString(REGISTRY_PAGE_TERMINALS_DIRECTORY, 
			_T(""));
	}

	if (registry.QueryValue(REGISTRY_PAGE_TERMINALS_SERVER,
							NULL, NULL, NULL)!=ERROR_SUCCESS)
	{
		registry.SetValueString(REGISTRY_PAGE_TERMINALS_SERVER, 
			_T(""));
	}

	if (registry.QueryValue(REGISTRY_PAGE_PANEL_POSITION_NEW_TIME,
		NULL, NULL, NULL)!=ERROR_SUCCESS)
	{
		registry.SetValueDWORD(REGISTRY_PAGE_PANEL_POSITION_NEW_TIME, 5);
	}

	if (registry.QueryValue(REGISTRY_PAGE_PANEL_POSITION_RECENT_TIME,
		NULL, NULL, NULL)!=ERROR_SUCCESS)
	{
		registry.SetValueDWORD(REGISTRY_PAGE_PANEL_POSITION_NEW_TIME, 5);
	}
	registry.CloseKey();
}

int CTaskBarApp::ExitInstance() 
{
	//destroy the image list
	//PlutoVIPImageList.DeleteImageList();
	
	return CWinApp::ExitInstance();
}


