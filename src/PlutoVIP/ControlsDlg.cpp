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
 * @file  ControlsDlg.cpp												
 * @brief source file @todo ask
 */
 
 /**
  *
  * Copyright: copyright information goes here
  *
  */


#include "stdafx.h"
#include "PlutoUtils/CommonIncludes.h"	
#include "VIPShared/VIPIncludes.h"
#include "VIPShared/PlutoConfig.h"
#include "PlutoVIP.h"
#include "ControlsDlg.h"
#include "Dib.h"
#include "HighlightedCustomer.h"
#include "ImageDefs.h"
#include "PlutoVIPMain.h"
#include "registry.h"
#include "ImageDefs.h"
#include "VIPShared/VIPMenu.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define RUN_TIMER		10
#define DETECT_MOUSE	11

extern map<unsigned long, Customer*> m_mapCustomers;

#define SPACE_FROM_BORDER	2  //2 px
#define SPACE_FOR_NICKNAME	20	//20 px
#define DETECT_MOUSE_TIMER	300
#define POINT_OFFSET		15


/** extern*/ TaskBarsContainer g_TaskBars;



CControlsDlg::CControlsDlg(CWnd* pParent /** @test =NULL*/)
	: CAppBar(CControlsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CControlsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_ImageType=IMAGE_TYPE_UNKNOWN;
	HighlighDlg=NULL;
	m_pParent=NULL;
	/** @test m_Minute=5; */
}


CControlsDlg::~CControlsDlg()
{
	if (HighlighDlg!=NULL)
		delete HighlighDlg;
}

void CControlsDlg::DoDataExchange(CDataExchange* pDX)
{
	CAppBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CControlsDlg)
	//}}AFX_DATA_MAP
	//DDX_Control(pDX, IDC_START, m_btnStart);
}


BEGIN_MESSAGE_MAP(CControlsDlg, CAppBar)
	//{{AFX_MSG_MAP(CControlsDlg)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_APPBAR_AUTOHIDE, OnAppbarAutohide)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_SYSCOMMAND()
	ON_WM_TIMER()
	ON_NOTIFY(NM_CLICK, IDC_CUSTOMER, OnClickCustomer)
	ON_WM_DESTROY()
	ON_NOTIFY(LVN_HOTTRACK, IDC_CUSTOMER, OnActivateCustomer)
	ON_BN_CLICKED(IDOK, OnOK)
	ON_BN_CLICKED(IDCANCEL, OnOK)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	//}}AFX_MSG_MAP
//	ON_WM_CREATE()
ON_WM_WINDOWPOSCHANGED()
ON_WM_ACTIVATE()
END_MESSAGE_MAP()


/** CControlsDlg message handlers */
/** @test
void CControlsDlg::OnStart() 
{
    CRect rect;
    m_btnStart.GetWindowRect (&rect);
//    ShowMenu (rect, IDR_EPIC_BUTTON_MENU, -1);
}
*/


BOOL CControlsDlg::OnInitDialog() 
{
	CAppBar::OnInitDialog();
	
	CRect rc;
	GetClientRect(&rc);

	/** prepare the appbar */
	PrepareAppBar();

	/** Prepare the Image List */
	PrepareImageList();

	return TRUE; /** return TRUE unless you set the focus to a control */
	       /**@exception OCX Property Pages should return FALSE */
}

void CControlsDlg::SetupButtons()
{	
	/** prepare the image list */
	PrepareImageList();
	
	int Position=0;
	POINT pt;

	map<unsigned long,class Customer *>::iterator itCustomers;
	for (itCustomers=/** @test m_pDetection->*/m_mapCustomers.begin();
		 itCustomers!=/** @test m_pDetection->*/m_mapCustomers.end();
		 ++itCustomers)
	{
		Customer *pCustomer = (*itCustomers).second;
		if(!pCustomer || pCustomer->m_cAppBar!=m_ImageType) 
			/** do not add this customer */
			continue;
		
		int CustomerImage=-1;		/** find image of customer */
		map<unsigned long, PlutoCustomer*>::iterator it = mapPlutoImageList.find(pCustomer->m_iPlutoId);
		if( it!=mapPlutoImageList.end() )
		{
			/** image was found. */
			CustomerImage=mapPlutoImageList[pCustomer->m_iPlutoId]->ImageNumber;
		}

		/** shift the thumbnail to desired position */
		int ThumbnailSize=GetImageSize();

		if (m_Side==ABE_TOP || m_Side==ABE_BOTTOM)
		{
			/** pt.x = CUSTOMER_IMAGE_GAP + NewItem * (ThumbnailSize + CUSTOMER_IMAGE_GAP); */
			pt.y = 0;/** SPACE_FROM_BORDER; */
		}
		else	//ABE_LEFT || ABE_RIGHT
		{
			pt.x = SPACE_FROM_BORDER;
			/** pt.y = CUSTOMER_IMAGE_GAP + NewItem * (ThumbnailSize + CUSTOMER_IMAGE_GAP); */
		}
	}

	CDC* pDC = GetDC();
	RefreshTaskBar(pDC);
	ReleaseDC(pDC);
}

