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
#include "LRPhoneMenu.h"
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
 *	class LRPhoneMenu
 *
 */
//---------------------------------------------------------------------------------------------------------
COLORREF LRPhoneMenu::m_clTextColor[tbcNone] = { RGB(0,0,0), RGB(0,0,0), RGB(0,0,0), RGB(0,0,0) };
vector<MenuItemInfo> LRPhoneMenu::m_vQueuedItems;
bool LRPhoneMenu::m_bEnabled = true;
//---------------------------------------------------------------------------------------------------------
/*static*/ void LRPhoneMenu::QueueItem( MenuItemInfo ItemInfo )
{	
	if ( m_bEnabled )
		m_vQueuedItems.push_back( ItemInfo );
}
//---------------------------------------------------------------------------------------------------------
/*static*/ void LRPhoneMenu::GetMenuImages( MenuItemInfo::ItemType nItemsType )
{
	OrbiterApp::GetInstance()->GetMenuImages( nItemsType, m_vQueuedItems );
	m_vQueuedItems.clear();
}

//---------------------------------------------------------------------------------------------------------
/*static*/ LRPhoneMenu* LRPhoneMenu::Create()
{	
	LRPhoneMenu* pMenu = new LRPhoneMenu;
	pMenu->CreateMenuRoot();
	return pMenu;
}
//---------------------------------------------------------------------------------------------------------
LRPhoneMenuItem* LRPhoneMenu::FindItem( MenuItemInfo::ItemType nItemsType, MenuItemInfo& ItemInfo )
{
	if ( NULL==m_pCrtRoomItem ) return NULL;
	return m_pCrtRoomItem->FindItem( nItemsType, ItemInfo );
	
}
//---------------------------------------------------------------------------------------------------------
void LRPhoneMenu::SetMenuImages( MenuItemInfo::ItemType nItemsType, vector<MenuItemInfo*>& vItems )
{
	bool bNeedRepaint = false;
	if ( NULL==m_pCrtRoomItem ) return;
	for ( vector<MenuItemInfo*>::iterator iter = vItems.begin(); iter!=vItems.end() ; ++iter ){
		bNeedRepaint = m_pCrtRoomItem->SetItemInfo( nItemsType, *(*iter) ) || bNeedRepaint;
	}
	if ( nItemsType==MenuItemInfo::stScenarios ) {
		((LRPhoneMenuItemRoot*)m_pMenuRoot)->ResetQueryImages();
	}

	if ( bNeedRepaint ) Paint();
}
//---------------------------------------------------------------------------------------------------------
void LRPhoneMenu::SetCrtRoom( long nCrtRoom )
{
	if ( m_pCrtRoomItem ) {

		if ( IsShowing() ) {
			Hide();
		}		
		m_pCrtRoomItem->SelectRoom( nCrtRoom );
		Show( 0, 0);
	}
}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ void LRPhoneMenu::AddItem( LRMenuItem* pMenuItem )
{
	LRMenu::AddItem( pMenuItem );
	if ( NULL==m_pCrtRoomItem ) {
		m_pCrtRoomItem = dynamic_cast<LRCrtRoomItem*>(pMenuItem);
	}
}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ void LRPhoneMenu::CreateMenuRoot()
{ 
	m_pMenuRoot = new LRPhoneMenuItemRoot; 
	m_pDisplayMenu = m_pMenuRoot;
	m_bIsShowing = false;
}


//---------------------------------------------------------------------------------------------------------
/*
 *
 *	class LRPhoneMenuItem
 *
 */
