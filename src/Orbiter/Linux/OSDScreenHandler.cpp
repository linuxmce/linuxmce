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
#ifndef WIN32
	#include "OrbiterLinux.h"
#endif

#include "OSDScreenHandler.h"
#include "pluto_main/Define_Variable.h"
#include "pluto_main/Define_Screen.h"
#include "pluto_main/Define_Text.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_DataGrid.h"
#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_EventParameter.h"
#include "pluto_main/Define_CommMethod.h"
#include "pluto_main/Define_Country.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "Gen_Devices/AllScreens.h"
#include "PlutoUtils/DatabaseUtils.h"
#include "../WizardLogic.h"
#include "pluto_main/Define_FloorplanObjectType.h"
#include "DataGrid.h"
#include "../OrbiterRenderer.h"
#include "../ScreenHistory.h"

#ifdef ENABLE_MOUSE_BEHAVIOR
#	include "../MouseBehavior.h"
#	include "../SpeedMouseHandler.h"
#	include "../VolumeMouseHandler.h"
#	include "../LightMouseHandler.h"
#endif

//-----------------------------------------------------------------------------------------------------
OSDScreenHandler::OSDScreenHandler(Orbiter *pOrbiter, map<int,int> *p_MapDesignObj) :
        ScreenHandler(pOrbiter, p_MapDesignObj)
{
	m_tWizardIsRunning = 0;
	m_bAlreadyPlaySeeAndHearMe = false;
	m_bAlreadyPlayFinalGreeting = false;
	m_bLightsFlashThreadRunning=m_bLightsFlashThreadQuit=false;
	m_pWizardLogic = new WizardLogic(pOrbiter);
	m_dwMessageInterceptorCounter_ReportingChildDevices = 0;
	m_tWaitingForRegistration = m_tRegistered = 0;
	m_bTurnedPnpOn=false;

	if(!m_pWizardLogic->Setup())
	{
		//do something here; unable to connect to db
	}

	//WARNING: HARDCODING!!!!
	m_bHasVideoWizardFiles = FileUtils::FileExists("/home/videowiz/greetings.mpg");
}
//-----------------------------------------------------------------------------------------------------
OSDScreenHandler::~OSDScreenHandler()
{
	delete m_pWizardLogic;
}

void OSDScreenHandler::DisableAllVideo()
{
	int arrayPK_DesignObj[] = { 4549,4550,4551,4552,4553,4554,4555,4556,4557,4558,4559,4560,4561,4562,4563,4564,4565,4566,4567,4624,4627,4633,4634,4637,4638,4641,4644,4659,4662,4670,4671,4672,4673,4674,4675,4676,4677,4678,4680,4689,4697,4700,4702,4703,5065,5066,5072, 0};
	for(int i=0;;++i)
	{
		if( arrayPK_DesignObj[i]==0 )
			return;
		DesignObj_Orbiter *pObj = m_pOrbiter->FindObject( StringUtils::itos(arrayPK_DesignObj[i]) + ".0.0." TOSTRING(DESIGNOBJ_generic_app_desktop_CONST) );
		if( pObj )
			pObj->m_bHidden=pObj->m_bHideByDefault=true;
	}
}

//-----------------------------------------------------------------------------------------------------
// Video Wizard greetings screen
//-----------------------------------------------------------------------------------------------------
void OSDScreenHandler::SCREEN_VideoWizard(long PK_Screen)
{
	if( m_pOrbiter->m_dwPK_Device_LocalMediaPlayer && m_pOrbiter->DeviceIsRegistered(m_pOrbiter->m_dwPK_Device_LocalMediaPlayer)!='Y' && (m_tWizardIsRunning==0 || time(NULL)-m_tWizardIsRunning<15) )
	{
		if( m_tWizardIsRunning==0 )
		{
			RegisterPersistentCallBack(cbOnGotoScreen, (ScreenHandlerCallBack) &OSDScreenHandler::VideoWizard_OnGotoScreen, new GotoScreenCallBackData());
			SCREEN_PopupMessage(SCREEN_PopupMessage_CONST, "One moment|NO_BUTTON", "", "", "", "", "1");
			m_tWizardIsRunning = time(NULL);
		}
		else
			DisplayMessageOnOrbiter(0,"One moment",false,"0",true,"NO_BUTTON");

		m_pOrbiter->StartScreenHandlerTimer(2000);
		RegisterCallBack(cbOnTimer,	(ScreenHandlerCallBack) &OSDScreenHandler::VideoWizard_OnTimer, new CallBackData());
		return;
	}

	DCE::CMD_MH_Stop_Media CMD_MH_Stop_Media(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_MediaPlugIn,0,0,0,"",false);
	m_pOrbiter->SendCommand(CMD_MH_Stop_Media);

	PrepForWizard();

	DesignObjText *pText = m_pOrbiter->FindText( m_pOrbiter->FindObject(DESIGNOBJ_Greetings_CONST),TEXT_STATUS_CONST );
	if( !m_bHasVideoWizardFiles )
	{
		//DisableAllVideo();
		if( pText )
			pText->m_sText = m_pOrbiter->m_mapTextString[TEXT_No_bonus_CD_CONST];
	}
	else if( pText )
		pText->m_sText = m_pOrbiter->m_mapTextString[TEXT_video_clip_here_CONST];

	if( true || (m_pOrbiter->m_bNewOrbiter==true && m_bAlreadyPlaySeeAndHearMe==false))
	{
		ScreenHandlerBase::SCREEN_VideoWizard(PK_Screen);
		m_bAlreadyPlaySeeAndHearMe=true;
	}
	else if( !m_pWizardLogic->HasRemoteControl() )
	{
		// Help the user get a remote control
		m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(DESIGNOBJ_NoRemoteControl_CONST),
									"", "", false, false );
	}
	else if( m_pWizardLogic->HouseAlreadySetup() )
	{
		if( m_pOrbiter->m_bNewOrbiter )
			// Everthing is setup already.  Goto the 'pick a wizard screen'
			m_pOrbiter->CMD_Goto_Screen("",SCREEN_This_Room_CONST); 
		else
			// Everthing is setup already.  Goto the 'pick a wizard screen'
			m_pOrbiter->CMD_Goto_Screen("",SCREEN_Which_Wizard_CONST);
		return;
	}
	else
	{
		// Everthing is setup already.  Goto the 'pick a wizard screen'
		m_pOrbiter->CMD_Goto_Screen("",SCREEN_UsersWizard_CONST);
		return;
	}

	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &OSDScreenHandler::VideoWizard_ObjectSelected, new ObjectInfoBackData());
	RegisterCallBack(cbOnRenderScreen, (ScreenHandlerCallBack) &OSDScreenHandler::VideoWizard_OnScreen, new RenderScreenCallBackData());

	m_pOrbiter->WaitForMessageQueue();  // The screen unload will send a stop_media.  Be sure it gets through before we send the play media
	Sleep(50); // Just a tiny sleep to be sure the aforementioned stop media gets through first.  Should be imperceptable

	DCE::CMD_Play_Media CMD_Play_Media(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_LocalMediaPlayer,0,0,"","/home/videowiz/greetings.mpg");
	m_pOrbiter->SendCommand(CMD_Play_Media);
}

bool OSDScreenHandler::VideoWizard_OnScreen(CallBackData *pData)
{
	RenderScreenCallBackData *pRenderScreenCallBackData = (RenderScreenCallBackData *)pData;
	if( pRenderScreenCallBackData->m_pObj->m_iBaseObjectID==DESIGNOBJ_NoRemoteControl_CONST )
	{
		m_pOrbiter->StartScreenHandlerTimer(500);
	}
	return false;
}

bool OSDScreenHandler::VideoWizard_OnTimer(CallBackData *pData)
{
	if( m_pOrbiter->m_bBypassScreenSaver==false )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "OSDScreenHandler::VideoWizard_OnTimer Waiting for media player to register");
		DCE::SCREEN_VideoWizard SCREEN_VideoWizard(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device);
		m_pOrbiter->SendCommand(SCREEN_VideoWizard);
		return true;
	}
	if( GetCurrentScreen_PK_DesignObj()==DESIGNOBJ_NoRemoteControl_CONST )
	{
		if(m_pWizardLogic->HasRemoteControl(false))
		{
			NeedToRender render2( m_pOrbiter, "OSDScreenHandler::VideoWizard_OnTimer" );  // Redraw anything that was changed by this command
			if( !m_pWizardLogic->HouseAlreadySetup() )
				m_pOrbiter->CMD_Goto_Screen("",SCREEN_UsersWizard_CONST);
			else
				m_pOrbiter->CMD_Goto_Screen("",SCREEN_Which_Wizard_CONST);
			return false;
		}
	}
	return true;
}

bool OSDScreenHandler::VideoWizard_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;

	switch(GetCurrentScreen_PK_DesignObj())
	{
		case DESIGNOBJ_Greetings_CONST:
		{
			if(pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butType_of_Wizard_CONST)
			{
				if( !m_pWizardLogic->HasRemoteControl() )
				{
					// Help the user get a remote control
					m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(DESIGNOBJ_NoRemoteControl_CONST),
												"", "", false, false );
					m_pOrbiter->StartScreenHandlerTimer(500);
					return true;
				}
				else if( !m_pWizardLogic->HouseAlreadySetup() )
				{
					m_pOrbiter->CMD_Goto_Screen("",SCREEN_UsersWizard_CONST);
					return true;
				}
				else
				{
					// The house is already setup
					if( m_pOrbiter->m_bNewOrbiter )
						// Everthing is setup already.  Goto the 'pick a wizard screen'
						m_pOrbiter->CMD_Goto_Screen("",SCREEN_This_Room_CONST); 
					else
						// Everthing is setup already.  Goto the 'pick a wizard screen'
						m_pOrbiter->CMD_Goto_Screen("",SCREEN_Which_Wizard_CONST);
					return true;
				}
			}
			break;
		}
		case DESIGNOBJ_NoRemoteControl_CONST:
		{
			if(pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butNoRemote_TypeOfWizard_CONST)
			{
				if( !m_pWizardLogic->HouseAlreadySetup() )
				{
					m_pOrbiter->CMD_Goto_Screen("",SCREEN_UsersWizard_CONST);
					return true;
				}
			}
			break;
		}

		case DESIGNOBJ_Final_CONST:
		{
			if(pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butStartUsingMonster_CONST)
			{
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "'Start using the system' button was clicked!");

				// if vdr or myth are not installed
				if(!m_pWizardLogic->AnyPVRSoftwareInstalled())
				{
					// install mythtv by default
					LoggerWrapper::GetInstance()->Write(LV_WARNING, "No PVR Software is installed. We'll install mythtv by default");
					m_pWizardLogic->SetPVRSoftware('M');
				}

				string sResponse;
				CMD_Regen_Orbiter cmd_Regen_Orbiter(m_pOrbiter->m_dwPK_Device, m_pOrbiter->m_dwPK_Device_OrbiterPlugIn, m_pOrbiter->m_dwPK_Device, "", "Y");
				m_pOrbiter->SendCommand(cmd_Regen_Orbiter, &sResponse);

				if(sResponse != "OK")
				{
					//the router will send a go to screen to orbiter to tell the user
					//that it cannot reload
					return true;
				}
			}
		}
	};
	return false;
}

//-----------------------------------------------------------------------------------------------------
// Users Wizard
//-----------------------------------------------------------------------------------------------------
void OSDScreenHandler::SCREEN_UsersWizard(long PK_Screen)
{
	PrepForWizard();
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Seek_Value_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Datagrid_Input_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_DesignObj_CurrentSecti_CONST, TOSTRING(DESIGNOBJ_butYourNameWizard_CONST));

	ScreenHandlerBase::SCREEN_UsersWizard(PK_Screen);
	if(m_pWizardLogic->AlreadyHasUsers())
	{
		//skip "what's your name" screen
		m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(DESIGNOBJ_FamilyMembers_CONST),
                                       "", "", false, false );
	}

	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &OSDScreenHandler::UsersWizard_ObjectSelected, new ObjectInfoBackData());
	RegisterCallBack(cbDataGridSelected, (ScreenHandlerCallBack) &OSDScreenHandler::UsersWizard_DatagridSelected, new DatagridCellBackData());
	RegisterCallBack(cbOnTimer,	(ScreenHandlerCallBack) &OSDScreenHandler::UsersWizard_OnTimer, new CallBackData());
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::UsersWizard_OnTimer(CallBackData *pData)
{
	if(!m_sPendingUserToAdd.empty())
	{
		m_pWizardLogic->AddUser(m_sPendingUserToAdd);		

		NeedToRender render(m_pOrbiter, "alert_user_creation_2");
		m_pOrbiter->CMD_Display_Alert("User created!", "user_creation", "2", interuptAlways);

		switch(m_AddUserNextAction)
		{
			case naNone:
				m_pOrbiter->CMD_Refresh("*");
				break;

			case naNextScreen:
				m_pOrbiter->CMD_Goto_Screen("",SCREEN_CountryWizard_CONST);
				break;

			case naDisplayUserInfo:
				{
					string sText=m_pOrbiter->m_mapTextString[TEXT_user_name_for_admin_CONST] + "|" + m_pOrbiter->m_mapTextString[TEXT_Ok_CONST];
					StringUtils::Replace(&sText,"<%=USER%>",m_sPendingUserToAdd);
					string sMessage="0 -300 1 " TOSTRING(COMMAND_Goto_DesignObj_CONST) " " TOSTRING(COMMANDPARAMETER_PK_DesignObj_CONST) " " TOSTRING(DESIGNOBJ_FamilyMembers_CONST);
					DCE::SCREEN_House_Setup_Popup_Message SCREEN_House_Setup_Popup_Message(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device,
						sText,sMessage,"1");
					m_pOrbiter->ReceivedMessage(SCREEN_House_Setup_Popup_Message.m_pMessage);
				}
				break;
		}

		m_sPendingUserToAdd = "";
	}
    
	return false;
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::HandleAddUser(AddUserNextAction action)
{
	PLUTO_SAFETY_LOCK(vm, m_pOrbiter->m_VariableMutex);
	string sUsername = m_pOrbiter->m_mapVariable_Find(VARIABLE_Seek_Value_CONST);
	vm.Release();

	m_AddUserNextAction = action;
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Seek_Value_CONST, "");

	if(sUsername != "")
	{
		m_pOrbiter->CMD_Set_Text(StringUtils::ltos(GetCurrentScreen_PK_DesignObj()), "", TEXT_USR_ENTRY_CONST);

		NeedToRender render(m_pOrbiter, "alert_user_creation_1");
		m_pOrbiter->CMD_Display_Alert("Adding user '" + sUsername + "', please wait...", "user_creation", "20", interuptAlways);

		//add the user async
		m_sPendingUserToAdd = sUsername;
		m_pOrbiter->StartScreenHandlerTimer(0);
		return true;
	}
	else if(action != naNextScreen)
	{
		m_pOrbiter->CMD_Set_Text(StringUtils::ltos(GetCurrentScreen_PK_DesignObj()),
                                 "ERROR: Please enter your name", TEXT_USR_ENTRY_CONST);
		return true;
	}
	else
	{
		m_pOrbiter->CMD_Goto_Screen("", SCREEN_CountryWizard_CONST);
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::UsersWizard_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;

	switch(GetCurrentScreen_PK_DesignObj())
	{
		case DESIGNOBJ_YourName_CONST:
		{
			if(pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butFamilyMembers_CONST)
			{
				string sUsername = m_pOrbiter->m_mapVariable_Find(VARIABLE_Seek_Value_CONST);
				if (sUsername.empty())
				{
					// Go back to the same screen again. 
					m_pOrbiter->CMD_Goto_DesignObj(0, 
						StringUtils::ltos(DESIGNOBJ_YourName_CONST),
						"", "", false, false );
					return true;	// screen changed.	
				}
				else
				{
					HandleAddUser(naDisplayUserInfo);
					return true;
				}
			}
		}
		break;

		case DESIGNOBJ_FamilyMembers_CONST:
		{
			switch(pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				case DESIGNOBJ_butAdduser_CONST:
				{
					if(HandleAddUser(naNone))
						return true; //error message
				}
				break;

				case DESIGNOBJ_butLocation_CONST:
				{
					HandleAddUser(naNextScreen);
					return true;// we'll go to next screen once will finish adding the user
				}
				break;

				case DESIGNOBJ_butRemoveUser_CONST:
				{
					int nPK_User = atoi(m_pOrbiter->m_mapVariable_Find(VARIABLE_Misc_Data_1_CONST).c_str());
					PLUTO_SAFETY_LOCK(vm, m_pOrbiter->m_VariableMutex);
					m_pOrbiter->CMD_Set_Variable(VARIABLE_Seek_Value_CONST, "");
					if(nPK_User)
					{
						m_pWizardLogic->RemoveUser(nPK_User);
						m_pOrbiter->CMD_Set_Text(StringUtils::ltos(GetCurrentScreen_PK_DesignObj()),
                                                 "", TEXT_USR_ENTRY_CONST);
						m_pOrbiter->CMD_Refresh("*");
					}
					else
					{
						m_pOrbiter->CMD_Set_Text(StringUtils::ltos(GetCurrentScreen_PK_DesignObj()),
                                                 "ERROR: No user selected to be removed", TEXT_USR_ENTRY_CONST);
						return true;
					}
				}
				break;
			}
		}
		break;
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::UsersWizard_DatagridSelected(CallBackData *pData)
{
	DatagridCellBackData *pCellInfoData = (DatagridCellBackData *)pData;

	switch(GetCurrentScreen_PK_DesignObj())
	{
		case DESIGNOBJ_FamilyMembers_CONST:
		{
			m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, pCellInfoData->m_sValue);
		}
		break;
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
// Country Wizard
//-----------------------------------------------------------------------------------------------------
void OSDScreenHandler::SCREEN_CountryWizard(long PK_Screen)
{
	PrepForWizard();
	m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_DesignObj_CurrentSecti_CONST, TOSTRING(DESIGNOBJ_butLocationWizard_CONST));

	ScreenHandlerBase::SCREEN_CountryWizard(PK_Screen);
	if( m_pWizardLogic->GetLocation() )
	{
		PLUTO_SAFETY_LOCK(vm, m_pOrbiter->m_VariableMutex);
		if( m_pOrbiter->m_mapVariable_Find(VARIABLE_Misc_Data_5_CONST)!="VALID_DATA_FOUND" )  // A keyword so we know when we already asked this question
		{
			m_pOrbiter->ForceCurrentScreenIntoHistory();
			m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_5_CONST, "VALID_DATA_FOUND");
			DCE::SCREEN_House_Setup_Popup_Message SCREEN_House_Setup_Popup_Message(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device,
				"Is this your location?\n<%=31%>|Yes|No","0 -300 1 741 159 192|0 -300 1 741 159 194","1");
			m_pOrbiter->ReceivedMessage(SCREEN_House_Setup_Popup_Message.m_pMessage);
			return;
		}
	}

	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_5_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Seek_Value_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Datagrid_Input_CONST, "");

	string sCountry = StringUtils::ltos(m_pWizardLogic->GetCountry());
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, sCountry);

	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &OSDScreenHandler::CountryWizard_ObjectSelected, new ObjectInfoBackData());
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::CountryWizard_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = dynamic_cast<ObjectInfoBackData *>(pData);

	if(NULL == pObjectInfoData)
		return false;

	switch(GetCurrentScreen_PK_DesignObj())
	{
		case DESIGNOBJ_Location_CONST:
		{
			if(pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butPostalCode_CONST) 
			{
				PLUTO_SAFETY_LOCK(vm, m_pOrbiter->m_VariableMutex);
				int PK_Country = atoi(m_pOrbiter->m_mapVariable_Find(VARIABLE_Misc_Data_1_CONST).c_str());
				m_pWizardLogic->SetCountry(PK_Country);

				string sCityRegion = m_pWizardLogic->GetCityRegion();
				m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST, sCityRegion);
				DesignObjText *pText = m_pOrbiter->FindText( m_pOrbiter->FindObject(DESIGNOBJ_City_CONST),TEXT_STATUS_CONST );
				if( pText )
				{
					if( PK_Country==COUNTRY_UNITED_STATES_CONST )  // Actually you can search for postal code no matter the country, but so far they're only populated for the U.S., and we call them 'Zip Codes'
					{
						pText->m_sText = m_pOrbiter->m_mapTextString[TEXT_Enter_your_postal_code_CONST];
						m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_4_CONST,"enterpostal");
					}
					else
					{
						pText->m_sText = m_pOrbiter->m_mapTextString[TEXT_Enter_city_CONST];  
						m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_4_CONST,"entercity");
					}
				}
			}
		}
		break;

		case DESIGNOBJ_City_CONST: 
		{
			if(pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butWizRooms_CONST )
			{
				string sUserInput = m_pOrbiter->m_mapVariable_Find(VARIABLE_Seek_Value_CONST);
				if( sUserInput.size() )
				{
					m_pOrbiter->CMD_Set_Variable(VARIABLE_Seek_Value_CONST,"");
					int iWidth=0,iHeight=0,PK_Variable=0;
					string sString;
					bool bSuccessfull;
					DCE::CMD_Populate_Datagrid CMD_Populate_Datagrid(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_DatagridPlugIn,"","city_" + StringUtils::itos(m_pOrbiter->m_dwPK_Device),DATAGRID_Cities_CONST,
						m_pOrbiter->m_mapVariable_Find(VARIABLE_Misc_Data_1_CONST) + "," + sUserInput,0,&PK_Variable,&sString,&bSuccessfull,&iWidth,&iHeight);
					m_pOrbiter->SendCommand(CMD_Populate_Datagrid);
					DesignObjText *pText = m_pOrbiter->FindText( m_pOrbiter->FindObject(DESIGNOBJ_City_CONST),TEXT_STATUS_CONST );
					if( pText )
					{
						if( iHeight )
							pText->m_sText = m_pOrbiter->m_mapTextString[TEXT_Select_City_CONST];  
						else
							pText->m_sText = m_pOrbiter->m_mapTextString[TEXT_Cannot_find_City_CONST]; 
					}
					DesignObj_DataGrid *pObj = (DesignObj_DataGrid *) m_pOrbiter->FindObject(DESIGNOBJ_dgCities_CONST);
					if( pObj )
					{
						// Re-acquire the grid now
						pObj->Flush();
						m_pOrbiter->CMD_Refresh("");
					}
					return true;
				}

				PLUTO_SAFETY_LOCK(vm, m_pOrbiter->m_VariableMutex);
				string sLocation = m_pOrbiter->m_mapVariable_Find(VARIABLE_Datagrid_Input_CONST);
				bool bLocationOk = m_pWizardLogic->SetLocation(sLocation);
				if( !bLocationOk )
				{
					DCE::SCREEN_House_Setup_Popup_Message SCREEN_House_Setup_Popup_Message(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device,
						m_pOrbiter->m_mapTextString[TEXT_Cannot_find_City_CONST] + "|Try again|Skip it","0 -300 1 741 159 194|0 -300 1 741 159 192","1");
					m_pOrbiter->ReceivedMessage(SCREEN_House_Setup_Popup_Message.m_pMessage);
					return true;
				}
			}
		}
		break;
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
// Rooms Wizard
//-----------------------------------------------------------------------------------------------------
void OSDScreenHandler::SCREEN_RoomsWizard(long PK_Screen)
{
	PrepForWizard();
	m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_DesignObj_CurrentSecti_CONST, TOSTRING(DESIGNOBJ_butRoomsWizard_CONST));
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_4_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Seek_Value_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Datagrid_Input_CONST, "");

	ScreenHandlerBase::SCREEN_RoomsWizard(PK_Screen);

	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &OSDScreenHandler::RoomsWizard_ObjectSelected, new ObjectInfoBackData());
	RegisterCallBack(cbCapturedKeyboardBufferChanged, (ScreenHandlerCallBack) &OSDScreenHandler::RoomsWizard_CapturedKeyboardBufferChanged, new ObjectInfoBackData());
	RegisterCallBack(cbDataGridSelected, (ScreenHandlerCallBack) &OSDScreenHandler::RoomsWizard_DatagridSelected, new DatagridCellBackData());
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::RoomsWizard_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;

	switch(GetCurrentScreen_PK_DesignObj())
	{
		case DESIGNOBJ_Rooms_CONST:
		{
			if(pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butAddRoomType_CONST)
			{
				long PK_RoomType = atoi(m_pOrbiter->m_mapVariable_Find(VARIABLE_Misc_Data_4_CONST).c_str());
				if(PK_RoomType)
				{
					m_pWizardLogic->AddRoomOfType(PK_RoomType);
					RefreshDatagrid(DESIGNOBJ_dgRoomTypes_CONST);
				}
			}
			else if(pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butRemoveRoomType_CONST)
			{
				long PK_RoomType = atoi(m_pOrbiter->m_mapVariable_Find(VARIABLE_Misc_Data_4_CONST).c_str());
				if(PK_RoomType)
				{
                    m_pWizardLogic->RemoveRoomOfType(PK_RoomType);
					RefreshDatagrid(DESIGNOBJ_dgRoomTypes_CONST);
				}
			}
			else if(pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butConfirmNames_CONST)
			{
				if( m_pWizardLogic->AlreadyHasRooms()==false )
				{
					DCE::SCREEN_House_Setup_Popup_Message SCREEN_House_Setup_Popup_Message(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device,
						m_pOrbiter->m_mapTextString[TEXT_Must_Create_1_room_CONST],"","1");
					m_pOrbiter->ReceivedMessage(SCREEN_House_Setup_Popup_Message.m_pMessage);
					return true;
				}
			}
		}
		break;
		
		case DESIGNOBJ_ConfirmNames_CONST:
		{
			if(pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butLightsFromRoom_CONST && m_pWizardLogic->HouseAlreadySetup() )
			{
				DCE::CMD_On CMD_On(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_PlugAndPlayPlugIn,0,"");
				m_pOrbiter->SendCommand(CMD_On);
//m_pOrbiter->m_bNewOrbiter =true;
				if( m_pOrbiter->m_bNewOrbiter && !m_bTurnedPnpOn )
				{
					m_bTurnedPnpOn=true;
					string sText=m_pOrbiter->m_mapTextString[TEXT_pnp_active_CONST] + "|" + m_pOrbiter->m_mapTextString[TEXT_Ok_CONST];
					string sMessage="0 -300 1 " TOSTRING(COMMAND_Goto_Screen_CONST) " " TOSTRING(COMMANDPARAMETER_PK_Screen_CONST) " " TOSTRING(SCREEN_LightsSetup_CONST);
					DCE::SCREEN_House_Setup_Popup_Message SCREEN_House_Setup_Popup_Message(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device,
						sText,sMessage,"0");
					m_pOrbiter->ReceivedMessage(SCREEN_House_Setup_Popup_Message.m_pMessage);
					return true;
				}
			}
		}
		break;
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::RoomsWizard_CapturedKeyboardBufferChanged(CallBackData *pData)
{
	switch(GetCurrentScreen_PK_DesignObj())
	{
		case DESIGNOBJ_ConfirmNames_CONST:
		{
			int PK_Room = atoi(m_pOrbiter->m_mapVariable_Find(VARIABLE_Misc_Data_1_CONST).c_str());
			string sName = m_pOrbiter->m_mapVariable_Find(VARIABLE_Seek_Value_CONST);

			if(sName != "" && PK_Room)
			{
				m_pWizardLogic->ChangeRoomName(PK_Room, sName);
				m_pOrbiter->CMD_Refresh("*");
			}
		}
		break;
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::RoomsWizard_DatagridSelected(CallBackData *pData)
{
	DatagridCellBackData *pCellInfoData = (DatagridCellBackData *)pData;

	switch(GetCurrentScreen_PK_DesignObj())
	{
		case DESIGNOBJ_ConfirmNames_CONST:
		{
			m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, pCellInfoData->m_sValue);
			m_pOrbiter->CMD_Set_Variable(VARIABLE_Seek_Value_CONST, pCellInfoData->m_sText);
			m_pOrbiter->CMD_Set_Text(StringUtils::ltos(GetCurrentScreen_PK_DesignObj()),
                                     pCellInfoData->m_sText, TEXT_USR_ENTRY_CONST);
		}
		break;
	}

	return false;
}

void OSDScreenHandler::SCREEN_This_Room(long PK_Screen, bool bAlways)
{
	PrepForWizard();
	ScreenHandler::SCREEN_This_Room(PK_Screen,bAlways);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_DesignObj_CurrentSecti_CONST, TOSTRING(DESIGNOBJ_butThisRoom_CONST));
	if( !bAlways )
	{
		int PK_Room = m_pWizardLogic->WhatRoomIsThisDeviceIn(m_pWizardLogic->GetTopMostDevice(m_pOrbiter->m_dwPK_Device));
		if( PK_Room )
		{
			m_pOrbiter->ForceCurrentScreenIntoHistory();
			m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, DatabaseUtils::GetNameForRoom(m_pWizardLogic,PK_Room));

			string sText=m_pOrbiter->m_mapTextString[TEXT_confirm_room_CONST] + "|" + m_pOrbiter->m_mapTextString[TEXT_YES_CONST] + "|" + m_pOrbiter->m_mapTextString[TEXT_NO_CONST];
			string sMessage="0 -300 1 " TOSTRING(COMMAND_Goto_Screen_CONST) " " TOSTRING(COMMANDPARAMETER_PK_Screen_CONST) " " TOSTRING(SCREEN_TV_Manufacturer_CONST) "|" 
				"0 -300 1 " TOSTRING(COMMAND_Goto_Screen_CONST) " " TOSTRING(COMMANDPARAMETER_PK_Screen_CONST) " " TOSTRING(SCREEN_This_Room_CONST) " " TOSTRING(COMMANDPARAMETER_Always_CONST) " 1";

			DCE::SCREEN_Media_Player_Setup_Popup_Message SCREEN_Media_Player_Setup_Popup_Message(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device,
				sText,sMessage,"1");
			m_pOrbiter->ReceivedMessage(SCREEN_Media_Player_Setup_Popup_Message.m_pMessage);

			return;
		}
	}

	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &OSDScreenHandler::ThisRoom_ObjectSelected, new ObjectInfoBackData());
	RegisterCallBack(cbDataGridSelected, (ScreenHandlerCallBack) &OSDScreenHandler::This_Room_GridSelected, new DatagridCellBackData());
}


