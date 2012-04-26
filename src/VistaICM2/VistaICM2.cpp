/*

	Written for LinuxMCE. (c) LinuxMCE Foundation.
	Licensed under the GNU GPL v3, see COPYING for more details.

Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
	Adam Shortland <los93sol@los93sol.com>

*/
//<-dceag-d-b->
#include "VistaICM2.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include <pthread.h>

/* For the web command socket */
#include <curl/curl.h>
#include <curl/easy.h>

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>

/* For the UDP socket */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h> /* memset() */
#include <sys/time.h> /* select() */
#include <stdlib.h>

VistaICM2 *g_pVistaICM2 = NULL;	// Needed by our thread to get access to main thread's state.

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
VistaICM2::VistaICM2(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: VistaICM2_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
  m_ccpThread = 0;
  m_bRunning = false; 
  g_pVistaICM2 = this;
}

/* //<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
VistaICM2::VistaICM2(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: VistaICM2_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
} */

//<-dceag-dest-b->
VistaICM2::~VistaICM2()
//<-dceag-dest-e->
{

  m_bRunning = false;	// We're no longer running, the thread will see this, and stop.

  // Wait for the UDP comms thread to clean itself up.
  if (m_ccpThread != 0)
    pthread_join (m_ccpThread, NULL);

}

/********************************/
/** CCP Processing Thread here **/
/********************************/