//---------------------------------------------------------------------------------------------------------
void LRPhoneMenuItem::CheckForImage( void )
{
	if ( m_bAskedForPictures ) {
		DWORD dwCrtTime = GetTickCount();
		if ( dwCrtTime-m_dwAskedTime >= DEFAULT_GETIMAGE_TIMEOUT )
			m_bAskedForPictures = false;
	}

	if ( m_bHasPicture && (NULL == m_pImage) && !m_bAskedForPictures ) {
		LRPhoneMenuItem* pParent = dynamic_cast<LRPhoneMenuItem*>(m_pParent);
		if ( NULL != pParent ) {
			LRPhoneMenu::QueueItem( GetItemInfo() );			
			m_bAskedForPictures = true;
			m_dwAskedTime = GetTickCount();
		}
	}
}
//---------------------------------------------------------------------------------------------------------
void LRPhoneMenuItem::Paint( RECT r, bool bHighlight, bool bForceRepaint )
{
	CheckForImage();

	if ( !BeginPaint( r, bForceRepaint ) ) return;	

	
	// Set background color
	COLORREF clFillColor, clTextColor;
	if ( !bHighlight ) {
		clFillColor = LRPhoneMenu::GetColor( LRPhoneMenu::tbcBack );
		clTextColor = LRPhoneMenu::GetColor( LRPhoneMenu::tbcFore );
	}
	else {
		clFillColor = LRPhoneMenu::GetColor( LRPhoneMenu::tbcHlBack );
		clTextColor = LRPhoneMenu::GetColor( LRPhoneMenu::tbcHlFore );
	}

	Rect rr(r);

	// Paint background
	LocalRenderer::FillRect( rr, clFillColor );

	rr.left += DRAW_ITEMTEXT_MARGIN;
	rr.right -= DRAW_ITEMTEXT_MARGIN;
	rr.top += DRAW_ITEMTEXT_MARGIN;
	rr.bottom -= DRAW_ITEMTEXT_MARGIN;

	// Paint shorcut if he's got one
	if ( m_ucShortcut ) {
		tstring sShortcut;
		sShortcut += m_ucShortcut;		
		LocalRenderer::DrawText( sShortcut.c_str(), rr, DRAW_SHORTCUT_COLOR );
		rr.left += DRAW_SHORTCUT_WIDTH;
	}

	// Draw a triangle if it has submenu
	if ( HasSubMenu() ) {
		LocalRenderer::FillTriangle( rr.right-DRAW_SUBMENUMARK_WIDTH, (rr.top+rr.bottom)/2 - DRAW_SUBMENUMARK_HEIGHT/2, 
									 rr.right-DRAW_SUBMENUMARK_WIDTH, (rr.top+rr.bottom)/2 + DRAW_SUBMENUMARK_HEIGHT/2, 
									 rr.right, (rr.top+rr.bottom)/2, DRAW_SUBMENUMARK_COLOR );
		rr.right -= DRAW_SUBMENUMARK_WIDTH+2;
	}
		
	if ( NULL != m_pImage ) {
		Rect rImage( rr );
		rImage.right = rImage.left + DRAW_IMAGE_WIDTH;
		rImage.bottom = rImage.top + DRAW_IMAGE_HEIGHT;
		LocalRenderer::DrawImage( m_pImage, m_nImageSize, rImage );				
	}	
	
	rr.left += DRAW_IMAGE_WIDTH;	\
	rr.left += DRAW_ITEMTEXT_MARGIN;
	// Draw caption
	LocalRenderer::DrawText( m_sCaption.c_str(), rr, clTextColor, m_bStyleBold );


	EndPaint();
}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ bool LRPhoneMenuItem::SetItemInfo( MenuItemInfo::ItemType nItemsType, MenuItemInfo& ItemInfo )
{
	bool bFound = false;
	for ( vector<LRMenuItem*>::iterator iter=m_vMenuItems.begin() ; iter!=m_vMenuItems.end(); ++iter ){
		LRPhoneMenuItem* pItem = (LRPhoneMenuItem*)(*iter);
		if ( nItemsType == pItem->GetItemType() ) {			
			if ( *pItem == ItemInfo ) {
				PLUTO_SAFETY_LOCK(cm, LocalRenderer::Mutex() );
				pItem->SetImage( ItemInfo.m_nImageType, ItemInfo.m_nImageSize, ItemInfo.m_pImage );
				pItem->SetDirty( true );
				bFound = true;
			}
		}
		else {
			bFound = pItem->SetItemInfo( nItemsType, ItemInfo );
		}
	}
	return bFound;
}
//---------------------------------------------------------------------------------------------------------
LRPhoneMenuItem* LRPhoneMenuItem::FindItem( MenuItemInfo::ItemType nItemsType, MenuItemInfo& ItemInfo )
{
	LRPhoneMenuItem* pRet = NULL;
	for ( vector<LRMenuItem*>::iterator iter=m_vMenuItems.begin() ; iter!=m_vMenuItems.end(); ++iter ){
		if ( nItemsType == ((LRPhoneMenuItem*)(*iter))->GetItemType() ) {			
			if ( *(LRPhoneMenuItem*)(*iter) == ItemInfo ) {
				pRet = (LRPhoneMenuItem*)(*iter);
				break;
			}
		}
		else {
			pRet = ((LRPhoneMenuItem*)(*iter))->FindItem( nItemsType, ItemInfo );
			if ( NULL != pRet ) 
				break;				
		}		
	}
	return pRet;
}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ void LRPhoneMenuItem::PaintSubmenu( bool bForceRepaint )
{	
	LRPhoneMenu::m_vQueuedItems.clear();

	LRMenuItem::PaintSubmenu( bForceRepaint );

	if ( LRPhoneMenu::m_vQueuedItems.size()>0 ) {
		LRPhoneMenu::GetMenuImages( GetSubmenuType() );
	}
}

