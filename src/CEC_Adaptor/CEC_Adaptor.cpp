/*
     Copyright (C) 2013 LinuxMCE

     www.linuxmce.org


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#include "CEC_Adaptor.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "CECDevice.h"
#include <cec.h>
#include "PlutoUtils/LinuxSerialUSB.h"
#include "pluto_main/Define_CommandParameter.h"

using namespace CEC;

#include <libcec/cecloader.h>

libcec_configuration g_config;
ICECCallbacks g_callbacks;

/**
 * Globals for libCEC - find a better way to do this.
 */

int CecLogMessage(void *cbParam, const cec_log_message message)
{
  switch (message.level)
    {
    case CEC_LOG_ERROR:
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"libCEC - ERR: %s",message.message);
      break;
    case CEC_LOG_WARNING:
      LoggerWrapper::GetInstance()->Write(LV_WARNING,"libCEC - WRN: %s",message.message);
      break;
    case CEC_LOG_NOTICE:
    case CEC_LOG_TRAFFIC:
    case CEC_LOG_DEBUG:
      LoggerWrapper::GetInstance()->Write(LV_DEBUG,"libCEC - DBG: %s",message.message);
      break;
    default:
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"CECLogMessage: CALLBACK! Unhandled!" );
      break;
    }
  return 0;
}

int CecKeyPress(void *cbParam, const cec_keypress key)
{
  CEC_Adaptor *pCEC_Adaptor=(CEC_Adaptor *)cbParam;
  char cKey[5];
  snprintf(cKey,5,"0x%x",key.keycode);
  string sKey=cKey;

  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Keypress - Duration %d - Keycode 0x%x",key.duration,key.keycode);

  if (key.duration > 0) // Ignore key downs
    {
      map < string,pair<string,int> >::iterator it=pCEC_Adaptor->m_mapCodesToButtons.find(sKey);
      if ( it==pCEC_Adaptor->m_mapCodesToButtons.end() )
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Can't find a mapping for button %s",sKey.c_str());
      else
	{
	  // Send it off to IRReceiverBase
	  pCEC_Adaptor->ReceivedCode(it->second.second,it->second.first.c_str());

	  // Send to AV Wizard if needed.
	  if (pCEC_Adaptor->m_dwPK_Device==DEVICEID_MESSAGESEND)
	    {
	      LoggerWrapper::GetInstance()->Write(LV_STATUS,"Sending key to AV Wizard");
	      pCEC_Adaptor->ForceKeystroke(it->second.first,pCEC_Adaptor->m_sAVWHost,pCEC_Adaptor->m_iAVWPort);
	      return 0;
	    }

	}
    }

  return 0;
}

int CecCommand(void *cbParam, const cec_command command)
{
  CEC_Adaptor *pCEC_Adaptor=(CEC_Adaptor *)cbParam;
  // TODO need to monitor commands from other cec devices to know id/la/pa/state

  cec_opcode op_code = command.GetResponseOpcode(command.opcode);
  string sop_code = pCEC_Adaptor->m_pParser->ToString(op_code);

  char sParms[1000];
  bool done = false;

  for ( int i = 0; !done; i++ ) {
    sParms[i] = command.parameters[i];
    done = (command.parameters[i] == 0);
  }

  switch ( op_code )
  {
    case CEC_OPCODE_GIVE_OSD_NAME:
  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"CECCommand: CALLBACK! From: %i, To: %i, set=%i, Opcode: %i/%s, Parm1=%i, sParms: '%s'", 
	(int) command.initiator, (int) command.destination, command.opcode_set, (int) op_code, sop_code.c_str(), command.parameters[0], sParms );

      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"CECCommand: CALLBACK! From         : %i", (int) command.initiator );
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"CECCommand: CALLBACK! To           : %i", (int) command.destination );
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"CECCommand: CALLBACK! Give OSD Name: %i", command.parameters[0] );
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"CECCommand: CALLBACK! sParms       : %s", sParms );
      break;
    case CEC_OPCODE_SET_OSD_NAME:
  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"CECCommand: CALLBACK! From: %i, To: %i, set=%i, Opcode: %i/%s, Parm1=%i, sParms: '%s'", 
	(int) command.initiator, (int) command.destination, command.opcode_set, (int) op_code, sop_code.c_str(), command.parameters[0], sParms );

      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"CECCommand: CALLBACK! From         : %i", (int) command.initiator );
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"CECCommand: CALLBACK! To           : %i", (int) command.destination );
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"CECCommand: CALLBACK! Set OSD Name : %i", command.parameters[0] );
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"CECCommand: CALLBACK! sParms       : %s", sParms );
      break;
    case CEC_OPCODE_ACTIVE_SOURCE:
  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"CECCommand: CALLBACK! From: %i, To: %i, set=%i, Opcode: %i/%s, Parm1=%i, sParms: '%s'", 
	(int) command.initiator, (int) command.destination, command.opcode_set, (int) op_code, sop_code.c_str(), command.parameters[0], sParms );

      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"CECCommand: CALLBACK! From         : %i", (int) command.initiator );
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"CECCommand: CALLBACK! To           : %i", (int) command.destination );
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"CECCommand: CALLBACK! Active Source: %i", command.parameters[0] );
      break;
    case CEC_OPCODE_DEVICE_VENDOR_ID:
      {
  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"CECCommand: CALLBACK! From: %i, To: %i, set=%i, Opcode: %i/%s, Parm1=%i, sParms: '%s'", 
	(int) command.initiator, (int) command.destination, command.opcode_set, (int) op_code, sop_code.c_str(), command.parameters[0], sParms );

      cec_vendor_id VendorId=(cec_vendor_id) command.parameters[0];
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"CECCommand: CALLBACK! VendorID     : %i/%s", (int) VendorId,  pCEC_Adaptor->m_pParser->ToString(VendorId) );
      break;
      }
    case CEC_OPCODE_NONE:
