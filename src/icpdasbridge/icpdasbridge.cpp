/*
     Copyright (C) 2013 LinuxMCE

     www.linuxmce.org


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#include "icpdasbridge.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"

//<-dceag-d-e->
#include <fcntl.h>
#include <signal.h>

//<-dceag-const-b->

void * StartEventThread(void * Arg)
{
	icpdasbridge * picpdasbridge = (icpdasbridge *) Arg;
	picpdasbridge->EventThread();
	return NULL;
}

// The primary constructor when the class is created as a stand-alone device
icpdasbridge::icpdasbridge(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: icpdasbridge_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
icpdasbridge::icpdasbridge(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: icpdasbridge_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
}

//<-dceag-dest-b->
icpdasbridge::~icpdasbridge()
//<-dceag-dest-e->
{
	signal(SIGSEGV,SIG_IGN);           //ignore SIGFAULT here
	m_bQuit_set(false);                    //force quit	
	Sleep(600);                        //wait a little
	
// 	PLUTO_SAFETY_LOCK(sl, icpdas_mutex);
	
//	if (m_EventThread != 0)
//	{
		pthread_cancel(m_EventThread);     //pthread_cancel is asynchron so call it first and continue cleanup		
		pthread_join(m_EventThread, NULL); //finish
//	}
}

//<-dceag-getconfig-b->
bool icpdasbridge::GetConfig()
{
	// Get ICPDAS IP address and port

	if( !icpdasbridge_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->
	
	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated
	LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Before opening socket to ICPDAS at IP: %s Port: %d ", DATA_Get_TCP_Address().c_str(), DATA_Get_TCP_Port() ); 

        LoggerWrapper::GetInstance()->Write(LV_STATUS,"ICPDAS populating clients from LinuxMCE database");
        populate_children();
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"ICPDAS populating done");
                        

	if (!Open_icpdas_Socket())
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Couldn't open socket connection to ICPDAS at IP: %s Port: %d ", DATA_Get_TCP_Address().c_str(), DATA_Get_TCP_Port() ); 
		exit(2);	
	}
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Socket connection opened to ICPDAS at IP : %s Port: %d ",DATA_Get_TCP_Address().c_str(), DATA_Get_TCP_Port());
		                
	return true;
}

void icpdasbridge::CreateChildren()
{
	string device_data;

	icpdasbridge_Command::CreateChildren();

// 	send_to_icpdas("getdevices");

/*	do
	{
		struct timeval tv;
		tv.tv_sec = 5;
		tv.tv_usec = 0;
		device_data = read_from_icpdas(&tv);
		if (device_data == "error") {
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed to get devices from icpdas");
			exit(2);
		}
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Reading data from icpdas");
		
	} while (device_data != "endlistdevices");
*/

	Sleep(1000);
//	is_open_for_learning = open_for_learning();

	if (pthread_create(&m_EventThread, NULL, StartEventThread, (void *) this))
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed to create Event Thread");
		m_bQuit_set(true);
		exit(1);
	}

/*	if (pthread_create(&m_SocketThread, NULL, StartSocketThread, (void *) this))
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Failed to create Socket Thread");
	}
*/
}

void icpdasbridge::EventThread()
{
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "EventThread started");
	
	while (! m_bQuit_get())
	{
		Sleep(50);
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"EventThread");
		if(read_from_icpdas() == "error")
		{
			close(icpdas_socket);
			Open_icpdas_Socket();
		}
	}
}

void icpdasbridge::icp2dce(std::string sPort, std::string sValue)
{

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "statechange from ICP: Port %s, new state %s", sPort.c_str(), sValue.c_str());

        MapCommand_Impl::iterator child_iter;
        Command_Impl *result;
        Command_Impl *pChildDeviceCommand;
                        
// find out what the device id in the LinuxMCE universe is, and send the new value to the router.

	for (child_iter=m_mapCommandImpl_Children.begin(); child_iter!=m_mapCommandImpl_Children.end(); ++child_iter)
	{
		pChildDeviceCommand = (*child_iter).second;

		if (pChildDeviceCommand->m_pData->WithinCategory(DEVICECATEGORY_Environment_CONST))
		{
			std::string this_pin;

//			if (pChildDeviceCommand->PK_DeviceTemplate_get() == DEVICETEMPLATE_Generic_Input_Ouput_CONST)
				Command_Impl * child = pChildDeviceCommand;

			this_pin = child->m_pData->m_mapParameters[DEVICEDATA_PortChannel_Number_CONST];

			LoggerWrapper::GetInstance()->Write(LV_STATUS, "statechange Reply: testing %s, %s default state: %s", 
				child->m_sName.c_str(), this_pin.c_str(), child->m_pData->m_mapParameters[DEVICEDATA_Default_State_CONST].c_str());

		}
	}

}


