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
#include "EIB.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
EIB::EIB(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: EIB_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
	con=0;
	_beibdStarted=true;
	_bhasToStop=false;
	_bfirstcon=true;
	_bisreaddone=false;
	pthread_mutex_init(&mutexCon,NULL);
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
EIB::EIB(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: EIB_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
}

//<-dceag-dest-b->
EIB::~EIB()
//<-dceag-dest-e->
{
	_bhasToStop=true;
	crash(false);
	for(map<int, knxDevice*>::iterator it= childrenmap.begin();it!=childrenmap.end();it++)
		delete(it->second);
	childrenmap.clear();
}

//<-dceag-getconfig-b->
bool EIB::GetConfig()
{
	if( !EIB_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"EIB GetConfig done");

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated

	//populate the map of knxDevices
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"EIB populating");
	populate_children();
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"  EIB populating done");
	
	//we start the threads needed to handle eibd
	startEibdListener();

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"EIB threadspawning done");
	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool EIB::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
EIB_Command *Create_EIB(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new EIB(pPrimaryDeviceCommand, pData, pEvent, pRouter);
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
void EIB::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	sCMD_Result = "UNHANDLED CHILD";
	map<int, knxDevice* >::iterator it=childrenmap.find(pDeviceData_Impl->m_dwPK_Device);
	if(it==childrenmap.end() ) return;
	knxDevice *devicefound=it->second;
	sCMD_Result="OK";
	Telegram *ptel=NULL;
	vector<Telegram *> aptel;
	switch(pMessage->m_dwID)
	{
		case COMMAND_Read_CONST:
		{
			vector<Telegram> v_tel=devicefound->Command_Read();
			for(unsigned int i=0;i < v_tel.size(); i++) sendTelegram(&v_tel[i]);
			return;
		}break;
		case COMMAND_Generic_On_CONST: ptel=devicefound->Command_On() ; break;
		case COMMAND_Generic_Off_CONST: ptel=devicefound->Command_Off() ; break;
		case COMMAND_Open_CONST: ptel=devicefound->Command_Open() ; break;
		case COMMAND_Close_CONST: ptel=devicefound->Command_Close() ; break;
		case COMMAND_Stop_CONST: ptel=devicefound->Command_Stop() ; break;
		case COMMAND_Set_Level_CONST:
		{
			int level= ((unsigned int)atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Level_CONST].c_str())*255)/100;
			ptel=devicefound->Command_Set_Level(level) ;
		}break;
		case COMMAND_Set_Color_RGB_CONST:
		{
			int R = ((unsigned int)atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Red_Level_CONST].c_str())*255)/100;
			int G = ((unsigned int)atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Green_Level_CONST].c_str())*255)/100;
			int B = ((unsigned int)atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Blue_Level_CONST].c_str())*255)/100;
			aptel=devicefound->Command_Set_Color(R,G,B) ;
		}break;
		case COMMAND_Set_Angle_Percent_CONST:
		{
			int angle= ((unsigned int)atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Angle_Percent_CONST].c_str())*255)/100;
			ptel=devicefound->Command_Set_Angle_Percent(angle) ;
		}break;
		case COMMAND_Set_Mode_CONST:
		{
			int mode=(unsigned int)atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Mode_CONST].c_str());
			ptel=devicefound->Command_Set_Mode(mode) ;
		}break;
		case COMMAND_Set_Temperature_CONST:
		{
			float ftemp=atof(pMessage->m_mapParameters[COMMANDPARAMETER_Value_To_Assign_CONST].c_str());
			ptel=devicefound->Command_Set_Temperature(ftemp) ;
		}break;
	}
	// check if there is no command to send (ptel = NULL and array of ptel, aptel is empty)
	if(ptel==NULL && aptel.size() == 0)
	{ 
		sCMD_Result = "UNHANDLED CHILD"; 
	}
	else if(ptel != NULL)
	{
		string aff=string("[")+StringUtils::itos(ptel->getShortUserData())+string("]");
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "  KNX: sending Telegram from: %s; to: %s; type: %s shortdata %s",
								Telegram::paddrtostring(ptel->getSrcAddress()).c_str(),
								Telegram::gaddrtostring(ptel->getGroupAddress()).c_str(),
								ptel->decodeType().c_str(),
								aff.c_str());
		sendTelegram(ptel);
		// cleanup regarding http://forum.linuxmce.org/index.php?topic=5648.msg57082#msg57082
		delete(ptel);
	}
	else if(aptel.size() > 0)
	{
		int i=0;
		int j=aptel.size();
		for(i=0; i < j; i++)
		{
			ptel = aptel.at(i);
			if(ptel != NULL)
			{
				string aff=string("[")+StringUtils::itos(ptel->getShortUserData())+string("]");
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "  KNX: sending Telegram %d/%d from: %s; to: %s; type: %s shortdata %s",
									i+1,
									j,
									Telegram::paddrtostring(ptel->getSrcAddress()).c_str(),
									Telegram::gaddrtostring(ptel->getGroupAddress()).c_str(),
									ptel->decodeType().c_str(),
									aff.c_str());
				sendTelegram(ptel);
				delete(ptel);
				aptel.at(i)=NULL;
			}
		}
		// Clear objects from vector ...
		aptel.clear();
		// ... and trim vector to size 0
		vector<Telegram *>().swap(aptel); 
	}
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void EIB::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN COMMAND";
}


