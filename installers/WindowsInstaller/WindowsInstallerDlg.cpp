// WindowsInstallerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WindowsInstaller.h"
#include "WindowsInstallerDlg.h"
#include ".\windowsinstallerdlg.h"
#include "PackageListDialog.h"

#include "afxinet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

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
	EnableActiveAccessibility();
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CWindowsInstallerDlg dialog



CWindowsInstallerDlg::CWindowsInstallerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWindowsInstallerDlg::IDD, pParent)
{
	EnableActiveAccessibility();
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pNextDialog = NULL;
}

CWindowsInstallerDlg::~CWindowsInstallerDlg()
{
	if(NULL != m_pNextDialog)
		delete m_pNextDialog;
}

void CWindowsInstallerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_DEVICEID, m_DeviceID);
	DDX_Control(pDX, IDC_EDIT_ACTIVATIONCODE, m_ActivationCode);
	DDX_Control(pDX, IDC_STATUS_FIRST, m_Status);
}

BEGIN_MESSAGE_MAP(CWindowsInstallerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(ID_NEXT, OnBnClickedNext)
	ON_BN_CLICKED(IDC_EXIT, OnBnClickedExit)
END_MESSAGE_MAP()


// CWindowsInstallerDlg message handlers

BOOL CWindowsInstallerDlg::OnInitDialog()
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

	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CWindowsInstallerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CWindowsInstallerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CWindowsInstallerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CWindowsInstallerDlg::OnBnClickedCancel()
{
	if(HandleCancel(this))
		OnCancel();
}

void CWindowsInstallerDlg::OnBnClickedNext()
{
	//todo: check for values
	unsigned long iDeviceID = 0;
	unsigned long iActivationID = 0;

	CString sDeviceID;
	CString sActivationCode;

	m_DeviceID.GetWindowText(sDeviceID);
	m_ActivationCode.GetWindowText(sActivationCode);

	try
	{
		iDeviceID = atoi(sDeviceID);
		iActivationID = atoi(sActivationCode);
	}
	catch(...)
	{
	}

	if(!iDeviceID)
	{
		::MessageBox(m_hWnd, "Invalid device id", "Error", MB_OK | MB_ICONERROR);
		return;
	}

	if(!iActivationID)
	{
		::MessageBox(m_hWnd, "Invalid activation code", "Error", MB_OK | MB_ICONERROR);
		return;
	}

	const CString sPlutoHomeURL = "http://activate.plutohome.com/activation_win.php?code=" + sDeviceID + "-" + sActivationCode;
	m_Status.SetWindowText("Connecting to plutohome site...");

	CInternetSession inetSession;
	CHttpFile *psFile = (CHttpFile *)inetSession.OpenURL(sPlutoHomeURL);

	m_Status.SetWindowText("Verifying activate code...");

	char Buffer[16];
	psFile->Read(Buffer, sizeof(Buffer));
	psFile->Close();
	delete psFile;
	inetSession.Close();

	m_Status.SetWindowText("Done.");

	if(Buffer[0] == 'O' && Buffer[1] == 'K')
	{
		RECT rect;
		GetWindowRect(&rect);
		SetWindowPos(&CWnd::wndNoTopMost, 0, 0, 0, 0, SWP_SHOWWINDOW);

		if(NULL == m_pNextDialog)
		{
			m_pNextDialog = new CPackageListDialog(this, sPlutoHomeURL);
			m_pNextDialog->Create(IDD_DIALOG_PACKAGESLIST, this);
		}

		m_pNextDialog->ShowWindow(SW_SHOWNORMAL);
		m_pNextDialog->SetWindowPos(&CWnd::wndNoTopMost, rect.left, rect.top, rect.right - rect.left, 
			rect.bottom - rect.top, SWP_SHOWWINDOW);
	}
	else
		::MessageBox(m_hWnd, "Invalid activation code", "Error", MB_OK | MB_ICONERROR);
}

void CWindowsInstallerDlg::OnBnClickedExit()
{
	if(HandleCancel(this))
		OnCancel();
}

void CWindowsInstallerDlg::CloseAll()
{
	OnCancel();
}