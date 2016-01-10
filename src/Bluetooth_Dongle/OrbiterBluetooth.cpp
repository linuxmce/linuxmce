/*
 OrbiterBluetooth
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

#include "OrbiterBluetooth.h"
#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/PlutoDefs.h"
#include "DCE/Logger.h"
#include "OrbiterBluetooth.h"
#include "SerializeClass/ShapesColors.h"
#include "BD/BDCommandProcessor.h"
#include "VIPShared/BD_CP_ShowImage.h"
#include "VIPShared/BD_CP_ShowList.h"
#include "VIPShared/BD_CP_CaptureKeyboard.h"
#include "VIPShared/BD_CP_SimulateEvent.h"
//---------------------------------
#include "VIPShared/BD_CP_MenuData.h"
#include "pluto_main/Define_DesignObj.h"
//---------------------------------
#include "VIPShared/PlutoPhoneCommands.h"

#include "Orbiter/ScreenHistory.h"
#include "Orbiter/SDL/JpegWrapper.h"
#include "Orbiter/OrbiterRenderer.h"

#include "../VIPShared/BD_CP_SetBkgndImage.h"
#include "../VIPShared/BD_PC_MouseEvent.h"
#include "../VIPShared/BD_CP_ShowMenu.h"
#include "../VIPShared/BD_CP_SetMenuImages.h"
#include "Orbiter/TextStyle.h"

#include "pluto_main/Define_Variable.h"

#define ADVANCED_OPTIONS_SCREEN "2022"
#define IMAGE_QUALITY_SCREEN    "1274"

#include "DataGrid.h"
using namespace DCE;
//-----------------------------------------------------------------------------------------------------
OrbiterBluetooth::OrbiterBluetooth(class BDCommandProcessor *pBDCommandProcessor, int DeviceID, 
    int PK_DeviceTemplate, string ServerAddress, string sLocalDirectory, bool bLocalMode, int nImageWidth, 
    int nImageHeight, pluto_pthread_mutex_t *pExternalScreenMutex)
: Orbiter(DeviceID, PK_DeviceTemplate, ServerAddress, sLocalDirectory, bLocalMode, nImageWidth, 
    nImageHeight, pExternalScreenMutex), m_pBDCommandProcessor(pBDCommandProcessor)
{
    m_bShowListSent = false;
    m_ImageQuality = 70;
}
//-----------------------------------------------------------------------------------------------------
bool OrbiterBluetooth::GetConfig()
{
    if(!Orbiter::GetConfig())
        return false;

    m_ImageQuality = DATA_Get_ImageQuality();

    if(m_ImageQuality < 10 || m_ImageQuality > 100)
        m_ImageQuality = 70; //default

    m_bDisplayOn=true;  // Override the default behavior -- when the phone starts the display is already on

    return true;
}
//-----------------------------------------------------------------------------------------------------
OrbiterBluetooth::~OrbiterBluetooth()
{
	//don't destroy anything until maint thread exists
	KillMaintThread();
}
//-----------------------------------------------------------------------------------------------------
void OrbiterBluetooth::CMD_Capture_Keyboard_To_Variable(string sPK_DesignObj,int iPK_Variable,string sOnOff,string sType,string sReset,int iPK_Text,bool bDataGrid,string &sCMD_Result,Message *pMessage)
{
    if(
        Orbiter::BuildCaptureKeyboardParams(
            sPK_DesignObj,
            iPK_Variable,
            sOnOff,
            sType,
            sReset,
            iPK_Text,
            bDataGrid
        )
    )
    {
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Sending BD_CP_CaptureKeyboard command with parameters: On: %d, Reset: %d, PinType: %d, DataGrid: %d, PK_Variable: %d, Text: %s",
			m_bCaptureKeyboard_OnOff, m_bCaptureKeyboard_Reset, m_iCaptureKeyboard_EditType,
			m_bCaptureKeyboard_DataGrid, m_iCaptureKeyboard_PK_Variable, m_sCaptureKeyboard_Text.c_str()
		);
#endif
		BD_CP_CaptureKeyboard *pBD_CP_CaptureKeyboard = new BD_CP_CaptureKeyboard(
				m_bCaptureKeyboard_OnOff,
				m_bCaptureKeyboard_DataGrid,
                m_bCaptureKeyboard_Reset,
				m_iCaptureKeyboard_EditType,
				m_iCaptureKeyboard_PK_Variable,
				m_sCaptureKeyboard_Text
		);
                
		if( m_pBDCommandProcessor )
			m_pBDCommandProcessor->AddCommand(pBD_CP_CaptureKeyboard);
    }
}
//-----------------------------------------------------------------------------------------------------
void OrbiterBluetooth::SimulateMouseClick(int x, int y)
{
	//do nothing
#ifndef WIN32
	printf("OrbiterBluetooth::SimulateMouseClick. The event will be ignored (we don't have mouse on the phone)\n");
#endif	
}
//-----------------------------------------------------------------------------------------------------
void OrbiterBluetooth::SimulateKeyPress(long key)
{
	BD_CP_SimulateEvent *pBD_CP_SimulateEvent = new BD_CP_SimulateEvent(0, key);

	if( m_pBDCommandProcessor )
		m_pBDCommandProcessor->AddCommand(pBD_CP_SimulateEvent);

#ifndef WIN32
	printf("OrbiterBluetooth::SimulateKeyPress with key code: %d\n", key);
#endif
}
//-----------------------------------------------------------------------------------------------------
void OrbiterBluetooth::OnReload()
{  
    LoggerWrapper::GetInstance()->Write(LV_WARNING,"Orbiter reloading");

    //kill the connection. this will kill orbiter and then recreate it when the phone will be detected
    m_pBDCommandProcessor->m_bDead = true;
}  
//-----------------------------------------------------------------------------------------------------
void OrbiterBluetooth::SetImageQuality(unsigned long nImageQuality)
{
    m_ImageQuality = nImageQuality;
    DATA_Set_ImageQuality(nImageQuality, true);

    m_bRerenderScreen = true;
    Renderer()->RedrawObjects();
}
//-----------------------------------------------------------------------------------------------------
void OrbiterBluetooth::ImageGenerated(const string& csImageFileName)
{
	bool bSignalStrengthScreen = false;
	bool bQualityImageScreen = false;
	if(NULL != m_pScreenHistory_Current)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Current screen: %s",  m_pScreenHistory_Current->GetObj()->m_ObjectID.c_str());
		bSignalStrengthScreen = m_pScreenHistory_Current->GetObj()->m_ObjectID.find(ADVANCED_OPTIONS_SCREEN) != string::npos;
	}

	if(NULL != m_pScreenHistory_Current)
	{
		bQualityImageScreen = m_pScreenHistory_Current->GetObj()->m_ObjectID.find(IMAGE_QUALITY_SCREEN) != string::npos;
	}

	string sRepeatedKeysList;
	GetRepeatedKeysForScreen(m_pScreenHistory_Current->GetObj(), sRepeatedKeysList);
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "Repeated keys list %s: ", sRepeatedKeysList.c_str());
#endif

	//load the image into a buffer and create 'BD_CP_ShowImage' command
	size_t iImageSize;
	char *pImage = FileUtils::ReadFileIntoBuffer(csImageFileName, iImageSize);

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "Ready to send a picture, size %d, reporting signal strength %d", iImageSize, bSignalStrengthScreen);
#endif

	BD_CP_ShowImage *pBD_CP_ShowImage = new BD_CP_ShowImage(0, (unsigned long)iImageSize, pImage, 
		(unsigned long)(sRepeatedKeysList.length()), sRepeatedKeysList.c_str(), bSignalStrengthScreen,
		bQualityImageScreen, m_ImageQuality);

	PLUTO_SAFE_DELETE_ARRAY(pImage);

	//hide any list if need it sending a 'BD_CP_ShowList' with Off option
	if( m_bShowListSent && !m_vectObjs_GridsOnScreen.size() )
	{
		list<string> listGrid;
		BD_CP_ShowList *pBD_CP_ShowList = new BD_CP_ShowList(0, 0, 0, 0, 0, listGrid, false, false);
		if( m_pBDCommandProcessor )
		{
			m_pBDCommandProcessor->AddCommand(pBD_CP_ShowList);
			m_bShowListSent = false;
		}
	}

	//finally, send the image
	if( m_pBDCommandProcessor )
		m_pBDCommandProcessor->AddCommand(pBD_CP_ShowImage);

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "ShowImage command added to the queue");
#endif

}
//-----------------------------------------------------------------------------------------------------
void OrbiterBluetooth::Initialize( GraphicType Type, int iPK_Room/*=0*/, int iPK_EntertainArea/*=0*/)
{
	Orbiter::Initialize(Type, iPK_Room, iPK_EntertainArea);
}
//-----------------------------------------------------------------------------------------------------


