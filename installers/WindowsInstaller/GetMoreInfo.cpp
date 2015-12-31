// GetMoreInfo.cpp : implementation file
//

#include "stdafx.h"
#include <afxinet.h>

#include "WindowsInstaller.h"
#include "GetMoreInfo.h"
#include ".\getmoreinfo.h"
#include "PackageListDialog.h"

#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/FileUtils.h"


#include "unzip_wrapper.h"
#define USE_CACHE_FILES
// CGetMoreInfo dialog

IMPLEMENT_DYNAMIC(CGetMoreInfo, CDialog)
CGetMoreInfo::CGetMoreInfo(CDialog *pPrevDialog, CWnd* pParent /*=NULL*/)
	: CDialog(CGetMoreInfo::IDD, pParent)
{
	m_pPrevDialog = pPrevDialog;
	m_bAboutToCancel = false;
}

CGetMoreInfo::~CGetMoreInfo()
{
}

BOOL CGetMoreInfo::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_PlutoDriveComboBox.Clear();
	for(char c = 'C'; c <= 'Z'; c++)
		m_PlutoDriveComboBox.AddString(CString(c) + ":");

	m_PlutoDriveComboBox.SetCurSel(0);

	m_RadioBoxOption1.SetCheck(BST_CHECKED);
	m_RadioBoxOption2.SetCheck(BST_UNCHECKED);
	m_RadioBoxOption3.SetCheck(BST_UNCHECKED);

	return TRUE;
}

void CGetMoreInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_INSTALL, m_InstallEdit);
	DDX_Control(pDX, IDC_PROGRESS_INSTALL, m_Progress);
	DDX_Control(pDX, IDC_EDIT1, m_ServerIPAddressEdit);
	DDX_Control(pDX, IDC_RADIO1, m_RadioBoxOption1);
	DDX_Control(pDX, IDC_RADIO3, m_RadioBoxOption2);
	DDX_Control(pDX, IDC_RADIO2, m_RadioBoxOption3);
	DDX_Control(pDX, IDC_MOREINFO_PREV, m_PrevButton);
	DDX_Control(pDX, IDC_MOREINFO_INSTALL, m_InstallButton);
	DDX_Control(pDX, IDC_MOREINFO_CANCEL, m_CancelFinishButton);
	DDX_Control(pDX, IDC_ACTIVE_CLIENTS_SIDE, m_PlutoDriveComboBox);
}


BEGIN_MESSAGE_MAP(CGetMoreInfo, CDialog)
	ON_BN_CLICKED(IDC_MOREINFO_PREV, OnBnClickedMoreinfoPrev)
	ON_BN_CLICKED(IDC_MOREINFO_CANCEL, OnBnClickedMoreinfoCancel)
	ON_BN_CLICKED(IDC_MOREINFO_INSTALL, OnBnClickedMoreinfoInstall)
	ON_BN_CLICKED(IDC_RADIO1, OnBnClickedOption1)
	ON_BN_CLICKED(IDC_RADIO3, OnBnClickedOption2)
	ON_BN_CLICKED(IDC_RADIO2, OnBnClickedOption3)
END_MESSAGE_MAP()


// CGetMoreInfo message handlers

void CGetMoreInfo::OnBnClickedMoreinfoPrev()
{
	ShowWindow(SW_HIDE);

	ASSERT(NULL != m_pPrevDialog);
	m_pPrevDialog->ShowWindow(SW_SHOW);

	RECT rect;
	GetWindowRect(&rect);
	m_pPrevDialog->ShowWindow(SW_SHOWNORMAL);
	m_pPrevDialog->SetWindowPos(&CWnd::wndNoTopMost, rect.left, rect.top, rect.right - rect.left, 
		rect.bottom - rect.top, SWP_SHOWWINDOW);
}

void CGetMoreInfo::OnBnClickedMoreinfoCancel()
{
	CString sCaption;
	m_CancelFinishButton.GetWindowText(sCaption);

	if(sCaption == "&Finish")
		CloseAll();
	else
		if(HandleCancel(this))
			CloseAll();
}

