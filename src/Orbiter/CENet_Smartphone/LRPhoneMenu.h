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
#ifndef _LRPHONEMENU_H_
#define _LRPHONEMENU_H_

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
#include "LRMenu.h"

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
#define DEFAULT_SYSTEM_CLOSE_CAPTION _T("Quit")
#define DEFAULT_SYSTEM_MIN_CAPTION _T("Minimize")
#define DEFAULT_SYSTEM_ITEM_HEIGHT 17

#define DRAW_SHORTCUT_COLOR  RGB(255,136,136)
#define DRAW_SHORTCUT_WIDTH  10

#define DRAW_SUBMENUMARK_WIDTH 5
#define DRAW_SUBMENUMARK_HEIGHT 10
#define DRAW_SUBMENUMARK_COLOR RGB(0,0,0)

#define DRAW_IMAGE_WIDTH 18
#define DRAW_IMAGE_HEIGHT 18


#define DEFAULT_GETIMAGE_TIMEOUT	10000	//milliseconds
//---------------------------------------------------------------------------------------------------------

/*
 *
 *	LRPhoneMenu - Phone menu
 *
 */
class LRCrtRoomItem;
class LRPhoneMenuItemRoot;
class LRPhoneMenuItem;
class LRPhoneMenu: public LRMenu {
protected:
	LRCrtRoomItem* m_pCrtRoomItem;
private:
	LRPhoneMenu(){m_pCrtRoomItem = NULL;}
public:
	typedef enum { tbcFore=0, tbcBack, tbcHlFore, tbcHlBack, tbcNone } TextBitsColor;
protected:
	static COLORREF m_clTextColor[ tbcNone ];	
	virtual void CreateMenuRoot();
		
public:		
	// Constructor blocked so we can use CreateMenuRoot with LRPhoneMenuItemRoot
	static LRPhoneMenu* Create();

	// Set/Get (highlighted/foreground/background)items color
	static void SetColor( TextBitsColor ColorIdx, COLORREF Color ){
		if ( ColorIdx<tbcNone )	m_clTextColor[ColorIdx] = Color;
	}
	static COLORREF GetColor( TextBitsColor ColorIdx ){
		if ( ColorIdx==tbcNone ) return RGB(0,0,0);
		else return m_clTextColor[ColorIdx];
	}

	// rewrite AddItem to set m_pCrtRoomItem
	virtual void AddItem( LRMenuItem* pMenuItem );

	// Set current room
	void SetCrtRoom( long nCrtRoom );

	// Set menu items images 
	void SetMenuImages( MenuItemInfo::ItemType nItemsType, vector<MenuItemInfo*>& vItems );

	// Find item
	LRPhoneMenuItem* FindItem( MenuItemInfo::ItemType nItemsType, MenuItemInfo& ItemInfo );

	//
	// static implementation of a items queue for GetMenuImages command
	//	
	// queue
	static vector<MenuItemInfo> m_vQueuedItems;
protected:
	// if not enabled items are not added
	static bool m_bEnabled;
public:
	static bool QueueEnabled() { return m_bEnabled; }
	static void SetQueueEnabled( bool bEnabled = true ) { m_bEnabled = bEnabled; }

	// add item to queue
	static void QueueItem( MenuItemInfo ItemInfo );	
	// send command GetMenuImages
	static void GetMenuImages( MenuItemInfo::ItemType nItemsType );
};


/*
 *
 *	LRPhoneMenuItem - base class for phone menu items
 *
 */
class LRPhoneMenuItem : public LRMenuItem {
protected:
	bool m_bStyleBold;	
	bool m_bHasPicture;
	bool m_bAskedForPictures;	
	DWORD m_dwAskedTime;
	MenuItemInfo::ItemType m_ItemType, m_SubmenuType;

	// checks for image and adds item to queue if it doesn't have one
	virtual void CheckForImage( void );
public:
	LRPhoneMenuItem( LRMenuItem* pParent=NULL ) { 
		m_bStyleBold = false;
		m_bHasPicture = true;
		m_bAskedForPictures = false;
		m_dwAskedTime = 0;
		m_ItemType = m_SubmenuType = MenuItemInfo::stNone;
	}
	LRPhoneMenuItem( LRMenuItemData& ItemData, LRMenuItem* pParent=NULL ){
		m_bStyleBold = false;
		m_bHasPicture = true;
		m_bAskedForPictures = false;
		m_dwAskedTime = 0;
		m_ItemType = m_SubmenuType = MenuItemInfo::stNone;
	}
	virtual bool operator==( MenuItemInfo& ItemInfo ) { return false; }