//<-dceag-sample-b->!
/*		**** SAMPLE ILLUSTRATING HOW TO USE THE BASE CLASSES ****

**** IF YOU DON'T WANT DCEGENERATOR TO KEEP PUTTING THIS AUTO-GENERATED SECTION ****
**** ADD AN ! AFTER THE BEGINNING OF THE AUTO-GENERATE TAG, LIKE //<=dceag-sample-b->! ****
Without the !, everything between <=dceag-sometag-b-> and <=dceag-sometag-e->
will be replaced by DCEGenerator each time it is run with the normal merge selection.
The above blocks are actually <- not <=.  We don't want a substitution here
*/
//<-dceag-sample-e->

/*

	COMMANDS TO IMPLEMENT

*/

//<-dceag-c273-b->

	/** @brief COMMAND: #273 - EIB_Write */
	/** Peforms a WRITE request on the bus */
		/** @param #108 Address */
			/** EIB Group Address */
		/** @param #109 Data String */
			/** EIB Data */
		/** @param #110 DataType */
			/** EIB Data Type:
[1]-switch (1b)
[2]-dimming-control (4b)
[3]-time (3B)
[4]-date (4B)
[5]-value (2B)
[6]-scaling (1B)
[7]-drive control
[9]-float
[10]-16bit counter
[11]-32bit counter
[13]-ASCII character
[14]-8bit counter
[15]-character string */

void EIB::CMD_EIB_Write(string sAddress,string sData_String,int iDataType,string &sCMD_Result,Message *pMessage)
//<-dceag-c273-e->
{
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Need to implement command eib_write");
	cout << "Need to implement command #273 - EIB_Write" << endl;
	cout << "Parm #108 - Address=" << sAddress << endl;
	cout << "Parm #109 - Data_String=" << sData_String << endl;
	cout << "Parm #110 - DataType=" << iDataType << endl;
}

//<-dceag-c275-b->

	/** @brief COMMAND: #275 - EIB_Read */
	/** Performs a READ request on the bus */
		/** @param #108 Address */
			/** EIB Group Address */

void EIB::CMD_EIB_Read(string sAddress,string &sCMD_Result,Message *pMessage)
//<-dceag-c275-e->
{
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Need to implement command eib_read");
	cout << "Need to implement command #275 - EIB_Read" << endl;
	cout << "Parm #108 - Address=" << sAddress << endl;
}

//<-dceag-c350-b->

	/** @brief COMMAND: #350 - Process Incoming Data */
	/** This Internal command is sent to Ruby interpreter when data is availabe on input. Is used only in Generic Serial Devices. */

void EIB::CMD_Process_Incoming_Data(string &sCMD_Result,Message *pMessage)
//<-dceag-c350-e->
{
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Need to implement command process_incoming_data");
	cout << "Need to implement command #350 - Process Incoming Data" << endl;
}

//<-dceag-c351-b->

	/** @brief COMMAND: #351 - Process IDLE */
	/** This Internal command is sent to Ruby interpreter when it is in IDLE state. */