void CControlsDlg::SetImageType(int iImage)
{
	m_ImageType=iImage;
}

void CControlsDlg::OnOK()
{

}

void CControlsDlg::SetWindowInvisible()
{
	ShowWindow(SW_HIDE);
}

void CControlsDlg::SetWindowVisible()
{
	ShowWindow(SW_SHOW);
}

void CControlsDlg::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent==RUN_TIMER)
	{

		map<unsigned long,class Customer *>::iterator itCustomers;
		for (itCustomers = m_mapCustomers.begin(); itCustomers != m_mapCustomers.end(); ++itCustomers)
		{
			Customer *pCustomer = (*itCustomers).second;
			if(!pCustomer) 
				continue;

			PlutoCustomer* pPlutoCustomer=NULL;

			int CustomerImage = -1;		/** find image of customer */
			map<unsigned long, PlutoCustomer*>::iterator it = mapPlutoImageList.find(pCustomer->m_iPlutoId);
			if( it != mapPlutoImageList.end() )
			{
				pPlutoCustomer=(*it).second;

				CTime Now = CTime::GetCurrentTime();
				CTimeSpan Diff;
				Diff = Now - pPlutoCustomer->Time;
				
				if (Diff.GetTotalSeconds() >= m_Minute * 60) 
				{
					/** change the customer appbar */
					switch (pCustomer->m_cAppBar) 
					{
						case APPBAR_NEW:
							pPlutoCustomer->Time=CTime::GetCurrentTime();
							pCustomer->m_cAppBar=APPBAR_RECENT;
							break;
							
						case APPBAR_RECENT:
							pPlutoCustomer->Time=CTime::GetCurrentTime();
							pCustomer->m_cAppBar=-1;
							break;
					}//switch
				}//if
			}//if
		}//for

	g_TaskBars.RefreshAppBars();

	}//if RUN_TIMER

	CAppBar::OnTimer(nIDEvent);
}

void CControlsDlg::RunTimer(int iMinute)
{
	m_Minute=iMinute;
	SetTimer(RUN_TIMER /** @test nIDEvent*/, iMinute * 60 * 1000, NULL);
}

void CControlsDlg::SetPhoneDetection(PhoneDetection_VIP *phoneDetection)
{
	m_pDetection=phoneDetection;
}

void CControlsDlg::SetParent(CDialog *pParent)
{
	ASSERT(pParent!=NULL);
	m_pParent=pParent;
}

void CControlsDlg::SetDesktopSide(int Side)
{
	m_Side=Side;
}


void CControlsDlg::HideFloatAdornments(BOOL fHide) {
	
	if (fHide) 
	{
		ModifyStyle(WS_CAPTION | WS_SYSMENU, 0, SWP_DRAWFRAME);
	} 
	else 
	{
		ModifyStyle(0, WS_CAPTION | WS_SYSMENU, SWP_DRAWFRAME);
	}
}



/** Overridable functions */

/** Tell our derived class that there is a proposed state change */
void CControlsDlg::OnAppBarStateChange (BOOL fProposed, UINT uStateProposed) {
	
	/** Hide the window adorments when docked. */
	HideFloatAdornments((uStateProposed == ABE_FLOAT) ? FALSE : TRUE);
}

void CControlsDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) {

  /**
   * Get the minimum size of the window assuming it has no client area.
   * This is the width/height of the window that must always be present
   */
   CRect rcBorder(0, 0, 0, 0);
   AdjustWindowRectEx(&rcBorder, GetStyle(), FALSE, GetExStyle());

   if (GetState() == ABE_FLOAT) {
      lpMMI->ptMinTrackSize.x = m_szMinTracking.cx + rcBorder.Width();
      lpMMI->ptMinTrackSize.y = m_szMinTracking.cy + rcBorder.Height();
   }

  /** The appbar can't be more than half the width or height of the screen when docked or when floating */
   lpMMI->ptMaxTrackSize.x = GetSystemMetrics(SM_CXSCREEN);
   lpMMI->ptMaxTrackSize.y = GetSystemMetrics(SM_CYSCREEN);
   if (!IsEdgeTopOrBottom(GetState()))
      lpMMI->ptMaxTrackSize.x /= 2;

   if (!IsEdgeLeftOrRight(GetState()))
      lpMMI->ptMaxTrackSize.y /= 2;
	
	CAppBar::OnGetMinMaxInfo(lpMMI);
}





void CControlsDlg::OnSize(UINT nType, int cx, int cy) {

	CAppBar::OnSize(nType, cx, cy);

	SetupButtons();
}





void CControlsDlg::OnContextMenu(CWnd* pWnd, CPoint point) {

  /** We cannot use MFC's Command Update Handlers for the menu items because  they only work when you have a CFrameWnd-derived class. */
   CMenu menu;
	menu.LoadMenu(MAKEINTRESOURCE(IDR_PLUTO_CONTEXT_MENU));
   menu.CheckMenuItem(ID_APPBAR_ALWAYSONTOP, MF_BYCOMMAND | 
      IsBarAlwaysOnTop() ? MF_CHECKED : MF_UNCHECKED);
   menu.CheckMenuItem(ID_APPBAR_AUTOHIDE, MF_BYCOMMAND | 
      IsBarAutohide() ? MF_CHECKED : MF_UNCHECKED);
	menu.GetSubMenu(0)->TrackPopupMenu(0, 
		point.x, point.y, this, NULL);	
}



/** CAppBar command handlers */


void CControlsDlg::OnSysCommand(UINT nID, LPARAM lParam) {
	/** @todo Add your message handler code here and/or call default */

   if (nID == SC_CLOSE) {
   /**
    * We have to manually add this so that the dialog box closes when 
    * when the user clicks the close button ( which appears in the top, right )
    * corner of the dialog box when it is floating ). 
    */
      OnCancel();
   }
	
	CAppBar::OnSysCommand(nID, lParam);
}




void CControlsDlg::OnAppbarAutohide() {

   /** Get the current state and change the auto-hide setting only. */
   APPBARSTATE abs;
	abs.m_cbSize = sizeof(abs);
	GetState(&abs);
	abs.m_fAutohide = !IsBarAutohide();
   SetState(abs);

   CRegistry reg;
   reg.OpenKey(HKEY_CURRENT_USER, REGISTRY_KEY_VIPSERVICE_FULL);
   if (reg.IsOpen())
   {
	   CString temp;
	   temp.Format(_T("Panel%sAppearance"), m_AppBarName);
	   
		if (IsBarAutohide())
			reg.SetValueDWORD(temp, APPBAR_APPEARANCE_AUTOHIDE);
		else
			reg.SetValueDWORD(temp, APPBAR_APPEARANCE_VISIBLE);

   }
   reg.CloseKey();
}

void CControlsDlg::OnCancel() 
{
	CAppBar::OnCancel();
}


void CControlsDlg::SetAppBarName(CString szName)
{
	m_AppBarName=szName;
}

