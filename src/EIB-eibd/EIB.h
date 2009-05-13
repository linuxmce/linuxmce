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
#ifndef EIB_h
#define EIB_h

//	DCE Implemenation for #49 EIB

#include "Gen_Devices/EIBBase.h"
//<-dceag-d-e->


#include "eibclient.h"
#include "Telegram.h"
#include "knxDevice.h"
#include "DCE/Logger.h"

#include <map>
using namespace std;
using namespace knx;

#define EIBD_SOCKET_URL "local:/tmp/eib"
#define MAX_TEMPSTR_LENGTH	15
 //after that number of  errors while trying to conenct, aborting.
#define EIBMAXCONNEXIONERROR 20
	//time after an EIBconnection error before we start another one. increase it in case your comp is slow
#define EIBERRORDELAY 5

//<-dceag-decl-b->
namespace DCE
{
	class EIB : public EIB_Command
	{
//<-dceag-decl-e->

public:
		// Public member variables


//<-dceag-const-b->
public:
		// Constructors/Destructor
		EIB(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~EIB();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		EIB(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_bus_type();
	string DATA_Get_bus_address();

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/


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

	virtual void CMD_EIB_Write(string sAddress,string sData_String,int iDataType) { string sCMD_Result; CMD_EIB_Write(sAddress.c_str(),sData_String.c_str(),iDataType,sCMD_Result,NULL);};
	virtual void CMD_EIB_Write(string sAddress,string sData_String,int iDataType,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #275 - EIB_Read */
	/** Performs a READ request on the bus */
		/** @param #108 Address */
			/** EIB Group Address */

	virtual void CMD_EIB_Read(string sAddress) { string sCMD_Result; CMD_EIB_Read(sAddress.c_str(),sCMD_Result,NULL);};
	virtual void CMD_EIB_Read(string sAddress,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #350 - Process Incoming Data */
	/** This Internal command is sent to Ruby interpreter when data is availabe on input. Is used only in Generic Serial Devices. */

	virtual void CMD_Process_Incoming_Data() { string sCMD_Result; CMD_Process_Incoming_Data(sCMD_Result,NULL);};
	virtual void CMD_Process_Incoming_Data(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #351 - Process IDLE */
	/** This Internal command is sent to Ruby interpreter when it is in IDLE state. */

	virtual void CMD_Process_IDLE() { string sCMD_Result; CMD_Process_IDLE(sCMD_Result,NULL);};
	virtual void CMD_Process_IDLE(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #355 - Process Initialize */
	/** This Internal command is sent to Ruby interpreter when initialize occurs. */

	virtual void CMD_Process_Initialize() { string sCMD_Result; CMD_Process_Initialize(sCMD_Result,NULL);};
	virtual void CMD_Process_Initialize(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #356 - Process Release */
	/** This Internal command is sent to Ruby interpreter when release occurs. */

	virtual void CMD_Process_Release() { string sCMD_Result; CMD_Process_Release(sCMD_Result,NULL);};
	virtual void CMD_Process_Release(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #373 - Private Method Listing */
	/** Used for ruby code mapping where user can add several private helper members. */

	virtual void CMD_Private_Method_Listing() { string sCMD_Result; CMD_Private_Method_Listing(sCMD_Result,NULL);};
	virtual void CMD_Private_Method_Listing(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #384 - Process Receive Command For Child */
	/** Method that will be called when command arrives for child device */

	virtual void CMD_Process_Receive_Command_For_Child() { string sCMD_Result; CMD_Process_Receive_Command_For_Child(sCMD_Result,NULL);};
	virtual void CMD_Process_Receive_Command_For_Child(string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->
public:
	//required for spawning the childs. not supposed to be used by outside though..

	void SpawnEibd();

	void ListenEibd();


protected:
	EIBConnection *con;//connection to the eibd
	
	pthread_mutex_t mutexCon;//mutex to be sure the connexion is well shared.
	
	//is set to 0 at the cration, whenever a thread gets a stop order, it sets this to true. when that is true, threads stop
	bool _bhasToStop;

	//the eibd spawner sets it to true/false, according to the eibd state. no need to try to establish a connection while it remains false;
	bool _beibdStarted;

	//the eibd spawner sets that to true when it spawns eibd. connexion manager sets that to false once it has successfully started
	bool _bfirstcon;
	// the same as above, but for the read() command
	bool _bisreaddone;

	pthread_t spawnerThread,listenerThread;//the thread_id of the swpanderand the listener

	//the string reprensenting the gateway to get knx.
	string sgateway;

	//the map of knxDevices
	map<int, knxDevice*> childrenmap;

//protected functions
protected:
	//get the gateway string from the params given by DCE
	void getAway();

	void populate_children();

	//starts the thread to keep eibd on
	void startEibdSpawner();

	//starts the thread to listen to incomming knx datagrams
	void startEibdListener();
	//tests if the telegram accords to the devicedata and acts according to the result
	bool processTelegram(const Telegram *pt);

	//regenerates the connection to eibd
	void regenerateCon();

	//what we do in case we have to crash the EIB
	void crash(bool withexit=true);

	bool sendTelegram(const Telegram *pt);
	};
//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
