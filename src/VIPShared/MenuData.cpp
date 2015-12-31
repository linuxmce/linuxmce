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
#include "MenuData.h"
#include <algorithm>
#include "PlutoUtils/PlutoDefs.h"

#include "DCE/Logger.h"

using namespace DCE;

#ifdef SMARTPHONE
	#include "LRMenu.h"
	#include "LRPhoneMenu.h"
#endif

//---------------------------------------------------------------------------------------------------------
/*
 *
 *	class MD_SubScenario
 *
 */
//---------------------------------------------------------------------------------------------------------
void MD_SubScenario::Write( SerializeClass& sc )
{
	sc.Write_string( m_sSubScenId );
	sc.Write_string( m_sSubScenName );
}
//---------------------------------------------------------------------------------------------------------
void MD_SubScenario::Read( SerializeClass& sc )
{
	sc.Read_string( m_sSubScenId );
	sc.Read_string( m_sSubScenName );
}
//---------------------------------------------------------------------------------------------------------
MD_SubScenario* MD_Scenario::GetSubScenario( string& sSubScenarioId )
{
	vector<MD_SubScenario>::iterator iter = ( std::find( m_vSubScenarios.begin(), 
		m_vSubScenarios.end(), MD_SubScenario( sSubScenarioId ) ) );
	if ( iter==m_vSubScenarios.end() ) return NULL;
	return &(*iter);
}

//---------------------------------------------------------------------------------------------------------
/*
 *
 *	class MD_Scenario
 *
 */
//---------------------------------------------------------------------------------------------------------
void MD_Scenario::Write( SerializeClass& sc )
{
	sc.Write_string( m_sScenarioName );
	sc.Write_long( (long)(m_vSubScenarios.size()) );
	for ( vector<MD_SubScenario>::iterator iter=m_vSubScenarios.begin(); iter!=m_vSubScenarios.end(); ++iter ){
		iter->Write( sc );
	}
}
//---------------------------------------------------------------------------------------------------------
void MD_Scenario::Read( SerializeClass& sc )
{
	sc.Read_string( m_sScenarioName );
	long lSize = sc.Read_long();
	for ( long i=0; i<lSize; ++i ){
		MD_SubScenario subscen;
		subscen.Read( sc );
		AddSubScenario( subscen );
	}
}
//---------------------------------------------------------------------------------------------------------
#ifdef SMARTPHONE
void MD_Scenario::CreateMenu( LRMenuItem* pRoot )
{
	for ( vector<MD_SubScenario>::iterator iter=m_vSubScenarios.begin(); iter!=m_vSubScenarios.end(); ++iter ){
		LRSubScenarioItem *pSubSceneItem = new LRSubScenarioItem;
		pSubSceneItem->SetCaptionToString( iter->GetName() );
		pSubSceneItem->SetId( iter->GetId() );
		pRoot->AddItem( pSubSceneItem );
	}
}
#endif

//---------------------------------------------------------------------------------------------------------
/*
 *
 *	class MD_Room
 *
 */
//---------------------------------------------------------------------------------------------------------
MD_Scenario* MD_Room::GetScenario( string sScenarioName )
{
	vector<MD_Scenario>::iterator iter = ( std::find( m_vScenarios.begin(), 
		m_vScenarios.end(), MD_Scenario( sScenarioName ) ) );
	if ( iter==m_vScenarios.end() ) return NULL;
	return &(*iter);
}
//---------------------------------------------------------------------------------------------------------
void MD_Room::Write( SerializeClass& sc )
{	
	sc.Write_long( (long)(m_iRoomId) );
	sc.Write_string( m_sRoomName );
	sc.Write_long( (long)(m_vScenarios.size()) );
	for ( vector<MD_Scenario>::iterator iter=m_vScenarios.begin(); iter!=m_vScenarios.end(); ++iter ){
		iter->Write( sc );
	}
}
//---------------------------------------------------------------------------------------------------------
void MD_Room::Read( SerializeClass& sc )
{
	m_iRoomId = sc.Read_long( );
	sc.Read_string( m_sRoomName );	
	long lSize = sc.Read_long();
	for ( long i=0; i<lSize; ++i ){
		MD_Scenario scen;
		scen.Read( sc );
		AddScenario( scen );
	}	
}
//---------------------------------------------------------------------------------------------------------
#ifdef SMARTPHONE
void MD_Room::CreateMenu( LRMenuItem* pRoot )
{
	for ( vector<MD_Scenario>::iterator iter=m_vScenarios.begin(); iter!=m_vScenarios.end(); ++iter ){
		LRScenarioItem* pScenItem = new LRScenarioItem;
		pScenItem->SetCaptionToString( iter->GetName() );
		pRoot->AddItem( pScenItem );
		iter->CreateMenu( pScenItem );
	}
}
#endif

//---------------------------------------------------------------------------------------------------------
/*
 *
 *	class MenuData
 *
 */
