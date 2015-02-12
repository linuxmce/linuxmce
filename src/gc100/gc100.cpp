/*
	gc100
	
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation
	
	www.plutohome.com		
	
	Phone: +1 (877) 758-8648
	
	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
	
	See the GNU General Public License for more details.
*/

//<-dceag-d-b->
#include "gc100.h"
#include "gc100_irl.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_EventParameter.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"
#include "PlutoUtils/LinuxSerialUSB.h"

#include <fcntl.h>
#include <signal.h>
#ifdef WIN32
#define strcasecmp _stricmp
#endif

const timespec LEARNING_TIMEOUT = {40, 0}; // 40 s

//const timespec READY_TIMEOUT = {0, 800000000}; // 800 ms
const timespec READY_TIMEOUT = {3, 0};

const int GC100_COMMAND_PORT = 4998;
const int hardcoded_Socket_Port = 6777;

/** pthread wrapper functions for the in-object thread functions */
// Learning thread
void * StartLearningThread(void * Arg)
{
	LearningInfo *pLearningInfo = (LearningInfo *) Arg;

	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	pLearningInfo->m_pgc100->LearningThread(pLearningInfo);
	delete pLearningInfo;
	return NULL;
}

// Learning thread, called from socket
void * StartLearningThread_via_Socket(void * Arg)
{
	gc100 * pgc100 = (gc100 *) Arg;
	
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	pgc100->LearningThread(NULL);
	return NULL;
}

// gc100 generated events processor: pin change, IR completion
void * StartEventThread(void * Arg)
{
	gc100 * pgc100 = (gc100 *) Arg;
	pgc100->EventThread();
	return NULL;
}

// raw learning socket handler
void * StartSocketThread(void * Arg)
{
	gc100 * pgc100 = (gc100 *) Arg;
	pgc100->SocketThread(hardcoded_Socket_Port);
	return NULL;
}

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
gc100::gc100(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: gc100_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
	, gc100_mutex("gc100")
{
	recv_pos = 0;
	ir_cmd_id = 1;
	m_bStopLearning = false;
	gc100_mutex.Init(NULL);
	m_bQuit_set(false);
	m_bLearning = false;
	m_bMustConvertRC5_6 = true;
	sent_messages = received_messages = 0;
	bChildrenNeedToBeReported = true;
}

//<-dceag-getconfig-b->
bool gc100::GetConfig()
{
	if( !gc100_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	IRBase::setCommandImpl(this);
	IRBase::setAllDevices(&(GetData()->m_AllDevices));

	// TODO: figure this one out: ReplaceParams
	//ReplaceParams(replacement);

	//learn_device = string("/dev/ttyS_") + StringUtils::ltos(m_dwPK_Device) + "_0"; // DEBUG
	learn_device = TranslateSerialUSB(DATA_Get_COM_Port_on_PC());
	
	if (!Open_gc100_Socket())
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Couldn't open socket connection to GC100");
		exit(2);
	}
	return true;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
gc100::~gc100()
//<-dceag-dest-e->
{
	signal(SIGSEGV,SIG_IGN);           //ignore SIGFAULT here
	m_bQuit_set(true);                    //force quit	
	handleStop();                      //force IRBase::DispatchMessage to stop
	Sleep(600);                        //wait a little
	
	PLUTO_SAFETY_LOCK(sl, gc100_mutex);
	
	if (m_EventThread != 0)
	{
		pthread_cancel(m_EventThread);     //pthread_cancel is asynchron so call it first and continue cleanup		
		pthread_join(m_EventThread, NULL); //finish
	}
/*	if (m_SocketThread != 0)
	{
		pthread_cancel(m_SocketThread);     //pthread_cancel is asynchron so call it first and continue cleanup		
		pthread_join(m_SocketThread, NULL); //finish
	}*/
	
	sl.Release();
	pthread_mutex_destroy(&gc100_mutex.mutex);
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool gc100::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void gc100::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	m_pThisMessage = pMessage;

	cout << "Message ID: " << pMessage->m_dwID << endl;
	cout << "From:" << pMessage->m_dwPK_Device_From << endl;
	cout << "To: " << pMessage->m_dwPK_Device_To << endl;
	
	map<long, string>::iterator i;
	for (i = pMessage->m_mapParameters.begin(); i != pMessage->m_mapParameters.end(); i++)
	{
		cout << "Parameter: " << i->first << " Value: " << i->second << endl;
	}

	// It's a Send Code command
	if (pMessage->m_dwID == COMMAND_Send_Code_CONST)
	{
		int iRepeat = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Repeat_CONST].c_str());
		if (iRepeat < 1)
			iRepeat = 1;
		if (iRepeat > 3)
			iRepeat = 3;

		string sPort;
		if (pDeviceData_Impl)
		{
			if (pDeviceData_Impl->m_dwPK_DeviceCategory == DEVICECATEGORY_Infrared_Interface_CONST)
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "Message recipient is my child");
				sPort = pDeviceData_Impl->m_mapParameters[DEVICEDATA_PortChannel_Number_CONST];
			}
			else
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "Message recipient is not my child. Seeing if its parent is.");

				Command_Impl *pChild = GetChild(pDeviceData_Impl->m_dwPK_Device_ControlledVia);
				if (pChild != NULL)
				{
					LoggerWrapper::GetInstance()->Write(LV_STATUS, "Message recipient is my nephew.");
					sPort = pChild->m_pData->m_mapParameters[DEVICEDATA_PortChannel_Number_CONST];
				}
				else
				{
					LoggerWrapper::GetInstance()->Write(LV_WARNING,
						"The parent of the target device is not my child.");
				}
			}
		}
		string sCode = pMessage->m_mapParameters[COMMANDPARAMETER_Text_CONST];
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Sending IR to port '%s', code '%s'", sPort.c_str(), sCode.c_str());
		SendIR(sPort, sCode, iRepeat);
		sCMD_Result = "OK";
		return;
	}

	// DCE does not populate this structure for our children's children, so we have to do it
	if (pDeviceData_Impl->m_dwPK_Device_ControlledVia > 0 && pDeviceData_Impl->m_pDevice_ControlledVia == NULL)
		pDeviceData_Impl->m_pDevice_ControlledVia = m_pData->FindChild(pDeviceData_Impl->m_dwPK_Device_ControlledVia);
	// Let the IR Base class try to handle the message
	if (pDeviceData_Impl && pDeviceData_Impl->FindSelfOrParentWithinCategory(DEVICECATEGORY_Infrared_Interface_CONST) != NULL
		&& IRBase::ProcessMessage(pMessage))
	{
		printf("Message processed by IRBase class\n");
		sCMD_Result = "OK";
		return;
	}

	if (pMessage->m_dwID == COMMAND_Learn_IR_CONST)
	{
		cout << "Cmd: Learn IR '" << pMessage->m_mapParameters[COMMANDPARAMETER_OnOff_CONST] << "' == '" <<
			atoi(pMessage->m_mapParameters[COMMANDPARAMETER_OnOff_CONST].c_str()) << "'" << endl;
		cout << "C: " << (atoi(pMessage->m_mapParameters[COMMANDPARAMETER_OnOff_CONST].c_str()) != 0) << endl;
		if (atoi(pMessage->m_mapParameters[COMMANDPARAMETER_OnOff_CONST].c_str()) != 0)
			LEARN_IR(pMessage->m_dwPK_Device_To, atoi(pMessage->m_mapParameters[COMMANDPARAMETER_PK_Command_CONST].c_str()),
			pMessage->m_dwPK_Device_From, atoi(pMessage->m_mapParameters[COMMANDPARAMETER_PK_Text_CONST].c_str()));
		else
			LEARN_IR_CANCEL();
		sCMD_Result = "OK";
		return;
	}

	// This is a relay command
	cout << "Processing..." << endl;

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Message for %s passed to Relay", pDeviceData_Impl->m_sDescription.c_str());

	bool bParm;
	switch (pMessage->m_dwID)
	{
		case COMMAND_Toggle_Power_CONST:
			bParm = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_OnOff_CONST].c_str()) != 0;
			break;
		case COMMAND_Set_Level_CONST:
			bParm = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Level_CONST].c_str()) != 0;
			break;
		default:
			bParm = pMessage->m_dwID == COMMAND_Generic_On_CONST;
	}
	if (relay_power(pDeviceData_Impl, bParm))
	{
		sCMD_Result = "OK";
		return;
	}

	// DONT: Don't delete there comments until implementation is complete
	// I use the for inspiration... uh... I mean reference so I don't redesign the wheel (not completely anyway) :)
	//if (pMessage->m_dwID == 350 /* ACTION_SET_FREQUENCY */)
	//{
	//	SendString("OK");
	//	string IR_FREQUENCY=pMessage->m_mapParameters[106];
	//	//SET_LEARN_FREQUENCY(IR_FREQUENCY);
	//	return true;
	//}
	//if (pMessage->m_dwID == 348 /*ACTION_LEARN_IR*/)
	//{
	//	SendString("OK");
	//	string ABSOLUTE_CHANNEL=pMessage->m_mapParameters[35]; // Not used
	//	string ID=pMessage->m_mapParameters[53]; // Action ID
	//	string IR_FREQUENCY=pMessage->m_mapParameters[106]; // Not used
	//	LEARN_IR(StringUtils::itos(pMessage->m_dwPK_Device_To),ABSOLUTE_CHANNEL,ID,IR_FREQUENCY);
	//	return true;
	//}
	//if (pMessage->m_dwID == 349 /*ACTION_LEARN_IR_CANCEL*/)
	//{
	//	SendString("OK");
	//	LEARN_IR_CANCEL("");
	//	return true;
	//}
	//if (pMessage->m_dwID == 402 /* send_data */)
	//{
	//	SendString("OK");
	//	send_data("","");
	//	return true;
	//}
	//if (pMessage->m_dwID == 403 /* recv_data */)
	//{
	//	SendString("OK");
	//	recv_data("");
	//	return true;
	//}
	//if (pMessage->m_dwID == 1 /* On */)
	//{
	//	SendString("OK");
	//	relay_power(pMessage,true);
	//	return true;
	//}
	//if (pMessage->m_dwID == 2 /* Off */)
	//{
	//	SendString("OK");
	//	relay_power(pMessage,false);
	//	return true;
	//}

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Mesage Received %d completely unhandled",pMessage->m_dwID);
	//SendString("OK"); // DEBUG
	//return true; // DEBUG

	sCMD_Result = "UNHANDLED CHILD";
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void gc100::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	cout << "Message ID: " << pMessage->m_dwID << endl;
	cout << "From: " << pMessage->m_dwPK_Device_From << endl;
	cout << "To: " << pMessage->m_dwPK_Device_To << endl;
	
	map<long, string>::iterator i;
	for (i = pMessage->m_mapParameters.begin(); i != pMessage->m_mapParameters.end(); i++)
	{
		cout << "Parameter: " << i->first << " Value: " << i->second << endl;
	}
	
	sCMD_Result = "UNKNOWN DEVICE";
}

