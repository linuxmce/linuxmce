//Insteon API.  This file handles the communications interface to the low-level PLC driver

/*
     Copyright (C) 2007 Peter Kalogiannis

     www.linuxmce.com

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/


#ifndef PLUTOINSTEONAPI_H_
#define PLUTOINSTEONAPI_H_


extern "C" {
	#include "libilib/ilib.h"
}

#include <deque>
#include <map>
#include <string>
#include <time.h>

#include "InsteonJob.h"

using namespace std;
typedef deque<InsteonJob*> JobsDeque;
typedef JobsDeque::iterator JobsDequeIterator;
typedef JobsDeque::const_iterator JobsDequeCIterator;



 namespace DCE
{
	class Insteon;
}


class LMCEInsteonAPI {
	
	private:
			

	public:
	
		/**serial state enum representing the state of the object 
		 * regarding to serial connection*/
		enum SerialState { STOPPED, IDLE, RUNNING, WAITTING };
		
		/** The jobs can be managed synchronous or asynchronous.
		  * synchronous  = succesive asks and answers
		  * asynchronous = the asks and answer doesn't have to follow any time rules
		  */
		enum ListenType { SYNCHRONOUS, ASYNCHRONOUS };

		/** We need a handle to the PLC device
		 *	This is returned by the library call	
		 */
		 ilib_t * plc_handle;
	 
		 
		virtual bool isInitialized() const;

		virtual void setInitialized(bool ready);


		/**get an instance of this class
		 * @return the instance of this class*/		
		static LMCEInsteonAPI* instance();


		/** Sets the Pluto device which takes care of Pluto interface.
		  * It is used to send events over the Pluto system.
		  */
		virtual void setLMCE(DCE::Insteon *insteon);

				/**connects to port and starts the first command
		 * @return true if OK*/
		virtual bool start();
				
				/**listen for responses
		 * @param timeout the listen timeout
		 * @return true if succesfull*/
//		virtual bool listen(time_t timeout);
		
		/**listen for responses, but it can manage asynchronously the jobs
		 * @param timeout the listen timeout
		 * @return true if succesfull*/
		virtual bool listenAsynchronous();
		
			/**disconnects from the serial port
		 * @return true if succesfull*/
//		virtual bool stop();
	
	
		/** insert a job into the queue
		 * @param job is the InsteonJob to be inserted
		 * @return true if succesfull*/
		 virtual bool insertJob(InsteonJob* job);
	
				/**insert a node
		 * @param node the node to be inserted
		 * @return true if succesfull*/
		virtual void insertNode(unsigned char* node, unsigned char group);
		
		/**removes the node with the specifid ID
		 * @param id the node id to be removed
		 * @return true if succesfull*/
//		virtual bool removeNode(unsigned short id);
		
		
		/** Print some debug information about each node from the ZWave network.*/
//		virtual void showInsteonNetwork() const;
		
//		virtual bool Register();
		
		/**process the data
		 * @param buffer the data to be processed
		 * @param length the len of the data to be processed
		 * @return true if successfull*/
//		virtual bool processData(const char * buffer, size_t length);
		
		/**remove database links from PLC controller
		 * @return true if successfull*/
		 virtual bool RemoveLinks();
	
	
		/**sends data
		 * @param buffer the data to be sent
		 * @param length the length of the data to be sent
		 * @return true if successfull*/
		virtual bool sendData(unsigned char *buffer, size_t length);
	
		
		virtual ~LMCEInsteonAPI();
		bool GetPLCHandle();
		int SendOnCommand(int);
		int SendLinkCommand();
		void PrintPLCInfo();
		int SetFullOn(unsigned char* target);
		int SetFullOff(unsigned char* target);
		int GetLocalRampRate(unsigned char* target);
		int SetLocalRampRate(unsigned char* target, unsigned char rate);
		int LinkRemoteDevice(unsigned char* target, unsigned char group, unsigned char on_level, unsigned char ramp_rate);
		int UnLinkRemoteDevice(unsigned char* target, unsigned char group, unsigned char on_level, unsigned char ramp_rate);
		int SendGroupOn(unsigned char group);
		int SendGroupOff(unsigned char group);
		int SetLevelandRamp(unsigned char* target, unsigned char level, unsigned char ramp);
		void GetDevID();
	private:	
		unsigned char plc_devid[3];
		unsigned char firmware_id;
		LMCEInsteonAPI();
		static LMCEInsteonAPI * ref;
		
		class Private;
		Private * d;
};

#endif /*PLUTOINSTEONAPI_H_*/