bool OSDScreenHandler::This_Room_GridSelected(CallBackData *pData)
{
	DatagridCellBackData *pCellInfoData = (DatagridCellBackData *)pData;
	if(GetCurrentScreen_PK_DesignObj()==DESIGNOBJ_InWhichRoomIsTheSystem_CONST)
	{
		int PK_Room = atoi(pCellInfoData->m_sValue.c_str());
		string sPK_Device = pCellInfoData->m_pDataGridCell->m_mapAttributes_Find("PK_Device");
		if( sPK_Device.empty()==false )
			return true;
		m_pOrbiter->CMD_Show_Object( TOSTRING(DESIGNOBJ_butTVProvider_CONST), 0,"","", PK_Room ? "1" : "0" );
	}

	return false;
}


bool OSDScreenHandler::ThisRoom_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;

	switch(GetCurrentScreen_PK_DesignObj())
	{
		case DESIGNOBJ_InWhichRoomIsTheSystem_CONST:
		{
			if(pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butTVProvider_CONST)
			{
				string sPK_Room = m_pOrbiter->m_mapVariable_Find(VARIABLE_Datagrid_Input_CONST);
				if(sPK_Room == "")
					return true;

				int nPK_Device_TopMost = m_pWizardLogic->GetTopMostDevice(m_pOrbiter->m_dwPK_Device);
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "Setting the room for top most device %d, room %s",
                                      nPK_Device_TopMost, sPK_Room.c_str());
				m_pWizardLogic->SetRoomForDevice(StringUtils::ltos(nPK_Device_TopMost), sPK_Room);
				m_pOrbiter->m_pData->m_dwPK_Room = atoi(sPK_Room.c_str());
			}
		}
		break;
	}
	return false;
}

//-----------------------------------------------------------------------------------------------------
// A/V Connection Wizard
//-----------------------------------------------------------------------------------------------------
void OSDScreenHandler::SCREEN_TV_Manufacturer(long PK_Screen)
{
	PrepForWizard();
	ScreenHandlerBase::SCREEN_TV_Manufacturer(PK_Screen);
	m_tWaitingForRegistration = 0;
	m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_DesignObj_CurrentSecti_CONST, TOSTRING(DESIGNOBJ_butTVManufWizard_CONST));
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_3_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Datagrid_Input_CONST, "");
	
	m_pWizardLogic->m_nPK_Device_TV = 0;
	m_pWizardLogic->m_nPK_Command_Input_Video_On_TV = 0;
	string sTV,sInput;
	/*bool bHasTvInPath = */m_pWizardLogic->GetAvPath(m_pOrbiter->m_pData->m_dwPK_Device_ControlledVia,
                                              m_pWizardLogic->m_nPK_Device_TV,2,m_pWizardLogic->m_nPK_Command_Input_Video_On_TV,sTV,sInput);
	if( !m_pWizardLogic->m_nPK_Device_TV )
		m_pWizardLogic->m_nPK_Device_TV = m_pWizardLogic->FindFirstDeviceInCategory(DEVICECATEGORY_TVsPlasmasLCDsProjectors_CONST,m_pOrbiter->m_dwPK_Device,&sTV);

	if(m_pWizardLogic->m_nPK_Device_TV )
	{
		m_pOrbiter->DATA_Set_Ignore_First_Event(true,true);  // We have a TV, so the first click/key/etc. should be ignored since the TV is off and it will just turn it on
		m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST, StringUtils::itos(DatabaseUtils::GetDeviceTemplateForDevice(m_pWizardLogic,m_pWizardLogic->m_nPK_Device_TV)));
		m_pOrbiter->CMD_Set_Variable(VARIABLE_Device_List_CONST, sTV);
		m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_5_CONST, sInput);
		m_pOrbiter->ForceCurrentScreenIntoHistory();
		if( m_pWizardLogic->m_nPK_Command_Input_Video_On_TV )
		{

			string sText=m_pOrbiter->m_mapTextString[TEXT_confirm_tv_and_input_CONST] + "|" + m_pOrbiter->m_mapTextString[TEXT_YES_CONST] + "|" + 
				m_pOrbiter->m_mapTextString[TEXT_right_tv_wrong_input_CONST] + "|" + m_pOrbiter->m_mapTextString[TEXT_wrong_tv_CONST] + "|" + 
				m_pOrbiter->m_mapTextString[TEXT_dont_control_my_TV_CONST];
			string sMessage="0 -300 1 " TOSTRING(COMMAND_Goto_Screen_CONST) " " TOSTRING(COMMANDPARAMETER_PK_Screen_CONST) " " TOSTRING(SCREEN_Receiver_CONST) "|" 
				"0 -300 1 " TOSTRING(COMMAND_Goto_DesignObj_CONST) " " TOSTRING(COMMANDPARAMETER_PK_DesignObj_CONST) " " TOSTRING(DESIGNOBJ_DirectToTV_CONST) "|"
				"0 -300 1 " TOSTRING(COMMAND_Goto_DesignObj_CONST) " " TOSTRING(COMMANDPARAMETER_PK_DesignObj_CONST) " " TOSTRING(DESIGNOBJ_TVManuf_CONST) "|"
				"0 " + StringUtils::itos(m_pOrbiter->m_dwPK_Device_GeneralInfoPlugIn) + " 1 " TOSTRING(COMMAND_Delete_Device_CONST) " " TOSTRING(COMMANDPARAMETER_PK_Device_CONST) " " + StringUtils::itos(m_pWizardLogic->m_nPK_Device_TV) + "\n"
				"0 -300 1 " TOSTRING(COMMAND_Goto_Screen_CONST) " " TOSTRING(COMMANDPARAMETER_PK_Screen_CONST) " " TOSTRING(SCREEN_Receiver_CONST);

			DCE::SCREEN_Media_Player_Setup_Popup_Message SCREEN_Media_Player_Setup_Popup_Message(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device,
				sText,sMessage,"1");
			m_pOrbiter->ReceivedMessage(SCREEN_Media_Player_Setup_Popup_Message.m_pMessage);
		}
		else
		{
			string sText=m_pOrbiter->m_mapTextString[TEXT_Confirm_TV_CONST] + "|" + m_pOrbiter->m_mapTextString[TEXT_YES_CONST] + "|" + m_pOrbiter->m_mapTextString[TEXT_wrong_tv_CONST] + "|" + 
				m_pOrbiter->m_mapTextString[TEXT_dont_control_my_TV_CONST];
			string sMessage="0 -300 1 " TOSTRING(COMMAND_Goto_DesignObj_CONST) " " TOSTRING(COMMANDPARAMETER_PK_DesignObj_CONST) " " TOSTRING(DESIGNOBJ_DirectToTV_CONST) "|" 
				"0 -300 1 " TOSTRING(COMMAND_Goto_DesignObj_CONST) " " TOSTRING(COMMANDPARAMETER_PK_DesignObj_CONST) " " TOSTRING(DESIGNOBJ_TVManuf_CONST) "|"
				"0 " + StringUtils::itos(m_pOrbiter->m_dwPK_Device_GeneralInfoPlugIn) + " 1 " TOSTRING(COMMAND_Delete_Device_CONST) " " TOSTRING(COMMANDPARAMETER_PK_Device_CONST) " " + StringUtils::itos(m_pWizardLogic->m_nPK_Device_TV) + "\n"
				"0 -300 1 " TOSTRING(COMMAND_Goto_Screen_CONST) " " TOSTRING(COMMANDPARAMETER_PK_Screen_CONST) " " TOSTRING(SCREEN_Receiver_CONST);

			DCE::SCREEN_Media_Player_Setup_Popup_Message SCREEN_Media_Player_Setup_Popup_Message(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device,
				sText,sMessage,"1");
			m_pOrbiter->ReceivedMessage(SCREEN_Media_Player_Setup_Popup_Message.m_pMessage);
		}
	}
	else
	{
		m_pOrbiter->DATA_Set_Ignore_First_Event(false,true);
		m_pWizardLogic->FindPnpDevices(TOSTRING(DEVICECATEGORY_TVsPlasmasLCDsProjectors_CONST));
		m_pOrbiter->StartScreenHandlerTimer(500);
	}

	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &OSDScreenHandler::TV_Manufacturer_ObjectSelected, 
		new ObjectInfoBackData());
	RegisterCallBack(cbDataGridSelected, (ScreenHandlerCallBack) &OSDScreenHandler::TV_Manufacturer_GridSelected, 
		new DatagridCellBackData());
	RegisterCallBack(cbOnTimer,	(ScreenHandlerCallBack) &OSDScreenHandler::TV_OnTimer, new CallBackData());

	RegisterCallBack(cbCapturedKeyboardBufferChanged, 
		(ScreenHandlerCallBack) &OSDScreenHandler::AV_Devices_CapturedKeyboardBufferChanged,
		new ObjectInfoBackData());
	RegisterCallBack(cbOnRenderScreen, (ScreenHandlerCallBack) &OSDScreenHandler::TV_OnScreen, new RenderScreenCallBackData());

	if( !m_pWizardLogic->m_nPK_Device_TV && m_tLastDeviceAdded && time(NULL)-m_tLastDeviceAdded<10 ) // The user just recently added a new tv or other device.  Wait a few seconds to confirm
	{
		m_pOrbiter->ForceCurrentScreenIntoHistory();
		string sText=m_pOrbiter->m_mapTextString[TEXT_Waiting_for_device_to_startup_CONST] + "|NO_BUTTON";
		DCE::SCREEN_Media_Player_Setup_Popup_Message SCREEN_Media_Player_Setup_Popup_Message(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device,
			sText,"","1");
		m_pOrbiter->ReceivedMessage(SCREEN_Media_Player_Setup_Popup_Message.m_pMessage);

		m_pOrbiter->StartScreenHandlerTimer(10000); // Wait 10 seconds before continuing
		m_tWaitingForRegistration=1;  // Special indicator for the timer that it should just return to this screen whenever it's called
	}
}
bool OSDScreenHandler::TV_OnScreen(CallBackData *pData)
{
	return false;
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::TV_OnTimer(CallBackData *pData)
{
	if( m_tWaitingForRegistration==1 )
	{
		m_pOrbiter->CMD_Goto_Screen("",SCREEN_TV_Manufacturer_CONST);
		return false;
	}
	int PK_Device_TV = m_pWizardLogic->FindFirstDeviceInCategory(DEVICECATEGORY_TVsPlasmasLCDsProjectors_CONST,m_pOrbiter->m_dwPK_Device);

	if(PK_Device_TV)
	{
		m_pOrbiter->DATA_Set_Ignore_First_Event(true,true);  // We have a TV, so the first click/key/etc. should be ignored since the TV is off and it will just turn it on
		NeedToRender render2( m_pOrbiter, "OSDScreenHandler::VideoWizard_OnTimer" );  // Redraw anything that was changed by this command
		m_pOrbiter->CMD_Goto_DesignObj(0,TOSTRING(DESIGNOBJ_DirectToTV_CONST),"","",false,false);
		m_pWizardLogic->m_nPK_Device_TV=PK_Device_TV;
		m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST, StringUtils::itos(DatabaseUtils::GetDeviceTemplateForDevice(m_pWizardLogic,m_pWizardLogic->m_nPK_Device_TV)));
		return false;
	}
	return true;
}

//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::TV_Manufacturer_GridSelected(CallBackData *pData)
{
	DatagridCellBackData *pCellInfoData = (DatagridCellBackData *)pData;
	
	switch(GetCurrentScreen_PK_DesignObj())
	{
		case DESIGNOBJ_TVManuf_CONST:
			{
				m_pWizardLogic->m_dwPK_Manufacturer = atoi(pCellInfoData->m_sValue.c_str());
				m_pOrbiter->CMD_Show_Object( TOSTRING(DESIGNOBJ_butTVModel_CONST), 0,"","", m_pWizardLogic->m_dwPK_Manufacturer ? "1" : "0" );
			}
			break;
		case DESIGNOBJ_TVModel_CONST:
			{
				m_pWizardLogic->m_nPK_Device_TV = atoi(pCellInfoData->m_sValue.c_str());
				m_pOrbiter->CMD_Show_Object( TOSTRING(DESIGNOBJ_butDirectToTv_CONST), 0,"","", m_pWizardLogic->m_nPK_Device_TV ? "1" : "0" );
			}
			break;
		case DESIGNOBJ_WhichTVInput_CONST:
			{
				m_pWizardLogic->m_nPK_Command_Input_Video_On_TV = atoi(pCellInfoData->m_sValue.c_str());
				m_pOrbiter->CMD_Show_Object( TOSTRING(DESIGNOBJ_butReceiver_CONST), 0,"","", m_pWizardLogic->m_nPK_Command_Input_Video_On_TV ? "1" : "0" );
			}
			break;
		case DESIGNOBJ_SelectPort_CONST:
			{
				m_pOrbiter->CMD_Show_Object( TOSTRING(DESIGNOBJ_butSelectedPort_CONST), 0,"","", pCellInfoData->m_sValue.size() ? "1" : "0" );
			}
			break;
	}

	return false;
}