	// reimplemented to draw phone menu items using received styles
	virtual void Paint( RECT r, bool bHighlight, bool bForceRepaint=false );

	// reimplemented too add items to get image queue
	virtual void PaintSubmenu( bool bForceRepaint=false );

	// created a iteminfo for the item
	virtual MenuItemInfo GetItemInfo( void ) { return MenuItemInfo(); }
	
	// submenu item types
	MenuItemInfo::ItemType GetSubmenuType(){ return m_SubmenuType; }
	// item type
	MenuItemInfo::ItemType GetItemType(){ return m_ItemType; }

	// finds a item having ItemInfo
	virtual LRPhoneMenuItem* FindItem( MenuItemInfo::ItemType nItemsType, MenuItemInfo& ItemInfo );	

	// Sets item info for an item - in fact it just sets the image received
	virtual bool SetItemInfo( MenuItemInfo::ItemType nItemsType, MenuItemInfo& ItemInfo );
};

/*
 *
 *	LRPhoneMenuItemRoot - phone menu root item
 *
 */
class LRPhoneMenuItemRoot : public LRPhoneMenuItem {
protected:
public:
	//Remove links to scenarios
	LRPhoneMenuItemRoot( LRMenuItem* pParent=NULL ) { 
		m_SubmenuType = MenuItemInfo::stScenarios;
	}
	LRPhoneMenuItemRoot( LRMenuItemData& ItemData, LRMenuItem* pParent=NULL ){
		m_SubmenuType = MenuItemInfo::stScenarios;
	}	

	// custom function to remove links to scenario items
	void RemoveLinks( void );

	virtual void PaintSubmenu( bool bForceRepaint=false );

	void ResetQueryImages() { m_bAskedForPictures = false; } 
};

/*
 *
 *	LRCrtRoomItem - Current room item ( first item in menu )
 *
 */
class LRCrtRoomItem : public LRPhoneMenuItem {
public:
	LRCrtRoomItem( LRMenuItem* pParent=NULL ) { 
		m_bStyleBold = true;
		m_bHasPicture = false;
		m_SubmenuType = MenuItemInfo::stRooms;
	}
	LRCrtRoomItem( LRMenuItemData& ItemData, LRMenuItem* pParent=NULL ){
		m_bStyleBold = true;
		m_bHasPicture = false;
		m_SubmenuType = MenuItemInfo::stRooms;
	}	
	
	// Select current room
	void SelectRoom( int iRoomId );

	// reimplemented do draw current room item in a different way
	virtual void Paint( RECT r, bool bHighlight, bool bForceRepaint=false );

	
};

/*
 *
 *	LRRoomItem - Rooms items
 *
 */
class LRRoomItem : public LRPhoneMenuItem {
protected:
	int m_iRoomId;

	// reimplemented to add links in root menu when choosing a room
	virtual bool DoAction( void );

public:
	LRRoomItem( LRMenuItem* pParent=NULL ):LRPhoneMenuItem(pParent) { 
		m_iRoomId = -1;
		m_ItemType = MenuItemInfo::stRooms;
		m_SubmenuType = MenuItemInfo::stScenarios;
	}
	LRRoomItem( LRMenuItemData& ItemData, LRMenuItem* pParent=NULL ):LRPhoneMenuItem( ItemData, pParent ){		
		m_iRoomId = -1;
		m_ItemType = MenuItemInfo::stRooms;
		m_SubmenuType = MenuItemInfo::stScenarios;
	}

	virtual bool operator==( MenuItemInfo& ItemInfo );

	// All Scenarios are invisible - they'll be linked in root menu
	virtual void AddItem( LRMenuItem* pMenuItem ) { 		
		LRMenuItem::AddItem( pMenuItem ); 		
		pMenuItem->SetVisible( false );
	}
	virtual LRMenuItem* AddItem( LRMenuItemData& ItemData ) { 
		LRMenuItem* pItem = LRMenuItem::AddItem( ItemData ); 
		pItem->SetVisible( false );
		return pItem;
	}

