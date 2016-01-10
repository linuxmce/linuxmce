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
/**
 *
 * @file  HighlightedCustomer.cpp												
 * @brief source file for the  HighlightedCustomer class											
 */
 
 /**
  *
  * Copyright: copyright information goes here
  *
  */


#include "stdafx.h"
#include "PlutoVIP.h"

#include "PlutoUtils/CommonIncludes.h"	
#include "PlutoUtils/MyStl.h"
#include "VIPShared/VIPIncludes.h"
#include "Customer.h"
#include "ADO2.h"
#include "HighlightedCustomer.h"
#include "Registry.h"
#include "AddVisit.h"
#include "AddPurchase.h"
#include "RedeemVisit.h"
#include "RedeemPurchase.h"
#include "DatabaseDefs.h"

#include "ControlsDlg.h"
#include "VIPShared/BD_CP_ShowImage.h"
#include "BD/BDCommandProcessor.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CHighlightedCustomer::CHighlightedCustomer(CWnd* pParent /** @test =NULL*/)
	: CDialog(CHighlightedCustomer::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHighlightedCustomer)
	m_Address = _T("");
	m_Comment = _T("");
	m_FullName = _T("");
	m_City = _T("");
	m_State = _T("");
	m_Zip = _T("");
	m_StoreCustomerID = 0;
	m_Bdate_Gender = _T("");
	m_Extra = _T("");
	m_Phones = _T("");
	//}}AFX_DATA_INIT
	m_pCustomer=NULL;
	m_Timer=0;

	ZeroMemory(&PositionInfo, sizeof(Position));
	CommentsIsChanged=FALSE;
}


void CHighlightedCustomer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHighlightedCustomer)
	DDX_Control(pDX, IDC_CUSTOMER_EMAIL, m_email);
	DDX_Control(pDX, IDC_PURCHASE, m_Purchase);
	DDX_Control(pDX, IDC_VISIT, m_Visit);
	DDX_Control(pDX, IDC_PICTURE, m_Picture);
	DDX_Control(pDX, IDC_NICKNAME, m_NickName);
	DDX_Text(pDX, IDC_ADDRESS, m_Address);
	DDX_Text(pDX, IDC_COMMENT, m_Comment);
	DDX_Text(pDX, IDC_FULLNAME, m_FullName);
	DDX_Text(pDX, IDC_CITY, m_City);
	DDX_Text(pDX, IDC_STATE, m_State);
	DDX_Text(pDX, IDC_ZIP, m_Zip);
	DDX_Text(pDX, IDC_STORE_CUSTOMER_ID, m_StoreCustomerID);
	DDX_Text(pDX, IDC_BDATE_GENDER, m_Bdate_Gender);
	DDX_Text(pDX, IDC_EXTRA, m_Extra);
	DDX_Text(pDX, IDC_PHONES, m_Phones);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHighlightedCustomer, CDialog)
	//{{AFX_MSG_MAP(CHighlightedCustomer)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_TASK, OnTask)
	ON_BN_CLICKED(IDC_CHANGE_STATE, OnChangeState)
	ON_COMMAND(ID_CUSTOMER_NEW, OnCustomerNew)
	ON_COMMAND(ID_CUSTOMER_RECENT, OnCustomerRecent)
	ON_COMMAND(ID_CUSTOMER_ACTIVE, OnCustomerActive)
	ON_COMMAND(ID_TASK_CHARGE_CUSTOMER, OnTaskChargeCustomer)
	ON_COMMAND(ID_TASK_SEND_IMAGE, OnTaskSendImage)
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_ACTION, OnAction)
	ON_COMMAND(ID_ACTION_ADDVISIT, OnActionAddVisit)
	ON_COMMAND(ID_ACTION_REDEEMPURCHASE, OnActionRedeemPurchase)
	ON_COMMAND(ID_ACTION_ADDPURCHASE, OnActionAddPurchase)
	ON_COMMAND(ID_ACTION_REDEEMVISIT, OnActionRedeemVisit)
	ON_EN_CHANGE(IDC_COMMENT, OnChangeComment)
	ON_WM_DESTROY()
	ON_EN_CHANGE(IDC_STORE_CUSTOMER_ID, OnChangeStoreCustomerId)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