void CControlsDlg::PrepareAppBar()
{
	/** Set Appbars Name */
	SetWindowText(m_AppBarName);
	
	CRect rc, dlgrc;

	CRegistry reg;
	CString RegName=m_AppBarName + CString(APPBAR_CX);

	DWORD dwCx=0;
	DWORD dwAutoHide=0;

	if (reg.OpenKey(HKEY_CURRENT_USER, REGISTRY_KEY_VIPSERVICE_FULL)!=NULL)
	{
		/** load the AppBar's state variables from the registry. */
		dwCx=reg.GetValueDWORD(RegName);
		CString RegStr=m_AppBarName + CString(APPBAR_AUTOHIDE);
		dwAutoHide=reg.GetValueDWORD(RegStr);

		if (dwCx!=0)
		{
			if (m_Side==ABE_LEFT || m_Side==ABE_RIGHT)
			{
				SetWindowPos(NULL, 0, 0, dwCx, 0/** rc.Height()*/, SWP_SHOWWINDOW);
			}
			else
			{
				SetWindowPos(NULL, 0, 0, 0/** rc.Width()*/, dwCx, SWP_SHOWWINDOW);
			}
		}
		else
		{
			if (m_Side==ABE_LEFT || m_Side==ABE_RIGHT)
			{

				SetWindowPos(NULL, 0, 0, 0/** rc.Height()*/, dlgrc.Width(), SWP_SHOWWINDOW);
			}
			else
			{

				SetWindowPos(NULL, 0, 0, dlgrc.Width(), 0/** rc.Height()*/, SWP_SHOWWINDOW);

			}
		}
		reg.CloseKey();
	}
	
/** @test
	if (m_Side==ABE_LEFT || m_Side==ABE_RIGHT)
	{
		//m_Customer.SetWindowPos(NULL, 0, 0, rc.Height(), dlgrc.Height(), SWP_SHOWWINDOW);
		SetWindowPos(NULL, 0, 0, rc.Height(), dlgrc.Width(), SWP_SHOWWINDOW);
	}
	else
	{
		//m_Customer.SetWindowPos(NULL, 0, 0, dlgrc.Width(), rc.Height(), SWP_SHOWWINDOW);
		SetWindowPos(NULL, 0, 0, dlgrc.Width(), rc.Height(), SWP_SHOWWINDOW);
	}
*/	
	/** Set the CAppBar class's behavior flags */
	m_fdwFlags = ABF_ALLOWANYWHERE | ABF_MIMICTASKBARAUTOHIDE;
	
	GetClientRect(&rc);
	
	/** Width has no limits, height sizes in client-area-height increments */
	
	
	
	/** The appbar has a minimum client-area size that is determined by the  client area set in the dialog box template. */
	
    if (m_Side==ABE_LEFT || m_Side==ABE_RIGHT)
	{
		m_szSizeInc.cx = 1;/** rc.Width(); */
		m_szSizeInc.cy = 1; 
		m_szMinTracking.cx = dwCx;/** rc.Width(); */
		m_szMinTracking.cy = rc.Height();
	}
	else
	{
		m_szSizeInc.cx = 1;
		m_szSizeInc.cy = 1;/** rc.Height(); */
		m_szMinTracking.cx = rc.Width();/** rc.Height(); */
		m_szMinTracking.cy = dwCx;/** rc.Width(); */
	}
	
	/** Setup default state data for the AppBar */
	APPBARSTATE abs;
	abs.m_cbSize = sizeof(abs);
	abs.m_uState = m_Side/** ABE_TOP*/;
	abs.m_fAutohide = dwAutoHide; /** FALSE; */
	abs.m_fAlwaysOnTop = TRUE;/** FALSE; //TRUE; */
	
	/** Set the default floating location of the appbar */
	GetWindowRect(&abs.m_rcFloat);
	
	/** Make the default width twice the width set in the dialog editor */
	if (m_Side==ABE_LEFT || m_Side==ABE_RIGHT)
		abs.m_rcFloat.bottom += abs.m_rcFloat.Height();
	else
		abs.m_rcFloat.right += abs.m_rcFloat.Width();		
	
	/** Temporarily turn off window adornments to determine the dimensions of the appbar when docked. */
	HideFloatAdornments(TRUE);
	AdjustWindowRectEx(&rc, GetStyle(), FALSE, GetExStyle());
	HideFloatAdornments(FALSE);
	
	abs.m_auDimsDock[ABE_LEFT]   = rc.Width();
	abs.m_auDimsDock[ABE_TOP]    = rc.Height();
	abs.m_auDimsDock[ABE_RIGHT]  = rc.Width();
	abs.m_auDimsDock[ABE_BOTTOM] = rc.Height();
	
	/** Check the registry to see if we have been used before and if so, reload our persistent settings. */
	/** @test
	CRegistry reg;

	if ( reg.OpenKey( HKEY_CURRENT_USER, REGISTRY_KEY_VIPSERVICE_FULL )!=NULL )
	{
		DWORD cbData = sizeof( abs );
		reg.GetValueBinary( m_AppBarName, ( PBYTE ) &abs, &cbData );
		reg.CloseKey( );
	}
	*/

	/** Set the initial state of the appbar. */
	SetState(abs);
}