//<-dceag-sample-b->!

/*

	COMMANDS TO IMPLEMENT

*/

// Returns string starting with freq.  The "sendir,<address>,<id>," are prepended elsewhere
bool gc100::ConvertPronto(string ProntoCode, string &gc_code, int Repeat)
{
	if (Repeat < 1)
		Repeat = 1;
	if (Repeat > 3)
		Repeat = 3;

	std::string token;
	int token_int;
	long int frequency;
	string::size_type pos;
	bool return_value;
	int num_single,num_repeat;
	int expected_length;
	bool error;

	return_value=true;
	error=false;

	//ProntoCode="0000 0048 0000 0018 00c0 00c1 0030 0092 0030 0092 0030 0030 0030 0092 0030 0092 0030 0092 0030 0030 0030 0092 0030 0092 0030 0092 0030 0030 0030 0030 0030 0030 0030 0092 0030 0030 0030 0030 0030 0030 0030 0092 0030 0030 0030 0030 0030 0030 0030 0092 0030 0870"; // DEBUG
	gc_code=""; // Start with this
	pos=0;

	// Get first token
	// Format ID

	if (ProntoCode.length() < 29)
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "convert_pronto: Total code length is %d, below minimum possible length",ProntoCode.length());
		return_value=false;
	}
	else
	{
		token=StringUtils::Tokenize(ProntoCode," ",pos);
		sscanf(token.c_str(),"%4x",&token_int);

		if (token_int==0 || token_int==256 )
		{
			// Carrier divider
			token=StringUtils::Tokenize(ProntoCode," ",pos);
			sscanf(token.c_str(),"%4x",&token_int);

			frequency=(((41450/token_int)+5)/10)*1000;
			gc_code=gc_code+StringUtils::itos(frequency)+","+StringUtils::itos(Repeat);

			// # burst pairs for single sequence
			token=StringUtils::Tokenize(ProntoCode," ",pos);
			sscanf(token.c_str(),"%4x",&token_int);
			num_single=token_int;

			// # burst pairs for repeat sequence
			token=StringUtils::Tokenize(ProntoCode," ",pos);
			sscanf(token.c_str(),"%4x",&token_int);
			num_repeat=token_int;

			// Figure repeat offset
			gc_code+=","+StringUtils::itos(num_single*2+1);

			expected_length=(num_single+num_repeat)*10+19;
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "convert_pronto: Total code length is %d, expecting %d",ProntoCode.length(),expected_length);

			if (true)
			{ // For right now don't enforce the length check
				for (int i=0; (i< ((num_single+num_repeat)*2)) && !error; i++)
				{
					token=StringUtils::Tokenize(ProntoCode," ",pos);
					if (token=="")
					{
						LoggerWrapper::GetInstance()->Write(LV_WARNING, "convert_pronto: Token was NULL, probably because code was too short or incorrectly formatted");
						error=true;
					}
					else
					{
						sscanf(token.c_str(),"%4x",&token_int);
						gc_code+=","+StringUtils::itos(token_int);
					}
				}
			}
			else
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "convert_pronto: Total code length is %d, expecting %d",ProntoCode.length(),expected_length);
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "convert_pronto: Code was incorrect length");
				return_value=false;
			}
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "convert_pronto: Can't convert code types other than 0000");
			return_value=false;
		}

		// Ending
		//LoggerWrapper::GetInstance()->Write(LV_STATUS, "convert_pronto: Final conversion: %s",gc_code.c_str());

		//gc_code="58000,1,1,192,193,48,146,48,146,48,48,48,146,48,146,48,146,48,48,48,146,48,146,48,146,48,48,48,48,48,48,48,146,48,48,48,48,48,48,48,146,48,48,48,48,48,48,48,146,48,2160"; // DEBUG
	}
	return return_value;
}

bool gc100::send_to_gc100(string Cmd)
{
	// returns true if it was ready to send, false if blocked because of not ready
	char command[16384];
	int result;
	bool return_value;

	return_value = true;

//	PLUTO_SAFETY_LOCK(sl, gc100_mutex);

	sprintf(command, "%s\r", Cmd.c_str()); // gc100 commands end in CR (w/o LF)
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Sending command %s\n", command);

	result = send(gc100_socket,command,strlen(command), 0);
	if (result < (int) strlen(command))
	{
		string x = strerror(errno);
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Short write to GC100: %s (wrote: %d)\n", strerror(errno), result);
		return_value = false;
	}

	// allow packet to be sent through the network; gc100 can't distinguish between commands if they go together
	Sleep(100);
	return return_value;
}

