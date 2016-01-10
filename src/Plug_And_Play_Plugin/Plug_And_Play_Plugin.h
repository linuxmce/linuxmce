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
#ifndef Plug_And_Play_Plugin_h
#define Plug_And_Play_Plugin_h

//	DCE Implemenation for #1809 Plug And Play Plug-in

#include "Gen_Devices/Plug_And_Play_PluginBase.h"
//<-dceag-d-e->

class Database_pluto_main;

//<-dceag-decl-b->
namespace DCE
{
	class Plug_And_Play_Plugin : public Plug_And_Play_Plugin_Command
	{
//<-dceag-decl-e->
		// Private member variables
		friend class PnpQueue;
		friend class Pnp_PreCreateOptions;

		class PnpQueue *m_pPnpQueue;
		Database_pluto_main *m_pDatabase_pluto_main;
		pluto_pthread_mutex_t m_PnpMutex;
	    pthread_mutexattr_t m_MutexAttr;
		pthread_cond_t m_PnpCond;
	    class Orbiter_Plugin *m_pOrbiter_Plugin;
		class Datagrid_Plugin *m_pDatagrid_Plugin;
		bool m_bSuspendProcessing;
		int m_iPK_PnpQueue_Starting;  // The last queue id when this device started, so we know which entries are new vs. old

		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Plug_And_Play_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Plug_And_Play_Plugin();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->
		virtual void PrepareToDelete();

//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		Plug_And_Play_Plugin(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

		bool DeviceDetected( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );
		bool DeviceRemoved( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );
		bool DoneDetectingDevices( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );

		class DataGridTable *PNPDevices( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );

		bool m_bSuspendProcessing_get() { return m_bSuspendProcessing; }
		class Orbiter_Plugin *m_pOrbiter_Plugin_get() { return m_pOrbiter_Plugin; }

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #192 - On */
	/** Start processing any incoming alerts again */
		/** @param #97 PK_Pipe */
			/** Normally when a device is turned on all the inputs and outputs are selected automatically.  If this parameter is specified, only the settings along this pipe will be set. */
		/** @param #98 PK_Device_Pipes */
			/** Normally when a device is turned on the corresponding "pipes" are enabled by default. if this parameter is blank.  If this parameter is 0, no pipes will be enabled.  This can also be a comma seperated list of devices, meaning only the pipes to those devic */

	virtual void CMD_On(int iPK_Pipe,string sPK_Device_Pipes) { string sCMD_Result; CMD_On(iPK_Pipe,sPK_Device_Pipes.c_str(),sCMD_Result,NULL);};
	virtual void CMD_On(int iPK_Pipe,string sPK_Device_Pipes,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #193 - Off */
	/** Stop processing incoming pnp alerts unless they have 'auto create without prompting'.  Used during the video wizard when there are no rooms/users */
		/** @param #97 PK_Pipe */
			/** Normally when a device is turned on all the inputs and outputs are selected automatically.  If this parameter is specified, only the settings along this pipe will be set. */

	virtual void CMD_Off(int iPK_Pipe) { string sCMD_Result; CMD_Off(iPK_Pipe,sCMD_Result,NULL);};
	virtual void CMD_Off(int iPK_Pipe,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #700 - Choose Pnp Device Template */
	/** We have chosen a new pnp device template */
		/** @param #57 PK_Room */
			/** The room this is in.  0 if not known */
		/** @param #150 PK_DHCPDevice */
			/** The template for the device */
		/** @param #224 PK_PnpQueue */
			/** The queue entry we're selecting for */

	virtual void CMD_Choose_Pnp_Device_Template(int iPK_Room,int iPK_DHCPDevice,int iPK_PnpQueue) { string sCMD_Result; CMD_Choose_Pnp_Device_Template(iPK_Room,iPK_DHCPDevice,iPK_PnpQueue,sCMD_Result,NULL);};
	virtual void CMD_Choose_Pnp_Device_Template(int iPK_Room,int iPK_DHCPDevice,int iPK_PnpQueue,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #804 - Set Pnp Options */
	/** Set options for a pnp device that will soon be created */
		/** @param #5 Value To Assign */
			/** The value to assign to it */
		/** @param #52 PK_DeviceData */
			/** The device data to assign */
		/** @param #224 PK_PnpQueue */
			/** The pnp entry */

	virtual void CMD_Set_Pnp_Options(string sValue_To_Assign,int iPK_DeviceData,int iPK_PnpQueue) { string sCMD_Result; CMD_Set_Pnp_Options(sValue_To_Assign.c_str(),iPK_DeviceData,iPK_PnpQueue,sCMD_Result,NULL);};
	virtual void CMD_Set_Pnp_Options(string sValue_To_Assign,int iPK_DeviceData,int iPK_PnpQueue,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #805 - Ignore PNP Device */
	/** Ignore a device detected */
		/** @param #224 PK_PnpQueue */
			/** The entry in the pnp queue */
		/** @param #225 Always */
			/** If true, blacklist this device in the 'unknown devices' list so we never ask again */

	virtual void CMD_Ignore_PNP_Device(int iPK_PnpQueue,bool bAlways) { string sCMD_Result; CMD_Ignore_PNP_Device(iPK_PnpQueue,bAlways,sCMD_Result,NULL);};
	virtual void CMD_Ignore_PNP_Device(int iPK_PnpQueue,bool bAlways,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #806 - PNP Detection Script Finished */
	/** A PNP Detection script has finished running */
		/** @param #13 Filename */
			/** The name of the PNP Detection Script */
		/** @param #18 Errors */
			/** If not empty, there were errors.  These will be logged. */
		/** @param #44 PK_DeviceTemplate */
			/** The device template of the device.  0 means this script couldn't identify the device */
		/** @param #109 Data String */
			/** The device data to be assigned when creating the device
PK_DeviceData\tIK_DeviceData\n
.... */
		/** @param #224 PK_PnpQueue */
			/** The queue entry */

	virtual void CMD_PNP_Detection_Script_Finished(string sFilename,string sErrors,int iPK_DeviceTemplate,string sData_String,int iPK_PnpQueue) { string sCMD_Result; CMD_PNP_Detection_Script_Finished(sFilename.c_str(),sErrors.c_str(),iPK_DeviceTemplate,sData_String.c_str(),iPK_PnpQueue,sCMD_Result,NULL);};
	virtual void CMD_PNP_Detection_Script_Finished(string sFilename,string sErrors,int iPK_DeviceTemplate,string sData_String,int iPK_PnpQueue,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #822 - Pre Pnp Script Done */
	/** The script that runs before prompting the user if he wants to create the device has finished.  This is in the 'Pre Pnp Script' parameter. */
		/** @param #109 Data String */
			/** The device data to be assigned when creating the device
PK_DeviceData\tIK_DeviceData\n
.... */
		/** @param #119 True/False */
			/** If false, don't bother trying to create the device */
		/** @param #224 PK_PnpQueue */
			/** the queue id */

	virtual void CMD_Pre_Pnp_Script_Done(string sData_String,bool bTrueFalse,int iPK_PnpQueue) { string sCMD_Result; CMD_Pre_Pnp_Script_Done(sData_String.c_str(),bTrueFalse,iPK_PnpQueue,sCMD_Result,NULL);};
	virtual void CMD_Pre_Pnp_Script_Done(string sData_String,bool bTrueFalse,int iPK_PnpQueue,string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