//	ON_WM_CREATE()
ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


/** CHighlightedCustomer message handlers */



void CHighlightedCustomer::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	/** @test Invalidate(FALSE);	*/ 
}

BOOL CHighlightedCustomer::OnInitDialog() 
{
	CDialog::OnInitDialog();

	return TRUE; /** return TRUE unless you set the focus to a control */
	       /** @exception OCX Property Pages should return FALSE */
}

void CHighlightedCustomer::CommitChanges()
{
	/** show dialog in proper location (depend on selected customer) */
	AdjustDialog();

	CFont fontHeader;
	VERIFY(fontHeader.CreatePointFont(180,_T("Times New Roman")));

	m_NickName.SetFont(&fontHeader, false);
	m_NickName.SetTextColor(RGB(128,0,0), false);
	m_NickName.SetWindowText(m_pCustomer->m_sNickname.c_str());

	m_Address = m_pCustomer->m_sAddress.c_str();
	m_FullName.Format(_T("%s %s"), m_pCustomer->m_sFirstName.c_str(),
									m_pCustomer->m_sLastName.c_str());
	m_City=m_pCustomer->m_sCity.c_str();
	m_State=m_pCustomer->m_sState.c_str();
	m_Zip=m_pCustomer->m_sZip.c_str();
	m_Comment=m_pCustomer->m_sComments.c_str();

	m_Bdate_Gender = m_pCustomer->m_sBirthdate.c_str();
	
	CString s;
	MYSTL_ITERATE_LIST(m_pCustomer->m_listPhoneNumbers,VIPPhoneNumber,pn,itPN)
	{
		s += pn->m_sNumber.c_str() + CString(_T(", "));
	}
	m_Phones=s;
	
	s=_T("");
	MYSTL_ITERATE_LIST(m_pCustomer->m_listAttributes,MiscVIPAttribute,pa,itPA)
	{
		s += pa->m_sType.c_str() + CString(_T(": ")) + 
			pa->m_sValue.c_str() + CString(_T(", "));
	}
	m_Extra=s;

	CString email=m_pCustomer->m_sEmail.c_str();
	if (!email.IsEmpty())
	{
		m_email.SetWindowText(email);

		email.Format(_T("mailto:%s"), m_pCustomer->m_sEmail.c_str());
		m_email.SetLinkUrl(email);

		/** force CMyHyperLink component to redraw its background */
		m_email.ShowWindow(SW_HIDE);
		m_email.ShowWindow(SW_SHOW);

	}
	else
	{
		m_email.ShowWindow(SW_HIDE);
	}
	
	CString szFolder;
	DWORD dwTerminals;
	CRegistry registry;
	registry.OpenKey(HKEY_CURRENT_USER, REGISTRY_KEY_VIPSERVICE_FULL);
	dwTerminals=registry.GetValueDWORD(REGISTRY_PAGE_TERMINALS_TERMINALS);
	if (dwTerminals)	/** True -> we use multiple terminals */
		szFolder=registry.GetValueString(REGISTRY_PAGE_TERMINALS_DIRECTORY);
	else
		szFolder=THUMBNAIL_IMAGE_DIRECTORY;
	registry.CloseKey();

	CString szPicture;
	szPicture.Format(_T("%s\\%d.jpg"), szFolder.GetBuffer(0),
										m_pCustomer->m_iPlutoId);

	/** Initialize Visit & Purchase information */
	CString szVisit;
	szVisit.Format(_T("Total Visit: <b>%d</b><br>Available Visit: <b>%d</b>"), 
		m_pCustomer->m_iTotalVisit, m_pCustomer->m_iAvailableVisit);
	m_Visit.SetWindowText(szVisit);

	CString szPurchase;
	szPurchase.Format(_T("Total Purchase: <b>%.2f</b><br>Available Purchase: <b>%.2f</b>"), 
		m_pCustomer->m_iTotalPurchase, m_pCustomer->m_iAvailablePurchase);
	m_Purchase.SetWindowText(szPurchase);
	
	/** read customer comment from PlutoId table */
	ReadCustomerComment();

	UpdateData(FALSE);
	
	/** Draw the image */
	m_Picture.SetPicture(szPicture);
}

