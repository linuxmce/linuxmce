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
#ifndef _MENU_ITEMINFO_H_
#define _MENU_ITEMINFO_H_

//---------------------------------------------------------------------------------------------------------
#include <string>
using namespace std;
//---------------------------------------------------------------------------------------------------------
#include "../SerializeClass/SerializeClass.h"
//---------------------------------------------------------------------------------------------------------


class MenuItemInfo {
public:
	unsigned char m_nImageType;
	char *m_pImage;
	unsigned long m_nImageSize;
	string m_sScenName;
	int m_nRoomId;
	string m_sObjectId;

	typedef enum { stNone=0, stRooms, stScenarios, stSubScenarios } ItemType;

	MenuItemInfo(){
		m_nImageType = 0;
		m_pImage = NULL;
		m_nImageSize = 0;
	}
	MenuItemInfo( const MenuItemInfo& Item ) { 
		m_nImageType = 0;
		m_pImage = NULL;
		m_nImageSize = 0;
		*this = Item; 
	}
	MenuItemInfo& operator=( const MenuItemInfo& Item );
	~MenuItemInfo();

	void write( unsigned char nItemType, SerializeClass &sc );
	void read( unsigned char nItemType, SerializeClass &sc );
	
	void SetImage( unsigned char ImageType,unsigned long ImageSize,const char *pImage );
};


#endif.
