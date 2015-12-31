/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "common/CommonIncludes.h"
#include "common/StringUtils.h"
#include "common/OCLogger.h"
#include "common/Serial/SerialPort.h"

#include "common/OCDeviceData.h"
#include "DCE2/GC100Command.h"
#include "DCE2/IR_PORTCommand.h"
#include "common/IR/IRBase.h"

class module_info {
	public:
	
	int module; 	// GC-100 module number
	int slot; 	// within the module, 1-3
	std::string type; // SERIAL, REALY, IR, or IRL
	std::string key; // e.g. 4:3 - might make a good primary key
	int global_slot; // i.e. 1-9 if there are 3 IR emitter blocks. e.g. the "1-6" IR labels on the back
};


// Now for the GC100 class
class GC100CommandImpl: public GC100Command, public IRBase
{

public:
	GC100CommandImpl(class OCLogger *pOCLogger, int DeviceID, string ServerAddress, string replacement);
	virtual ~GC100CommandImpl();
	
	// Begin shared section
	// These items may be accessed by multiple threads therefore must be locked
	
	my_pthread_mutex_t shared_lock;
	
	std::map<std::string, class module_info> module_map; // Keep track of the GC100's configuration // OK
	std::map<std::string, int> slot_map;		// OK
	int gc100_socket;				// OK
	std::string ip_addr;				// OK
	bool receiving_data;				// OK
	bool is_open_for_learning;			// OK
	int learn_fd;					// OK
	std::list<std::string> command_list;		// OK
	bool ready;					// OK
	int pending_ir_cmd;				// OK
	char recv_buffer[4096]; // Replies from the GC100 go here // OK
	int recv_pos;					// OK
	bool m_bLearning; 				// OK
	std::string learn_input_string; 		// OK
	int m_IRDeviceID, m_IRActionID, m_ControllerID; // OK
	int timeout_count;				// OK

	// End Shared section
	
	int ir_cmd_id;
	
	// Learning stuff
	std::string learn_device;
	bool open_for_learning();
	bool learning_timeout;
	int learning_timeout_count;

	virtual OCDeviceCommand *CreateCommand(int PKID_MasterDeviceList, OCDeviceCommand *pPrimaryDeviceCommand, OCDeviceData *pData, OCDeviceEvent *pEvent);
	
	virtual bool ReceivedOCMessage(class OCMessage *pMessage);
	virtual void LEARN_IR(string PKID_Device,string ABSOLUTE_CHANNEL,string ID,string IR_FREQUENCY);
	virtual void SEND_CODE(string ABSOLUTE_CHANNEL,string TOKENS,string TEXT) {};
	virtual void LEARN_IR_CANCEL(string ABSOLUTE_CHANNEL) { m_bLearning = false; };
	virtual void SET_LEARN_FREQUENCY(string IR_FREQUENCY); 
	virtual void recv_data(string CHANNEL_NAME);
	virtual void send_data(string TEXT,string CHANNEL_NAME);

	bool Open_gc100_Socket();
	void MonitorIR();
	bool send_to_gc100();
	std::string read_from_gc100();
	bool ConvertPronto(string ProntoCode, string &gc_code);
	std::string IRL_to_pronto(string learned_string);
	
	void relay_power(class OCMessage *pMessage, bool power_on);

	void Start_seriald();

protected:
	virtual void SendIR(string Port, string IRCode);

private:

	void parse_gc100_reply(std::string message);
	void parse_message_device(std::string message);
	void parse_message_completeir(std::string message);
	void parse_message_statechange(std::string message, bool change);
	void sleep_ms(int time);
};


// a little stub class to handle registration of devices on specific ports
class IR_PORTCommandImpl : public IR_PORTCommand
{
public:
	IR_PORTCommandImpl(OCDeviceCommand *pPrimaryDeviceCommand, OCDeviceData *pData, OCDeviceEvent *pEvent);
}; // end class

// Define our own IO_pin class
class IO_PinCommandImpl: public IO_PinCommand {
public:
	IO_PinCommandImpl(OCDeviceCommand *pPrimaryDeviceCommand, OCDeviceData *pData, OCDeviceEvent *pEvent);

	GC100CommandImpl* m_Parent;
	class OCLogger* m_Logger;
	std::string my_pin_string;
};