void CHighlightedCustomer::KillMe(int iTimer)
{
	if (iTimer > 0 && NULL != m_hWnd)
	{
		m_Timer = iTimer;
		TRACE("Oh, I will  die soon!\n");
		
		SetTimer(DEFAULT_TIMER, m_Timer, NULL);		
	}
}

void CHighlightedCustomer::OnTimer(UINT nIDEvent) 
{
	switch (nIDEvent)
	{
		case DEFAULT_TIMER:
			TRACE("Oh, you killed me!\n");
			KillTimer(DEFAULT_TIMER);	
			ShowWindow(SW_HIDE);
			return;
	}
	
	CDialog::OnTimer(nIDEvent);
}

void CHighlightedCustomer::OnTask() 
{
	CRect rect;
	GetDlgItem(IDC_TASK)->GetWindowRect(&rect);
	/** @test ClientToScreen(&rect); */

	CMenu menu;
	if (!menu.LoadMenu(IDR_PLUTO_CONTEXT_MENU)) 
	{
		return;
	}
	
	CMenu* pSubMenu = menu.GetSubMenu(2);	/** 0: _DUMMY_, 1: _STATE_, 2: _TASK_, 3: _ACTION_ */
	if (pSubMenu == NULL) 
		return;
	
	::TrackPopupMenu(pSubMenu->m_hMenu, 0, 
		rect.left, rect.bottom, 0, m_hWnd, NULL);
	
	menu.DestroyMenu();
}

void CHighlightedCustomer::OnChangeState() 
{
	CRect rect;
	GetDlgItem(IDC_CHANGE_STATE)->GetWindowRect(&rect);
	CMenu menu;
	if (!menu.LoadMenu(IDR_PLUTO_CONTEXT_MENU)) 
		return;
	
	
	CMenu* pSubMenu = menu.GetSubMenu(1);	/** 0: _DUMMY_, 1: _STATE_, 2: _TASK_, 3: _ACTION_ */
	if (pSubMenu == NULL) 
		return;
	
	::TrackPopupMenu(pSubMenu->m_hMenu, 0, 
		rect.left, rect.bottom, 0, m_hWnd, NULL);
	
	menu.DestroyMenu();	
}

void CHighlightedCustomer::SetCustomer(Customer* pCustomer)
{
	m_pCustomer=pCustomer;
}

/** 0=not detected anymore, 1=new app bar, 2=active, 3=recent */
void CHighlightedCustomer::OnCustomerNew() 
{
	m_pCustomer->m_cAppBar=APPBAR_NEW;	
	
	CDialog::OnOK();
	g_TaskBars.RefreshAppBars();
}

void CHighlightedCustomer::OnCustomerRecent() 
{
	m_pCustomer->m_cAppBar=APPBAR_RECENT;

	CDialog::OnOK();
	g_TaskBars.RefreshAppBars();
}

void CHighlightedCustomer::OnCustomerActive() 
{
	m_pCustomer->m_cAppBar=APPBAR_ACTIVE;

	CDialog::OnOK();
	g_TaskBars.RefreshAppBars();
}

void CHighlightedCustomer::OnTaskChargeCustomer() 
{
	AfxMessageBox("Charge the customer");
}

void CHighlightedCustomer::OnTaskSendImage() 
{
	if(!m_pCustomer->GetCommandProcessor()) /** not connected */
		return;

	unsigned char ImageType = 0;
	unsigned long ImageSize;
	char *pImage;

	char CurrentFolder[1024];
	::GetCurrentDirectory(1024, CurrentFolder);

	CFileDialog* pDialog = new CFileDialog(true);

	if(IDOK == pDialog->DoModal())
	{
		string FilePath = pDialog->GetPathName();
		
		size_t s;
		pImage = FileUtils::ReadFileIntoBuffer(FilePath.c_str(), s);
		ImageSize = (unsigned long)s;

		::SetCurrentDirectory(CurrentFolder);

		BD_CP_ShowImage *pCommand = new BD_CP_ShowImage(ImageType, ImageSize, pImage, 0, "", false, false, 0);

		if(NULL != m_pCustomer->GetCommandProcessor()) //** not connected */
			m_pCustomer->GetCommandProcessor()->AddCommand(pCommand);

		delete pImage;
		pImage = NULL;
	}


	delete pDialog;
	pDialog = NULL;
}

