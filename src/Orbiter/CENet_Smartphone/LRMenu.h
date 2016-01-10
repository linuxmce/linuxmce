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
#ifndef _LRMENU_H_
#define _LRMENU_H_

//---------------------------------------------------------------------------------------------------------
#include <string>
#include <vector>
#include <map>
using namespace std;
//---------------------------------------------------------------------------------------------------------
#include <PocketFrog.h>
using namespace Frog;
//---------------------------------------------------------------------------------------------------------
#include "OrbiterApp.h"
//---------------------------------------------------------------------------------------------------------
#include "VIPShared/MenuItemInfo.h"
#include "PlutoUtils/MultiThreadIncludes.h"

//---------------------------------------------------------------------------------------------------------
#define _LOCAL_RENDERED_OBJECTS_
//---------------------------------------------------------------------------------------------------------
#if defined(_VC80_UPGRADE)		//--- CHANGED4WM5 ----//	
	#define DisplayDevice Display
#else
	using namespace Frog::Internal;
#endif
//---------------------------------------------------------------------------------------------------------
#if defined UNICODE || defined _UNICODE
	typedef wstring tstring;
#else
	typedef string tstring;
#endif
//---------------------------------------------------------------------------------------------------------
typedef unsigned char uchar;

//---------------------------------------------------------------------------------------------------------
#define DEFAULT_MENUITEM_WIDTH	150
#define DEFAULT_MENUITEM_HEIGHT	23
#define DEFAULT_BORDER_COLOR RGB( 0, 0, 0 )
#define DRAW_ITEMTEXT_MARGIN 3
#define DEFAULT_SMALLFONT_POINT_TEXT 9

#define DEFAULT_PAGEUP_SHORTCUT '0'
#define DEFAULT_PAGEDOWN_SHORTCUT '9'
#define DEFAULT_PAGEUP_TEXT _T("Page up")

#define DEFAULT_SCROLLBAR_HEIGHT 23
#define DRAW_SCROLLBAR_COLOR  RGB(238,232,170)
#define DEFAULT_SMALLFONT_POINT_SCROLL 10

#define DEFAULT_CRTROOMITEM_CAPTION "Rooms"

//---------------------------------------------------------------------------------------------------------

#define MENU_UP_KEY		'w'
#define MENU_DOWN_KEY	's'
#define MENU_LEFT_KEY	'a'
#define MENU_RIGHT_KEY	'd'
#define MENU_EXEC_KEY	'x'
#define MENU_BACK_KEY	'b'

//---------------------------------------------------------------------------------------------------------
#define RETURN_ON_TRUE( test ) { \
							if ( test ) \
								return; \
		}
#define CHECK_UPDATING( ) RETURN_ON_TRUE( m_bChanging )
//---------------------------------------------------------------------------------------------------------

/*
 *
 *	LocalRenderer - static class implementing methods for drawing on a DisplayDevice
 *
 */
class LocalRenderer {
protected:
	static Rect m_rViewport;
	static Rect m_rAppViewport;
	static pluto_pthread_mutex_t m_RenderMutex;
	static pthread_mutexattr_t m_MutexAttr;
	static void CheckMutex( void );
	static bool m_bEnable;
public:
	// Draw text - for bold creates new font from the current one
	static void DrawText( LPCTSTR Text, Rect &r, COLORREF color, BOOL bBold=false );
	static void DrawSmallText( LPCTSTR Text, Rect &r, COLORREF color, int iPointSize, BOOL bBold=false );

	// Draw an image
	static void DrawImage( char *pData,int nSize, Rect &r );

	// Draw a filled rectangle
	static void FillRect( Rect &r, COLORREF color );
	
	// Update Display Device
	static void Update( Rect* r );

	// Save/restore a rectangle from/to the Display Device
	//		- used for saving/restoring area behinf submenus
	static Surface* SaveRect( Rect r );
	static void RestoreRect( int iX, int iY, Surface* pSurface, Rect* srcRect=NULL );

	// Draws a rectangle
	static void DrawRect( RECT r, COLORREF color );

	// Draws a horizontal rectangle
	static void DrawHRect( RECT r, COLORREF color );

	// Draws a horizontal line  - used in DrawHRect
	static void DrawHLine( int iX, int iY, int iW, COLORREF color );