//      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"CECCommand: CALLBACK! Poll. parameters[0]: %i", command.parameters[0] );
//      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"CECCommand: CALLBACK! Poll. sParms       : %s", sParms );
      break;
    default:
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"CECCommand: CALLBACK! Unhandled!" );
      break;
  }



  return 0;
}

int CecConfigurationChanged(void *cbParam, const libcec_configuration config)
{
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"CECConfigurationChanged: CALLBACK! Unhandled!");
      return 0;
}

int CecAlert(void *cbParam, const libcec_alert type, const libcec_parameter param)
{
  switch (type)
  {
  case CEC_ALERT_CONNECTION_LOST:
/*    if (!CReconnect::Get().IsRunning())
    {
      PrintToStdOut("Connection lost - trying to reconnect\n");
      CReconnect::Get().CreateThread(false);
    }
*/
  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"CEC_ALERT_CONNECTION_LOST!");
    break;
  default:
    break;
  }
  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"CEC_ALERT!");
  return 0;
}


int CecMenuStateChanged(void *cbParam, const cec_menu_state state)
{
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"CECMenuStateChanged: CALLBACK! Unhandled!");
      return 0;
}

void CecSourceActivated(void *cbParam, const cec_logical_address, const uint8_t val)
{
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"CECSourceActivated: CALLBACK! Unhandled!");
}

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
CEC_Adaptor::CEC_Adaptor(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
  : CEC_Adaptor_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
  , IRReceiverBase(this)
{
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
CEC_Adaptor::CEC_Adaptor(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: CEC_Adaptor_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
}

//<-dceag-dest-b->
CEC_Adaptor::~CEC_Adaptor()
//<-dceag-dest-e->
{
	
}

void CEC_Adaptor::PrepareToDelete()
{
  Command_Impl::PrepareToDelete();
  if (m_pParser)
    {
      m_pParser->Close();
      UnloadLibCec(m_pParser);
      m_pParser=NULL;
    }
}

//<-dceag-getconfig-b->
bool CEC_Adaptor::GetConfig()
{
	if( !CEC_Adaptor_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated

	if ( m_dwPK_Device != DEVICEID_MESSAGESEND && !m_bLocalMode )
	  {
	    if ( !m_Virtual_Device_Translator.GetConfig(m_pData) )
	      return false;
	
	    // Set up IRBase/IRReceiverBase
	    IRBase::setCommandImpl(this);
	    IRBase::setAllDevices(&(GetData()->m_AllDevices));
	    IRReceiverBase::GetConfig(m_pData);
	  }

	if ( !m_bLocalMode )
	  {
	      LoggerWrapper::GetInstance()->Write(LV_STATUS,"NOT Local Mode - find keys from remote configuration");
	      DeviceData_Base *pDevice = m_pData->m_AllDevices.m_mapDeviceData_Base_FindFirstOfCategory(DEVICECATEGORY_Infrared_Plugins_CONST);
	    
	    if ( pDevice )
	      m_dwPK_Device_IRPlugin = pDevice->m_dwPK_Device;
	    else
	      m_dwPK_Device_IRPlugin = 0;
	    
	    LoggerWrapper::GetInstance()->Write(LV_STATUS,"Getting sibling remotes");
	    string sResult;
	    DCE::CMD_Get_Sibling_Remotes CMD_Get_Sibling_Remotes(m_dwPK_Device,m_dwPK_Device_IRPlugin, DEVICECATEGORY_CEC_Remote_Controls_CONST, &sResult);
	    SendCommand(CMD_Get_Sibling_Remotes);
	    
	    vector<string> vectRemotes;
	    StringUtils::Tokenize(sResult, "`",vectRemotes);
	    size_t i;
	    for (i=0;i<vectRemotes.size();i++)
	      {
		vector<string> vectRemoteConfigs;
		StringUtils::Tokenize(vectRemotes[i],"~",vectRemoteConfigs);
		if (vectRemoteConfigs.size() == 3)
		  {
		    vector<string> vectCodes;
		    int PK_DeviceRemote = atoi(vectRemoteConfigs[0].c_str());
		    LoggerWrapper::GetInstance()->Write(LV_STATUS,"Adding remote ID %d, layout %s\r\n",PK_DeviceRemote,vectRemoteConfigs[1].c_str());
		    StringUtils::Tokenize(vectRemoteConfigs[2],"\r\n",vectCodes);
		    for (size_t s=0;s<vectCodes.size();++s)
		      {
			string::size_type pos=0;
			string sButton = StringUtils::Tokenize(vectCodes[s]," ",pos);
			while(pos<vectCodes[s].size())
			  {
			    string sCode = StringUtils::Tokenize(vectCodes[s]," ",pos);
			    m_mapCodesToButtons[sCode] = make_pair<string,int> (sButton,PK_DeviceRemote);
			    LoggerWrapper::GetInstance()->Write(LV_STATUS,"Code: %s will fire button %s",sCode.c_str(),sButton.c_str());
			  }
		      }
		  }
	      }
m_mapLMCEtoCECcodes[COMMAND_Send_Generic_EnterGo_CONST] = CEC_USER_CONTROL_CODE_SELECT;
m_mapLMCEtoCECcodes[COMMAND_Move_Up_CONST] = CEC_USER_CONTROL_CODE_UP;
m_mapLMCEtoCECcodes[COMMAND_Move_Down_CONST] = CEC_USER_CONTROL_CODE_DOWN;
m_mapLMCEtoCECcodes[COMMAND_Move_Left_CONST] = CEC_USER_CONTROL_CODE_LEFT;
m_mapLMCEtoCECcodes[COMMAND_Move_Right_CONST] = CEC_USER_CONTROL_CODE_RIGHT;
m_mapLMCEtoCECcodes[COMMAND_Back_Prior_Menu_CONST] = CEC_USER_CONTROL_CODE_EXIT;
m_mapLMCEtoCECcodes[COMMAND_PS_Playstation_CONST] = CEC_USER_CONTROL_CODE_ROOT_MENU;
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_ROOT_MENU;
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_SETUP_MENU;
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_CONTENTS_MENU;
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_FAVORITE_MENU;
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_TOP_MENU;
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_DVD_MENU;
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_NUMBER_ENTRY_MODE; //           = 0x1D,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_NUMBER11; //                    = 0x1E,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_NUMBER12; //                    = 0x1F,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_NUMBER0; //                     = 0x20,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_NUMBER1; //                     = 0x21,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_NUMBER2; //                     = 0x22,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_NUMBER3; //                     = 0x23,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_NUMBER4; //                     = 0x24,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_NUMBER5; //                     = 0x25,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_NUMBER6; //                     = 0x26,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_NUMBER7; //                     = 0x27,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_NUMBER8; //                     = 0x28,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_NUMBER9; //                     = 0x29,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_DOT; //                         = 0x2A,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_ENTER; //                       = 0x2B,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_CLEAR; //                       = 0x2C,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_NEXT_FAVORITE; //               = 0x2F,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_CHANNEL_UP; //                  = 0x30,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_CHANNEL_DOWN; //                = 0x31,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_PREVIOUS_CHANNEL; //            = 0x32,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_SOUND_SELECT; //                = 0x33,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_INPUT_SELECT; //                = 0x34,
m_mapLMCEtoCECcodes[COMMAND_Display_CONST] = CEC_USER_CONTROL_CODE_DISPLAY_INFORMATION; //         = 0x35,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_HELP; //                        = 0x36,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_PAGE_UP; //                     = 0x37,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_PAGE_DOWN; //                   = 0x38,
  // reserved: 0x39 ... 0x3F
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_POWER; //                       = 0x40,
m_mapLMCEtoCECcodes[COMMAND_Vol_Up_CONST] = CEC_USER_CONTROL_CODE_VOLUME_UP; //                   = 0x41,
m_mapLMCEtoCECcodes[COMMAND_Vol_Down_CONST] = CEC_USER_CONTROL_CODE_VOLUME_DOWN; //                 = 0x42,
m_mapLMCEtoCECcodes[COMMAND_Mute_CONST] = CEC_USER_CONTROL_CODE_MUTE; //                        = 0x43,
m_mapLMCEtoCECcodes[COMMAND_Change_Playback_Speed_CONST] = CEC_USER_CONTROL_CODE_PLAY; //                        = 0x44,
m_mapLMCEtoCECcodes[COMMAND_Stop_CONST] = CEC_USER_CONTROL_CODE_STOP; //                        = 0x45,
m_mapLMCEtoCECcodes[COMMAND_Pause_Media_CONST] = CEC_USER_CONTROL_CODE_PAUSE; //                       = 0x46,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_RECORD; //                      = 0x47,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_REWIND; //                      = 0x48,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_FAST_FORWARD; //                = 0x49,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_EJECT; //                       = 0x4A,
m_mapLMCEtoCECcodes[COMMAND_Skip_Fwd_ChannelTrack_Greater_CONST] = CEC_USER_CONTROL_CODE_FORWARD; //                     = 0x4B,
m_mapLMCEtoCECcodes[COMMAND_Skip_Back_ChannelTrack_Lower_CONST] = CEC_USER_CONTROL_CODE_BACKWARD; //                    = 0x4C,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_STOP_RECORD; //                 = 0x4D,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_PAUSE_RECORD; //                = 0x4E,
  // reserved: 0x4F
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_ANGLE; //                       = 0x50,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_SUB_PICTURE; //                 = 0x51,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_VIDEO_ON_DEMAND; //             = 0x52,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_ELECTRONIC_PROGRAM_GUIDE; //    = 0x53,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_TIMER_PROGRAMMING; //           = 0x54,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_INITIAL_CONFIGURATION; //       = 0x55,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_SELECT_BROADCAST_TYPE; //       = 0x56,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_SELECT_SOUND_PRESENTATION; //   = 0x57,
  // reserved: 0x58 ... 0x5F
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_PLAY_FUNCTION; //               = 0x60,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_PAUSE_PLAY_FUNCTION; //         = 0x61,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_RECORD_FUNCTION; //             = 0x62,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_PAUSE_RECORD_FUNCTION; //       = 0x63,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_STOP_FUNCTION; //               = 0x64,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_MUTE_FUNCTION; //               = 0x65,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_RESTORE_VOLUME_FUNCTION; //     = 0x66,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_TUNE_FUNCTION; //               = 0x67,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_SELECT_MEDIA_FUNCTION; //       = 0x68,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_SELECT_AV_INPUT_FUNCTION; //    = 0x69,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_SELECT_AUDIO_INPUT_FUNCTION; // = 0x6A,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_POWER_TOGGLE_FUNCTION; //       = 0x6B,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_POWER_OFF_FUNCTION; //          = 0x6C,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_POWER_ON_FUNCTION; //           = 0x6D,
  // reserved: 0x6E ... 0x70
m_mapLMCEtoCECcodes[COMMAND_Blue_CONST] = CEC_USER_CONTROL_CODE_F1_BLUE; //                     = 0x71,
m_mapLMCEtoCECcodes[COMMAND_Red_CONST] = CEC_USER_CONTROL_CODE_F2_RED; //                      = 0X72,
m_mapLMCEtoCECcodes[COMMAND_Green_CONST] = CEC_USER_CONTROL_CODE_F3_GREEN; //                    = 0x73,
m_mapLMCEtoCECcodes[COMMAND_Yellow_CONST] = CEC_USER_CONTROL_CODE_F4_YELLOW; //                   = 0x74,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_F5; //                          = 0x75,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_DATA; //                        = 0x76,
  // reserved: 0x77 ... 0xFF
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_AN_RETURN; //                   = 0x91, // return (Samsung)
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_AN_CHANNELS_LIST; //            = 0x96, // channels list (Samsung)
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_MAX; //                         = 0x96,
//m_mapLMCEtoCECcodes[??] = CEC_USER_CONTROL_CODE_UNKNOWN; //                     = 0xFF

	  }
	else
	  {
	    // Local Mode, hard code values to lookup.
	    // This may be expanded later, but I merely want
	    // to implement the bare minimum for a direction pad. -tschak
	    m_mapCodesToButtons["0x0"] = make_pair<string,int>("ok",0);
	    m_mapCodesToButtons["0x1"] = make_pair<string,int>("up",0);
	    m_mapCodesToButtons["0x2"] = make_pair<string,int>("down",0);
	    m_mapCodesToButtons["0x3"] = make_pair<string,int>("left",0);
	    m_mapCodesToButtons["0x4"] = make_pair<string,int>("right",0);
	    m_mapCodesToButtons["0xd"] = make_pair<string,int>("back",0);
	  }

	g_config.Clear();
	g_callbacks.Clear();
	snprintf(g_config.strDeviceName, 13, "LinuxMCE");
	g_config.clientVersion = _LIBCEC_VERSION_CURRENT;
	g_config.bActivateSource = 0;
	g_callbacks.CBCecLogMessage = &CecLogMessage;
	g_callbacks.CBCecKeyPress = &CecKeyPress;
	g_callbacks.CBCecCommand = &CecCommand;
	g_callbacks.CBCecConfigurationChanged = &CecConfigurationChanged;
	g_callbacks.CBCecAlert = &CecAlert;
	g_callbacks.CBCecMenuStateChanged = &CecMenuStateChanged;
	g_callbacks.CBCecSourceActivated = &CecSourceActivated;
	g_config.callbacks = &g_callbacks;
	g_config.callbackParam = this;


        // Cannot be both a playback and recording device, but a recording device provides playback capabilities
	g_config.deviceTypes.Add(CEC_DEVICE_TYPE_PLAYBACK_DEVICE);
//	g_config.deviceTypes.Add(CEC_DEVICE_TYPE_RECORDING_DEVICE);
//	g_config.deviceTypes.Add(CEC_DEVICE_TYPE_TUNER);

	m_pParser = LibCecInitialise(&g_config);

	if (!m_pParser)
	  {
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not load libcec.so. Bailing!");
	    return false;
	  }

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"libCEC initialized - Version %s",m_pParser->ToString((cec_version)g_config.serverVersion));

	m_pParser->InitVideoStandalone();

	m_sPort = TranslateSerialUSB(DATA_Get_COM_Port_on_PC());

	if (m_sPort.empty() || m_sPort == "0")
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"No COM Port specified. Trying autodetect.  Please set a port in the Interfaces section of Web Admin, or re-detect device.");

		cec_adapter devices[10];
		uint8_t iDevicesFound = m_pParser->FindAdapters(devices, 10, NULL);
		if (iDevicesFound <= 0)
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"No devices found with autodetect");
			UnloadLibCec(m_pParser);
			return false;
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Device path: %s, port: %s",devices[0].path,devices[0].comm);
			m_sPort = devices[0].comm;
		}
	}

	if (!m_pParser->Open(m_sPort.c_str()))
	{
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not open serial port %s",m_sPort.c_str());
	    UnloadLibCec(m_pParser);
	    return false;
	}

	// Set the physical address of MD, in case it is not detected by libCEC?