void CHighlightedCustomer::SetPositionInfo(Position PosInfo)
{
	PositionInfo = PosInfo;
}

void CHighlightedCustomer::AdjustDialog()
{
	CRect rcTaskBar;

	APPBARDATA appBarData;
    appBarData.cbSize=sizeof(appBarData);
    if (SHAppBarMessage(ABM_GETTASKBARPOS,&appBarData))
		rcTaskBar=appBarData.rc;
	else
	{
		rcTaskBar.left=0;
		rcTaskBar.top=0;
		rcTaskBar.right=150;
		rcTaskBar.bottom=60;
	}
		
	CRect rect, Deskrect;
	GetClientRect(&rect);
	static int DialogWidth=rect.Width();
	static int DialogHeight=rect.Height();
	int DesktopWidth=GetSystemMetrics(SM_CXSCREEN);
	int DesktopHeight=GetSystemMetrics(SM_CYSCREEN);

	int x, y;
	
	GetDesktopWindow()->GetClientRect(&Deskrect);

	switch (PositionInfo.iDirection)
	{
		case ABE_LEFT:
			y = PositionInfo.pPosition.y;
			if (y > (DesktopHeight - DialogHeight - rcTaskBar.Height()))
				y=DesktopHeight-DialogHeight - rcTaskBar.Height();
			
			MoveWindow(PositionInfo.WidthHeight.x,
						y,
						DialogWidth, 
						DialogHeight);
			break;

		case ABE_RIGHT:
			y=PositionInfo.pPosition.y;
			if (y > (DesktopHeight - DialogHeight - rcTaskBar.Height()))
				y=DesktopHeight-DialogHeight - rcTaskBar.Height();
			
			MoveWindow(Deskrect.Width()/** @test DesktopWidth*/ - (PositionInfo.WidthHeight.x + rect.Width()),
						y, 
						DialogWidth, 
						DialogHeight);
			break;

		case ABE_TOP:
			x=PositionInfo.pPosition.x;
			if (x > (DesktopWidth - DialogWidth))
				x=DesktopWidth - DialogWidth;
			
			MoveWindow(x, 
						PositionInfo.WidthHeight.y,
						DialogWidth, 
						DialogHeight);
			break;

		case ABE_BOTTOM:
			x=PositionInfo.pPosition.x;
			if (x > (DesktopWidth - DialogWidth))
				x=DesktopWidth - DialogWidth;
			
			MoveWindow(x, 
						DesktopHeight - (PositionInfo.WidthHeight.y + rect.Height()),
						DialogWidth, 
						DialogHeight);
			break;
	}
}

void CHighlightedCustomer::OnMouseMove(UINT nFlags, CPoint point) 
{
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(tme);
	tme.hwndTrack = m_hWnd;
	tme.dwFlags = TME_LEAVE|TME_HOVER;
	tme.dwHoverTime = 1;
	_TrackMouseEvent(&tme);
	
	CDialog::OnMouseMove(nFlags, point);
}

LRESULT CHighlightedCustomer::OnMouseHover(WPARAM wparam, LPARAM lparam) 
{
	KillTimer(0);
	return S_OK;
}


LRESULT CHighlightedCustomer::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	/** if mouse is in dialog return without doing anything
	 * but if mouse is outside of dialog set timer to kill dialog */
	POINT point;
	GetCursorPos(&point);

	CRect rect;
	GetClientRect(&rect);
	ClientToScreen(&rect);

	if (rect.PtInRect(point))
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = /**  @test TME_LEAVE|*/TME_HOVER;
		tme.dwHoverTime = 1;
		_TrackMouseEvent(&tme);
		return 0;
	}
	else
		if(NULL != m_hWnd)
			SetTimer(DEFAULT_TIMER, 500, NULL);
	
	return 0;
}

