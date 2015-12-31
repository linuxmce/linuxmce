/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
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
#include "LRMenu.h"
#include "../../pluto_main/Define_Button.h"
#include "PlutoUtils/PlutoDefs.h"
#include "VIPShared/MenuData.h"
#include "Win32/DrawTextExUTF8.h"

//---------------------------------------------------------------------------------------------------------
#if defined(SMARTPHONE2005) || defined(_VC80_UPGRADE)		//--- CHANGED4WM5 ----//
	#define clock		GetTickCount
	#define Surface_GetWidth pSurface->GetWidth()
	#define Surface_GetHeight pSurface->GetHeight()
	#define pf_uint8_t PHAL::uint8_t
#else	
	#include "src/internal/graphicbuffer.h"
	#include "src/internal/raster2d.h"

	#define Surface_GetWidth pSurface->m_width
	#define Surface_GetHeight pSurface->m_height
	#define pf_uint8_t uint8_t
#endif
//---------------------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------------------
/*
 *
 *	class LRMenuItemData
 *
 */
//---------------------------------------------------------------------------------------------------------
LRMenuItemData::~LRMenuItemData( )
{
	PLUTO_SAFE_DELETE(m_pImage);
}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ void LRMenuItemData::SetImage( unsigned char ImageType,unsigned long ImageSize,const char *pImage )
{
	m_nImageType = ImageType;
	m_nImageSize = ImageSize;
	PLUTO_SAFE_DELETE(m_pImage);
	if ( NULL != pImage ) {
		m_pImage = new char[m_nImageSize];
		memcpy(m_pImage, pImage, m_nImageSize);
	}
}
//---------------------------------------------------------------------------------------------------------
void LRMenuItemData::SetCaptionToString( string sCaption )
{
	#if defined UNICODE || defined _UNICODE
		m_sCaption.resize( sCaption.length() );
		copy( sCaption.begin(), sCaption.end(), m_sCaption.begin() );
	#else
		m_sCaption = sCaption;
	#endif
}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ void LRMenuItemData::GetStringCaption( string& sCaption )
{
	#if defined UNICODE || defined _UNICODE
		sCaption.resize( m_sCaption.length() );
		copy( m_sCaption.begin(), m_sCaption.end(), sCaption.begin() );
	#else
		sCaption = m_sCaption;
	#endif
}

//---------------------------------------------------------------------------------------------------------
/*
 *
 *	class LocalRenderer
 *
 */
//---------------------------------------------------------------------------------------------------------

/*static*/ Rect LocalRenderer::m_rViewport;
/*static*/ Rect LocalRenderer::m_rAppViewport;
/*static*/ pluto_pthread_mutex_t LocalRenderer::m_RenderMutex( "RenderMenu" );
/*static*/ pthread_mutexattr_t LocalRenderer::m_MutexAttr;
/*static*/ bool LocalRenderer::m_bEnable = false;

/*static*/ void LocalRenderer::CheckMutex( void )
{
	if ( !m_RenderMutex.m_bInitialized ) {
		pthread_mutexattr_init( &m_MutexAttr );
		pthread_mutexattr_settype( &m_MutexAttr,  PTHREAD_MUTEX_RECURSIVE_NP );
		m_RenderMutex.Init( &m_MutexAttr );
	}
}


//---------------------------------------------------------------------------------------------------------

/*static*/ void LocalRenderer::DrawSmallText( LPCTSTR Text, Rect &r, COLORREF color, int iPointSize, BOOL bBold )
{
	if ( !m_bEnable ) return; 

	HDC hdc = GetDisplay()->GetBackBuffer()->GetDC(false);

 	::SetTextColor(hdc, color);
	::SetBkMode(hdc, TRANSPARENT);	

	RECT rectLocation = { r.left, r.top, r.right, r.bottom };

	tstring tsCaption = Text;
	string sCaption;
	#if defined(UNICODE) || defined(_UNICODE)		
		sCaption.resize( tsCaption.length() );		
		copy( tsCaption.begin(), tsCaption.end(), sCaption.begin() );
	#else
		sCaption = tsCaption;
	#endif
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = -(iPointSize  *  GetDeviceCaps(hdc,  LOGPIXELSY))  /  72;

	if ( bBold ) lf.lfWeight = FW_BOLD;
				
	::GetTextFace( hdc, LF_FACESIZE, lf.lfFaceName );
	HFONT hFont = ::CreateFontIndirect(&lf);
	if ( hFont ) {
		HFONT hOldFont = (HFONT) ::SelectObject(hdc, hFont);		
		DrawTextExUTF8(hdc, sCaption.c_str(), -1, &rectLocation, DT_WORDBREAK | DT_NOPREFIX | DT_VCENTER, NULL); 			
		::SelectObject(hdc, hOldFont);
		::DeleteObject(hFont);
	}				
		

	GetDisplay()->GetBackBuffer()->ReleaseDC(hdc);
}