void gc100::parse_message_device(std::string message)
{
	int module;
	std::string key;
	std::string type;
	int count;
	class module_info scratch;
	string::size_type pos,pos2;
	std::string token,token2;
	int next_slot;
	std::map<std::string,int>::iterator slot_iter;

	pos=0;
	pos2=0;

	// first token is 'device'
	token=StringUtils::Tokenize(message,",",pos);
	if (token != "device")
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"Sanity check: was expecting 'device' not %s",token.c_str());
	}

	// second token is module number
	token=StringUtils::Tokenize(message,",",pos);
	sscanf(token.c_str(),"%d",&module);
	if (module<1)
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"Sanity check: module number should be positive not %d",module);
	}

	// third token is number and type of device, e.g. '3 IR'
	token=StringUtils::Tokenize(message,",",pos);

	// Now sub-tokenize the last token to split it into number and type
	token2=StringUtils::Tokenize(token," ",pos2);
	sscanf(token.c_str(),"%d",&count);
	if (count<1)
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"Sanity check: slot count should be positive not %d",count);
	}

	type=StringUtils::Tokenize(token," ",pos2); // Module type is the only thing left

	/*
	 * gc100 and iTach w/ IR ports always report '3 IR' for IR modules
	 * iTach Flex reports '1 IR' or '1 IR_BLASTER' for the single IR configuration (second one is for GC-BL2)
	 *   and '1 IRTRIPORT' or '1 IRTRIPORT_BLASTER' for the 3-port IR configuration (second one is for GC-BL2)
	 * We use this information to signel the iTachFlex out and not attempt to find sensors on its IR ports
	 */
	bool iTachFlex = false;
	if (count == 1 && type == "IR")
	{
		iTachFlex = true;
	}
	else if (type == "IR_BLASTER")
	{
		type = "IR";
		iTachFlex = true;
	}
	else if (type == "IRTRIPORT" || type == "IRTRIPORT_BLASTER")
	{
		/*
		 * while the iTach Flex reports '1 IRTRIPORT', the 3 ports are accessible at IDs 1:1, 1:2, 1:3
		 * we translate this to '3 IR' and the original gc100 code works as expected
		 */
		type = "IR";
		count = 3;
		iTachFlex = true;
	}

	// OK we have all the info from the message now.  Create other derived values and add the right number of devices
	// First figure out what the next slot number should be
	slot_iter=slot_map.find(type);
	if (slot_iter==slot_map.end())
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Entering default value for type %s",type.c_str());
		slot_map.insert(pair<std::string,int> (type,1));
	}

	slot_iter=slot_map.find(type);
	if (slot_iter==slot_map.end())
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"Sanity check: slot number not found for type %s",type.c_str());
	}
	else
	{
		next_slot=slot_iter->second;
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"The next slot for this type should be %d",next_slot);

		for (int i=0; i<count; i++) {
			key=StringUtils::itos(module)+":"+StringUtils::itos(i+1);

			scratch.module=module;
			scratch.slot=i+1;
			scratch.type=type;
			scratch.key=key;
			scratch.global_slot=next_slot;

			module_map.insert(pair<std::string, class module_info> (key,scratch));
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Created new module: mod=%d slot=%d type=%s key=%s global=%d",scratch.module,scratch.slot,scratch.type.c_str(),scratch.key.c_str(),scratch.global_slot);

			// An IR module might be used for input, so request the state.  
			// Either it will return the state, or an error if it's really for IR (which we ignore)
			if (scratch.type == "IR" && !iTachFlex)
			{
				send_to_gc100("getstate,"+key);
				sent_messages++;
				cout << "sent messages: " << sent_messages << endl;
			}

			next_slot++;
		}
		// Now save next_slot so the next module can number (global number only) starting from where we left off
		slot_map.erase(type);
		slot_map.insert(pair<std::string,int> (type,next_slot));
	}
}

void gc100::Start_seriald()
{
	std::map<std::string, class module_info>::iterator serial_iter;
	int global_slot;
	char command[512];

	//sprintf(command,"killall socat");
	//system(command);

	PLUTO_SAFETY_LOCK(sl, gc100_mutex);

	// TODO: sPortConfig from Core's DeviceData parameters, not ours
	//string sPortConfig = m_pData->m_mapParameters[DEVICEDATA_Port_CONST];
	//string::size_type begin = 0, end = 0;
	//vector<pair<string, string> > vect_sPorts, vect_sMyPorts;
	//while (begin < sPortConfig.length())
	//{
	//	end = sPortConfig.find(";", begin);
	//	string sPortData = sPortConfig.substr(begin, end-begin);
	//	string::size_type middle;
	//	middle = sPortData.find("|");
	//	string sDevice = sPortData.substr(0, middle);
	//	string sDescription = sPortData.substr(middle + 1, end - middle - 1);
	//	pair<string, string> pssPort = pair<string, string>(sDevice, sDescription);
	//	vect_sPorts.insert(vect_sPorts.end(), pssPort);
	//	if (sDevice.find(string("ttyS_") + StringUtils::ltos(m_dwPK_Device) + "_"))
	//		vect_sMyPorts.insert(vect_sMyPorts.end(), pssPort);
	//}

	for (serial_iter=module_map.begin(); serial_iter!=module_map.end(); ++serial_iter)
	{
		if (serial_iter->second.type == "SERIAL")
		{
			global_slot = serial_iter->second.global_slot;

			string sDevName(string("ttyS_") + StringUtils::ltos(m_dwPK_Device) + "_" + StringUtils::ltos(global_slot - 1));
			string sDevice(string("/dev/") + sDevName);

#ifndef WIN32
			symlink("/dev/null", sDevice.c_str());	// make sure the link exists, to avoid race condition
													// when gc100-serial-bridge is delayed, for UpdatedAvailableSerialPorts to work
													// and also avoid introducing a busy loop like the one currently commented out below
#endif

//			sprintf(command, "socat -v TCP4:%s:%d PTY,link=%s,echo=false,icanon=false,raw >>/var/log/pluto/%s.log 2>&1 &",
//				ip_addr.c_str(), global_slot+GC100_COMMAND_PORT, sDevice.c_str(), sDevName.c_str());
			snprintf(command, 512, "/usr/pluto/bin/gc100-serial-bridge.sh %s %d %s &", ip_addr.c_str(), global_slot+GC100_COMMAND_PORT, sDevice.c_str());
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "seriald cmd: %s", command);
			system(command);

#if 0
			struct stat Stat;
			while (lstat(sDevice.c_str(), &Stat) == -1)
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "Waiting for %s symlink to be created", sDevice.c_str());
				Sleep(1000);
			}
#endif
		}
	}
	snprintf(command, 512, "/usr/pluto/bin/UpdateAvailableSerialPorts.sh");
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Update Available Serial Ports cmd: %s", command);
	system(command);
}

