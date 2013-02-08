//
// C++ Interface: PlutoInsteonlAPI
//
//
// Author:	Peter Kalogiannis (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
//<-dceag-d-b->

//	DCE Implemenation for #XXXX Insteon


#ifndef INSTEONSERIALAPI_H_
#define INSTEONSERIALAPI_H_




#include "DCE/PlainClientSocket.h"

extern "C" {
	#include "libilib/ilib.h"
}


static const int ReceiveCount[] = {
        5,      /* 0x40 Write to IBIOS: A2 L2 ACK */
        1,      /* 0x41 IBIOS message: L dL. */
        7,      /* 0x42 Read from IBIOS: A2 L2 C2 ACK */
        0,      /* 0x43 ETX-terminated IBIOS message: d(variable) ETX (3) */
        7,      /* 0x44 Get Checksim: A2 L2 C2 ACK */
        1,      /* 0x45 Event report: EN */
        5,      /* 0x46 Mask (bit-manipulation): A2 OR AND ACK */
        3,      /* 0x47 Simiulated event: EN TV ACK */
        7,      /* 0x48 Get version: I3 D2 FR */
        2,      /* 0x49 Debug reporting next instruction to execute: A2 */
        2,      /* 0x4a X10 recieved: XT d1 */
        -1,     /* 0x4b */
        -1,     /* 0x4c */
        -1,     /* 0x4d */
        -1,     /* 0x4e */
        10,     /* 0x4f Insteon packet: EN I3(source) I3(dest) MF d2 [d14] ACK*/
};

static const char * const	EventExplanations[] = {
	"SALad initialized",	/* 0 */
	"Received the first message in a hop sequence addressed to me",	/* 1 */
	"Received the first message in a hop sequence not addressed to me",/*2*/
	"Received a duplicate message",	/* 3 */
	"Recieved an ACK to my direct message",	/* 4 */
	"Did not get an ACK to my direct message, even after 5 retries",/* 5 */ 
	"Received a message to an unknown address, and censored it",	/* 6 */
	"Received a response to my join-me message",			/* 7 */
	"Received an X10 byte",						/* 8 */
	"Received an X10 extended-message byte",			/* 9 */
	"A SET button tap sequence has begun",			/* a */
	"The SET button is being pressed",				/* b */
	"The SET button has been released",				/* c */
	"The tick counter has expired",					/* d */
	"An alarm tripped",						/* e */
	"The dedicated midnight alarm tripped",				/* f */
	"The dedicated 2:00 AM alarm tripped",				/* 10 */
	"Received a serial byte for SALad processing",			/* 11 */
	"Received an unknown IBIOS serial command",			/* 12 */
	"Received an interrupt from my daughter card",			/* 13 */
	"The load was turned on",					/* 14 */
	"The load was turned off",					/* 15 */
};


/* namespace DCE
{
	class Insteon;
}
*/

class PlutoInsteonAPI {
	
	private:
		/** mutex for ZWave tasks.*/
	    pluto_pthread_mutex_t m_InsteonMutex;
		
		/** mutex to force the pooling job to run.*/
		/** Is this supposed to be polling?		  */
	    pluto_pthread_mutex_t m_PollingMutex;
		
		// mutex attribute
	    pthread_mutexattr_t m_MutexAttr;

		// ZWave API
		InsteonSerialAPI * m_InsteonAPI;

		/** Pooling thread */
		static pthread_t m_PoolThread;
		
		/** Download configuration thread */
		static pthread_t m_ReceiveThread;
				
		

	public:
		PlutoInsteonAPI();
		PlutoInsteonAPI(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		
		virtual bool Register();
		
		~PlutoInsteonAPI();
		int GetPLCHandle();
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
	private:	
		ilib_t * plc_handle;
		unsigned char plc_devid[3];
};

#endif /*INSTEONSERIALAPI_H_*/
