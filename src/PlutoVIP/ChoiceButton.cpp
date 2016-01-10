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
/******************************************************************************/
/**
 * @file ChoiceButton.cpp
 *
 * @brief Implementation of CChoiceButton
 *
 * @version 1.2
 *
 * @date 02.06.2004
 *
 * see ChoiceButton.h
 *
 ******************************************************************************/

// ChoiceButton.cpp : implementation file
//
// Version 1.1
//	added DefautlButton Support
//	BM_SETSTATE for pushed state

#include "stdafx.h"
//#include "ExplorerButton.h"
#include "ChoiceButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChoiceButton

CChoiceButton::CChoiceButton(): m_rcMainRect(0,0,0,0), m_rcMenuRect(0,0,0,0)
{
	m_bPushed  = FALSE;
	m_bMenuEnabled = FALSE;
	m_bMenuPushed = FALSE;
	m_bImageLoaded = FALSE;
	m_nMenuChoice = (UINT)-1;
	m_bDefault = FALSE;
	m_bFocus = FALSE;

	m_nDropWidth		= CHOICE_DROP_WIDTH;
	m_nMenuButtonWidth	= CHOICE_MENU_WIDTH;
	m_nBorderSpacing	= CHOICE_BORDER_SPACING;
	m_nPicTextSpacing	= CHOICE_PICTEXT_SPACING;

	m_menuChoice.CreatePopupMenu();
}

CChoiceButton::~CChoiceButton()
{
	m_menuChoice.DestroyMenu();
}


BEGIN_MESSAGE_MAP(CChoiceButton, CButton)
	//{{AFX_MSG_MAP(CChoiceButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_GETDLGCODE()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_KEYUP()
	ON_WM_SYSKEYDOWN()
	//}}AFX_MSG_MAP
	ON_MESSAGE(BM_SETSTYLE, OnSetStyle)
	ON_MESSAGE(BM_SETSTATE, OnSetState)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChoiceButton message handlers

/******************************************************************************/
/**
* @brief Creates the Button.
*
* Because OwnerDrawn buttons cannot be default buttons, we hav to capture the BS_DEFPUSHBUTTON
* flag and save it in a internal flag. the remove it and set the BS_OWNERDRAW flag.
* so if you want to initially set this button as default don't set the OWNERDRAW flag your self
*
* several versions of Create follow
*
* @param varname: [in, out]  
*
* @return 
*
* @sa 
*
******************************************************************************/
BOOL CChoiceButton::Create(LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nCmdID)
{
	// TODO: Add your specialized code here and/or call the base class
	if( dwStyle & BS_DEFPUSHBUTTON )
	{
		m_bDefault = TRUE;
		dwStyle &= ~BS_DEFPUSHBUTTON;
		dwStyle |= BS_PUSHBUTTON;
	}

	dwStyle |= BS_OWNERDRAW;
	m_rcMainRect = rect;
		
	//return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
	return CButton::Create(lpszCaption, dwStyle, rect, pParentWnd, nCmdID);
}

BOOL CChoiceButton::Create(UINT nStrID, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nCmdID)
{
	CString strTmp;
	strTmp.LoadString(nStrID);
	return Create(strTmp, dwStyle, rect, pParentWnd, nCmdID);
}

BOOL CChoiceButton::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nCmdID)
{
	return Create((LPCTSTR)NULL, dwStyle, rect, pParentWnd, nCmdID);
}


/******************************************************************************/
/**
* @brief same as Create, only that this is called when you make a control variable of a button resource with class wizard.
*
* Description
*
* @param varname: [in, out]  
*
* @return 
*
* @sa 
*
******************************************************************************/
void CChoiceButton::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	if( GetStyle() & BS_DEFPUSHBUTTON )
	{
		m_bDefault = TRUE;
		ModifyStyle(BS_DEFPUSHBUTTON, BS_PUSHBUTTON);
	}

	GetWindowRect(&m_rcMainRect);
	ModifyStyle(0, BS_OWNERDRAW);

	CButton::PreSubclassWindow();
}