//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::TV_Manufacturer_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = dynamic_cast<ObjectInfoBackData *>(pData);
	//int nDeviceId = 0;
	string aux;

	switch( GetCurrentScreen_PK_DesignObj() )
	{
		case DESIGNOBJ_TVModel_CONST:
		{
			if(DESIGNOBJ_butDirectToTv_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				string sModel = m_pOrbiter->m_mapVariable_Find(VARIABLE_Misc_Data_2_CONST);
				if(sModel == "")
					return true;

				// Delete tv's first since maybe the user is returning to this wizard
				int PK_CommMethod = DatabaseUtils::GetCommMethodForDeviceTemplate(m_pWizardLogic,atoi(sModel.c_str()));
				if( PK_CommMethod==COMMMETHOD_RS232_CONST )
				{
					m_pOrbiter->CMD_Goto_DesignObj(0,StringUtils::itos(DESIGNOBJ_SelectPort_CONST),"","",false,false);
					return true;  // We're redirecting the flow
				}
				else
				{
					m_pWizardLogic->DeleteDevicesInThisRoomOfType(DEVICECATEGORY_TVsPlasmasLCDsProjectors_CONST);
					m_pWizardLogic->m_nPK_Device_TV = m_pWizardLogic->AddDevice(atoi(sModel.c_str()));
					m_pOrbiter->DATA_Set_Ignore_First_Event(true,true);  // We have a TV, so the first click/key/etc. should be ignored since the TV is off and it will just turn it on
				}
			}
			else if(DESIGNOBJ_butModelNotListed2_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				m_pOrbiter->CMD_Goto_Screen("",SCREEN_Receiver_CONST);
			}
		}
		break;

		case DESIGNOBJ_DirectToTV_CONST:
		{
			DesignObjText *pText = m_pOrbiter->FindText( m_pOrbiter->FindObject(DESIGNOBJ_WhichTVInput_CONST),TEXT_STATUS_CONST );
			if(DESIGNOBJ_butDirectlyToTv_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				m_pWizardLogic->m_bUsingReceiverForVideo = false;
				if( pText )
					pText->m_sText = m_pOrbiter->m_mapTextString[TEXT_input_from_md_CONST];
			}
			else if(DESIGNOBJ_butToReceiver_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				m_pWizardLogic->m_bUsingReceiverForVideo = true;
				if( pText )
					pText->m_sText = m_pOrbiter->m_mapTextString[TEXT_input_from_Receiver_to_TV_CONST];
			}
			else if(DESIGNOBJ_butNeither_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject) // 4734
			{
				string sText=m_pOrbiter->m_mapTextString[TEXT_Need_to_use_Pluto_Admin_CONST] + "|" + m_pOrbiter->m_mapTextString[TEXT_Ok_CONST];
				string sMessage="0 -300 1 " TOSTRING(COMMAND_Goto_Screen_CONST) " " TOSTRING(COMMANDPARAMETER_PK_Screen_CONST) " " TOSTRING(SCREEN_Receiver_CONST);

				DCE::SCREEN_Media_Player_Setup_Popup_Message SCREEN_Media_Player_Setup_Popup_Message(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device,
					sText,sMessage,"1");
				m_pOrbiter->ReceivedMessage(SCREEN_Media_Player_Setup_Popup_Message.m_pMessage);
			}
		}
		break;

		case DESIGNOBJ_WhichTVInput_CONST:
		{
			if( pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butReceiver_CONST )
			{
				m_pWizardLogic->m_nPK_Command_Input_Video_On_TV = atoi(m_pOrbiter->m_mapVariable_Find(VARIABLE_Misc_Data_4_CONST).c_str());
				if( !m_pWizardLogic->m_bUsingReceiverForVideo )
					m_pWizardLogic->SetAvPath(m_pOrbiter->m_pData->m_dwPK_Device_ControlledVia,
                                              m_pWizardLogic->m_nPK_Device_TV,2,m_pWizardLogic->m_nPK_Command_Input_Video_On_TV); // 2 = video
			}
			else if(DESIGNOBJ_butNoneTvInput_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				m_pWizardLogic->SetAvPath(m_pOrbiter->m_pData->m_dwPK_Device_ControlledVia,
					m_pWizardLogic->m_nPK_Device_TV, 2, 0); // 2 = video
			}
		}
		break;

		case DESIGNOBJ_SelectPort_CONST:
		{
            if(DESIGNOBJ_butSelectedPort_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				PLUTO_SAFETY_LOCK(vm, m_pOrbiter->m_VariableMutex);
				string sCell = m_pOrbiter->m_mapVariable_Find(VARIABLE_Datagrid_Input_CONST);
				string::size_type pos=0;
				int iPK_Device_ControlledVia = atoi(StringUtils::Tokenize(sCell,",",pos).c_str());
				string sPort = StringUtils::Tokenize(sCell,",",pos);
				int PK_DeviceTemplate = atoi(m_pOrbiter->m_mapVariable_Find(VARIABLE_Misc_Data_2_CONST).c_str());
				// Delete receivers first since maybe the user is returning to this wizard
				m_pWizardLogic->DeleteDevicesInThisRoomOfType(DEVICECATEGORY_TVsPlasmasLCDsProjectors_CONST);
				m_pWizardLogic->m_nPK_Device_TV = m_pWizardLogic->AddDevice(PK_DeviceTemplate,
																				StringUtils::itos(DEVICEDATA_COM_Port_on_PC_CONST) + "|" + sPort,
																				iPK_Device_ControlledVia);

				m_pOrbiter->CMD_Goto_DesignObj(0,StringUtils::itos(DESIGNOBJ_DirectToTV_CONST),"","",true,false);
				m_pOrbiter->DATA_Set_Ignore_First_Event(true,true);  // We have a TV, so the first click/key/etc. should be ignored since the TV is off and it will just turn it on
				return true;
			}
		}
		break;
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
// Receiver
//-----------------------------------------------------------------------------------------------------
void OSDScreenHandler::SCREEN_Receiver(long PK_Screen)
{
	PrepForWizard();
	m_tWaitingForRegistration = 0;
	m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_DesignObj_CurrentSecti_CONST, TOSTRING(DESIGNOBJ_butReceiverWizard_CONST));
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_3_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_4_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Datagrid_Input_CONST, "");
	ScreenHandlerBase::SCREEN_Receiver(PK_Screen);

	long PK_Command_Input = 0;
	string sReceiver,sInput;
	m_pWizardLogic->m_nPK_Device_Receiver = 0;  // We will refind this
	/*bool bHasReceiverInPath = */m_pWizardLogic->GetAvPath(m_pOrbiter->m_pData->m_dwPK_Device_ControlledVia,
                                              m_pWizardLogic->m_nPK_Device_Receiver,1,PK_Command_Input,sReceiver,sInput);

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "OSDScreenHandler::SCREEN_Receiver receiver %d %s input %d",
		m_pWizardLogic->m_nPK_Device_Receiver, sReceiver.c_str(), PK_Command_Input);

	if( m_pWizardLogic->m_nPK_Device_Receiver )
	{
		// Be sure it's really a receiver and not just the TV being used as the audio device
		string sTV,sInput;
		/*bool bHasTvInPath = */m_pWizardLogic->GetAvPath(m_pOrbiter->m_pData->m_dwPK_Device_ControlledVia,
												m_pWizardLogic->m_nPK_Device_TV,2,m_pWizardLogic->m_nPK_Command_Input_Video_On_TV,sTV,sInput);

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "OSDScreenHandler::SCREEN_Receiver receiver %d tv %d",
			m_pWizardLogic->m_nPK_Device_Receiver, m_pWizardLogic->m_nPK_Device_TV);

		if( m_pWizardLogic->m_nPK_Device_TV==m_pWizardLogic->m_nPK_Device_Receiver ) // It's just the tv
		{
			m_pWizardLogic->m_nPK_Device_Receiver=0;
			PK_Command_Input=0;
		}
	}

	if( !m_pWizardLogic->m_nPK_Device_Receiver )
	{
		m_pWizardLogic->m_nPK_Device_Receiver = m_pWizardLogic->FindFirstDeviceInCategory(DEVICECATEGORY_AmpsPreampsReceiversTuners_CONST,m_pOrbiter->m_dwPK_Device,&sReceiver);
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "OSDScreenHandler::SCREEN_Receiver no receiver category matched: %d", m_pWizardLogic->m_nPK_Device_Receiver);
	}

	if( m_pWizardLogic->m_nPK_Device_Receiver )
	{
		m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST, StringUtils::itos(DatabaseUtils::GetDeviceTemplateForDevice(m_pWizardLogic,m_pWizardLogic->m_nPK_Device_Receiver)));
		m_pOrbiter->CMD_Set_Variable(VARIABLE_Device_List_CONST, sReceiver);
		m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_5_CONST, sInput);
		m_pOrbiter->ForceCurrentScreenIntoHistory();
		if( PK_Command_Input )
		{
			string sText=m_pOrbiter->m_mapTextString[TEXT_confirm_receiver_and_input_CONST] + "|" + m_pOrbiter->m_mapTextString[TEXT_YES_CONST] + "|" + 
				m_pOrbiter->m_mapTextString[TEXT_right_receiver_wrong_input_CONST] + "|" + m_pOrbiter->m_mapTextString[TEXT_Wrong_receiver_CONST] + "|" + m_pOrbiter->m_mapTextString[TEXT_dont_control_my_receiver_CONST];
			string sMessage="0 -300 1 " TOSTRING(COMMAND_Goto_Screen_CONST) " " TOSTRING(COMMANDPARAMETER_PK_Screen_CONST) " " TOSTRING(SCREEN_AV_Devices_CONST) "|" 
				"0 -300 1 " TOSTRING(COMMAND_Goto_DesignObj_CONST) " " TOSTRING(COMMANDPARAMETER_PK_DesignObj_CONST) " " TOSTRING(DESIGNOBJ_ReceiverInputs_CONST) "|"
				"0 -300 1 " TOSTRING(COMMAND_Goto_DesignObj_CONST) " " TOSTRING(COMMANDPARAMETER_PK_DesignObj_CONST) " " TOSTRING(DESIGNOBJ_NoReceiver_CONST) "|"
				"0 " + StringUtils::itos(m_pOrbiter->m_dwPK_Device_GeneralInfoPlugIn) + " 1 " TOSTRING(COMMAND_Delete_Device_CONST) " " TOSTRING(COMMANDPARAMETER_PK_Device_CONST) " " + StringUtils::itos(m_pWizardLogic->m_nPK_Device_Receiver) + "\n"
				"0 -300 1 " TOSTRING(COMMAND_Goto_Screen_CONST) " " TOSTRING(COMMANDPARAMETER_PK_Screen_CONST) " " TOSTRING(SCREEN_AV_Devices_CONST);

			DCE::SCREEN_Media_Player_Setup_Popup_Message SCREEN_Media_Player_Setup_Popup_Message(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device,
				sText,sMessage,"1");
			m_pOrbiter->ReceivedMessage(SCREEN_Media_Player_Setup_Popup_Message.m_pMessage);
		}
		else
		{
			string sText=m_pOrbiter->m_mapTextString[TEXT_confirm_receiver_CONST] + "|" + m_pOrbiter->m_mapTextString[TEXT_YES_CONST] + "|" + m_pOrbiter->m_mapTextString[TEXT_Wrong_receiver_CONST]
				+ "|" + m_pOrbiter->m_mapTextString[TEXT_dont_control_my_receiver_CONST];
			string sMessage="0 -300 1 " TOSTRING(COMMAND_Goto_DesignObj_CONST) " " TOSTRING(COMMANDPARAMETER_PK_DesignObj_CONST) " " TOSTRING(DESIGNOBJ_ReceiverInputs_CONST) "|" 
				"0 -300 1 " TOSTRING(COMMAND_Goto_DesignObj_CONST) " " TOSTRING(COMMANDPARAMETER_PK_DesignObj_CONST) " " TOSTRING(DESIGNOBJ_NoReceiver_CONST) "|"
				"0 " + StringUtils::itos(m_pOrbiter->m_dwPK_Device_GeneralInfoPlugIn) + " 1 " TOSTRING(COMMAND_Delete_Device_CONST) " " TOSTRING(COMMANDPARAMETER_PK_Device_CONST) " " + StringUtils::itos(m_pWizardLogic->m_nPK_Device_Receiver) + "\n"
				"0 -300 1 " TOSTRING(COMMAND_Goto_Screen_CONST) " " TOSTRING(COMMANDPARAMETER_PK_Screen_CONST) " " TOSTRING(SCREEN_AV_Devices_CONST);

			DCE::SCREEN_Media_Player_Setup_Popup_Message SCREEN_Media_Player_Setup_Popup_Message(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device,
				sText,sMessage,"1");
			m_pOrbiter->ReceivedMessage(SCREEN_Media_Player_Setup_Popup_Message.m_pMessage);
		}
	}
	else
	{
		m_pWizardLogic->FindPnpDevices(TOSTRING(DEVICECATEGORY_AmpsPreampsReceiversTuners_CONST));
		m_pOrbiter->StartScreenHandlerTimer(500);
	}

	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &OSDScreenHandler::Receiver_ObjectSelected, new ObjectInfoBackData());
	RegisterCallBack(cbDataGridSelected, (ScreenHandlerCallBack) &OSDScreenHandler::Receiver_GridSelected, new DatagridCellBackData());
	RegisterCallBack(cbOnTimer,	(ScreenHandlerCallBack) &OSDScreenHandler::Receiver_OnTimer, new CallBackData());
	RegisterCallBack(cbOnRenderScreen, (ScreenHandlerCallBack) &OSDScreenHandler::Receiver_OnScreen, new RenderScreenCallBackData());

	if( !m_pWizardLogic->m_nPK_Device_Receiver && m_tLastDeviceAdded && time(NULL)-m_tLastDeviceAdded<10 ) // The user just recently added a new receiver or other device.  Wait a few seconds to confirm
	{
		m_pOrbiter->ForceCurrentScreenIntoHistory();
		string sText=m_pOrbiter->m_mapTextString[TEXT_Waiting_for_device_to_startup_CONST] + "|NO_BUTTON";
		DCE::SCREEN_Media_Player_Setup_Popup_Message SCREEN_Media_Player_Setup_Popup_Message(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device,
			sText,"","1");
		m_pOrbiter->ReceivedMessage(SCREEN_Media_Player_Setup_Popup_Message.m_pMessage);
		m_pOrbiter->StartScreenHandlerTimer(10000); // Wait 10 seconds before continuing
		m_tWaitingForRegistration=1;  // Special indicator for the timer that it should just return to this screen whenever it's called
	}
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::Receiver_OnScreen(CallBackData *pData)
{
	return false;
}

//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::Receiver_OnTimer(CallBackData *pData)
{
	if( m_tWaitingForRegistration==1 )
	{
		NeedToRender render( m_pOrbiter, "OSDScreenHandler::Receiver_OnTimer SCREEN_Receiver_CONST" );  // Redraw anything that was changed by this command
		m_pOrbiter->CMD_Goto_Screen("",SCREEN_Receiver_CONST);
		return false;
	}

	int PK_Device_Receiver = m_pWizardLogic->FindFirstDeviceInCategory(DEVICECATEGORY_AmpsPreampsReceiversTuners_CONST,m_pOrbiter->m_dwPK_Device);

	if(PK_Device_Receiver)
	{
		NeedToRender render2( m_pOrbiter, "OSDScreenHandler::Receiver_OnTimer" );  // Redraw anything that was changed by this command
		m_pOrbiter->CMD_Goto_DesignObj(0,TOSTRING(DESIGNOBJ_ReceiverInputs_CONST),"","",false,false);
		m_pWizardLogic->m_nPK_Device_Receiver = PK_Device_Receiver;
		m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST, StringUtils::itos(DatabaseUtils::GetDeviceTemplateForDevice(m_pWizardLogic,m_pWizardLogic->m_nPK_Device_Receiver)));
		return false;
	}
	return true;
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::Receiver_GridSelected(CallBackData *pData)
{
	DatagridCellBackData *pCellInfoData = (DatagridCellBackData *)pData;

	switch(GetCurrentScreen_PK_DesignObj())
	{
		case DESIGNOBJ_ReceiverManuf_CONST:
			{
				m_pWizardLogic->m_dwPK_Manufacturer = atoi(pCellInfoData->m_sValue.c_str());
				m_pOrbiter->CMD_Show_Object( TOSTRING(DESIGNOBJ_butReceiverModel_CONST), 0,"","", m_pWizardLogic->m_dwPK_Manufacturer ? "1" : "0" );
			}
			break;
		case DESIGNOBJ_ReceiverModel_CONST:
			{
				m_pWizardLogic->m_nPK_Device_Receiver = atoi(pCellInfoData->m_sValue.c_str());
				m_pOrbiter->CMD_Show_Object( TOSTRING(DESIGNOBJ_butReceiverInputs_CONST), 0,"","", m_pWizardLogic->m_nPK_Device_Receiver ? "1" : "0" );
			}
			break;
		case DESIGNOBJ_ReceiverInputs_CONST:
			{
				int PK_Command_Input = atoi(pCellInfoData->m_sValue.c_str());
				m_pOrbiter->CMD_Show_Object( TOSTRING(DESIGNOBJ_butAVDevices_CONST), 0,"","", PK_Command_Input ? "1" : "0" );
			}
			break;
		case DESIGNOBJ_SelectPort_CONST:
			{
				m_pOrbiter->CMD_Show_Object( TOSTRING(DESIGNOBJ_butSelectedPort_CONST), 0,"","", pCellInfoData->m_sValue.size() ? "1" : "0" );
			}
			break;
	}
	return false;
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::Receiver_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;

	switch(GetCurrentScreen_PK_DesignObj())
	{
		case DESIGNOBJ_Receiver_CONST:
		{
			if(DESIGNOBJ_butNoReceiver_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject )
			{
				string sTV,sInput;
				/*bool bHasTvInPath = */m_pWizardLogic->GetAvPath(m_pOrbiter->m_pData->m_dwPK_Device_ControlledVia,
														m_pWizardLogic->m_nPK_Device_TV,2,m_pWizardLogic->m_nPK_Command_Input_Video_On_TV,sTV,sInput);
				if( !m_pWizardLogic->m_nPK_Device_TV )
					m_pWizardLogic->m_nPK_Device_TV = m_pWizardLogic->FindFirstDeviceInCategory(DEVICECATEGORY_TVsPlasmasLCDsProjectors_CONST,m_pOrbiter->m_dwPK_Device,&sTV);

				if( m_pWizardLogic->m_nPK_Device_TV && m_pWizardLogic->m_nPK_Command_Input_Video_On_TV )
				{
					// No Receiver -- I use the tv for audio
					m_pWizardLogic->SetAvPath(m_pOrbiter->m_pData->m_dwPK_Device_ControlledVia,
											m_pWizardLogic->m_nPK_Device_TV,1,m_pWizardLogic->m_nPK_Command_Input_Video_On_TV); // 1 = audio
					m_pWizardLogic->DeleteDevicesInThisRoomOfType(DEVICECATEGORY_AmpsPreampsReceiversTuners_CONST);
				}
			}
		}
		break;

		case DESIGNOBJ_ReceiverManuf_CONST:
		{
			if(DESIGNOBJ_butReceiverModel_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				string sManufacturer = m_pOrbiter->m_mapVariable_Find(VARIABLE_Misc_Data_1_CONST);
				if(sManufacturer == "")
					return true;
			}
		}
		break;

		case DESIGNOBJ_ReceiverModel_CONST:
		{
			if(DESIGNOBJ_butReceiverInputs_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				string sModel = m_pOrbiter->m_mapVariable_Find(VARIABLE_Misc_Data_2_CONST);
				if(sModel == "")
					return true;

				int PK_CommMethod = DatabaseUtils::GetCommMethodForDeviceTemplate(m_pWizardLogic,atoi(sModel.c_str()));
				if( PK_CommMethod==COMMMETHOD_RS232_CONST )
				{
					m_pOrbiter->CMD_Goto_DesignObj(0,StringUtils::itos(DESIGNOBJ_SelectPort_CONST),"","",false,false);
					return true;  // We're redirecting the flow
				}
				else
				{
					// Delete receivers first since maybe the user is returning to this wizard
					m_pWizardLogic->DeleteDevicesInThisRoomOfType(DEVICECATEGORY_AmpsPreampsReceiversTuners_CONST);
					m_pWizardLogic->m_nPK_Device_Receiver = m_pWizardLogic->AddDevice(atoi(sModel.c_str()));
				}
			}
			else if(DESIGNOBJ_butModelNotListed2_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				m_pOrbiter->CMD_Goto_Screen("",SCREEN_AV_Devices_CONST);
			}
		}
		break;

		case DESIGNOBJ_SelectPort_CONST:
		{
            if(DESIGNOBJ_butSelectedPort_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				PLUTO_SAFETY_LOCK(vm, m_pOrbiter->m_VariableMutex);
				string sCell = m_pOrbiter->m_mapVariable_Find(VARIABLE_Datagrid_Input_CONST);
				string::size_type pos=0;
				int iPK_Device_ControlledVia = atoi(StringUtils::Tokenize(sCell,",",pos).c_str());
				string sPort = StringUtils::Tokenize(sCell,",",pos);
				int PK_DeviceTemplate = atoi(m_pOrbiter->m_mapVariable_Find(VARIABLE_Misc_Data_2_CONST).c_str());

				// Delete receivers first since maybe the user is returning to this wizard
				m_pWizardLogic->DeleteDevicesInThisRoomOfType(DEVICECATEGORY_AmpsPreampsReceiversTuners_CONST);
				m_pWizardLogic->m_nPK_Device_Receiver = m_pWizardLogic->AddDevice(PK_DeviceTemplate,
																				StringUtils::itos(DEVICEDATA_COM_Port_on_PC_CONST) + "|" + sPort,
																				iPK_Device_ControlledVia);

				m_pOrbiter->CMD_Goto_DesignObj(0,StringUtils::itos(DESIGNOBJ_ReceiverInputs_CONST),"","",true,false);
				return true;
			}
		}
		break;

		case DESIGNOBJ_ReceiverInputs_CONST:
        {
            if(DESIGNOBJ_butAVDevices_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
            {
                string sReceiverInputs = m_pOrbiter->m_mapVariable_Find(VARIABLE_Misc_Data_4_CONST);
                if(sReceiverInputs == "")
                    return true;

                //int PK_Device_From = 0;
                int PK_Command_Input = atoi(sReceiverInputs.c_str());

                // Set the input
                m_pWizardLogic->SetAvPath(m_pOrbiter->m_pData->m_dwPK_Device_ControlledVia,
                                          m_pWizardLogic->m_nPK_Device_Receiver,1 /* audio */,PK_Command_Input);

                // Video also goes to the receiver, and the receiver's video goes to the tv
                if( m_pWizardLogic->m_bUsingReceiverForVideo )
                {
                    m_pWizardLogic->SetAvPath(m_pOrbiter->m_pData->m_dwPK_Device_ControlledVia,
                                              m_pWizardLogic->m_nPK_Device_Receiver,2 /* video */,PK_Command_Input);
                    m_pWizardLogic->SetAvPath(m_pWizardLogic->m_nPK_Device_Receiver,
                                              m_pWizardLogic->m_nPK_Device_TV,2 /* video */,m_pWizardLogic->m_nPK_Command_Input_Video_On_TV);
                }
            }
			else if(DESIGNOBJ_butNoneAudioInputs_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
                m_pWizardLogic->SetAvPath(m_pOrbiter->m_pData->m_dwPK_Device_ControlledVia,
	                m_pWizardLogic->m_nPK_Device_Receiver,1 /* audio */, 0);

				if( m_pWizardLogic->m_bUsingReceiverForVideo )
				{
					m_pWizardLogic->SetAvPath(m_pOrbiter->m_pData->m_dwPK_Device_ControlledVia,
						m_pWizardLogic->m_nPK_Device_Receiver,2 /* video */,0);
				}
			}
        }
        break;
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
// Other AV Devices
//-----------------------------------------------------------------------------------------------------
void OSDScreenHandler::SCREEN_AV_Devices(long PK_Screen)
{
	PrepForWizard();
	m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_DesignObj_CurrentSecti_CONST, TOSTRING(DESIGNOBJ_butAVDevicesWizard_CONST));
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Datagrid_Filter_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_3_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Datagrid_Input_CONST, "");

	ScreenHandlerBase::SCREEN_AV_Devices(PK_Screen);

	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &OSDScreenHandler::AV_Devices_ObjectSelected, new ObjectInfoBackData());
	RegisterCallBack(cbDataGridSelected, (ScreenHandlerCallBack) &OSDScreenHandler::AV_Devices_GridSelected, 
		new DatagridCellBackData());
}
//-----------------------------------------------------------------------------------------------------