	// Draws a filled triangle
	static void FillTriangle( int iX1, int iY1, int iX2, int iY2, int iX3, int iY3, COLORREF color );

	// Get current display device from orbiterapp
	static DisplayDevice* GetDisplay() {
		return OrbiterApp::GetInstance()->GetDisplay();
	}

	// Drawing viewport 
	static void SetViewport( Rect rViewport ) { m_rViewport = rViewport; }
	static Rect& GetViewport( ) { return m_rViewport; }

	// Application viewport - used to reset viewport 
	static void SetAppViewport( Rect rViewport ) { m_rAppViewport = rViewport; }
	static Rect& GetAppViewport( ) { return m_rAppViewport; }

	static pluto_pthread_mutex_t& Mutex(){
		CheckMutex();
		return m_RenderMutex;
	}

	static void Enable( bool bEnable=true ) { m_bEnable = bEnable; }

};

/*
 *
 *	LRMenuItemData - menu items drawing data
 *
 */
class LRMenuItemData {
protected:
	bool m_bVisible;
	tstring m_sCaption;
	UINT m_uiWidth, m_uiHeight;	
	unsigned char m_nImageType;
	char *m_pImage;
	unsigned long m_nImageSize;

public:	
	LRMenuItemData() /*: m_RenderItemMutex( "RenderItem" )*/
	{
		m_sCaption = _T("");
		m_uiWidth = DEFAULT_MENUITEM_WIDTH; m_uiHeight = DEFAULT_MENUITEM_HEIGHT;
		m_bVisible = true;
		m_nImageType = 0;
		m_nImageSize = 0;
		m_pImage = NULL;
	}
	LRMenuItemData( LRMenuItemData& ItemData )/* : m_RenderItemMutex( "RenderItem" )*/
	{
		m_sCaption = _T("");
		m_uiWidth = DEFAULT_MENUITEM_WIDTH; m_uiHeight = DEFAULT_MENUITEM_HEIGHT;
		m_bVisible = true;
		m_nImageType = 0;
		m_nImageSize = 0;
		m_pImage = NULL;

		*this = ItemData;
	}
	LRMenuItemData& operator=( LRMenuItemData& ItemData ){
		m_sCaption = ItemData.m_sCaption;
		m_uiWidth = ItemData.m_uiWidth;
		m_uiHeight = ItemData.m_uiHeight;
		m_bVisible = ItemData.m_bVisible;
		SetImage(ItemData.m_nImageType, ItemData.m_nImageSize, ItemData.m_pImage);
		return *this;
	}
	virtual ~LRMenuItemData( );

	// Item width/height
	virtual UINT GetWidth() { return m_uiWidth; }
	virtual UINT GetHeight() { return m_uiHeight; }
	virtual void SetWidth( UINT uiWidth ) { m_uiWidth = uiWidth; }
	virtual void SetHeight( UINT uiHeight ) { m_uiHeight = uiHeight; }

	// Item caption
	virtual void SetCaption( tstring sCaption ) { m_sCaption = sCaption; }
	virtual void SetCaptionToString( string sCaption );
	virtual tstring& GetCaption( ) { return m_sCaption; }
	virtual void GetStringCaption( string& sCaption );

	// Visible/Invisible item
	virtual void SetVisible( bool bVisible ) { m_bVisible = bVisible; }
	virtual bool GetVisible( ) { return m_bVisible; }

	virtual void SetImage( unsigned char ImageType,unsigned long ImageSize,const char *pImage );

};

/*
 *
 *	LRMenuItemData - menu item
 *
 */
class LRMenuItem: public LRMenuItemData {
protected:
	vector<LRMenuItem*> m_vMenuItems;
	LRMenuItem* m_pParent;
	UINT m_uiItemsHeight, m_uiItemsWidth;
	int m_iStart, m_iEnd, m_iHighlight;
	bool m_bDirty;
	Rect m_rSubmenu;	
	bool m_bHasScroll;

	Surface *m_pClone;
	void CalcSubmenuLayout( RECT rViewport );	
	map<TCHAR, int> m_mShotcuts;
	TCHAR m_ucShortcut;

	void AddShortcut( int iPos, TCHAR ucShortcut );	

	// method to override to do something on item select
	virtual bool DoAction( void ){ return false; }