void CHighlightedCustomer::OnAction() 
{
	CRect rect;
	GetDlgItem(IDC_ACTION)->GetWindowRect(&rect);
	CMenu menu;
	if (!menu.LoadMenu(IDR_PLUTO_CONTEXT_MENU)) 
		return;
	
	
	CMenu* pSubMenu = menu.GetSubMenu(3);	/** 0: _DUMMY_, 1: _STATE_, 2: _TASK_, 3: _ACTION_ */
	if (pSubMenu == NULL) 
		return;
	
	::TrackPopupMenu(pSubMenu->m_hMenu, 0, 
		rect.left, rect.bottom, 0, m_hWnd, NULL);
	
	menu.DestroyMenu();			
}


void CHighlightedCustomer::OnActionAddVisit() 
{
	CAddVisit visit;
	if (visit.DoModal()==IDOK)
	{
		CADODatabase DB;
		CADORecordset record;
		//int PlutoID;
		int Visit=visit.m_Visit;
		
		//CString strConnection = DATABASE_PROVIDER;
		if(DB.Open(DATABASE_PROVIDER))
		{	
			record = CADORecordset(&DB);
			CString szTable;
								
			/** add new record to LoyaltyVisit Table */
			szTable=(_T("[LoyaltyVisit]"));
			if (record.Open(szTable, CADORecordset::openTable))
			{
				CTime Now=CTime::GetCurrentTime();
				record.AddNew();
				record.SetFieldValue(_T("FKID_PlutoId"), m_pCustomer->m_iPlutoId);
				record.SetFieldValue(_T("VisitDateTime"), Now.FormatGmt(_T("%c")));
				record.SetFieldValue(_T("Quantity"), Visit);
				record.Update();
				record.Close();
			}
			
			/** also update pCustomer	 */
			m_pCustomer->m_iAvailableVisit+=Visit;
			m_pCustomer->m_iTotalVisit+=Visit;

			/** update customer data in PlutoId table */
			szTable.Format(_T("SELECT PlutoId.* FROM [PlutoId] WHERE PlutoId.PKID_PlutoId=%d"),
				m_pCustomer->m_iPlutoId);
			
			if (record.Open(szTable, CADORecordset::openQuery))
			{
				record.Edit();
				record.SetFieldValue(_T("TotalVisits"), m_pCustomer->m_iTotalVisit);
				record.SetFieldValue(_T("AvailableVisits"), m_pCustomer->m_iAvailableVisit);
				record.Update();
				record.Close();
			}

			DB.Close();
		}
		
		
		CString szVisit;
		szVisit.Format(_T("Total Visit: <b>%d</b><br>Available Visit: <b>%d</b>"), 
			m_pCustomer->m_iTotalVisit, m_pCustomer->m_iAvailableVisit);
		m_Visit.SetWindowText(szVisit);
	}
}