bool OSDScreenHandler::AV_Devices_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;

	switch(GetCurrentScreen_PK_DesignObj())
	{
		case DESIGNOBJ_AVDevices_CONST:
		{
			if(DESIGNOBJ_butAVDevicesManuf_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				string sDeviceType = m_pOrbiter->m_mapVariable_Find(VARIABLE_Datagrid_Filter_CONST);
				if(sDeviceType == "")
					return true;
			}
		}
		break;

		case DESIGNOBJ_AVDeviceManuf_CONST:
		{
			if(DESIGNOBJ_butAVDeviceModel_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				string sManufacturer = m_pOrbiter->m_mapVariable_Find(VARIABLE_Misc_Data_1_CONST);
				if(sManufacturer == "")
					return true;
			}
		}
		break;

		case DESIGNOBJ_AVDeviceModel_CONST:
		{
			if(DESIGNOBJ_butAVOutputs_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				string sModel = m_pOrbiter->m_mapVariable_Find(VARIABLE_Misc_Data_2_CONST);
				if(sModel == "")
					return true;

				//we have the AV Device Manufacturer and AV Device Model!
				m_pWizardLogic->m_nPK_Device_Last_AV = m_pWizardLogic->AddDevice(atoi(sModel.c_str()));
				m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST,StringUtils::itos(m_pWizardLogic->m_nPK_Device_Last_AV));
			}
			else if(DESIGNOBJ_butModelNotListed2_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				m_pOrbiter->CMD_Goto_Screen("",SCREEN_Get_Capture_Card_Port_CONST);
			}
		}
		break;

		case DESIGNOBJ_AVDeviceVideo_CONST:
		{
			if(DESIGNOBJ_butAVDeviceVideoInputs_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				string sInstalledAVDevice = m_pOrbiter->m_mapVariable_Find(VARIABLE_Misc_Data_3_CONST);
				if(sInstalledAVDevice == "")
					return true;

				m_pWizardLogic->m_nPK_Device_AV_Installed = atoi(sInstalledAVDevice.c_str());
				m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_3_CONST,
                                             StringUtils::itos(DatabaseUtils::GetDeviceTemplateForDevice(m_pWizardLogic,m_pWizardLogic->m_nPK_Device_AV_Installed)));
			}
		}
		break;

		case DESIGNOBJ_AVDeviceVideoInputs_CONST:
		{
			if(DESIGNOBJ_butAVDeviceAudio_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				string sInput = m_pOrbiter->m_mapVariable_Find(VARIABLE_Misc_Data_4_CONST);
				if(sInput != "")
				{
					m_pWizardLogic->SetAvPath(m_pWizardLogic->m_nPK_Device_Last_AV,
                                              m_pWizardLogic->m_nPK_Device_AV_Installed, 2 /* video */, atoi(sInput.c_str()));
				}
			}
			else if(DESIGNOBJ_butNoneVideoInputs_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				m_pWizardLogic->SetAvPath(m_pWizardLogic->m_nPK_Device_Last_AV,
		            m_pWizardLogic->m_nPK_Device_AV_Installed, 2 /* video */, 0);
			}
		}
		break;

		case DESIGNOBJ_AVDeviceAudio_CONST:
		{
			if(DESIGNOBJ_butAVDeviceAudioInputs_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				string sInstalledAVDevice = m_pOrbiter->m_mapVariable_Find(VARIABLE_Misc_Data_3_CONST);
				if(sInstalledAVDevice == "")
					return true;

				m_pWizardLogic->m_nPK_Device_AV_Installed = atoi(sInstalledAVDevice.c_str());
				m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_3_CONST,
                                             StringUtils::itos(DatabaseUtils::GetDeviceTemplateForDevice(m_pWizardLogic,m_pWizardLogic->m_nPK_Device_AV_Installed)));
			}
		}
		break;

		case DESIGNOBJ_AVDeviceAudioInputs_CONST:
		{
			if(DESIGNOBJ_butMoreAVDevices_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				string sInput = m_pOrbiter->m_mapVariable_Find(VARIABLE_Misc_Data_4_CONST);
				if(sInput != "")
				{
					m_pWizardLogic->SetAvPath(m_pWizardLogic->m_nPK_Device_Last_AV,
                                              m_pWizardLogic->m_nPK_Device_AV_Installed, 1 /* audio */, atoi(sInput.c_str()));
				}
			}
			else if(DESIGNOBJ_butNoneAudioInputs_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				m_pWizardLogic->SetAvPath(m_pWizardLogic->m_nPK_Device_Last_AV,
		            m_pWizardLogic->m_nPK_Device_AV_Installed, 1 /* audio */, 0);
			}
		}
		break;
	}

	return false;
}

bool OSDScreenHandler::AV_Devices_GridSelected(CallBackData *pData)
{
	DatagridCellBackData *pCellInfoData = (DatagridCellBackData *)pData;

	switch(GetCurrentScreen_PK_DesignObj())
	{
		case DESIGNOBJ_AVDevices_CONST:
			{
				int PK_DeviceCategory = atoi(pCellInfoData->m_sValue.c_str());
				m_pOrbiter->CMD_Show_Object( TOSTRING(DESIGNOBJ_butAVDevicesManuf_CONST), 0,"","", PK_DeviceCategory ? "1" : "0" );
			}
			break;
		case DESIGNOBJ_AVDeviceManuf_CONST:
			{
				int PK_Manufacturer = atoi(pCellInfoData->m_sValue.c_str());
				m_pOrbiter->CMD_Show_Object( TOSTRING(DESIGNOBJ_butAVDeviceModel_CONST), 0,"","",PK_Manufacturer ? "1" : "0" );
			}
			break;
		case DESIGNOBJ_AVDeviceModel_CONST:
			{
				int PK_DeviceTemplate_AV = atoi(pCellInfoData->m_sValue.c_str());
				m_pOrbiter->CMD_Show_Object( TOSTRING(DESIGNOBJ_butAVOutputs_CONST), 0,"","", PK_DeviceTemplate_AV ? "1" : "0" );
			}
			break;
		case DESIGNOBJ_AVDeviceAudio_CONST:
			{
				int PK_Device = atoi(pCellInfoData->m_sValue.c_str());
				m_pOrbiter->CMD_Show_Object( TOSTRING(DESIGNOBJ_butAVDeviceAudioInputs_CONST), 0,"","", PK_Device ? "1" : "0" );
			}
			break;
		case DESIGNOBJ_AVDeviceVideo_CONST:
			{
				int PK_Device = atoi(pCellInfoData->m_sValue.c_str());
				m_pOrbiter->CMD_Show_Object( TOSTRING(DESIGNOBJ_butAVDeviceVideoInputs_CONST), 0,"","", PK_Device ? "1" : "0" );
			}
			break;
	}
	return false;
}

void OSDScreenHandler::SCREEN_Wizard_Done(long PK_Screen)
{
	PrepForWizard();
	if( m_pWizardLogic->HouseAlreadySetup()==false )
	{
		DCE::SCREEN_VideoWizard _SCREEN_VideoWizard(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device);
		m_pOrbiter->ReceivedMessage(_SCREEN_VideoWizard.m_pMessage);
		return;
	}
	else if( m_pWizardLogic->WhatRoomIsThisDeviceIn(m_pWizardLogic->GetTopMostDevice(m_pOrbiter->m_dwPK_Device))==0 )
	{
		string sMessage="0 -300 1 " TOSTRING(COMMAND_Goto_Screen_CONST) " " TOSTRING(COMMANDPARAMETER_PK_Screen_CONST) " " TOSTRING(SCREEN_This_Room_CONST);

		DCE::SCREEN_Media_Player_Setup_Popup_Message SCREEN_Media_Player_Setup_Popup_Message(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device,
			m_pOrbiter->m_mapTextString[TEXT_Must_pick_room_CONST] + "|" + m_pOrbiter->m_mapTextString[TEXT_Ok_CONST],sMessage,"1");
		m_pOrbiter->ReceivedMessage(SCREEN_Media_Player_Setup_Popup_Message.m_pMessage);
		return;
	}

	m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_DesignObj_CurrentSecti_CONST, TOSTRING(DESIGNOBJ_butDoneMediaSetup_CONST));
	ScreenHandler::SCREEN_Wizard_Done(PK_Screen);
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"OSDScreenHandler::SCREEN_Wizard_Done selected DESIGNOBJ_Final_CONST already played %d",(int) m_bAlreadyPlayFinalGreeting);
#endif
	RegisterCallBack(cbOnRenderScreen, (ScreenHandlerCallBack) &OSDScreenHandler::VideoWizardDone_OnScreen, new RenderScreenCallBackData());
	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &OSDScreenHandler::VideoWizard_ObjectSelected, new ObjectInfoBackData());
}

bool OSDScreenHandler::VideoWizardDone_OnScreen(CallBackData *pData)
{
	//RenderScreenCallBackData *pRenderScreenCallBackData = (RenderScreenCallBackData *)pData;
	if( !m_bAlreadyPlayFinalGreeting )
	{
		m_bAlreadyPlayFinalGreeting=true;
		DCE::CMD_Play_Media CMD_Play_Media(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_LocalMediaPlayer,0,0,"","/home/videowiz/final.mpg");
		m_pOrbiter->SendCommand(CMD_Play_Media);
	}
	return false;
}

void OSDScreenHandler::SCREEN_House_Setup_Popup_Message(long PK_Screen, string sText, string sCommand_Line, string sCannotGoBack)
{
	PrepForWizard();
	SCREEN_PopupMessage(PK_Screen, sText, sCommand_Line, "", "", "", sCannotGoBack);
}

void OSDScreenHandler::SCREEN_Media_Player_Setup_Popup_Message(long PK_Screen, string sText, string sCommand_Line, string sCannotGoBack)
{
	PrepForWizard();
	SCREEN_PopupMessage(PK_Screen, sText, sCommand_Line, "", "", "", sCannotGoBack);
}

void OSDScreenHandler::HandleLightingScreen()
{
	NeedToRender render2( m_pOrbiter, "OSDScreenHandler::HandleLightingScreen" );  // Redraw anything that was changed by this command
	int PK_DeviceTemplate = DatabaseUtils::GetDeviceTemplateForDevice(m_pWizardLogic,m_pWizardLogic->m_nPK_Device_Lighting);
	int NumLights = m_pWizardLogic->GetNumLights();
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, StringUtils::itos(NumLights));
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST,StringUtils::itos((int) m_pWizardLogic->m_dequeNumLights.size()));
	m_nLightInDequeToAssign=0;

	if( PK_DeviceTemplate==DEVICETEMPLATE_ZWave_CONST )
	{
		if( NumLights )
		{
			m_pOrbiter->ForceCurrentScreenIntoHistory();
			if( m_pWizardLogic->m_dequeNumLights.size()>0 )
			{
				// There are unassigned lights.  If user choses yes, assign lights.  No, he will re-pair
				string sText=m_pOrbiter->m_mapTextString[TEXT_Already_paired_lights_CONST] + "|" + m_pOrbiter->m_mapTextString[TEXT_YES_CONST] + "|" + m_pOrbiter->m_mapTextString[TEXT_No_add_lights_again_CONST];
				string sMessage="0 -300 1 " TOSTRING(COMMAND_Goto_DesignObj_CONST) " " TOSTRING(COMMANDPARAMETER_PK_DesignObj_CONST) " " TOSTRING(DESIGNOBJ_LightsSetupInclude_CONST) "|" 
					"0 -300 1 " TOSTRING(COMMAND_Goto_DesignObj_CONST) " " TOSTRING(COMMANDPARAMETER_PK_DesignObj_CONST) " " TOSTRING(DESIGNOBJ_Explain_Pair_ZWave_Lights_CONST);

				DCE::SCREEN_House_Setup_Popup_Message SCREEN_House_Setup_Popup_Message(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device,
					sText,sMessage,"1");
				m_pOrbiter->ReceivedMessage(SCREEN_House_Setup_Popup_Message.m_pMessage);
			}
			else
			{
				// There are lights, and all are assigned.  If user choses yes, skip ahead to alarm panel.  No, he will re-pair
				string sText=m_pOrbiter->m_mapTextString[TEXT_Already_paired_lights_CONST] + "|" + m_pOrbiter->m_mapTextString[TEXT_YES_CONST] + "|" +m_pOrbiter->m_mapTextString[TEXT_No_add_lights_again_CONST];
				string sMessage="0 -300 1 " TOSTRING(COMMAND_Goto_Screen_CONST) " " TOSTRING(COMMANDPARAMETER_PK_Screen_CONST) " " TOSTRING(SCREEN_AlarmPanel_CONST) "|" 
					"0 -300 1 " TOSTRING(COMMAND_Goto_DesignObj_CONST) " " TOSTRING(COMMANDPARAMETER_PK_DesignObj_CONST) " " TOSTRING(DESIGNOBJ_Explain_Pair_ZWave_Lights_CONST);

				DCE::SCREEN_House_Setup_Popup_Message SCREEN_House_Setup_Popup_Message(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device,
					sText,sMessage,"1");
				m_pOrbiter->ReceivedMessage(SCREEN_House_Setup_Popup_Message.m_pMessage);
			}
		}
		else
			// There are no lights.  Help the user pair them
			m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(DESIGNOBJ_Explain_Pair_ZWave_Lights_CONST),
										"", "", false, false );
	}
	else if( NumLights )
	{
		if( m_pWizardLogic->m_dequeNumLights.size()>0 )
			m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(DESIGNOBJ_LightsSetupInclude_CONST),
										"", "", false, false );
		else
			m_pOrbiter->CMD_Goto_Screen("",SCREEN_AlarmPanel_CONST);
	}
	else
	{
		m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, DatabaseUtils::GetDescriptionForDevice(m_pWizardLogic,m_pWizardLogic->m_nPK_Device_Lighting));
		string sText=m_pOrbiter->m_mapTextString[TEXT_Lighting_interface_with_no_lights_CONST] + "|" + m_pOrbiter->m_mapTextString[TEXT_Ok_CONST];
		string sMessage="0 -300 1 " TOSTRING(COMMAND_Goto_Screen_CONST) " " TOSTRING(COMMANDPARAMETER_PK_Screen_CONST) " " TOSTRING(SCREEN_AlarmPanel_CONST);

		m_pOrbiter->ForceCurrentScreenIntoHistory();
		DCE::SCREEN_House_Setup_Popup_Message SCREEN_House_Setup_Popup_Message(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device,
			sText,sMessage,"1");
		m_pOrbiter->ReceivedMessage(SCREEN_House_Setup_Popup_Message.m_pMessage);
	}
}

//-----------------------------------------------------------------------------------------------------
void OSDScreenHandler::SCREEN_LightsSetup(long PK_Screen)
{
	PrepForWizard();
	m_tWaitingForRegistration=m_tRegistered=0;
	m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_DesignObj_CurrentSecti_CONST, TOSTRING(DESIGNOBJ_butLightsWizard_CONST)); 
	ScreenHandler::SCREEN_LightsSetup(PK_Screen);

	m_pWizardLogic->m_nPK_Device_Lighting =	m_pWizardLogic->FindFirstDeviceInCategory(DEVICECATEGORY_Lighting_Interface_CONST,0);
	if( m_pWizardLogic->m_nPK_Device_Lighting )
	{
		char cRegistered = m_pOrbiter->DeviceIsRegistered(m_pWizardLogic->m_nPK_Device_Lighting);
		if( cRegistered != 'Y' )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"OSDScreenHandler::SCREEN_LightsSetup interface hasn't registered yet");
			m_pOrbiter->StartScreenHandlerTimer(500);
		}
		else
			HandleLightingScreen();
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"OSDScreenHandler::SCREEN_LightsSetup setting pnp devices");
		m_pWizardLogic->FindPnpDevices(TOSTRING(DEVICECATEGORY_Lighting_Interface_CONST));
		m_pOrbiter->StartScreenHandlerTimer(500);
	}

	m_nLightInDequeToAssign=0;

	RegisterCallBack(cbMessageIntercepted, (ScreenHandlerCallBack) &OSDScreenHandler::LightsSetup_Intercepted, new MsgInterceptorCellBackData());
	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &OSDScreenHandler::LightsSetup_ObjectSelected, new ObjectInfoBackData());
	RegisterCallBack(cbDataGridSelected, (ScreenHandlerCallBack) &OSDScreenHandler::LightsSetup_SelectedGrid, new DatagridCellBackData());
	RegisterCallBack(cbOnRenderScreen, (ScreenHandlerCallBack) &OSDScreenHandler::LightsSetup_OnScreen, new RenderScreenCallBackData());
	RegisterCallBack(cbOnGotoScreen, (ScreenHandlerCallBack) &OSDScreenHandler::LightsSetup_OnGotoScreen, new GotoScreenCallBackData());
	RegisterCallBack(cbOnTimer,	(ScreenHandlerCallBack) &OSDScreenHandler::Lights_OnTimer, new CallBackData());
}
//-----------------------------------------------------------------------------------------------------

bool OSDScreenHandler::LightsSetup_OnGotoScreen(CallBackData *pData)
{
	//GotoScreenCallBackData *pGotoScreenCallBackData = (GotoScreenCallBackData *)pData;
	m_bLightsFlashThreadQuit=true;
	return false;
}

void *LightsFlashThread(void *p)
{
	OSDScreenHandler* pOSDScreenHandler = (OSDScreenHandler *)p;
	pOSDScreenHandler->LightsSetup_Timer();
	return NULL;
}

bool OSDScreenHandler::LightsSetup_OnScreen(CallBackData *pData)
{
	RenderScreenCallBackData *pRenderScreenCallBackData = (RenderScreenCallBackData *)pData;
	if( pRenderScreenCallBackData->m_pObj->m_iBaseObjectID==DESIGNOBJ_LightSetupRooms_CONST )
	{
		if( m_nLightInDequeToAssign < (int) m_pWizardLogic->m_dequeNumLights.size() )
		{
			PLUTO_SAFETY_LOCK(vm,m_pOrbiter->m_VariableMutex);
			if( !m_bLightsFlashThreadRunning )
			{
				m_bLightsFlashThreadRunning=true;
				m_bLightsFlashThreadQuit=false;
				pthread_t t;
				pthread_create(&t, NULL, LightsFlashThread, (void*)this);
			}
		}
		else
		{
			m_pOrbiter->CMD_Goto_Screen("",SCREEN_AlarmPanel_CONST);
			m_pOrbiter->CMD_Refresh("");
		}
	}
	return false;
}

bool OSDScreenHandler::Lights_OnTimer(CallBackData *pData)
{
	if( GetCurrentScreen_PK_DesignObj()==DESIGNOBJ_CopyZWaveData_CONST )
	{
		int iNumLightsTotal = m_pWizardLogic->GetNumLights();
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"OSDScreenHandler::Lights_OnTimer got EVENT_Reporting_Child_Devices_CONST %d lights",iNumLightsTotal);

		if( iNumLightsTotal==0 )
		{
			DesignObjText *pText = m_pOrbiter->FindText( m_pOrbiter->FindObject(DESIGNOBJ_NoLights_CONST),TEXT_STATUS_CONST );
			if( pText )
				pText->m_sText = m_pOrbiter->m_mapTextString[TEXT_No_Lights_Found_CONST];
			m_pOrbiter->CMD_Refresh("");
			return false;
		}

		m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST,StringUtils::itos(iNumLightsTotal));
		m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST,StringUtils::itos((int) m_pWizardLogic->m_dequeNumLights.size()));

		NeedToRender render2( m_pOrbiter, "OSDScreenHandler::Lights_OnTimer" );  // Redraw after the goto designobj below

		if( m_pWizardLogic->m_dequeNumLights.size()>0 )
		{
			m_nLightInDequeToAssign=0;
			m_pOrbiter->CMD_Goto_DesignObj(0,StringUtils::itos(DESIGNOBJ_LightsSetupInclude_CONST),"","",false,false);
		}
		else
			m_pOrbiter->CMD_Goto_Screen("",SCREEN_AlarmPanel_CONST);
		return false;
	}
	else if( GetCurrentScreen_PK_DesignObj()==DESIGNOBJ_NoLights_CONST || m_tWaitingForRegistration || m_tRegistered )
	{
		m_pWizardLogic->m_nPK_Device_Lighting =	m_pWizardLogic->FindFirstDeviceInCategory(DEVICECATEGORY_Lighting_Interface_CONST,0);
LoggerWrapper::GetInstance()->Write(LV_STATUS,"OSDScreenHandler::Lights_OnTimer light %d waiting %d",m_pWizardLogic->m_nPK_Device_Lighting,(int) m_tWaitingForRegistration );
		if( m_pWizardLogic->m_nPK_Device_Lighting )
		{
			char cRegistered = m_tRegistered ? 'Y' : m_pOrbiter->DeviceIsRegistered(m_pWizardLogic->m_nPK_Device_Lighting);
LoggerWrapper::GetInstance()->Write(LV_STATUS,"SDScreenHandler::Lights_OnTimer registered %c", cRegistered);
			if( cRegistered=='Y' )
			{
				// Wait another 10 seconds after the lighting device first registers so it has time to report all it's children
				if( !m_tRegistered )
				{
LoggerWrapper::GetInstance()->Write(LV_STATUS,"SDScreenHandler::Lights_OnTimer now registered, waiting 5 secs");
					m_tRegistered=time(NULL);
					m_tWaitingForRegistration=0;
					return true;
				}
				else if( time(NULL)-m_tRegistered<10 )
				{
LoggerWrapper::GetInstance()->Write(LV_STATUS,"SDScreenHandler::Lights_OnTimer more time to startup");
					return true;
				}
LoggerWrapper::GetInstance()->Write(LV_STATUS,"SDScreenHandler::Lights_OnTimer now registered");
				HandleLightingScreen();
				return false;  // Kill the timer
			}
			else if( !m_tWaitingForRegistration )
			{
				m_tWaitingForRegistration = time(NULL);
				string sText=m_pOrbiter->m_mapTextString[TEXT_Waiting_for_device_to_startup_CONST] + "|NO_BUTTON";
				DCE::SCREEN_House_Setup_Popup_Message SCREEN_House_Setup_Popup_Message(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device,
					sText,"","1");
				m_pOrbiter->ReceivedMessage(SCREEN_House_Setup_Popup_Message.m_pMessage);

LoggerWrapper::GetInstance()->Write(LV_STATUS,"SDScreenHandler::Lights_OnTimer displaying waiting message %d",(int) m_tWaitingForRegistration);
			}
			else if( time(NULL)-m_tWaitingForRegistration>60 )
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"OSDScreenHandler::Lights_OnTimer timed out waiting for %d",m_pWizardLogic->m_nPK_Device_Lighting);
				m_tWaitingForRegistration=0;

				string sText=m_pOrbiter->m_mapTextString[TEXT_Device_did_not_start_CONST] + "|" + m_pOrbiter->m_mapTextString[TEXT_Ok_CONST];
				string sMessage="0 -300 1 " TOSTRING(COMMAND_Goto_Screen_CONST) " " TOSTRING(COMMANDPARAMETER_PK_Screen_CONST) " " TOSTRING(SCREEN_AlarmPanel_CONST);

				DCE::SCREEN_House_Setup_Popup_Message SCREEN_House_Setup_Popup_Message(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device,
					sText,sMessage,"1");
				m_pOrbiter->ReceivedMessage(SCREEN_House_Setup_Popup_Message.m_pMessage);

LoggerWrapper::GetInstance()->Write(LV_STATUS,"SDScreenHandler::Lights_OnTimer registration failed");
				return false;
			}
		}
	}