//---------------------------------------------------------------------------------------------------------
/*
 *
 *	class LRPhoneMenuItemRoot
 *
 */
//---------------------------------------------------------------------------------------------------------
void LRPhoneMenuItemRoot::RemoveLinks( void )
{
	if ( m_vMenuItems.size()<=1 ) return;

	for ( vector<LRMenuItem*>::iterator iter=m_vMenuItems.begin() ; iter!=m_vMenuItems.end();  ){
		if ( (*iter)->IsLink() ) {
			PLUTO_SAFE_DELETE( *iter );
			m_vMenuItems.erase( iter );
		}
		else ++iter;
	}
}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ void LRPhoneMenuItemRoot::PaintSubmenu( bool bForceRepaint/*=false*/ )
{
	if ( m_bAskedForPictures ) {
		DWORD dwCrtTime = GetTickCount();
		// 
		if ( dwCrtTime-m_dwAskedTime >= DEFAULT_GETIMAGE_TIMEOUT+1000 ){
			m_bAskedForPictures = false;
		}
	}

	if ( !m_bAskedForPictures ) { 
		LRPhoneMenu::m_vQueuedItems.clear();

		LRMenuItem::PaintSubmenu( bForceRepaint );

		if ( LRPhoneMenu::m_vQueuedItems.size()>0 ) {
			LRPhoneMenu::GetMenuImages( GetSubmenuType() );
			m_bAskedForPictures = true;
			m_dwAskedTime = GetTickCount();
		}
	}
	else {
		LRPhoneMenu::SetQueueEnabled( false );
		LRMenuItem::PaintSubmenu( bForceRepaint );
		LRPhoneMenu::SetQueueEnabled( );
	}
}

//---------------------------------------------------------------------------------------------------------
/*
 *
 *	class LRCrtRoomItem
 *
 */
//---------------------------------------------------------------------------------------------------------
void LRCrtRoomItem::SelectRoom( int iRoomId )
{
	if ( iRoomId<0 ) return;

	for ( vector<LRMenuItem*>::iterator iter=m_vMenuItems.begin() ; iter!=m_vMenuItems.end(); ++iter ) {
		LRRoomItem* pRoomItem = dynamic_cast<LRRoomItem*>( *iter );
		if ( !pRoomItem ) return;
		if ( pRoomItem->GetRoomId()==iRoomId ) { // RoomId found
			// Set CrtRoomItem caption  = Room caption
			SetCaption( pRoomItem->GetCaption() );
			// Create links in root menu
			pRoomItem->CreateLinks();
			break;
		}
	}
}
//---------------------------------------------------------------------------------------------------------
void LRCrtRoomItem::Paint( RECT r, bool bHighlight, bool bForceRepaint )
{
	if ( !BeginPaint( r, bForceRepaint) ) return;
		
	Rect rr( r );
	// Draw a double contour
	LocalRenderer::DrawHRect( rr, DEFAULT_BORDER_COLOR );
	rr.left++;rr.top++;rr.right--;rr.bottom--;

	EndPaint();
	SetDirty();


	// Draw text in bold
	LRPhoneMenuItem::Paint( rr, bHighlight, bForceRepaint );
	
}

//---------------------------------------------------------------------------------------------------------
/*
 *
 *	class LRRoomItem
 *
 */
//---------------------------------------------------------------------------------------------------------
bool LRRoomItem::DoAction( void )
{
	m_pParent->SetCaption( m_sCaption );
	CreateLinks();
	return true;
}
//---------------------------------------------------------------------------------------------------------
void LRRoomItem::CreateLinks( void )
{
	LRCrtRoomItem* pCrtRoom = dynamic_cast<LRCrtRoomItem*>(m_pParent);;
	LRPhoneMenuItemRoot* pRootItem = dynamic_cast<LRPhoneMenuItemRoot*>(pCrtRoom->GetParent());
	if ( !pCrtRoom || !pRootItem ) return;

	pRootItem->RemoveLinks();
	for ( vector<LRMenuItem*>::reverse_iterator iter=m_vMenuItems.rbegin(); iter!=m_vMenuItems.rend(); ++iter ) {
		LRMenuItemLink* pLink = new LRMenuItemLink( *iter, NULL );
		pRootItem->InsertItem( 1, pLink );
	}	
}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ bool LRRoomItem::operator==( MenuItemInfo& ItemInfo )
{
	if ( m_iRoomId==ItemInfo.m_nRoomId ) 
		return true;
	return false;
}




