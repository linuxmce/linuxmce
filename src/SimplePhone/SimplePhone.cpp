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
//<-dceag-d-b->
#include "SimplePhone.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_Variable.h"

#include "Gen_Devices/AllScreens.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/ProcessUtils.h"

#include "SIP_Thread.h"

#include "pluto_main/Define_Button.h"
#include "PlutoUtils/LinuxSoundcard.h"

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
SimplePhone::SimplePhone(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: SimplePhone_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
	m_SIP_Thread = (pthread_t)NULL;
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
SimplePhone::SimplePhone(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: SimplePhone_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
	m_SIP_Thread = (pthread_t)NULL;
}

//<-dceag-dest-b->
SimplePhone::~SimplePhone()
//<-dceag-dest-e->
{
	LS_bQuit = true;
	if (m_SIP_Thread != (pthread_t)NULL)
		pthread_join(m_SIP_Thread, NULL);
}

//<-dceag-getconfig-b->
bool SimplePhone::GetConfig()
{
	if( !SimplePhone_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

    // Put your code here to initialize the data in this class
    // The configuration parameters DATA_ are now populated
    
	string sServerIP = DATA_Get_Server_IP();
	string sSoundCard = DATA_Get_Sound_Card();
	string sAlsaDevice = "asym_analog";
	    
	SimplePhoneConf::Instance().Set_Server_IP(sServerIP.empty() ? "dcerouter" : sServerIP);

	LoggerWrapper::GetInstance()->Write(LV_WARNING, "Server IP: %s", SimplePhoneConf::Instance().Get_Server_IP().c_str());

	if (sSoundCard.empty() || sSoundCard == "0") // the 0 catches the weird edge case that somebody flips it back to Please Select... on the dropdown.
	  {
	    /* Sound card device data is empty - Get MD Audio Settings */
	    
	    string sAudioSettings = Get_MD_AudioSettings();
	    LoggerWrapper::GetInstance()->Write(LV_STATUS, "MD Audio Settings: %s", sAudioSettings.c_str());
	    // TODO: FIX ME with the changes to asound.conf by l3top
	    for (size_t i = 0; i < sAudioSettings.length(); i++)
	      {
		switch (sAudioSettings[i])
		  {
		  case 'C':
		  case 'O':
		    sAlsaDevice = "default"; // used to be "asym_spdif";
		  break;
		  case 'S':
		  case 'L':
		    sAlsaDevice = "default"; // used to be "asym_analog";
		  break;
		  case 'H':
		    sAlsaDevice = "default"; // used to be "asym_hdmi";
		    break;
		  default:
		    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Flag unprocessed: '%c'", sAudioSettings[i]);
		  }
	      }
	  }
	else
	  {
	    /* An explicit sound card is specified, use it. */
	    string sSoundCardNumber = TranslateSoundcard(sSoundCard);
	    m_sSoundCardNumber=sSoundCardNumber;
	    if (sSoundCardNumber.empty()) // Somehow the translate failed
	      {
		sAlsaDevice = "default"; // use the default.
	      }
	    else
	      {
		sAlsaDevice = "plughw:"+sSoundCardNumber;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Soundcard explicitly set by pulldown to plughw:%s",sSoundCardNumber.c_str());
	      }
	  }
	/* Read config file template, replace certain settings and write Linphone config file for SIP Thread */
	vector<string> vectLinphoneConfig;
	FileUtils::ReadFileIntoVector("/usr/pluto/templates/simplephone.conf", vectLinphoneConfig);
	for (size_t i = 0; i < vectLinphoneConfig.size(); i++)
	{
		const char * pcConfigLine = vectLinphoneConfig[i].c_str();
		if (strstr(pcConfigLine, "alsadev=") == pcConfigLine)
		{
			vectLinphoneConfig[i] = "alsadev=" + sAlsaDevice;
		}

                if (strstr(pcConfigLine, "playback_dev_id=") == pcConfigLine)
		{
			vectLinphoneConfig[i] = "playback_dev_id=ALSA:" + sAlsaDevice;
		}

		if (strstr(pcConfigLine, "ringer_dev_id=") == pcConfigLine)
		{
			vectLinphoneConfig[i] = "ringer_dev_id=ALSA:" + sAlsaDevice;
		}

		if (strstr(pcConfigLine, "capture_dev_id=") == pcConfigLine)
		{
			vectLinphoneConfig[i] = "capture_dev_id=ALSA:" + sAlsaDevice;
		}

	}

        if ( sAlsaDevice != "donttouch" ) {
            FileUtils::WriteVectorToFile("/etc/pluto/simplephone.conf", vectLinphoneConfig);
        }

	m_sExtension = m_pEvent->GetDeviceDataFromDatabase(m_dwPK_Device, DEVICEDATA_PhoneNumber_CONST);
	if (m_sExtension.length() == 0)
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "SimplePhone::GetConfig: Extension is empty. Attempting to sync with AMP");

		const char * const cmd[] = { "/usr/pluto/bin/LaunchRemoteCmd.sh", "dcerouter", "/usr/pluto/bin/db_phone_config.sh", NULL };
		string sOutput, sStdErr;
		ProcessUtils::GetCommandOutput(cmd[0], cmd, sOutput, sStdErr);

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "SimplePhone::GetConfig: Output of sync command:\n%s\nSimplePhone::GetConfig: End of output", sOutput.c_str());

		m_sExtension = m_pEvent->GetDeviceDataFromDatabase(m_dwPK_Device, DEVICEDATA_PhoneNumber_CONST);
		if (m_sExtension.length() == 0)
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "SimplePhone::GetConfig: Extension is empty after sync. This is wrong.");
			return false;
		}
	}
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "SimplePhone::GetConfig: Starting with extension '%s'", m_sExtension.c_str());

	m_sPassword = m_pEvent->GetDeviceDataFromDatabase(m_dwPK_Device, DEVICEDATA_Password_CONST);
	if (m_sPassword.length() == 0)
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "SimplePhone::GetConfig: Password is empty. Attempting to sync with AMP");

		const char * const cmd[] = { "/usr/pluto/bin/LaunchRemoteCmd.sh", "dcerouter", "/usr/pluto/bin/db_phone_config.sh", NULL };
		string sOutput, sStdErr;
		ProcessUtils::GetCommandOutput(cmd[0], cmd, sOutput, sStdErr);

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "SimplePhone::GetConfig: Output of sync command:\n%s\nSimplePhone::GetConfig: End of output", sOutput.c_str());

		m_sPassword = m_pEvent->GetDeviceDataFromDatabase(m_dwPK_Device, DEVICEDATA_Password_CONST);
		if (m_sPassword.length() == 0)
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "SimplePhone::GetConfig: Password is empty after sync. Will use extension as password. This is a security risk.");
			m_sPassword = m_sExtension;
		}
	}
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "SimplePhone::GetConfig: Received password for extension '%s'", m_sExtension.c_str());
	
	// set mic as default capture device, mute, increase capture volume level
	const char * cmd = "/usr/pluto/bin/SoundCards_SetupAudioCapture.sh";
	system(cmd);

    return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool SimplePhone::Register()