LoggerWrapper::GetInstance()->Write(LV_STATUS,"SDScreenHandler::Lights_OnTimer keeping timer");
	return true;
}

void OSDScreenHandler::LightsSetup_Timer()
{
	// We're going to create our own sockets since we're looking for delivery confirmation on
	// the flashing commands so we can report status to the user.  These may be slow, so do
	// this on it's own socket so it doesn't block sending/receiving datagrads and other ui stuff
	Event_Impl event_Impl(m_pOrbiter->m_dwPK_Device, 0, m_pOrbiter->m_sHostName);

	static bool bLastTimeOn=true;
	while( !m_bLightsFlashThreadQuit && m_nLightInDequeToAssign < (int) m_pWizardLogic->m_dequeNumLights.size() )
	{
		PLUTO_SAFETY_LOCK(vm,m_pOrbiter->m_VariableMutex);
		string sID = m_pWizardLogic->m_dequeNumLights[m_nLightInDequeToAssign].second;
		int PK_Device = m_pWizardLogic->m_dequeNumLights[m_nLightInDequeToAssign].first;
		string sDescription = DatabaseUtils::GetDescriptionForDevice(m_pWizardLogic,PK_Device);

		string sText = string("Turning ") + (bLastTimeOn ? "OFF " : "ON ") + StringUtils::itos(PK_Device) + " " + sDescription + "...";
		DesignObj_Orbiter *pObjStatus = m_pOrbiter->FindObject(StringUtils::itos(DESIGNOBJ_LightSetupRooms_CONST) + ".0.0." + StringUtils::itos(DESIGNOBJ_objLightStatus_CONST));
		DesignObjText *pText = m_pOrbiter->FindText( pObjStatus,TEXT_STATUS_CONST );
		if( pText )
			pText->m_sText = sText;
		vm.Release();

		{
			if(NULL != pObjStatus)
				m_pOrbiter->Renderer()->RenderObjectAsync(pObjStatus);
			/*
              PLUTO_SAFETY_LOCK( nd, m_pOrbiter->m_NeedRedrawVarMutex );
              if( pObjStatus )
              m_pOrbiter->m_vectObjs_NeedRedraw.push_back(pObjStatus);
              nd.Release();
			*/
			NeedToRender render2( m_pOrbiter, "OSDScreenHandler::LightsSetup_Timer1" );  // Redraw anything that was changed by this command
		}

		string sResponse;
		if( !bLastTimeOn )
		{
			DCE::CMD_Send_Command_To_Child CMD_Send_Command_To_Child(m_pOrbiter->m_dwPK_Device,m_pWizardLogic->m_nPK_Device_Lighting, 
                                                                     sID,COMMAND_Generic_On_CONST,"");
			event_Impl.SendMessage(CMD_Send_Command_To_Child.m_pMessage,sResponse);
		}
		else
		{
			DCE::CMD_Send_Command_To_Child CMD_Send_Command_To_Child(m_pOrbiter->m_dwPK_Device,m_pWizardLogic->m_nPK_Device_Lighting, 
                                                                     sID,COMMAND_Generic_Off_CONST,"");
			event_Impl.SendMessage(CMD_Send_Command_To_Child.m_pMessage,sResponse);
		}
		bLastTimeOn=!bLastTimeOn;

		vm.Relock();
		if( pText )
			pText->m_sText = sText + "(" + sResponse  + ")";
		vm.Release();

		{
			if(NULL != pObjStatus)
				m_pOrbiter->Renderer()->RenderObjectAsync(pObjStatus);
			/*
              PLUTO_SAFETY_LOCK( nd, m_pOrbiter->m_NeedRedrawVarMutex );
              if( pObjStatus )
              m_pOrbiter->m_vectObjs_NeedRedraw.push_back(pObjStatus);
			*/
			NeedToRender render2( m_pOrbiter, "OSDScreenHandler::LightsSetup_Timer2" );  // Redraw anything that was changed by this command
		}

		time_t timeout = time(NULL) + 4;
		int nLightInDequeToAssign=m_nLightInDequeToAssign;
		while(timeout>time(NULL) && nLightInDequeToAssign==m_nLightInDequeToAssign && !m_bLightsFlashThreadQuit)
			Sleep(500);  // max 500 ms delay
	}
	m_bLightsFlashThreadRunning=false;
}

bool OSDScreenHandler::LightsSetup_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;

	switch(GetCurrentScreen_PK_DesignObj())
	{
		case DESIGNOBJ_Explain_Pair_ZWave_Lights_CONST:
		{
			if(DESIGNOBJ_butZwaveLightsDownload_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				if( !m_dwMessageInterceptorCounter_ReportingChildDevices )
				{
					m_dwMessageInterceptorCounter_ReportingChildDevices = m_pOrbiter->RegisterMsgInterceptor((MessageInterceptorFn)(&Orbiter::ScreenHandlerMsgInterceptor),0,0,0,0,MESSAGETYPE_EVENT,EVENT_Reporting_Child_Devices_CONST);
					m_pOrbiter->RegisterMsgInterceptor((MessageInterceptorFn)(&Orbiter::ScreenHandlerMsgInterceptor),0,0,0,0,MESSAGETYPE_EVENT,EVENT_Download_Config_Done_CONST);
				}
				DesignObjText *pText = m_pOrbiter->FindText( m_pOrbiter->FindObject(DESIGNOBJ_NoLights_CONST),TEXT_STATUS_CONST );
				if( pText )
					pText->m_sText = m_pOrbiter->m_mapTextString[TEXT_Lights_Setup_CONST];

				string sResponse;
				DCE::CMD_Download_Configuration CMD_Download_Configuration(m_pOrbiter->m_dwPK_Device,m_pWizardLogic->m_nPK_Device_Lighting,"");
				if( !m_pOrbiter->SendCommand(CMD_Download_Configuration,&sResponse) )
				{
					DesignObjText *pText = m_pOrbiter->FindText( m_pOrbiter->FindObject(DESIGNOBJ_CopyZWaveData_CONST),TEXT_STATUS_CONST );
					if( pText )
						pText->m_sText = m_pOrbiter->m_mapTextString[TEXT_Error_getting_data_CONST] + ": ZWave interface not responding";
				}
			}
		}
		break;
		case DESIGNOBJ_LightSetupRooms_CONST:
		{
			if(DESIGNOBJ_butLightType2_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				int PK_Room = atoi(m_pOrbiter->m_mapVariable_Find(VARIABLE_Datagrid_Input_CONST).c_str());
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"OSDScreenHandler::LightsSetup_ObjectSelected DESIGNOBJ_LightSetupRooms_CONST device %d room %d",
					m_pWizardLogic->m_dequeNumLights[m_nLightInDequeToAssign].first,PK_Room);
				if( !PK_Room || !DatabaseUtils::SetDeviceInRoom(m_pWizardLogic,m_pWizardLogic->m_dequeNumLights[m_nLightInDequeToAssign].first,PK_Room) )
					return true;  // This room isn't valid
			}
		}
		break;
		case DESIGNOBJ_LightType_CONST:
		{
			if(DESIGNOBJ_butLightName2_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				string sType = m_pOrbiter->m_mapVariable_Find(VARIABLE_Misc_Data_1_CONST);
				DatabaseUtils::SetDeviceData(m_pWizardLogic,m_pWizardLogic->m_dequeNumLights[m_nLightInDequeToAssign].first,
											DEVICEDATA_PK_FloorplanObjectType_CONST,sType);
				string sDefaultLightName = DatabaseUtils::GetDescriptionFromTable(m_pWizardLogic,FLOORPLANOBJECTTYPE_TABLE,atoi(sType.c_str()));
				m_pOrbiter->CMD_Set_Variable(VARIABLE_Seek_Value_CONST, sDefaultLightName);
			}
		}
		break;
		case DESIGNOBJ_LightName_CONST:
		{
			if(DESIGNOBJ_butLightsSetupRoom_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				DatabaseUtils::SetDescriptionForDevice(m_pWizardLogic,m_pWizardLogic->m_dequeNumLights[m_nLightInDequeToAssign].first,m_pOrbiter->m_mapVariable_Find(VARIABLE_Seek_Value_CONST));
				m_nLightInDequeToAssign++;
			}
		}
		break;
	};

	return false;
}

bool OSDScreenHandler::LightsSetup_SelectedGrid(CallBackData *pData)
{
	DatagridCellBackData *pDatagridCellBackData = (DatagridCellBackData *) pData;
	if( pDatagridCellBackData->m_nPK_Datagrid == DATAGRID_Rooms_CONST )
	{
		int PK_Room = atoi(pDatagridCellBackData->m_sValue.c_str());
		m_pOrbiter->CMD_Show_Object( TOSTRING(DESIGNOBJ_butLightType2_CONST), 0,"","", PK_Room ? "1" : "0" );
	}
	else if( pDatagridCellBackData->m_nPK_Datagrid == DATAGRID_LightType_CONST )
	{
		int PK_FloorplanObjecType = atoi(pDatagridCellBackData->m_sValue.c_str());
		m_pOrbiter->CMD_Show_Object( TOSTRING(DESIGNOBJ_butLightName2_CONST), 0,"","", PK_FloorplanObjecType ? "1" : "0" );
	}

	return false;
}

//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::LightsSetup_Intercepted(CallBackData *pData)
{
	MsgInterceptorCellBackData *pMsgInterceptorCellBackData = (MsgInterceptorCellBackData *) pData;
	if(GetCurrentScreen_PK_DesignObj()==DESIGNOBJ_CopyZWaveData_CONST)
	{
		if( pMsgInterceptorCellBackData->m_pMessage->m_dwMessage_Type==MESSAGETYPE_EVENT &&
            pMsgInterceptorCellBackData->m_pMessage->m_dwID==EVENT_Download_Config_Done_CONST )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"OSDScreenHandler::LightsSetup_Intercepted got EVENT_Download_Config_Done_CONST");
			DesignObjText *pText = m_pOrbiter->FindText( m_pOrbiter->FindObject(DESIGNOBJ_NoLights_CONST),TEXT_STATUS_CONST );
			string sResult = pMsgInterceptorCellBackData->m_pMessage->m_mapParameters[EVENTPARAMETER_Error_Message_CONST];

			if( sResult.size() )
			{
				if( pText )
					pText->m_sText = m_pOrbiter->m_mapTextString[TEXT_Error_getting_data_CONST] + ": " + sResult;
				m_pOrbiter->CMD_Refresh("");
				return false;
			}
			if( pText )
				pText->m_sText = m_pOrbiter->m_mapTextString[TEXT_Monster_Remote_Found_CONST];

			DCE::CMD_Report_Child_Devices CMD_Report_Child_Devices(m_pOrbiter->m_dwPK_Device,m_pWizardLogic->m_nPK_Device_Lighting);
			m_pOrbiter->SendCommand(CMD_Report_Child_Devices);
			m_pOrbiter->CMD_Refresh("");
			return false;
		}
		if( pMsgInterceptorCellBackData->m_pMessage->m_dwMessage_Type==MESSAGETYPE_EVENT &&
            pMsgInterceptorCellBackData->m_pMessage->m_dwID==EVENT_Reporting_Child_Devices_CONST )
		{
			string sResult = pMsgInterceptorCellBackData->m_pMessage->m_mapParameters[EVENTPARAMETER_Error_Message_CONST];
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"OSDScreenHandler::LightsSetup_Intercepted got EVENT_Reporting_Child_Devices_CONST %s",sResult.c_str());
			if( sResult.size() )
			{
				DesignObjText *pText = m_pOrbiter->FindText( m_pOrbiter->FindObject(DESIGNOBJ_NoLights_CONST),TEXT_STATUS_CONST );
				if( pText )
					pText->m_sText = m_pOrbiter->m_mapTextString[TEXT_Error_getting_data_CONST] + ": " + sResult;
				m_pOrbiter->CMD_Refresh("");
				return false;
			}

			m_pOrbiter->StartScreenHandlerTimer(2000);  // Wait 2 seconds for the lights to be created

			return false;
		}
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
void OSDScreenHandler::HandleAlarmScreen()
{
	NeedToRender render2( m_pOrbiter, "OSDScreenHandler::HandleAlarmScreen" );  // Redraw anything that was changed by this command
	/*int PK_DeviceTemplate = */DatabaseUtils::GetDeviceTemplateForDevice(m_pWizardLogic,m_pWizardLogic->m_nPK_Device_AlarmPanel);
	int NumSensors = m_pWizardLogic->GetNumSensors();
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, StringUtils::itos(NumSensors));
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST,StringUtils::itos((int) m_pWizardLogic->m_dequeNumSensors.size()));
	m_nSensorInDequeToAssign=0;

	if( NumSensors &&  m_pWizardLogic->m_dequeNumSensors.size()>0 )
		m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(DESIGNOBJ_AlarmSetupInclude_CONST),
										"", "", false, false );
	else
	{
		m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, DatabaseUtils::GetDescriptionForDevice(m_pWizardLogic,m_pWizardLogic->m_nPK_Device_AlarmPanel));
		string sText;
		if( NumSensors )
			sText=m_pOrbiter->m_mapTextString[TEXT_security_panel_already_setup_CONST] + "|" + m_pOrbiter->m_mapTextString[TEXT_Ok_CONST];
		else
			sText=m_pOrbiter->m_mapTextString[TEXT_Security_interface_with_no_sensors_CONST] + "|" + m_pOrbiter->m_mapTextString[TEXT_Ok_CONST];
		string sMessage="0 -300 1 " TOSTRING(COMMAND_Goto_Screen_CONST) " " TOSTRING(COMMANDPARAMETER_PK_Screen_CONST) " " TOSTRING(SCREEN_VOIP_Provider_CONST);

		m_pOrbiter->ForceCurrentScreenIntoHistory();
		DCE::SCREEN_House_Setup_Popup_Message SCREEN_House_Setup_Popup_Message(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device,
			sText,sMessage,"1");
		m_pOrbiter->ReceivedMessage(SCREEN_House_Setup_Popup_Message.m_pMessage);
	}
}
//-----------------------------------------------------------------------------------------------------
void OSDScreenHandler::SCREEN_AlarmPanel(long PK_Screen)
{
	PrepForWizard();
	m_tWaitingForRegistration=m_tRegistered=0;
	m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_DesignObj_CurrentSecti_CONST, TOSTRING(DESIGNOBJ_butAlarmPanelWizard_CONST)); 
	ScreenHandlerBase::SCREEN_AlarmPanel(PK_Screen);

	m_pWizardLogic->m_nPK_Device_AlarmPanel =	m_pWizardLogic->FindFirstDeviceInCategory(DEVICECATEGORY_Security_Interface_CONST,0);
	if( m_pWizardLogic->m_nPK_Device_AlarmPanel )
		HandleAlarmScreen();
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"OSDScreenHandler::SCREEN_AlarmPanel setting pnp devices");
		m_pWizardLogic->FindPnpDevices(TOSTRING(DEVICECATEGORY_Security_Interface_CONST));
		m_pOrbiter->StartScreenHandlerTimer(500);
	}

	m_nSensorInDequeToAssign=0;

	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &OSDScreenHandler::AlarmSetup_ObjectSelected, new ObjectInfoBackData());
	RegisterCallBack(cbDataGridSelected, (ScreenHandlerCallBack) &OSDScreenHandler::AlarmSetup_SelectedGrid, new DatagridCellBackData());
	RegisterCallBack(cbOnRenderScreen, (ScreenHandlerCallBack) &OSDScreenHandler::AlarmSetup_OnScreen, new RenderScreenCallBackData());
	RegisterCallBack(cbOnGotoScreen, (ScreenHandlerCallBack) &OSDScreenHandler::AlarmSetup_OnGotoScreen, new GotoScreenCallBackData());
	RegisterCallBack(cbOnTimer,	(ScreenHandlerCallBack) &OSDScreenHandler::Alarm_OnTimer, new CallBackData());
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::AlarmSetup_OnGotoScreen(CallBackData *pData)
{
	//GotoScreenCallBackData *pGotoScreenCallBackData = (GotoScreenCallBackData *)pData;
	return false;
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::AlarmSetup_OnScreen(CallBackData *pData)
{
	RenderScreenCallBackData *pRenderScreenCallBackData = (RenderScreenCallBackData *)pData;
	if( pRenderScreenCallBackData->m_pObj->m_iBaseObjectID==DESIGNOBJ_AlarmSetupRooms_CONST )
	{
		if( m_nSensorInDequeToAssign < (int) m_pWizardLogic->m_dequeNumSensors.size() )
		{
			m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_5_CONST, DatabaseUtils::GetDescriptionForDevice(m_pWizardLogic,m_pWizardLogic->m_dequeNumSensors[m_nSensorInDequeToAssign].first));
		}
		else
		{
			m_pOrbiter->CMD_Goto_Screen("",SCREEN_VOIP_Provider_CONST);
			m_pOrbiter->CMD_Refresh("");
		}
	}
	return false;
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::Alarm_OnTimer(CallBackData *pData)
{
	if( GetCurrentScreen_PK_DesignObj()==DESIGNOBJ_NoAlarmPanel_CONST || m_tWaitingForRegistration || m_tRegistered )
	{
		m_pWizardLogic->m_nPK_Device_AlarmPanel =	m_pWizardLogic->FindFirstDeviceInCategory(DEVICECATEGORY_Security_Interface_CONST,0);
LoggerWrapper::GetInstance()->Write(LV_STATUS,"SDScreenHandler::Alarm_OnTimer sensor %d waiting %d",m_pWizardLogic->m_nPK_Device_AlarmPanel,(int) m_tWaitingForRegistration );
		if( m_pWizardLogic->m_nPK_Device_AlarmPanel )
		{
			char cRegistered = m_tRegistered ? 'Y' : m_pOrbiter->DeviceIsRegistered(m_pWizardLogic->m_nPK_Device_AlarmPanel);
LoggerWrapper::GetInstance()->Write(LV_STATUS,"SDScreenHandler::Alarm_OnTimer registered %c", cRegistered);
			if( cRegistered=='Y' )
			{
				// Wait another 10 seconds after the alarm device first registers so it has time to report all it's children
				if( !m_tRegistered )
				{
LoggerWrapper::GetInstance()->Write(LV_STATUS,"SDScreenHandler::Alarm_OnTimer now registered, waiting 5 secs");
					m_tRegistered=time(NULL);
					m_tWaitingForRegistration=0;
					return true;
				}
				else if( time(NULL)-m_tRegistered<10 )
				{
LoggerWrapper::GetInstance()->Write(LV_STATUS,"SDScreenHandler::Alarm_OnTimer more time to startup");
					return true;
				}
LoggerWrapper::GetInstance()->Write(LV_STATUS,"SDScreenHandler::Alarm_OnTimer now registered");
				HandleAlarmScreen();
				return false;  // Kill the timer
			}
			else if( !m_tWaitingForRegistration )
			{
				m_tWaitingForRegistration = time(NULL);
				string sText=m_pOrbiter->m_mapTextString[TEXT_Waiting_for_device_to_startup_CONST] + "|NO_BUTTON";
				DCE::SCREEN_House_Setup_Popup_Message SCREEN_House_Setup_Popup_Message(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device,
					sText,"","1");
				m_pOrbiter->ReceivedMessage(SCREEN_House_Setup_Popup_Message.m_pMessage);

LoggerWrapper::GetInstance()->Write(LV_STATUS,"SDScreenHandler::Alarm_OnTimer displaying waiting message %d",(int) m_tWaitingForRegistration);
			}
			else if( time(NULL)-m_tWaitingForRegistration>60 )
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"OSDScreenHandler::Alarm_OnTimer timed out waiting for %d",m_pWizardLogic->m_nPK_Device_AlarmPanel);
				m_tWaitingForRegistration=0;

				string sText=m_pOrbiter->m_mapTextString[TEXT_Device_did_not_start_CONST] + "|" + m_pOrbiter->m_mapTextString[TEXT_Ok_CONST];
				string sMessage="0 -300 1 " TOSTRING(COMMAND_Goto_Screen_CONST) " " TOSTRING(COMMANDPARAMETER_PK_Screen_CONST) " " TOSTRING(SCREEN_VOIP_Provider_CONST);

				DCE::SCREEN_House_Setup_Popup_Message SCREEN_House_Setup_Popup_Message(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device,
					sText,sMessage,"1");
				m_pOrbiter->ReceivedMessage(SCREEN_House_Setup_Popup_Message.m_pMessage);

LoggerWrapper::GetInstance()->Write(LV_STATUS,"SDScreenHandler::Alarm_OnTimer registration failed");
				return false;
			}
		}
	}
