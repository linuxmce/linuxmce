// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "ImportContacts.h"

#include "MainFrm.h"
#include ".\mainfrm.h"

#include "Log.h"
#include "DialogConnectSettings.h"
#include <string>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_IMPORT_CONNECTSETTINGS, OnImportConnectsettings)
	ON_COMMAND(ID_IMPORT_OUTLOOK, OnImportOutlook)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// create a view to occupy the client area of the frame
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	// TODO: Delete these three lines if you don't want the toolbar to be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// forward focus to the view window
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CMainFrame::OnImportConnectsettings()
{
	CDialogConnectSettings dlg;

	string server,dataBaseName;
	string user,password;

	dlg.m_ServerName = m_PlutoData.getServerName().c_str();
	dlg.m_DatabaseName = m_PlutoData.getDatabaseName().c_str();
	dlg.m_User = m_PlutoData.getUser().c_str();
	dlg.m_Password = m_PlutoData.getPassword().c_str();

	dlg.m_ConnectString = m_PlutoData.getConnectionString().c_str();

	if( dlg.DoModal() == IDOK )
	{
		m_PlutoData.setConnectionString( (LPCSTR) dlg.m_ServerName, 
			(LPCSTR) dlg.m_DatabaseName,(LPCSTR) dlg.m_User, 
			(LPCSTR) dlg.m_Password );
	}
}

void CMainFrame::OnImportOutlook()
{
	ContactsList list;
	vector<Contact *>::iterator it;
	Contact  *pContact = NULL;
	
	int nContactNo = 0;
	bool bRes;

	Log::m_pLog->writeLine( "Start reading from Outlook" );
	nContactNo = m_Outlook.readContacts(list);
	Log::m_pLog->writeLine( "Finish read from Outlook" );

	if( nContactNo )
	{
		m_PlutoData.connect();
		nContactNo = m_PlutoData.writeContacts( list );
	}

	Log::m_pLog->showLine( 
		MAKE_STRING( string("Import ") << nContactNo << " contacts\n" << \
		"Update" << m_PlutoData.getUpdateRowsNo() << " contacts" ), 
		"Message" );
}