void gc100::parse_message_statechange(std::string message, bool change)
{
	std::string token;
	std::string::size_type pos;
	std::string module_address; // probably in 4:3 format
	int input_state;
	MapCommand_Impl::iterator child_iter;
	Command_Impl *nomination = NULL;
	Command_Impl *result;
	int global_pin_target;
	std::map<std::string, class module_info>::iterator map_iter;
	Command_Impl *pChildDeviceCommand;
	std::string target_type;

	pos=0;
	//LoggerWrapper::GetInstance()->Write(LV_STATUS, "statechange Reply received from GC100: %s",message.c_str());

	string sCommand = StringUtils::Tokenize(message,",",pos); // state/statechange
	module_address = StringUtils::Tokenize(message,",",pos); // address

	token=StringUtils::Tokenize(message,",",pos); // input state
	sscanf(token.c_str(),"%d",&input_state);

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "statechange Reply interpreted as module: %s change to %d",module_address.c_str(),input_state);

	// Now figure out which child this belongs to.
	nomination=NULL;
	result=NULL;
	global_pin_target=0;

	// First look up this module to see what its global pin number is; we might need it later
	map_iter=module_map.find(module_address);
	
	if (map_iter != module_map.end())
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "statechange Reply: found module of type %s, %d", map_iter->second.type.c_str(), map_iter->second.global_slot);
		target_type = map_iter->second.type;

		if (target_type == "IR")
			map_iter->second.type = target_type = "SENSOR";

		if (target_type == "SENSOR" || target_type == "RELAY")
		{
			global_pin_target=map_iter->second.global_slot;
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "statechange Reply: found module but it's not SENSOR/RELAY; it's %s",target_type.c_str());
		}
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "statechange Reply: did not find module %s in map",module_address.c_str());
	}

	// Now do the search of all the children.  We're going to look at the pin number.
	// What we'd really like to find is something in the 4:3 format.  If we see that,
	// it takes priority.  The next best thing will be a global pin id (ie 5)

	for (child_iter=m_mapCommandImpl_Children.begin(); child_iter!=m_mapCommandImpl_Children.end(); ++child_iter)
	{
		pChildDeviceCommand = (*child_iter).second;

		if (pChildDeviceCommand->m_pData->WithinCategory(DEVICECATEGORY_Environment_CONST))
		{
			std::string this_pin;

//			if (pChildDeviceCommand->PK_DeviceTemplate_get() == DEVICETEMPLATE_Generic_Input_Ouput_CONST)
				Command_Impl * child = pChildDeviceCommand;

			this_pin = child->m_pData->m_mapParameters[DEVICEDATA_PortChannel_Number_CONST];

			LoggerWrapper::GetInstance()->Write(LV_STATUS, "statechange Reply: testing %s, %s (state %d) default state: %s", 
				child->m_sName.c_str(), this_pin.c_str(), input_state, child->m_pData->m_mapParameters[DEVICEDATA_Default_State_CONST].c_str());

			//LoggerWrapper::GetInstance()->Write(LV_STATUS, "statechange Reply: found a child pin number of %s, direction is %s",this_pin.c_str(),io_direction.c_str());

			if (target_type == "SENSOR" || target_type == "RELAY")
			{
				// See if it matches exactly
				if (this_pin == module_address)
				{
					LoggerWrapper::GetInstance()->Write(LV_STATUS, "statechange Reply: matches exactly in m:s format");
					result = child;
				}

				// See if it matches the global number
				if (global_pin_target > 0 && atoi(this_pin.c_str()) == global_pin_target)
				{
					LoggerWrapper::GetInstance()->Write(LV_STATUS, "statechange Reply: matches global number");
					nomination = child;
				}
			}
			else
			{
				//LoggerWrapper::GetInstance()->Write(LV_STATUS, "statechange Reply: Sorry, not eligible (not the correct type)");
			}
		}
	}

	if (result==NULL)
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "statechange Reply: no exact match in m:s format, trying global pin number");
		result=nomination;
	}

	if (result!=NULL)
	{
		if( result->m_pData->m_mapParameters[DEVICEDATA_Default_State_CONST]=="1" )
			input_state = !input_state;
		if (target_type == "SENSOR")
		{
			Message *pMessage = new Message(result->m_dwPK_Device, 0, PRIORITY_NORMAL, MESSAGETYPE_EVENT, EVENT_Sensor_Tripped_CONST,
				1, EVENTPARAMETER_Tripped_CONST, StringUtils::itos(input_state).c_str());
			QueueMessageToRouter(pMessage);

			// If this sensor has children, fire the same event from them too
			DeviceData_Base *pDevice = m_pData->m_AllDevices.m_mapDeviceData_Base_Find(result->m_dwPK_Device);
			if( pDevice )
			{
				for(vector<DeviceData_Base *>::iterator itChildren = pDevice->m_vectDeviceData_Base_Children.begin(); itChildren != pDevice->m_vectDeviceData_Base_Children.end(); ++itChildren )
				{
					DeviceData_Base *pDevice_Child = *itChildren;
					Message *pMessage = new Message(pDevice_Child->m_dwPK_Device, 0, PRIORITY_NORMAL, MESSAGETYPE_EVENT, EVENT_Sensor_Tripped_CONST,
						1, EVENTPARAMETER_Tripped_CONST, StringUtils::itos(input_state).c_str());
					QueueMessageToRouter(pMessage);
				}
			}
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "statechange Reply: target_type not SENSOR, not sending event");
		}

		if (change)
		{
			// If this was a 'statechange', reissue a state request.  At least once a statechange has been missed.
			std::string verify_request;

			verify_request="getstate,"+module_address;
			send_to_gc100(verify_request);
		}
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "statechange Reply: Sorry, after all that searching, I can't determine which child device should be sent the pin_changed");
	}
}

void gc100::parse_gc100_reply(std::string message)
{
	// Main parsing routine for all replies received back from GC100.
	// Specific message types are dispatched to a specific handling routine
	// so that this routine stays a manageable size!

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Reply received from GC100: %s",message.c_str());

	if (message.length()>6)
	{
		if (message.substr(0,6)=="device")
		{
			parse_message_device(message); // pass it off to the "device message" routine
			return;
		}
	}

	if (message.length()>10)
	{
		if (message.substr(0,10)=="completeir")
		{
			m_bIRComplete = true;
		}
	}

	if (message.length()>6)
	{
		if ( (message.substr(0,6)=="state,") )
		{
			parse_message_statechange(message,false);
			received_messages++;
		}
	}

	if (message.length()>10)
	{
		if ( (message.substr(0,10)=="statechang") )
		{
			parse_message_statechange(message,true);
		}
	}

	if (message.substr(0, 14) == "unknowncommand")
	{
		// From: http://www.globalcache.com/support/port4998.html
		// unknowncommand 5/6/7 = Connector address 1/2/3 is set up as "sensor in" when attempting to send an IR command.
		if (message.substr(15) == "5" || message.substr(15) == "6" || message.substr(15) == "7")
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Attepted to send IR through a sensor input");
			m_bIRComplete = true;
		}
		if (message.substr(15, 2) == "13")
			received_messages++;
	}

	if (bChildrenNeedToBeReported)
	{
		cout << "sent messages: " << sent_messages << endl;
		cout << "received messages: " << received_messages << endl;
	}
	if (bChildrenNeedToBeReported && received_messages == sent_messages)
	{
		ReportChildren();
		bChildrenNeedToBeReported = false;
	}
}

std::string gc100::read_from_gc100(struct timeval *timeout)
{
	std::string return_value;
	return_value = "";
	

//	PLUTO_SAFETY_LOCK(sl, gc100_mutex);
	while (1)
	{
		pthread_testcancel();

		fd_set fdset;

		FD_ZERO(&fdset);
		FD_SET(gc100_socket, &fdset);

		struct timeval tv;
		if (timeout != NULL)
			tv = *timeout;

		if (
			select(gc100_socket + 1, &fdset, NULL, NULL, timeout != NULL ? &tv : NULL) <= 0
			|| !FD_ISSET(gc100_socket, &fdset)
			|| recv(gc100_socket, &recv_buffer[recv_pos], 1, 0) <= 0
		)
		{
			return_value = "error";
			break;
		}
		pthread_testcancel();
		if (recv_buffer[recv_pos] == '\r')
		{
			recv_buffer[recv_pos] = '\0';
			if (strlen(recv_buffer) > 0) {
				parse_gc100_reply(string(recv_buffer));
				recv_pos = 0;
				return_value = string(recv_buffer);
				break;
			}
		}
		else
		{
			recv_pos ++;
		}
	}
	return return_value; // returns the last complete message
}