void EIB::CMD_Process_IDLE(string &sCMD_Result,Message *pMessage)
//<-dceag-c351-e->
{
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Need to implement command process_idle");
	cout << "Need to implement command #351 - Process IDLE" << endl;
}

//<-dceag-c355-b->

	/** @brief COMMAND: #355 - Process Initialize */
	/** This Internal command is sent to Ruby interpreter when initialize occurs. */

void EIB::CMD_Process_Initialize(string &sCMD_Result,Message *pMessage)
//<-dceag-c355-e->
{
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Need to implement command proces_initialize");
	cout << "Need to implement command #355 - Process Initialize" << endl;
}

//<-dceag-c356-b->

	/** @brief COMMAND: #356 - Process Release */
	/** This Internal command is sent to Ruby interpreter when release occurs. */

void EIB::CMD_Process_Release(string &sCMD_Result,Message *pMessage)
//<-dceag-c356-e->
{
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Need to implement command #356 - Process release");
	cout << "Need to implement command #356 - Process Release" << endl;
}

//<-dceag-c373-b->

	/** @brief COMMAND: #373 - Private Method Listing */
	/** Used for ruby code mapping where user can add several private helper members. */

void EIB::CMD_Private_Method_Listing(string &sCMD_Result,Message *pMessage)
//<-dceag-c373-e->
{
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Need to implement command #373 - pricate method listing");
	cout << "Need to implement command #373 - Private Method Listing" << endl;
}

//<-dceag-c384-b->

	/** @brief COMMAND: #384 - Process Receive Command For Child */
	/** Method that will be called when command arrives for child device */

void EIB::CMD_Process_Receive_Command_For_Child(string &sCMD_Result,Message *pMessage)
//<-dceag-c384-e->
{
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Need to implement command #384 - Process Receive Command For Child");
	cout << "Need to implement command #384 - Process Receive Command For Child" << endl;
}


//start the eibd listener
extern "C" void *launchListen( void* );

void *launchListen(void *pt)
{
	EIB *base = static_cast<EIB*>(pt);
	base->ListenEibd();
	return NULL;
}

void EIB::startEibdListener()
{
	pthread_create(&listenerThread, NULL, launchListen, (void*)this);
}

void EIB::ListenEibd()
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "starting listening thread");
	Telegram tl;
	int received=0;
	while (true)
	{
		//if we have to leave.. we leave
		if(_bhasToStop)
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "  listening thread has to stop now");
			return ;
		}

		if(!_bisreaddone && _beibdStarted && con!=0)//first thing to do on connection
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"send read requests");
			_bisreaddone=true;
			for(map<int, knxDevice * >::iterator it=childrenmap.begin();it!=childrenmap.end();it++)
			{
				int ierrors=0;
				int isent=0;
				vector<Telegram> v_tel = it->second->Command_Read();
				for(unsigned int i=0;i<v_tel.size();i++)
					if (sendTelegram(&v_tel[i]))
						isent++;
					else ierrors++;
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"  device type %s sent %i Telegram, %i errors",it->second->stype().c_str(),isent, ierrors);
			}
		}

		pthread_mutex_lock (&mutexCon);
		while(con==0)//need a valid connection to discuss through
		{
			pthread_mutex_unlock (&mutexCon);	//we need to unlock it to regenerate it
			regenerateCon();
			pthread_mutex_lock (&mutexCon);
		}

		//is there a message on the queue?
		received=EIB_Poll_Complete(con);
		switch(received)
		{
			case(-1):
			{//error
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "connection loss while listening");
				pthread_mutex_unlock (&mutexCon);
				regenerateCon();
				pthread_mutex_lock (&mutexCon);
			}//no breack here: we need to unlock the mutex
			case(0)	:
			{//no data to receive
				pthread_mutex_unlock (&mutexCon);
				// nothing to do, sleep a bit to not hog the cpu
				usleep(50);
			}break;
			default	:
			{//data to receive
				tl.receivefrom(con);
				pthread_mutex_unlock (&mutexCon);
				string aff=string("[")+StringUtils::itos(tl.getShortUserData())+string("]");
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "Processing received Telegram from: %s; to: %s; type: %s shortdata %s",
								Telegram::paddrtostring(tl.getSrcAddress()).c_str(),
								Telegram::gaddrtostring(tl.getGroupAddress()).c_str(),
								tl.decodeType().c_str(),
								aff.c_str());
				processTelegram(&tl);
			}
		}
	}
}