LoggerWrapper::GetInstance()->Write(LV_STATUS,"SDScreenHandler::Alarm_OnTimer keeping timer");
	return true;
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::AlarmSetup_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;

	switch(GetCurrentScreen_PK_DesignObj())
	{
		case DESIGNOBJ_AlarmSetupRooms_CONST:
		{
			if(DESIGNOBJ_butAlarmType2_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				int PK_Room = atoi(m_pOrbiter->m_mapVariable_Find(VARIABLE_Datagrid_Input_CONST).c_str());
				if( !PK_Room || !DatabaseUtils::SetDeviceInRoom(m_pWizardLogic,m_pWizardLogic->m_dequeNumSensors[m_nSensorInDequeToAssign].first,PK_Room) )
					return true;  // This room isn't valid
				int PK_DeviceTemplate = DatabaseUtils::GetDeviceTemplateForDevice(m_pWizardLogic,m_pWizardLogic->m_dequeNumSensors[m_nSensorInDequeToAssign].first);
				if( PK_DeviceTemplate!=DEVICETEMPLATE_Generic_Sensor_CONST )
				{
					m_pOrbiter->CMD_Goto_DesignObj(0,TOSTRING(DESIGNOBJ_AlarmSensorName_CONST),"","",true,true);
					return true;
				}
			}
		}
		break;
		case DESIGNOBJ_AlarmSensorType_CONST:
		{
			if(DESIGNOBJ_butAlarmName2_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				int PK_DeviceTemplate = atoi(m_pOrbiter->m_mapVariable_Find(VARIABLE_Datagrid_Input_CONST).c_str());
				if( PK_DeviceTemplate )
				{
					DatabaseUtils::SetDeviceTemplateForDevice(m_pWizardLogic,m_pWizardLogic->m_dequeNumSensors[m_nSensorInDequeToAssign].first,
												PK_DeviceTemplate);
					string sDefaultSensorName = DatabaseUtils::GetDescriptionFromTable(m_pWizardLogic,DEVICETEMPLATE_TABLE,PK_DeviceTemplate);
					m_pOrbiter->CMD_Set_Variable(VARIABLE_Seek_Value_CONST, sDefaultSensorName);
				}
			}
		}
		break;
		case DESIGNOBJ_AlarmSensorName_CONST:
		{
			if(DESIGNOBJ_butAlarmSetupRoom_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				DatabaseUtils::SetDescriptionForDevice(m_pWizardLogic,m_pWizardLogic->m_dequeNumSensors[m_nSensorInDequeToAssign].first,m_pOrbiter->m_mapVariable_Find(VARIABLE_Seek_Value_CONST));
				m_nSensorInDequeToAssign++;
			}
		}
		break;
	};

	return false;
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::AlarmSetup_SelectedGrid(CallBackData *pData)
{
	DatagridCellBackData *pDatagridCellBackData = (DatagridCellBackData *) pData;
	if( pDatagridCellBackData->m_nPK_Datagrid == DATAGRID_Rooms_CONST )
	{
		int PK_Room = atoi(pDatagridCellBackData->m_sValue.c_str());
		m_pOrbiter->CMD_Show_Object( TOSTRING(DESIGNOBJ_butAlarmType2_CONST), 0,"","", PK_Room ? "1" : "0" );
	}
	else if( pDatagridCellBackData->m_nPK_Datagrid == DATAGRID_Sensor_Type_CONST )
	{
		int PK_FloorplanObjecType = atoi(pDatagridCellBackData->m_sValue.c_str());
		m_pOrbiter->CMD_Show_Object( TOSTRING(DESIGNOBJ_butAlarmName2_CONST), 0,"","", PK_FloorplanObjecType ? "1" : "0" );
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
// VOIP Provider
//-----------------------------------------------------------------------------------------------------
void OSDScreenHandler::SCREEN_VOIP_Provider(long PK_Screen)
{
	PrepForWizard();
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Seek_Value_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_4_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Datagrid_Input_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_DesignObj_CurrentSecti_CONST, TOSTRING(DESIGNOBJ_butVoipAccountWizard_CONST)); // todo

	ScreenHandlerBase::SCREEN_VOIP_Provider(PK_Screen);

	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &OSDScreenHandler::VOIP_Provider_ObjectSelected, new ObjectInfoBackData());
	RegisterCallBack(cbCapturedKeyboardBufferChanged, (ScreenHandlerCallBack) &OSDScreenHandler::VOIP_CapturedKeyboardBufferChanged, new ObjectInfoBackData());
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::VOIP_CapturedKeyboardBufferChanged(CallBackData *pData)
{
	switch(GetCurrentScreen_PK_DesignObj())
	{
		case DESIGNOBJ_VoipNumber_CONST:
		{
			string sPhoneNumber = m_pOrbiter->m_mapVariable_Find(VARIABLE_Seek_Value_CONST);
			m_pOrbiter->CMD_Show_Object(TOSTRING(DESIGNOBJ_butVoipUsername_CONST),0,"","",sPhoneNumber.size()==0 ? "0" : "1");
		}
		break;

		case DESIGNOBJ_VoipUsername_CONST:
		{
			string sUsername = m_pOrbiter->m_mapVariable_Find(VARIABLE_Seek_Value_CONST);
			m_pOrbiter->CMD_Show_Object(TOSTRING(DESIGNOBJ_butVoipPassword_CONST),0,"","",sUsername.size()==0 ? "0" : "1");
		}
		break;

		case DESIGNOBJ_VoipPassword_CONST:
		{
			string sPassword = m_pOrbiter->m_mapVariable_Find(VARIABLE_Seek_Value_CONST);
			m_pOrbiter->CMD_Show_Object(TOSTRING(DESIGNOBJ_butFloorplan_vp_CONST),0,"","",sPassword.size()==0 ? "0" : "1");
		}
		break;
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::VOIP_Provider_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;

	switch(GetCurrentScreen_PK_DesignObj())
	{

		case DESIGNOBJ_VoipProvider_CONST:
		{
			if(DESIGNOBJ_butConfigureAccount_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				string sVOIPProvider = m_pOrbiter->m_mapVariable_Find(VARIABLE_Misc_Data_4_CONST);
				if(sVOIPProvider == "")
					return true;
			}
		}
		break;

		case DESIGNOBJ_VoipNumber_CONST:
		{
			if(DESIGNOBJ_butVoipUsername_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				string sPhoneNumber = m_pOrbiter->m_mapVariable_Find(VARIABLE_Seek_Value_CONST);
				m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, sPhoneNumber);
				if(sPhoneNumber == "")
					return true;
			}
			else if( atoi(pObjectInfoData->m_pObj->m_ObjectID.c_str())==DESIGNOBJ_VoipNumber_CONST ) // Don't do this yet if it's still doing selected objects from teh prior screen
			{
				string sPhoneNumber = m_pOrbiter->m_mapVariable_Find(VARIABLE_Seek_Value_CONST);
				m_pOrbiter->CMD_Show_Object(TOSTRING(DESIGNOBJ_butVoipUsername_CONST),0,"","",sPhoneNumber.size()==0 ? "0" : "1");
			}
		}
		break;

		case DESIGNOBJ_VoipUsername_CONST:
		{
			if(DESIGNOBJ_butVoipPassword_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				string sUsername = m_pOrbiter->m_mapVariable_Find(VARIABLE_Seek_Value_CONST);
				m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST, sUsername);
				if(sUsername == "")
					return true;
			}
			else if( atoi(pObjectInfoData->m_pObj->m_ObjectID.c_str())==DESIGNOBJ_VoipUsername_CONST ) // Don't do this yet if it's still doing selected objects from teh prior screen
			{
				string sUsername = m_pOrbiter->m_mapVariable_Find(VARIABLE_Seek_Value_CONST);
				m_pOrbiter->CMD_Show_Object(TOSTRING(DESIGNOBJ_butVoipPassword_CONST),0,"","",sUsername.size()==0 ? "0" : "1");
			}
		}
		break;

		case DESIGNOBJ_VoipPassword_CONST:
		{
			if(DESIGNOBJ_butFloorplan_vp_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				string sPassword = m_pOrbiter->m_mapVariable_Find(VARIABLE_Seek_Value_CONST);
				if(sPassword == "")
					return true;

				string sPhoneNumber = m_pOrbiter->m_mapVariable_Find(VARIABLE_Misc_Data_1_CONST);
				string sUsername = m_pOrbiter->m_mapVariable_Find(VARIABLE_Misc_Data_2_CONST);
				string sVOIPProvider = m_pOrbiter->m_mapVariable_Find(VARIABLE_Misc_Data_4_CONST);

				DCE::CMD_PL_Add_VOIP_Account CMD_PL_Add_VOIP_Account_(m_pOrbiter->m_dwPK_Device,
                                                                      m_pOrbiter->m_dwPK_Device_TelecomPlugIn, sVOIPProvider, sPhoneNumber, sPassword, sUsername);
				m_pOrbiter->SendCommand(CMD_PL_Add_VOIP_Account_);
			}
			else if( atoi(pObjectInfoData->m_pObj->m_ObjectID.c_str())==DESIGNOBJ_VoipPassword_CONST ) // Don't do this yet if it's still doing selected objects from teh prior screen
			{
				string sPassword = m_pOrbiter->m_mapVariable_Find(VARIABLE_Seek_Value_CONST);
				m_pOrbiter->CMD_Show_Object(TOSTRING(DESIGNOBJ_butMediaWizard_CONST),0,"","",sPassword.size()==0 ? "0" : "1");
			}
		}
		break;
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OSDScreenHandler::SCREEN_PVR_Remote(long PK_Screen)
{
	m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_DesignObj_CurrentSecti_CONST, TOSTRING(DESIGNOBJ_butPVRSoftware_CONST));
	ScreenHandlerBase::SCREEN_PVR_Remote(PK_Screen);
	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &OSDScreenHandler::PVRSoftware_ObjectSelected, new ObjectInfoBackData());
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OSDScreenHandler::SCREEN_PVR_Software(long PK_Screen)
{
	PrepForWizard();
	m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_DesignObj_CurrentSecti_CONST, TOSTRING(DESIGNOBJ_butPVRSoftware_CONST));
	ScreenHandlerBase::SCREEN_PVR_Software(PK_Screen);
	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &OSDScreenHandler::PVRSoftware_ObjectSelected, new ObjectInfoBackData());
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::PVRSoftware_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;
	if(pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butMyth_CONST )
		m_pWizardLogic->SetPVRSoftware('M');
	else if(pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butVDR_CONST)
		m_pWizardLogic->SetPVRSoftware('V');

	return false;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OSDScreenHandler::SCREEN_Final_House_Setup(long PK_Screen)
{
	PrepForWizard();
	if( m_pWizardLogic->HouseAlreadySetup()==false )
	{
		string sMessage="0 -300 1 " TOSTRING(COMMAND_Goto_Screen_CONST) " " TOSTRING(COMMANDPARAMETER_PK_Screen_CONST) " " TOSTRING(SCREEN_UsersWizard_CONST);

		DCE::SCREEN_House_Setup_Popup_Message SCREEN_House_Setup_Popup_Message(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device,
			m_pOrbiter->m_mapTextString[TEXT_Must_add_users_rooms_CONST] + "|" + m_pOrbiter->m_mapTextString[TEXT_Ok_CONST],sMessage,"1");
		m_pOrbiter->ReceivedMessage(SCREEN_House_Setup_Popup_Message.m_pMessage);
		return;
	}

	// if vdr or myth are not installed
	if(!m_pWizardLogic->AnyPVRSoftwareInstalled())
	{
		// install mythtv by default
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "No PVR Software is installed. We'll install mythtv by default");
		m_pWizardLogic->SetPVRSoftware('M');
	}

	m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_DesignObj_CurrentSecti_CONST, TOSTRING(DESIGNOBJ_butDoneHouseSetup_CONST)); 
	ScreenHandlerBase::SCREEN_Final_House_Setup(PK_Screen);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OSDScreenHandler::SCREEN_mnuSpeedControl(long PK_Screen)
{
	if(!m_pOrbiter->m_mapVariable_Find(VARIABLE_My_Channel_ID_CONST).empty())
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "OSDScreenHandler : won't go to mnuSpeedControl, we have a call in progress");
		return;
	}

	LoggerWrapper::GetInstance()->Write( LV_STATUS, "OSDScreenHandler::SCREEN_mnuSpeedControl()" );
	ScreenHandler::SCREEN_mnuSpeedControl(PK_Screen);
	
	RegisterCallBack( cbOnCustomRender, (ScreenHandlerCallBack)&OSDScreenHandler::SpeedControlCustomRender, new RenderScreenCallBackData() );
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::SpeedControlCustomRender(CallBackData *pData)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "OSDScreenHandler::SpeedControlCustomRender() screen %d",m_pOrbiter->m_pScreenHistory_Current->PK_Screen());
	if( m_pOrbiter->m_pScreenHistory_Current->PK_Screen()!=SCREEN_mnuSpeedControl_CONST )
		return false;

	//we don't need this right now
	RenderScreenCallBackData *pRenderData = dynamic_cast<RenderScreenCallBackData *>(pData);
	if(NULL == pRenderData || NULL == pRenderData->m_pObj)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "OSDScreenHandler::SpeedControlCustomRender() : "
			"pData is not a RenderScreenCallBackData or the designobj within is NULL: renderdata %p", pRenderData);
		return false;
	}

	DesignObj_Orbiter *pObj = pRenderData->m_pObj;
	PlutoRectangle rectTotal = pObj->m_rPosition;
	PlutoPoint point;

	PlutoGraphic *pBackgroundGraphic = NULL;

#ifdef ENABLE_MOUSE_BEHAVIOR
	//get the background image
	static PlutoGraphic *pLocalBackgroundGraphic = NULL;
	if(NULL == pLocalBackgroundGraphic)
	{
		pLocalBackgroundGraphic = m_pOrbiter->Renderer()->GetBackground(rectTotal);
	}

	pBackgroundGraphic = pLocalBackgroundGraphic;
#endif

	if(NULL != pBackgroundGraphic)
		m_pOrbiter->Renderer()->RenderGraphic( pBackgroundGraphic,  rectTotal, pObj->m_bDisableAspectLock, point );

	pObj->RenderGraphic(rectTotal, pObj->m_bDisableAspectLock, point);

#ifdef ENABLE_MOUSE_BEHAVIOR

	if(NULL != m_pOrbiter && NULL != m_pOrbiter->m_pMouseBehavior)
	{
		const MouseHandler *pMouseHandler = m_pOrbiter->m_pMouseBehavior->GetMouseHandler();
		const SpeedMouseHandler *pcSpeedMouseHandler = dynamic_cast<const SpeedMouseHandler *>(pMouseHandler);
		if(NULL == pcSpeedMouseHandler)
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "OSDScreenHandler::SpeedControlCustomRender() : "
				"Unable to get SpeedMouseHandler");
			return false;
		}

		SpeedMouseHandler *pSpeedMouseHandler = const_cast<SpeedMouseHandler *>(pcSpeedMouseHandler);
		pSpeedMouseHandler->DrawInfo();
	}
	else
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "OSDScreenHandler::SpeedControlCustomRender() : "
			"MouseBehavior not activated in Orbiter!");
#endif

	VectorDesignObjText::iterator iText;
	for( iText=pObj->m_vectDesignObjText.begin(  ); iText != pObj->m_vectDesignObjText.end(  ); ++iText )
	{
		DesignObjText *pText = *iText;
		if(  pText->m_bPreRender  )
			continue;
		PROFILE_START( ctText );
		TextStyle *pTextStyle = pText->m_mapTextStyle_Find( 0 );
		string TextToDisplay = m_pOrbiter->SubstituteVariables(m_pOrbiter->SubstituteVariables(pText->m_sText, pText->m_pObject, 0, 0), pText->m_pObject, 0, 0);
		m_pOrbiter->Renderer()->RenderText( TextToDisplay, pText, pTextStyle, point, "speedtext_" + StringUtils::ltos(pText->m_PK_Text));
		PROFILE_STOP( ctText,  "Text ( obj below )" );
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------=======

bool OSDScreenHandler::AV_Devices_CapturedKeyboardBufferChanged(CallBackData *pData)
{
	DatagridCellBackData *pCellInfoData = NULL; 
	string sEditVal, sSelectedId;

	//PLUTO_SAFETY_LOCK(vm, m_pOrbiter->m_VariableMutex);
	sEditVal = m_pOrbiter->m_mapVariable_Find(VARIABLE_Seek_Value_CONST);
	//int nSelectedId = 0;
	pCellInfoData = dynamic_cast<DatagridCellBackData *>(pData);

	switch( GetCurrentScreen_PK_DesignObj() )
	{
		case DESIGNOBJ_TVManufNotListed_CONST:
			m_pOrbiter->CMD_Show_Object(TOSTRING(DESIGNOBJ_butTVModelNotListed_CONST),0,"","", sEditVal.size() ? "1" : "0");//4849
			break;

		case DESIGNOBJ_TVModelNotListed_CONST:
			m_pOrbiter->CMD_Show_Object(TOSTRING(DESIGNOBJ_butTVWhatDelays_CONST),0,"","", sEditVal.size() ? "1" : "0");//4926
			break;
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OSDScreenHandler::SCREEN_mnuVolume(long PK_Screen)
{
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "ScreenHandler::SCREEN_mnuVolume" );
	ScreenHandlerBase::SCREEN_mnuVolume(PK_Screen);
	
	RegisterCallBack( cbOnCustomRender, (ScreenHandlerCallBack)&OSDScreenHandler::VolumeControlCustomRender, new RenderScreenCallBackData() );
}

bool OSDScreenHandler::VolumeControlCustomRender(CallBackData *pData)
{
	RenderScreenCallBackData *pRenderScreenCallBackData = (RenderScreenCallBackData *)pData;
	DesignObj_Orbiter *pObj = pRenderScreenCallBackData->m_pObj;
	PlutoRectangle rectTotal = pObj->m_rPosition;
	PlutoPoint point;

	PlutoGraphic *pBackgroundGraphic = NULL;

#ifdef ENABLE_MOUSE_BEHAVIOR
	if( m_pOrbiter->m_pMouseBehavior )
	{
		//hook the object to have the background image merged with the desktop image
		//will do this only once
		static PlutoGraphic *pBackgroundBackgroundX = NULL;
		//static PlutoGraphic *pBackgroundBackgroundY = NULL;

		if( m_pOrbiter->m_pMouseBehavior->m_pMouseHandler )
		{
			if(NULL == pBackgroundBackgroundX)
				pBackgroundBackgroundX = m_pOrbiter->Renderer()->GetBackground(rectTotal);

			pBackgroundGraphic = pBackgroundBackgroundX;
		}
		/*
		else if( m_pOrbiter->m_pMouseBehavior->m_cLocked_Axis_Current==AXIS_LOCK_Y && m_pOrbiter->m_pMouseBehavior->m_pMouseHandler_Vertical )
		{
			if(NULL == pBackgroundBackgroundY)
				pBackgroundBackgroundY = m_pOrbiter->GetBackground(rectTotal);

			pBackgroundGraphic = pBackgroundBackgroundY;
		} 
		*/
	} 
#endif

	if(NULL != pBackgroundGraphic)
		m_pOrbiter->Renderer()->RenderGraphic( pBackgroundGraphic,  rectTotal, pObj->m_bDisableAspectLock, point );

	pObj->RenderGraphic(rectTotal, pObj->m_bDisableAspectLock, point);

#ifdef ENABLE_MOUSE_BEHAVIOR
	if( m_pOrbiter->m_pMouseBehavior )
	{
		if( m_pOrbiter->m_pMouseBehavior->m_pMouseHandler )
		{
			VolumeMouseHandler *pVolumeMouseHandler = dynamic_cast<VolumeMouseHandler *> (m_pOrbiter->m_pMouseBehavior->m_pMouseHandler);
			pVolumeMouseHandler->CustomRender();
		}
		/*
		else if( m_pOrbiter->m_pMouseBehavior->m_cLocked_Axis_Current==AXIS_LOCK_Y && m_pOrbiter->m_pMouseBehavior->m_pMouseHandler_Vertical )
		{
			LightMouseHandler *pLightMouseHandler = dynamic_cast<LightMouseHandler *> (m_pOrbiter->m_pMouseBehavior->m_pMouseHandler_Vertical);
			pLightMouseHandler->CustomRender();
		}
		*/
	}
#endif

	return false;
}




//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OSDScreenHandler::SCREEN_mnuLights(long PK_Screen)
{
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "ScreenHandler::SCREEN_mnuLights" );
	ScreenHandlerBase::SCREEN_mnuLights(PK_Screen);
	
	RegisterCallBack( cbOnCustomRender, (ScreenHandlerCallBack)&OSDScreenHandler::LightControlCustomRender, new RenderScreenCallBackData() );
}

bool OSDScreenHandler::LightControlCustomRender(CallBackData *pData)
{
	RenderScreenCallBackData *pRenderScreenCallBackData = (RenderScreenCallBackData *)pData;
	DesignObj_Orbiter *pObj = pRenderScreenCallBackData->m_pObj;
	PlutoRectangle rectTotal = pObj->m_rPosition;
	PlutoPoint point;

	PlutoGraphic *pBackgroundGraphic = NULL;

#ifdef ENABLE_MOUSE_BEHAVIOR
	if( m_pOrbiter->m_pMouseBehavior )
	{
		//hook the object to have the background image merged with the desktop image
		//will do this only once
		static PlutoGraphic *pBackgroundBackgroundX = NULL;
		//static PlutoGraphic *pBackgroundBackgroundY = NULL;

		if( m_pOrbiter->m_pMouseBehavior->m_pMouseHandler )
		{
			if(NULL == pBackgroundBackgroundX)
				pBackgroundBackgroundX = m_pOrbiter->Renderer()->GetBackground(rectTotal);

			pBackgroundGraphic = pBackgroundBackgroundX;
		}
		/*
		else if( m_pOrbiter->m_pMouseBehavior->m_cLocked_Axis_Current==AXIS_LOCK_Y && m_pOrbiter->m_pMouseBehavior->m_pMouseHandler_Vertical )
		{
			if(NULL == pBackgroundBackgroundY)
				pBackgroundBackgroundY = m_pOrbiter->GetBackground(rectTotal);

			pBackgroundGraphic = pBackgroundBackgroundY;
		} 
		*/
	} 
#endif

	if(NULL != pBackgroundGraphic)
		m_pOrbiter->Renderer()->RenderGraphic( pBackgroundGraphic,  rectTotal, pObj->m_bDisableAspectLock, point );

	pObj->RenderGraphic(rectTotal, pObj->m_bDisableAspectLock, point);

#ifdef ENABLE_MOUSE_BEHAVIOR
	if( m_pOrbiter->m_pMouseBehavior )
	{
		if( m_pOrbiter->m_pMouseBehavior->m_pMouseHandler )
		{
			LightMouseHandler *pLightMouseHandler = dynamic_cast<LightMouseHandler *> (m_pOrbiter->m_pMouseBehavior->m_pMouseHandler);
			pLightMouseHandler->CustomRender();
		}
		/*
		else if( m_pOrbiter->m_pMouseBehavior->m_cLocked_Axis_Current==AXIS_LOCK_Y && m_pOrbiter->m_pMouseBehavior->m_pMouseHandler_Vertical )
		{
			LightMouseHandler *pLightMouseHandler = dynamic_cast<LightMouseHandler *> (m_pOrbiter->m_pMouseBehavior->m_pMouseHandler_Vertical);
			pLightMouseHandler->CustomRender();
		}
		*/
	}
#endif
	return false;
}

void OSDScreenHandler::SCREEN_mnuPlaybackControl(long PK_Screen)
{
	ScreenHandlerBase::SCREEN_mnuPlaybackControl(PK_Screen);
	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &OSDScreenHandler::PlayBackControlSelected, new ObjectInfoBackData());
}

