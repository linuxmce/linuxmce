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
// PlutoVIPMain.cpp : implementation file
//

#include "stdafx.h"
#include "PlutoUtils/CommonIncludes.h"	
#include "VIPShared/VIPIncludes.h"
#include "VIPShared/PlutoConfig.h"
#include "DCE/Logger.h"
#include "PlutoVIPMain.h"
#include "TrayIcon.h"
#include "Preferences.h"
#include "BindStatusCallback.h"
#include "Registry.h"
#include "ExternalLibraries.h"
#include "UpdateDlg.h"
#include "AnimEffect.h"
#include "UtilityFunc.h"
#include "AppBar.h"

#include "RAS.h"
#include "RASError.h"

#pragma comment(lib, "RASAPI32.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//hack
#include "VA_SendMenuToPhone.h"


UINT CheckVersion(LPVOID pParam);
void CheckAppVersion(CString szFileName);
BOOL IsRASConnectionEstablished();

LONG g_fAbortDownload=0;
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlutoVIPMain dialog

CPlutoVIPMain::CPlutoVIPMain(CWnd* pParent /*=NULL*/)
	: CDialog(CPlutoVIPMain::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPlutoVIPMain)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
/*
	VR_ShowMenu req("C:\\test.vmc");
	// 3660: 413789114352
	// 3650: 413780833863
	VA_SendMenuToPhone va(0,&req,413789114352);
	va.ProcessAction();
*/
	g_TaskBars.Init();

	//m_pPhoneDetection_VIP = NULL;
	//pTaskbar=NULL;
}

CPlutoVIPMain::~CPlutoVIPMain()
{
	//do we need to destroy m_pPhoneDetection_VIP or
	//it will destroy itself?
	if (m_pPhoneDetection_VIP)
	{
		delete m_pPhoneDetection_VIP;
		m_pPhoneDetection_VIP=NULL;
	}
}


void CPlutoVIPMain::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPlutoVIPMain)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPlutoVIPMain, CDialog)
	//{{AFX_MSG_MAP(CPlutoVIPMain)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_COMMAND(ID_HELP_ABOUT, OnHelpAbout)
	ON_COMMAND(ID_HELP_CONTENTS, OnHelpContents)
	ON_COMMAND(ID_HELP_PLUTOVIPONTHEWEB_FREQUENTLYASKEDQUESTION, OnHelpPlutoVIPFAQ)
	ON_COMMAND(ID_HELP_PLUTOVIPONTHEWEB_FREESTUFF, OnHelpPlutoVIPFreeStuff)
	ON_COMMAND(ID_HELP_PLUTOVIPONTHEWEB_ONLINESUPPORT, OnHelpPlutoVIPOnlineSupport)
	ON_COMMAND(ID_HELP_PLUTOVIPONTHEWEB_PLUTOVIPHOMEPAGE, OnHelpPlutoviponthewebPlutoVIPHomepage)
	ON_COMMAND(ID_HELP_PLUTOVIPONTHEWEB_PRODUCTNEWS, OnHelpPlutoVIPProductNews)
	ON_COMMAND(ID_HELP_TIPOFTHEDAY, OnHelpTipOfTheDay)
	ON_COMMAND(ID_HELP_CHECK_UPDATE, OnCheckUpdateVersion)
	ON_COMMAND(ID_MAIN_EXIT, OnExit)
	ON_COMMAND(ID_MAIN_PREFERENCES, OnPreferences)
	ON_COMMAND(ID_TRAY_ABOUT, OnTrayAbout)
	ON_COMMAND(ID_TRAY_EXIT, OnTrayExit)
	ON_COMMAND(ID_TRAY_PREFERENCES, OnTrayPreferences)
	ON_COMMAND(ID_TRAY_RESTORE, OnTrayRestore)
	ON_COMMAND(ID_MAIN_MINIMIZED, OnMinimized)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_TRAYICON, OnTrayNotification)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlutoVIPMain message handlers

void CPlutoVIPMain::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	if (cx==0 && cy==0)		//minimize the window
		//ShowWindow(SW_HIDE);
	{
		
		//MakeWindowTransparent(m_hWnd, 0);
		//PostMessage(WM_SIZE, SIZE_RESTORED);
	}
}