void CGetMoreInfo::CloseAll()
{
	m_bAboutToCancel = true;

	OnCancel();
	((CPackageListDialog *)m_pPrevDialog)->CloseAll();
}

void CGetMoreInfo::OnBnClickedMoreinfoInstall()
{
	if(FileUtils::FileExists(string(GetLogFilePath())))
		::DeleteFile(GetLogFilePath());

	//test the installation drive
	CString sDrive = GetInstallationDrive();
	CString sDestinationFile;

	FILE *f = fopen(sDrive + "/pluto.tst", "w");
	if(!f)
	{
		LogInstallAction("ERROR: Invalid drive: " + sDrive);
		return;
	}
	else
		fclose(f);

	//disabling controls
	m_PlutoDriveComboBox.EnableWindow(FALSE);
	m_ServerIPAddressEdit.EnableWindow(FALSE);
	m_RadioBoxOption1.EnableWindow(FALSE);
	m_RadioBoxOption2.EnableWindow(FALSE);
	m_RadioBoxOption3.EnableWindow(FALSE);
	m_PrevButton.EnableWindow(FALSE);
	m_InstallButton.EnableWindow(FALSE);
	UpdateWindow();

	//svn installation
	if(NeedToInstallSVN() && !IsSVNInstalled())
		if(!InstallSVN())
		{
			LogInstallAction("ERROR: Subversion is not installed!");
			return;
		}
		else
		{
			LogInstallAction("Need to restart the computer.");
			MessageBox("In order to use Subversion, will have to restart the computer.", "Restart needed");
			return;
		}


	m_InstallEdit.Clear();

	//saving mysql server ip to windows/system32/drivers/etc/hosts
	AddMySqlServerIPToHosts();
	CString sPlutoFolder = sDrive + "\\pluto\\bin";
	FileUtils::MakeDir(string(sPlutoFolder));
    LogInstallAction("Creating pluto folder '" + sPlutoFolder + "'...");

	//installing packages
	BeginWaitCursor();
	INT_PTR nCount = aryListBoxSel.GetCount();
	m_Progress.SetRange(0, short(nCount));
	for(INT_PTR i = 0; i < nCount; i++)
	{
		InstallPackage(aryListBoxSel.GetAt(i));

		if(m_bAboutToCancel)
			break;

		m_Progress.SetPos(int(i + 1));
	}

	//post install scripts
	LogInstallAction("Running postinstall scripts");
	for(int i = 0; i < m_vectPostInstall.size(); i++)
		LaunchProgram(m_vectPostInstall[i]);

    m_vectPostInstall.clear();

	EndWaitCursor();
	m_CancelFinishButton.SetWindowText("&Finish");
}

void CGetMoreInfo::OnBnClickedOption1()
{
	m_RadioBoxOption2.SetCheck(BST_UNCHECKED);
	m_RadioBoxOption3.SetCheck(BST_UNCHECKED);	

	m_ServerIPAddressEdit.SetReadOnly();
}

void CGetMoreInfo::OnBnClickedOption2()
{
	m_RadioBoxOption1.SetCheck(BST_UNCHECKED);
	m_RadioBoxOption3.SetCheck(BST_UNCHECKED);	

	m_ServerIPAddressEdit.SetReadOnly();
}

void CGetMoreInfo::OnBnClickedOption3()
{
	m_RadioBoxOption1.SetCheck(BST_UNCHECKED);
	m_RadioBoxOption2.SetCheck(BST_UNCHECKED);

	m_ServerIPAddressEdit.SetReadOnly(FALSE);
	m_ServerIPAddressEdit.SetFocus();
}