bool OSDScreenHandler::PlayBackControlSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;

	if( pObjectInfoData->m_PK_DesignObj_SelectedObject==DESIGNOBJ_hiddenShowSpeedControl_CONST )
		m_pOrbiter->CMD_Goto_Screen("", m_pOrbiter->m_iPK_Screen_OSD_Speed);
	else if( pObjectInfoData->m_PK_DesignObj_SelectedObject==DESIGNOBJ_hiddenShowMediaTracks_CONST )
		m_pOrbiter->CMD_Goto_Screen("", m_pOrbiter->m_iPK_Screen_OSD_Track);
	return false;
}

void OSDScreenHandler::SCREEN_PopupMessage(long PK_Screen, string sText, string sCommand_Line, string sDescription, string sPromptToResetRouter, string sTimeout, string sCannotGoBack)
{
	if( m_tWizardIsRunning && sDescription=="new_device_reload" )
		return;  // Don't display these messages during the wizard

	ScreenHandler::SCREEN_PopupMessage(PK_Screen, sText, sCommand_Line, sDescription, sPromptToResetRouter, sTimeout, sCannotGoBack);
}

void OSDScreenHandler::ReceivedGotoScreenMessage(int nPK_Screen, Message *pMessage)
{
	if( m_tWizardIsRunning )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"OSDScreenHandler::ReceivedGotoScreenMessage checking screen %d",nPK_Screen);
		if( nPK_Screen == SCREEN_PopupMessage_CONST && pMessage && pMessage->m_mapParameters[COMMANDPARAMETER_Description_CONST]=="new_device_reload" )
			return;
		else if( nPK_Screen == SCREEN_Need_Regen_Orbiter_CONST || nPK_Screen == SCREEN_Need_Reload_Router_CONST )
			return;
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"OSDScreenHandler::ReceivedGotoScreenMessage letting through screen %d",nPK_Screen);
	}

	ScreenHandler::ReceivedGotoScreenMessage(nPK_Screen,pMessage);
}

//-----------------------------------------------------------------------------------------------------
void OSDScreenHandler::SCREEN_Get_Capture_Card_Port(long PK_Screen)
{
	PrepForWizard();
	m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_DesignObj_CurrentSecti_CONST, TOSTRING(DESIGNOBJ_butInputs_CONST));
	m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_Device_1_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Datagrid_Input_CONST, "");
	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &OSDScreenHandler::CaptureCardPort_ObjectSelected,	new ObjectInfoBackData());
	RegisterCallBack(cbDataGridSelected, (ScreenHandlerCallBack) &OSDScreenHandler::CaptureCardPort_DatagridSelected, new DatagridCellBackData());
	RegisterCallBack(cbDataGridRendering, (ScreenHandlerCallBack) &OSDScreenHandler::CaptureCardPort_GridRendering,	new DatagridAcquiredBackData());
	ScreenHandlerBase::SCREEN_Get_Capture_Card_Port(PK_Screen);
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::CaptureCardPort_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;

	PLUTO_SAFETY_LOCK(vm, m_pOrbiter->m_ScreenMutex);
	if( pObjectInfoData->m_PK_DesignObj_SelectedObject==DESIGNOBJ_butNewCamera_CONST )
	{
		int PK_Device=0;
		int PK_Room = m_pWizardLogic->WhatRoomIsThisDeviceIn(m_pWizardLogic->GetTopMostDevice(m_pOrbiter->m_dwPK_Device));
		DCE::CMD_Create_Device CMD_Create_Device(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_GeneralInfoPlugIn,
			DEVICETEMPLATE_Generic_Analog_Camera_CONST,"",PK_Room,"",TOSTRING(DEVICEDATA_FK_Device_Capture_Card_Port_CONST) "|" +
			m_pOrbiter->m_mapVariable_Find(VARIABLE_Datagrid_Input_CONST),
			0,atoi(m_pOrbiter->m_mapVariable_Find(VARIABLE_Datagrid_Input_CONST).c_str()),"Camera",m_pOrbiter->m_dwPK_Device,0,&PK_Device);
		m_pOrbiter->SendCommand(CMD_Create_Device);
		m_pOrbiter->CMD_Go_back("","");
	}
	else if( pObjectInfoData->m_PK_DesignObj_SelectedObject==DESIGNOBJ_butNoDeviceOnPort_CONST )
	{
		DCE::CMD_Specify_Capture_Card_Port CMD_Specify_Capture_Card_Port(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_MediaPlugIn,
			0,atoi(m_pOrbiter->m_mapVariable_Find(VARIABLE_Datagrid_Input_CONST).c_str()));
		string sResponse;  // Send with return confirmation so the grid doesn't refresh until this command is done
		m_pOrbiter->SendCommand(CMD_Specify_Capture_Card_Port,&sResponse);
		m_pOrbiter->CMD_Go_back("","");
	}
	else if( pObjectInfoData->m_PK_DesignObj_SelectedObject==DESIGNOBJ_butJustTheRoomItsIn_CONST )
		m_pWizardLogic->SetExternalDeviceInRoom(atoi(m_pOrbiter->m_mapVariable_Find(VARIABLE_PK_Device_1_CONST).c_str()),"0");
	else if( pObjectInfoData->m_PK_DesignObj_SelectedObject==DESIGNOBJ_butEveryRoomWithMD_CONST )
		m_pWizardLogic->SetExternalDeviceInRoom(atoi(m_pOrbiter->m_mapVariable_Find(VARIABLE_PK_Device_1_CONST).c_str()),"*");
	else if( pObjectInfoData->m_PK_DesignObj_SelectedObject==DESIGNOBJ_butNext_RoomsForExternalDevice_CONST )
		m_pWizardLogic->SetExternalDeviceInRoom(atoi(m_pOrbiter->m_mapVariable_Find(VARIABLE_PK_Device_1_CONST).c_str()),
			m_pOrbiter->m_mapVariable_Find(VARIABLE_Misc_Data_1_CONST));


	return false;
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::CaptureCardPort_GridRendering(CallBackData *pData)
{
	// This is called every time a new section of the grid is to be rendered.  We want to find the child object for the check and hide/show it
	DatagridAcquiredBackData *pDatagridAcquiredBackData = (DatagridAcquiredBackData *) pData;  // Call back data containing relevant values for the grid/table being rendered

	if( pDatagridAcquiredBackData->m_pObj->m_iPK_Datagrid==DATAGRID_Capture_Card_Ports_CONST )
	{
		// Iterate through all the cells
		for(MemoryDataTable::iterator it=pDatagridAcquiredBackData->m_pDataGridTable->m_MemoryDataTable.begin();it!=pDatagridAcquiredBackData->m_pDataGridTable->m_MemoryDataTable.end();++it)
		{
			DataGridCell *pCell = it->second;
			pair<int,int> colRow = DataGridTable::CovertColRowType(it->first);  // Get the column/row for the cell
			colRow.first -= pDatagridAcquiredBackData->m_pObj->m_GridCurCol;
			colRow.second -= pDatagridAcquiredBackData->m_pObj->m_GridCurRow;

			// See if there is an object assigned for this column/row
			map< pair<int,int>, DesignObj_Orbiter *>::iterator itobj = pDatagridAcquiredBackData->m_pObj->m_mapChildDgObjects.find( colRow );
			if( itobj!=pDatagridAcquiredBackData->m_pObj->m_mapChildDgObjects.end() )
			{
				DesignObj_Orbiter *pObj = itobj->second;  // This is the cell's object.
				DesignObj_DataList::iterator iHao;

				// Iterate through all the object's children
				for( iHao=pObj->m_ChildObjects.begin(  ); iHao != pObj->m_ChildObjects.end(  ); ++iHao )
				{
					DesignObj_Orbiter *pDesignObj_Orbiter = (DesignObj_Orbiter *)( *iHao );
					if( pDesignObj_Orbiter->m_iBaseObjectID==DESIGNOBJ_iconCheckMark_CONST )
						pDesignObj_Orbiter->m_bHidden = pCell->m_mapAttributes_Find("InUse_PK_Device").empty()==true;
				}
			}
		}
	}
	return false;
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::CaptureCardPort_DatagridSelected(CallBackData *pData)
{
	DatagridCellBackData *pCellInfoData = (DatagridCellBackData *)pData;
	if( pCellInfoData->m_nPK_Datagrid==DATAGRID_Capture_Card_Audio_Ports_CONST )
	  {
	    DCE::CMD_Specify_Capture_Card_Audio_Port CMD_Specify_Capture_Card_Audio_Port(
						     m_pOrbiter->m_dwPK_Device,
						     m_pOrbiter->m_dwPK_Device_MediaPlugIn,
						     atoi(m_pOrbiter->m_mapVariable_Find(VARIABLE_PK_Device_1_CONST).c_str()),
						     atoi(pCellInfoData->m_sValue.c_str()));
	    string sResponse; // Send with response so the grid does not refresh.
	    m_pOrbiter->SendCommand(CMD_Specify_Capture_Card_Audio_Port,&sResponse);
	    
	    m_pOrbiter->CMD_Goto_DesignObj(0,TOSTRING(DESIGNOBJ_RoomsForExternalDevice_CONST),"","",false,true);
	    // Note: VARIABLE_PK_Device_1 is still set from the datagrid below, so we are okay.
	    return true; // We did a goto screen.
	  }
	if( pCellInfoData->m_nPK_Datagrid==DATAGRID_Capture_Card_Port_Devices_CONST )
	{
		DCE::CMD_Specify_Capture_Card_Port CMD_Specify_Capture_Card_Port(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_MediaPlugIn,
			atoi(pCellInfoData->m_sValue.c_str()),atoi(m_pOrbiter->m_mapVariable_Find(VARIABLE_Datagrid_Input_CONST).c_str()));
		string sResponse;  // Send with return confirmation so the grid doesn't refresh until this command is done
		m_pOrbiter->SendCommand(CMD_Specify_Capture_Card_Port,&sResponse);

		m_pOrbiter->CMD_Goto_DesignObj(0,TOSTRING(DESIGNOBJ_Specify_Audio_Inputs_CONST),"","",false,true);
		//m_pOrbiter->CMD_Goto_DesignObj(0,TOSTRING(DESIGNOBJ_RoomsForExternalDevice_CONST),"","",false,true);
		m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_Device_1_CONST, pCellInfoData->m_sValue);
		return true; // We did a goto screen
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
void OSDScreenHandler::SCREEN_Get_Capture_Card_Audio_Port(long PK_Screen)
{
  PrepForWizard();
  m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_DesignObj_CurrentSecti_CONST, TOSTRING(DESIGNOBJ_butInputs_CONST));
  m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_Device_1_CONST,"");
  m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST,"");
  m_pOrbiter->CMD_Set_Variable(VARIABLE_Datagrid_Input_CONST,"");
  RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &OSDScreenHandler::CaptureCardAudioPort_ObjectSelected, new ObjectInfoBackData());
  RegisterCallBack(cbDataGridSelected, (ScreenHandlerCallBack) &OSDScreenHandler::CaptureCardAudioPort_DatagridSelected, new DatagridCellBackData());
  RegisterCallBack(cbDataGridRendering, (ScreenHandlerCallBack) &OSDScreenHandler::CaptureCardAudioPort_GridRendering, new DatagridAcquiredBackData());
  ScreenHandlerBase::SCREEN_Get_Capture_Card_Audio_Port(PK_Screen);
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::CaptureCardAudioPort_ObjectSelected(CallBackData *pData)
{
  ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;

  PLUTO_SAFETY_LOCK(vm, m_pOrbiter->m_ScreenMutex);
  // FIXME: Come back here and implement what we need.
  return false;
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::CaptureCardAudioPort_GridRendering(CallBackData *pData)
{
  // This is actually called to deal with hiding/showing check to show already configured ports.
  DatagridAcquiredBackData *pDatagridAcquiredBackData = (DatagridAcquiredBackData *) pData;
  
  if ( pDatagridAcquiredBackData->m_pObj->m_iPK_Datagrid==DATAGRID_Capture_Card_Audio_Ports_CONST )
    {
      for (MemoryDataTable::iterator it=pDatagridAcquiredBackData->m_pDataGridTable->m_MemoryDataTable.begin();it!=pDatagridAcquiredBackData->m_pDataGridTable->m_MemoryDataTable.end();++it)
	{
	  DataGridCell *pCell = it->second;
	  pair<int, int> colRow = DataGridTable::CovertColRowType(it->first);
	  colRow.first  -= pDatagridAcquiredBackData->m_pObj->m_GridCurCol;
	  colRow.second -= pDatagridAcquiredBackData->m_pObj->m_GridCurRow;
	  
	  map< pair<int, int>, DesignObj_Orbiter *>::iterator itobj = pDatagridAcquiredBackData->m_pObj->m_mapChildDgObjects.find( colRow );
	  if ( itobj != pDatagridAcquiredBackData->m_pObj->m_mapChildDgObjects.end() )
	    {
	      DesignObj_Orbiter *pObj = itobj->second; // cell's object
	      DesignObj_DataList::iterator iHao;

	      for ( iHao=pObj->m_ChildObjects.begin();iHao!=pObj->m_ChildObjects.end();++iHao)
		{
		  DesignObj_Orbiter *pDesignObj_Orbiter = (DesignObj_Orbiter *)(*iHao);
		  if (pDesignObj_Orbiter->m_iBaseObjectID==DESIGNOBJ_iconCheckMark_CONST)
		    pDesignObj_Orbiter->m_bHidden = pCell->m_mapAttributes_Find("InUse_PK_Device").empty()==true;
		}
	    }
	}
    }
  return false;
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::CaptureCardAudioPort_DatagridSelected(CallBackData *pData)
{
  DatagridCellBackData *pCellInfoData = (DatagridCellBackData *)pData;
  if ( pCellInfoData->m_nPK_Datagrid==DATAGRID_Capture_Card_Audio_Ports_CONST )
    {
      DCE::CMD_Specify_Capture_Card_Port CMD_Specify_Capture_Card_Audio_Port(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_MediaPlugIn, atoi(pCellInfoData->m_sValue.c_str()),atoi(m_pOrbiter->m_mapVariable_Find(VARIABLE_Datagrid_Input_CONST).c_str()));
      string sResponse;
      m_pOrbiter->SendCommand(CMD_Specify_Capture_Card_Audio_Port,&sResponse);
      
      // FIXME: Come back here and add a Goto DesignObj to the next screen!
      // FIXME: Come back here and add a Set Variable, as needed for the next screen!
      return true; // we did a goto screen.
    }
  return false; // FIXME: Do we need this?
}
//-----------------------------------------------------------------------------------------------------
// Create some 'choose provider stages' to keep track of where we are
#define CPS_DATAGRID				1
#define CPS_CHOOSE_SOURCE			2
#define CPS_GETTING_USERNAME		3
#define CPS_GETTING_PASSWORD		4
#define CPS_GETTING_PROVIDER_LIST	5
#define CPS_PROMPTING_PROVIDER		6
#define CPS_GETTING_DEVICE_LIST		7
#define CPS_PROMPTING_DEVICE		8
#define CPS_GETTING_PACKAGE_LIST	9
#define CPS_PROMPTING_PACKAGE		10
#define CPS_GETTING_LINEUP_LIST		11
#define CPS_PROMPTING_LINEUP		12

void OSDScreenHandler::SCREEN_Choose_Provider_for_Device(long PK_Screen)
{
	PrepForWizard();
	m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_DesignObj_CurrentSecti_CONST, TOSTRING(DESIGNOBJ_butProviders_CONST));
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"ScreenHandler::SCREEN_Choose_Provider_for_Device");
#endif

	ScreenHandlerBase::SCREEN_Choose_Provider_for_Device(PK_Screen);

	// See if there are any pending channel scans.  If so, switch to the screen to wait for the channel scan to finish
	PendingTaskList pendingTaskList;
	for(map<int,class DeviceData_Base *>::iterator it = m_pOrbiter->m_pData->m_AllDevices.m_mapDeviceData_Base.begin();it != m_pOrbiter->m_pData->m_AllDevices.m_mapDeviceData_Base.end();++it)
	{
		DeviceData_Base *pDevice = it->second;
		if( pDevice->WithinCategory(DEVICECATEGORY_Media_Player_Plugins_CONST) )
			m_pOrbiter->ReportPendingTasksFromDevice(m_pOrbiter->m_pcRequestSocket->m_pClientSocket,m_pOrbiter->m_dwPK_Device,pDevice->m_dwPK_Device,&pendingTaskList);
	}

	for(list<PendingTask *>::iterator it=pendingTaskList.m_listPendingTask.begin();it!=pendingTaskList.m_listPendingTask.end();++it)
	{
		PendingTask *pPendingTask = *it;
		if( pPendingTask->m_sType == "channel_scan" )
		{
			// Send ourselves a message, don't just call CMD_Goto_Screen because we want go back to return here
			// and this screen isn't in the history yet
			DCE::SCREEN_Scanning_Progress SCREEN_Scanning_Progress(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device);
			m_pOrbiter->SendCommand(SCREEN_Scanning_Progress);
			return;
		}
	}

	m_pOrbiter->CMD_Set_Variable(VARIABLE_Location_CONST, "");
	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &OSDScreenHandler::ChooseProvider_ObjectSelected, new ObjectInfoBackData());
	RegisterCallBack(cbDataGridSelected, (ScreenHandlerCallBack) &OSDScreenHandler::ChooseProvider_DatagridSelected, new DatagridCellBackData());
	RegisterCallBack(cbMessageIntercepted, (ScreenHandlerCallBack) &OSDScreenHandler::ChooseProvider_Intercepted, new MsgInterceptorCellBackData());
	RegisterCallBack(cbDataGridRendering, (ScreenHandlerCallBack) &OSDScreenHandler::ChooseProvider_GridRendering,	new DatagridAcquiredBackData());
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::ChooseProvider_Intercepted(CallBackData *pData)
{
	//int PK_Device = atoi(m_pOrbiter->m_mapVariable_Find(VARIABLE_PK_Device_1_CONST).c_str());

	string sValue; // If this is a set variable for VARIABLE_Execution_Result_CONST, then we got back a response and can continue
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"ScreenHandler::ChooseProvider_Intercepted");
#endif
	MsgInterceptorCellBackData *pMsgInterceptorCellBackData = (MsgInterceptorCellBackData *) pData;
	if( pMsgInterceptorCellBackData->m_pMessage->m_dwMessage_Type==MESSAGETYPE_COMMAND && 
		pMsgInterceptorCellBackData->m_pMessage->m_dwID==COMMAND_Set_Variable_CONST )
	{
		map<long, string>::iterator it=pMsgInterceptorCellBackData->m_pMessage->m_mapParameters.find(COMMANDPARAMETER_PK_Variable_CONST);
		if( it==pMsgInterceptorCellBackData->m_pMessage->m_mapParameters.end() || atoi(it->second.c_str())!=VARIABLE_Execution_Result_CONST )
			return false;

		it=pMsgInterceptorCellBackData->m_pMessage->m_mapParameters.find(COMMANDPARAMETER_Value_To_Assign_CONST);
		if( it==pMsgInterceptorCellBackData->m_pMessage->m_mapParameters.end() )
			return false;  // Should never happen
		sValue = it->second;
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"ScreenHandler::ChooseProvider_Intercepted value is %s",sValue.c_str());
#endif
	}
	else
	{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"ScreenHandler::ChooseProvider_Intercepted not set variable %d",pMsgInterceptorCellBackData->m_pMessage->m_dwID);
#endif
		return false;
	}

	DesignObj_Orbiter *pObj = m_pOrbiter->FindObject(TOSTRING(DESIGNOBJ_mnuGenericDataGrid_CONST) ".0.0." TOSTRING(DESIGNOBJ_dgGenericDataGrid_CONST));
	if( !pObj )
		return false; // shouldn't happen
	
	DesignObj_DataGrid *pObj_Grid = dynamic_cast<DesignObj_DataGrid *> (pObj);
	pObj_Grid->m_bFlushOnScreen=false;
	pObj_Grid->m_GridCurRow = pObj_Grid->m_GridCurCol = 0;
	DataGridTable *pDataGridTable = new DataGridTable();
	pObj_Grid->DataGridTable_Set(pDataGridTable,0,0);
	DataGridCell *pCell;

	vector<string> vectLines;
	StringUtils::Tokenize(sValue,"\r\n",vectLines);
	vector<string>::iterator it = vectLines.begin();
	while(it!=vectLines.end() && *it!="OK")
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"ScreenHandler::ChooseProvider_Intercepted skipping %s",it->c_str());
#endif
		++it;
	}

	int iRow=0;
	while( it!=vectLines.end() )
	{
		string::size_type pos = it->find('\t');
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"ScreenHandler::ChooseProvider_Intercepted processing %s",it->c_str());
#endif
		if( pos == string::npos )
		{
			it++;
			continue; // Shouldn't happen
		}
		//int ID = atoi( it->c_str() );
		string sValue = it->substr(pos+1);
		StringUtils::Replace(&sValue,"\t"," ");  // This will be something like 1	abc	def	ghi, and 1 is the id, and display abc def ghi without the tabs in between

		pCell = new DataGridCell(sValue,it->substr(0,pos));
		pObj_Grid->DataGridTable_Get()->SetData(0,iRow++,pCell);
		++it;
	}
	
	if(iRow==0)
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"ScreenHandler::ChooseProvider_Intercepted no OK CALLING CMD_Goto_Screen");
#endif
		m_pOrbiter->CMD_Goto_Screen("",SCREEN_Choose_Provider_for_Device_CONST);
		string sText = m_pOrbiter->m_mapTextString[TEXT_error_with_provider_CONST];
		DCE::SCREEN_PopupMessage SCREEN_PopupMessage(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device,sText,"","errorprovider","1","","1");
		m_pOrbiter->ReceivedMessage(SCREEN_PopupMessage.m_pMessage);
		return false; // Keep processing it.
	}

	pDataGridTable->m_RowCount = pDataGridTable->GetRows();
	pDataGridTable->m_ColumnCount = pDataGridTable->GetCols();

	m_iStage++;

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"ScreenHandler::ChooseProvider_Intercepted row %d col %d stage %d",
		pDataGridTable->m_RowCount,pDataGridTable->m_ColumnCount,m_iStage);