BOOL CPlutoVIPMain::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	
	//Initialize tray icon
	InitializeTrayIcon();

	//Move the main dialog to center
	CenterWindow();
	
	//Launch three AppBars
	RegisterAppBars();

	//Check version for product update if connection was established before
	if (CheckInternetConnection()==TRUE)
		AfxBeginThread(CheckVersion, this);

	//Run Phone Detection Routine
	DetectPhones();
	

	//CoInitialize(0);
	//We call below function since we only need to create one object
	//CoCreateInstance(CLSID_TaskbarList,0, 
	//	CLSCTX_INPROC_SERVER,IID_ITaskbarList,(void**)&pTaskbar);
	//Below function will initialize the taskbar list object
	//pTaskbar->HrInit();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPlutoVIPMain::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPlutoVIPMain::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPlutoVIPMain::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


void CPlutoVIPMain::OnDestroy() 
{
	UnregisterAppBars();
	
	CRect rc;
	GetWindowRect( rc );
	ShowWindow(SW_HIDE);

	AnimEffect ae;
	ae.Open(rc);
	ae.Effect(AnimEffect::EffectType::Random);
	ae.Close(rc);

	g_TaskBars.Destroy();

	Shell_NotifyIcon(NIM_DELETE, &m_TrayIcon);	

	CDialog::OnDestroy();	
}

void CPlutoVIPMain::RegisterAppBars()
{
	CRegistry registry;
	int NewSide, RecentSide, ActiveSide, NewApp, RecentApp, ActiveApp;
	NewSide=RecentSide=ActiveSide=0;
	NewApp=RecentApp=ActiveApp=0;
	
	int NewTimer=1, RecentTimer=1;
	
	registry.OpenKey(HKEY_CURRENT_USER, REGISTRY_KEY_VIPSERVICE_FULL);
	
	//AppBar position
	NewSide=registry.GetValueDWORD(REGISTRY_PAGE_PANEL_POSITION_NEW_SIDE);
	RecentSide=registry.GetValueDWORD(REGISTRY_PAGE_PANEL_POSITION_RECENT_SIDE);
	ActiveSide=registry.GetValueDWORD(REGISTRY_PAGE_PANEL_POSITION_ACTIVE_SIDE);

	//AppBar appearance
	NewApp=registry.GetValueDWORD(REGISTRY_PAGE_PANEL_POSITION_NEW_APPEARANCE);
	RecentApp=registry.GetValueDWORD(REGISTRY_PAGE_PANEL_POSITION_RECENT_APPEARANCE);
	ActiveApp=registry.GetValueDWORD(REGISTRY_PAGE_PANEL_POSITION_ACTIVE_APPEARANCE);

	//AppBar Timer
	NewTimer=registry.GetValueDWORD(REGISTRY_PAGE_PANEL_POSITION_NEW_TIME);
	RecentTimer=registry.GetValueDWORD(REGISTRY_PAGE_PANEL_POSITION_RECENT_TIME);
	
	registry.CloseKey();
	
	// 0=not detected anymore, 1=new app bar, 2=active, 3=recent
	if (g_TaskBars.m_NewCustomer == NULL)
    {
        g_TaskBars.m_NewCustomer = new CControlsDlg;
        ASSERT (g_TaskBars.m_NewCustomer);
		
		g_TaskBars.m_NewCustomer->SetImageType(IMAGE_TYPE_NEW);
		g_TaskBars.m_NewCustomer->SetDesktopSide(NewSide);
        g_TaskBars.m_NewCustomer->SetParent(this);
		g_TaskBars.m_NewCustomer->SetAppBarName(_T("New"));
		g_TaskBars.m_NewCustomer->Create(IDD_APPBAR, GetDesktopWindow());
		g_TaskBars.m_NewCustomer->RunTimer(NewTimer);
		
		
		switch (NewApp)
		{
			case APPBAR_APPEARANCE_VISIBLE:
				//m_NewCustomer->SetAlwaysOnTop(TRUE);
				//m_NewCustomer->SetWindowVisible();
				break;

			case APPBAR_APPEARANCE_AUTOHIDE:
				//m_NewCustomer->SetAlwaysOnTop(TRUE);
				//m_NewCustomer->SetAutoHide(TRUE);
				break;

			case APPBAR_APPEARANCE_INVISIBLE:
				//m_NewCustomer->SetWindowInvisible();
				break;
		}
    }
    
	if (g_TaskBars.m_RecentCustomer == NULL)
    {
        g_TaskBars.m_RecentCustomer = new CControlsDlg;
        ASSERT (g_TaskBars.m_RecentCustomer);
		
		g_TaskBars.m_RecentCustomer->SetImageType(IMAGE_TYPE_RECENT);
		g_TaskBars.m_RecentCustomer->SetDesktopSide(RecentSide);
        g_TaskBars.m_RecentCustomer->SetParent(this);
		g_TaskBars.m_RecentCustomer->SetAppBarName(_T("Recent"));
		g_TaskBars.m_RecentCustomer->Create (IDD_APPBAR, GetDesktopWindow());
		g_TaskBars.m_RecentCustomer->RunTimer(RecentTimer);
		
		switch (RecentApp)
		{
		case APPBAR_APPEARANCE_VISIBLE:
			//m_RecentCustomer->SetAlwaysOnTop(TRUE);
			//m_RecentCustomer->SetWindowVisible();
			break;
			
		case APPBAR_APPEARANCE_AUTOHIDE:
			//m_NewCustomer->SetAlwaysOnTop(TRUE);
			//m_RecentCustomer->SetAutoHide(TRUE);
			break;
			
		case APPBAR_APPEARANCE_INVISIBLE:
			//m_RecentCustomer->SetWindowInvisible();
			break;
		}
    }
	
	if (g_TaskBars.m_ActiveCustomer == NULL)
    {
		g_TaskBars.m_ActiveCustomer = new CControlsDlg;
		ASSERT (g_TaskBars.m_ActiveCustomer);
		
		g_TaskBars.m_ActiveCustomer->SetImageType(IMAGE_TYPE_ACTIVE);
		g_TaskBars.m_ActiveCustomer->SetDesktopSide(ActiveSide);
		g_TaskBars.m_ActiveCustomer->SetAppBarName(_T("Active"));
		g_TaskBars.m_ActiveCustomer->Create (IDD_APPBAR, GetDesktopWindow());
		g_TaskBars.m_ActiveCustomer->SetParent(this);
		
		
		switch (ActiveApp)
		{
		case APPBAR_APPEARANCE_VISIBLE:
			//m_ActiveCustomer->SetAlwaysOnTop(TRUE);
			//m_ActiveCustomer->SetWindowVisible();
			break;
			
		case APPBAR_APPEARANCE_AUTOHIDE:
			//m_NewCustomer->SetAlwaysOnTop(TRUE);
			//m_ActiveCustomer->SetAutoHide(TRUE);
			break;
			
		case APPBAR_APPEARANCE_INVISIBLE:
			//m_ActiveCustomer->SetWindowInvisible();
			break;
		}
    }
}

