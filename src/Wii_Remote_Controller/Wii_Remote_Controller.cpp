/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#include "Wii_Remote_Controller.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include <wiiuse.h>

// Alarms
#define FIND_GAMEPADS 0 

void * StartInputThread(void * Arg)
{
  Wii_Remote_Controller *pWii_Remote_Controller = (Wii_Remote_Controller *) Arg;  // ahh the joy of pointers...
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
  pWii_Remote_Controller->Wii_Capture(pWii_Remote_Controller->m_DeviceID);
  return NULL;
}

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Wii_Remote_Controller::Wii_Remote_Controller(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Wii_Remote_Controller_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
Wii_Remote_Controller::Wii_Remote_Controller(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: Wii_Remote_Controller_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
}

//<-dceag-dest-b->
Wii_Remote_Controller::~Wii_Remote_Controller()
//<-dceag-dest-e->
{
	
}

//<-dceag-getconfig-b->
bool Wii_Remote_Controller::GetConfig()
{
	if( !Wii_Remote_Controller_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated


	if ( !m_Virtual_Device_Translator.GetConfig(m_pData) )
	  return false;

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated

	m_pAlarmManager = new AlarmManager();
	m_pAlarmManager->Start(2);

	IRBase::setCommandImpl(this);
	IRBase::setAllDevices(&(GetData()->m_AllDevices));
	IRReceiverBase::GetConfig(m_pData);

	DeviceData_Base *pDevice = m_pData->m_AllDevices.m_mapDeviceData_Base_FindFirstOfCategory(DEVICECATEGORY_Infrared_Plugins_CONST);

	if ( pDevice )
	  m_dwPK_Device_IRPlugin = pDevice->m_dwPK_Device;
	else
	  m_dwPK_Device_IRPlugin = 0;

	string sResult;
	DCE::CMD_Get_Sibling_Remotes CMD_Get_Sibling_Remotes(m_dwPK_Device,m_dwPK_Device_IRPlugin, DEVICECATEGORY_Wii_Remote_Controls_CONST, &sResult);
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
	
	// Create the input thread.
	if (pthread_create(&m_inputCaptureThread, NULL, StartInputThread, (void *) this))
	  {
	    // failed, bail.
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Failed to create Input thread.");
	    m_bQuit_set(true);
	    return false;
	    }

	return true;

}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Wii_Remote_Controller::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
Wii_Remote_Controller_Command *Create_Wii_Remote_Controller(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new Wii_Remote_Controller(pPrimaryDeviceCommand, pData, pEvent, pRouter);
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
void Wii_Remote_Controller::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void Wii_Remote_Controller::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN COMMAND";
}

//<-dceag-sample-b->
/*		**** SAMPLE ILLUSTRATING HOW TO USE THE BASE CLASSES ****

**** IF YOU DON'T WANT DCEGENERATOR TO KEEP PUTTING THIS AUTO-GENERATED SECTION ****
**** ADD AN ! AFTER THE BEGINNING OF THE AUTO-GENERATE TAG, LIKE //<=dceag-sample-b->! ****
Without the !, everything between <=dceag-sometag-b-> and <=dceag-sometag-e->
will be replaced by DCEGenerator each time it is run with the normal merge selection.
The above blocks are actually <- not <=.  We don't want a substitution here

void Wii_Remote_Controller::SomeFunction()
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

void USB_Game_Pad::AlarmCallback(int id, void *param)
{
  switch (id)
    {
    case FIND_WII_REMOTES:
      FindWiiRemotes();
      break;
    }
}

void USB_Game_Pad::CreateChildren()
{
  USB_Game_Pad_Command::CreateChildren(); // superclass
  Start(); // Call IRBase Start.
}

int USB_Game_Pad::Wii_Capture(int deviceID)
{
  
  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Starting Wii Remote Capture thread.");
  // Schedule an initial device probe.
  m_pAlarmManager->AddRelativeAlarm(1, this, FIND_WII_REMOTES, NULL);
  Sleep(2000);

  while (!m_bQuit_get())
    {
      if (m_bWii1Active)
	ProcessGamePad(m_iWii1fd);
      if (m_bWii2Active)
	ProcessGamePad(m_iWii2fd);
      if (m_bWii3Active)
	ProcessGamePad(m_iWii3fd);
      if (m_bWii4Active)
	ProcessGamePad(m_iWii4fd);
      
      usleep(10000);

    }
  // for now...
  return 0;
}

//<-dceag-c191-b->

	/** @brief COMMAND: #191 - Send Code */
	/** The I/R code -- usually in Pronto format */
		/** @param #9 Text */
			/** The I/R code -- usually in Pronto format */

void Wii_Remote_Controller::CMD_Send_Code(string sText,string &sCMD_Result,Message *pMessage)
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

void Wii_Remote_Controller::CMD_Learn_IR(int iPK_Device,string sOnOff,int iPK_Text,int iPK_Command,string &sCMD_Result,Message *pMessage)
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

void Wii_Remote_Controller::CMD_Set_Screen_Type(int iValue,string &sCMD_Result,Message *pMessage)
//<-dceag-c687-e->
{
	cout << "Need to implement command #687 - Set Screen Type" << endl;
	cout << "Parm #48 - Value=" << iValue << endl;
}