bool gc100::Open_gc100_Socket()
{
	bool return_value;

	// socket stuff
	struct hostent *hp;
	struct sockaddr_in sin;
	int res;

	PLUTO_SAFETY_LOCK(sl, gc100_mutex);
	return_value=false;

	ip_addr=GetIpAddress();
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Connecting to gc100 IP Address: %s", ip_addr.c_str());

	// Do the socket connect
	hp = gethostbyname(ip_addr.c_str());

	if (!hp)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Unable to resolve host name.  Could not connect to GC100");
	}
	else
	{
		gc100_socket = socket(PF_INET, SOCK_STREAM, 0);
		if (gc100_socket < 0)
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Unable to allocate socket.  Could not connect to GC100");
		}
		else
		{
			memset(&sin,0,sizeof(sin));
			sin.sin_family=AF_INET;
			sin.sin_port=htons(GC100_COMMAND_PORT);

			memcpy(&sin.sin_addr, hp->h_addr, sizeof(sin.sin_addr));
			if (connect(gc100_socket, (sockaddr *) &sin, sizeof(sin)))
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Unable to connect to GC100 on socket.");
				close(gc100_socket);
			}
			else
			{
#ifdef WIN32
				{
					unsigned long x = 1;
					res = 0/*ioctlsocket(gc100_socket, FIONBIO, &x)*/;
				}
				if ((res == SOCKET_ERROR))
#else
				res = fcntl(gc100_socket, F_GETFL);
				if ((res < 0) || (fcntl(gc100_socket, F_SETFL, res /*| O_NONBLOCK*/) < 0))
#endif
				{
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Unable to set flags on GC100 socket.");
					close(gc100_socket);
				}
				else
				{
					LoggerWrapper::GetInstance()->Write(LV_STATUS, "GC100 socket connect OK");
					return_value=true;
				}
			}
		}
	}

	return return_value;
}

bool gc100::GetPinDeviceID(class DeviceData_Impl *pDeviceData,string &this_pin,int &this_device_id)
{
	if( !pDeviceData || pDeviceData->m_dwPK_Device==m_dwPK_Device)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "gc100::GetPinDeviceID Cannot find port_channel for destination device");
		return false;
	}

	this_pin = pDeviceData->m_mapParameters[DEVICEDATA_PortChannel_Number_CONST];
	if( this_pin.empty()==false )
	{
		// We found it
		this_device_id = pDeviceData->m_dwPK_Device;
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "gc100::GetPinDeviceID got pin %s device %d", this_pin.c_str(), this_device_id);
		return true;
	}

	// Maybe it's a grand-child of the gc100, like a child of the relays.  If so, see if the parent is a child of the gc100
	Command_Impl *pCommand_Impl = GetChild(pDeviceData->m_dwPK_Device_ControlledVia);
	if( pCommand_Impl && pCommand_Impl->m_pData )
		return GetPinDeviceID(pCommand_Impl->m_pData,this_pin,this_device_id);

	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "gc100::GetPinDeviceID Cannot find port_channel for destination device %d", pDeviceData->m_dwPK_Device);
	return false;
}

// TODO: This resembles a little (or is that a lot?) with (part of) parse_message_statechange (or it looks like it does, anyway) -- Radu
bool gc100::relay_power(class DeviceData_Impl *pDeviceData, bool power_on)
{
	bool bResult = false;
	std::map<std::string, class module_info>::iterator map_iter;

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Relay Pwr.: target device is %d", pDeviceData->m_dwPK_Device);

	std::string this_pin;
	int this_device_id;

	if( GetPinDeviceID(pDeviceData,this_pin,this_device_id)==false )
		return false;

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Relay Pwr.: This device is %d",this_device_id);

	PLUTO_SAFETY_LOCK(sl, gc100_mutex);
	std::string module_id;
	std::map<std::string,class module_info>::iterator module_iter;

	module_iter=module_map.find(this_pin);

	if (module_iter==module_map.end())
	{
		std::map<std::string,class module_info>::iterator module_iter2;
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Relay Pwr.: Did not find %s in module map",this_pin.c_str());

		// OK, it must be in the global number format.  Search through the RELAY modules until you find it.
		for (module_iter2=module_map.begin(); module_iter2!=module_map.end(); ++module_iter2++)
		{
			if (module_iter2->second.type == "RELAY") {
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"Relay Pwr.: Found a Relay number %d",module_iter2->second.global_slot);

				if (StringUtils::itos(module_iter2->second.global_slot) == this_pin)
				{
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"Relay Pwr.: Matched Relay number %d.  ID will be %s",module_iter2->second.global_slot,module_iter2->first.c_str());
					module_id=module_iter2->first;
				}
			}
		}
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Relay Pwr.: Found %s in module map",this_pin.c_str());
		module_id=this_pin;
	}

	// Module ID should now be the key of the desired relay
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Relay Pwr.: Regardless, module ID is %s",module_id.c_str());

	// Sanity check.  Make sure module_id really is there
	module_iter=module_map.find(module_id);

	if (module_iter==module_map.end())
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"Relay Pwr.: Module_iter %s should be found but isn't",module_id.c_str());
	}
	else
	{
		std::string cmd;

		// Compose the command to the GC100
		cmd = "setstate," + module_id + "," + (power_on ? "1" : "0") ;
		send_to_gc100(cmd);
		bResult = true;
	}

	return bResult;
}

void gc100::SendIR(string Port, string IRCode,int iRepeat)
{
	if( IRCode.find('&')!=string::npos )
	{
		string::size_type pos=0;
		while(pos<IRCode.size() && pos!=string::npos )
		{
			string _ircode = StringUtils::Tokenize(IRCode,"&",pos);
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"pos %d size %d Checking %s\n for multiple codes, got: %s",pos,(int) IRCode.size(),IRCode.c_str(),_ircode.c_str());
			SendIR(Port,_ircode,iRepeat);
			if( pos<IRCode.size() )
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"Sleeping for 500 ms since there are multiple codes");
				Sleep(2000);
			}
		}
		return;
	}
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "SendIR wrapper: Port = %s code = %s", Port.c_str(), IRCode.c_str());
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "SendIR: Port = %s", Port.c_str());
	
	m_bIRComplete = false;

	std::string gc_code,send_cmd;
	std::map<std::string,class module_info>::iterator slot_iter;
	//int timeout_count;

	//ConvertPronto(IRCode.substr(2), gc_code);
	bool ConvertReturn = ConvertPronto(IRCode, gc_code, iRepeat);
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "SendIR: Result of Pronto conversion was: %s", gc_code.c_str());

	if (! ConvertReturn)
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "SendIR: Pronto conversion failed");
		return;
	}

	// Search through all the IR modules and send the code to any IR modules which either matches
	// the 4:2 style port id or the absolute ('6') port ID or if the port string is blank

	PLUTO_SAFETY_LOCK(sl, gc100_mutex);
	for (slot_iter=module_map.begin(); slot_iter !=module_map.end(); ++slot_iter)
	{
		//LoggerWrapper::GetInstance()->Write(LV_STATUS, "SendIR: Examining slot %s",slot_iter->second.key.c_str());

		if ((slot_iter->second.type=="IR") &&
			(
				(slot_iter->second.key == Port) ||
				(StringUtils::itos(slot_iter->second.global_slot) == Port) ||
				(Port == "")
			)
		)
		{
			//LoggerWrapper::GetInstance()->Write(LV_STATUS, "SendIR: Yes, this is one I should send to %s",slot_iter->second.key.c_str());
// TODO: actual send, not queue
			// Create header for this command: sendir,<address>,<id>,

			send_cmd = std::string("sendir,") + slot_iter->second.key.c_str() + std::string(",") + StringUtils::itos(ir_cmd_id)
				+ "," + gc_code;
			m_bIRComplete = false;
			send_to_gc100(send_cmd);

			timeval StartTime;
			gettimeofday(&StartTime, NULL);
			timespec tsStartTime;
			timeval_to_timespec(&StartTime, &tsStartTime);

			while (! m_bIRComplete)
			{
				timeval CurTime;
				gettimeofday(&CurTime, NULL);
				timespec tsCurTime;
				timeval_to_timespec(&CurTime, &tsCurTime);

				if (tsCurTime > tsStartTime + READY_TIMEOUT)
				{
					LoggerWrapper::GetInstance()->Write(LV_WARNING, "Timed out waiting for completeir");
					// Do not reissue the previous command, because it may be a sendir command to an ir port
					// that is configured as discrete input.  In that situation the command will never work
					// and you'll be forced to retry it forever
					break;
				}
			}
			// we get here when event thread got a completeir
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Finished sending IR");

			ir_cmd_id++;
		} // end if it's IR
	} // end loop through devices
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Finished sending all IRs");
}