//---------------------------------------------------------------------------------------------------------
void MenuData::Write( SerializeClass& sc ) 
{
	sc.Write_long( (long)(m_Rooms.size()) );

	for ( MD_Rooms::iterator iter = m_Rooms.begin(); iter!=m_Rooms.end(); ++iter ) {
		iter->Write( sc );
	}

	sc.Write_long( m_tsItemText.m_ForeColor.m_Value );
	sc.Write_long( m_tsItemText.m_BackColor.m_Value );
	sc.Write_long( m_tsHlItemText.m_ForeColor.m_Value );
	sc.Write_long( m_tsHlItemText.m_BackColor.m_Value );

	sc.Write_long( (long)m_iCrtRoom );
}
//---------------------------------------------------------------------------------------------------------
void MenuData::Read( SerializeClass& sc ) 
{
	unsigned long ulRooms = sc.Read_long();
			
	for ( size_t iRooms=0; iRooms<ulRooms; ++iRooms ){
		MD_Room room;
		room.Read( sc );
		AddRoom( room );
	}

	m_tsItemText.m_ForeColor.m_Value = sc.Read_long();
	m_tsItemText.m_BackColor.m_Value = sc.Read_long();
	m_tsHlItemText.m_ForeColor.m_Value = sc.Read_long();
	m_tsHlItemText.m_BackColor.m_Value = sc.Read_long();

	m_iCrtRoom = sc.Read_long();

}
//---------------------------------------------------------------------------------------------------------
MD_Room* MenuData::GetRoom( int iRoomId )
{
	MD_Rooms::iterator iter = ( std::find( m_Rooms.begin(), 
		m_Rooms.end(), MD_Room( iRoomId ) ) );
	if ( iter==m_Rooms.end() ) return NULL;
	return &(*iter);
}
//---------------------------------------------------------------------------------------------------------
void MenuData::AddRoom( int iRoomId, string sRoomName )
{
	MD_Room room( iRoomId, sRoomName );
	AddRoom( room );	
}
//---------------------------------------------------------------------------------------------------------
void MenuData::AddScenario( int iRoomId, string sScenarioName )
{
	MD_Scenario scen( sScenarioName );
	MD_Room* pRoom = GetRoom( iRoomId );
	if ( pRoom ) pRoom->AddScenario( scen );
}
//---------------------------------------------------------------------------------------------------------
void MenuData::AddSubScenario( int iRoomId, string sScenarioName, string& sSubId, string sSubName )
{
	MD_SubScenario subscen( sSubId, sSubName );
	MD_Room* pRoom = GetRoom( iRoomId );
	if ( pRoom ) {
		MD_Scenario *pScen = pRoom->GetScenario( sScenarioName );
		if (pScen) pScen->AddSubScenario( subscen );
	}	
}
//---------------------------------------------------------------------------------------------------------
#ifdef SMARTPHONE
LRMenu* MenuData::CreateMainMenu()
{
	LRMenu* pMainMenu = LRPhoneMenu::Create();

	pMainMenu->BeginUpdate();

	LRMenuItemData ItemData;
	LRCrtRoomItem *pCrtRoomItem = new LRCrtRoomItem;
	pCrtRoomItem->SetCaptionToString( DEFAULT_CRTROOMITEM_CAPTION );
	pMainMenu->AddItem( pCrtRoomItem );
	for ( MD_Rooms::iterator iterRoom=m_Rooms.begin(); iterRoom!=m_Rooms.end(); ++iterRoom ) {
		LRRoomItem* pItem = new LRRoomItem;
		pItem->SetCaptionToString( iterRoom->GetName() );
		pItem->SetRoomId( iterRoom->GetId() );
		pCrtRoomItem->AddItem( pItem );
		iterRoom->CreateMenu( pItem );
	}
	LRSystemItem* pSysItem = new LRSystemItem( LRSystemItem::sitMinimize );
	pMainMenu->AddItem( pSysItem );
	pSysItem = new LRSystemItem( LRSystemItem::sitSysClose );		
	pMainMenu->AddItem( pSysItem );

	pCrtRoomItem->SelectRoom( m_iCrtRoom );	

	LRPhoneMenu::SetColor( LRPhoneMenu::tbcFore, RGB( m_tsItemText.m_ForeColor.R(), m_tsItemText.m_ForeColor.G(),
													  m_tsItemText.m_ForeColor.B() ) );
	LRPhoneMenu::SetColor( LRPhoneMenu::tbcBack, RGB( m_tsItemText.m_BackColor.R(), m_tsItemText.m_BackColor.G(),
													  m_tsItemText.m_BackColor.B() ) );
	LRPhoneMenu::SetColor( LRPhoneMenu::tbcHlFore, RGB( m_tsHlItemText.m_ForeColor.R(), m_tsHlItemText.m_ForeColor.G(),
													    m_tsHlItemText.m_ForeColor.B() ) );
	LRPhoneMenu::SetColor( LRPhoneMenu::tbcHlBack, RGB( m_tsHlItemText.m_BackColor.R(), m_tsHlItemText.m_BackColor.G(),
													    m_tsHlItemText.m_BackColor.B() ) );

	pMainMenu->EndUpdate();

	return pMainMenu;
}

#endif