void CControlsDlg::OnDestroy() 
{
	const int iOffsetX = 8;
	const int iOffsetY = 26;

	CAppBar::OnDestroy();
	
	CRect rc;
	GetClientRect(&rc);

	/** Save the current state of the appbar in the registry so that we'll come up in the same state the next time the user runs us. */
	CRegistry reg;
	CString RegName=m_AppBarName + CString(APPBAR_CX);

	reg.OpenKey(HKEY_CURRENT_USER, REGISTRY_KEY_VIPSERVICE_FULL);
	
	/** Save the AppBar's state variables to the registry.*/
	if (m_Side==ABE_LEFT || m_Side==ABE_RIGHT)
		reg.SetValueDWORD(RegName, rc.Width() + iOffsetX);
	else
		reg.SetValueDWORD(RegName, rc.Height() + iOffsetY);

	RegName=m_AppBarName + CString(APPBAR_AUTOHIDE);

	/** save the appbar auto hidden feature */
	reg.SetValueDWORD(RegName, IsBarAutohide());
	
	reg.CloseKey();
}

void CControlsDlg::AddCustomerImageToImageList(ULONG Id, CString FileName, int iThumbnailSize)
{
	CBitmap*    pImage = NULL;	
	HBITMAP     hBitmap = NULL;
	HPALETTE    hPal = NULL;
	HDC			hMemDC = NULL;	
	HGDIOBJ		hOldObj = NULL;
	
	CDib		dib;
	int			nWidth, nHeight;

	CFile ImgFile;
		
	if( !ImgFile.Open(FileName, CFile::modeRead) )
	{
		/** delete pImage; we have not allocated memory for pImage */
		mapPlutoImageList[Id]->ImageNumber=-1;
		return;
	}

	if( dib.Read(ImgFile)==0 )
	{
		ImgFile.Close();
		/** delete pImage; we have not allocated memory for pImage */
		mapPlutoImageList[Id]->ImageNumber=-1;
		return;
	}
	ImgFile.Close();

	nWidth = dib.m_pBMI->bmiHeader.biWidth;
	nHeight = dib.m_pBMI->bmiHeader.biHeight;
	dib.m_pBMI->bmiHeader.biWidth = iThumbnailSize;
	dib.m_pBMI->bmiHeader.biHeight = iThumbnailSize;
		
	/** create thumbnail bitmap section */
	hBitmap = ::CreateDIBSection(NULL, 
								 dib.m_pBMI, 
							     DIB_RGB_COLORS, 
							     NULL, 
							     NULL, 
							     0); 

	/** restore dib header */
	dib.m_pBMI->bmiHeader.biWidth = nWidth;
	dib.m_pBMI->bmiHeader.biHeight = nHeight;

	/** select thumbnail bitmap into screen dc */
	hMemDC = ::CreateCompatibleDC(NULL);	
	hOldObj = ::SelectObject(hMemDC, hBitmap);
	  
	/** grayscale image, need palette */
	if(dib.m_pPalette != NULL)
	{
		hPal = ::SelectPalette(hMemDC, (HPALETTE)dib.m_pPalette->GetSafeHandle(), FALSE);
		::RealizePalette(hMemDC);
	}
	  
	/** set stretch mode */
	::SetStretchBltMode(hMemDC, COLORONCOLOR);

	/** populate the thumbnail bitmap bits */
		::StretchDIBits(hMemDC, 0, 0, 
					iThumbnailSize, iThumbnailSize, 
					0, 0, 
					dib.m_pBMI->bmiHeader.biWidth, 
					dib.m_pBMI->bmiHeader.biHeight, 
					dib.m_pBits, 
					dib.m_pBMI, 
					DIB_RGB_COLORS, 
					SRCCOPY);

	/** restore DC object */
	::SelectObject(hMemDC, hOldObj);
	  
	/** restore DC palette */
	if(dib.m_pPalette != NULL)
		::SelectPalette(hMemDC, (HPALETTE)hPal, FALSE);
	  
	/** clean up */
	::DeleteObject(hMemDC);
		
	/** attach the thumbnail bitmap handle to an CBitmap object */
	pImage = new CBitmap();		 
	pImage->Attach(hBitmap);

	/** add bitmap to our image list */
	int ImageNumber=CustomerImageList.Add(pImage, (CBitmap*) NULL);
	
	/** save image number in mapPlutoImageList */
	mapPlutoImageList[Id]->ImageNumber=ImageNumber;

	delete pImage;
}