//	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Setting Physical Address to: %s, %u", DATA_Get_PortChannel_Number().c_str() ,  strtol( DATA_Get_PortChannel_Number().c_str(), NULL, 16) );
//	m_pParser->SetPhysicalAddress( strtol( DATA_Get_PortChannel_Number().c_str(), NULL, 16) );

	m_myCECAddresses = m_pParser->GetLogicalAddresses(); // Return logical addresses for *this* device
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"My Primary CEC Logical Address  : %s",m_pParser->ToString( (cec_logical_address) m_myCECAddresses.primary ) );
	for ( uint8_t iPtr = 0; iPtr <= 11; iPtr++ ) {
		if ( m_myCECAddresses.addresses[iPtr] ) {
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"My Secondary CEC Logical Address: %s",m_pParser->ToString( (cec_logical_address) m_myCECAddresses.addresses[iPtr] ) );
		}
	}

	// check all active devices on the CEC bus, let's try to match them to DTs in our tree
	m_CECAddresses.Clear();
	m_CECAddresses = m_pParser->GetActiveDevices(); // Seems to return logical addresses for all known CEC devices on the bus

	m_mapPhysicalAddress_to_LA.clear();
	m_mapPK_Device_to_PA.clear();

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Primary CEC bus Logical Address: %s",m_pParser->ToString( m_CECAddresses.primary ) );
	for ( uint8_t iPtr = 0; iPtr <= 11; iPtr++ ) {
		if ( m_CECAddresses[iPtr] ) {
			// iPtr = Logical Address

			uint64_t iPhysicalAddress = m_pParser->GetDevicePhysicalAddress( (cec_logical_address) iPtr );
			m_mapPhysicalAddress_to_LA[iPhysicalAddress] = iPtr;

			CECDevice device;
			device.iVendorId = m_pParser->GetDeviceVendorId((cec_logical_address)iPtr);
			device.iPhysicalAddress = m_pParser->GetDevicePhysicalAddress((cec_logical_address)iPtr);
			device.bActive = m_pParser->IsActiveSource((cec_logical_address)iPtr);
			device.iCecVersion = m_pParser->GetDeviceCecVersion((cec_logical_address)iPtr);
			device.power = m_pParser->GetDevicePowerStatus((cec_logical_address)iPtr);
			device.osdName = m_pParser->GetDeviceOSDName((cec_logical_address)iPtr);

//			device.lang = CECDEVICE_UNKNOWN;
			m_pParser->GetDeviceMenuLanguage((cec_logical_address)iPtr, &device.lang);

			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Active CEC Logical Address: %s, at Physical Address: %X, by Vendor: %s",m_pParser->ToString( (cec_logical_address) iPtr ), device.iPhysicalAddress, m_pParser->ToString( (cec_vendor_id) device.iVendorId ) );


			m_vectCecDevices.push_back(device);
		}
	}

	// Find MDs PK_Device
	int iPK_Device = 0;
	DeviceData_Base *pDevice = m_pData->GetTopMostDevice();
	if ( pDevice )
		iPK_Device = pDevice->m_dwPK_Device;