void CHighlightedCustomer::OnActionAddPurchase() 
{
	CAddPurchase purchase;
	if (purchase.DoModal()==IDOK)
	{
		CADODatabase DB;
		CADORecordset record;
		//int PlutoID;
		double Purchase=purchase.m_Purchase;
		
		//CString strConnection = DATABASE_PROVIDER;
		if(DB.Open(DATABASE_PROVIDER))
		{	
			record = CADORecordset(&DB);
			CString szTable;
			
			/** add new record to LoyaltyVisit Table */
			szTable=(_T("[LoyaltyPurchase]"));
			if (record.Open(szTable, CADORecordset::openTable))
			{
				CTime Now=CTime::GetCurrentTime();
				record.AddNew();
				record.SetFieldValue(_T("FKID_PlutoId"), m_pCustomer->m_iPlutoId);
				record.SetFieldValue(_T("VisitDateTime"), Now.FormatGmt(_T("%c")));
				record.SetFieldValue(_T("Quantity"), Purchase);
				record.Update();
				record.Close();
			}
			
			/** also update pCustomer	*/
			m_pCustomer->m_iAvailablePurchase+=Purchase;
			m_pCustomer->m_iTotalPurchase+=Purchase;
			
			/** update customer data in PlutoId table */
			szTable.Format(_T("SELECT PlutoId.* FROM [PlutoId] WHERE PlutoId.PKID_PlutoId=%d"),
				m_pCustomer->m_iPlutoId);
			
			if (record.Open(szTable, CADORecordset::openQuery))
			{
				record.Edit();
				record.SetFieldValue(_T("TotalPurchase"), m_pCustomer->m_iTotalPurchase);
				record.SetFieldValue(_T("AvailablePurchase"), m_pCustomer->m_iAvailablePurchase);
				record.Update();
				record.Close();
			}
			
			DB.Close();
		}
		
		
		CString szPurchase;
		szPurchase.Format(_T("Total Purchase: <b>%.2f</b><br>Available Purchase: <b>%.2f</b>"), 
			m_pCustomer->m_iTotalPurchase, m_pCustomer->m_iAvailablePurchase);
		m_Purchase.SetWindowText(szPurchase);
	}
}

void CHighlightedCustomer::OnActionRedeemPurchase() 
{
	CRedeemPurchase purchase;
	purchase.SetPurchaseStatistics(m_pCustomer->m_iTotalPurchase, m_pCustomer->m_iAvailablePurchase);
	if (purchase.DoModal()==IDOK)
	{
		CADODatabase DB;
		CADORecordset record;
		//int PlutoID;
		double RedeemPurchase=purchase.m_Redeem;
		CString Description=purchase.m_Description;
		
		//CString strConnection = DATABASE_PROVIDER;
		if(DB.Open(DATABASE_PROVIDER))
		{	
			record = CADORecordset(&DB);
			CString szTable;
			
			/** add new record to LoyaltyVisit Table */
			szTable=(_T("[LoyaltyPurchase_Redeem]"));
			if (record.Open(szTable, CADORecordset::openTable))
			{
				CTime Now=CTime::GetCurrentTime();
				record.AddNew();
				record.SetFieldValue(_T("FKID_PlutoId"), m_pCustomer->m_iPlutoId);
				record.SetFieldValue(_T("PurchaseDateTime"), Now.FormatGmt(_T("%c")));
				record.SetFieldValue(_T("Quantity"), RedeemPurchase);
				record.SetFieldValue(_T("Description"), Description);
				record.Update();
				record.Close();
			}
			
			/** also update pCustomer	 */
			m_pCustomer->m_iAvailablePurchase-=RedeemPurchase;
						
			/** update customer data in PlutoId table */
			szTable.Format(_T("SELECT PlutoId.* FROM [PlutoId] WHERE PlutoId.PKID_PlutoId=%d"),
				m_pCustomer->m_iPlutoId);
			
			if (record.Open(szTable, CADORecordset::openQuery))
			{
				record.Edit();
				record.SetFieldValue(_T("AvailablePurchase"), m_pCustomer->m_iAvailablePurchase);
				record.Update();
				record.Close();
			}
			
			DB.Close();
		}
		
		CString szPurchase;
		szPurchase.Format(_T("Total Purchase: <b>%.2f</b><br>Available Purchase: <b>%.2f</b>"), 
			m_pCustomer->m_iTotalPurchase, m_pCustomer->m_iAvailablePurchase);
		m_Purchase.SetWindowText(szPurchase);
	}	
}



