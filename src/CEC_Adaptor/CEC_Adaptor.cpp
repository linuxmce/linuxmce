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

#include <libcec/cecloader.h>
#include "PlutoUtils/LinuxSerialUSB.h"
using namespace CEC;

libcec_configuration g_config;
ICECCallbacks g_callbacks;

/**
 * Globals for libCEC - find a better way to do this.
 */

int CecLogMessage(void *UNUSED(cbParam), const cec_log_message &message)
{
  switch (message.level)
    {
    case CEC_LOG_ERROR:
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"libCEC: %s",message.message);
      break;
    case CEC_LOG_WARNING:
      LoggerWrapper::GetInstance()->Write(LV_WARNING,"libCEC: %s",message.message);
      break;
    case CEC_LOG_NOTICE:
    case CEC_LOG_TRAFFIC:
    case CEC_LOG_DEBUG:
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"libCEC: %s",message.message);
      break;
    default:
      break;
    }
  return 0;
}

int CecKeyPress(void *cbParam, const cec_keypress &key)
{
  CEC_Adaptor *pCEC_Adaptor=(CEC_Adaptor *)cbParam;
  char cKey[5];
  snprintf(cKey,5,"0x%x",key.keycode);
  string sKey=cKey;

  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Keypress - Duration %d - Keycode 0x%x",key.duration,key.keycode);

  if (key.duration > 0) // Ignore key downs
    {
      map <string,pair<string,int> >::iterator it=pCEC_Adaptor->m_mapCodesToButtons.find(sKey);
      if ( it==pCEC_Adaptor->m_mapCodesToButtons.end() )
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Can't find a mapping for button %s",sKey.c_str());
      else
	{
	  // Send it off to IRReceiverBase
	  pCEC_Adaptor->ReceivedCode(it->second.second,it->second.first.c_str());
	  
	  // Send to AV Wizard if needed.
	  if (pCEC_Adaptor->m_dwPK_Device==DEVICEID_MESSAGESEND)
	    {
	      pCEC_Adaptor->ForceKeystroke(it->second.first,pCEC_Adaptor->m_sAVWHost,pCEC_Adaptor->m_iAVWPort);
	      return 0;
	    }
	  
	}
    }

  return 0;
}

int CecCommand(void *UNUSED(cbParam), const cec_command &UNUSED(command))
{
  // TODO
  return 0;
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
	    	    DeviceData_Base *pDevice = m_pData->m_AllDevices.m_mapDeviceData_Base_FindFirstOfCategory(DEVICECATEGORY_Infrared_Plugins_CONST);
	    
	    if ( pDevice )
	      m_dwPK_Device_IRPlugin = pDevice->m_dwPK_Device;
	    else
	      m_dwPK_Device_IRPlugin = 0;
	    
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
	snprintf(g_config.strDeviceName, 13, "LinuxMCE");
	g_config.callbackParam = this;
	g_config.clientVersion = CEC_CLIENT_VERSION_1_6_1;
	g_callbacks.CBCecLogMessage = &CecLogMessage;
	g_callbacks.CBCecKeyPress = &CecKeyPress;
	g_callbacks.CBCecCommand = &CecCommand;
	g_config.callbacks = &g_callbacks;

	g_config.deviceTypes.add(CEC_DEVICE_TYPE_RECORDING_DEVICE);
	g_config.deviceTypes.add(CEC_DEVICE_TYPE_TUNER);
	g_config.deviceTypes.add(CEC_DEVICE_TYPE_PLAYBACK_DEVICE);

	m_pParser = LibCecInitialise(&g_config);
	
	// Tweak with this until we like it -tschak
	m_pParser->SetLogicalAddress(CECDEVICE_TUNER1);

	if (!m_pParser)
	  {
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not load libcec.so. Bailing!");
	    return false;
	  }

	LoggerWrapper::GetInstance()->Write(LV_WARNING,"libCEC initialized - Version %s",m_pParser->ToString((cec_server_version)g_config.serverVersion));

	m_sPort = TranslateSerialUSB(DATA_Get_COM_Port_on_PC());

	if (m_sPort.empty() || m_sPort == "0")
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"No COM Port specified. Trying port 'RPI'.  Please set a port in the Interfaces section of Web Admin, or re-detect device.");
		m_sPort = "RPI";
	}

	if (!m_pParser->Open(m_sPort.c_str()))
	  {
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not open serial port %s",m_sPort.c_str());
	    return false;
	  }

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
  if (IRBase::ProcessMessage(pMessage))
    {
      printf("Message Processed by IRBase");
      sCMD_Result = "OK";
    }
  
  sCMD_Result = "UNHANDLED CHILD";
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