// TODO: Create LearningInfo object and spawn new thread
void gc100::LEARN_IR(long PK_Device, long PK_Command, long PK_Device_Orbiter, long PK_Text)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"RECEIVED LEARN_IR PKID_Device='%ld' CommandID='%ld'", PK_Device, PK_Command);

	PLUTO_SAFETY_LOCK(sl, gc100_mutex);

	learn_input_string=""; // Clear out the IR to learn the next sequence clean

//	if (is_open_for_learning)
//	{
		if (! m_bLearning)
		{
			LearningInfo * pLI = new LearningInfo(PK_Device, PK_Command, PK_Device_Orbiter, PK_Text, this);
			if (pthread_create(&m_LearningThread, NULL, StartLearningThread, (void *) pLI))
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed to create Event Thread");
				m_bQuit_set(true);
				exit(1);
			}
			m_bLearning = true;
			pthread_detach(m_LearningThread);
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Learning thread already running");
		}
//	}
//	else
//	{
//		LoggerWrapper::GetInstance()->Write(LV_WARNING,"Can't learn because no learning port is open");
//		DCE::CMD_Set_Text CMD_Set_Text(m_dwPK_Device, PK_Device_Orbiter, "", "Can't learn because no learning port is open", PK_Text);
//		SendCommand(CMD_Set_Text);
//	}
}

int gc100::LEARN_IR_via_Socket()
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Received LEARN_IR via Socket");
	PLUTO_SAFETY_LOCK(sl, gc100_mutex);

	learn_input_string = "";

	if (! m_bLearning)
	{
		if (pthread_create(&m_LearningThread, NULL, StartLearningThread_via_Socket, (void *) this))
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed to create Event Thread");
			m_bQuit_set(true);
			exit(1);
		}
		m_bLearning = false;
		pthread_detach(m_LearningThread);
		return 0;
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Learning thread already running");
		string sMsg = "BUSY\n";
		send(learn_client, sMsg.c_str(), sMsg.length(), 0);
		return -1;
	}
}

bool gc100::open_for_learning()
{
	bool return_value;

	return_value = true;

#ifdef WIN32
#define O_RDONLY _O_RDONLY
#define O_NONBLOCK 0
#define open _open
#endif

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Trying to open learning device: %s", learn_device.c_str());
	learn_fd = open(learn_device.c_str(), O_RDONLY /*| O_NONBLOCK*/);

	if (learn_fd < 0)
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Failed to open learning device: %s %s", learn_device.c_str(), strerror(errno));
		return_value = false;
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Opened learning device successfully: %s (%d)", learn_device.c_str(), learn_fd);
	}

#ifdef WIN32
	{
		unsigned long x = 1;
		int res = ioctlsocket(gc100_socket, FIONBIO, &x);
	}
#endif
	return return_value;
}