void CPlutoVIPMain::UnregisterAppBars()
{
	if (g_TaskBars.m_NewCustomer)
    {
        g_TaskBars.m_NewCustomer->SendMessage (WM_DESTROY, 0, 0L);
        delete g_TaskBars.m_NewCustomer;
        g_TaskBars.m_NewCustomer = NULL;
    }

	if (g_TaskBars.m_RecentCustomer)
    {
        g_TaskBars.m_RecentCustomer->SendMessage (WM_DESTROY, 0, 0L);
        delete g_TaskBars.m_RecentCustomer;
        g_TaskBars.m_RecentCustomer = NULL;
    }

	if (g_TaskBars.m_ActiveCustomer)
    {
        g_TaskBars.m_ActiveCustomer->SendMessage (WM_DESTROY, 0, 0L);
        delete g_TaskBars.m_ActiveCustomer;
        g_TaskBars.m_ActiveCustomer = NULL;
    }
}

void CPlutoVIPMain::OnHelpAbout() 
{
	CAboutDlg about;
	about.DoModal();
}

void CPlutoVIPMain::OnHelpContents() 
{
	ShellExecute(NULL, _T("open"), PLUTOVIP_HELP, NULL, NULL, SW_SHOW);	
}

void CPlutoVIPMain::OnHelpPlutoVIPFAQ() 
{
	ShellExecute(NULL, _T("open"), PLUTOVIP_WEB_FAQ, NULL, NULL, SW_SHOW);	
}