void LocalRenderer::DrawText( LPCTSTR Text, Rect &r, COLORREF color, BOOL bBold )
{
	if ( !m_bEnable ) return; 

	RECT rectLocation = { r.left, r.top, r.right, r.bottom };

	tstring tsCaption = Text;
	string sCaption;
	#if defined(UNICODE) || defined(_UNICODE)		
		sCaption.resize( tsCaption.length() );		
		copy( tsCaption.begin(), tsCaption.end(), sCaption.begin() );
	#else
		sCaption = tsCaption;
	#endif

	RECT rectText=rectLocation;
	HDC hdc = GetDisplay()->GetBackBuffer()->GetDC(false);
	DrawTextExUTF8(hdc, sCaption.c_str(), -1, &rectText, DT_WORDBREAK | DT_NOPREFIX | DT_CALCRECT, NULL);
	GetDisplay()->GetBackBuffer()->ReleaseDC(hdc);

	Rect rt( rectText );
	Rect rl( rectLocation );
	if ( rt.GetHeight()>rl.GetHeight() ) {
		LocalRenderer::DrawSmallText( Text, r, color, DEFAULT_SMALLFONT_POINT_TEXT, bBold );
	}
	else {
		HDC hdc = GetDisplay()->GetBackBuffer()->GetDC(false);

 		::SetTextColor(hdc, color);
		::SetBkMode(hdc, TRANSPARENT);	
		if ( bBold ) { // if bold -> create new font from the current one
			LOGFONT lf;
			memset(&lf, 0, sizeof(LOGFONT));
			lf.lfWeight = FW_BOLD;
			::GetTextFace( hdc, LF_FACESIZE, lf.lfFaceName );
			HFONT hFont = ::CreateFontIndirect(&lf);
			if ( hFont ) {
				HFONT hOldFont = (HFONT) ::SelectObject(hdc, hFont);		
				DrawTextExUTF8(hdc, sCaption.c_str(), -1, &rectLocation, DT_WORDBREAK | DT_NOPREFIX | DT_VCENTER, NULL); 			
				::SelectObject(hdc, hOldFont);
				::DeleteObject(hFont);
			}				
			
		} 
		else DrawTextExUTF8(hdc, sCaption.c_str(), -1, &rectLocation, DT_WORDBREAK | DT_NOPREFIX | DT_VCENTER, NULL); 

		GetDisplay()->GetBackBuffer()->ReleaseDC(hdc);
	}
}
//---------------------------------------------------------------------------------------------------------
void LocalRenderer::DrawImage( char *pData,int nSize, Rect &r )
{
	if ( !m_bEnable ) return; 

	Surface *pSurface = LoadImage( GetDisplay(), (pf_uint8_t*)pData, (pf_uint8_t*)(pData + nSize));

	if(pSurface)
	{		
		if(Surface_GetWidth == r.GetWidth() && Surface_GetHeight == r.GetHeight() )
			GetDisplay()->Blit( r.left, r.top, pSurface );
		else //zoom
		{
			Rect dest(r);	

			double ZoomX = r.GetWidth() / double(Surface_GetWidth);
			double ZoomY = r.GetHeight() / double(Surface_GetHeight);

			dest.right = dest.left + int(Surface_GetWidth * ZoomX);
			dest.bottom = dest.top + int(Surface_GetHeight * ZoomY);     
        
			if( dest.right-dest.left>0 && dest.bottom-dest.top>0 ) 
				GetDisplay()->BlitStretch(dest, pSurface); 
		}

		delete pSurface;
	}
}
//---------------------------------------------------------------------------------------------------------
void LocalRenderer::FillRect( Rect &r, COLORREF color )
{
	if ( !m_bEnable ) return; 

	GetDisplay()->FillRect(r.left, r.top, r.right, r.bottom, Color( color ) );
}
//---------------------------------------------------------------------------------------------------------
void LocalRenderer::Update( Rect* r )
{
	if ( !m_bEnable ) return; 

	if ( r ) {
		GetDisplay()->Update( r );
	}
	else {
		GetDisplay()->Update( &m_rAppViewport );
	}
}