//	string tmp_node_id = pDevice->m_mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST);
//	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MD's PK_Device=%i, CEC Physical Address: %s",iPK_Device, tmp_node_id.c_str() );
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"MD's PK_Device=%i",iPK_Device );

	// Check number of children (should only be one)
	size_t size = m_pData->m_vectDeviceData_Impl_Children.size();
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Number of children=%i", size );

	// Find PK_Device of child devices, this should only be an embedded transmit device template
	for(std::vector<DeviceData_Impl *>::iterator it = m_pData->m_vectDeviceData_Impl_Children.begin();
			it !=  m_pData->m_vectDeviceData_Impl_Children.end(); ++it)
	{
		DeviceData_Impl *pChildDevice = (*it);

		if ( pChildDevice != NULL ) {
			string tmp_node_id = pChildDevice->m_mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST);
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Child's PK_Device=%i, CEC Physical Address: %s (N/A)", pChildDevice->m_dwPK_Device, tmp_node_id.c_str() );

			// Check number of children (should only be one - the embedded transmit device)
			size_t size1 = pChildDevice->m_vectDeviceData_Impl_Children.size();
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Number of grand children=%i", size1 );

			// iterate over grand children - LCME DTs for CEC bus devices
			DeviceData_Impl *pChildDevice1 = NULL;
			for( VectDeviceData_Impl::const_iterator it1 = pChildDevice->m_vectDeviceData_Impl_Children.begin();
				it1 != pChildDevice->m_vectDeviceData_Impl_Children.end(); ++it1 )
			{
				pChildDevice1 = (*it1);
				if( pChildDevice1 != NULL )
				{
					string sPhysicalAddress = pChildDevice1->m_mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST);
					m_mapPK_Device_to_PA[pChildDevice1->m_dwPK_Device] = strtol(sPhysicalAddress.c_str(), NULL, 16);
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"Grandchild's PK_Device=%i, CEC Physical Address: %s, %i", pChildDevice1->m_dwPK_Device, sPhysicalAddress.c_str(), m_mapPK_Device_to_PA[pChildDevice1->m_dwPK_Device] );
				}
			}

		}
	}

	cec_logical_address ActiveSource = m_pParser->GetActiveSource();
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"CEC Active Source: %s", m_pParser->ToString(ActiveSource) );

	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool CEC_Adaptor::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