void CPlutoVIPMain::OnHelpPlutoVIPFreeStuff() 
{
	ShellExecute(NULL, _T("open"), PLUTOVIP_WEB_FREE_STUFF, NULL, NULL, SW_SHOW);	
}

void CPlutoVIPMain::OnHelpPlutoVIPOnlineSupport() 
{
	ShellExecute(NULL, _T("open"), PLUTOVIP_WEB_ONLINE_SUPPORT, NULL, NULL, SW_SHOW);	
}

void CPlutoVIPMain::OnHelpPlutoviponthewebPlutoVIPHomepage() 
{
	ShellExecute(NULL, _T("open"), PLUTOVIP_WEB_HOMEPAGE, NULL, NULL, SW_SHOW);
}

void CPlutoVIPMain::OnHelpPlutoVIPProductNews() 
{
	ShellExecute(NULL, _T("open"), PLUTOVIP_WEB_PRODUCT_NEWS, NULL, NULL, SW_SHOW);	
}

void CPlutoVIPMain::OnHelpTipOfTheDay() 
{
	// TODO: Add your command handler code here
	
}

void CPlutoVIPMain::OnExit() 
{
	int iResult=AfxMessageBox("Are you sure?\n", 
		MB_YESNO | MB_ICONASTERISK);

	if (iResult==IDYES)
		OnTrayExit();
}

void CPlutoVIPMain::OnPreferences() 
{
	DWORD NewOld, RecentOld, ActiveOld;
	DWORD NewNew, RecentNew, ActiveNew;

	CRegistry reg;
	reg.OpenKey(HKEY_CURRENT_USER, REGISTRY_KEY_VIPSERVICE_FULL);
	if (reg.IsOpen())
	{
		NewOld=reg.GetValueDWORD(REGISTRY_PAGE_PANEL_POSITION_NEW_APPEARANCE);
		RecentOld=reg.GetValueDWORD(REGISTRY_PAGE_PANEL_POSITION_RECENT_APPEARANCE);
		ActiveOld=reg.GetValueDWORD(REGISTRY_PAGE_PANEL_POSITION_ACTIVE_APPEARANCE);
	}

	CPreferences pref(_T("Preferences"));	
	pref.DoModal();

	if (reg.IsOpen())
	{
		NewNew=reg.GetValueDWORD(REGISTRY_PAGE_PANEL_POSITION_NEW_APPEARANCE);
		RecentNew=reg.GetValueDWORD(REGISTRY_PAGE_PANEL_POSITION_RECENT_APPEARANCE);
		ActiveNew=reg.GetValueDWORD(REGISTRY_PAGE_PANEL_POSITION_ACTIVE_APPEARANCE);
	}

	APPBARSTATE abs;
	abs.m_cbSize = sizeof(abs);
	
	if (NewNew!=NewOld)
		if (NewNew==APPBAR_APPEARANCE_AUTOHIDE)
		{
			g_TaskBars.m_NewCustomer->GetState(&abs);
			abs.m_fAutohide = TRUE;
			g_TaskBars.m_NewCustomer->SetState(abs);
		}
		else
		{
			g_TaskBars.m_NewCustomer->GetState(&abs);
			abs.m_fAutohide = FALSE;
			g_TaskBars.m_NewCustomer->SetState(abs);
		}
		
	if (RecentNew!=RecentOld)
		if (RecentNew==APPBAR_APPEARANCE_AUTOHIDE)
		{
			g_TaskBars.m_RecentCustomer->GetState(&abs);
			abs.m_fAutohide = TRUE;
			g_TaskBars.m_RecentCustomer->SetState(abs);
		}
		else
		{
			g_TaskBars.m_RecentCustomer->GetState(&abs);
			abs.m_fAutohide = FALSE;
			g_TaskBars.m_RecentCustomer->SetState(abs);
		}
			
	if (ActiveNew!=ActiveOld)
		if (ActiveNew==APPBAR_APPEARANCE_AUTOHIDE)
		{
			g_TaskBars.m_ActiveCustomer->GetState(&abs);
			abs.m_fAutohide = TRUE;
			g_TaskBars.m_ActiveCustomer->SetState(abs);
		}
		else
		{
			g_TaskBars.m_ActiveCustomer->GetState(&abs);
			abs.m_fAutohide = FALSE;
			g_TaskBars.m_ActiveCustomer->SetState(abs);
		}			
}