void icpdasbridge::parse_icpdas_reply(std::string message)
{
	// Main parsing routine for all replies received back from GC100.
	// Specific message types are dispatched to a specific handling routine
	// so that this routine stays a manageable size!

//	LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Reply received from ICPDAS: %s",message.c_str());

	string sMessage;
	string sPort;
	string sNewValue ("");
	string sCOM ("");
	vector<string> vMessage;
	vector<string> vEntry;


	if (message.length()>0)
	{
		vMessage = StringUtils::Split(message,":");

		if (vMessage[0] == "INFO") 	
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "We got the connect back from ICPDAS: %s",message.c_str());
			return;
		}
		if (vMessage[0]=="CHANGE")
		{	
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "We got a CHANGE back from ICPDAS: %s",message.c_str());
			sPort = StringUtils::Split(vMessage[1],"=")[1]+":"+StringUtils::Split(vMessage[2],"=")[1]+":"+StringUtils::Split(vMessage[3],"=")[1];
			sNewValue = vMessage[4];
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "We think it is port: %s and the new value should be %s",sPort.c_str(), sNewValue.c_str());
			icp2dce(sPort,sNewValue);			
			return;
		}
		
	}		

/*	if (bChildrenNeedToBeReported)
	{
		cout << "sent messages: " << sent_messages << endl;
		cout << "received messages: " << received_messages << endl;
	}
	if (bChildrenNeedToBeReported && received_messages == sent_messages)
	{
		ReportChildren();
		bChildrenNeedToBeReported = false;
	}
*/
}

void icpdasbridge::populate_children()
{
	VectDeviceData_Impl& vDeviceData = m_pData->m_vectDeviceData_Impl_Children;
	for(VectDeviceData_Impl::size_type i = 0; i < vDeviceData.size(); i++)
	{
		string sChannel = vDeviceData[i]->mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST);
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "creating child device:ICP address %s is Device ID %i of type %i",sChannel.c_str(),vDeviceData[i]->m_dwPK_Device,vDeviceData[i]->m_dwPK_DeviceTemplate);

//		icpDevice *child=icpDevice::getICPDevice(vDeviceData[i]);
/*		if(child!=NULL)
		{
//			childrenmap[vDeviceData[i]->m_dwPK_Device]=child;
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "   child created: %s",child->stype().c_str());
		}
*/	}
}

std::string icpdasbridge::read_from_icpdas(struct timeval *timeout)
{
	std::string return_value;
	return_value = "";
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Before looping stream");
	

//	PLUTO_SAFETY_LOCK(sl, icpdas_mutex);
	while (1)
	{
		pthread_testcancel();

		
		fd_set fdset;

		FD_ZERO(&fdset);
		FD_SET(icpdas_socket, &fdset);

		struct timeval tv;
		if (timeout != NULL)
			tv = *timeout;

		if (
			select(icpdas_socket + 1, &fdset, NULL, NULL, timeout != NULL ? &tv : NULL) <= 0
			|| !FD_ISSET(icpdas_socket, &fdset)
			|| recv(icpdas_socket, &recv_buffer[recv_pos], 1, 0) <= 0
		)
		{
			return_value = "error";
			break;
		}
		pthread_testcancel();
//		LoggerWrapper::GetInstance()->Write(LV_DEBUG, "before recv_buffer compare");
		if (recv_buffer[recv_pos] == '\n')
		{
			recv_buffer[recv_pos] = '\0';

			LoggerWrapper::GetInstance()->Write(LV_DEBUG, "recv buffer: %s", recv_buffer);
			if (strlen(recv_buffer) > 0) {
				parse_icpdas_reply(string(recv_buffer));
				recv_pos = 0;
				return_value = string(recv_buffer);
//				break;
			}
		}
		else
		{
			recv_pos ++;
//			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "recv buffer: %s",recv_buffer);
			
		}
	}
	return return_value; // returns the last complete message
}

