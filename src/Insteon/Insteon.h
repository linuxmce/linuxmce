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
#ifndef Insteon_h
#define Insteon_h

//	DCE Implemenation for #1899 Insteon

#include "Gen_Devices/InsteonBase.h"
//<-dceag-d-e->

#include "LMCEInsteonAPI.h"

//<-dceag-decl-b->
namespace DCE
{
	class Insteon : public Insteon_Command
	{
//<-dceag-decl-e->
	
			// Private member variables
		/** mutex for Insteon tasks.*/
	    pluto_pthread_mutex_t m_InsteonMutex;
		
		/** mutex to force the pooling job to run.*/
	    pluto_pthread_mutex_t m_PoolingMutex;
		
		// mutex attribute
	    pthread_mutexattr_t m_MutexAttr;	// Private member variables

		/** Insteon API.*/
		LMCEInsteonAPI* m_InsteonAPI;

		/* Handle to the Smarthome PLC device */
		ilib_t * plc_handle;

		/** Keeps the name for serial device used by ZWave.*/
		static string insteonSerialDevice;

		/** Flag used by the asynch thread.*/
		static bool m_AsynchStarted;
		
		/** Asynchronous jobs management thread */
		static pthread_t m_AsynchThread;

		// Private methods
		/** Starts or stops the Insteon asynchronous jobs management thread.*/
		void asynchThread(bool start);
		
public:		
		/** Checks serial connection and performs Insteon initialization.*/
		bool ConfirmConnection();

		/** Listening asynchronously for Insteon answers.*/
		void AsynchInsteon();


public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Insteon(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Insteon();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		Insteon(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	bool DATA_Get_Only_One_Per_PC();
	bool DATA_Get_Autoassign_to_parents_room();

			*****EVENT***** accessors inherited from base class
	void EVENT_Device_OnOff(bool bOnOff);
	void EVENT_Reporting_Child_Devices(string sError_Message,string sText);
	void EVENT_Download_Config_Done(string sError_Message);

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #756 - Report Child Devices */
	/** Report all the child sensors this has by firing an event 'Reporting Child Devices' */

	virtual void CMD_Report_Child_Devices() { string sCMD_Result; CMD_Report_Child_Devices(sCMD_Result,NULL);};
	virtual void CMD_Report_Child_Devices(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #760 - Send Command To Child */
	/** After reporting new child devices, there may be children we want to test, but we haven't done a quick reload router and can't send them messages directly.  This way we can send 'live' messages to children */
		/** @param #10 ID */
			/** The internal ID used for this device--not the Pluto device ID. */
		/** @param #154 PK_Command */
			/** The command to send */
		/** @param #202 Parameters */
			/** Parameters for the command in the format:
PK_CommandParameter|Value|... */

	virtual void CMD_Send_Command_To_Child(string sID,int iPK_Command,string sParameters) { string sCMD_Result; CMD_Send_Command_To_Child(sID.c_str(),iPK_Command,sParameters.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Send_Command_To_Child(string sID,int iPK_Command,string sParameters,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #776 - Reset */
	/** Reset device. */
		/** @param #51 Arguments */
			/** Argument string
NOEMON or CANBUS */

	virtual void CMD_Reset(string sArguments) { string sCMD_Result; CMD_Reset(sArguments.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Reset(string sArguments,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #788 - StatusReport */
	/** Test comand. Asq a report */
		/** @param #51 Arguments */
			/** Argument string */

	virtual void CMD_StatusReport(string sArguments) { string sCMD_Result; CMD_StatusReport(sArguments.c_str(),sCMD_Result,NULL);};
	virtual void CMD_StatusReport(string sArguments,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #920 - Add Insteon Device to Group */
	/** Create a group and add the specified devices to the group number (stored in PLC adapter) */
		/** @param #249 Group ID */
			/** The Insteon Group to which to add the node */
		/** @param #250 Nodes List */
			/** The list of associated Z-Wave nodes (comma separated) */

	virtual void CMD_Add_Insteon_Device_to_Group(int iGroup_ID,string sNodes_List) { string sCMD_Result; CMD_Add_Insteon_Device_to_Group(iGroup_ID,sNodes_List.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Add_Insteon_Device_to_Group(int iGroup_ID,string sNodes_List,string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