/******************************************************************************/
/**
* @brief actually draws the button, with bitmap, dropdown button and so on
*
* handles default states
*
* @param varname: [in, out]  
*
* @return 
*
* @sa 
*
******************************************************************************/
void CChoiceButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your code to draw the specified item
	if( lpDrawItemStruct->CtlType != ODT_BUTTON )
		return;

	CDC* dcDraw = CDC::FromHandle(lpDrawItemStruct->hDC);

	//get window text
	CString strCaption;
	GetWindowText(strCaption);

	//get the desired stlye of the button
	DWORD nStyle = GetStyle();
	UINT nState = DFCS_BUTTONPUSH;
	//ensure the FLAT Buttonstyle is considered
	if( nStyle & BS_FLAT ) nState |= DFCS_FLAT;

	//get rect of the button
	//GetWindowRect(&m_rcMainRect);
	m_rcMainRect = lpDrawItemStruct->rcItem;

	if( m_bDefault )
		m_rcMainRect.DeflateRect(1,1);

	//if menu is enabled, draw it
	if( m_bMenuEnabled )
	{
		DWORD nMenuState = nState;

		m_rcMenuRect = m_rcMainRect;
		m_rcMainRect.right -= m_nMenuButtonWidth;
		m_rcMenuRect.left   = m_rcMainRect.right;

		CPoint ptArrow(m_rcMenuRect.CenterPoint());
		//if menu button is pushed, draw it as pushed & offset the arrow point
		if( m_bMenuPushed )
		{
			/*
			if( nState & DFCS_FLAT )
				nMenuState |= DFCS_MONO;
			else
			*/
				nMenuState |= DFCS_PUSHED;
			ptArrow.Offset(m_nDropWidth,m_nDropWidth);
		}

		dcDraw->DrawFrameControl(&m_rcMenuRect, DFC_BUTTON, nMenuState);
		
		if( lpDrawItemStruct->itemState & ODS_DISABLED )
		{
			DrawArrow(dcDraw,ptArrow + CPoint(1,1), RGB(255,255,255));
			DrawArrow(dcDraw,ptArrow, GetSysColor(COLOR_GRAYTEXT));
		}
		else
		{
			DrawArrow(dcDraw,ptArrow, 0);
		}
	}

	//rectangle for drawing the text and drawing the pic
	CRect rcText(m_rcMainRect);
	//deflate text border by m_nBorderSpacing
	rcText.DeflateRect(m_nBorderSpacing,m_nBorderSpacing);

	//if main button is pushed, offset the text rect
	//and set pushed flag for border drawing
	if( m_bPushed )
	{
		//rcText.DeflateRect(m_nDropWidth,m_nDropWidth,0,0);
		rcText.OffsetRect(m_nDropWidth,m_nDropWidth);
		/*
		if( nState & DFCS_FLAT )
			nState |= DFCS_MONO;
		else
		*/
			nState |= DFCS_PUSHED;
	}

	//draw main button border
	dcDraw->DrawFrameControl(&m_rcMainRect, DFC_BUTTON, nState);

	if( m_bImageLoaded )
	{
		CPoint ptPic;
		ptPic.x = rcText.left;
		ptPic.y = rcText.CenterPoint().y - m_bmpInfo.bmHeight/2;
		/*
		switch(nStyle)
		{
			case BS_TOP:	ptPic.y = rcText.top; break;
			case BS_BOTTOM:	ptPic.y = rcText.bottom - m_bmpInfo.bmHeight; break;
			default:
			case BS_VCENTER:ptPic.y = rcText.CenterPoint().y - m_bmpInfo.bmHeight/2; break;
		};
		*/

		if( lpDrawItemStruct->itemState & ODS_DISABLED )
			dcDraw->DrawState(ptPic,CSize(m_bmpInfo.bmWidth, m_bmpInfo.bmHeight),m_bmpImageDisabled, DST_BITMAP|DSS_DISABLED);
		else
			dcDraw->DrawState(ptPic,CSize(m_bmpInfo.bmWidth, m_bmpInfo.bmHeight),m_bmpImage, DST_BITMAP|DSS_NORMAL);
		
		//offset left border of Text rect with m_nPicTextSpacing and bitmap width
		rcText.left += m_bmpInfo.bmWidth + m_nPicTextSpacing;
	}

	//set text alignment
	UINT uTextAlign = DT_SINGLELINE|DT_CENTER|DT_VCENTER;
	/*
	switch(nStyle & 0xf00)
	{
		case BS_TOP: uTextAlign |= DT_TOP; break;
		case BS_BOTTOM: uTextAlign |= DT_BOTTOM; break;
		default: uTextAlign |= DT_VCENTER;
	};
	switch(nStyle & 0xf00)
	{
		case BS_LEFT: uTextAlign |= DT_LEFT; break;
		case BS_RIGHT: uTextAlign |= DT_RIGHT; break;
		default: uTextAlign |= DT_CENTER;
	};
	if( nStyle & BS_MULTILINE )
		uTextAlign |= DT_WORDBREAK;
	else
		uTextAlign |= DT_SINGLELINE;
	*/

	//draw text
    dcDraw->SetBkMode(TRANSPARENT);
    dcDraw->SetBkColor(GetSysColor(COLOR_BTNFACE));
	if( lpDrawItemStruct->itemState & ODS_DISABLED )
	{
		rcText.OffsetRect(1,1);
		dcDraw->SetTextColor(RGB(255,255,255));
		dcDraw->DrawText(strCaption, &rcText, uTextAlign);
		rcText.OffsetRect(-1,-1);
		dcDraw->SetTextColor(GetSysColor(COLOR_GRAYTEXT));
		dcDraw->DrawText(strCaption, &rcText, uTextAlign);
	}
	else
	{
		dcDraw->SetTextColor(GetSysColor(COLOR_BTNTEXT));
		dcDraw->DrawText(strCaption, &rcText, uTextAlign);
	}

	//draw focus rect around txt
	if( m_bFocus )
		dcDraw->DrawFocusRect(rcText);

	//if default button
	//draw default button border (black rect)
	if( m_bDefault )
	{
		dcDraw->FrameRect(&lpDrawItemStruct->rcItem, CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH)));
		if( m_bPushed )
		{
			CRect rcPush(lpDrawItemStruct->rcItem);
			rcPush.DeflateRect(1,1);
			dcDraw->FrameRect(&rcPush, CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH)));
		}
	}
}