//<-dceag-reg-e->
{
    return Connect(PK_DeviceTemplate_get());
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
    cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You
    can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
SimplePhone_Command *Create_SimplePhone(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new SimplePhone(pPrimaryDeviceCommand, pData, pEvent, pRouter);
}
//<-dceag-createinst-e->

/*
    When you receive commands that are destined to one of your children,
    then if that child implements DCE then there will already be a separate class
    created for the child that will get the message.  If the child does not, then you will
    get all    commands for your children in ReceivedCommandForChild, where
    pDeviceData_Base is the child device.  If you handle the message, you
    should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void SimplePhone::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
    sCMD_Result = "UNHANDLED CHILD";
}

/*
    When you received a valid command, but it wasn't for one of your children,
    then ReceivedUnknownCommand gets called.  If you handle the message, you
    should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void SimplePhone::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
    sCMD_Result = "UNKNOWN DEVICE";
}

//<-dceag-sample-b->
/*		**** SAMPLE ILLUSTRATING HOW TO USE THE BASE CLASSES ****

**** IF YOU DON'T WANT DCEGENERATOR TO KEEP PUTTING THIS AUTO-GENERATED SECTION ****
**** ADD AN ! AFTER THE BEGINNING OF THE AUTO-GENERATE TAG, LIKE //<=dceag-sample-b->! ****
Without the !, everything between <=dceag-sometag-b-> and <=dceag-sometag-e->
will be replaced by DCEGenerator each time it is run with the normal merge selection.
The above blocks are actually <- not <=.  We don't want a substitution here

void SimplePhone::SomeFunction()
{
	// If this is going to be loaded into the router as a plug-in, you can implement: 	virtual bool Register();
	// to do all your registration, such as creating message interceptors

	// If you use an IDE with auto-complete, after you type DCE:: it should give you a list of all
	// commands and requests, including the parameters.  See "AllCommandsRequests.h"

	// Examples:
	
	// Send a specific the "CMD_Simulate_Mouse_Click" command, which takes an X and Y parameter.  We'll use 55,77 for X and Y.
	DCE::CMD_Simulate_Mouse_Click CMD_Simulate_Mouse_Click(m_dwPK_Device,OrbiterID,55,77);
	SendCommand(CMD_Simulate_Mouse_Click);

	// Send the message to orbiters 32898 and 27283 (ie a device list, hence the _DL)
	// And we want a response, which will be "OK" if the command was successfull
	string sResponse;
	DCE::CMD_Simulate_Mouse_Click_DL CMD_Simulate_Mouse_Click_DL(m_dwPK_Device,"32898,27283",55,77)
	SendCommand(CMD_Simulate_Mouse_Click_DL,&sResponse);

	// Send the message to all orbiters within the house, which is all devices with the category DEVICECATEGORY_Orbiter_CONST (see pluto_main/Define_DeviceCategory.h)
	// Note the _Cat for category
	DCE::CMD_Simulate_Mouse_Click_Cat CMD_Simulate_Mouse_Click_Cat(m_dwPK_Device,DEVICECATEGORY_Orbiter_CONST,true,BL_SameHouse,55,77)
    SendCommand(CMD_Simulate_Mouse_Click_Cat);

	// Send the message to all "DeviceTemplate_Orbiter_CONST" devices within the room (see pluto_main/Define_DeviceTemplate.h)
	// Note the _DT.
	DCE::CMD_Simulate_Mouse_Click_DT CMD_Simulate_Mouse_Click_DT(m_dwPK_Device,DeviceTemplate_Orbiter_CONST,true,BL_SameRoom,55,77);
	SendCommand(CMD_Simulate_Mouse_Click_DT);

	// This command has a normal string parameter, but also an int as an out parameter
	int iValue;
	DCE::CMD_Get_Signal_Strength CMD_Get_Signal_Strength(m_dwDeviceID, DestDevice, sMac_address,&iValue);
	// This send command will wait for the destination device to respond since there is
	// an out parameter
	SendCommand(CMD_Get_Signal_Strength);  

	// This time we don't care about the out parameter.  We just want the command to 
	// get through, and don't want to wait for the round trip.  The out parameter, iValue,
	// will not get set
	SendCommandNoResponse(CMD_Get_Signal_Strength);  

	// This command has an out parameter of a data block.  Any parameter that is a binary
	// data block is a pair of int and char *
	// We'll also want to see the response, so we'll pass a string for that too

	int iFileSize;
	char *pFileContents
	string sResponse;
	DCE::CMD_Request_File CMD_Request_File(m_dwDeviceID, DestDevice, "filename",&pFileContents,&iFileSize,&sResponse);
	SendCommand(CMD_Request_File);

	// If the device processed the command (in this case retrieved the file),
	// sResponse will be "OK", and iFileSize will be the size of the file
	// and pFileContents will be the file contents.  **NOTE**  We are responsible
	// free deleting pFileContents.


	// To access our data and events below, you can type this-> if your IDE supports auto complete to see all the data and events you can access

	// Get our IP address from our data
	string sIP = DATA_Get_IP_Address();

	// Set our data "Filename" to "myfile"
	DATA_Set_Filename("myfile");

	// Fire the "Finished with file" event, which takes no parameters
	EVENT_Finished_with_file();
	// Fire the "Touch or click" which takes an X and Y parameter
	EVENT_Touch_or_click(10,150);
}
*/
//<-dceag-sample-e->

/*

    COMMANDS TO IMPLEMENT

*/

//<-dceag-c334-b->

	/** @brief COMMAND: #334 - Phone_Initiate */
	/** Initiates a call */
		/** @param #2 PK_Device */
			/** The device to iniate the call from.  Used by Telecom plugin to forward to a sip phone.  Not needed when sending to a phone directly.  If this is not specified, telecom plugin assumes the correct option is the from device */
		/** @param #83 PhoneExtension */
			/** Extention to dial */

void SimplePhone::CMD_Phone_Initiate(int iPK_Device,string sPhoneExtension,string &sCMD_Result,Message *pMessage)
//<-dceag-c334-e->
{
    if(LS_ActiveCall())
    {
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Already have a call");
        sCMD_Result="ERROR";
        return;
    }
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Try to call %s", sPhoneExtension.c_str());
    LS_InitiateCall(sPhoneExtension.c_str());
    sCMD_Result="OK";
}

//<-dceag-c335-b->

	/** @brief COMMAND: #335 - Phone_Answer */
	/** Answer a call */

void SimplePhone::CMD_Phone_Answer(string &sCMD_Result,Message *pMessage)
//<-dceag-c335-e->
{
    if(LS_AcceptCall())
        sCMD_Result="OK";
    else
        sCMD_Result="ERROR";
}

//<-dceag-c336-b->

	/** @brief COMMAND: #336 - Phone_Drop */
	/** Drop a call */

void SimplePhone::CMD_Phone_Drop(string &sCMD_Result,Message *pMessage)
//<-dceag-c336-e->
{
    if (LS_DropCall())
        sCMD_Result="OK";
    else
        sCMD_Result="ERROR";
}

void SimplePhone::CreateChildren()
{
	if (pthread_create(&m_SIP_Thread, NULL, LS_Thread, (void *) this))
	{
		m_SIP_Thread = (pthread_t)NULL;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed to create Linphone SIP Thread");
		m_bQuit_set(true);
		exit(1);
	}
}
//<-dceag-c28-b->

	/** @brief COMMAND: #28 - Simulate Keypress */
	/** Send a DTMF code */
		/** @param #26 PK_Button */
			/** What key to simulate being pressed.  If 2 numbers are specified, separated by a comma, the second will be used if the Shift key is specified. */
		/** @param #41 StreamID */
			/** ID of stream to apply */
		/** @param #50 Name */
			/** The application to send the keypress to. If not specified, it goes to the DCE device. */

void SimplePhone::CMD_Simulate_Keypress(string sPK_Button,int iStreamID,string sName,string &sCMD_Result,Message *pMessage)
//<-dceag-c28-e->
{
	(void) sName; // prevent warning of unused variable

    unsigned char ch = 0;

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Received '%s'",sPK_Button.c_str());
    sCMD_Result="ERROR";
    if(LS_ActiveCall())
    {

	    if (!sPK_Button.empty()) {
	    	switch(atoi(sPK_Button.c_str()))
		    {
		    	case BUTTON_0_CONST:
				ch = '0';
				break;
			case BUTTON_1_CONST:
				ch = '1';
				break;
			case BUTTON_2_CONST:
				ch = '2';
				break;
			case BUTTON_3_CONST:
				ch = '3';
				break;
			case BUTTON_4_CONST:
				ch = '4';
				break;
			case BUTTON_5_CONST:
				ch = '5';
				break;
			case BUTTON_6_CONST:
				ch = '6';
				break;
			case BUTTON_7_CONST:
				ch = '7';
				break;
			case BUTTON_8_CONST:
				ch = '8';
				break;
			case BUTTON_9_CONST:
				ch = '9';
				break;
			case BUTTON_Asterisk_CONST:
				ch = '*';
				break;
			case BUTTON_Pound_CONST:
				ch = '#';
				break;
		    }
	    }

	    /*		if( !sPK_Button.empty() )
		{
			// only read the first character
			ch = sPK_Button[0];
		}
	    */
	

	// why the hell was it done this way?

        if( isdigit(ch) || (ch == '*') || (ch == '#'))
        {
            LoggerWrapper::GetInstance()->Write(LV_STATUS, "Will send '%d'(%c) as DTMF",ch,ch);
            LS_SentDTMF(ch);
            sCMD_Result="OK";
        }
        else
        {
            LoggerWrapper::GetInstance()->Write(LV_STATUS, "'%d'(%c) is invalid DTMF character",ch,ch);
        }
    }
    else
    {
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Looks like there is no call");
    }
    
}

string SimplePhone::Get_MD_AudioSettings()
{

	m_pUSBMicrophoneDevice = m_pData->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_Conference_Microphones_CONST);

	if (!m_pUSBMicrophoneDevice)
	{
		// M/D where we are attached to
		int PK_MD = m_pData->m_dwPK_Device_MD;

		return m_pData->m_pEvent_Impl->GetDeviceDataFromDatabase(PK_MD,DEVICEDATA_Audio_settings_CONST);
	} else
	{
		return "M"; // Mic found
	}

}

string SimplePhone::GetExtension()
{
	return m_sExtension;
}

string SimplePhone::GetPassword()
{
	return m_sPassword;
}
//<-dceag-c313-b->

	/** @brief COMMAND: #313 - Set Volume */
	/** Set volume of Target Audio card device. 0 to 100, with 0 meaning mute and * meaning whatever the previous value was. */
		/** @param #76 Level */
			/** A value between 0 and 100 where 0 is mute and 100 is full volume.  Numbers preceded with a - or + are relative.  +15 means up 15, -10 means down 10 */

void SimplePhone::CMD_Set_Volume(string sLevel,string &sCMD_Result,Message *pMessage)
//<-dceag-c313-e->
{
  // This can be expanded upon later, but for now, this will at least get things started!
  string sCmd;
  if (sLevel=="*")
    {
      sCmd = "amixer -c "+m_sSoundCardNumber+" set Mic 0 capture mute";
      system(sCmd.c_str());
    }
  else if (sLevel=="#")
    {
      sCmd = "amixer -c "+m_sSoundCardNumber+" set Mic "+StringUtils::itos(DATA_Get_Volume_Level())+" capture mute";
      system(sCmd.c_str());
    }
}