void CHighlightedCustomer::OnActionRedeemVisit() 
{
	CRedeemVisit visit;
	visit.SetVisitStatistics(m_pCustomer->m_iTotalVisit, m_pCustomer->m_iAvailableVisit);
	if (visit.DoModal()==IDOK)
	{
		CADODatabase DB;
		CADORecordset record;
		//int PlutoID;
		int RedeemVisit=visit.m_Redeem;
		CString Description=visit.m_Description;
		
		//CString strConnection = DATABASE_PROVIDER;
		if(DB.Open(DATABASE_PROVIDER))
		{	
			record = CADORecordset(&DB);
			CString szTable;
			
			/** add new record to LoyaltyVisit Table */
			szTable=(_T("[LoyaltyVisit_Redeem]"));
			if (record.Open(szTable, CADORecordset::openTable))
			{
				CTime Now=CTime::GetCurrentTime();
				record.AddNew();
				record.SetFieldValue(_T("FKID_PlutoId"), m_pCustomer->m_iPlutoId);
				record.SetFieldValue(_T("VisitDateTime"), Now.FormatGmt(_T("%c")));
				record.SetFieldValue(_T("Quantity"), RedeemVisit);
				record.SetFieldValue(_T("Description"), Description);
				record.Update();
				record.Close();
			}
			
			/** also update pCustomer	 */
			m_pCustomer->m_iAvailableVisit-=RedeemVisit;
			
			/** update customer data in PlutoId table */
			szTable.Format(_T("SELECT PlutoId.* FROM [PlutoId] WHERE PlutoId.PKID_PlutoId=%d"),
				m_pCustomer->m_iPlutoId);
			
			if (record.Open(szTable, CADORecordset::openQuery))
			{
				record.Edit();
				record.SetFieldValue(_T("AvailableVisits"), m_pCustomer->m_iAvailableVisit);
				record.Update();
				record.Close();
			}
			
			DB.Close();
		}
		
		CString szVisit;
		szVisit.Format(_T("Total Visit: <b>%d</b><br>Available Visit: <b>%d</b>"), 
			m_pCustomer->m_iTotalVisit, m_pCustomer->m_iAvailableVisit);
		m_Visit.SetWindowText(szVisit);
	}	
}



void CHighlightedCustomer::ReadCustomerComment()
{
	CADODatabase DB;
	CADORecordset record;

	//CString strConnection = DATABASE_PROVIDER;
	if(DB.Open(DATABASE_PROVIDER))
	{	
		record = CADORecordset(&DB);
		CString szTable;
		
		//update customer comment in PlutoId table
		szTable.Format(_T("SELECT PlutoId.* FROM [PlutoId] WHERE PlutoId.PKID_PlutoId=%d"),
			m_pCustomer->m_iPlutoId);
		
		if (record.Open(szTable, CADORecordset::openQuery))
		{
			CString Comments;
			int StoreCustomerID;
			record.GetFieldValue(_T("Comments"), Comments);
			record.GetFieldValue(_T("StoreCustomerId"), StoreCustomerID);
			record.Close();
			
			m_StoreCustomerID=StoreCustomerID;
			m_Comment=Comments;
		}
		
		DB.Close();
	}
}	


void CHighlightedCustomer::OnChangeComment() 
{
	CommentsIsChanged=TRUE;	
}

void CHighlightedCustomer::OnDestroy() 
{
	CDialog::OnDestroy();

	/** Save the customer comments in PlutoId table */
	if (CommentsIsChanged)
	{
		CADODatabase DB;
		CADORecordset record;
		
		/** CString strConnection = DATABASE_PROVIDER; */
		if(DB.Open(DATABASE_PROVIDER))
		{	
			record = CADORecordset(&DB);
			CString szTable;
			
			/** update customer comment in PlutoId table */
			szTable.Format(_T("SELECT PlutoId.* FROM [PlutoId] WHERE PlutoId.PKID_PlutoId=%d"),
				m_pCustomer->m_iPlutoId);
			
			if (record.Open(szTable, CADORecordset::openQuery))
			{
				UpdateData(TRUE);
				CString Comments=m_Comment;
				record.Edit();
				record.SetFieldValue(_T("Comments"), Comments);
				record.SetFieldValue(_T("StoreCustomerId"), m_StoreCustomerID);
				record.Update();
				record.Close();
				
				m_Comment=Comments;
			}
			
			DB.Close();
		}
	}
}

void CHighlightedCustomer::OnChangeStoreCustomerId() 
{
	CommentsIsChanged=TRUE;		
}

void CHighlightedCustomer::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);
}
