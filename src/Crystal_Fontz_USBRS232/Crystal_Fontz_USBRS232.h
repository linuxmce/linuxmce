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
#ifndef Crystal_Fontz_USBRS232_h
#define Crystal_Fontz_USBRS232_h

//	DCE Implemenation for #1722 Crystal Fontz USB/RS232

#include "Gen_Devices/Crystal_Fontz_USBRS232Base.h"
//<-dceag-d-e->

#include "VFD_LCD/VFD_LCD_Base.h"
#include "Serial/SerialPort.h"

//============================================================================
typedef unsigned char ubyte;
typedef signed char sbyte;
typedef unsigned short word;
typedef unsigned long dword;
typedef union
{
	ubyte
		as_bytes[2];
	word
		as_word;
}WORD_UNION;

//============================================================================
#define MAX_DATA_LENGTH 22
//============================================================================
#define MAX_COMMAND 36
//============================================================================

class COMMAND_PACKET
{
public:
	ubyte
		command;
	ubyte
		data_length; 
	ubyte
		data[MAX_DATA_LENGTH];
	WORD_UNION
		CRC;

	COMMAND_PACKET() { command=0; }
	COMMAND_PACKET(COMMAND_PACKET &command_packet) 
	{ 
		command=command_packet.command; 
		data_length=command_packet.data_length; 
		memcpy(data,command_packet.data,MAX_DATA_LENGTH);
		CRC.as_word=command_packet.CRC.as_word; 
	}

	void operator=( const COMMAND_PACKET& command_packet )
	{
		command=command_packet.command; 
		data_length=command_packet.data_length; 
		memcpy(data,command_packet.data,MAX_DATA_LENGTH);
		CRC.as_word=command_packet.CRC.as_word; 
	}
};

//<-dceag-decl-b->!
namespace DCE
{
	class Crystal_Fontz_USBRS232 : public Crystal_Fontz_USBRS232_Command, public VFD_LCD_Base
	{
//<-dceag-decl-e->
		// Private member variables

		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Crystal_Fontz_USBRS232(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Crystal_Fontz_USBRS232();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

		CSerialPort *m_pSerialPort;
		pluto_pthread_mutex_t m_CF_Mutex,m_CF_SendMutex; 
		bool m_bCF_ThreadRunning;
		COMMAND_PACKET *m_pCOMMAND_PACKET_LastResponse;

//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		Crystal_Fontz_USBRS232(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

		string GetFirmware();
		bool send_packet(COMMAND_PACKET *pCOMMAND_PACKET,COMMAND_PACKET &pCOMMAND_PACKET_Response);
		void RunCFThread();  // Listen for incoming packets
		void HandleIncomingPacket(COMMAND_PACKET *pcommand_packet);
		void DoUpdateDisplay(vector<string> *vectString);

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


//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