void CPlutoVIPMain::OnMinimized()
{
	ShowWindow(SW_HIDE);
	//pTaskbar->DeleteTab(m_hWnd);
	//MakeWindowTransparent(m_hWnd, 0);
}

void CPlutoVIPMain::OnTrayAbout() 
{
	CAboutDlg about;
	about.DoModal();	
}

void CPlutoVIPMain::OnTrayExit() 
{
	Shell_NotifyIcon(NIM_DELETE, &m_TrayIcon);
	OnOK();	
}

void CPlutoVIPMain::OnTrayPreferences() 
{
	CPreferences pref(_T("Preferences"));	
	pref.DoModal();	
}

void CPlutoVIPMain::OnTrayRestore() 
{
	//restore main dialog
	//ShowWindow(SW_NORMAL);
	//pTaskbar->AddTab(m_hWnd);
	//pTaskbar->ActivateTab(m_hWnd);
	//MakeWindowTransparent(m_hWnd, 0xFF);
	ShowWindow(SW_NORMAL);
	Invalidate();
}

LRESULT CPlutoVIPMain::OnTrayNotification(WPARAM wParam, LPARAM lParam) 
{
	// Return quickly if its not for this tray icon
    if (wParam != m_TrayIcon.uID) 
	{
        return 0L;
    }
	
    switch (LOWORD(lParam))
	{
		case WM_RBUTTONUP:
		{
			CMenu menu;
			if (!menu.LoadMenu(IDR_POPUP_TRAY_ICON)) 
			{
				return 0;
			}
			
			CMenu* pSubMenu = menu.GetSubMenu(0);
			if (pSubMenu == NULL) {
				return 0;
			}
			
			// Make chosen menu item the default (bold font)
			::SetMenuDefaultItem(pSubMenu->m_hMenu,
				ID_TRAY_RESTORE, FALSE);
			
			// Display the menu at the current mouse location. There's a "bug"
			// (Microsoft calls it a feature) in Windows 95 that requires calling
			// SetForegroundWindow. To find out more, search for Q135788 in MSDN.
			//
			CPoint pos;
			GetCursorPos(&pos);
			::SetForegroundWindow(m_hWnd); 
			
			::TrackPopupMenu(pSubMenu->m_hMenu, 0, pos.x, pos.y,
				0, m_hWnd, NULL);
			
			::PostMessage(m_hWnd, WM_NULL, 0, 0);
			
			menu.DestroyMenu();
			
		}
		break;
		
		case WM_LBUTTONDBLCLK:
		{
			// double click received, the default action is to execute default menu 
			//item
			SetForegroundWindow();

			OnTrayRestore();
		}
	}
	
    return 1;
}


void CPlutoVIPMain::InitializeTrayIcon()
{
	/*
	if (!m_TrayIcon.Create(
	_T("PlutoVIP Service!"), // Toolktip text
	this,							// Parent window
	IDR_MAINFRAME,					// Icon resource ID
	IDR_POPUP_TRAY_ICON,            // Resource ID of popup menu
	ID_TRAY_RESTORE,                // Default menu item for popup menu
	false))							
	{
	TRACE0("Failed to create tray icon\n");
	return -1;
	}
	m_TrayIcon.SetNotificationWnd(this);
	*/
	
	m_TrayIcon.cbSize=sizeof(NOTIFYICONDATA);
	m_TrayIcon.hIcon=m_hIcon;
	m_TrayIcon.hWnd=m_hWnd;
	_tcscpy(m_TrayIcon.szTip, _T("PlutoVIP Service"));
	m_TrayIcon.uCallbackMessage=WM_TRAYICON;
	m_TrayIcon.uFlags=NIF_ICON | NIF_MESSAGE | NIF_TIP;
	Shell_NotifyIcon(NIM_ADD, &m_TrayIcon);
}

void CPlutoVIPMain::DetectPhones()
{	
	m_pPhoneDetection_VIP = new PhoneDetection_VIP();
	m_pPhoneDetection_VIP->SetAppBars(g_TaskBars.m_NewCustomer, 
		g_TaskBars.m_ActiveCustomer, 
		g_TaskBars.m_RecentCustomer);
	m_pPhoneDetection_VIP->StartScanning();  // Start scanning for phones*/
}