/******************************************************************************/
/**
* @brief draws the little arrow on the dropdown button, call from within DrawItem
*
* @param varname: [in, out]  
*
* @return 
*
* @sa 
*
******************************************************************************/
void CChoiceButton::DrawArrow(CDC *pDC, CPoint ptArrow, COLORREF crCol)
{
	CPen penTmp(PS_SOLID,1,crCol);
	CPen* pOldPen = pDC->SelectObject(&penTmp);

	CPoint ptDest;
	ptArrow += CPoint(-1,2);

	pDC->SetPixel(ptArrow, crCol);


	ptArrow -= CPoint(1,1);
	pDC->MoveTo(ptArrow);

	ptDest = ptArrow;
	ptDest += CPoint(3,0);
	pDC->LineTo(ptDest);

	ptArrow -= CPoint(1,1);
	pDC->MoveTo(ptArrow);

	ptDest = ptArrow;
	ptDest += CPoint(5,0);
	pDC->LineTo(ptDest);

	ptArrow -= CPoint(1,1);
	pDC->MoveTo(ptArrow);

	ptDest = ptArrow;
	ptDest += CPoint(7,0);
	pDC->LineTo(ptDest);
	
	pDC->SelectObject(pOldPen);
}


/******************************************************************************/
/**
* @brief handler for the LeftMousebuttonDown event
*
* to ensure the default behavior on click on the main button, only call the default
* cbutton handler
*
* on click on the dropdown button, track the popupmenu, get the choice and
* send a BN_CLICKED message to parent window
*
* @param varname: [in, out]  
*
* @return 
*
* @sa 
*
******************************************************************************/
void CChoiceButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if( m_rcMainRect.PtInRect(point) )
	{
		CButton::OnLButtonDown(nFlags, point);
	}
	else if( m_bMenuEnabled && m_rcMenuRect.PtInRect(point) )
	{
		BOOL bOldMenu = m_bMenuPushed;
		m_bMenuPushed = TRUE;
		CPoint ptMenu;
		ptMenu.x = m_rcMenuRect.left;
		ptMenu.y = m_rcMenuRect.bottom;
		ClientToScreen(&ptMenu);
		RedrawWindow(); //draw button with pushed menu button
		//draw popup menu and get users selection
		UINT nChoice = (UINT)m_menuChoice.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_NONOTIFY|TPM_RETURNCMD,ptMenu.x,ptMenu.y,this);
		if( nChoice != 0 )
		{
			SetChoice(nChoice & (~CHOICE_MENUID));
			GetParent()->PostMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM) GetSafeHwnd());
			m_bMenuPushed = FALSE;
			m_bPushed = FALSE;
		}
		else
		{
			m_bMenuPushed = bOldMenu;
		}
		
		Invalidate();
	}
}