//---------------------------------------------------------------------------------------------------------
/*
 *
 *	class LRScenarioItem
 *
 */
//---------------------------------------------------------------------------------------------------------
/*virtual*/ void LRScenarioItem::CheckForImage( void )
{
	if ( !LRPhoneMenu::QueueEnabled() ) return;

	if ( m_bHasPicture && (NULL == m_pImage) && !m_bAskedForPictures ) {
		LRPhoneMenuItem* pParent = dynamic_cast<LRPhoneMenuItem*>(m_pParent);
		if ( NULL != pParent ) {
			LRPhoneMenu::QueueItem( GetItemInfo() );			
			m_bAskedForPictures = true;
			m_dwAskedTime = GetTickCount();
		}
	}
}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ bool LRScenarioItem::operator==( MenuItemInfo& ItemInfo )
{
	string sCaption;
	GetStringCaption( sCaption );
	if ( sCaption==ItemInfo.m_sScenName ) 
		return true;
	return false;
}



//---------------------------------------------------------------------------------------------------------
/*
 *
 *	class LRSubScenarioItem
 *
 */
//---------------------------------------------------------------------------------------------------------
bool LRSubScenarioItem::DoAction( void )
{
	if ( OrbiterApp::GetInstance() ) {
		OrbiterApp::GetInstance()->SendSelectedItem( m_sId );
	}
	return true;
}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ bool LRSubScenarioItem::operator==( MenuItemInfo& ItemInfo )
{
	return m_sId==ItemInfo.m_sObjectId;
}
//---------------------------------------------------------------------------------------------------------





//---------------------------------------------------------------------------------------------------------
/*
 *
 *	class LRSystemItem
 *
 */
//---------------------------------------------------------------------------------------------------------
/*virtual*/ bool LRSystemItem::DoAction( void )
{
	switch( m_SysItemType ){
		case sitSysClose:
			{
				PLUTO_SAFETY_LOCK(cm, LocalRenderer::Mutex() );
				OrbiterApp::GetInstance()->OnQuit();
				return false;
				break;
			}
		case sitMinimize:
			{
				PLUTO_SAFETY_LOCK(cm, LocalRenderer::Mutex() );
				OrbiterApp::GetInstance()->Hide();
				return false;
				break;
			}
	}
	return false;
}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ void LRSystemItem::Paint( RECT r, bool bHighlight, bool bForceRepaint/*=false*/ )
{
	if ( !BeginPaint( r, bForceRepaint ) ) return;	

	
	// Set background color
	COLORREF clFillColor, clTextColor;
	if ( !bHighlight ) {
		clFillColor = LRPhoneMenu::GetColor( LRPhoneMenu::tbcBack );
		clTextColor = LRPhoneMenu::GetColor( LRPhoneMenu::tbcFore );
	}
	else {
		clFillColor = LRPhoneMenu::GetColor( LRPhoneMenu::tbcHlBack );
		clTextColor = LRPhoneMenu::GetColor( LRPhoneMenu::tbcHlFore );
	}

	Rect rr(r);

	// Paint background
	LocalRenderer::FillRect( rr, clFillColor );

	rr.left += DRAW_ITEMTEXT_MARGIN;
	rr.right -= DRAW_ITEMTEXT_MARGIN;
	rr.top += DRAW_ITEMTEXT_MARGIN;
	rr.bottom -= DRAW_ITEMTEXT_MARGIN;

	// Paint shorcut if he's got one
	if ( m_ucShortcut ) {
		tstring sShortcut;
		sShortcut += m_ucShortcut;		
		LocalRenderer::DrawSmallText( sShortcut.c_str(), rr, DRAW_SHORTCUT_COLOR, DEFAULT_SMALLFONT_POINT_TEXT, true );
		rr.left += DRAW_SHORTCUT_WIDTH;
	}

	rr.left += DRAW_ITEMTEXT_MARGIN;
	// Draw caption
	LocalRenderer::DrawSmallText( m_sCaption.c_str(), rr, clTextColor, DEFAULT_SMALLFONT_POINT_TEXT, true );


	EndPaint();

}
