// PackageListDialog.cpp : implementation file
//

#include "stdafx.h"
#include "WindowsInstaller.h"
#include "WindowsInstallerDlg.h"
#include "PackageListDialog.h"
#include ".\packagelistdialog.h"
#include "PlutoUtils/StringUtils.h"
#include "GetMoreInfo.h"

#include "afxinet.h"

// CPackageListDialog dialog

IMPLEMENT_DYNAMIC(CPackageListDialog, CDialog)
CPackageListDialog::CPackageListDialog(CDialog *pPrevDialog, CString sPlutoHomeURL, CWnd* pParent /*=NULL*/)
	: CDialog(CPackageListDialog::IDD, pParent)
{
	m_pNextDialog = NULL;
	m_pPrevDialog = pPrevDialog;
	m_sPlutoHomeURL = sPlutoHomeURL;
}

CPackageListDialog::~CPackageListDialog()
{
	if(NULL != m_pNextDialog)
		delete m_pNextDialog;
}

BOOL CPackageListDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	return TRUE;
}

void CPackageListDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATUS_PACK, m_Status);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Control(pDX, IDC_HEADER_PACK2, m_Header);
}


BEGIN_MESSAGE_MAP(CPackageListDialog, CDialog)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(ID_PREV_PACK, OnBnClickedPrevPack)
	ON_BN_CLICKED(ID_NEXT_PACK, OnBnClickedNextPack)
	ON_BN_CLICKED(IDC_EXIT_PACK, OnBnClickedExitPack)
	ON_BN_CLICKED(ID_PACKAGES_PACK, OnBnClickedPackagesPack)
	ON_LBN_SELCHANGE(IDC_LIST1, OnLbnSelchangeList)
END_MESSAGE_MAP()


// CPackageListDialog message handlers

void CPackageListDialog::OnBnClickedCancel()
{
	if(HandleCancel(this))
		CloseAll();
}

void CPackageListDialog::OnBnClickedPrevPack()
{
	ShowWindow(SW_HIDE);

	ASSERT(NULL != m_pPrevDialog);

	RECT rect;
	GetWindowRect(&rect);

	m_pPrevDialog->SetWindowPos(&CWnd::wndNoTopMost, rect.left, rect.top, rect.right - rect.left, 
		rect.bottom - rect.top, SWP_SHOWWINDOW);
	m_pPrevDialog->ShowWindow(SW_SHOW);
}

void CPackageListDialog::OnBnClickedNextPack()
{
	//TODO: no package selected?

	aryListBoxSel.RemoveAll();

	// Get the indexes of all the selected items.
	int nCount = m_list.GetSelCount();

	aryListBoxSel.SetSize(nCount);
	m_list.GetSelItems(nCount, aryListBoxSel.GetData());

	//next dialog
	ShowWindow(SW_HIDE);

	if(NULL == m_pNextDialog)
	{
		m_pNextDialog = new CGetMoreInfo(this);
		m_pNextDialog->Create(IDD_DIALOG_MOREINFO, this);
	}

	m_pNextDialog->ShowWindow(SW_SHOW);

	RECT rect;
	GetWindowRect(&rect);
	m_pNextDialog->ShowWindow(SW_SHOWNORMAL);
	m_pNextDialog->SetWindowPos(&CWnd::wndNoTopMost, rect.left, rect.top, rect.right - rect.left, 
		rect.bottom - rect.top, SWP_SHOWWINDOW);
}

void CPackageListDialog::OnBnClickedExitPack()
{
	if(HandleCancel(this))
		CloseAll();
}

void CPackageListDialog::OnBnClickedPackagesPack()
{
	m_Status.SetWindowText("Connecting to plutohome site...");

	CInternetSession inetSession;
	CHttpFile *psFile = (CHttpFile *)inetSession.OpenURL(m_sPlutoHomeURL);

	m_Status.SetWindowText("Downloading the list with the software to be installed...");

	const unsigned long ciSize = 1024;
	unsigned long iRealSize = 0;
	unsigned long iFileSize = 0;
	bool bFinished = false;

	char Buffer[ciSize];
	::ZeroMemory(Buffer, sizeof(Buffer));

	char TempFolderPath[MAX_PATH];
	::GetTempPath(MAX_PATH, TempFolderPath);
	CString sTempFilePath = CString(TempFolderPath) + "PlutoPackageList.txt";


	FILE *fw = fopen(sTempFilePath, "w");
	if(!fw)
	{
		::MessageBox(m_hWnd, "Unable to create temporary file " + sTempFilePath, "Error", MB_OK);
		return;
	}

	while((iRealSize = psFile->Read(Buffer, ciSize)) > 0)
	{
		iFileSize += iRealSize;
		fwrite(Buffer, iRealSize, 1, fw);
		::ZeroMemory(Buffer, sizeof(Buffer));
	}
	fclose(fw);

	psFile->Close();
	delete psFile;
	inetSession.Close();

	FILE *fr = fopen(sTempFilePath, "r");
	if(!fr)
	{
		::MessageBox(m_hWnd, "Unable to read temporary file " + sTempFilePath, "Error", MB_OK);
		return;
	}

	char *pRawData = new char[iFileSize];
	fread(pRawData, iFileSize, 1, fr);
	fclose(fr);

	ParseRawData(CString(pRawData));
	delete pRawData;

	PopulateList();
}

