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
#include "MenuItemInfo.h"
#include <algorithm>
#include "PlutoUtils/PlutoDefs.h"

#ifdef SMARTPHONE
	#include "LRMenu.h"
#endif

//---------------------------------------------------------------------------------------------------------
/*
 *
 *	class MenuItemInfo
 *
 */
//---------------------------------------------------------------------------------------------------------

MenuItemInfo& MenuItemInfo::operator=( const MenuItemInfo& Item )
{
	m_sScenName = Item.m_sScenName;
	m_nRoomId = Item.m_nRoomId;
	m_sObjectId = Item.m_sObjectId;

	m_nImageType = Item.m_nImageType;

    m_nImageSize=Item.m_nImageSize;
	PLUTO_SAFE_DELETE( m_pImage );
	if ( NULL != Item.m_pImage ) {
		m_pImage = new char[m_nImageSize];
		memcpy(m_pImage, Item.m_pImage, m_nImageSize);
	}

	return *this;
}
//---------------------------------------------------------------------------------------------------------
MenuItemInfo::~MenuItemInfo()
{
	PLUTO_SAFE_DELETE( m_pImage );
	m_nImageType = 0;
    m_nImageSize = 0;
}
//---------------------------------------------------------------------------------------------------------
void MenuItemInfo::write( unsigned char nItemType, SerializeClass &sc )
{
	switch( nItemType ){
		case stRooms:
			sc.Write_long( m_nRoomId );
			break;
		case stScenarios:
			sc.Write_string( m_sScenName );	
			break;
		case stSubScenarios:
			sc.Write_long( m_nRoomId );
			sc.Write_string( m_sObjectId );
			break;
	};

	#ifdef BLUETOOTH_DONGLE
		if ( NULL != m_pImage ) {
			sc.Write_unsigned_char(m_nImageType);
			sc.Write_long(m_nImageSize);
			sc.Write_block(m_pImage, m_nImageSize);
		}
	#endif
}
//---------------------------------------------------------------------------------------------------------
void MenuItemInfo::read( unsigned char nItemType, SerializeClass &sc )
{
	switch( nItemType ){
		case stRooms:
			m_nRoomId = sc.Read_long( );
			break;
		case stScenarios:
			sc.Read_string( m_sScenName );	
			break;
		case stSubScenarios:
			m_nRoomId = sc.Read_long( );
			sc.Read_string( m_sObjectId );
			break;
	};
	#ifdef SMARTPHONE
		m_nImageType = sc.Read_unsigned_char();
		m_nImageSize = sc.Read_long();
		m_pImage = sc.Read_block(m_nImageSize);
	#endif
}
//---------------------------------------------------------------------------------------------------------
void MenuItemInfo::SetImage( unsigned char ImageType,unsigned long ImageSize,const char *pImage )
{
	m_nImageType = ImageType;

    m_nImageSize = ImageSize;
	PLUTO_SAFE_DELETE( m_pImage );
	if ( NULL!=pImage ){
		m_pImage = new char[m_nImageSize];
		memcpy(m_pImage, pImage, m_nImageSize);
	}
}