CEC_Adaptor_Command *Create_CEC_Adaptor(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new CEC_Adaptor(pPrimaryDeviceCommand, pData, pEvent, pRouter);
}
//<-dceag-createinst-e->

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void CEC_Adaptor::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
  sCMD_Result = "UNHANDLED CHILD";

  LoggerWrapper::GetInstance()->Write(LV_STATUS,"ReceivedCommandForChild - To: %i, ID: %i", pMessage->m_dwPK_Device_To, pMessage->m_dwID);

  cout << "Message ID: " << pMessage->m_dwID << endl;
  cout << "From:" << pMessage->m_dwPK_Device_From << endl;
  cout << "To: " << pMessage->m_dwPK_Device_To << endl;

  map<long, string>::iterator i;
  for (i = pMessage->m_mapParameters.begin(); i != pMessage->m_mapParameters.end(); i++)
  {
    cout << "Parameter: " << i->first << " Value: " << i->second << endl;
  }

  string sPhysicalAddress = pDeviceData_Impl->m_mapParameters[DEVICEDATA_PortChannel_Number_CONST]; // PortChannel of 'To' device
  uint64_t iPhysicalAddress = m_mapPK_Device_to_PA[pMessage->m_dwPK_Device_To]; // Get the Physical Address based on PK_Device
  cout << "**Physical Address: " << sPhysicalAddress << ", " << iPhysicalAddress << endl;

  cec_logical_address LogicalAddress = (cec_logical_address) m_mapPhysicalAddress_to_LA[iPhysicalAddress];

  LoggerWrapper::GetInstance()->Write(LV_STATUS,"ReceivedCommandForChild - To PA: %s, LA: %i", sPhysicalAddress.c_str(), (int) LogicalAddress );

  switch (pMessage->m_dwID)
  {
    case COMMAND_Send_Generic_EnterGo_CONST:
    case COMMAND_Move_Up_CONST:
    case COMMAND_Move_Down_CONST:
    case COMMAND_Move_Left_CONST:
    case COMMAND_Move_Right_CONST:
    case COMMAND_Back_Prior_Menu_CONST:
    case COMMAND_Display_CONST:
    case COMMAND_Change_Playback_Speed_CONST:
    case COMMAND_Pause_Media_CONST:
    case COMMAND_Skip_Fwd_ChannelTrack_Greater_CONST:
    case COMMAND_Skip_Back_ChannelTrack_Lower_CONST:
    case COMMAND_Stop_CONST:
    case COMMAND_Blue_CONST:
    case COMMAND_Red_CONST:
    case COMMAND_Green_CONST:
    case COMMAND_Yellow_CONST:
    case COMMAND_PS_Playstation_CONST:
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"ReceivedCommandForChild - UCP - To LA: %i, Code: %i", (int) LogicalAddress, m_mapLMCEtoCECcodes[pMessage->m_dwID] );
      if ( m_pParser->SendKeypress( LogicalAddress, m_mapLMCEtoCECcodes[pMessage->m_dwID] ) )
      {
	if ( m_pParser->SendKeyRelease( LogicalAddress ) ) 
        {
	  LoggerWrapper::GetInstance()->Write(LV_STATUS,"ReceivedCommandForChild: UCP - successful");
	  sCMD_Result = "OK";
        }
      }
      return;
      break;
    case COMMAND_Generic_On_CONST:
      if ( m_pParser->PowerOnDevices( LogicalAddress ) )
      {
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"ReceivedCommandForChild: Power On");
	sCMD_Result = "OK";
      }
      return;
      break;
    case COMMAND_Generic_Off_CONST:
      if ( m_pParser->StandbyDevices( LogicalAddress ) )
      {
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"ReceivedCommandForChild: Power Off");
        sCMD_Result = "OK";
      }
      return;
      break;
    case COMMAND_Vol_Up_CONST:
      if ( m_pParser->VolumeUp() )
      {
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"ReceivedCommandForChild: Volume Up");
        sCMD_Result = "OK";
      }
      return;
      break;
    case COMMAND_Vol_Down_CONST:
      if ( m_pParser->VolumeDown() )
      {
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"ReceivedCommandForChild: Volume Down");
        sCMD_Result = "OK";
      }
      return;
      break;
    case COMMAND_Mute_CONST:
      if ( m_pParser->AudioToggleMute() )
      {
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"ReceivedCommandForChild: Mute");
        sCMD_Result = "OK";
      }
      return;
      break;
    case COMMAND_Input_Select_CONST:
      char cPort = '0';
      string sCommand = pMessage->m_mapParameters[COMMANDPARAMETER_PK_Command_Input_CONST];
      uint64_t iCommand = strtol ( sCommand.c_str(), NULL, 10 );
      switch ( iCommand )
      {
	case COMMAND_HDMI_1_CONST:
	  cPort = '1';
	  break;
	case COMMAND_HDMI_2_CONST:
	  cPort = '2';
	  break;
	case COMMAND_HDMI_3_CONST:
	  cPort = '3';
	  break;
	case COMMAND_HDMI_4_CONST:
	  cPort = '4';
	  break;
	case COMMAND_HDMI_5_CONST:
	  cPort = '5';
	  break;
	case COMMAND_HDMI_6_CONST:
	  cPort = '6';
	  break;
	default:
	  return;
	  break;
      }

      std::size_t found = sPhysicalAddress.find_first_of("0");
      sPhysicalAddress[found]=cPort;

      LoggerWrapper::GetInstance()->Write(LV_STATUS,"ReceivedCommandForChild: Make input active: %s",  sPhysicalAddress.c_str() );

      LoggerWrapper::GetInstance()->Write(LV_STATUS,"TRYINGTOSETACTIVE???? - To PA: %s, LA: %i", sPhysicalAddress.c_str(), (int) LogicalAddress );

      // TODO: 0xXF - X = our LA
      uint8_t from = ( m_myCECAddresses.primary << 4 ) | 0x0F;
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"ReceivedCommandForChild: Make input active from/to: %u",  from );

      cec_command cmd;
      cmd.Clear();
      cmd.PushBack(from); /* from - to */
      cmd.PushBack(0x82); /* cmd number */
      cmd.PushBack( strtol( sPhysicalAddress.substr(0, 2).c_str(), NULL, 16) );
      cmd.PushBack( strtol( sPhysicalAddress.substr(2, 2).c_str(), NULL, 16) );

      if ( m_pParser->Transmit(cmd) )
      // SetStreamPath to destination PA