void EIB::populate_children()
{
	VectDeviceData_Impl& vDeviceData = m_pData->m_vectDeviceData_Impl_Children;
	for(VectDeviceData_Impl::size_type i = 0; i < vDeviceData.size(); i++)
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "creating child device:%i",vDeviceData[i]->m_dwPK_Device);
		knxDevice *child=knxDevice::getKnxDevice(vDeviceData[i]);
		if(child!=NULL)
		{
			childrenmap[vDeviceData[i]->m_dwPK_Device]=child;
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "   child created: %s",child->stype().c_str());
		}
	}
}

bool EIB::processTelegram(const Telegram *tl)
{
	bool retour=false;
	for(map<int, knxDevice*>::iterator it= childrenmap.begin();it!=childrenmap.end();it++)
		if(it->second!=NULL)
		{
			Message *p_mes =it->second->handleTelegram(tl);
			if(p_mes!=NULL)
			{
				retour=true;
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "  Telegram handles by child %s",it->second->stype().c_str());
				if(!GetEvents()->SendMessage(p_mes))
					LoggerWrapper::GetInstance()->Write(LV_WARNING, "Error sending Event.");
			}
		}
	return true;
}

void EIB::regenerateCon()
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"regenerating the connection");
	int error_count=0;

	pthread_mutex_lock (&mutexCon);
	if(con!=0) EIBClose(con);
	con=0;
	pthread_mutex_unlock (&mutexCon);
	_bisreaddone=false;

	//we make return() in this 
	while(! _bhasToStop)
	{
		//we wait for the eibd to be started-and when it just started we wait again once
		while(!_beibdStarted)
		{
			sleep(EIBERRORDELAY);	//we can't regenerate connection till the eibd is started..
		}
		if( _bfirstcon) sleep(EIBERRORDELAY);
		_bfirstcon=false;
		
		pthread_mutex_lock (&mutexCon);
		if(con!=0)//maybe another programm has set it to correct. then we just leave
		{
			pthread_mutex_unlock (&mutexCon);
			return;
		}else{	//the connection is still undone: we set it
			con = EIBSocketURL (EIBD_SOCKET_URL);
			if(con!=0)
			{
				if (EIBOpen_GroupSocket (con, 0) == -1)
				{
					error_count++;
					LoggerWrapper::GetInstance()->Write(LV_WARNING,"  error while creating groupsocket, error count:",error_count);
					EIBClose(con);
					con=0;
				}else{
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"  connection received socket: %is",con);
					_bisreaddone=false;//we need to send read again
					pthread_mutex_unlock (&mutexCon);
					return;
				}
			}else{
				error_count++;
				LoggerWrapper::GetInstance()->Write(LV_WARNING,"  error while opening eibd port, error count:",error_count);
			}
		}
		pthread_mutex_unlock (&mutexCon);
		
		if(error_count>=EIBMAXCONNEXIONERROR)
		{
			crash();
		}
		sleep(EIBERRORDELAY);
	}
}

void EIB::crash(bool withexit)
{
	_bhasToStop=true;
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "the eib has to crash now. forcing children threads to crash, withexit:%i",withexit);
	pthread_mutex_lock (&mutexCon);
	if(con!=0) EIBClose(con);	//crash the connection to force the listener to exit
	con=0;
	pthread_mutex_unlock (&mutexCon);
	pthread_join(listenerThread, NULL);
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "  listener child crashed");
	if (withexit) exit(1);
	return;
}

bool EIB::sendTelegram(const Telegram *pt)
{
	pthread_mutex_lock (&mutexCon);
	while(con==0)//need a valid connection to discuss through
	{
		pthread_mutex_unlock (&mutexCon);	//we need to unlock it to regenerate it
		regenerateCon();
		pthread_mutex_lock (&mutexCon);
	}
	bool ret=pt->sendTo(con);
	pthread_mutex_unlock (&mutexCon);
	return ret;
}