void CGetMoreInfo::LogInstallAction(CString str)
{
	CString sBuffer;
	m_InstallEdit.GetWindowText(sBuffer);

	if(sBuffer != "")
		m_InstallEdit.SetWindowText(sBuffer + "\r\n" + str);
	else
		m_InstallEdit.SetWindowText(str);

	m_InstallEdit.LineScroll(m_InstallEdit.GetLineCount());
	m_InstallEdit.UpdateWindow();

	string sTempStr(str + "\n");	
	FILE *f = fopen(GetLogFilePath(), "a+");
	if(f)
	{
		fwrite(sTempStr.c_str(), sTempStr.length(), 1, f);
		fclose(f);
	}
}

void CGetMoreInfo::InstallPackage(int iIndex)
{
	ProcessMessages();

	if(vectPackageInfos[iIndex].sPackageSourceDescription == "MySQL 4 server" &&
		BST_UNCHECKED == m_RadioBoxOption1.GetCheck()
	)
	{
		LogInstallAction("Skipping package: " + vectPackageInfos[iIndex].sPackageSourceDescription + "...");
		return;
	}

	LogInstallAction("");
	LogInstallAction("Installing package: " + vectPackageInfos[iIndex].sPackageSourceDescription + "...");

	if(vectPackageInfos[iIndex].sParams == csNotSpecified.c_str())
		vectPackageInfos[iIndex].sParams = "";

	const CString sServer = vectPackageInfos[iIndex].sURL;
	CString sFileName = 
		vectPackageInfos[iIndex].sPackageName + 
		(
			vectPackageInfos[iIndex].sVersion != csNotSpecified.c_str() ?
				("_" + vectPackageInfos[iIndex].sVersion)				:
				CString()
		) +
		vectPackageInfos[iIndex].sParams;
	CString sURL = vectPackageInfos[iIndex].sRepository;

	ParseAndExecuteCommands(sServer, sURL, vectPackageInfos[iIndex].sComments, sFileName,
		vectPackageInfos[iIndex].sSourceImplementationPath);

	//mysql server - hack: rename the folder to be 'mysql'
	if(vectPackageInfos[iIndex].sPackageSourceDescription == "MySQL 4 server")
	{
		CString sDrive = GetInstallationDrive();
		//TODO: we should use the unzip lib to read the parent folder
		CString sMySQLDirName = sDrive + "\\" + "mysql-4.0.23-win"; //hack!!!
		CString sMySQLDirNameDest = sDrive + "\\" + "mysql";

		if(FileUtils::DirExists(string(sMySQLDirNameDest)))
			::MoveFile(sMySQLDirNameDest, sMySQLDirNameDest + ".old");

		::MoveFile(sMySQLDirName, sMySQLDirNameDest);

		LaunchProgram(sMySQLDirNameDest + "\\bin\\winmysqladmin.exe", false);
	}
}

void CGetMoreInfo::AddMySqlServerIPToHosts()
{
	LogInstallAction("Adding mysql server ip to windows hosts list...");

	CString sIPAddress;

	if(BST_CHECKED == m_RadioBoxOption3.GetCheck())
		m_ServerIPAddressEdit.GetWindowText(sIPAddress);
	else
		sIPAddress = "127.0.0.1";

	char Buffer[MAX_PATH];
	::GetWindowsDirectory(Buffer, MAX_PATH);

	CString sHostsFilePath = CString(Buffer) + "\\system32\\drivers\\etc\\hosts";
	CString sMySQLLine = sIPAddress + "\tmysql\n";

	FILE *f = fopen(sHostsFilePath, "a+");
	if(NULL != f)
	{
		fwrite(sMySQLLine, sMySQLLine.GetLength(), 1, f);
		fclose(f);
	}
	else
	{
		::MessageBox(m_hWnd, 
			CString("Unable to save the ip for mysql server to hosts file\r\n") + 
				CString("Please add manually the line '") +
				sMySQLLine + 
				"' to 'sHostsFilePath' file",
			"Error", 
			MB_OK
		);
	}
}