//      if ( m_pParser->SetStreamPath( strtol( sPhysicalAddress.c_str(), NULL, 16 )) )
      {
        LoggerWrapper::GetInstance()->Write(LV_STATUS,"ReceivedCommandForChild: SetStreamPath OK - %s", sPhysicalAddress.c_str() );

	if ( LogicalAddress == m_myCECAddresses.primary && m_pParser->SetActiveSource() )
	{
	        LoggerWrapper::GetInstance()->Write(LV_STATUS,"ReceivedCommandForChild: SetActiveSource!! OK - %s", sPhysicalAddress.c_str() );
	}

        sCMD_Result = "OK";
      }
      return;
      break;
  }

  LoggerWrapper::GetInstance()->Write(LV_STATUS,"ReceivedCommandForChild: UNHANDLED COMMAND/CHILD - To: %i, ID: %i",  pMessage->m_dwPK_Device_To, pMessage->m_dwID);
/*
  if (IRBase::ProcessMessage(pMessage))
    {
      printf("Message Processed by IRBase");
      sCMD_Result = "OK";
    }
*/

}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void CEC_Adaptor::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"ReceivedCommandForChild: UNKNOWN DEVICE");
	sCMD_Result = "UNKNOWN DEVICE";
}

void CEC_Adaptor::CreateChildren()
{
  CEC_Adaptor_Command::CreateChildren();
  Start(); // call IRBase Start();
}