void CControlsDlg::PrepareImageList()
{
	int iSize=GetImageSize();

	if (CustomerImageList.m_hImageList==NULL)
	{
		CustomerImageList.Create(iSize, iSize, ILC_COLOR24, 10, 10);
	}
	else		/** we had image list, delete all items and read new images */
	{
		CustomerImageList.DeleteImageList();
		ASSERT(CustomerImageList.GetSafeHandle() == NULL);		

		/** create the image list again */
		CustomerImageList.Create(iSize, iSize, ILC_COLOR24, 10, 10);

		map<unsigned long, Customer*>::iterator itCustomers;
		for (itCustomers=/** @test m_pDetection->*/m_mapCustomers.begin();
		itCustomers!=/** @test m_pDetection->*/m_mapCustomers.end();
		++itCustomers)
		{
			Customer *pCustomer = (*itCustomers).second;
			if(!pCustomer || pCustomer->m_cAppBar!=m_ImageType) 
				/** do not add this customer */
				continue;
			
			/** find a customer */
			ULONG CustomerID=pCustomer->m_iPlutoId;

			/** Add customer image to image list */
			CString CustomerImageFile=FindCustomerImageFileFromID(CustomerID, GRAPHICTYPE_BMP);
			AddCustomerImageToImageList(CustomerID, CustomerImageFile, iSize);
		}

	}
}


int CControlsDlg::GetImageSize()
{
	CRect rc;
	GetWindowRect(&rc);

	int VH=0;
	
	int Items = 1;/** m_Customer.GetItemCount(); */
	int iSize=min(rc.Width(), rc.Height());
	
	if (m_Side==ABE_LEFT || m_Side==ABE_RIGHT)
	{
		if (Items * (iSize + SPACE_FOR_NICKNAME ) > rc.Height())
		{
			/** we need the scroll bar */
			VH=GetSystemMetrics(SM_CXVSCROLL);
		}
	}
	else
	{
		if (Items * (iSize + 4) > rc.Width())
		{
			/** we need the scroll bar */
			VH=GetSystemMetrics(SM_CYHSCROLL);
		}

		VH += SPACE_FOR_NICKNAME;
	}
	
	/** @test iSize = iSize - 2 * SPACE_FROM_BORDER - VH;	 */
	iSize = iSize - 4 * SPACE_FROM_BORDER;	

	return iSize; 
}


void CControlsDlg::OnClickCustomer(NMHDR* pNMHDR, LRESULT* pResult) 
{
	FireHighlightedCustomerDialog();
}

void CControlsDlg::OnActivateCustomer(NMHDR* pNMHDR, LRESULT* pResult) 
{
	FireHighlightedCustomerDialog();
}

void CControlsDlg::FireHighlightedCustomerDialog()
{	
	static DWORD iSelectedItemId=0;
}

void CControlsDlg::OnPaint()
{
	CPaintDC dc(this); /**< device context for painting */
	/** @todo Add your message handler code here */
	/** Do not call CAppBar::OnPaint() for painting messages */

	RefreshTaskBar(&dc);
}

void CControlsDlg::RefreshTaskBar(CDC *pDC)
{
	int nImageCount;

	if(NULL == pDC)
		return;

	CRect rc;
	GetClientRect(&rc);
	pDC->FillSolidRect(&rc, ::GetSysColor(COLOR_3DFACE));

	if(NULL == CustomerImageList.m_hImageList || !(nImageCount = CustomerImageList.GetImageCount()))
		return;

	int i = 0;

	map<unsigned long,class Customer *>::iterator itCustomers;
	for (itCustomers = m_mapCustomers.begin(); itCustomers != m_mapCustomers.end(); ++itCustomers)
	{
		Customer *pCustomer = (*itCustomers).second;
		if(!pCustomer || pCustomer->m_cAppBar != m_ImageType) 
			continue;
		
		int CustomerImage = -1;		/** find image of customer */
		map<unsigned long, PlutoCustomer*>::iterator it = mapPlutoImageList.find(pCustomer->m_iPlutoId);
		if( it != mapPlutoImageList.end() )
		{
			/** image was found. */
			CustomerImage = mapPlutoImageList[pCustomer->m_iPlutoId]->ImageNumber;

			DrawItem(pDC, i, CustomerImage, pCustomer->m_sNickname);

			i++;
		}
	}
}

