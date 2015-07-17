/*
     Copyright (C) 2013 LinuxMCE 

     www.linuxmce.org


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#ifndef Bluetooth_Adapter_h
#define Bluetooth_Adapter_h

//	DCE Implemenation for #2328 Bluetooth Adapter

#include "Gen_Devices/Bluetooth_AdapterBase.h"
//<-dceag-d-e->

//<-dceag-decl-b->
namespace DCE
{
	class Bluetooth_Adapter : public Bluetooth_Adapter_Command
	{
//<-dceag-decl-e->
		// Private member variables

		// Private methods
public:
		// Public member variables
	  pthread_t m_Adapter_Runloop_Thread;

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Bluetooth_Adapter(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Bluetooth_Adapter();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

//<-dceag-const2-b->!

		// Implement adapter runloop.
		virtual void CreateChildren();

		// The runloop.
		virtual void AdapterRunloop();

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	bool DATA_Get_Only_One_Per_PC();
	bool DATA_Get_Autoassign_to_parents_room();
	bool DATA_Get_PNP_Create_Without_Prompting();
	bool DATA_Get_Immediate_Reload_Isnt_Necessar();

			*****EVENT***** accessors inherited from base class
	void EVENT_Device_Detected(string sMac_Address,string sText,string sIP_Address,int iPK_DeviceTemplate,string sVendorModelID,int iPK_CommMethod,int iPK_PnpProtocol,string sPNP_Serial_Number,string sDeviceData,string sCategory,string sSignature);
	void EVENT_Device_Removed(string sMac_Address,string sText,int iPK_Device,string sIP_Address,int iPK_DeviceTemplate,string sVendorModelID,int iPK_CommMethod,int iPK_PnpProtocol,string sPNP_Serial_Number,string sDeviceData,string sCategory);

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #1143 - Bluetooth Pair Device */
	/** Pair Bluetooth Device to Media Director */
		/** @param #47 Mac address */
			/** Bluetooth MAC Address */

	virtual void CMD_Bluetooth_Pair_Device(string sMac_address) { string sCMD_Result; CMD_Bluetooth_Pair_Device(sMac_address.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Bluetooth_Pair_Device(string sMac_address,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #1144 - Bluetooth Connect Device */
	/** Connect Bluetooth Device to Media Director kernel. */
		/** @param #47 Mac address */
			/** Bluetooth MAC Address */

	virtual void CMD_Bluetooth_Connect_Device(string sMac_address) { string sCMD_Result; CMD_Bluetooth_Connect_Device(sMac_address.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Bluetooth_Connect_Device(string sMac_address,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #1145 - Bluetooth Disconnect Device */
	/** Disconnect Bluetooth Device from Media Director kernel */
		/** @param #47 Mac address */
			/** Bluetooth MAC Address */

	virtual void CMD_Bluetooth_Disconnect_Device(string sMac_address) { string sCMD_Result; CMD_Bluetooth_Disconnect_Device(sMac_address.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Bluetooth_Disconnect_Device(string sMac_address,string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