void CPlutoVIPMain::ProgressUpdate(LPCTSTR szIEMsg, 
								   LPCTSTR szCustomMsg, 
								   const int nPercentDone)
{
	//do nothing
}


UINT CheckVersion(LPVOID pParam)
{
	/*
	Check version of PlutoVIP program
	if a newer version is detected, download it
	first download PlutoVIP_Version.txt from 
	
	http://www.plutovip.com/PlutoVIP_Version.txt

	then if the newer version is available, 
	download it.

	format of text file is as below:
	Version
	Major
	Minor
	Build
	Setup program


	Here is an example: (1.1.2.1265)
	1
	1
	2
	1265
	http://www.plutovip.com/PlutoVIP_Setup.exe

	*/

	g_fAbortDownload = 0;

	CCallback callback;
	HRESULT   hr;
	CString   sURL, sFile;
	sURL=PLUTOVIP_VERSION_URL;

	//we do not have progress in this step
	callback.HaveProgress(FALSE);


	TCHAR TempPath[256];

	GetTempPath(sizeof(TempPath)/sizeof(TCHAR), TempPath);
	
	sFile.Format(_T("%s~setuproutine"), TempPath);
	
	callback.SetDialogType(DIALOG_TYPE_PLUTOVIPMAIN);
    callback.m_pDlg = (CPlutoVIPMain*) pParam;
	
    callback.m_bUseTimeout = TRUE;
		
    callback.m_timeToStop = CTime::GetCurrentTime() +
			CTimeSpan( 0, 0, 0, 120 /*120 sec */ );
	
    hr = URLDownloadToFile (NULL,      // ptr to ActiveX container
							sURL,      // URL to get
							sFile,     // file to store data in
							0,         // reserved
							&callback  // ptr to IBindStatusCallback
							);
	
    if ( SUCCEEDED(hr) )
	{
        //Check the version
		//read the file: sFile
		CheckAppVersion(sFile);
	}
    else
	{
        LPTSTR lpszErrorMessage;
        CString sMsg;
		
        if ( FormatMessage ( FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, hr, 
			MAKELANGID ( LANG_NEUTRAL, SUBLANG_DEFAULT ),
			(LPTSTR) &lpszErrorMessage, 0, NULL ))
		{
            sMsg.Format ( _T("Download failed.  Error = 0x%08lX\n\n%s"),
				(DWORD) hr, lpszErrorMessage );
			
            LocalFree ( lpszErrorMessage );

			AfxMessageBox ( sMsg );
		}
        else
		{
            //no file to download
			//do nothing
		}
	}	

	return 0;
}

void CheckAppVersion(CString szFileName)
{
	FILE* fp=NULL;
	fp=_tfopen(szFileName, _T("rt"));
	if (fp==NULL)
	{
		//can not open the file
		//we have some problem
		return;
	}

	//reading the file
	/*
	 format of text file is as below:
	 format of text file is as below:
	 Version
	 Major
	 Minor
	 Build
	 Setup program
	 
	   
	1
	1
	2
	1265
	http://www.plutovip.com/PlutoVIP_Setup.exe
	 */
	TCHAR szVersion[100], szMajor[100], szMinor[100], szBuild[100], szURL[500];
	_ftscanf(fp, _T("%s%s%s%s%s"),  szVersion,
								szMajor,
								szMinor,
								szBuild,
								szURL);

	fclose(fp);

	CRegistry registry;
	registry.OpenKey(HKEY_CURRENT_USER, REGISTRY_KEY_VIPSERVICE_FULL);
	CString VersionFull=registry.GetValueString(PLUTOVIP_VERSION_FULL);

	/*
	CString Version=registry.GetValueString(PLUTOVIP_VERSION_VERSION);
	CString Major=registry.GetValueString(PLUTOVIP_VERSION_MAJOR);
	CString Minor=registry.GetValueString(PLUTOVIP_VERSION_MINOR);
	CString Build=registry.GetValueString(PLUTOVIP_VERSION_BUILD);
	*/

	registry.CloseKey();

	CString dummyVersion;
	dummyVersion.Format(_T("%s.%s.%s.%s"), szVersion, szMajor, szMinor, szBuild);

	if (VersionFull.Compare(dummyVersion)==0)
	{
		//user has the same version
		//do nothing
		return;
	}
	else
	{
		//parse the version number
		/*
		int iVersion, iMajor, iMinor, iBuild;
		iVersion=atoi(Version.GetBuffer(0));
		iMajor=atoi(Major.GetBuffer(0));
		iMinor=atoi(Minor.GetBuffer(0));
		iBuild=atoi(Build.GetBuffer(0));
		*/

		//we must download latest setup program
		CUpdateDlg update;
		update.SetURL(szURL);
		update.Create(IDD_UPDATE);
	}
}