void CControlsDlg::DrawItem(CDC *pDC, int Index, int CustomerImageIndex, string sNickName)
{
	CRect rc, TaskPosRect;
	GetClientRect(&rc);
	GetWindowRect(&TaskPosRect);

	int Offset = rc.Width() + POINT_OFFSET;
	if(m_Side == ABE_LEFT || m_Side == ABE_RIGHT)
		Offset = rc.Width() + POINT_OFFSET;
	else
		Offset = rc.Height() + POINT_OFFSET;

	int OffsetX = TaskPosRect.left < 0 ? TaskPosRect.left : 0;		
	int OffsetY = TaskPosRect.top < 0 ? TaskPosRect.top : 0;		

	CPoint Pos;
	CSize Size;

	if(m_Side == ABE_LEFT || m_Side == ABE_RIGHT)
	{
		Pos.x = OffsetX + SPACE_FROM_BORDER;
		Pos.y = OffsetY + SPACE_FROM_BORDER + Index * Offset;
		Size.cx = rc.Width() - 2 * SPACE_FROM_BORDER;
		Size.cy = rc.Width() - 2 * SPACE_FROM_BORDER;
	}
	else
	{
		Pos.x = OffsetX + SPACE_FROM_BORDER + Index * Offset;
		Pos.y = OffsetY + SPACE_FROM_BORDER;
		Size.cx = rc.Height() - 2 * SPACE_FROM_BORDER - POINT_OFFSET;
		Size.cy = rc.Height() - 2 * SPACE_FROM_BORDER - POINT_OFFSET;
	}

	int cx, cy;

	CustomerImageList.DrawIndirect(
		pDC, 
		CustomerImageIndex, 
		Pos, 
		Size, 
		CPoint(0, 0),
		ILD_SCALE 
	);

	if(m_Side == ABE_LEFT || m_Side == ABE_RIGHT)
	{
		RECT rect = 
		{ 
			OffsetX + SPACE_FROM_BORDER, 
			OffsetY + SPACE_FROM_BORDER + Index * Offset + rc.Width() - SPACE_FROM_BORDER, 
			OffsetX + rc.Width() - SPACE_FROM_BORDER, 
			OffsetY + SPACE_FROM_BORDER + Index * Offset + rc.Width() - SPACE_FROM_BORDER + 25
		};

		pDC->DrawText(sNickName.c_str(), &rect, 0);
	}
	else
	{
		RECT rect = 
		{ 
			OffsetX + SPACE_FROM_BORDER + Index * Offset, 
			OffsetY + rc.Height() - 15, 
			OffsetX - SPACE_FROM_BORDER + Index * Offset + rc.Height(), 
			OffsetY + rc.Height()
		};

		pDC->DrawText(sNickName.c_str(), &rect, 0);
	}
}

Customer* CControlsDlg::SelectedCustomer(CPoint point)
{
	CRect rc;
	GetClientRect(&rc);

	Customer *pCustomer = NULL;
	Customer *pSelectedCustomer = NULL;

	int i = 0;
	int Index = -1;
	int Offset = 1;

	if(m_Side == ABE_LEFT || m_Side == ABE_RIGHT)
	{
		Offset = rc.Width() + POINT_OFFSET;

		_ASSERT(Offset > 0);
		Index = point.y / Offset;
	}
	else
	{
		Offset = rc.Height() + POINT_OFFSET;

		_ASSERT(Offset > 0);
		Index = point.x / Offset;
	}


	map<unsigned long,class Customer *>::iterator itCustomers;
	for (itCustomers = m_mapCustomers.begin(); itCustomers != m_mapCustomers.end(); ++itCustomers)
	{
		pCustomer = (*itCustomers).second;
		if(!pCustomer || pCustomer->m_cAppBar != m_ImageType) 
			continue;
		
		/** //int CustomerImage = -1;		//find image of customer */
		map<unsigned long, PlutoCustomer*>::iterator it = mapPlutoImageList.find(pCustomer->m_iPlutoId);
		if( it != mapPlutoImageList.end() )
		{
			/** image was found. */
			if(i == Index)
			{
				pSelectedCustomer = pCustomer;
				break;
			}

			i++;
		}
	}

	return pSelectedCustomer;
}

void CControlsDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	static bool bTimerActivated = false;

	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(tme);
	tme.hwndTrack = m_hWnd;
	tme.dwFlags = TME_LEAVE|TME_HOVER;
	tme.dwHoverTime = 1;
	_TrackMouseEvent(&tme);

	m_Mutex.Lock();

	Customer* pCustomer = SelectedCustomer(point); 

    if (
		  NULL != pCustomer			
		  && 
		  (
		    	NULL == HighlighDlg 
				||
				(
					NULL != HighlighDlg &&
					NULL == HighlighDlg->m_hWnd
				)
				||
				(
					NULL != HighlighDlg					&& 
					NULL != HighlighDlg->m_hWnd			&&
					NULL != HighlighDlg->GetCustomer()	&&
					HighlighDlg->GetCustomer()->m_iPlutoId != pCustomer->m_iPlutoId
				)
				||
				(
					NULL != HighlighDlg					&& 
					NULL != HighlighDlg->m_hWnd			&&
					!HighlighDlg->IsWindowVisible()
				)
    	   )
		)
	{
		bTimerActivated = false;

		CPoint ItemPoint = point;
		CRect rc;
		GetClientRect(&rc);

		if(m_Side == ABE_LEFT || m_Side == ABE_RIGHT)
		{
			int Offset = rc.Width() + POINT_OFFSET;

			_ASSERT(Offset > 0);
			ItemPoint.y = point.y / Offset * Offset;			
		}
		else
		{
			int Offset = rc.Height() + POINT_OFFSET;

			_ASSERT(Offset > 0);
			ItemPoint.x = point.x / Offset * Offset;			
		}

		ClientToScreen(&ItemPoint);
		
		CRect rect;
		GetClientRect(&rect);
		
		Position pos;
		pos.iDirection = m_Side;		/** side: ABE_LEFT, ABE_TOP, ABE_RIGHT, ABE_BOTTOM */
		pos.pPosition = ItemPoint;		/** upper left corner of selected item */
		pos.WidthHeight.x = rect.Width();	/** width of CControlDlg */
		pos.WidthHeight.y = rect.Height();	/** height of CControlDlg */
		
		if(NULL == HighlighDlg)
		{
			HighlighDlg = new CHighlightedCustomer(NULL);
			HighlighDlg->Create(IDD_HIGHLIGHT, this);

			ASSERT(NULL != HighlighDlg);
		}

		HighlighDlg->SetCustomer(pCustomer);
		HighlighDlg->SetPositionInfo(pos);
		HighlighDlg->CommitChanges();

		TRACE("Here I am!");

		HighlighDlg->ShowWindow(SW_SHOW);
	}

	if(
		NULL == pCustomer			&& 
		!bTimerActivated			&&
		NULL != HighlighDlg			
	)
	{
		ASSERT(NULL != HighlighDlg);

		if(NULL != HighlighDlg->m_hWnd)
		{
			HighlighDlg->KillMe(500);
			bTimerActivated = true;
		}
	}

	m_Mutex.Unlock();

	CAppBar::OnMouseMove(nFlags, point);
}

LRESULT CControlsDlg::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
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

	m_Mutex.Lock();

	if(NULL != HighlighDlg)
		if(NULL != HighlighDlg->m_hWnd)		{
			TRACE("You're out!\n");
			HighlighDlg->KillMe(500);
		}

	m_Mutex.Unlock();

	return 0;
}

LRESULT CControlsDlg::OnMouseHover(WPARAM wparam, LPARAM lparam) 
{
	return S_OK;
}


void CControlsDlg::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CAppBar::OnWindowPosChanged(lpwndpos);

}

void CControlsDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CAppBar::OnActivate(nState, pWndOther, bMinimized);

	RefreshTaskBar(GetDC());
}