void CGetMoreInfo::ParseAndExecuteCommands(CString sServer, CString sURL, CString sCommands, CString sFileName, CString sSourceImplementationPath)
{
	CString sDrive = GetInstallationDrive();
	CString sDestinationFile;

	bool bRealUrlInComment = false;

	if(sCommands == csNotSpecified.c_str()) //assume that the downloaded file should be copied to /pluto/bin folder
	{
		sDestinationFile = sDrive + "\\pluto\\bin";
	}
	else
	{
		char Buffer[MAX_PATH];
		::GetTempPath(MAX_PATH, Buffer);

        sDestinationFile = Buffer;
	}

	enum URLType { utHTTP, utFTP, utSVN };
	URLType urlType;

	if(sCommands != csNotSpecified.c_str()) //we have additional pre-download instructions
	{
		string sData(sCommands);
		vector<string> vectLines;
		StringUtils::Tokenize(sData, "`", vectLines);

		for(size_t i = 0; i < vectLines.size(); ++i)
		{
			string sLine(vectLines[i].c_str());
			vector<string> vectWords;
			StringUtils::Tokenize(sLine, " ", vectWords); 

			if(vectWords[0] == "REALURL")
			{
				sURL = vectWords[1].c_str();
				bRealUrlInComment = true;
			}

			vectWords.clear();
		}

		vectLines.clear();
	}

	CString sFullUrl = sServer + sURL;

	//check from url type (svn, http, ftp)
	if(sFullUrl.Find("http://svn.") == 0) 
		urlType = utSVN;
	else if(sFullUrl.Find("http://") == 0) 
		urlType = utHTTP;
	else if(sFullUrl.Find("ftp://") == 0) 
		urlType = utFTP;
	else 
	{
		LogInstallAction("WARNING: Unknown url type: " + sURL + ". Assuming that is an http url");
		urlType = utHTTP;
	}

	if(urlType != utSVN && !bRealUrlInComment)
		sURL = sURL + "/" + sFileName;	

	ProcessMessages();

#ifdef USE_CACHE_FILES
	if(!FileUtils::FileExists(string(sDestinationFile + sFileName)))
#endif
	{
		switch(urlType) 
		{
			case utHTTP:
				if(!DownloadFileFromHttp(sServer, sURL, sDestinationFile + sFileName))
					return;
				break;
			case utFTP:
				if(!DownloadFileFromFtp(sServer, sURL, sDestinationFile + sFileName))
					return;
				break;
			case utSVN:
				if(!DownloadFileFromSvn(sServer, sURL, sSourceImplementationPath, sDestinationFile + sFileName))
					return;
				break;
		}
	}
#ifdef USE_CACHE_FILES
	else
		LogInstallAction("Found file in cache: " + sDestinationFile + sFileName + ". ");
#endif

	ProcessMessages();

	if(sCommands != csNotSpecified.c_str()) //we have additional instructions
	{
		string sData(sCommands);
		vector<string> vectLines;
		StringUtils::Tokenize(sData, "`", vectLines);

		for(size_t i = 0; i < vectLines.size(); ++i)
		{
			string sLine(vectLines[i].c_str());
			vector<string> vectWords;
			StringUtils::Tokenize(sLine, " ", vectWords); 

			if(vectWords[0] == "UNZIP")
				UnzipFileToFolder(sDestinationFile + sFileName, sDrive + vectWords[1].c_str());
			else if(vectWords[0] == "RUN")
				LaunchProgram(sDestinationFile + sFileName);
			else if(vectWords[0] == "LAUNCH")
			{
				LogInstallAction("Scheduling postinstall script" + sDrive + vectWords[1].c_str());
				m_vectPostInstall.push_back(sDrive + vectWords[1].c_str());
			}

			vectWords.clear();
        }

		vectLines.clear();
	}
}