//---------------------------------------------------------------------------------------------------------
Surface* LocalRenderer::SaveRect( Rect r )
{
	if ( !m_bEnable ) return NULL; 

#if defined(_VC80_UPGRADE) // different implementation for PocketFrog versions
	Surface* copy = new Surface( r.GetWidth(), r.GetHeight() );
	if (copy){
        Rasterizer rasterizer( copy );
		rasterizer.Blit( 0, 0, GetDisplay()->GetBackBuffer(), &r );
    }
	return copy;
#else
	DisplayDevice *pDisplayDevice = GetDisplay();
	Surface *pSurface= pDisplayDevice->CreateSurface( r.GetWidth(), r.GetHeight() );
	Rasterizer* pRasterizer = pDisplayDevice->CreateRasterizer(pSurface);
	pRasterizer->Blit(0, 0, GetDisplay()->GetBackBuffer(), &r);
	delete pRasterizer;	
	return pSurface;
#endif
}
//---------------------------------------------------------------------------------------------------------
void LocalRenderer::RestoreRect( int iX, int iY, Surface* pSurface, Rect* srcRect )
{
	if ( !m_bEnable ) return; 

#if defined(_VC80_UPGRADE) // different implementation for PocketFrog versions
	Rasterizer rasterizer( GetDisplay()->GetBackBuffer() );
	rasterizer.Blit( iX, iY, pSurface, srcRect );	
#else	
	DisplayDevice *pDisplayDevice = GetDisplay();
	Surface *pLocalSurface = pDisplayDevice->GetBackBuffer();
	Rasterizer* pRasterizer =  pDisplayDevice->CreateRasterizer( pLocalSurface );			
	pRasterizer->Blit( iX, iY, pSurface, srcRect );
	delete pRasterizer;
#endif
}
//---------------------------------------------------------------------------------------------------------
void LocalRenderer::DrawRect( RECT r, COLORREF color )
{
	if ( !m_bEnable ) return; 

	Pixel cc(color);
	Rect rr(r);
	GetDisplay()->DrawRect( rr.left, rr.top, rr.right, rr.bottom, cc );
}
//---------------------------------------------------------------------------------------------------------
void LocalRenderer::DrawHRect( RECT r, COLORREF color )
{
	if ( !m_bEnable ) return; 

	Pixel cc(color);
	Rect rr(r);
	GetDisplay()->DrawHLine( rr.left, rr.top, rr.GetWidth(), cc );
	GetDisplay()->DrawHLine( rr.left, rr.bottom-1, rr.GetWidth(), cc );
	GetDisplay()->DrawVLine( rr.left, rr.top, rr.GetHeight()-1, cc );
	GetDisplay()->DrawVLine( rr.right-1, rr.top, rr.GetHeight()-1, cc );
}
//---------------------------------------------------------------------------------------------------------
void LocalRenderer::FillTriangle( int iX1, int iY1, int iX2, int iY2, int iX3, int iY3, COLORREF color )
{
	if ( !m_bEnable ) return; 

	Point points[3];
	points[0] = Point( iX1, iY1 );
	points[1] = Point( iX2, iY2 );
	points[2] = Point( iX3, iY3 );
	GetDisplay()->FillPoly( points, 3, Color(color) );
}
//---------------------------------------------------------------------------------------------------------
void LocalRenderer::DrawHLine( int iX, int iY, int iW, COLORREF color )
{
	if ( !m_bEnable ) return; 

	Pixel cc(color);
	GetDisplay()->DrawHLine( iX, iY, iW, cc );
}


//---------------------------------------------------------------------------------------------------------
/*
 *
 *	class LRMenu
 *
 */