void *
ccpThread (void *param)
{

      int sd, rc;
      struct sockaddr_in cliAddr, remoteServAddr;
      struct hostent *h;
      int optval = 1;
      unsigned int optlen = sizeof (optval);
      int bufsize = 1024;	/* a 1K buffer */
      ssize_t buflength;	/* Actual Buffer Size */
      char *buffer;

      buffer = (char *) malloc (bufsize);

      /* get server IP address (no check if input is IP address or DNS name */
      h = gethostbyname ("192.168.80.255");

      inet_ntoa (*(struct in_addr *) h->h_addr_list[0]);

      remoteServAddr.sin_family = h->h_addrtype;
      memcpy ((char *) &remoteServAddr.sin_addr.s_addr,
	      h->h_addr_list[0], h->h_length);
      remoteServAddr.sin_port = htons (3947);

      /* socket creation */
      sd = socket (AF_INET, SOCK_DGRAM, 0);
      setsockopt (sd, SOL_SOCKET, SO_BROADCAST, &optval, optlen);	/* Required for our purposes. */

      if (sd < 0)
	{
	  printf ("cannot open socket");
	  pthread_exit (NULL);
	}

      /* bind any port */
      cliAddr.sin_family = AF_INET;
      cliAddr.sin_addr.s_addr = htonl (INADDR_ANY);
      cliAddr.sin_port = htons (3947);

      rc = bind (sd, (struct sockaddr *) &cliAddr, sizeof (cliAddr));
      if (rc < 0)
	{
	  printf ("cannot bind port\n");
	  pthread_exit (NULL);
	}

      while (g_pVistaICM2->m_bRunning == true)
	{

	cout << "Top of Runloop, waiting on data." << endl;

	cout << "m_bRunning is: " << g_pVistaICM2->m_bRunning << endl;

	  buflength = recv (sd, buffer, bufsize, 0);

	cout << "Recieved something." << endl;

	  // Now parse it, and deal.
	  if ((buffer[0] == 0x02) && (buffer[1] == 0x04))	// Variable Parse
	  {
		std::string sBuffer(buffer,buflength);
		cout << "Buffer Size: " << buflength << endl;
		std::string::iterator it;
		it = sBuffer.begin()+12;				// Bypass the header, get to the meat.
		std::string sBufferData(it,sBuffer.end());
		cout << "Full packet: " << sBufferData << endl;
		vector<string> vectBufferData;
		StringUtils::Tokenize(sBufferData,"=",vectBufferData);

		if (vectBufferData[0] == "ArmStatus")
		{
			int iArmValue = atoi(vectBufferData[1].c_str());
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Calling doArmEvent with mode %d",iArmValue);
			switch (iArmValue)
			{
				case 0: 
					g_pVistaICM2->doArmEvent(0);
					break;
				case 1:
					g_pVistaICM2->doArmEvent(1);
					break;
				case 2:
					g_pVistaICM2->doArmEvent(2);
					break;
			}
		}
	else	if (vectBufferData[0] == "AlarmEvent")
		{
			if (!g_pVistaICM2->m_bAlarmActive)
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Alarm Event Detected on %s.",vectBufferData[1].c_str());
				g_pVistaICM2->doAlarmEvent(vectBufferData[1]);
			}
		}
	else	if (vectBufferData[0] == "FireEvent")
		{
			if (!g_pVistaICM2->m_bAlarmActive)
			{
				g_pVistaICM2->doFireEvent(vectBufferData[1]);
			}
		}
	else	if (vectBufferData[0] == "display")
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Display Now Reads: %s",vectBufferData[1].c_str());
			// DCE::CMD_Display_Alert CMD_Display_Alert(m_dwPK_Device, DEVICETEMPLATE_VirtDev_All_Orbiters_CONST, 
			//						0, "VistaICM Display Reads: "+vectBufferData[1], 3, 
			//						"VistaICM2");
			// SendCommand(CMD_Display_Alert);
		}
	else	if (vectBufferData[0] == "ID")
		{
		}
	else	if (vectBufferData[0] == "Ready")
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Panel Ready, setting Alarm status to false.");
			g_pVistaICM2->m_bAlarmActive = false;
		}
	else	if (vectBufferData[0].find("FAULT") != string::npos)
		{
			if (!g_pVistaICM2->m_bAlarmActive)
			{
			// First, strip off everything but the zone # from the variable.
			string sZS = vectBufferData[0].substr(6,vectBufferData[0].length());
			int iZS;
			istringstream tmpStream;
			tmpStream.str(sZS);
			tmpStream >> iZS;
			int iPK_Device = g_pVistaICM2->getDeviceForZone(iZS);
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Zone to trigger from display packet = %d",iZS);

			string sSensorStatus = vectBufferData[1];

			// and send the Tripped event
			g_pVistaICM2->sendTripped(iPK_Device, sSensorStatus);
			}
		}
	else	if (vectBufferData[0].find("ZS") != string::npos )
		{
			// First, strip off everything but the zone # from the variable.
			string sZS = vectBufferData[0].substr(3,vectBufferData[0].length());
			int iZS;
			istringstream tmpStream;
			tmpStream.str(sZS);
			tmpStream >> iZS;
			int iPK_Device = g_pVistaICM2->getDeviceForZone(iZS);
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Zone to trigger = %d",iZS);

			string sSensorStatus = vectBufferData[1];

			// and send the Tripped event
			g_pVistaICM2->sendTripped(iPK_Device, sSensorStatus);

		}
	else    {
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Unhandled Variable of %s = %s",vectBufferData[0].c_str(),vectBufferData[1].c_str());
		}
		 
	  }

	  if ((buffer[0] == 0x04) && (buffer[1] == 0x01))	// Trigger
	  {
		std::string sBuffer(buffer,buflength);
		std::string::iterator it;
		it = sBuffer.begin()+12;
		std::string sBufferData(it,sBuffer.end());
		cout << "whole Trigger Packet: " << sBuffer << endl;
		vector<string> vectBufferData;
		StringUtils::Tokenize(sBufferData,"_",vectBufferData);
		vector<string> vectTriggerData;
		StringUtils::Tokenize(vectBufferData[2],":",vectTriggerData);
		
		if (vectTriggerData[1] == "Alarm")
		{
		}
	else	if (vectTriggerData[1] == "Fire")
		{
		}
	else	if (vectTriggerData[1] == "Low Battery")
		{
		}
	else	if (vectTriggerData[1] == "Power Failure")
		{
		}
	else	if (vectTriggerData[1] == "Power Returned")
		{
		}
	else	if (vectTriggerData[1] == "Disarmed")
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Trigger: Disarmed");
			g_pVistaICM2->doArmEvent(0);
		}
	else	if (vectTriggerData[1] == "Armed Stay") 
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Trigger: Armed Stay");
			g_pVistaICM2->doArmEvent(1);
		}
	else	if (vectTriggerData[1] == "Armed Away")
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Trigger Armed Away");
			g_pVistaICM2->doArmEvent(2);
		}
	else	{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Unhandled Trigger Packet: %s = %s",vectTriggerData[0].c_str(),vectTriggerData[1].c_str());
		}
	  }

	cout << "Bottom of Runloop" << endl;
	}

	cout << "About to free buffer" << endl;
      free (buffer);
	cout << "Buffer Freed." << endl;

  cout << "About to exit Thread." << endl;
  pthread_exit (NULL);

}