bool CGetMoreInfo::DownloadFileFromHttp(CString sServer, CString sURL, CString sDestinationFile)
{
	CString sFullURL = sServer + sURL;
    LogInstallAction("Downloading file: '" + sFullURL + "'...");

	CInternetSession inetSession;
	CHttpFile *psFile = (CHttpFile *)inetSession.OpenURL(sFullURL, 1, INTERNET_FLAG_TRANSFER_BINARY);

	const unsigned long ciSize = 1024;
	BYTE Buffer[ciSize];
	bool bFinished = false;
	CString sPartialHeaderFile;

	FILE *f = fopen(sDestinationFile, "wb");
	if(NULL != f)
	{
		UINT iSize;
		while((iSize = psFile->Read(Buffer, ciSize)) > 0)
		{
			if(sPartialHeaderFile.IsEmpty())
				sPartialHeaderFile = (char *)Buffer;

			fwrite(Buffer, iSize, 1, f);
			::ZeroMemory(Buffer, sizeof(Buffer));

			ProcessMessages(20);

			if(m_bAboutToCancel)
			{
				::DeleteFile(sDestinationFile);
				return false;
			}
		}

		fclose(f);
	}
	else
	{
		LogInstallAction("ERROR: Unable to save " + sFullURL + " file to " + sDestinationFile);
		psFile->Close();
		delete psFile;
		inetSession.Close();
		return false;
	}

	psFile->Close();
	delete psFile;
	inetSession.Close();

	if(sPartialHeaderFile.Find("Not Found") > 0)
	{
		LogInstallAction("ERROR: Unable to download " + sFullURL + " file!. The file wasn't found on the server.");
		::DeleteFile(sDestinationFile);
		return false;
	}

	LogInstallAction("Downloaded " + sFullURL + " file to " + sDestinationFile);
	return true;
}

bool CGetMoreInfo::DownloadFileFromFtp(CString sServer, CString sURL, CString sDestinationFile)
{
	CString sFullURL = sServer + sURL;
	LogInstallAction("Downloading file: '" + sFullURL + "'...");

	//assumes server and file names have been initialized
	CInternetSession session("My FTP Session");
	CFtpConnection* pConn = NULL;

	pConn = session.GetFtpConnection(sServer);
	//get the file
	bool bResult = 0 == pConn->GetFile(sServer + sURL, sDestinationFile);

	delete pConn;
	session.Close();

	LogInstallAction("Downloaded " + sFullURL + " file to " + sDestinationFile);
	return bResult;
}

bool CGetMoreInfo::DownloadFileFromSvn(CString sServer, CString sRepository, CString sSourceImplementationPath, CString sDestinationFile)
{
	CString sDrive = GetInstallationDrive();
	CString sFullURL = sServer + "/" + sRepository + "/" + sSourceImplementationPath;
	LogInstallAction("Checking out folder from subversion: '" + sFullURL + "'...");

	PROCESS_INFORMATION pi;
	::ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

	STARTUPINFO si;
	::ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.lpReserved = 0;

	string sTempFullPath = sFullURL;
	//StringUtils::Replace(sTempFullPath, "http://svn.", "svn://");
	sFullURL = sTempFullPath.c_str();

	CString sSvnCommandLine = "svn co " + sFullURL + " " + sDrive + "/pluto/" + sSourceImplementationPath;
	LogInstallAction("Running subversion : '" + sSvnCommandLine + "'...");

	if(!::CreateProcess(NULL, const_cast<char *>(string(sSvnCommandLine).c_str()), NULL, NULL, NULL, 0, NULL, NULL, &si, &pi)) //all ok
	{
		return FALSE;
	}

	UpdateWindow();

	::WaitForSingleObject(pi.hProcess, 10 * 1000 * 1000); //wait 10 minutes for user to install

	if(FileUtils::DirExists(string(sDrive + "/pluto/" + sSourceImplementationPath)))
		LogInstallAction("Checked out folder " + sFullURL + " file " + sDrive + "/pluto/" + sSourceImplementationPath);
	else
		LogInstallAction("ERROR: Subversion failed to checkout folder " + sDrive + "/pluto/" + sSourceImplementationPath);

	UpdateWindow();

	return TRUE;
}