bool icpdasbridge::send_to_icpdas(string Cmd)
{
	// returns true if it was ready to send, false if blocked because of not ready
	char command[16384];
	int result;
	bool return_value;

	return_value = true;

//	PLUTO_SAFETY_LOCK(sl, icpdas_mutex);

	sprintf(command, "%s\r", Cmd.c_str()); 
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Sending command %s\n", command);

	result = send(icpdas_socket,command,strlen(command), 0);
	if (result < (int) strlen(command))
	{
		string x = strerror(errno);
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Short write to GC100: %s (wrote: %d)\n", strerror(errno), result);
		return_value = false;
	}

	Sleep(100);
	return return_value;
}


bool icpdasbridge::Open_icpdas_Socket()
// Shamelessly copied from gc100
{
	bool return_value;

	// socket stuff
	struct hostent *hp;
	struct sockaddr_in sin;
	int res;

//	PLUTO_SAFETY_LOCK(sl, icpdas_mutex);
	return_value=false;

//	ip_addr=DATA_Get_TCP_Address();
	LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Connecting to ICPDAS at IP address: %s", DATA_Get_TCP_Address().c_str());

	// Do the socket connect
	hp = gethostbyname(DATA_Get_TCP_Address().c_str());

	if (!hp)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Unable to resolve host name.  Could not connect to ICPDAS");
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Trying to allocate socket");
		icpdas_socket = socket(PF_INET, SOCK_STREAM, 0);
		if (icpdas_socket < 0)
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Unable to allocate socket.  Could not connect to ICPDAS");
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Socket allocated - dealing with more details");
			memset(&sin,0,sizeof(sin));
			sin.sin_family=AF_INET;
			sin.sin_port=htons(DATA_Get_TCP_Port());

			memcpy(&sin.sin_addr, hp->h_addr, sizeof(sin.sin_addr));
			LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Socket allocated - memcpy completed");
			if (connect(icpdas_socket, (sockaddr *) &sin, sizeof(sin)))
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Unable to connect to ICPDAS on socket.");
				close(icpdas_socket);
			}
			else
			{
				res = fcntl(icpdas_socket, F_GETFL);
				if ((res < 0) || (fcntl(icpdas_socket, F_SETFL, res /*| O_NONBLOCK*/) < 0))
				{
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Unable to set flags on ICPDAS socket.");
					close(icpdas_socket);
				}
				else
				{
					LoggerWrapper::GetInstance()->Write(LV_STATUS, "ICPDAS socket connect OK");
					return_value=true;
				}
			}
		}
	}

	return return_value;
}


//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool icpdasbridge::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
icpdasbridge_Command *Create_icpdasbridge(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new icpdasbridge(pPrimaryDeviceCommand, pData, pEvent, pRouter);
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
void icpdasbridge::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	sCMD_Result = "UNHANDLED CHILD";

	string portChannel = pDeviceData_Impl->m_mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST);
	string command ("");
	LoggerWrapper::GetInstance()->Write(LV_DEBUG,"Received Command For CHILD %s", portChannel.c_str());
	switch (pMessage->m_dwID) {
		case COMMAND_Generic_On_CONST:
			LoggerWrapper::GetInstance()->Write(LV_DEBUG,"ON RECEIVED FOR CHILD %s", portChannel.c_str());
			command="1";
			break;
		case COMMAND_Generic_Off_CONST:
			LoggerWrapper::GetInstance()->Write(LV_DEBUG,"OFF RECEIVED FOR CHILD %s", portChannel.c_str());
			command="0";
			break;
		case COMMAND_Set_Level_CONST:
			LoggerWrapper::GetInstance()->Write(LV_DEBUG,"SET LEVEL RECEIVED FOR CHILD %s", portChannel.c_str());
			command = pMessage->m_mapParameters[COMMANDPARAMETER_Level_CONST]; // .c_str(); 
			break;
		case COMMAND_Set_Temperature_CONST:
			LoggerWrapper::GetInstance()->Write(LV_DEBUG,"SET TEMPERATURE RECEIVED FOR CHILD %s", portChannel.c_str());
			command = pMessage->m_mapParameters[COMMANDPARAMETER_Level_CONST]; // .c_str(); 
			break;
			;;
		case COMMAND_Set_Fan_CONST:
			int fan = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_OnOff_CONST].c_str());
			LoggerWrapper::GetInstance()->Write(LV_DEBUG,"SET FAN RECEIVED FOR CHILD %s, level: %d",portChannel.c_str(),fan);
			if (fan == 1) {
				command = "onhigh";
			} else {
				command = "autohigh";
			}
			break;
			;;
	}

	send_to_icpdas("SET:"+portChannel+":"+command);
	
	
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void icpdasbridge::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
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

void icpdasbridge::SomeFunction()
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


