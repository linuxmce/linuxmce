/*
 BD_CP_SetMenuImages
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

#ifdef VIPPHONE
#ifdef VIPDESIGN
#include "stdafx.h"
#include "VIPDesign/VIPDesignDoc.h"
#include "VIPShared/PlutoConfig.h"
#endif
#else
#ifndef SYMBIAN
#include <iostream>
#include <fstream>
#endif
#endif

#ifndef SYMBIAN
#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/PlutoDefs.h"
#include "BD/BDCommandProcessor.h"
#else
#include "VIPShared/VIPIncludes.h"
#include "PlutoMOAppUi.h"
#include "Logger.h"
#endif

#ifdef SMARTPHONE
#include "OrbiterApp.h"
#endif

#include "PlutoUtils/MyStl.h"
#include "BD_CP_SetMenuImages.h"
#include "BD_PC_GetSignalStrength.h"

BD_CP_SetMenuImages::BD_CP_SetMenuImages( MenuItemInfo::ItemType nItemsType, vector<MenuItemInfo>& vItems ) 
{
	m_nItemsType = nItemsType;
	
	MenuItemInfo *pItem = NULL;
	for ( vector<MenuItemInfo>::iterator iter=vItems.begin(); iter!=vItems.end() ; ++iter ){
		pItem = new MenuItemInfo( *iter );
		m_vItems.push_back( pItem );
	}
}

void BD_CP_SetMenuImages::ClearItems()
{
	for ( vector<MenuItemInfo*>::iterator iter=m_vItems.begin(); iter!=m_vItems.end() ; ++iter ){
		PLUTO_SAFE_DELETE( *iter );
	}
}


BD_CP_SetMenuImages::~BD_CP_SetMenuImages()
{
	ClearItems();
}

void BD_CP_SetMenuImages::ConvertCommandToBinary()
{
	BDCommand::ConvertCommandToBinary();
	Write_unsigned_char( m_nItemsType );
	Write_long( (long)m_vItems.size() );
	for (vector<MenuItemInfo*>::iterator iter=m_vItems.begin(); iter!=m_vItems.end(); ++iter ) {
		(*iter)->write( m_nItemsType, *this );
	}
}

void BD_CP_SetMenuImages::ParseCommand(unsigned long size,const char *data)
{
#ifdef SYMBIAN
    LOG("#	BD_CP_SetMenuImages::ParseCommand  #\n");
#endif

	BDCommand::ParseCommand(size, data);
	#ifdef SMARTPHONE
		ClearItems();

		m_nItemsType = (MenuItemInfo::ItemType)Read_unsigned_char( );
		long nSize = Read_long( );		
		for ( long i=0; i<nSize; ++i ){
			MenuItemInfo* pItem = new MenuItemInfo;
			pItem->read( m_nItemsType, *this );
			m_vItems.push_back( pItem );
		}
	#endif
}

bool BD_CP_SetMenuImages::ProcessCommand(BDCommandProcessor *pProcessor)
{
#ifdef VIPPHONE
#ifdef SYMBIAN
	 LOG("#	Received 'SetMenuImages' command  #\n");
#endif //SYMBIAN

#ifdef SMARTPHONE
	 OrbiterApp::GetInstance()->SetMenuImages( m_nItemsType, m_vItems );
#endif

#ifdef VIPDESIGN
#endif //VIPDESIGN

#endif //VIPPHONE

	return true;
}
