/*
     Copyright (C) 2013 LinuxMCE 

     www.linuxmce.org


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#ifndef icpdasbridge_h
#define icpdasbridge_h

//	DCE Implemenation for #2329 icpdas-bridge

#include "Gen_Devices/icpdasbridgeBase.h"
// #include "DCE/PlainClientSocket.h"
// #include "DCE/SocketListener.h"
// #include "DCE/ServerSocket.h"

//<-dceag-d-e->

//<-dceag-decl-b->
namespace DCE
{
	class icpdasbridge : public icpdasbridge_Command
	{
//<-dceag-decl-e->
		// Private member variables
private: 
        	bool receiving_data;                            
                int learn_fd;                               
                char recv_buffer[4096];
                int icpdas_socket;
                int recv_pos;
		pthread_t m_EventThread; //, m_LearningThread, m_SocketThread;
        
                // Public member variables
                
                // Begin shared section
                // These items may be accessed by multiple threads therefore must be locked
                                
 		pluto_pthread_mutex_t icpdas_mutex;
                                        
		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		icpdasbridge(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);

		virtual bool Open_icpdas_Socket(); // Opens the socket to the server
                std::string read_from_icpdas(struct timeval *timeout = NULL);   // Reads data from icpdas but does not interpret them at all
		void parse_icpdas_reply(std::string message);    // Parses the icpdas by finding out the verb at the beginning, stripping together the port, and the value to call
		void icp2dce(std::string sPort, std::string sValue); // This gets called with the icpdas port information as stored in lmce and the value as provided by icpdas, i.e. VAL=
                int icpval2dce(std::string sValue);	// Helper function called by icp2dce to convert VAL values into values usable by lmce
                // The SendXXXXXEvent functions are used by icp2dce to transfer the received icpdas event to lmce
		// Shamelessly copied from agecontrolbridge.
                void SendLightChangedEvent(unsigned int PK_Device, int value);
                void SendSensorTrippedEvent(unsigned int PK_Device, bool value);
                void SendTemperatureChangedEvent(unsigned int PK_Device, float value);
                void SendBrightnessChangedEvent(unsigned int PK_Device, int value);
                
                // The next function receives commands from LinuxMCE and formats them so icpdas understands them
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		bool send_to_icpdas(string Cmd); // Helper function to do the actual command to icpdas.


		void populate_children();  // NOT used atm.
                void EventThread();
                
                virtual void CreateChildren();                
 		
 		virtual ~icpdasbridge();
		virtual bool GetConfig();
		virtual bool Register();
		
		
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		icpdasbridge(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	int DATA_Get_TCP_Port();
	string DATA_Get_TCP_Address();
	string DATA_Get_Username();
	string DATA_Get_Password();

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/

//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