/******************************************************************************/
/**
* @brief handler for RightMousebuttonDown event
*
* if the m_bPushed flag is set, send a BN_CLICKED msg to parent
* then send message to return to unpushed state
* and call default handler, to quit button click handling
*
* @param varname: [in, out]  
*
* @return 
*
* @sa 
*
******************************************************************************/
void CChoiceButton::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if( m_bPushed )
		GetParent()->PostMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM) GetSafeHwnd());
	SendMessage(BM_SETSTATE, FALSE, 0);
	CButton::OnLButtonUp(nFlags,point);
}


/******************************************************************************/
/**
* @brief if a doubleclick occurs (fast mouse clicks), make the normal LeftMouseButtonDown handling
*
* @param varname: [in, out]  
*
* @return 
*
* @sa 
*
******************************************************************************/
void CChoiceButton::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	OnLButtonDown(nFlags,point);
}


/******************************************************************************/
/**
* @brief handler for WM_SETSTATE, wParam==true: button is pushed, ==false: button is not pushed
*
* just set the push flags and invalidate the buttonrect. rest is done by DrawItem
* to prevent flickering, only invalidate the region if wParam != m_bPushed
*
* @param varname: [in, out]  
*
* @return 
*
* @sa 
*
******************************************************************************/
void CChoiceButton::OnSetState(WPARAM wParam, LPARAM lParam)
{
	if( (BOOL)wParam != m_bPushed )
	{
		m_bPushed = m_bMenuPushed = wParam;
		Invalidate();
	}
}