	// Room Id - for selecting current room
	void SetRoomId( int iRoomId ) { m_iRoomId = iRoomId; }
	int  GetRoomId() { return m_iRoomId; }

	// Create links to scenarios in root menu
	void CreateLinks( void );

	virtual MenuItemInfo GetItemInfo( void ){
		MenuItemInfo ItemInfo;
		ItemInfo.m_nRoomId = m_iRoomId;
		return ItemInfo;
	}
	
};

/*
 *
 *	LRScenarioItem - Scenario items
 *
 */
class LRScenarioItem : public LRPhoneMenuItem {
protected:
	virtual void CheckForImage( void );
public:
	LRScenarioItem( LRMenuItem* pParent=NULL ) { 
		m_ItemType = MenuItemInfo::stScenarios;
		m_SubmenuType = MenuItemInfo::stSubScenarios;
	}
	LRScenarioItem( LRMenuItemData& ItemData, LRMenuItem* pParent=NULL ){		
		m_ItemType = MenuItemInfo::stScenarios;
		m_SubmenuType = MenuItemInfo::stSubScenarios;
	}
	
	virtual MenuItemInfo GetItemInfo( void )
	{
		MenuItemInfo ItemInfo;
		string sCaption;
		GetStringCaption( sCaption );
		ItemInfo.m_sScenName = sCaption;
		return ItemInfo;
	}
	
	virtual bool operator==( MenuItemInfo& ItemInfo );


};

/*
 *
 *	LRSubScenarioItem - SubScenario items
 *
 */
class LRSubScenarioItem : public LRPhoneMenuItem {
protected:
	string m_sId;

	// reimplemented to send SelectedItem on choosing a SubScenario
	virtual bool DoAction( void );

public:		
	LRSubScenarioItem( LRMenuItem* pParent=NULL ) { 
		m_sId = "";
		m_ItemType = MenuItemInfo::stSubScenarios;
	}
	LRSubScenarioItem( LRMenuItemData& ItemData, LRMenuItem* pParent=NULL ){
		m_sId = "";
		m_ItemType = MenuItemInfo::stSubScenarios;
	}

	virtual bool operator==( MenuItemInfo& ItemInfo );

	// SubScenario Id - sent to SelectedItem
	void SetId( string sId ) { m_sId = sId; }
	string& GetId( ) { return m_sId; }

	virtual MenuItemInfo GetItemInfo( void ){
		MenuItemInfo ItemInfo;
		ItemInfo.m_sObjectId = m_sId;
		ItemInfo.m_nRoomId = ((LRRoomItem*)(m_pParent->GetParent()))->GetRoomId();
		return ItemInfo;
	}
	
};

/*
 *
 *	LRSystemItem - System items: Minimize, Close
 *
 */
const LPCTSTR DefaultSysItemText[] = { DEFAULT_SYSTEM_CLOSE_CAPTION, DEFAULT_SYSTEM_MIN_CAPTION };
class LRSystemItem : public LRPhoneMenuItem {
public:
	typedef enum { sitSysClose = 0, sitMinimize } SystemItemType;
protected:
	virtual bool DoAction( void );
	int m_SysItemType;
public:	
	LRSystemItem( SystemItemType SysItemType, LRMenuItem* pParent=NULL ) 
		:LRPhoneMenuItem( pParent )
	{ 
		m_SysItemType = SysItemType;
		m_sCaption = DefaultSysItemText[m_SysItemType];
		m_uiHeight = DEFAULT_SYSTEM_ITEM_HEIGHT;
	}
	LRSystemItem( SystemItemType SysItemType, LRMenuItemData& ItemData, LRMenuItem* pParent=NULL )
		:LRPhoneMenuItem( ItemData, pParent )
	{
		m_SysItemType = SysItemType;
		m_sCaption = DefaultSysItemText[m_SysItemType];
		m_uiHeight = DEFAULT_SYSTEM_ITEM_HEIGHT;
	}

	virtual void Paint( RECT r, bool bHighlight, bool bForceRepaint=false );
};


#endif