//---------------------------------------------------------------------------------------------------------
LRMenu::LRMenu()
{
	CreateMenuRoot();
}
//---------------------------------------------------------------------------------------------------------
LRMenu::~LRMenu()
{
	PLUTO_SAFE_DELETE( m_pMenuRoot );
	m_pMenuRoot = NULL;
}
//---------------------------------------------------------------------------------------------------------
LRMenuItem* LRMenu::AddItem( LRMenuItemData& ItemData )
{	
	if ( !m_bChanging ) return NULL;

	LRMenuItem* pItem = m_pMenuRoot->AddItem( ItemData );
	return pItem;
}
//---------------------------------------------------------------------------------------------------------
void LRMenu::AddItem( LRMenuItem* pMenuItem )
{
	if ( !m_bChanging ) return;

	if ( !pMenuItem ) return;
	m_pMenuRoot->AddItem( pMenuItem );
}
//---------------------------------------------------------------------------------------------------------
void LRMenu::Show( int iX, int iY )
{
	CHECK_UPDATING();
	if ( m_bIsShowing ) return;

	m_bIsShowing = true;
	Rect r = LocalRenderer::GetViewport();
	m_pDisplayMenu->SetSubmenuX( r.left + iX );
	m_pDisplayMenu->SetSubmenuY( r.top + iY );
	m_pDisplayMenu->Show( );	
}
//---------------------------------------------------------------------------------------------------------
void LRMenu::Hide()
{
	CHECK_UPDATING();
	if ( !m_bIsShowing ) return;
	m_bIsShowing = false;
	while ( NULL != m_pDisplayMenu->GetParent() ) {
		m_pDisplayMenu->Hide();
		m_pDisplayMenu = m_pDisplayMenu->GetParent();
	}
	m_pDisplayMenu->Hide();

}
//---------------------------------------------------------------------------------------------------------
void LRMenu::Paint( bool bFull )
{	
	CHECK_UPDATING();

	if ( !m_bIsShowing ) return;
	if ( !bFull ){
		m_pDisplayMenu->PaintSubmenu( true );
	}
	else {
		LRMenuItem *pSubmenu = m_pDisplayMenu;
		vector<LRMenuItem*> vSubmenus;
		while ( pSubmenu ) {
			vSubmenus.push_back( pSubmenu );
			pSubmenu = pSubmenu->GetParent();
		}
		for ( vector<LRMenuItem*>::reverse_iterator iter = vSubmenus.rbegin(); iter!= vSubmenus.rend(); ++iter ){
			(*iter)->Show( true );
		}
	}
}
//---------------------------------------------------------------------------------------------------------
bool LRMenu::KeyPress( uchar ucKey )
{
	if ( !m_pDisplayMenu ) return false;
	if ( !IsShowing() ) return false;

	if ( ucKey>='0' && ucKey<='9' ) {
		if ( !m_pDisplayMenu->SelectItem( ucKey ) ) return true;
		if ( ( ucKey==DEFAULT_PAGEUP_SHORTCUT || ucKey==DEFAULT_PAGEDOWN_SHORTCUT ) && m_pDisplayMenu->HasScroll() ) {
			m_pDisplayMenu->MoveCursor( -1 );
		}
		else ucKey = MENU_EXEC_KEY;
		m_pDisplayMenu->PaintSubmenu( );
	}
	switch( ucKey ){
		case MENU_UP_KEY:{			
			m_pDisplayMenu->MoveCursor( 0 );
			m_pDisplayMenu->PaintSubmenu( );
						 }
			break;
		case MENU_DOWN_KEY:{
			m_pDisplayMenu->MoveCursor( 1 );
			m_pDisplayMenu->PaintSubmenu( );
						   }
			break;
		case MENU_BACK_KEY:
		case MENU_LEFT_KEY:
			m_pDisplayMenu->Hide( );
			if ( m_pDisplayMenu->GetParent() ){				
				m_pDisplayMenu = m_pDisplayMenu->GetParent();
				m_pDisplayMenu->BringToFront( );
			}else {				
				m_bIsShowing = false;
			}
			break;
		case MENU_RIGHT_KEY:
			if (m_pDisplayMenu->GetCrtItem() )
				if ( m_pDisplayMenu->GetCrtItem()->HasSubMenu() ) {				
					m_pDisplayMenu = m_pDisplayMenu->GetCrtItem();
					m_pDisplayMenu->Exec( );
				}
			break;
		case MENU_EXEC_KEY:
			if ( m_pDisplayMenu->GetCrtItem() ) {
				if ( m_pDisplayMenu->GetCrtItem()->HasSubMenu() ) {
					m_pDisplayMenu = m_pDisplayMenu->GetCrtItem();
					m_pDisplayMenu->Exec( );
				}
				else {
					if ( !m_pDisplayMenu->GetCrtItem()->Exec( ) ) break;
					m_pDisplayMenu->Hide();
					if ( m_pDisplayMenu->GetParent() ){
						m_pDisplayMenu = m_pDisplayMenu->GetParent();
						if ( m_pDisplayMenu->Changed() ) {
							m_pDisplayMenu->Hide();
							m_pDisplayMenu->Show();
						}
						else m_pDisplayMenu->BringToFront( );
					}
					else {
						m_bIsShowing = false;
					}				
				}
			}
			break;

	}
	return true;
}
//---------------------------------------------------------------------------------------------------------
bool LRMenu::HandleKeys( int nPK_Button, bool bKeyUp )
{
	if ( NULL == m_pDisplayMenu ) return false;

	bool bHandled = true;

	if ( !bKeyUp ){
		switch( nPK_Button ){
			case BUTTON_Up_Arrow_CONST:
				bHandled = KeyPress( MENU_UP_KEY );
				break;
			case BUTTON_Down_Arrow_CONST:
				bHandled = KeyPress( MENU_DOWN_KEY );
				break;
			case BUTTON_Left_Arrow_CONST:
				bHandled = KeyPress( MENU_LEFT_KEY );
				break;
			case BUTTON_Right_Arrow_CONST:
				bHandled = KeyPress( MENU_RIGHT_KEY );
				break;
			case BUTTON_Enter_CONST:
				bHandled = KeyPress( MENU_EXEC_KEY );
				break;
			case BUTTON_Phone_C_CONST:
			case BUTTON_Rept_Phone_C_CONST:
				bHandled = KeyPress( MENU_BACK_KEY );
				break;
			default:
				bHandled = false;
				break;
		}
	}
	else {
		if ( BUTTON_Phone_Soft_left_CONST==nPK_Button && !IsShowing() ) {
			Show( 0, 0 );
		}
		else if ( nPK_Button>=BUTTON_1_CONST && nPK_Button<BUTTON_0_CONST ) {
			uchar ucKey = '1' + nPK_Button - BUTTON_1_CONST;
			bHandled = KeyPress( ucKey );
		}
		else if ( BUTTON_0_CONST==nPK_Button ) {
			bHandled = KeyPress( '0' );
		}
		else bHandled = false;
	}
	return bHandled;
}
//---------------------------------------------------------------------------------------------------------
bool LRMenu::HandleStylus( int iX, int iY )
{
	if ( NULL == m_pDisplayMenu ) return false;
	if ( !m_pDisplayMenu->SelectItem( iX, iY ) ) {
		KeyPress( MENU_BACK_KEY ) ;		
	}
	else {
		m_pDisplayMenu->PaintSubmenu( );
		KeyPress( MENU_EXEC_KEY ) ;
	}

	return true;
}