	bool m_bIsShowing, m_bIsOnTop;	
	virtual void ShowSubmenu( bool bShow=true );
	
	void ClearSubmenu();

	Rect m_rClientRect;
	bool m_bPainting;

	bool BeginPaint( RECT r, bool bForceRepaint );
	bool EndPaint() { m_bDirty = false; m_bPainting = false; return true; }

	int m_iItemsCount;
	

public:	
	LRMenuItem( LRMenuItem* pParent=NULL ){ 
		m_pParent = pParent; 
		m_uiItemsHeight = m_uiItemsWidth = 0; 
		m_iEnd = m_iStart = 0;
		m_bDirty = true;
		m_bHasScroll = false;
		m_pClone = NULL;
		m_ucShortcut = 0;
		m_bIsShowing = m_bIsOnTop = false;
		m_iHighlight = -1;
		m_rClientRect = Rect(0,0,0,0);
		m_bPainting = false;
		m_iItemsCount = 0;
	}
	LRMenuItem( LRMenuItemData& ItemData, LRMenuItem* pParent=NULL )
		:LRMenuItemData(ItemData)
	{
		m_uiItemsHeight = m_uiItemsWidth = 0;
		m_pParent = pParent;
		m_iEnd = m_iStart = 0;
		m_bDirty = true;
		m_bHasScroll = false;
		m_pClone = NULL;
		m_ucShortcut = 0;
		m_bIsShowing = m_bIsOnTop = false;
		m_iHighlight = -1;
		m_rClientRect = Rect(0,0,0,0);
		m_bPainting = false;
		m_iItemsCount = 0;
	}
	virtual ~LRMenuItem();

	// MArk all items in submenu as bDirty
	void MakeSubmenuDirty( bool bDirty = true );

	// Paint item
	virtual void Paint( RECT r, bool bHighlight, bool bForceRepaint=false );
	
	/* To DO: When Item heights are not all the same and the menu doesn't fit inside the viewport ( has scrollbar )
				there should be some drawing glitches at the bottom of the menu ( as the menu height changes )
	*/
	virtual void PaintSubmenu( bool bForceRepaint=false );

	// Move cursor up/down inside submenu
	virtual void MoveCursor( int iDir );

	// Show Submenu
	virtual void Show( bool bForceRepaint=false ) {
		ShowSubmenu( true );
		PaintSubmenu( bForceRepaint );
		m_bIsOnTop = true;
	}

	// Hide submenu
	virtual void Hide( ){
		ShowSubmenu( false );
		m_bIsOnTop = false;
	}

	// Repaint submenu
	virtual void BringToFront( ){
		if ( m_bIsShowing )	{
			PaintSubmenu( );
			m_bIsOnTop = true;
		}

	}

	// Execute - launches DoAction
	virtual bool Exec();

	// Delete and Remove  submenu items
	virtual void Clear( void ) { ClearSubmenu(); }

	//Add new submenu item
	virtual LRMenuItem* AddItem( LRMenuItemData& ItemData );
	virtual void AddItem( LRMenuItem* pMenuItem );
	virtual void InsertItem( unsigned int uiWhere, LRMenuItem* pMenuItem );

	//Get current selected submenu item
	virtual LRMenuItem* GetCrtItem( void ) { 
		if ( m_iHighlight>=0 )	return m_vMenuItems[m_iHighlight]; 
		return NULL;
	}

	// Calculate Submenu height/width
	virtual UINT GetSubmenuHeight( int iLimit=-1 );
	virtual UINT GetSubmenuWidth( void );

	// if Dirty it'll be redraw on submenu repaint
	virtual void SetDirty( bool bDirty = true ) { m_bDirty = bDirty; }
	virtual bool IsDirty() { return m_bDirty; }	
	
	// Submenu position/rect
	virtual void GetSubmenuRect( RECT& r ){	r = m_rSubmenu; }
	virtual void SetSubmenuX( int iX ) { m_rClientRect.left = m_rClientRect.right = iX; }
	virtual void SetSubmenuY( int iY ) { m_rClientRect.top = m_rClientRect.bottom = iY; }
	virtual int GetSubmenuX() { return ( m_rClientRect.left + m_rClientRect.right ) / 2; }
	virtual int GetSubmenuY() { return ( m_rClientRect.top + m_rClientRect.bottom ) / 2; }

