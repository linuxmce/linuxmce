// OrbiterCEInstallerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "OrbiterCEInstaller.h"
#include "OrbiterCEInstallerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "afxinet.h"

/////////////////////////////////////////////////////////////////////////////
// COrbiterCEInstallerDlg dialog

COrbiterCEInstallerDlg::COrbiterCEInstallerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COrbiterCEInstallerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COrbiterCEInstallerDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void COrbiterCEInstallerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COrbiterCEInstallerDlg)
	DDX_Control(pDX, IDC_STATIC2, m_Static);
	DDX_Control(pDX, IDC_EDIT1, m_Path);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(COrbiterCEInstallerDlg, CDialog)
	//{{AFX_MSG_MAP(COrbiterCEInstallerDlg)
	ON_BN_CLICKED(IDC_BUTTON1, OnInstall)
	ON_BN_CLICKED(IDC_BUTTON2, OnCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COrbiterCEInstallerDlg message handlers

BOOL COrbiterCEInstallerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	CenterWindow(GetDesktopWindow());	// center to the hpc screen

	CMenu *pMenu = GetMenu();

	if(pMenu)
		pMenu->DestroyMenu();

	return TRUE;  // return TRUE  unless you set the focus to a control
}


bool COrbiterCEInstallerDlg::DownloadFile(CString sFile, CString sDestinationFile)
{
	const CString sFullPath = "http://10.0.0.175/lmce-admin/fdownload.php?filepath=" + sFile;

	//MessageBox(sFullPath);

	CInternetSession inetSession;
	CHttpFile *psFile = (CHttpFile *)inetSession.OpenURL(sFullPath, 1, INTERNET_FLAG_TRANSFER_BINARY);

	const unsigned long ciSize = 1024;
	BYTE Buffer[ciSize];
	bool bFinished = false;
	CString sPartialHeaderFile;
	
	char pDestinationFile[MAX_PATH];
	wcstombs(pDestinationFile, sDestinationFile.GetBuffer(MAX_PATH), MAX_PATH);

	m_Static.SetWindowText(CString("Copying file: ") + sDestinationFile);
	m_Static.UpdateWindow();

	FILE *f = fopen(pDestinationFile, "wb");
	if(NULL != f)
	{
		UINT iSize;
		while((iSize = psFile->Read(Buffer, ciSize)) > 0)
		{
			if(sPartialHeaderFile.IsEmpty())
				sPartialHeaderFile = (char *)Buffer;

			fwrite(Buffer, iSize, 1, f);
			::ZeroMemory(Buffer, sizeof(Buffer));
		}

		fclose(f);
	}
	else
	{
		MessageBox("ERROR: Unable to save " + sFullPath + " file to " + sDestinationFile);

		m_Static.SetWindowText(TEXT("Installation failed."));
		m_Static.UpdateWindow();

		psFile->Close();
		delete psFile;
		inetSession.Close();
		return false;
	}

	psFile->Close();
	delete psFile;
	inetSession.Close();

	if(sPartialHeaderFile.Find(TEXT("Not Found")) > 0)
	{
		MessageBox("ERROR: Unable to download " + sFullPath + " file!. The file wasn't found on the server.");

		m_Static.SetWindowText(TEXT("Installation failed."));
		m_Static.UpdateWindow();
		//::DeleteFile(sDestinationFile);
		return false;
	}

	return true;
}

void COrbiterCEInstallerDlg::OnInstall() 
{
	unsigned short sBuf[256];
	m_Path.GetWindowText(sBuf, 256);

	bool bSuccess = true;

	m_Static.SetWindowText(TEXT("Installation in progress, please wait..."));
	m_Static.UpdateWindow();

	if(bSuccess)
		bSuccess = bSuccess & DownloadFile("orbiter/binaries/AYGSHELL.DLL", CString(sBuf) + "/AYGSHELL.DLL");

	if(bSuccess)
		bSuccess = bSuccess & DownloadFile("orbiter/binaries/SDL.dll", CString(sBuf) + "/SDL.DLL");

	if(bSuccess)
		bSuccess = bSuccess & DownloadFile("orbiter/binaries/PthreadsCE.dll", CString(sBuf) + "/PthreadsCE.dll");

	if(bSuccess)
		bSuccess = bSuccess & DownloadFile("orbiter/binaries/Orbiter.MD5", CString(sBuf) + "/Orbiter.MD5");

	if(bSuccess)
		bSuccess = bSuccess & DownloadFile("bin/OrbiterCE.exe", CString(sBuf) + "/OrbiterCE.exe");

	if(bSuccess)
	{
		m_Static.SetWindowText(TEXT("Installation completed!"));
		m_Static.UpdateWindow();
	}
}

void COrbiterCEInstallerDlg::OnCancel() 
{
	exit(0);
}