//<-dceag-getconfig-b->
bool VistaICM2::GetConfig()
{
	if( !VistaICM2_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

  // Put your code here to initialize the data in this class
  // The configuration parameters DATA_ are now populated

  // Set up the various class vars.
  m_sURL = GetIpAddress();

	DCE::CMD_Get_Device_Data CMD_Get_Device_Data(
		m_dwPK_Device, 4,
		13, DEVICEDATA_Configuration_CONST, true,
		&m_sCurrentHouseMode
	);

	SendCommand(CMD_Get_Device_Data);

	m_viHouseModes = parseHouseModes();
	m_sZones = DATA_Get_Zones();
	m_miZones = parseZones();
	m_sHouseCode = DATA_Get_Password();

  m_bRunning = true;
  // Start the CCP monitoring Thread
  pthread_create (&m_ccpThread, NULL, ccpThread, NULL);

  return true;
}


int VistaICM2::getDeviceForZone(int iZS)
{
	string sPort;			// The port devicedata
	int iPort;			// Port Device data converted to int.
	Command_Impl *nomination = NULL;
	Command_Impl *result;
	Command_Impl *pChildDeviceCommand;
	MapCommand_Impl::iterator child_iter;

	nomination=NULL;
	result=NULL;
	iPort = 0;
	int retValue = 0;

	for (child_iter=m_mapCommandImpl_Children.begin(); child_iter!=m_mapCommandImpl_Children.end(); ++child_iter)
	{
		pChildDeviceCommand = (*child_iter).second;

		string sPort = pChildDeviceCommand->m_pData->m_mapParameters[DEVICEDATA_PortChannel_Number_CONST];
		istringstream tmpStream;
		tmpStream.str(sPort);
		tmpStream >> iPort;
		if (iPort == iZS)
			retValue = pChildDeviceCommand->m_pData->m_dwPK_Device;
	}

	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Device for Port %d is %d",iZS,iPort);
	return retValue;
	
}

void VistaICM2::sendTripped(int iPK_Device, string sSensorStatus) 
{

	Message *pMessage = new Message(iPK_Device, 0, PRIORITY_NORMAL, MESSAGETYPE_EVENT, EVENT_Sensor_Tripped_CONST,
		1, EVENTPARAMETER_Tripped_CONST, sSensorStatus.c_str());
	QueueMessageToRouter(pMessage);

	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Sending Sensor Tripped for %d = %s",iPK_Device,sSensorStatus.c_str());

}

vector<int> VistaICM2::parseHouseModes()
{
	vector<string> vectBufferData;
	vector<int> houseModes;
	StringUtils::Tokenize(m_sCurrentHouseMode,",",vectBufferData);
	
	// At this point, it is a vectorized set of strings, we loop through, two at a time
	// and populate a new vector where the index is the device group, and the
	// value is a PK_HouseMode. 

	vector<string>::iterator it;
	it = vectBufferData.begin();
	int i = 0;
	while (it != vectBufferData.end()) 
	{
		int iCurrHouseMode;
		it++; 
		istringstream tmpStream;
		tmpStream.str(*it);
		tmpStream >> iCurrHouseMode;
		houseModes.push_back(iCurrHouseMode);
		it++;
	}

#ifdef DEBUG
	
	vector<int>::iterator viewIt;
	viewIt = houseModes.begin();
	i = 0;
	while (viewIt != houseModes.end()) 
	{
		int foo = *viewIt;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PK_DeviceGroup %d has mode %d",i,foo);
		viewIt++;
		i++;
	}

#endif

	return houseModes;

}

map<int,int> VistaICM2::parseZones()
{

	vector<string> vectBufferData;
	map<int,int> m_Zones;
	StringUtils::Tokenize(m_sZones,",",vectBufferData);

	vector<string>::iterator it;
	it = vectBufferData.begin();
	while (it != vectBufferData.end())
	{
		int iCurrZone, iCurrDevice;
		istringstream tmpStream;
		tmpStream.str(*it);
		tmpStream >> iCurrZone;
		it++;
		tmpStream.str(*it);
		tmpStream >> iCurrDevice;
		it++;
		m_Zones[iCurrZone] = iCurrDevice;
	}

	return m_Zones;

}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool VistaICM2::Register()
//<-dceag-reg-e->
{
  return Connect (PK_DeviceTemplate_get ());
}

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
void VistaICM2::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void VistaICM2::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
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

void VistaICM2::SomeFunction()
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

/**
 * Private functions.
 */

/* Send a single command to the ICM */
bool
VistaICM2::sendCommand (string s_Command)
{

  string sResponse;

  CURLcode res;
  struct curl_slist *headerlist = NULL;
  string sParams;
  string sFullURL;

  sFullURL = m_sURL + "/cmd?cmd=(0.1.1)" + s_Command;

  curl_global_init (CURL_GLOBAL_ALL);
  CURL *curl = curl_easy_init ();

  if (curl)
    {
      curl_easy_setopt (curl, CURLOPT_URL, sFullURL.c_str ());
      curl_easy_setopt (curl, CURLOPT_FOLLOWLOCATION, 1);	// allow redirects 
      curl_easy_setopt (curl, CURLOPT_VERBOSE, 1);
      curl_easy_setopt (curl, CURLOPT_PORT, 80);

      curl_easy_setopt (curl, CURLOPT_HTTPHEADER, headerlist);

      res = curl_easy_perform (curl);

      //always cleanup 
      curl_easy_cleanup (curl);
      curl_slist_free_all (headerlist);
    }

  return res == CURLE_OK ? true : false;

}

void
VistaICM2::sendDigits (string s_Digits)
{
  string::iterator it;

  it = s_Digits.begin ();

  while (it != s_Digits.end ())
    {
      string sIndvCmd (it, it + 1);
      if (!sendCommand (sIndvCmd))
	{
	  LoggerWrapper::GetInstance ()->Write (LV_CRITICAL,
						"Unable to write individual command to Panel: %s",
						sIndvCmd.c_str ());
	}
      else
	{
	  LoggerWrapper::GetInstance ()->Write (LV_CRITICAL,
						"Sent %s to panel.",
						sIndvCmd.c_str ());
	}
      it++;
    }
}

void
VistaICM2::doArmEvent (int iValue)
{
	string sHouseMode;
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "doArmEvent: Set to %d",iValue);
	switch (iValue) 
	{
		case 0:
			sHouseMode = "1";	
			break;
		case 1:
			sHouseMode = "3";
			break;
		case 2:
			sHouseMode = "2";
			break;
	}
 	DCE::CMD_Set_House_Mode CMD_Set_House_Mode(m_dwPK_Device,13,sHouseMode,0,m_sHouseCode,0,"W");
	SendCommand(CMD_Set_House_Mode);
}

