/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

/** @file EIB.h
Header for EIB class.
*/
//<-dceag-d-b->
#ifndef EIB_h
#define EIB_h

//	DCE Implemenation for #49 EIB

#include "Gen_Devices/EIBBase.h"
//<-dceag-d-e->

#include "messagepool.h"
#include "duplicatefilterpoolinterceptor.h"

//<-dceag-decl-b->!
/** @namespace DCE
The Data Commands and Events (DCE) namespace
*/
namespace DCE
{

    /** @class EIB
    For EIB (European Installation Bus) Lighting control.
    */
	class EIB : public EIB_Command, public EIBBUS::DuplicateFilterPoolInterceptor
	{
//<-dceag-decl-e->
		// Private member variables

		// Private methods
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
        virtual bool Connect(int iPK_DeviceTemplate);
//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		EIB(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

protected:
		/*callback methods from Message Pool*/
		virtual void handleUniqueTelegram(const EIBBUS::TelegramMessage *pt);

private:
		/*process telegram methods*/
		bool processTelegram(const EIBBUS::TelegramMessage *pt, DeviceData_Impl *pDevData);

		/*helper methods*/
		/*
		static bool getParamsFromChannel(const std::string& sChannel,
							unsigned int index, std::string& param);
		*/
		static int getParamsFromChannel(const std::string& sChannel,
							std::vector<std::string>& params);
		static float getFloatFromUShort(unsigned short tempr);
		static unsigned short getUShortFromFloat( float tempr );
private:
	enum DATATYPE {
		DT_UNKNOWN				= 0,
		DT_SWITCH 				= 1, 		/*1 bit*/
		DT_DIMMING_CONTROL 		= 2,		/*4 bit*/
		DT_TIME			 		= 3,		/*3 Bytes*/
		DT_DATE			 		= 4,		/*3 Bytes*/
		DT_VALUE		 		= 5,		/*2 Bytes*/
		DT_SCALING		 		= 6,		/*1 Byte*/
		DT_DRIVE_CONTROL		= 7,		/*1 bit*/
		DT_FLOAT				= 9,		/*4 Bytes*/
		DT_COUNTER16			= 10,		/*2 Bytes*/
		DT_COUNTER32			= 11,		/*4 Bytes*/
		DT_ASCIICHAR			= 13,		/*1 Byte*/
		DT_COUNTER8				= 14,		/*1 Byte*/
		DT_STRING				= 15		/*23 Bytes*/
	};

	EIBBUS::MessagePool m_msgPool;

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_COM_Port_on_PC();
	string DATA_Get_COM_Port_ParityBitStop();
	string DATA_Get_COM_Port_BaudRate();

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
	};


//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
