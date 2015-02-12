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
#ifndef gc100_h
#define gc100_h

//	DCE Implemenation for #40 gc100

#include "Gen_Devices/gc100Base.h"
//<-dceag-d-e->

#include "IRBase/IRBase.h"
#include "pluto_main/Define_DeviceCategory.h"
#include <map>
using namespace std;

typedef pair<int, int> IntPair;

namespace DCE
{
	class LearningInfo
	{
	public:
		long m_PK_Device, m_PK_Command, m_PK_Device_Orbiter, m_PK_Text;
		class gc100 *m_pgc100;
		LearningInfo(long PK_Device, long PK_Command, long PK_Device_Orbiter, long PK_Text, gc100 *pgc100)
		{
			m_PK_Device = PK_Device;
			m_PK_Command = PK_Command;
			m_PK_Device_Orbiter = PK_Device_Orbiter;
			m_pgc100 = pgc100;
			m_PK_Text = PK_Text;
		}
	};

	class module_info
	{
	public:
		int module; 	// GC-100 module number
		int slot; 	// within the module, 1-3
		std::string type; // SERIAL, RELAY, IR, or IRL
		std::string key; // e.g. 4:3 - might make a good primary key
		int global_slot; // i.e. 1-9 if there are 3 IR emitter blocks. e.g. the "1-6" IR labels on the back
	};
}

//<-dceag-decl-b->!
namespace DCE
{
	class gc100 : public gc100_Command, public IRBase
	{
//<-dceag-decl-e->
		// Private member variables
	private:

		// Private methods
protected:
	virtual void SendIR(string Port, string IRCode,int iRepeat);
	class Message *m_pThisMessage;

private:
	void parse_gc100_reply(std::string message);
	void parse_message_device(std::string message);
	void parse_message_statechange(std::string message, bool change);

	void ReportChildren();
	Command_Impl *GetChild(int dwPK_Device);

	pthread_t m_EventThread, m_LearningThread, m_SocketThread;

	// Public member variables

	// Begin shared section
	// These items may be accessed by multiple threads therefore must be locked
	
	pluto_pthread_mutex_t gc100_mutex;
	
	std::map<std::string, class module_info> module_map; // Keep track of the GC100's configuration // OK
	std::map<std::string, int> slot_map;		// OK
	int gc100_socket;				// OK
	std::string ip_addr;				// OK
	bool receiving_data;				// OK
//	bool is_open_for_learning;			// OK
	int learn_fd;					// OK
	char recv_buffer[4096]; // Replies from the GC100 go here // OK
	int recv_pos;					// OK
	std::string learn_input_string; 		// OK

	// End Shared section
	
	int ir_cmd_id;
	
	// Learning stuff
	std::string learn_device;
	bool open_for_learning();
	bool learning_timeout;
	int learning_timeout_count;

	bool m_bIRComplete;
	bool m_bLearning;
	bool m_bStopLearning;

	int learn_client, learn_server;

	int sent_messages, received_messages;
	bool bChildrenNeedToBeReported;
//<-dceag-const-b->
public:
		// Constructors/Destructor
		gc100(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~gc100();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_COM_Port_on_PC();
	bool DATA_Get_Dont_Auto_Configure();

			*****EVENT***** accessors inherited from base class
	void EVENT_Reporting_Child_Devices(string sError_Message,string sText);

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #191 - Send Code */
	/** Sends an I/R code to a device. */
		/** @param #9 Text */
			/** The I/R code -- usually in Pronto format */

	virtual void CMD_Send_Code(string sText) { string sCMD_Result; CMD_Send_Code(sText.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Send_Code(string sText,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #194 - Toggle Power */
	/** Set relay state (0 or 1) */
		/** @param #8 On/Off */
			/** Depending on each device On/Off can be interpreted differently, but in genereal On/Off has a value of 1 for on and 0 for Off */

	virtual void CMD_Toggle_Power(string sOnOff) { string sCMD_Result; CMD_Toggle_Power(sOnOff.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Toggle_Power(string sOnOff,string &sCMD_Result,Message *pMessage);


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

	virtual void CMD_Learn_IR(int iPK_Device,string sOnOff,int iPK_Text,int iPK_Command) { string sCMD_Result; CMD_Learn_IR(iPK_Device,sOnOff.c_str(),iPK_Text,iPK_Command,sCMD_Result,NULL);};
	virtual void CMD_Learn_IR(int iPK_Device,string sOnOff,int iPK_Text,int iPK_Command,string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->
		virtual void LEARN_IR(long PK_Device, long PK_Command, long PK_Device_Orbiter, long PK_Text);
		int LEARN_IR_via_Socket();
		virtual void LEARN_IR_CANCEL();

		void LearningThread(LearningInfo * pLearningInfo);
		void EventThread();
		void SocketThread(int port);

		bool Open_gc100_Socket();
		bool send_to_gc100(string Cmd);
		std::string read_from_gc100(struct timeval *timeout = NULL);
		bool ConvertPronto(string ProntoCode, string &gc_code, int Repeat = 1);

		// For a given device find the pin and device id.  return false if it's not found
		bool GetPinDeviceID(class DeviceData_Impl *pDeviceData,string &this_pin,int &this_device_id);

		bool relay_power(class DeviceData_Impl *pDeviceData, bool power_on);

		void Start_seriald();
		virtual void CreateChildren();
		virtual void SetQuitFlag();

		/** @brief Gets the "IPAddress" for the given device, or this device by default */
		string GetIpAddress( int dwPK_Device=0 );
	};
//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!