bool CGetMoreInfo::UnzipFileToFolder(CString sSourceFile, CString sDestinationFolder)
{
	LogInstallAction("Unzipping file: '" + sSourceFile + "' to folder '" + sDestinationFolder + "'");

	if(UnzipFile(string(sSourceFile).c_str(), string(sDestinationFolder).c_str()))
	{
		LogInstallAction("ERROR: Unable to Unzip file: '" + sSourceFile + "' to folder '" + sDestinationFolder + "'");
		return false;
	}

	return true;
}

bool CGetMoreInfo::LaunchProgram(CString sFilePath, bool bWaitProcessToTerminate/*= true*/)
{
	PROCESS_INFORMATION pi;
	::ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

	STARTUPINFO si;
	::ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.lpReserved = 0;

	if(FileUtils::FindExtension(string(sFilePath)) == "msi")
		sFilePath = "msiexec /i " + sFilePath;

	LogInstallAction("Running process: '" + sFilePath + "'");
	if(!::CreateProcess(NULL, const_cast<char *>(string(sFilePath).c_str()), NULL, NULL, NULL, 0, NULL, NULL, &si, &pi)) //all ok
	{
		LogInstallAction("ERROR: Failed to run '" + sFilePath + "'");
		return FALSE;
	}
	
	if(bWaitProcessToTerminate)
		::WaitForSingleObject(pi.hProcess, 10 * 60 * 1000); //wait 10 minutes for user to install

	return TRUE;
}

bool CGetMoreInfo::IsSVNInstalled()
{
	LogInstallAction("Checking to see if svn is installed...");

	PROCESS_INFORMATION pi;
	::ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

	STARTUPINFO si;
	::ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.lpReserved = 0;

	if(!::CreateProcess(NULL, "svn.exe", NULL, NULL, NULL, 0, NULL, NULL, &si, &pi)) //all ok
	{
		LogInstallAction("SVN is not installed...");
		return FALSE;
	}

	::TerminateProcess(pi.hThread, 0);
	LogInstallAction("SVN is installed...");
	return TRUE;
}

CString CGetMoreInfo::GetInstallationDrive()
{
	CString sDrive;
	m_PlutoDriveComboBox.GetWindowText(sDrive);

	return sDrive;
}

CString CGetMoreInfo::GetLogFilePath()
{
	char Buffer[MAX_PATH];
	::GetTempPath(MAX_PATH, Buffer);
	CString sLogFile = CString(Buffer) + "\\PlutoInstaller.log";
	return sLogFile;
}

bool CGetMoreInfo::InstallSVN()
{
	char Buffer[MAX_PATH];
	::GetTempPath(MAX_PATH, Buffer);

	CString sDestinationFile = CString(Buffer) + "\\" + "svn-1.0.9-setup.exe";

	//HARDCODE WARNING!!
	DownloadFileFromHttp(
		"http://summersoft.fay.ar.us/", 
		"pub/subversion/1.0.9/windows/svn-1.0.9-setup.exe", 
		sDestinationFile
	);
	
	return LaunchProgram(sDestinationFile);
}

void CGetMoreInfo::ProcessMessages(DWORD dwMs/*=100*/)
{
	// Main message loop:
	DWORD dwLast = ::GetTickCount();

	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		DWORD dwNow = ::GetTickCount();
		if(dwNow - dwLast > dwMs)
			break;
	}
}

bool CGetMoreInfo::NeedToInstallSVN()
{
	INT_PTR nCount = aryListBoxSel.GetCount();
	for(INT_PTR i = 0; i < nCount; i++)
	{
		int iIndex = aryListBoxSel.GetAt(i);
		if(vectPackageInfos[iIndex].sURL.Find("svn.") != -1)
		{
			LogInstallAction("Need to install SVN!");
			return true;
		}
	}

	LogInstallAction("Don't need to install SVN!");
	return false;
}