/******************************************************************************/
/**
* @brief sets the button image
*
* in fact sets 2 images, normal & disabled state.
* maps the provided transparent color crTrans, to the buttons face color
* for the disabled image, just map crTrans to White, CDC::DrawState does the rest
*
* @param UINT nResID: [in]  resource id of the bitmap
* @param COLORREF crTrans: [in] the color, that is the transparent color
* @param bool bResize: [in] if true, the buttons height is set to fit the image, default: true
*
* @return true, if bitmap could be loaded
*
* @sa 
*
******************************************************************************/
BOOL CChoiceButton::SetImage(UINT nResID, COLORREF crTrans, bool bResize)
{
	COLORMAP mapColor;
	mapColor.from = crTrans;
	mapColor.to  = GetSysColor(COLOR_BTNFACE);

	m_bmpImage.DeleteObject();
	if( !m_bmpImage.LoadMappedBitmap(nResID,0,&mapColor,1) )
	{
		m_bImageLoaded = false;
		return FALSE;
	}
		
	m_bmpImage.GetObject(sizeof(m_bmpInfo), &m_bmpInfo);

	mapColor.to = RGB(255,255,255);

	m_bmpImageDisabled.DeleteObject();
	if( !m_bmpImageDisabled.LoadMappedBitmap(nResID,0,&mapColor, 1) )
	{
		m_bImageLoaded = false;
		return FALSE;
	}

	m_bImageLoaded = true;

	if( bResize )
	{
		WINDOWPLACEMENT wp;
		if(GetWindowPlacement(&wp))
		{
			wp.rcNormalPosition.bottom = wp.rcNormalPosition.top + m_bmpInfo.bmHeight + m_nBorderSpacing + m_nBorderSpacing;
			SetWindowPlacement(&wp);
		}
	}

	return TRUE;
}


/******************************************************************************/
/**
* @brief adds a label to the menu, the first added item is checked&buttontext by default
*
* @param LPCTSTR lpszNewItem: [in] label to add
*
* @return the choice number of the added label, can be compared/used in SetChoice/GetChoice
*
* @sa 
*
******************************************************************************/
int CChoiceButton::AddMenuItem(LPCTSTR lpszNewItem)
{
	int iRetVal = m_menuChoice.GetMenuItemCount();
	UINT nFlags = MF_STRING;
	if( iRetVal == 0 )
	{
		nFlags |= MF_CHECKED;
		SetWindowText(lpszNewItem);
		m_bMenuEnabled = TRUE;
		m_nMenuChoice = 0;
	}
	else
		nFlags |= MF_UNCHECKED;

	if( !m_menuChoice.AppendMenu(nFlags, CHOICE_MENUID + iRetVal, lpszNewItem) )
		return -1;
	
	return iRetVal;
}


/******************************************************************************/
/**
* @brief deletes a menu item
*
* Description
*
* @param UINT nItem: [in] the return value of AddMenuItem of the specific item
*
* @return true on succes
*
* @sa 
*
******************************************************************************/
BOOL CChoiceButton::DeleteMenuItem(UINT nItem)
{
	if( !m_bMenuEnabled )
		return FALSE;

	return m_menuChoice.DeleteMenu(nItem, MF_BYPOSITION);
}


/******************************************************************************/
/**
* @brief adds a seperator to the menu
*
* Description
*
* @param none  
*
* @return void
*
* @sa 
*
******************************************************************************/
void CChoiceButton::AddMenuSeparator()
{
	if( !m_bMenuEnabled )
		return;

	m_menuChoice.AppendMenu(MF_SEPARATOR, 0, "");
}


/******************************************************************************/
/**
* @brief enables/disables(grays) a menu item
*
* @param UINT nItem: [in]  
* @param bool bEnable: [in]  
*
* @return previous state of the item
*
* @sa 
*
******************************************************************************/
bool CChoiceButton::EnableMenuItem(UINT nItem, bool bEnable)
{
	if( !m_bMenuEnabled )
		return false;

	UINT uState = MF_BYPOSITION;
	if( bEnable )
		uState |= MF_ENABLED;
	else
		uState |= MF_GRAYED;
	return (m_menuChoice.EnableMenuItem(nItem, uState) == MF_ENABLED) ? true : false;
}


/******************************************************************************/
/**
* @brief gets the checked menu item
*
* @param none
*
* @return choice number
*
* @sa 
*
******************************************************************************/
UINT CChoiceButton::GetChoice()
{
	return m_nMenuChoice;
}