void OrbiterBluetooth::Configure( PhoneConfig& cfg )
{	
	m_Cfg = cfg;
	if ( m_Cfg.GetMenuMode()==PhoneConfig::mmMenu ) {	
		// send background
		string sImageFileName = "/usr/pluto/orbiter/C" + StringUtils::ltos(m_dwPK_Device) + "/" + 
			StringUtils::ltos(DESIGNOBJ_objBackground_CONST) + ".0.0.png";

		if(FileUtils::FileExists(sImageFileName) && ( m_pBDCommandProcessor ) ) {
			size_t iImageSize;	
			char *pImage = FileUtils::ReadFileIntoBuffer(sImageFileName, iImageSize);
			BD_CP_SetBkgndImage *pBD_CP_SetBkgndImage = new BD_CP_SetBkgndImage(
				0, (unsigned long)iImageSize, pImage );			
			m_pBDCommandProcessor->AddCommand( pBD_CP_SetBkgndImage );            
			PLUTO_SAFE_DELETE_ARRAY(pImage);
		}

		//get data
		MenuData data;
		GetMenuData( data );
		//set TextStyle
		TextStyle* pTextStyle=m_mapTextStyle_Find( MenuData::GetItemStyleIndex() );
		if ( pTextStyle ) data.SetItemTextStyle( *pTextStyle );
		pTextStyle=m_mapTextStyle_Find( MenuData::GetHlItemStyleIndex() );
		if ( pTextStyle ) data.SetHlItemTextStyle( *pTextStyle );

		// send BD_CP_MenuData		
		if( m_pBDCommandProcessor ) {
			BD_CP_MenuData *pBD_CP_MenuData = new BD_CP_MenuData( data );
			m_pBDCommandProcessor->AddCommand( pBD_CP_MenuData );
		}
	}
}

