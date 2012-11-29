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
#include "USB_Game_Pad.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "PlutoUtils/ProcessUtils.h"
#include "Message.h"
#include "DCERouter.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"

#include <sys/types.h>
#include <sys/stat.h>
#ifndef WIN32
#include <sys/fcntl.h>
#endif

#include <linux/input.h>
#include <linux/joystick.h>

#include <sstream>

// Alarms
#define FIND_GAMEPADS 0 

void * StartInputThread(void * Arg)
{
  USB_Game_Pad *pUSB_Game_Pad = (USB_Game_Pad *) Arg;  // ahh the joy of pointers...
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
  pUSB_Game_Pad->Gamepad_Capture(pUSB_Game_Pad->m_DeviceID);
  return NULL;
}

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
USB_Game_Pad::USB_Game_Pad(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: USB_Game_Pad_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
	, IRReceiverBase(this)
	, m_GamePadMutex("usb_game_pad")
	, m_iAVWPort(0)
{
  m_GamePadMutex.Init(NULL);
  m_pAlarmManager=NULL;
  m_inputCaptureThread = 0;
  m_DeviceID = 0;
  m_bJoy1Active = m_bJoy2Active = m_bJoy3Active = m_bJoy4Active = false;
  m_iJoy1fd = m_iJoy2fd = m_iJoy3fd = m_iJoy4fd = -1;
  m_iErrors = 0;
}

//<-dceag-const2-b->!

void USB_Game_Pad::PrepareToDelete()
{
  Command_Impl::PrepareToDelete();
  delete m_pAlarmManager;
  m_pAlarmManager = NULL;
}

//<-dceag-dest-b->
USB_Game_Pad::~USB_Game_Pad()
//<-dceag-dest-e->
{
  
}

//<-dceag-getconfig-b->
bool USB_Game_Pad::GetConfig()
{
	if( !USB_Game_Pad_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	if ( m_dwPK_Device != DEVICEID_MESSAGESEND && !m_bLocalMode )
	  {
	    if ( !m_Virtual_Device_Translator.GetConfig(m_pData) )
	      return false;
	    
	    // Put your code here to initialize the data in this class
	    // The configuration parameters DATA_ are now populated
	    	    
	    IRBase::setCommandImpl(this);
	    IRBase::setAllDevices(&(GetData()->m_AllDevices));
	    IRReceiverBase::GetConfig(m_pData);
	  }

	m_pAlarmManager = new AlarmManager();
	m_pAlarmManager->Start(2);

	if ( !m_bLocalMode )
	  {
	    DeviceData_Base *pDevice = m_pData->m_AllDevices.m_mapDeviceData_Base_FindFirstOfCategory(DEVICECATEGORY_Infrared_Plugins_CONST);
	    
	    if ( pDevice )
	      m_dwPK_Device_IRPlugin = pDevice->m_dwPK_Device;
	    else
	      m_dwPK_Device_IRPlugin = 0;
	    
	    string sResult;
	    DCE::CMD_Get_Sibling_Remotes CMD_Get_Sibling_Remotes(m_dwPK_Device,m_dwPK_Device_IRPlugin, DEVICECATEGORY_USB_Game_Pad_Remote_Controls_CONST, &sResult);
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
	    // Local mode, hard code values to lookup.
	    // Button 1 = OK
	    // Buttons 2-10 = Mode buttons to blindly set video modes.
	    m_mapCodesToButtons["USB-GAMEPAD-UP"] = make_pair<string,int>("up",0);
	    m_mapCodesToButtons["USB-GAMEPAD-DOWN"] = make_pair<string,int>("down",0);
	    m_mapCodesToButtons["USB-GAMEPAD-LEFT"] = make_pair<string,int>("left",0);
	    m_mapCodesToButtons["USB-GAMEPAD-RIGHT"] = make_pair<string,int>("right",0);
	    m_mapCodesToButtons["USB-GAMEPAD-B1"] = make_pair<string,int>("ok",0);
	    m_mapCodesToButtons["USB-GAMEPAD-B2"] = make_pair<string,int>("1",0);
	    m_mapCodesToButtons["USB-GAMEPAD-B3"] = make_pair<string,int>("q",0);
	    m_mapCodesToButtons["USB-GAMEPAD-B4"] = make_pair<string,int>("a",0);
	    m_mapCodesToButtons["USB-GAMEPAD-B5"] = make_pair<string,int>("2",0);
	    m_mapCodesToButtons["USB-GAMEPAD-B6"] = make_pair<string,int>("w",0);
	    m_mapCodesToButtons["USB-GAMEPAD-B7"] = make_pair<string,int>("3",0);
	    m_mapCodesToButtons["USB-GAMEPAD-B8"] = make_pair<string,int>("4",0);
	    m_mapCodesToButtons["USB-GAMEPAD-B9"] = make_pair<string,int>("-",0);
	    m_mapCodesToButtons["USB-GAMEPAD-B10"] = make_pair<string,int>("+",0);
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
bool USB_Game_Pad::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->!

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void USB_Game_Pad::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void USB_Game_Pad::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
  sCMD_Result = "UNKNOWN DEVICE";  // Used for inheritance foo.
}

//<-dceag-sample-b->!

/*

	COMMANDS TO IMPLEMENT

*/

void USB_Game_Pad::AlarmCallback(int id, void *param)
{
  switch (id)
    {
    case FIND_GAMEPADS:
      FindGamePads();
      break;
    }
}

void USB_Game_Pad::SendIR(string Port, string sIRCode, int iRepeat)
{
  // not used, but must be implemented.
}

void USB_Game_Pad::FindGamePads()
{

  struct stat buf;

  // Joystick 1 ////////////////////////////////////////////////////

  if (stat("/dev/input/js0", &buf) != -1 && S_ISCHR(buf.st_mode))
    {
      // Joystick inode exists, let's see if we need to open it...
      if (m_iJoy1fd < 0)
	{
	  // not opened. open the joystick device.
	  m_iJoy1fd = open("/dev/input/js0",O_RDONLY | O_NONBLOCK);
	  if (m_iJoy1fd < 0)
	    {
	      // failed to open
	      m_bJoy1Active = false;
	    }
	  else
	    {
	      // success.
	      m_bJoy1Active = true;
	    }
	}
    }
  else
    {
      // Joystick inode does not exist, or has been removed. 
      if (m_iJoy1fd < 0)
	{
	  // the file descriptor is not opened. don't do anything.
	}
      else
	{
	  // close the file descriptor. 
	  close(m_iJoy1fd);
	  m_iJoy1fd = -1; // explicit reset so that the above logic will work 100%.
	}
      m_bJoy1Active = false;
    }

  // Joystick 2 ///////////////////////////////////////////////////

  if (stat("/dev/input/js1", &buf) != -1 && S_ISCHR(buf.st_mode))
    {
      // Joystick inode exists, let's see if we need to open it...
      if (m_iJoy2fd < 0)
	{
	  // not opened. open the joystick device.
	  m_iJoy2fd = open("/dev/input/js1",O_RDONLY | O_NONBLOCK);
	  if (m_iJoy2fd < 0)
	    {
	      // failed to open
	      m_bJoy2Active = false;
	    }
	  else
	    {
	      // success.
	      m_bJoy2Active = true;
	    }
	}
    }
  else
    {
      // Joystick inode does not exist, or has been removed. 
      if (m_iJoy2fd < 0)
	{
	  // the file descriptor is not opened. don't do anything.
	}
      else
	{
	  // close the file descriptor. 
	  close(m_iJoy2fd);
	  m_iJoy2fd = -1; // explicit reset so that the above logic will work 100%.
	}
      m_bJoy2Active = false;
    }

  // Joystick 3 ////////////////////////////////////////////////////

  if (stat("/dev/input/js2", &buf) != -1 && S_ISCHR(buf.st_mode))
    {
      // Joystick inode exists, let's see if we need to open it...
      if (m_iJoy3fd < 0)
	{
	  // not opened. open the joystick device.
	  m_iJoy1fd = open("/dev/input/js2",O_RDONLY | O_NONBLOCK);
	  if (m_iJoy3fd < 0)
	    {
	      // failed to open
	      m_bJoy3Active = false;
	    }
	  else
	    {
	      // success.
	      m_bJoy3Active = true;
	    }
	}
    }
  else
    {
      // Joystick inode does not exist, or has been removed. 
      if (m_iJoy3fd < 0)
	{
	  // the file descriptor is not opened. don't do anything.
	}
      else
	{
	  // close the file descriptor. 
	  close(m_iJoy3fd);
	  m_iJoy3fd = -1; // explicit reset so that the above logic will work 100%.
	}
      m_bJoy3Active = false;
    }

  // Joystick 4 //////////////////////////////////////////////

  if (stat("/dev/input/js3", &buf) != -1 && S_ISCHR(buf.st_mode))
    {
      // Joystick inode exists, let's see if we need to open it...
      if (m_iJoy4fd < 0)
	{
	  // not opened. open the joystick device.
	  m_iJoy1fd = open("/dev/input/js3",O_RDONLY | O_NONBLOCK);
	  if (m_iJoy4fd < 0)
	    {
	      // failed to open
	      m_bJoy4Active = false;
	    }
	  else
	    {
	      // success.
	      m_bJoy4Active = true;
	    }
	}
    }
  else
    {
      // Joystick inode does not exist, or has been removed. 
      if (m_iJoy4fd < 0)
	{
	  // the file descriptor is not opened. don't do anything.
	}
      else
	{
	  // close the file descriptor. 
	  close(m_iJoy4fd);
	  m_iJoy4fd = -1; // explicit reset so that the above logic will work 100%.
	}
      m_bJoy4Active = false;
    }
  
  // Reschedule a check in 3 seconds.
  m_pAlarmManager->CancelAlarmByType(FIND_GAMEPADS);
  m_pAlarmManager->AddRelativeAlarm(3, this, FIND_GAMEPADS, NULL);
}

void USB_Game_Pad::ProcessGamePad(int fd)
{
  if (m_cCurrentScreen != 'Z' || m_iErrors > 10)         // If the current screen is a Game FS, simply ignore.
    {
      unsigned char axes = 2;
      unsigned char buttons = 2;
      uint16_t btnmap[KEY_MAX - BTN_MISC + 1];
      uint8_t axmap[ABS_MAX + 1];
      struct ::js_event js; // one event packet.
      
      timespec ts_now;
      //  timespec ts_diff;
      
      ioctl(fd, JSIOCGAXES, &axes);
      ioctl(fd, JSIOCGBUTTONS, &buttons);
      ioctl(fd, JSIOCGAXMAP, axmap);
      ioctl(fd, JSIOCGBTNMAP, btnmap);
      
      while (read(fd, &js, sizeof(struct ::js_event)) == sizeof(struct ::js_event))
	{
	  // Parse the joystick event.
	  gettimeofday(&ts_now,NULL);
	  
	  switch (js.type)
	    {
	    case 1:  // buttons
	      if (js.value == 0)
		{
		  // button up
		  string sButtonNum = StringUtils::itos(js.number+1);
		  string sRet = "USB-GAMEPAD-B"+sButtonNum;
		  map <string,pair<string,int> >::iterator it=m_mapCodesToButtons.find(sRet);
		  if ( it==m_mapCodesToButtons.end() )
		    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Can't find a mapping for button %s",sRet.c_str());
		  else
		    {
		      // Send it off to IRRecieverBase
		      if (m_cCurrentScreen == 'G')
			{
				if (sRet == "USB-GAMEPAD-B13")
					ReceivedCode(it->second.second,it->second.first.c_str());
			}
		      else
			{
			
		      	     ReceivedCode(it->second.second,it->second.first.c_str());
			}
		      
		      if (m_dwPK_Device==DEVICEID_MESSAGESEND)
			{
			  ForceKeystroke(it->second.first, m_sAVWHost, m_iAVWPort);
			  return;
			}
		    }
		}
	      break;
	    case 2:  // axes
	      if (js.number % 2 == 0)
		{
		  // Horizontal axes.
		  if (js.value < -16384)
		    {
		      // left
		      string sRet = "USB-GAMEPAD-LEFT";
		      map <string,pair<string,int> >::iterator it=m_mapCodesToButtons.find(sRet);
		      if ( it==m_mapCodesToButtons.end() )
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Can't find a mapping for button %s",sRet.c_str());
		      else
			{
			  // Send it off to IRRecieverBase
			  if (m_cCurrentScreen != 'G')
			    {
			      ReceivedCode(it->second.second,it->second.first.c_str());
			      if (m_dwPK_Device==DEVICEID_MESSAGESEND)
				{
				  ForceKeystroke(it->second.first, m_sAVWHost, m_iAVWPort);
				  return;
				}
			    }
			}
		    }
		  if (js.value > 16384)
		    {
		      // right
		      string sRet = "USB-GAMEPAD-RIGHT";
		      map <string,pair<string,int> >::iterator it=m_mapCodesToButtons.find(sRet);
		      if ( it==m_mapCodesToButtons.end() )
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Can't find a mapping for button %s",sRet.c_str());
		      else
			{
			  // Send it off to IRRecieverBase
			  if (m_cCurrentScreen != 'G')
			    {
			      ReceivedCode(it->second.second,it->second.first.c_str());
			      if (m_dwPK_Device==DEVICEID_MESSAGESEND)
				{
				  ForceKeystroke(it->second.first, m_sAVWHost, m_iAVWPort);
				  return;
				}
			    }
			}
		    }
		}
	      else 
		{
		  // Vertical axes.
		  if (js.value < -16384)
		    {
		      // up
		      string sRet = "USB-GAMEPAD-UP";
		      map <string,pair<string,int> >::iterator it=m_mapCodesToButtons.find(sRet);
		      if ( it==m_mapCodesToButtons.end() )
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Can't find a mapping for button %s",sRet.c_str());
		      else
			{
			  // Send it off to IRRecieverBase
			  if (m_cCurrentScreen != 'G')
			    {
			      ReceivedCode(it->second.second,it->second.first.c_str());
			      if (m_dwPK_Device==DEVICEID_MESSAGESEND)
				{
				  ForceKeystroke(it->second.first, m_sAVWHost, m_iAVWPort);
				  return;
				}
			    }
			}
		    }
		  if (js.value > 16384)
		    {
		      // down
		      string sRet = "USB-GAMEPAD-DOWN";
		      map <string,pair<string,int> >::iterator it=m_mapCodesToButtons.find(sRet);
		      if ( it==m_mapCodesToButtons.end() )
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Can't find a mapping for button %s",sRet.c_str());
		      else
			{
			  // Send it off to IRRecieverBase
			  if (m_cCurrentScreen != 'G')
			    {
			      ReceivedCode(it->second.second,it->second.first.c_str());
			      if (m_dwPK_Device==DEVICEID_MESSAGESEND)
				{
				  ForceKeystroke(it->second.first, m_sAVWHost, m_iAVWPort);
				  return;
				}
			    }
			}
		    }
		}
	      break;
	    }
	  
	}
      
      if (errno != EAGAIN)
	{
	  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"ProcessGamePad(%d) Error reading event packet.",fd);
	  m_iErrors++;
	  return;
	} 
    }
    else
    {
        // Error Happened, Close the joystick, and let FindGamePads reopen it. 
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"ProcessGamePad(%d) closing this file descriptor and re-opening.",fd);
        m_iErrors = 0;
        close(fd);
    }
}

int USB_Game_Pad::Gamepad_Capture(int deviceID)
{
  
  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Starting Gamepad Capture thread.");
  // Schedule an initial device probe.
  m_pAlarmManager->AddRelativeAlarm(1, this, FIND_GAMEPADS, NULL);
  Sleep(2000);

  while (!m_bQuit_get())
    {
      if (m_bJoy1Active)
	ProcessGamePad(m_iJoy1fd);
      if (m_bJoy2Active)
	ProcessGamePad(m_iJoy2fd);
      if (m_bJoy3Active)
	ProcessGamePad(m_iJoy3fd);
      if (m_bJoy4Active)
	ProcessGamePad(m_iJoy4fd);
      
      usleep(10000);

    }
  // for now...
  return 0;
}

void USB_Game_Pad::CreateChildren()
{
  USB_Game_Pad_Command::CreateChildren(); // superclass
  Start(); // Call IRBase Start.
}

//<-dceag-c191-b->

	/** @brief COMMAND: #191 - Send Code */
	/** The I/R code -- usually in Pronto format */
		/** @param #9 Text */
			/** The I/R code -- usually in Pronto format */

void USB_Game_Pad::CMD_Send_Code(string sText,string &sCMD_Result,Message *pMessage)
//<-dceag-c191-e->
{
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

void USB_Game_Pad::CMD_Learn_IR(int iPK_Device,string sOnOff,int iPK_Text,int iPK_Command,string &sCMD_Result,Message *pMessage)
//<-dceag-c245-e->
{
}

//<-dceag-c687-b->

	/** @brief COMMAND: #687 - Set Screen Type */
	/** Sent by Orbiter when the screen changes to tells the i/r receiver what type of screen is displayed so it can adjust mappings if necessary. */
		/** @param #48 Value */
			/** a character: M=Main Menu, m=other menu, R=Pluto Remote, r=Non-pluto remote, N=navigable OSD on media dev, f=full screen media app, F=File Listing, c=computing list, C=Computing full screen */

void USB_Game_Pad::CMD_Set_Screen_Type(int iValue,string &sCMD_Result,Message *pMessage)
//<-dceag-c687-e->
{
  // defined in IRReceiverBase
  m_cCurrentScreen=(char) iValue;
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"Screen type now %c",m_cCurrentScreen);
}