/******************************************************************************/
/**
* @brief sets the specified menu item as the chosen one. (button text)
*
* @param UINT nMenuItem: [in]  
*
* @return void
*
* @sa 
*
******************************************************************************/
void CChoiceButton::SetChoice(UINT nMenuItem)
{
	if( !m_bMenuEnabled )
		return;

	CString strCap;
	if( m_menuChoice.GetMenuString(nMenuItem, strCap, MF_BYPOSITION) )
	{
		SetWindowText(strCap);
		m_menuChoice.CheckMenuItem(m_nMenuChoice, MF_BYPOSITION|MF_UNCHECKED);
		m_menuChoice.CheckMenuItem(nMenuItem, MF_BYPOSITION|MF_CHECKED);
		m_nMenuChoice = nMenuItem;
	}
}


/******************************************************************************/
/**
* @brief sets the push state (pushed/unpushed) of the button
*
* @param bool bPushed: [in]  
*
* @return previous pushstate
*
* @sa 
*
******************************************************************************/
bool CChoiceButton::SetPush(bool bPushed)
{
	bool bRetVal = m_bPushed ? true : false;
	SendMessage(BM_SETSTATE, bPushed ? TRUE : FALSE, 0);
	return bRetVal;
}


/******************************************************************************/
/**
* @brief returns the actual pushstate of the button
*
* @param none
*
* @return push state
*
* @sa 
*
******************************************************************************/
bool CChoiceButton::IsPushed()
{
	return m_bPushed ? true : false;
}


/******************************************************************************/
/**
* @brief sets the amount of pixels, the button contents (text, image...) should drop down on push, default: 1
*
* @param UINT nDropWidth: [in] dropwidth in pixels
*
* @return void
*
* @sa 
*
******************************************************************************/
void CChoiceButton::SetDropWidth(UINT nDropWidth)
{
	m_nDropWidth = nDropWidth;
	Invalidate();
}


/******************************************************************************/
/**
* @brief sets the width of the menu button, default: 20
*
* @param UINT nWidth: [in]  in pixels
*
* @return void
*
* @sa 
*
******************************************************************************/
void CChoiceButton::SetMenuButtonWidth(UINT nWidth)
{
	//7 is the width of the menu buttons arrow
	if( nWidth < (7+m_nBorderSpacing*2) )
		m_nMenuButtonWidth = 7+m_nBorderSpacing*2;
	else
		m_nMenuButtonWidth = nWidth;
	Invalidate();
}


/******************************************************************************/
/**
* @brief sets the spacing of the image/text from the buttons border, default: 4
*
* @param UINT nSpacing: [in]  in pixels
*
* @return 
*
* @sa 
*
******************************************************************************/
void CChoiceButton::SetBorderSpacing(UINT nSpacing)
{
	m_nBorderSpacing = nSpacing;
	Invalidate();
}


/******************************************************************************/
/**
* @brief sets the spacing between the right border of the image and the text rect, default: 4
*
* @param UINT nSpacing: [in]  in pixels
*
* @return 
*
* @sa 
*
******************************************************************************/
void CChoiceButton::SetPicTextSpacing(UINT nSpacing)
{
	m_nPicTextSpacing = nSpacing;
	Invalidate();
}

/////////////////////////////////////////////////////////////////////////////
// for handling of DefaultButton state
UINT CChoiceButton::OnGetDlgCode() 
{
	// TODO: Add your message handler code here and/or call default
    UINT nCode = CButton::OnGetDlgCode();
    nCode |= (m_bDefault ? DLGC_DEFPUSHBUTTON : DLGC_UNDEFPUSHBUTTON);
      return nCode;
}

LRESULT CChoiceButton::OnSetStyle(WPARAM wParam, LPARAM lParam)
{
	m_bDefault = wParam & BS_DEFPUSHBUTTON;
    // can't change control type after owner-draw is set.
    // let the system process changes to other style bits
    // and redrawing, while keeping owner-draw style
    return DefWindowProc(BM_SETSTYLE,
        (wParam & ~SS_TYPEMASK) | BS_OWNERDRAW, lParam);
}
/////////////////////////////////////////////////////////////////////////////