//-----------------------------------------------------------------------------------------------------

void OrbiterBluetooth::SelectedItem( string& sItemId )
{	
	#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "#	Received SelectedItem: %s", sItemId.c_str() );
	#endif

	DesignObj_Orbiter *pObj = FindObject(sItemId);
	if(NULL != pObj) {
		#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "#	Object found!" );
		#endif
		SelectedObject(pObj, smKeyboard, 0, 0);
	}
}

//-----------------------------------------------------------------------------------------------------
void OrbiterBluetooth::GetMenuData( MenuData& data  )
{
	int iObjects[] = { DESIGNOBJ_mnuLights_CONST, DESIGNOBJ_mnuMedia_CONST, 
		DESIGNOBJ_mnuClimate_CONST, DESIGNOBJ_mnuSecurity_CONST, DESIGNOBJ_mnuTelephony_CONST,
		DESIGNOBJ_mnuAdvancedOptions_CONST
	};
	string sScenarioDescs[] = { "Lights", "Media", "Climate", "Security", "Telephony", "Advanced Options" };	
	for(deque<LocationInfo *>::iterator itl = m_dequeLocation.begin(), end = m_dequeLocation.end(); itl != end; ++itl)
	{
		LocationInfo *pLocationInfo = *itl;
		string sRoomDesc = pLocationInfo->Description;
		int nRoomID = pLocationInfo->iLocation;

		LoggerWrapper::GetInstance()->Write(LV_WARNING, "#	Add room: %s", sRoomDesc.c_str() );
		// Add room
		data.AddRoom( nRoomID, sRoomDesc );				
		for(int i = 0; i < sizeof(iObjects) / sizeof(int); ++i) {

			LoggerWrapper::GetInstance()->Write(LV_WARNING, "#	Add scenario: %s", sScenarioDescs[i].c_str() );
			// Add scenario for room nRoomID
			data.AddScenario( nRoomID, sScenarioDescs[i] );	

			string sScenarioObjID = StringUtils::ltos(iObjects[i]) + "." + 
				(iObjects[i] == DESIGNOBJ_mnuAdvancedOptions_CONST ? "0" : StringUtils::itos(pLocationInfo->iLocation)) + ".0";
			DesignObj_Orbiter *pObj = FindObject(sScenarioObjID);
			if(NULL == pObj) {
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "#	Scenario not found!" );
				continue;
			}

			DesignObj_DataList::iterator it;				
			for(it = pObj->m_ChildObjects.begin(); it != pObj->m_ChildObjects.end(); ++it) {
				// Here are all the buttons
				DesignObj_Orbiter *pDesignObj_Orbiter = (DesignObj_Orbiter *)(*it);
				string sScenarioID = pDesignObj_Orbiter->GetVariableAssignment(VARIABLE_Array_ID_CONST);
				if( sScenarioID.empty() )
					continue;
				string sObjectID = pDesignObj_Orbiter->m_ObjectID;
				string sScenarioDesc = pDesignObj_Orbiter->GetVariableAssignment( VARIABLE_Array_Desc_CONST );				
				// Add subscenario for nRoomID, sScenarioDescs[i]
				data.AddSubScenario( nRoomID, sScenarioDescs[i], sObjectID, sScenarioDesc );
			}

		   //advanced options extra stuff
			if(iObjects[i] == DESIGNOBJ_mnuAdvancedOptions_CONST)
			{
				int iSubscenarios[] = 
				{
					DESIGNOBJ_butRestartDCERouter_CONST, DESIGNOBJ_objExitController2_CONST, 
					DESIGNOBJ_objResetUniverse2_CONST, DESIGNOBJ_butRegenAllOrbiters_CONST,
					DESIGNOBJ_butRegenOrbiter_CONST, DESIGNOBJ_butPendingTasks_CONST
				};

				string sSubscenariosDesc[] = 
				{
					"Quick reload router", "Exit Orbiter", "Reboot Core",
					"Regen All", "Regen Orbiter", "Pending Tasks"
				};

				for(int nIndex = 0; nIndex < sizeof(iSubscenarios) / sizeof(int); ++nIndex)
				{
					string sSubScenarioID = StringUtils::ltos(iObjects[i]) + ".0.0." + StringUtils::ltos(iSubscenarios[nIndex]);
					DesignObj_Orbiter *pDesignObj_Orbiter = FindObject(sSubScenarioID, pObj);

					LoggerWrapper::GetInstance()->Write(LV_WARNING, "#	Subscen %s Id %s!", 
						sSubscenariosDesc[nIndex].c_str(),	sSubScenarioID.c_str() );
					if(NULL == pDesignObj_Orbiter) {
						LoggerWrapper::GetInstance()->Write(LV_WARNING, "#	Subscen not found!" );
						continue;
					}

					string sScenarioDesc = sSubscenariosDesc[nIndex];
					string sObjectID = pDesignObj_Orbiter->m_ObjectID;
					// Add subscenario for nRoomID, sScenarioDescs[i]					
					data.AddSubScenario( nRoomID, sScenarioDescs[i], sObjectID, sScenarioDesc );
				}
			}

		}
	}
	data.SetCrtRoom( -1 );
	if ( m_pLocationInfo ) {
		data.SetCrtRoom( m_pLocationInfo->iLocation );
	}
}