void LRMenu::SetDirty( bool bDirty /* = true*/ )
{
	if ( NULL != m_pDisplayMenu ) {
		m_pDisplayMenu->MakeSubmenuDirty( bDirty );
	}
}


//---------------------------------------------------------------------------------------------------------
/*
 *
 *	class LRMenuItem
 *
 */
//---------------------------------------------------------------------------------------------------------
/*virtual*/ bool LRMenuItem::SelectItem( TCHAR ucShortcut )
{
	if ( !HasScroll() || 
		 ( ucShortcut!=DEFAULT_PAGEUP_SHORTCUT && ucShortcut!=DEFAULT_PAGEDOWN_SHORTCUT ) ){
		map<TCHAR, int>::iterator iterPos = m_mShotcuts.find( ucShortcut );
		if ( iterPos == m_mShotcuts.end() ) return false;
		if ( m_iHighlight>=0 && m_iHighlight<m_vMenuItems.size() ) 
			m_vMenuItems[m_iHighlight]->SetDirty();
		m_iHighlight = iterPos->second;	
		if ( m_iHighlight>=0 && m_iHighlight<m_vMenuItems.size() ) 
			m_vMenuItems[m_iHighlight]->SetDirty();	
	}
	else {
		//'0' -> up
		//'9' -> down
		if ( ucShortcut==DEFAULT_PAGEUP_SHORTCUT ) { //up
			m_iHighlight = m_iStart - m_iItemsCount;
			if ( m_iHighlight<0 ) m_iHighlight = 0;
		}
		else if ( ucShortcut==DEFAULT_PAGEDOWN_SHORTCUT ) { //down
			m_iHighlight = m_iStart + m_iItemsCount;
			if ( m_iHighlight>m_vMenuItems.size()-m_iItemsCount ) {
				m_iHighlight=m_vMenuItems.size()-m_iItemsCount;
				if (m_iHighlight<0 ) m_iHighlight = 0;
			}
		}

	}

	return true;
}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ bool LRMenuItem::SelectItem( int iX, int iY )
{
	if ( PointInSubmenu(iX, iY ) ){	
		int i=0;
		for ( vector<LRMenuItem*>::iterator iter=m_vMenuItems.begin() ; iter!=m_vMenuItems.end(); ++iter, ++i ){
			if ( (*iter)->GetVisible() && (*iter)->PointIn( iX, iY ) ) {
				if ( m_iHighlight>=0 && m_iHighlight<m_vMenuItems.size() ) 
					m_vMenuItems[m_iHighlight]->SetDirty();
				m_iHighlight = i;
				if ( m_iHighlight>=0 && m_iHighlight<m_vMenuItems.size() ) 
					m_vMenuItems[m_iHighlight]->SetDirty();	

				return true;
			}
		}
	}
	return false;
}
//---------------------------------------------------------------------------------------------------------
void LRMenuItem::ClearSubmenu()
{
	for ( vector<LRMenuItem*>::iterator iter=m_vMenuItems.begin() ; iter!=m_vMenuItems.end(); ++iter ){
		PLUTO_SAFE_DELETE( *iter );
	}
	m_vMenuItems.empty();
	m_uiItemsHeight = m_uiItemsWidth = 0;
}
//---------------------------------------------------------------------------------------------------------
LRMenuItem::~LRMenuItem()
{
	PLUTO_SAFE_DELETE( m_pClone );

	ClearSubmenu();
}
//---------------------------------------------------------------------------------------------------------
LRMenuItem* LRMenuItem::AddItem( LRMenuItemData& ItemData )
{
	LRMenuItem *pItem = new LRMenuItem( ItemData, this );	
	m_vMenuItems.push_back( pItem );
	m_uiItemsHeight = m_uiItemsWidth = 0;
	return pItem;
}
//---------------------------------------------------------------------------------------------------------
void LRMenuItem::AddItem( LRMenuItem* pMenuItem )
{ 
	if ( !pMenuItem ) return;	
	pMenuItem->SetParent( this );
	m_vMenuItems.push_back( pMenuItem );
	m_uiItemsHeight = m_uiItemsWidth = 0;
}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ void LRMenuItem::InsertItem( unsigned int uiWhere, LRMenuItem* pMenuItem )
{
	if ( !pMenuItem ) return;
	pMenuItem->SetParent( this );
	vector<LRMenuItem*>::iterator iter = m_vMenuItems.begin();
	iter += uiWhere;
	if ( iter != m_vMenuItems.begin() ) {
		m_vMenuItems.insert( iter, pMenuItem );
	}
	else m_vMenuItems.push_back( pMenuItem );
	m_uiItemsHeight = m_uiItemsWidth = 0;
}
//---------------------------------------------------------------------------------------------------------
bool LRMenuItem::Exec( )
{
	if ( HasSubMenu() ) {
		m_bIsOnTop = false;
		Show( );
		return false;
	}

	return DoAction( );
}
//---------------------------------------------------------------------------------------------------------
void LRMenuItem::MoveCursor( int iDir )
{
	if ( m_iHighlight<0 ) return;

	PLUTO_SAFETY_LOCK(cm, LocalRenderer::Mutex() );

	switch( iDir ){
		case 0: // Up
			if ( m_iHighlight==0 ) break;
			if ( m_iHighlight == m_iStart ) --m_iStart;
			m_vMenuItems[m_iHighlight--]->SetDirty();
			m_vMenuItems[m_iHighlight]->SetDirty();				
			break;
		case 1: //Down
			if ( m_iHighlight == m_vMenuItems.size()-1 ) break;
			if ( m_iHighlight == m_iEnd )	++m_iStart;
			m_vMenuItems[m_iHighlight++]->SetDirty();
			m_vMenuItems[m_iHighlight]->SetDirty();
			break;
		case -1: // set start at m_iHighlight
			m_iStart = m_iHighlight;
			break;

	}
}
//---------------------------------------------------------------------------------------------------------
bool LRMenuItem::BeginPaint( RECT r, bool bForceRepaint )
{
	if ( m_bPainting ) return false;

	if ( !m_bDirty && !bForceRepaint ) return false;

	m_rClientRect = r;
	m_bPainting = true;
	return true;
}
//---------------------------------------------------------------------------------------------------------
void LRMenuItem::Paint( RECT r, bool bHighlight, bool bForceRepaint )
{
	if ( !BeginPaint( r, bForceRepaint ) ) return;

	Rect rr( r );

	// Paint background
	if ( !bHighlight )
		LocalRenderer::FillRect( rr, RGB( 0,0, 255 ) );
	else
		LocalRenderer::FillRect( rr, RGB( 255,255, 0 ) );

	// Draw shortcut
	if ( m_ucShortcut ) {
		tstring sShortcut;
		sShortcut += m_ucShortcut;
		LocalRenderer::DrawText( sShortcut.c_str(), rr, RGB(255, 0, 0 ) );
		rr.left += 10;
	}

	// Draw caption
	LocalRenderer::DrawText( m_sCaption.c_str(), rr, RGB(0,0,0) );

	EndPaint();

}
//---------------------------------------------------------------------------------------------------------
void LRMenuItem::CalcSubmenuLayout( RECT rViewport )
{
	Rect r(rViewport);
	int iX = GetSubmenuX();		
	int iY = GetSubmenuY();	

	m_bHasScroll = false;

	if ( GetSubmenuHeight()>r.GetHeight() ) {
		m_bHasScroll = true;
		m_rSubmenu.top = rViewport.top+4;
		m_rSubmenu.bottom = m_rSubmenu.top+GetSubmenuHeight( rViewport.bottom-m_rSubmenu.top+1-DEFAULT_SCROLLBAR_HEIGHT*2 )
			+DEFAULT_SCROLLBAR_HEIGHT*2;
	}
	else {				
		m_rSubmenu.top = iY;
		if ( m_rSubmenu.top+GetSubmenuHeight()>r.bottom ) 
			m_rSubmenu.top = r.bottom - GetSubmenuHeight();		
		m_rSubmenu.bottom = m_rSubmenu.top+GetSubmenuHeight();
	}
	if ( GetSubmenuWidth()>r.GetWidth() ){
		m_rSubmenu.left = rViewport.left+2;
		m_rSubmenu.right = rViewport.right;
	}
	else {
		m_rSubmenu.left = iX;
		if ( m_rSubmenu.left+GetSubmenuWidth()>r.right ) 
			m_rSubmenu.left = r.right - GetSubmenuWidth();
		m_rSubmenu.right = m_rSubmenu.left + GetSubmenuWidth();
	}
	m_rSubmenu.right++;
	m_rSubmenu.bottom++;	
}
//---------------------------------------------------------------------------------------------------------
void LRMenuItem::MakeSubmenuDirty( bool bDirty )
{
	for ( vector<LRMenuItem*>::iterator iter=m_vMenuItems.begin() ; iter!=m_vMenuItems.end(); ++iter ){
		( *iter )->SetDirty( bDirty );
	}
}
//---------------------------------------------------------------------------------------------------------
void LRMenuItem::ShowSubmenu( bool bShow )
{
	if ( bShow ){	// Show Submenu
		CalcSubmenuLayout( LocalRenderer::GetViewport() );
		// Save background
		m_pClone = LocalRenderer::SaveRect( m_rSubmenu );
		m_bIsShowing = true;
		m_iHighlight = 0;
	}
	else if (m_pClone) {	// Hide submenu		
		// Restore background
		LocalRenderer::RestoreRect( m_rSubmenu.left, m_rSubmenu.top, m_pClone );
		PLUTO_SAFE_DELETE( m_pClone );		
		#if defined(SMARTPHONE2005)
			// Reset the viewport, because PocketFrog sets it on Update
			if ( NULL == m_pParent ) LocalRenderer::Update( NULL );
			else LocalRenderer::Update( &m_rSubmenu );
		#else
			LocalRenderer::Update( &m_rSubmenu );
		#endif		
		// Mark submenu as dirty
		MakeSubmenuDirty();
		m_iEnd = m_iStart = 0;
		m_iHighlight = -1;
		m_bIsShowing = false;
	}
}
//---------------------------------------------------------------------------------------------------------
void LRMenuItem::PaintSubmenu( bool bForceRepaint )
{
	if ( !m_bIsShowing ) return;

	PLUTO_SAFETY_LOCK(cm, LocalRenderer::Mutex() );

	Rect r(m_rSubmenu );
	bool bUpScroll = false, bDownScroll= false;
	int i=0, iFirstVisible=0, iLastVisible=0;

	// Get first/last visible items
	iFirstVisible = m_vMenuItems.size();
	iLastVisible = -1;
	for ( i=0; i<m_vMenuItems.size(); ++i ) {
		if ( m_vMenuItems[i]->GetVisible() ) {
			if ( i < iFirstVisible ) iFirstVisible = i;
			if ( i > iLastVisible )  iLastVisible = i;
		}
	}

	// has scroll up if first drawn item is not the first visible one
	bUpScroll = m_bHasScroll && ( m_iStart != iFirstVisible );
	// has scroll down if last drawn item is not the last visible one
	bDownScroll = m_bHasScroll && ( m_iStart+m_iItemsCount <= iLastVisible );
	// if there's only one scroll adjust rect
	if ( bUpScroll ^ bDownScroll )	r.bottom -= DEFAULT_SCROLLBAR_HEIGHT;	

	if ( m_bHasScroll ) bForceRepaint = true;
	
	// Draw contour	
	LocalRenderer::DrawHRect( r, DEFAULT_BORDER_COLOR );

	if ( bUpScroll ) { // paint up scroll
		Rect rr(r);
		rr.bottom = rr.top+DEFAULT_SCROLLBAR_HEIGHT+1;		

		LocalRenderer::FillRect( rr, DRAW_SCROLLBAR_COLOR );
		LocalRenderer::DrawHRect( rr, DEFAULT_BORDER_COLOR );

		rr.left += DRAW_ITEMTEXT_MARGIN;
		tstring sShortcut = _T("");
		sShortcut += DEFAULT_PAGEUP_SHORTCUT;
		LocalRenderer::DrawText( sShortcut.c_str(), rr, RGB(0, 0, 0 ) );		
		rr.left += 10;
		sShortcut = DEFAULT_PAGEUP_TEXT;
		LocalRenderer::DrawSmallText( sShortcut.c_str(), rr, RGB(0, 0, 0 ), DEFAULT_SMALLFONT_POINT_SCROLL, true );
		r.top += DEFAULT_SCROLLBAR_HEIGHT;		
	}

	if ( bDownScroll )	r.bottom -= DEFAULT_SCROLLBAR_HEIGHT+1;	

	// Adjust margins
	r.left +=1; r.top +=1;r.right -=1; r.bottom -=1;
	UINT uiCrtPos = r.top;

	

	// remove shortcuts - reassign shortcuts to items on painting
	m_mShotcuts.clear();
	TCHAR ucShortcut=_T('1');

	// Paint dirty/visible submenu items	
	for ( i=m_iStart ; i<m_vMenuItems.size() && uiCrtPos<r.bottom && i-m_iStart<=m_iItemsCount; ++i ) {
		if ( m_vMenuItems[i]->GetVisible() ) {
			Rect rItem( r.left, uiCrtPos, r.right, 
				uiCrtPos+m_vMenuItems[i]->GetHeight() );
			if ( rItem.bottom>r.bottom ) {
				//rItem.bottom = r.bottom;							
				break;
			}
			m_iEnd = i;
			AddShortcut( i, ucShortcut );
			m_vMenuItems[i]->Paint( rItem, m_iHighlight==i, bForceRepaint );

			LocalRenderer::DrawHLine( rItem.left, rItem.bottom, rItem.GetWidth(), RGB(0,0,0) );
			uiCrtPos += m_vMenuItems[i]->GetHeight()+1;						
			++ucShortcut;
		}
	}		

	r.bottom = uiCrtPos;
	r.top = r.bottom;	

	if ( bDownScroll ) { // paint down scroll
		/*
		r.bottom += DEFAULT_SCROLLBAR_HEIGHT+2;	
		Rect rr(r);
		rr.top = rr.bottom - DEFAULT_SCROLLBAR_HEIGHT-2;
		*/
		r.bottom += DEFAULT_SCROLLBAR_HEIGHT+1;	

		Rect rr(r);
		rr.top--;

		tstring sShortcut = _T("0");
		//Scroll down				
		rr.left -=1;rr.right +=1;
		LocalRenderer::FillRect( rr, DRAW_SCROLLBAR_COLOR );		
		LocalRenderer::DrawHRect( rr, DEFAULT_BORDER_COLOR );
		rr.left += DRAW_ITEMTEXT_MARGIN;
		sShortcut = _T("9");
		LocalRenderer::DrawText( sShortcut.c_str(), rr, RGB(0, 0, 0 ) );
		rr.left += 10;
		sShortcut = _T("Page down");
		LocalRenderer::DrawSmallText( sShortcut.c_str(), rr, RGB(0, 0, 0 ), DEFAULT_SMALLFONT_POINT_SCROLL, true );
	}
	if ( bDownScroll || bUpScroll ){				
		Rect rr(m_rSubmenu);
		rr.top = r.bottom;						
		Rect rrr( 0, m_pClone->GetHeight()-rr.GetHeight(), m_pClone->GetWidth(), m_pClone->GetHeight() );
		LocalRenderer::RestoreRect( rr.left, rr.top, m_pClone, &rrr );
	}
	

	//Update Dispaly Device
	LocalRenderer::Update( &m_rSubmenu );
}
//---------------------------------------------------------------------------------------------------------
void LRMenuItem::AddShortcut( int iPos, TCHAR ucShortcut )
{
	m_mShotcuts[ucShortcut] = iPos;
	m_vMenuItems[iPos]->SetShortcut( ucShortcut );
}
//---------------------------------------------------------------------------------------------------------
UINT LRMenuItem::GetSubmenuHeight( int iLimit )
{
	// Calculate submenu height by adding all items heights	
	if ( iLimit<0 ){
		if ( !m_uiItemsHeight && m_vMenuItems.size() ) {
			m_iItemsCount = 0;
			m_uiItemsHeight = 1;
			for ( vector<LRMenuItem*>::iterator iter=m_vMenuItems.begin() ; iter!=m_vMenuItems.end(); ++iter )
				if ( (*iter)->GetVisible() ) {
					m_uiItemsHeight += (*iter)->GetHeight()+1;
					++m_iItemsCount;
				}
		}
		return m_uiItemsHeight;
	}

	// if iLimit>=0 limit submenu height to first height smaller than iLimit
	UINT uiItemsHeight = 1;
	vector<LRMenuItem*>::iterator iter;
	m_iItemsCount = 0;
	for ( iter=m_vMenuItems.begin() ; iter!=m_vMenuItems.end() && uiItemsHeight<iLimit ; ++iter ) {
		if ( (*iter)->GetVisible() ) {
			++m_iItemsCount;
			uiItemsHeight += (*iter)->GetHeight()+1;			
		}
	}
	if ( uiItemsHeight>iLimit && iter!=m_vMenuItems.end() ){
		uiItemsHeight -= (*iter)->GetHeight()+1;
		--m_iItemsCount;
	}
	return uiItemsHeight;
}
//---------------------------------------------------------------------------------------------------------
UINT LRMenuItem::GetSubmenuWidth( void )
{
	// Calculate max width of submenu items
	if ( !m_uiItemsWidth && m_vMenuItems.size() ) {		
		for ( vector<LRMenuItem*>::iterator iter=m_vMenuItems.begin() ; iter!=m_vMenuItems.end(); ++iter )
			if ( (*iter)->GetVisible() ) {
				if ( m_uiItemsWidth < (*iter)->GetWidth() ) 
					m_uiItemsWidth = (*iter)->GetWidth();
			}
		// Add margin width
		m_uiItemsWidth += 2;
	}
	return m_uiItemsWidth;
}
//---------------------------------------------------------------------------------------------------------
bool LRMenuItem::HasSubMenu( void )
{
	for ( vector<LRMenuItem*>::iterator iter=m_vMenuItems.begin() ; iter!=m_vMenuItems.end(); ++iter ){
		if ( (*iter)->GetVisible() ) return true;
	}
	return false;
}
//---------------------------------------------------------------------------------------------------------
bool LRMenuItem::PointInSubmenu( int iX, int iY )
{
	if ( iX<=m_rSubmenu.left || iX>=m_rSubmenu.right ) return false;
	if ( iY<=m_rSubmenu.top || iY>=m_rSubmenu.bottom ) return false;
	return true;
}
//---------------------------------------------------------------------------------------------------------
bool LRMenuItem::PointIn( int iX, int iY )
{
	if ( iX<m_rClientRect.left || iX>m_rClientRect.right ) return false;
	if ( iY<m_rClientRect.top || iY>m_rClientRect.bottom ) return false;
	return true;
}