void gc100::LearningThread(LearningInfo * pLearningInfo)
{
	bool bLearnedCode = false;
	char learn_buffer[512];
	bool learning_error = false;
	int retval;
	int ErrNo;

	long PK_Command, PK_Device, PK_Device_Orbiter, PK_Text;

	// pLearningInfo is NULL when called from socket (raw learning)
	if (pLearningInfo)
	{
		PK_Command = pLearningInfo->m_PK_Command;
		PK_Device = pLearningInfo->m_PK_Device;
		PK_Device_Orbiter = pLearningInfo->m_PK_Device_Orbiter;
		PK_Text = pLearningInfo->m_PK_Text;

		{
			DCE::CMD_Set_Text CMD_Set_Text(m_dwPK_Device, PK_Device_Orbiter, "", "Waiting for IR code", PK_Text);
			SendCommand(CMD_Set_Text);
		}
	}
	else
	{
		PK_Command = PK_Device = PK_Device_Orbiter = PK_Text = 0;
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Learning thread started");
	timeval StartTime;
	gettimeofday(&StartTime, NULL);
	timespec tsStartTime;
	timeval_to_timespec(&StartTime, &tsStartTime);
	
	// Read any IR data present
	retval = 0;

	PLUTO_SAFETY_LOCK(sl, gc100_mutex);
	m_bLearning = true;

//	if (is_open_for_learning)
	if (open_for_learning())
	{
		struct timeval timeout, timeout_tmp;
		fd_set fdset;

		// clear gc100 serial buffer
		FD_ZERO(&fdset);
		FD_SET(learn_fd, &fdset);
		timeout_tmp.tv_usec = 0;
		timeout_tmp.tv_sec = 0;
		while (select(learn_fd + 1, &fdset, NULL, NULL, &timeout_tmp) > 0)
		{
			if (read(learn_fd, learn_buffer, 511) < 0)
			{
				retval = errno;
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "Buffer clearing loop exited with read() error: errno=%d (%s)\n", retval, strerror(retval));
				m_bQuit_set(true);
				break;
			}
			
			FD_ZERO(&fdset);
			FD_SET(learn_fd, &fdset);
			timeout_tmp.tv_usec = 0;
			timeout_tmp.tv_sec = 0;
		}

		FD_ZERO(&fdset);
		FD_SET(learn_fd, &fdset);
		
		timespec_to_timeval(&LEARNING_TIMEOUT, &timeout);
		if (timeout.tv_usec != 0)
		{
			timeout.tv_usec = 0;
			timeout.tv_sec++;
		}

		struct timeval tv1s;
		tv1s.tv_sec = 1;
		tv1s.tv_usec = 0;

		timeout_tmp = tv1s;
		while (! m_bQuit_get() && ! m_bStopLearning && timeout.tv_sec > 0 && select(learn_fd + 1, &fdset, NULL, NULL, &timeout_tmp) == 0)
		{
			timeout.tv_sec--;
			timeout_tmp = tv1s;
			FD_ZERO(&fdset);
			FD_SET(learn_fd, &fdset);
		}

		FD_ZERO(&fdset);
		FD_SET(learn_fd, &fdset);
		if (timeout.tv_sec > 0 && ! m_bQuit_get()&& ! m_bStopLearning && select(learn_fd + 1, &fdset, NULL, NULL, &tv1s) > 0)
		{
			retval = read(learn_fd, learn_buffer, 511);

			string ErrorMsg = "";
			while (retval > 0)
			{
				learn_buffer[retval]='\0';
				if (strstr(learn_buffer, "X") != NULL || strstr(learn_buffer, "Z") != NULL)
				{
					// X from GC-IRL indicates error: probably "code too long": "serial buffer overflow" according to GC-IRE docs
					// Z from GC-IRL indicates error (rarely seen though): "IR signal corruption" according to GC-IRE docs
					learning_error = true;
					ErrorMsg = "GC100 reported X or Z error.";
					LoggerWrapper::GetInstance()->Write(LV_WARNING, ErrorMsg.c_str());
				}
				else
				{
					// input sanity check
					for (int i = 0; i < retval; i++)
					{
						unsigned char c = learn_buffer[i];
						if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || c == ',' || c == '\r' || c == '-')
							continue;
						learning_error = true;
						ErrorMsg = "Garbage data received. Wrong baud rate?";
						LoggerWrapper::GetInstance()->Write(LV_WARNING, ErrorMsg.c_str());
					}
				}

				LoggerWrapper::GetInstance()->Write(LV_STATUS, "IR %d Data received: %s", retval, learn_buffer);
				learn_input_string += learn_buffer;
				receiving_data = true;

				timeout.tv_sec = 0;
				timeout.tv_usec = 100000;
				FD_ZERO(&fdset);
				FD_SET(learn_fd, &fdset);

				retval = select(learn_fd + 1, &fdset, NULL, NULL, &timeout);
				if (retval > 0)
					retval = read(learn_fd, learn_buffer, 511);
				cout << "R: " << retval << ", S: " << (FD_ISSET(learn_fd, &fdset) ? 1 : 0) << endl;
			}

			ErrNo = errno;
			learning_error |= retval < 0;

			if (receiving_data)
			{
				std::string pronto_result;

				if (!learning_error)
				{
					// Only send this if there was no error detected
					if (pLearningInfo)
					{
						LoggerWrapper::GetInstance()->Write(LV_STATUS, "Finished learning, Device=%d, Command=%d, Orbiter=%d, size=%d", PK_Device,
							PK_Command, PK_Device_Orbiter, learn_input_string.length());
					}
					else
					{
						LoggerWrapper::GetInstance()->Write(LV_STATUS, "Finished learning, socket");
					}

					LoggerWrapper::GetInstance()->Write(LV_STATUS, "Terminating on retval of %d err=%d, desc=%s", retval, ErrNo, strerror(ErrNo));
					LoggerWrapper::GetInstance()->Write(LV_STATUS, "Raw learn string received: %s\n", learn_input_string.c_str());

					pronto_result = gc100_irl::IRL_to_pronto(learn_input_string);
					LoggerWrapper::GetInstance()->Write(LV_STATUS, "Conversion to Pronto: %s", pronto_result.c_str());

					if (pLearningInfo)
					{
						// TODO: lookup IR plugin once and send message to it directly instead of this
						DCE::CMD_Store_Infrared_Code_Cat CMD_Store_Infrared_Code_Cat(m_dwPK_Device,
							DEVICECATEGORY_Infrared_Plugins_CONST, false, BL_SameHouse,
							PK_Device, pronto_result, PK_Command);
						getCommandImpl()->SendCommand(CMD_Store_Infrared_Code_Cat);
						getCodeMap()[longPair(PK_Device, PK_Command)] = pronto_result;
	
						bLearnedCode = true;
						DCE::CMD_Set_Text CMD_Set_Text(m_dwPK_Device, PK_Device_Orbiter, "", "Code learned successfully", PK_Text);
						SendCommand(CMD_Set_Text);
					}
					else
					{
						string sMsg = pronto_result + "\n";
						send(learn_client, sMsg.c_str(), sMsg.length(), 0);
					}
				}
				else
				{
					LoggerWrapper::GetInstance()->Write(LV_WARNING, "'Learned' event not sent because GC-IRL indicated error");
					if (pLearningInfo)
					{
						DCE::CMD_Set_Text CMD_Set_Text(m_dwPK_Device, PK_Device_Orbiter, "", string("'Learned' event not sent because GC-IRL indicated error: ") + ErrorMsg, PK_Text);
						SendCommand(CMD_Set_Text);
					}
					else
					{
						string sMsg = "GC100 ERROR\n";
						send(learn_client, sMsg.c_str(), sMsg.length(), 0);
					}
				}
			}
		}
		else
		{
			if (! m_bStopLearning && ! m_bQuit_get())
			{
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "Timeout");
				if (pLearningInfo)
				{
					DCE::CMD_Set_Text CMD_Set_Text(m_dwPK_Device, PK_Device_Orbiter, "", "Learning timed out", PK_Text);
					SendCommand(CMD_Set_Text);
				}
				else
				{
					string sMsg = "TIMEOUT\n";
					send(learn_client, sMsg.c_str(), sMsg.length(), 0);
				}
			}
		}
		close(learn_fd);
	} // end if is_open_for_learning
	else
	{
		if (pLearningInfo)
		{
			DCE::CMD_Set_Text CMD_Set_Text(m_dwPK_Device, PK_Device_Orbiter, "", "Failed to open learning device", PK_Text);
			SendCommand(CMD_Set_Text);
		}
		else
		{
			string sMsg = "CAN'T OPEN\n";
			send(learn_client, sMsg.c_str(), sMsg.length(), 0);
		}
	}
	m_bLearning = false;
	m_bStopLearning = false;

#pragma warning("TODO: generate screens, complete this function call")
	//if (pLearningInfo->m_PK_Device_Orbiter)
	//{
	//	DCE::CMD_Goto_DesignObj CMD_Goto_DesignObj(m_dwPK_Device, pLearningInfo->m_PK_Device_Orbiter,
	//		bLearnedCode ? DESIGNOBJ_learned : DESIGNOBJ_failed_to_learn);
	//	pLearningInfo->m_pgc100->SendCommand(CMD_Goto_DesignObj);
	//}
	
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Learning thread finished");
}

void gc100::ReportChildren()
{
	if (DATA_Get_Dont_Auto_Configure())
		return;
	// Report children to the DCE router
	// internal_id \t description \t room_name \t PK_DeviceTemplate \t floorplan_id \t PK_DeviceData \t DeviceData_value ... \n
	string sChildren;
	for (std::map<std::string, class module_info>::const_iterator It = module_map.begin(); It != module_map.end(); It++)
	{
		const int bsize = 4096;
		char buffer[bsize];
		memset(buffer, 0, bsize);

		int PK_DeviceTemplate;
		if (It->second.type == "IR")
			PK_DeviceTemplate = DEVICETEMPLATE_Generic_IR_Transmitter_CONST;
		else if (It->second.type == "SENSOR")
			PK_DeviceTemplate = DEVICETEMPLATE_Generic_Input_Ouput_CONST;
		else if (It->second.type == "RELAY")
			PK_DeviceTemplate = DEVICETEMPLATE_Generic_Relays_CONST;
		else
			continue;

		snprintf(buffer, bsize - 1, "%s\t%s %s\t\t%d\n",
			It->first.c_str(), It->second.type.c_str(), It->second.key.c_str(), PK_DeviceTemplate);
		sChildren += buffer;
	}
	cout << "Children to report: " << endl << sChildren << endl << "End of children list" << endl;
	EVENT_Reporting_Child_Devices("", sChildren);
}

// Get a child object for the given Device ID
Command_Impl *gc100::GetChild(int dwPK_Device)
{
	MapCommand_Impl::iterator child_iter;
	child_iter = m_mapCommandImpl_Children.find(dwPK_Device);
	if (child_iter != m_mapCommandImpl_Children.end())
		return child_iter->second;
	return NULL;
}