void 
VistaICM2::doAlarmEvent (string s_Value)
{
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "doAlarmEvent: %s",s_Value.c_str());
	m_bAlarmActive = true;
//	EVENT_Sensor_Tripped(0);
}

void
VistaICM2::doFireEvent (string s_Value)
{
	
}

void
VistaICM2::setHome (string s_Password)
{
  sendDigits (s_Password + "1");
  LoggerWrapper::GetInstance ()->Write (LV_STATUS, "House Mode set to Home.");
  m_bAlarmActive = false;
}

void
VistaICM2::setAway (string s_Password)
{
  sendDigits (s_Password + "2");
  LoggerWrapper::GetInstance ()->Write (LV_STATUS, "House Mode set to Away.");
  m_bAlarmActive = false;
}

void
VistaICM2::setStay (string s_Password)
{
  sendDigits (s_Password + "3");
  LoggerWrapper::GetInstance ()->Write (LV_STATUS, "House Mode set to Stay.");
  m_bAlarmActive = false;
}

void
VistaICM2::setInstant (string s_Password)
{
  sendDigits (s_Password + "7");
  LoggerWrapper::GetInstance ()->Write (LV_STATUS, "House Mode set to Instant.");
  m_bAlarmActive = false;
}

/*

	COMMANDS TO IMPLEMENT

*/