	// Menu item shortcut
	virtual void SetShortcut( TCHAR ucShortcut ){ m_ucShortcut = ucShortcut; }
	//Search submenu for shortcut
	virtual bool SelectItem( TCHAR ucShortcut );
	virtual bool SelectItem( int iX, int iY );

	// Check for submenu
	virtual bool HasSubMenu( void );

	// Menu item parent
	virtual void SetParent( LRMenuItem* pParent ) { m_pParent = pParent; }
	virtual LRMenuItem* GetParent() { return m_pParent; }

	// Menu item changed
	virtual bool Changed() { 
		return ( !m_uiItemsHeight || !m_uiItemsWidth ); 
	}

	// Set item caption
	virtual void SetCaption( tstring sCaption ) { 
		SetDirty();
		LRMenuItemData::SetCaption( sCaption ); 
	}
	virtual void SetCaptionToString( string sCaption ) {
		SetDirty();
		LRMenuItemData::SetCaptionToString( sCaption ); 
	} 	

	// returns false, as this is a real menu item
	virtual bool IsLink() { return false; }

	virtual bool PointInSubmenu( int iX, int iY );
	virtual bool PointIn( int iX, int iY );	

	virtual bool HasScroll() { return m_bHasScroll; }

};

/*
 *
 *	LRMenuItemData - fake menu item ( link to a real menu item )
 *		 all methods are delegated to the real item except for GetVisible and IsLink
 *       which always return true 
 *
 */
class LRMenuItemLink: public LRMenuItem {
protected:	
	LRMenuItem* m_pOrigItem;
	LRMenuItemLink( LRMenuItem* pParent=NULL ){}
	LRMenuItemLink( LRMenuItemData& ItemData, LRMenuItem* pParent=NULL ){}	

public:
	LRMenuItemLink( LRMenuItem* pItem, LRMenuItem* pParent=NULL ){ m_pOrigItem = pItem; m_pParent=pParent; }
	virtual ~LRMenuItemLink(){ m_pOrigItem = NULL; }
	LRMenuItem* GetOrigItem() { return m_pOrigItem; }

	virtual UINT GetWidth() { return m_pOrigItem->GetWidth(); }
	virtual UINT GetHeight() { return m_pOrigItem->GetHeight(); }
	virtual void SetWidth( UINT uiWidth ) { m_pOrigItem->SetWidth( uiWidth ); }
	virtual void SetHeight( UINT uiHeight ) { m_pOrigItem->SetHeight( uiHeight ); }
	virtual void SetCaption( tstring sCaption ) { m_pOrigItem->SetCaption( sCaption ); }
	virtual void SetCaptionToString( string sCaption ) { m_pOrigItem->SetCaptionToString( sCaption ); }
	virtual tstring& GetCaption( ) { return m_pOrigItem->GetCaption(); }
	virtual void GetStringCaption( string& sCaption ) { m_pOrigItem->GetStringCaption( sCaption ); }
	virtual void SetVisible( bool bVisible ) { m_pOrigItem->SetVisible( bVisible ); }
	virtual bool GetVisible( ) { return true; } // The only one changed


