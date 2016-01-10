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
// UpdateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PlutoVIP.h"
#include "UpdateDlg.h"
#include "BindStatusCallback.h"
#include "PlutoRegistryKeyDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern LONG g_fAbortDownload;
/////////////////////////////////////////////////////////////////////////////
// CUpdateDlg dialog


CUpdateDlg::CUpdateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUpdateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUpdateDlg)
	//}}AFX_DATA_INIT
}


void CUpdateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUpdateDlg)
	DDX_Control(pDX, IDC_UPDATE_PROGRESS, m_Progress);
	DDX_Control(pDX, IDC_STATUS, m_stCustomMsg);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUpdateDlg, CDialog)
	//{{AFX_MSG_MAP(CUpdateDlg)
	ON_BN_CLICKED(IDCANCEL, OnStop)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUpdateDlg message handlers

void CUpdateDlg::OnStop() 
{
	InterlockedExchange (&g_fAbortDownload, 1);

	CDialog::OnCancel();
}

BOOL CUpdateDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	//download the file from internet
	StartDownloading();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CUpdateDlg::SetURL(CString szURL)
{
	m_URL=szURL;
}

void CUpdateDlg::ProgressUpdate(LPCTSTR szIEMsg, 
								LPCTSTR szCustomMsg, 
								const int nPercentDone)
{
	ASSERT ( AfxIsValidString ( szIEMsg ));
    ASSERT ( AfxIsValidString ( szCustomMsg ));
    ASSERT ( nPercentDone >= 0  &&  nPercentDone <= 100 );
	
    m_stCustomMsg.SetWindowText ( szCustomMsg );
    m_Progress.SetPos ( nPercentDone );
}

void CUpdateDlg::StartDownloading()
{
	CCallback callback;
	HRESULT   hr;
	CString   sFile;
	
	TCHAR temp[512];
	GetTempPath(sizeof(temp)/sizeof(TCHAR), temp);
	sFile.Format("%s%s", temp, PLUTOVIP_SETUP_PROGRAM);

	callback.SetDialogType(DIALOG_TYPE_UPDATEDLG);
    callback.m_pDlg = this;
	
    
    // If the user wants a timeout, calculate the time when the download
    // should abort.
    callback.m_bUseTimeout = TRUE;
		
    callback.m_timeToStop = CTime::GetCurrentTime() +
			CTimeSpan( 0, 0, 0, 120 /*120 sec*/ );
	
    hr = URLDownloadToFile (NULL,      // ptr to ActiveX container
							m_URL,      // URL to get
							sFile,     // file to store data in
							0,         // reserved
							&callback  // ptr to IBindStatusCallback
							);
	
    if ( SUCCEEDED(hr) )
	{
        //download successfully completed
		//run the setup application
		STARTUPINFO si;
		si.cb=sizeof(STARTUPINFO);
		si.cbReserved2=0;
		si.dwFlags=STARTF_FORCEONFEEDBACK | STARTF_USESHOWWINDOW | STARTF_USESHOWWINDOW;
		si.lpDesktop=NULL;
		si.lpReserved=NULL;
		si.lpReserved2=NULL;
		si.lpTitle=NULL;
		si.wShowWindow=SW_SHOWDEFAULT;
				

		PROCESS_INFORMATION pi;
		
		CreateProcess(  sFile,
						NULL,
						NULL,
						NULL,
						FALSE,
						CREATE_DEFAULT_ERROR_MODE,
						NULL,
						NULL,
						&si,
						&pi
						);


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
		}
        else
		{
            sMsg.Format ( _T("Download failed.  Error = 0x%08lX\n"),
				(DWORD) hr );
		}
		
        AfxMessageBox (sMsg, MB_OK | MB_ICONERROR);
	}
}