//<-dceag-c19-b->

	/** @brief COMMAND: #19 - Set House Mode */
	/** Set house mode */
		/** @param #5 Value To Assign */
			/** A value from the HouseMode table, or -1=toggle monitor mode */
		/** @param #17 PK_Users */
			/** The user setting the mode.  If this is 0, it will match any user who has permission to set the house mode. */
		/** @param #99 Password */
			/** The password or PIN of the user.  This can be plain text or md5. */
		/** @param #100 PK_DeviceGroup */
			/** DeviceGroups are treated as zones.  If this device group is specified, only the devices in these zones (groups) will be set. */
		/** @param #101 Handling Instructions */
			/** How to handle any sensors that we are trying to arm, but are blocked.  Valid choices are: R-Report, change to a screen on the orbiter reporting this and let the user decide, W-Wait, arm anyway, but wait for the sensors to clear and then arm them, B-Bypass */

void VistaICM2::CMD_Set_House_Mode(string sValue_To_Assign,int iPK_Users,string sPassword,int iPK_DeviceGroup,string sHandling_Instructions,string &sCMD_Result,Message *pMessage)
//<-dceag-c19-e->
{
  cout << "Need to implement command #19 - Set House Mode" << endl;
  cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
  cout << "Parm #17 - PK_Users=" << iPK_Users << endl;
  cout << "Parm #99 - Password=" << sPassword << endl;
  cout << "Parm #100 - PK_DeviceGroup=" << iPK_DeviceGroup << endl;
  cout << "Parm #101 - Handling_Instructions=" << sHandling_Instructions <<
    endl;


  int iValue;
  std::istringstream iss (sValue_To_Assign);
  if (!(iss >> iValue))
    {
      // Do not do anything, incorrect value passed.  
    }
  else
    {
      switch (iValue)
	{
	case 1:
	  if (m_viHouseModes[0] != 1)
	    setHome (sPassword);	// Unarmed @ home
	  break;
	case 2:
	  if (m_viHouseModes[0] != 2)
	    setAway (sPassword);	// Armed away
	  break;
	case 3:
	  if (m_viHouseModes[0] != 3)
	    setStay (sPassword);	// Armed @ Home
	  break;
	case 4:
	  if (m_viHouseModes[0] != 4)
	    setInstant (sPassword);	// Sleeping
	  break;
	case 5:
	  if (m_viHouseModes[0] != 5)
	    setHome (sPassword);	// Entertaining
	  break;
	case 6:
	  if (m_viHouseModes[0] != 6)
	    setAway (sPassword);	// Armed Extended Away
	  break;
	}

	m_viHouseModes[0] = iValue;

    }

}

//<-dceag-c756-b->

	/** @brief COMMAND: #756 - Report Child Devices */
	/** Report Child Devices */

void VistaICM2::CMD_Report_Child_Devices(string &sCMD_Result,Message *pMessage)
//<-dceag-c756-e->
{
  cout << "Need to implement command #756 - Report Child Devices" << endl;
}

//<-dceag-c759-b->

	/** @brief COMMAND: #759 - Get Sensors List */
	/** Get the list of sensors */
		/** @param #109 Data String */
			/** [internal id] \t [description] \t [room name] \t [device template] \t [floorplan id] \n */

void VistaICM2::CMD_Get_Sensors_List(string *sData_String,string &sCMD_Result,Message *pMessage)
//<-dceag-c759-e->
{
  cout << "Need to implement command #759 - Get Sensors List" << endl;
  cout << "Parm #109 - Data_String=" << sData_String << endl;
}