	virtual void Clear( void ) { m_pOrigItem->Clear(); }
	virtual void Paint( RECT r, bool bHighlight, bool bForceRepaint=false ) { m_pOrigItem->Paint( r, bHighlight, bForceRepaint ); }
	virtual void PaintSubmenu( bool bForceRepaint=false ) { m_pOrigItem->PaintSubmenu( bForceRepaint ); }
	virtual void MoveCursor( int iDir ) { m_pOrigItem->MoveCursor( iDir ); }
	virtual void Show( bool bForceRepaint=false ) { m_pOrigItem->Show( bForceRepaint ); }
	virtual void Hide( ) { m_pOrigItem->Hide( ); }
	virtual void BringToFront( ) { m_pOrigItem->BringToFront( ); }
	virtual bool Exec() { return m_pOrigItem->Exec(); }
	virtual LRMenuItem* AddItem( LRMenuItemData& ItemData ) { return m_pOrigItem->AddItem( ItemData ); }
	virtual void AddItem( LRMenuItem* pMenuItem ) { m_pOrigItem->AddItem( pMenuItem ); }
	virtual void InsertItem( unsigned int uiWhere, LRMenuItem* pMenuItem ) { m_pOrigItem->InsertItem( uiWhere, pMenuItem ); }
	virtual LRMenuItem* GetCrtItem( void ) { return m_pOrigItem->GetCrtItem(); }
	virtual UINT GetSubmenuHeight( int iLimit=-1 )  { return m_pOrigItem->GetSubmenuHeight( iLimit ); }
	virtual UINT GetSubmenuWidth( void ) { return m_pOrigItem->GetSubmenuWidth(); }
	virtual void SetDirty( bool bDirty = true ) { m_pOrigItem->SetDirty( bDirty ); }
	virtual bool IsDirty() { return m_pOrigItem->IsDirty(); }	
	virtual void GetSubmenuRect( RECT& r ){ m_pOrigItem->GetSubmenuRect( r ); }
	virtual void SetSubmenuX( int iX ) { m_pOrigItem->SetSubmenuX( iX ); }
	virtual void SetSubmenuY( int iY ) { m_pOrigItem->SetSubmenuY( iY ); }
	virtual int GetSubmenuX() { return m_pOrigItem->GetSubmenuX(); }
	virtual int GetSubmenuY() { return m_pOrigItem->GetSubmenuY(); }
	virtual void SetShortcut( TCHAR ucShortcut ) { m_pOrigItem->SetShortcut( ucShortcut); }
	virtual bool HasSubMenu( void ) { return m_pOrigItem->HasSubMenu(); }
	virtual bool SelectItem( TCHAR ucShortcut ) { return m_pOrigItem->SelectItem( ucShortcut ); } 
	virtual bool SelectItem( int iX, int iY ) { return m_pOrigItem->SelectItem( iX, iY ); } 

	virtual bool Changed() { return m_pOrigItem->Changed(); }
	virtual bool IsLink() { return true; }

	virtual bool PointInSubmenu( int iX, int iY ){ return m_pOrigItem->PointInSubmenu( iX, iY ); }
	virtual bool PointIn( int iX, int iY ) { return m_pOrigItem->PointIn( iX, iY ); }
	virtual void SetImage( unsigned char ImageType,unsigned long ImageSize,const char *pImage ) {
		m_pOrigItem->SetImage( ImageType, ImageSize, pImage );
	}

	virtual bool HasScroll() { return m_pOrigItem->HasScroll(); }

	// GetParent/SetParent won't be rewritten
	//virtual void SetParent( LRMenuItem* pParent ) { m_pParent = pParent; }
	//virtual LRMenuItem* GetParent() { return m_pParent; }

};

/*
 *
 *	LRMenu - main menu
 *
 */
class LRMenu {
protected:
	UINT m_uiX, m_uiY;
	LRMenuItem* m_pMenuRoot, *m_pDisplayMenu;	
	bool m_bIsShowing;
	bool m_bChanging;

	virtual void CreateMenuRoot() { 
		m_pMenuRoot = new LRMenuItem; 
		m_pDisplayMenu = m_pMenuRoot;
		m_bIsShowing = false;
	}

public:
	LRMenu();
	~LRMenu();

	// Add menu item
	LRMenuItem* AddItem( LRMenuItemData& ItemData );
	virtual void AddItem( LRMenuItem* pMenuItem );

	// Begin/End items list change - menu won't draw while items list is changing
	void BeginUpdate() { m_bChanging = true; }
	void EndUpdate() { m_bChanging = false; }

	//Show/Hide main menu
	void Show( int iX, int iY );
	void Hide();

	//(Force)Paint menu - bFull==true, repaint all opened submenus
	void Paint( bool bFull = false );	

	
	//Translates Pluto keys to Menu commans
	bool HandleKeys( int nPK_Button, bool bKeyUp );	
	// implementing menu commands
	bool KeyPress( uchar ucKey );
	//Handle stylus events
	bool HandleStylus( int iX, int iY );

	// Main menu is active
	bool IsShowing() { return m_bIsShowing; }

	//Menu viewport
	void SetViewport( RECT rViewport ) { LocalRenderer::SetViewport( Rect(rViewport) );  } //repaint

	void SetDirty( bool bDirty = true );
};





#endif

