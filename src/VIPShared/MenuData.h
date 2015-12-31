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
#ifndef _MENU_DATA_H_
#define _MENU_DATA_H_

//---------------------------------------------------------------------------------------------------------
#include <map>
#include <vector>
#include <string>
using namespace std;
//---------------------------------------------------------------------------------------------------------
#include "../SerializeClass/SerializeClass.h"
#include "../Orbiter/TextStyle.h"
//---------------------------------------------------------------------------------------------------------
#ifdef SMARTPHONE	
	class LRMenu;
	class LRMenuItem;
#endif
//---------------------------------------------------------------------------------------------------------

/*
 *
 *	MD_SubScenario - SubScenario data for local rendered menus
 *
 */
class MD_SubScenario {
protected:
	string m_sSubScenId;
	string m_sSubScenName;
public:
	MD_SubScenario( string sSubScenId="", string sSubScenName="" ) {
		m_sSubScenId = sSubScenId;
		m_sSubScenName = sSubScenName;
	}
	~MD_SubScenario(){}
	MD_SubScenario( const MD_SubScenario& subscen ){ *this = subscen; }
	MD_SubScenario& operator=( const MD_SubScenario& subscen ){
		m_sSubScenId = subscen.m_sSubScenId;
		m_sSubScenName = subscen.m_sSubScenName;
		return *this;
	}
	// Compare subscenarios by id
	int operator==( const MD_SubScenario& subscen ){ return m_sSubScenId==subscen.m_sSubScenId; }

	// Serialization methods
	void Write( SerializeClass& sc );
	void Read( SerializeClass& sc );

	// Subscenario name
	string& GetName() { return m_sSubScenName; }
	// SubScenario id
	string& GetId() { return m_sSubScenId; }
};

/*
 *
 *	MD_Scenario - Scenario data for local rendered menus
 *
 */
class MD_Scenario {
protected:
	string m_sScenarioName;
	vector<MD_SubScenario> m_vSubScenarios;
public:
	MD_Scenario( string sScenarioName="" ) {
		m_sScenarioName = sScenarioName;
	}
	~MD_Scenario() {}
	MD_Scenario( const MD_Scenario& scen ) { *this = scen; }
	MD_Scenario& operator=( const MD_Scenario& scen ) {
		m_sScenarioName = scen.m_sScenarioName;
		m_vSubScenarios = scen.m_vSubScenarios;
		return *this;
	}
	//Compare scenarios by name
	int operator==( const MD_Scenario& scen ){ return m_sScenarioName==scen.m_sScenarioName; }


	//Add subscenario
	void AddSubScenario( MD_SubScenario& subscen ) { 
		m_vSubScenarios.push_back( subscen ); 
	}
	// Find subscenario by id
	MD_SubScenario* GetSubScenario( string& sSubScenarioId );

	//Serialization methods
	void Write( SerializeClass& sc );
	void Read( SerializeClass& sc );

	//Scenario name
	string& GetName() { return m_sScenarioName; }


	#ifdef SMARTPHONE
		// Create submenu items for scenario
		void CreateMenu( LRMenuItem* pRoot );
	#endif

};

/*
 *
 *	MD_Room - Room data for local rendered menus
 *
 */
class MD_Room {
protected:
	int m_iRoomId;
	string m_sRoomName;
	vector<MD_Scenario> m_vScenarios;
public:
	MD_Room( int iRoomId=0, string sRoomName="" ) {
		m_iRoomId = iRoomId;
		m_sRoomName = sRoomName;
	}
	~MD_Room() {}
	MD_Room( const MD_Room& room ) { *this = room; }
	MD_Room& operator=( const MD_Room& room ) { 
		m_iRoomId = room.m_iRoomId;
		m_sRoomName = room.m_sRoomName;
		m_vScenarios = room.m_vScenarios;
		return *this;
	}
	// Compare rooms by Id
	int operator==( const MD_Room& room ){ return m_iRoomId==room.m_iRoomId; }
	
	// Add scenario
	void AddScenario( MD_Scenario& scen ) {
		m_vScenarios.push_back( scen );
	}
	// Find Scenario by name
	MD_Scenario* GetScenario( string sScenarioName );

	// Serialization methods
	void Write( SerializeClass& sc );
	void Read( SerializeClass& sc );

	// Room name(description)
	string& GetName() { return m_sRoomName; }
	// RoomId
	int GetId() { return m_iRoomId; }

	#ifdef SMARTPHONE
		// Create submenu items for room
		void CreateMenu( LRMenuItem* pRoot );
	#endif
};

/*
 *
 *	MenuData - Menu data( items, drawing style ) for local rendered menus
 *
 */
class MenuData {
protected: 
	typedef vector<MD_Room> MD_Rooms;
	MD_Rooms m_Rooms;
	TextStyle m_tsItemText, m_tsHlItemText;	
	int m_iCrtRoom;

public:
	MenuData(){}
	~MenuData(){}
	MenuData& operator=( MenuData& data ){
		m_Rooms = data.m_Rooms;
		m_tsItemText = data.m_tsItemText;
		m_tsHlItemText = data.m_tsHlItemText;
		m_iCrtRoom = data.m_iCrtRoom;
		return *this;
	}

	// Serialization methods
	void Write( SerializeClass& sc );
	void Read( SerializeClass& sc );

	// find room by RoomId
	MD_Room* GetRoom( int iRoomId );

	// Add Rooms/Scenarios/SubScenarios
	void AddRoom( MD_Room& room ) { m_Rooms.push_back( room ); }
	void AddRoom( int iRoomId, string sRoomName );
	void AddScenario( int iRoomId, string sScenarioName );
	void AddSubScenario( int iRoomId, string sScenarioName, string& sSubId, string sSubName );

	// Item Text Style
	void SetItemTextStyle( TextStyle &ts ) { m_tsItemText = ts; }
	// Highlighted item text style
	void SetHlItemTextStyle( TextStyle &ts ) { m_tsHlItemText = ts; }
	// Current room
	void SetCrtRoom( int iCrtRoom ) { m_iCrtRoom = iCrtRoom; }

	// Hardcoded drawing styles for items
	static int GetItemStyleIndex() { return 17; }
	static int GetHlItemStyleIndex() { return 18; }

	#ifdef SMARTPHONE		
		// Create main menu
		LRMenu* CreateMainMenu();
	#endif
};

#endif.