// Must override so we can call IRBase::Start() after creating children
void gc100::CreateChildren()
{
	string device_data;

	gc100_Command::CreateChildren();
	Start();

	send_to_gc100("getdevices");

	do
	{
		struct timeval tv;
		tv.tv_sec = 5;
		tv.tv_usec = 0;
		device_data = read_from_gc100(&tv);
		if (device_data == "error") {
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed to get devices from gc100");
			exit(2);
		}
	} while (device_data != "endlistdevices");

	Start_seriald(); // Start gc_seriald processes according to serial port inventory
	Sleep(1000);
//	is_open_for_learning = open_for_learning();

	if (pthread_create(&m_EventThread, NULL, StartEventThread, (void *) this))
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed to create Event Thread");
		m_bQuit_set(true);
		exit(1);
	}

	if (pthread_create(&m_SocketThread, NULL, StartSocketThread, (void *) this))
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Failed to create Socket Thread");
	}
}

void gc100::EventThread()
{
	while (! m_bQuit_get())
	{
		Sleep(50);
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"EventThread");
		if(read_from_gc100() == "error")
		{
			close(gc100_socket);
			Open_gc100_Socket();
		}
	}
}

void gc100::LEARN_IR_CANCEL()
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Telling learning thread to stop: %d", m_bLearning);
	if (m_bLearning)
	{
		m_bStopLearning = true;
	}
}

void gc100::SetQuitFlag()
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Setting 'quit' flag");
	m_bQuit_set(true);
}

void gc100::SocketThread(int port)
{
	struct sockaddr_in saddr;
	const int buff_size = 4096;
	char buffer[buff_size], * parm;
	int count, ret;

	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = INADDR_ANY;

	// TODO: check for errors
	learn_server = socket(PF_INET, SOCK_STREAM, 0);
	{
        int x = 1;
        setsockopt(learn_server, SOL_SOCKET, SO_REUSEADDR, (char *)&x, sizeof(x));
	}
	bind(learn_server, (struct sockaddr *) &saddr, sizeof(saddr));
	listen(learn_server, 5);

	while (! m_bQuit_get())
	{
		learn_client = accept(learn_server, NULL, NULL);
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Accepted client on socket");
		while (! m_bQuit_get()&& (count = recv(learn_client, buffer, buff_size - 1, 0)) > 0)
		{
			buffer[count] = 0;
			if (strncmp("LEARN", buffer, 5) == 0)
			{
				ret = LEARN_IR_via_Socket();
				if (ret == 0)
				{
					while (m_bLearning)
					{
						Sleep(1000);
					}
				}
			}
			else if (strncmp("GC-PRONTO ", buffer, 10) == 0)
			{
				string sMsg;
				
				parm = buffer + 10;
				sMsg = gc100_irl::IRL_to_pronto(parm) + "\n";
				send(learn_client, sMsg.c_str(), sMsg.length(), 0);
			}
			else if (strncmp("PRONTO-GC ", buffer, 10) == 0)
			{
				string sMsg;
				
				parm = buffer + 10;
				ret = ConvertPronto(parm, sMsg);
				if (ret)
				{
					sMsg += "\n";
					send(learn_client, sMsg.c_str(), sMsg.length(), 0);
				}
				else
				{
					sMsg = "ERROR\n";
					send(learn_client, sMsg.c_str(), sMsg.length(), 0);
				}
			}
			else if (strncmp("QUIT", buffer, 4) == 0)
			{
				break;
			}
			else
			{
				string sMsg = "UNKNOWN\n";
				send(learn_client, sMsg.c_str(), sMsg.length(), 0);
			}
		
		}
		close(learn_client);
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Socket client disconnected");
	}
}
//<-dceag-createinst-b->!
//<-dceag-c191-b->

	/** @brief COMMAND: #191 - Send Code */
	/** Sends an I/R code to a device. */
		/** @param #9 Text */
			/** The I/R code -- usually in Pronto format */

void gc100::CMD_Send_Code(string sText,string &sCMD_Result,Message *pMessage)
//<-dceag-c191-e->
{
	int iRepeat = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Repeat_CONST].c_str());
	if (iRepeat < 1)
		iRepeat = 1;
	if (iRepeat > 3)
		iRepeat = 3;

	if( sText.size() && (sText[0]=='5' || sText[0]=='6') )
	{
		string sTextNew = ConvertRC5_6(sText);
LoggerWrapper::GetInstance()->Write(LV_STATUS,"Converted %s to %s",sText.c_str(),sTextNew.c_str());
sText=sTextNew;
	}
	SendIR("",sText,iRepeat);
}

//<-dceag-c194-b->

	/** @brief COMMAND: #194 - Toggle Power */
	/** Set relay state (0 or 1) */
		/** @param #8 On/Off */
			/** Depending on each device On/Off can be interpreted differently, but in genereal On/Off has a value of 1 for on and 0 for Off */

void gc100::CMD_Toggle_Power(string sOnOff,string &sCMD_Result,Message *pMessage)
//<-dceag-c194-e->
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

void gc100::CMD_Learn_IR(int iPK_Device,string sOnOff,int iPK_Text,int iPK_Command,string &sCMD_Result,Message *pMessage)
//<-dceag-c245-e->
{
// That's why we should implement this here instead of ReceivedCommandForChild (learn comes to gc100, cancel to tv)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
// 13      10/26/05 6:13:05.832            TCPIP: Accepting incoming connection on socket 146, port 3450, from IP 127.0.0.1.
// 08      10/26/05 6:13:05.832            Received Message from 0 (unknown / ) to 35 (gc100 / hybrid), type 1 id 245 Command:Learn IR, parameters:
// 08      10/26/05 6:13:05.833              Parameter 8(On/Off): 1
// 08      10/26/05 6:13:05.833              Parameter 71(PK_Command_Input(Skip Back - Channel/Track Lower)): 64
// 13      10/26/05 6:13:05.833            No response expected.  returning
// 10      10/26/05 6:13:05.885            Socket::ReceiveData failed, bytes left 0 m_Socket: 146 Incoming_Conn Socket 146 127.0.0.1
// 10      10/26/05 6:13:05.885            Socket::ReceiveString2 ReceiveData failed m_Socket: -1 Incoming_Conn Socket 146 127.0.0.1
// 05      10/26/05 6:13:05.885            TCPIP: Closing connection to -1003 (Router Dev #0) 0x8590e30 m_Socket: -1
// 13      10/26/05 6:13:08.134            TCPIP: Accepting incoming connection on socket 146, port 3450, from IP 127.0.0.1.
// 08      10/26/05 6:13:08.135            Received Message from 0 (unknown / ) to 36 (KV-X2501D / hybrid), type 1 id 245 Command:Learn IR, parameters:
// 08      10/26/05 6:13:08.135              Parameter 8(On/Off): 0
// 13      10/26/05 6:13:08.136            No response expected.  returning
	cout << "###" << endl;
	cout << "Cmd: Learn IR '" << sOnOff << "' == '" << atoi(sOnOff.c_str()) << "'" << endl;
	cout << "C: " << (atoi(sOnOff.c_str()) != 0) << endl;
	if (atoi(sOnOff.c_str()) != 0)
		LEARN_IR(iPK_Device, iPK_Command, pMessage->m_dwPK_Device_From, iPK_Text);
	else
		LEARN_IR_CANCEL();
	sCMD_Result = "OK";
	return;
}

string gc100::GetIpAddress( int dwPK_Device )
{
	// If this is for us and the IP address is already known, just pass it
	if( (dwPK_Device==0 || dwPK_Device==m_pData->m_dwPK_Device) && m_pData->m_sIPAddress.empty()==false )
		return m_pData->m_sIPAddress;

	// Get it from the server
	if( !m_pcRequestSocket )
		return "";

	string sResult;
	m_pcRequestSocket->m_pClientSocket->SendString("GET_IP " + StringUtils::itos( dwPK_Device ? dwPK_Device : m_dwPK_Device ) );
	m_pcRequestSocket->m_pClientSocket->ReceiveString(sResult);
	return sResult;
}