#endif

	string sText;
	switch( m_iStage )
	{
	case CPS_PROMPTING_PROVIDER:
		sText = m_pOrbiter->m_mapTextString[TEXT_Choose_Provider_CONST];
		break;
	case CPS_PROMPTING_DEVICE:
		sText = m_pOrbiter->m_mapTextString[TEXT_Provider_Device_CONST];
		break;
	case CPS_PROMPTING_PACKAGE:
		sText = m_pOrbiter->m_mapTextString[TEXT_Provider_Package_CONST];
		break;
	case CPS_PROMPTING_LINEUP:
		sText = m_pOrbiter->m_mapTextString[TEXT_Provider_Lineup_CONST];
		break;
	}
	m_pOrbiter->CMD_Set_Text( TOSTRING(DESIGNOBJ_mnuGenericDataGrid_CONST),sText, TEXT_STATUS_CONST );
	m_pOrbiter->CMD_Set_Text( TOSTRING(DESIGNOBJ_mnuGenericDataGrid_CONST) ".0.0." TOSTRING(DESIGNOBJ_butResponse1_CONST),m_pOrbiter->m_mapTextString[TEXT_Ok_CONST], TEXT_STATUS_CONST );
	m_pOrbiter->GotoDesignObj( TOSTRING(DESIGNOBJ_mnuGenericDataGrid_CONST) );

	return false; // Keep processing it
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::ChooseProvider_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;
	if( m_pOrbiter->m_pScreenHistory_Current->ScreenID().empty() )
		return false; // It might be another popup ontop of us, but it's not what we're looking for
	if( pObjectInfoData->m_pObj->m_pParentObject && pObjectInfoData->m_pObj->m_pParentObject->m_iBaseObjectID==DESIGNOBJ_mnuPopupMessage_CONST )
	{
		string sTokens = m_pOrbiter->m_mapVariable_Find(VARIABLE_Misc_Data_1_CONST);
		if( pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_butResponse1_CONST && m_iStage==CPS_CHOOSE_SOURCE && sTokens.empty()==false )
		{
			m_iStage++;
			ChooseProviderGetNextStage();
			return true;
		}
	}
	else if( pObjectInfoData->m_pObj->m_pParentObject && pObjectInfoData->m_pObj->m_pParentObject->m_iBaseObjectID==DESIGNOBJ_mnuGenericKeyboard_CONST )
	{
		if( pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_butResponse1_CONST && (m_iStage==CPS_GETTING_USERNAME || m_iStage==CPS_GETTING_PASSWORD) )
		{
			if( m_iStage==CPS_GETTING_USERNAME )
				m_pOrbiter->CMD_Set_Variable(VARIABLE_Username_CONST,m_pOrbiter->m_mapVariable_Find(VARIABLE_Seek_Value_CONST));
			else
				m_pOrbiter->CMD_Set_Variable(VARIABLE_Password_CONST,m_pOrbiter->m_mapVariable_Find(VARIABLE_Seek_Value_CONST));

			m_iStage++;
			ChooseProviderGetNextStage();
			return true;
		}
		else if( pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_butResponse2_CONST )
			m_pOrbiter->CMD_Go_back("","");
	}
	return false; // Keep processing it
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::ChooseProvider_DatagridSelected(CallBackData *pData)
{
	DatagridCellBackData *pCellInfoData = (DatagridCellBackData *)pData;
	if( pCellInfoData->m_sValue.empty() )
		return false;

	if( pCellInfoData->m_nPK_Datagrid==DATAGRID_Devices_Needing_Providers_CONST )
	{
		string sPK_DeviceTemplate_MediaType = pCellInfoData->m_pDataGridCell->m_mapAttributes_Find("PK_DeviceTemplate_MediaType");
		string sPK_Provider_Source = pCellInfoData->m_pDataGridCell->m_mapAttributes_Find("PK_ProviderSource");

		string sTokens = 
			sPK_DeviceTemplate_MediaType + "\t" +
			sPK_Provider_Source + "\t" +
			pCellInfoData->m_pDataGridCell->m_mapAttributes_Find("Description") + "\t" +
			pCellInfoData->m_pDataGridCell->m_mapAttributes_Find("Comments") + "\t" +
			pCellInfoData->m_pDataGridCell->m_mapAttributes_Find("UserNamePassword") + "\t" +
			pCellInfoData->m_pDataGridCell->m_mapAttributes_Find("ProviderCommandLine") + "\t" +
			pCellInfoData->m_pDataGridCell->m_mapAttributes_Find("DeviceCommandLine") + "\t" +
			pCellInfoData->m_pDataGridCell->m_mapAttributes_Find("PackageCommandLine") + "\t" +
			pCellInfoData->m_pDataGridCell->m_mapAttributes_Find("LineupCommandLine");

		m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_Device_1_CONST, pCellInfoData->m_sValue);
		m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST,sTokens);

		// VARIABLE_Misc_Data_2_CONST are the arguments to the scripts in the format:
		// sPK_DeviceTemplate_MediaType + "\t" + sPK_Provider_Source + "\t" + sProvider + "\t" + sDevice + "\t" + sPackage + "\t" + sLineup
 
		m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST,sPK_DeviceTemplate_MediaType + "\t" + sPK_Provider_Source + "\t");
//		m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_3_CONST,pCellInfoData->m_pDataGridCell->m_mapAttributes_Find("Provider"));
		m_pOrbiter->CMD_Set_Variable(VARIABLE_Username_CONST,"");
		m_pOrbiter->CMD_Set_Variable(VARIABLE_Password_CONST,"");
		m_iStage = CPS_CHOOSE_SOURCE;
		m_pOrbiter->m_pScreenHistory_Current->ScreenID(pCellInfoData->m_sValue);
		ChooseProviderGetNextStage();
	}
	else
	{
		string sPK_DeviceTemplate_MediaType,sPK_Provider_Source,sProvider,sPackage,sDevice,sLineup;
		string::size_type pos=0;

		string sArguments = m_pOrbiter->m_mapVariable_Find(VARIABLE_Misc_Data_2_CONST);

		sPK_DeviceTemplate_MediaType = StringUtils::Tokenize( sArguments, "\t", pos );
		sPK_Provider_Source = StringUtils::Tokenize( sArguments, "\t", pos );
		sProvider = StringUtils::Tokenize(sArguments,"\t",pos);
		sDevice = StringUtils::Tokenize(sArguments,"\t",pos);
		sPackage = StringUtils::Tokenize(sArguments,"\t",pos);
		sLineup = StringUtils::Tokenize(sArguments,"\t",pos);

		switch( m_iStage )
		{
		case CPS_PROMPTING_PROVIDER:
			sProvider = pCellInfoData->m_sValue;
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "OSDScreenHandler::ChooseProvider_DatagridSelected value %s text %s",
				pCellInfoData->m_sValue.c_str(), pCellInfoData->m_sText.c_str());
			m_pOrbiter->CMD_Set_Variable(VARIABLE_Location_CONST,pCellInfoData->m_sText);
			break;
		case CPS_PROMPTING_DEVICE:
			sDevice = pCellInfoData->m_sValue;
			break;
		case CPS_PROMPTING_PACKAGE:
			sPackage = pCellInfoData->m_sValue;
			break;
		case CPS_PROMPTING_LINEUP:
			sLineup = pCellInfoData->m_sValue;
			break;
		}

		m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST,sPK_DeviceTemplate_MediaType + "\t" + sPK_Provider_Source + "\t" + sProvider + "\t" + sDevice + "\t" + sPackage + "\t" + sLineup);
		ChooseProviderGetNextStage();
	}

	return true; 
}
//-----------------------------------------------------------------------------------------------------
void OSDScreenHandler::ChooseProviderGetNextStage()
{
	if( m_pOrbiter->m_pScreenHistory_Current->PK_Screen()!=SCREEN_Choose_Provider_for_Device_CONST || 
		atoi(m_pOrbiter->m_pScreenHistory_Current->ScreenID().c_str())==0 )
	{
		m_pOrbiter->m_pScreenHistory_Current->ScreenID( m_pOrbiter->m_mapVariable_Find(VARIABLE_PK_Device_1_CONST) );  // Don't know why we keep losing the id
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"ScreenHandler::ChooseProviderGetNextStage recurring bug to fix -- screen id is: %s PK_Screen %d",
			m_pOrbiter->m_pScreenHistory_Current->ScreenID().c_str(),m_pOrbiter->m_pScreenHistory_Current->PK_Screen());
	}

	NeedToRender render( m_pOrbiter, "ScreenHandler::ChooseProviderGetNextStage()" );  // Redraw anything that was changed by this command
	string::size_type pos=0;
	string sTokens = m_pOrbiter->m_mapVariable_Find(VARIABLE_Misc_Data_1_CONST);
	string sPK_DeviceTemplate_MediaType = StringUtils::Tokenize(sTokens,"\t",pos);
	string sPK_Provider_Source = StringUtils::Tokenize( sTokens, "\t", pos );
	string sDescription = StringUtils::Tokenize( sTokens, "\t", pos );
	string sComments = StringUtils::Tokenize( sTokens, "\t", pos );
	bool bRequireUsernamePassword = StringUtils::Tokenize( sTokens, "\t", pos )=="1";
	string sProviderCommandLine = StringUtils::Tokenize( sTokens, "\t", pos );
	string sDeviceCommandLine = StringUtils::Tokenize( sTokens, "\t", pos );
	string sPackageCommandLine = StringUtils::Tokenize( sTokens, "\t", pos );
	string sLineupCommandLine = StringUtils::Tokenize( sTokens, "\t", pos );
	string sArguments = m_pOrbiter->m_mapVariable_Find(VARIABLE_Misc_Data_2_CONST);

	// If we don't already have a provider, or we do and there are some comments to show the user, we'll have this stage
	if( m_iStage<=CPS_CHOOSE_SOURCE && (sPK_Provider_Source.empty() || sComments.empty()==false) )
	{
		m_iStage = CPS_CHOOSE_SOURCE;
// TODO -- need to prompt for the source if it's not provided		if( sPK_Provider_Source.empty()==false )
		if( sComments.empty() )
		{
			m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST,"");
			DisplayMessageOnOrbiter(0,
				"Sorry.  Can't find a TV Provider automatically for your country.  Please use Myth setup.", false,"0", false,
				m_pOrbiter->m_mapTextString[TEXT_Ok_CONST]);
		}
		else
		{
			DisplayMessageOnOrbiter(0,
				sComments, false,"0", false,
				m_pOrbiter->m_mapTextString[TEXT_Ok_CONST]);
		}
// TODO -- need to prompt for the source if it's not provided		else
	}
	else if( bRequireUsernamePassword && m_iStage<=CPS_GETTING_USERNAME || m_iStage==CPS_GETTING_PASSWORD )
	{
		if( m_iStage<CPS_GETTING_USERNAME )
			m_iStage=CPS_GETTING_USERNAME;

		string sText = m_pOrbiter->m_mapTextString[ m_iStage==CPS_GETTING_USERNAME ? TEXT_What_is_your_username_CONST : TEXT_What_is_your_password_CONST ];

		m_pOrbiter->CMD_Set_Variable(VARIABLE_Seek_Value_CONST,"");
		SCREEN_GenericKeyboard(SCREEN_GenericKeyboard_CONST, 
			sText + "|" + m_pOrbiter->m_mapTextString[TEXT_Ok_CONST], "",
			"provider username/password", "0");

	}
	else if( sProviderCommandLine.empty()==false && m_iStage<=CPS_GETTING_PROVIDER_LIST )
	{
		m_iStage = CPS_GETTING_PROVIDER_LIST;
		SpawnProviderScript(sProviderCommandLine,sArguments);
	}
	else if( sDeviceCommandLine.empty()==false && m_iStage<=CPS_GETTING_DEVICE_LIST )
	{
		m_iStage = CPS_GETTING_DEVICE_LIST;
		SpawnProviderScript(sDeviceCommandLine,sArguments);
	}
	else if( sPackageCommandLine.empty()==false && m_iStage<=CPS_GETTING_PACKAGE_LIST )
	{
		m_iStage = CPS_GETTING_PACKAGE_LIST;
		SpawnProviderScript(sPackageCommandLine,sArguments);
	}
	else if( sLineupCommandLine.empty()==false && m_iStage<=CPS_GETTING_LINEUP_LIST )
	{
		m_iStage = CPS_GETTING_LINEUP_LIST;
		SpawnProviderScript(sLineupCommandLine,sArguments);
	}
	else
	{
		// We got everything.  Set the provider information for this device
		string sAllArguments = 
			m_pOrbiter->m_mapVariable_Find(VARIABLE_Username_CONST) + "\t" +
			m_pOrbiter->m_mapVariable_Find(VARIABLE_Password_CONST) + "\t" +
			sArguments;

		string sDescription=m_pOrbiter->m_mapVariable_Find(VARIABLE_Location_CONST);

		int PK_Device = atoi(m_pOrbiter->m_mapVariable_Find(VARIABLE_PK_Device_1_CONST).c_str());
		string sResponse;
		DCE::CMD_Specify_Media_Provider CMD_Specify_Media_Provider(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_MediaPlugIn,PK_Device,sAllArguments,sDescription);
		m_pOrbiter->SendCommand(CMD_Specify_Media_Provider,&sResponse); // Send with return confirmation so it gets processed first
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"ScreenHandler::ChooseProviderGetNextStage CALLING CMD_Goto_Screen");
#endif

		m_pOrbiter->CMD_Goto_Screen("",SCREEN_Choose_Provider_for_Device_CONST);
	}
}
//-----------------------------------------------------------------------------------------------------
void OSDScreenHandler::SpawnProviderScript(string sCommandLine,string sArguments)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"ScreenHandler::SpawnProviderScript CMD %s args %s stage %d",sCommandLine.c_str(),sArguments.c_str(),m_iStage);
#endif
	DeviceData_Base *pDevice_Core = m_pOrbiter->m_pData->m_AllDevices.m_mapDeviceData_Base_FindFirstOfTemplate(DEVICETEMPLATE_DCERouter_CONST);
	DeviceData_Base *pDevice_AppServer = pDevice_Core->FindFirstRelatedDeviceOfTemplate(DEVICETEMPLATE_App_Server_CONST);
	if( !pDevice_AppServer )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"ScreenHandler::SpawnProviderScript no app server"); // shouldn't ever happen
		return; 
	}

	string sResult = "0 " + StringUtils::itos(m_pOrbiter->m_dwPK_Device) + " 1 " TOSTRING(COMMAND_Set_Variable_CONST)
			" " TOSTRING(COMMANDPARAMETER_PK_Variable_CONST) " " TOSTRING(VARIABLE_Execution_Result_CONST)
			" " TOSTRING(COMMANDPARAMETER_Value_To_Assign_CONST) " ";

	string sAllArguments = 
		m_pOrbiter->m_mapVariable_Find(VARIABLE_Username_CONST) + "\t" +
		m_pOrbiter->m_mapVariable_Find(VARIABLE_Password_CONST) + "\t" +
		sArguments;

	DCE::CMD_Spawn_Application CMD_Spawn_Application(m_pOrbiter->m_dwPK_Device,pDevice_AppServer->m_dwPK_Device,sCommandLine,"getprovider",
		sAllArguments,sResult + " \"Error running script\"",
		sResult + "\"<=spawn_log=>\"",false,false,false,false);
	m_pOrbiter->SendCommand(CMD_Spawn_Application);

	DisplayMessageOnOrbiter(0,m_pOrbiter->m_mapTextString[TEXT_please_wait_for_lookup_CONST],false,"0",false,m_pOrbiter->m_mapTextString[TEXT_CANCEL_CONST]);
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::ChooseProvider_GridRendering(CallBackData *pData)
{
	// This is called every time a new section of the grid is to be rendered.  We want to find the child object for the check and hide/show it
	DatagridAcquiredBackData *pDatagridAcquiredBackData = (DatagridAcquiredBackData *) pData;  // Call back data containing relevant values for the grid/table being rendered

	// Iterate through all the cells
	for(MemoryDataTable::iterator it=pDatagridAcquiredBackData->m_pDataGridTable->m_MemoryDataTable.begin();it!=pDatagridAcquiredBackData->m_pDataGridTable->m_MemoryDataTable.end();++it)
	{
		DataGridCell *pCell = it->second;
		pair<int,int> colRow = DataGridTable::CovertColRowType(it->first);  // Get the column/row for the cell
		colRow.first -= pDatagridAcquiredBackData->m_pObj->m_GridCurCol;
		colRow.second -= pDatagridAcquiredBackData->m_pObj->m_GridCurRow;

		// See if there is an object assigned for this column/row
		map< pair<int,int>, DesignObj_Orbiter *>::iterator itobj = pDatagridAcquiredBackData->m_pObj->m_mapChildDgObjects.find( colRow );
		if( itobj!=pDatagridAcquiredBackData->m_pObj->m_mapChildDgObjects.end() )
		{
			DesignObj_Orbiter *pObj = itobj->second;  // This is the cell's object.
			DesignObj_DataList::iterator iHao;

			// Iterate through all the object's children
			for( iHao=pObj->m_ChildObjects.begin(  ); iHao != pObj->m_ChildObjects.end(  ); ++iHao )
			{
				DesignObj_Orbiter *pDesignObj_Orbiter = (DesignObj_Orbiter *)( *iHao );
				if( pDesignObj_Orbiter->m_iBaseObjectID==DESIGNOBJ_iconCheckMark_CONST )
					pDesignObj_Orbiter->m_bHidden = pCell->m_mapAttributes_Find("PK_MediaProvider").empty()==true;
			}
		}
	}
	return false;
}

/*virtual*/ void OSDScreenHandler::SCREEN_Scanning_Progress(long PK_Screen)
{
	ScreenHandlerBase::SCREEN_Scanning_Progress(PK_Screen);
	RegisterCallBack(cbDataGridRendering, (ScreenHandlerCallBack) &OSDScreenHandler::Scanning_Progress_GridRendering,	new DatagridAcquiredBackData());

	// Whenever the grid is rendered empty, then we know we're all done and should go back
}

bool OSDScreenHandler::Scanning_Progress_GridRendering(CallBackData *pData)
{
	// This is called every time a new section of the grid is to be rendered.  If the grid is empty, then go back
	DatagridAcquiredBackData *pDatagridAcquiredBackData = (DatagridAcquiredBackData *) pData;  // Call back data containing relevant values for the grid/table being rendered

	if( pDatagridAcquiredBackData->m_pDataGridTable->m_MemoryDataTable.empty() )
	{
		NeedToRender render2( m_pOrbiter, "OSDScreenHandler::Scanning_Progress_GridRendering" );  // Redraw anything that was changed by this command
		m_pOrbiter->CMD_Remove_Screen_From_History(m_pOrbiter->m_pScreenHistory_Current->ScreenID(),m_pOrbiter->m_pScreenHistory_Current->PK_Screen());
	}

	return false;
}

void OSDScreenHandler::PrepForWizard()
{
	if( m_tWizardIsRunning==0 )
	{
		RegisterPersistentCallBack(cbOnGotoScreen, (ScreenHandlerCallBack) &OSDScreenHandler::VideoWizard_OnGotoScreen, new GotoScreenCallBackData());
		m_tWizardIsRunning = time(NULL);
	}

	// The video wizard is playing video overlays using a non-standard method; it's just sending play media's directly to Xine
	// which means that set now playing isn't called, and thus the xine window isn't activated.  Just force it to be active for now.
	// This will create an issue that when the user leaves the video wizard it won't re-activate the screen saver because we have no
	// practical way to know when the user leaves this section.  If we are going to have video overlays in other places we
	// should come up with a standard way of doing this.
	m_pOrbiter->CMD_Activate_Window("pluto-xine-playback-window.pluto-xine-playback-window");

	m_pOrbiter->m_bBypassScreenSaver = true;
	if( m_pOrbiter->m_pDevice_ScreenSaver && m_pOrbiter->m_pDevice_ScreenSaver->m_bDisabled==false )
	{
		m_pOrbiter->PurgeCallBack(&Orbiter::SendOnToSS);
		m_pOrbiter->CallMaintenanceInMiliseconds( 0, &Orbiter::SendOffToSS, NULL, pe_ALL, false );
	}

	m_pOrbiter->StopScreenSaver();
}

/*virtual*/ void OSDScreenHandler::SCREEN_Add_Software(long PK_Screen)
{
	PrepForWizard();
	ScreenHandler::SCREEN_Add_Software(PK_Screen);
}

bool OSDScreenHandler::VideoWizard_OnGotoScreen(CallBackData *pData)
{
	GotoScreenCallBackData *pGotoScreenCallBackData = dynamic_cast<GotoScreenCallBackData *>(pData);

	if(NULL != pGotoScreenCallBackData)
	{
		 if(pGotoScreenCallBackData->m_nPK_Screen == SCREEN_Main_CONST)
		 {
			if(
				NULL != m_pOrbiter->m_pScreenHistory_Current && 
				m_pOrbiter->m_pScreenHistory_Current->PK_Screen() == SCREEN_PopupMessage_CONST && 
				NULL != m_pOrbiter->m_pScreenHistory_Current->GetObj() && 
				m_pOrbiter->m_pScreenHistory_Current->GetObj()->m_ObjectID != TOSTRING(DESIGNOBJ_mnuScreenSaver_CONST) ".0.0"
			)
				m_pOrbiter->CMD_Go_back("", "");
			else
				m_pOrbiter->CMD_Goto_Screen("vw final", SCREEN_Wizard_Done_CONST, 1, 0, 0);

			return true;
		 }

		if(pGotoScreenCallBackData->m_nPK_Screen == SCREEN_mnuAmbiance_CONST || pGotoScreenCallBackData->m_nPK_Screen == SCREEN_mnuPlaybackControl_CONST)
			return true;
	}

	return false;
}