/******************************************************************************/
/**
* @brief resets the internal focus flag, handler for WM_KILLFOCUS, redraws button
*
* @param varname: [in, out]  
*
* @return void
*
* @sa 
*
******************************************************************************/
void CChoiceButton::OnKillFocus(CWnd* pNewWnd) 
{
	CButton::OnKillFocus(pNewWnd);
	
	// TODO: Add your message handler code here
	m_bFocus = FALSE;
	Invalidate();
}


/******************************************************************************/
/**
* @brief sets the internal focus flag, handler for WM_SETFOCUS, redraws button
*
* @param varname: [in, out]  
*
* @return 
*
* @sa 
*
******************************************************************************/
void CChoiceButton::OnSetFocus(CWnd* pOldWnd) 
{
	CButton::OnSetFocus(pOldWnd);
	
	// TODO: Add your message handler code here
	m_bFocus = TRUE;
	Invalidate();
}


/******************************************************************************/
/**
* @brief returns the SIZE of the button
*
* @param varname: [in, out]  
*
* @return 
*
* @sa 
*
******************************************************************************/
SIZE CChoiceButton::GetSize()
{
	CRect rcClient;
	GetClientRect(&rcClient);
	return rcClient.Size();
}


/******************************************************************************/
/**
* @brief Version 1.0, resizes the width of the button to fit the largest string
*
* with use of the currently set font, determines the width in pixels of the drawn
* largest string (searches the largest string if menu is enabled)
* and then adds spacings, width of image&menu button.
* and adds a user defined spacing, default: 30
*
* @param UINT uSpacing: [in]  additional spacing in pixels, default: 30
*
* @return void
*
* @sa 
*
******************************************************************************/
void CChoiceButton::SizeToContents(UINT uSpacing)
{
	CDC* pDC = GetDC();
	if( !pDC ) return;

	//select set font
	pDC->SelectObject(GetFont());

	UINT uWidth = 0;
	CString strTmp;

	if( m_bMenuEnabled )
	{
		for(UINT i=0; i<m_menuChoice.GetMenuItemCount(); i++)
		{
			UINT uTmp = 0;
			if( m_menuChoice.GetMenuString(i, strTmp, MF_BYPOSITION) )
			{
				uTmp = pDC->GetOutputTextExtent(strTmp).cx; //gets width of the drawn string
				if( uWidth < uTmp )
					uWidth = uTmp;
			}
		}
	}
	else
	{
		GetWindowText(strTmp);
		uWidth = pDC->GetOutputTextExtent(strTmp).cx;
	}

	//adds border spacing (twice (left and right side))
	uWidth += 2*m_nBorderSpacing + uSpacing;

	if( m_bImageLoaded )
	{
		uWidth += m_nPicTextSpacing;
		uWidth += m_bmpInfo.bmWidth;
	}

	if( m_bMenuEnabled )
		uWidth += m_nMenuButtonWidth;

	SetWindowPos(NULL, 0,0, uWidth, GetSize().cy, SWP_NOMOVE|SWP_NOZORDER);

}


/******************************************************************************/
/**
* @brief ensures default handling of pressing space upon a button
*
* @param varname: [in, out]  
*
* @return 
*
* @sa 
*
******************************************************************************/
void CChoiceButton::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	if( nChar == VK_SPACE )
		OnLButtonUp(MK_LBUTTON, m_rcMainRect.CenterPoint());
	else	
		CButton::OnKeyUp(nChar, nRepCnt, nFlags);
}


/******************************************************************************/
/**
* @brief ensures default handling on pressing ALT+DOWNKEY
*
* shows menu on ALT+DOWNKEY, actual simulates click on menu button
*
* @param varname: [in, out]  
*
* @return 
*
* @sa 
*
******************************************************************************/
void CChoiceButton::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	if( nChar == VK_DOWN )
		if( m_bMenuEnabled ) OnLButtonDown(MK_LBUTTON, m_rcMenuRect.CenterPoint());
	else
		CButton::OnSysKeyDown(nChar, nRepCnt, nFlags);
}