void CEC_Adaptor::SendIR(string Port, string sIRCode, int iRepeat)
{
  // Not used, but must be implemented as per IRBase
}

//<-dceag-sample-b->!

/*

	COMMANDS TO IMPLEMENT

*/

//<-dceag-c191-b->

	/** @brief COMMAND: #191 - Send Code */
	/** The I/R code -- usually in Pronto format */
		/** @param #9 Text */
			/** The I/R code -- usually in Pronto format */

void CEC_Adaptor::CMD_Send_Code(string sText,string &sCMD_Result,Message *pMessage)
//<-dceag-c191-e->
{
	cout << "Need to implement command #191 - Send Code" << endl;
	cout << "Parm #9 - Text=" << sText << endl;
}

//<-dceag-c245-b->

	/** @brief COMMAND: #245 - Learn IR */
	/** The next IR code received is to be learned in Pronto format and fire a Store IR Code command to the I/R Plugin when done */
		/** @param #2 PK_Device */
			/** You can specify the device to learn for here, or you can send the command to the device itself and leave this blank */
		/** @param #8 On/Off */
			/** Turn IR Learning mode on or off
0, 1 */
		/** @param #25 PK_Text */
			/** If specified, the text object  which should contain the result of the learn command */
		/** @param #154 PK_Command */
			/** Command ID for which the learning is done for */