void CPlutoVIPMain::OnCheckUpdateVersion()
{
	//Check version for product update
	AfxBeginThread(CheckVersion, this);	
}

BOOL CPlutoVIPMain::CheckInternetConnection()
{
	//how we are connected to Internet
	CRegistry registry;
	registry.OpenKey(HKEY_CURRENT_USER, REGISTRY_KEY_VIPSERVICE_FULL);
	DWORD dwInternet=registry.GetValueDWORD(REGISTRY_PAGE_INTERNET_INTERNET_CONNECTION);

	switch (dwInternet)
	{
		case INTERNET_CONNECTION_DSL_LAN:
			//no problem, continue to check the new version
			return TRUE;
			break;

		case INTERNET_CONNECTION_DIALUP:
			//first check if any connection was established before
			if (IsRASConnectionEstablished())
				return TRUE;
			else
				return FALSE;		//there is no connection, we just return
			break;

		case INTERNET_CONNECTION_NOT_CONNECT:
			return FALSE;			//return without checking
			break;
	}

	registry.CloseKey();

	return FALSE;
}

BOOL IsRASConnectionEstablished()
{
	DWORD dwCb = sizeof(RASCONN);
    DWORD dwErr = ERROR_SUCCESS;
    DWORD dwRetries = 5;
    DWORD dwConnections = 0;
    RASCONN* lpRasConn = NULL;

    //
    // Loop through in case the information from RAS changes between calls.
    //
    while (dwRetries--)
    {
        //
        // If the memory is allocated, free it.
        //
        if (NULL != lpRasConn)
        {
            HeapFree(GetProcessHeap(), 0, lpRasConn);
            lpRasConn = NULL;
        }
        //
        // Allocate the size needed for the RAS structure.
        //
        lpRasConn = (RASCONN*) HeapAlloc(GetProcessHeap(), 0, dwCb);
        if (NULL == lpRasConn)
        {
            dwErr = ERROR_NOT_ENOUGH_MEMORY;
            return FALSE;
        }
        //
        // Set the structure size for version checking purposes.
        //
        lpRasConn->dwSize = sizeof(RASCONN);
        //
        // Call the RAS API then exit the loop if we are successful or an unknown
        // error occurs.
        //
        dwErr = RasEnumConnections(
                    lpRasConn,
                    &dwCb,
                    &dwConnections);
        if (ERROR_BUFFER_TOO_SMALL != dwErr)
        {
            break;
        }
    }
    //
    // In the success case, print the names of the connections.
    //
    if (ERROR_SUCCESS == dwErr)
    {
        DWORD i=0;

        if (dwConnections)
        {
            TRACE0(lpRasConn[i].szEntryName);
			
			//
			// Free the memory if necessary.
			//
			if (NULL != lpRasConn)
			{
				HeapFree(GetProcessHeap(), 0, lpRasConn);
				lpRasConn = NULL;
			}

			return TRUE;
        }

		//free the memory
		if (NULL != lpRasConn)
		{
			HeapFree(GetProcessHeap(), 0, lpRasConn);
			lpRasConn = NULL;
		}
		return FALSE;
    }
    else
    {
        TRACE0("RasEnumConnections failed");
		return FALSE;
    }
 }



void CPlutoVIPMain::MinimizeParent()
{
	OnMinimized();
}

void CPlutoVIPMain::RefreshAppBars()
{
	g_TaskBars.RefreshAppBars();
	/*
	m_NewCustomer->SetupButtons();
	m_ActiveCustomer->SetupButtons();
	m_RecentCustomer->SetupButtons();
	*/
}