void CPackageListDialog::CloseAll()
{
	OnCancel();
	((CWindowsInstallerDlg *)m_pPrevDialog)->CloseAll();
}

void CPackageListDialog::ParseRawData(const CString &sHttpRawData)
{
	m_Status.SetWindowText("Parsing raw data file...");

	string sData(sHttpRawData);
	vector<string> vectLines;
	StringUtils::Tokenize(sData, "\n", vectLines);

	for(size_t i = 0; i < vectLines.size(); ++i)
		ParseRawLine(vectLines[i].c_str());

	vectLines.clear();
}

void CPackageListDialog::ParseRawLine(const CString &sHttpRawLine)
{
	if(sHttpRawLine.Find('|') >= 0)
	{
		//this should be a valid pipe delimited sequence with usefull infos about a package

		string sData(sHttpRawLine);
		if(sData[0] == '|')
			sData = csNotSpecified + sData;

		sData = StringUtils::Replace(sData, "||", "|" + csNotSpecified + "|");
		sData = StringUtils::Replace(sData, "||", "|" + csNotSpecified + "|"); //StringUtils::Replace bug?

		vector<string> vectParams;
		StringUtils::Tokenize(sData, "|", vectParams);

		int iIndex = 0;
		ASSERT(vectParams.size() == 16);
		PackageInfos packageInfos;
		packageInfos.sPackageID = vectParams[iIndex++].c_str();
		packageInfos.sPackageName = vectParams[iIndex++].c_str();
		packageInfos.sURL = vectParams[iIndex++].c_str();
		packageInfos.sComments = vectParams[iIndex++].c_str();
		packageInfos.sRepository = vectParams[iIndex++].c_str();
		packageInfos.sRepositoryType = vectParams[iIndex++].c_str();
		packageInfos.sVersion = vectParams[iIndex++].c_str();
		packageInfos.sBinaryExecutiblesPathPath = vectParams[iIndex++].c_str();
		packageInfos.sSourceIncludesPath = vectParams[iIndex++].c_str();
		packageInfos.sSourceImplementationPath = vectParams[iIndex++].c_str();
		packageInfos.sBinaryLibraryPath = vectParams[iIndex++].c_str();
		packageInfos.sConfiguration = vectParams[iIndex++].c_str();
		packageInfos.sRepositorySourceUsername = vectParams[iIndex++].c_str();
		packageInfos.sRepositorySourcePassword = vectParams[iIndex++].c_str();
		packageInfos.sParams = vectParams[iIndex++].c_str();
		packageInfos.sPackageSourceDescription = vectParams[iIndex++].c_str();
		vectPackageInfos.push_back(packageInfos);

		vectParams.clear();
	}
}

void CPackageListDialog::PopulateList()
{
	m_list.SetColumnWidth(600);
	for(size_t i = 0; i < vectPackageInfos.size(); ++i)
	{
		CString str = CString("  ") + StringUtils::itos(int(i + 1)).c_str() + ". " +  
			vectPackageInfos[i].sPackageName + " ( " +
			vectPackageInfos[i].sPackageSourceDescription + " )";
		m_list.AddString(str);
		m_list.SetItemHeight(int(i), 15);
		m_list.SetSel(int(i));
	}

	CString sHeader = CString("Found the following ") + StringUtils::itos(int(vectPackageInfos.size())).c_str() + " packages";
	m_Header.SetWindowText(sHeader);

	CString sStatus = CString("Package(s) selected:") + StringUtils::itos(m_list.GetSelCount()).c_str();
	m_Status.SetWindowText(sStatus);
}

void CPackageListDialog::OnLbnSelchangeList()
{
	CString sStatus = CString("Package(s) selected:") + StringUtils::itos(m_list.GetSelCount()).c_str();
	m_Status.SetWindowText(sStatus);
}