void CEC_Adaptor::CMD_Learn_IR(int iPK_Device,string sOnOff,int iPK_Text,int iPK_Command,string &sCMD_Result,Message *pMessage)
//<-dceag-c245-e->
{
	cout << "Need to implement command #245 - Learn IR" << endl;
	cout << "Parm #2 - PK_Device=" << iPK_Device << endl;
	cout << "Parm #8 - OnOff=" << sOnOff << endl;
	cout << "Parm #25 - PK_Text=" << iPK_Text << endl;
	cout << "Parm #154 - PK_Command=" << iPK_Command << endl;
}

//<-dceag-c687-b->

	/** @brief COMMAND: #687 - Set Screen Type */
	/** Sent by Orbiter when the screen changes to tells the i/r receiver what type of screen is displayed so it can adjust mappings if necessary. */
		/** @param #48 Value */
			/** a character: M=Main Menu, m=other menu, R=Pluto Remote, r=Non-pluto remote, N=navigable OSD on media dev, f=full screen media app, F=File Listing, c=computing list, C=Computing full screen */

void CEC_Adaptor::CMD_Set_Screen_Type(int iValue,string &sCMD_Result,Message *pMessage)
//<-dceag-c687-e->
{
	cout << "Need to implement command #687 - Set Screen Type" << endl;
	cout << "Parm #48 - Value=" << iValue << endl;
}