//-----------------------------------------------------------------------------------------------------
void OrbiterBluetooth::HandleMouseEvent( int iX, int iY, int EventType )
{
	switch ( (BD_PC_MouseEvent::MouseEventType)EventType ){
		case BD_PC_MouseEvent::meMove:
			break;
		case BD_PC_MouseEvent::meStylusDown:
			{
				/*
				* Simulate Mouse click at iX, iY
				*/
				Orbiter::Event orbiterEvent;
				orbiterEvent.data.region.m_iButton = 1;
				orbiterEvent.data.region.m_iX = iX;
				orbiterEvent.data.region.m_iY = iY;

				orbiterEvent.type = Orbiter::Event::REGION_DOWN;
				ProcessEvent(orbiterEvent);

				orbiterEvent.type = Orbiter::Event::REGION_UP;
				ProcessEvent(orbiterEvent);
				/*
				*---------
				*/
			}
			break;
		case BD_PC_MouseEvent::meStylusUp:
			break;

	}	
}
//-----------------------------------------------------------------------------------------------------
void OrbiterBluetooth::ShowMenu( )
{
	int nCrtRoom = 0;
	if ( m_pLocationInfo ) {
		nCrtRoom =  m_pLocationInfo->iLocation;
	}

	// send BD_CP_SHOWMENU
	if( m_pBDCommandProcessor ) {
		BD_CP_ShowMenu *pBD_CP_ShowMenu = new BD_CP_ShowMenu( nCrtRoom );
		m_pBDCommandProcessor->AddCommand( pBD_CP_ShowMenu );
	}
}
//-----------------------------------------------------------------------------------------------------
void OrbiterBluetooth::GetMenuImages( MenuItemInfo::ItemType nItemsType, vector<MenuItemInfo>& vItems )
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "#	GetMenuImages: %d", vItems.size() );
	//string sImageFileName = "/usr/pluto/orbiter/C" + StringUtils::ltos(m_dwPK_Device) + "/" + 
	//		StringUtils::ltos(DESIGNOBJ_objBackground_CONST) + ".0.0.png";
	string sImageFileName = "/usr/pluto/orbiter/skins/Basic/Phone/Smartphone_menu_test.png";

	char *pImage = NULL;
	size_t iImageSize;	
	if(FileUtils::FileExists(sImageFileName) && ( m_pBDCommandProcessor ) ) {		
		pImage = FileUtils::ReadFileIntoBuffer(sImageFileName, iImageSize);		
	}
	switch ( nItemsType ) {
		case MenuItemInfo::stSubScenarios:
			{
				for ( vector<MenuItemInfo>::iterator iter=vItems.begin(); iter!=vItems.end(); ++iter ) {
					if ( pImage ) {						
						(*iter).SetImage( 0, (unsigned long)iImageSize, pImage );
					}
					/*
					DesignObj_Orbiter *pObj = FindObject( rItem.m_sObjectId );
					if ( NULL != pObj ){
						// Set image
					}
					*/
				}				
			}
			break;
		case MenuItemInfo::stScenarios:
			{
				for ( vector<MenuItemInfo>::iterator iter=vItems.begin(); iter!=vItems.end(); ++iter ) {										
					if ( pImage ) {						
						(*iter).SetImage( 0, (unsigned long)iImageSize, pImage );
					}
					/*
					DesignObj_Orbiter *pObj = FindObject( rItem.m_sObjectId );
					if ( NULL != pObj ){
						// Set image
					}
					*/
				}				
			}
			break;
		case MenuItemInfo::stRooms:
			{
				for ( vector<MenuItemInfo>::iterator iter=vItems.begin(); iter!=vItems.end(); ++iter ) {
					if ( pImage ) {						
						(*iter).SetImage( 0, (unsigned long)iImageSize, pImage );
					}
					/*
					DesignObj_Orbiter *pObj = FindObject( rItem.m_sObjectId );
					if ( NULL != pObj ){
						// Set image
					}
					*/
				}				
			}
			break;
	}
	if( m_pBDCommandProcessor ) {
		BD_CP_SetMenuImages *pBD_CP_SetMenuImages = new BD_CP_SetMenuImages( nItemsType, vItems );
		m_pBDCommandProcessor->AddCommand( pBD_CP_SetMenuImages );
	}
	PLUTO_SAFE_DELETE_ARRAY(pImage);
}
//-----------------------------------------------------------------------------------------------------
