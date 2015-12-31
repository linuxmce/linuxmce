//Insteon API.  This file handles the communications interface to the low-level PLC driver

/*
     Copyright (C) 2007 Peter Kalogiannis

     www.linuxmce.com

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#include "LMCEInsteonAPI.h"
#include "main.h"


#include <string.h>
#include <stdlib.h>

extern "C" {
	#include <hid.h>
	#include "libilib/ilib.h"
}
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <iostream>


#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include "PlutoUtils/MultiThreadIncludes.h" 

#include "InsteonJob.h"
#include "InsteonJobSwitchChangeLevel.h"

#define WRITE_DELAY 50

using namespace std;
using namespace DCE;



LMCEInsteonAPI * LMCEInsteonAPI::ref = NULL;

// -------------- Private -----------------------------------
class LMCEInsteonAPI::Private
{
	public:

		static unsigned char count;
		
		Private(LMCEInsteonAPI*);

		~Private();
		
		bool applicationHandler();
		

		/** mutex for the asynchronous mode.*/
	    pluto_pthread_mutex_t asynchMutex;
		
		// mutex attribute
	    pthread_mutexattr_t mutexAttr;
		
		DCE::Insteon * insteon;
		JobsDeque jobsQueue;
//		JobsDeque jobsTimeoutQueue;
//		JobsDeque jobsPIR;
//		NodesMap nodes;
//		PlutoZWSerialAPI::SerialState state;
		LMCEInsteonAPI::ListenType listenType;
		
		unsigned long homeID;
		unsigned short nodeID;
		unsigned short sucID;
		std::string version;
		unsigned char capabilities;
		unsigned char firmwareVersion;
		time_t timeLeft;
		
		InsteonJob * currentJob;
		char command[65536];
		size_t commandLength;
		bool unknown;
		bool initialized;

	private:

		LMCEInsteonAPI * parent_;
};

unsigned char LMCEInsteonAPI::Private::count = 0;

LMCEInsteonAPI::Private::Private(LMCEInsteonAPI * parent)
	: asynchMutex("insteon_asynchronous"),
//	  connection(NULL),
	  insteon(NULL),
//	  state(LMCEInsteonAPI::STOPPED),
	  listenType(LMCEInsteonAPI::ASYNCHRONOUS),
/*	  homeID(0L),
	  nodeID(0),
	  sucID(0),
	  capabilities(0),
	  firmwareVersion(0),
	  timeLeft(0),
*/	  currentJob(NULL),
	  commandLength(0),
	  unknown(false),
	  initialized(false),
	  parent_(parent)
{
	pthread_mutexattr_init( &mutexAttr );
	pthread_mutexattr_settype( &mutexAttr, PTHREAD_MUTEX_RECURSIVE_NP );
	asynchMutex.Init( &mutexAttr );
	
	memset(command, 0, sizeof(command));
}

bool LMCEInsteonAPI::isInitialized() const
{
	return d->initialized;
}

void LMCEInsteonAPI::setInitialized(bool ready)
{
	d->initialized = ready;
}


void LMCEInsteonAPI::GetDevID()
{
	//First create local arrays for the function call to stuff with ID bytes
	unsigned char device_fw;
	
	if (NULL != plc_handle)
	{
		ibios_get_version(plc_handle, &device_fw,(unsigned char*)plc_devid);
	}
}

bool LMCEInsteonAPI::sendData(unsigned char *buffer, size_t length)
{
	//usleep(WRITE_DELAY);
	insteon_send_msg (plc_handle, buffer[0], buffer[1], &buffer[2]);

	// TODO: error
	
	return false;
}


bool LMCEInsteonAPI::listenAsynchronous()
{
	//TODO
	return false;
}
LMCEInsteonAPI::Private::~Private()
{
	PLUTO_SAFETY_LOCK(insm, asynchMutex);
	
	delete currentJob;
	currentJob = NULL;
	
/*	InsteonJob * job = NULL;
	for(JobsDequeIterator it=jobsQueue.begin(); it!=jobsQueue.end(); ++it)
	{
		job = (*it);
		delete job;
		job = NULL;
	}
	for(JobsDequeIterator it1=jobsTimeoutQueue.begin(); it1!=jobsTimeoutQueue.end(); ++it1)
	{
		job = (*it1);
		delete job;
		job = NULL;
	}
	for(JobsDequeIterator it2=jobsPIR.begin(); it2!=jobsPIR.end(); ++it2)
	{
		job = (*it2);
		delete job;
		job = NULL;
	}
	
	insm.Release();
*/
}

bool LMCEInsteonAPI::Private::applicationHandler()
{
	bool bReturn = true;
	
/*	if( command[0] == 0 && command[1] == FUNC_ID_APPLICATION_COMMAND_HANDLER )
	{
		// TODO
		// command[2] is 0
		// command[3] is source node
		// command[4] is length of data
		// command[5..length] == data
		// command[5] is command ID , see ZW_classcmd.h
		// command[6] is subcommand
		// command[7..length] parameters
		switch( command[5] )
		{
			case COMMAND_CLASS_WAKE_UP :
				wakeupPIRHandler(command[3], &command[5], command[4]);
				break;
				
			case COMMAND_CLASS_SENSOR_BINARY :
				alarmPIRHandler(command[3], &command[5], command[4]);
				break;
			
			default:
				bReturn = false;
				break;
		}
	}
*/	
	return bReturn;
}

void LMCEInsteonAPI::setLMCE(DCE::Insteon *insteon)
{
	d->insteon = insteon;
}

bool LMCEInsteonAPI::insertJob(InsteonJob* job)
{
	d->jobsQueue.push_back(job);
	return true;
}


bool LMCEInsteonAPI::start()
{
//	PLUTO_SAFETY_LOCK(am, d->asynchMutex);
	
	if( d->jobsQueue.size() )
		{
#ifdef PLUTO_DEBUG
		LoggerWrapper::GetInstance()->Write(LV_DEBUG, "++++++++++++ Insteon is connected");
#endif
		
		// clean up the timeout queue
/*			for(JobsDequeIterator it1=d->jobsTimeoutQueue.begin(); it1!=d->jobsTimeoutQueue.end(); ++it1)
			{
				job = (*it1);
				delete job;
				job = NULL;
			}
*/				
			if( d->jobsQueue.size() )
			{
				d->currentJob = d->jobsQueue.front();
				d->jobsQueue.pop_front();
			}
			
			if( d->currentJob != NULL && d->currentJob->run() )
			{
				return true;
			}
		}
	
	//LoggerWrapper::GetInstance()->Write(LV_WARNING, "LMCEInsteonAPI::start : connection problems");
	
	// it wasn't possible to start it, then just reset it
//	stop();
//	clearJobs();
	
	return false;
}

LMCEInsteonAPI* LMCEInsteonAPI::instance()
{
	if(ref == NULL)
	{
		ref = new LMCEInsteonAPI();
	}
	return ref;
}
 
bool LMCEInsteonAPI::GetPLCHandle()
{
	if ((plc_handle = ilib_open(USE_HIDDEV, "/dev/hiddev0")) != NULL)
	{
		d->initialized = true;
		return true;
	}
	return false;
}

bool LMCEInsteonAPI::RemoveLinks()
{
	if (NULL != plc_handle)
	{
		iplc_clear_links(plc_handle);
	}
}

void  LMCEInsteonAPI::insertNode(unsigned char* nodeID, unsigned char group)
{
	if (NULL != plc_handle)
	{
		iplc_add_id(plc_handle, nodeID, group, 0x01);
		LinkRemoteDevice(nodeID, group, 0xFF, 0x1F);
	}
}


void LMCEInsteonAPI::PrintPLCInfo()
{
	unsigned char plc_firmware;
	unsigned char plc_id[10];
	
	ibios_get_version(plc_handle, &plc_firmware, &plc_id[0]);
	cout << "PLC address: "<< plc_id<<endl;
	cout << "PLC firmware: "<<(int)plc_firmware<<endl;
}		

int LMCEInsteonAPI::SetFullOn(unsigned char* target)
{
	int retval = 0;
	if (plc_handle)
	{
		retval = insteon_send_msg (plc_handle, 0x11, 0xFF, target);
	}
	cout << "Turned on light at "<< hex <<(int)target[0]<<"."<<(int)target[1]<<"."<<(int)target[2]<<endl;
	cout << "command returned "<<hex<<retval<<endl;
	return retval;
}

int LMCEInsteonAPI::SetLevelandRamp(unsigned char* target, unsigned char level, unsigned char ramp)
{
	int retval = 0;
	unsigned char cmd2 =  (level << 4) | (ramp & 0x0f);
	if (plc_handle)
	{
		retval = insteon_send_msg (plc_handle, 0x2E, cmd2, target);
	}
//	cout << "Turned on light at "<< hex <<(int)target[0]<<"."<<(int)target[1]<<"."<<(int)target[2]<<endl;
//	cout << "command returned "<<hex<<retval<<endl;
	return retval; 
}

int LMCEInsteonAPI::SetFullOff(unsigned char* target)
{
	int retval = 0;
	if (plc_handle)
	{
		retval = insteon_send_msg (plc_handle, 0x13, 0x00, target);
	}
//	cout << "Turned off light at "<< hex <<(int)target[0]<<"."<<(int)target[1]<<"."<<(int)target[2]<<endl;
//	cout << "command returned "<<hex<<retval<<endl;
	return retval;
}

int LMCEInsteonAPI::SendGroupOn(unsigned char group)
{
	unsigned char group_on[3] = {0x00};
	group_on[2]=group;
	insteon_send_group_msg (plc_handle,0x11,0xFF,group_on);
	return 1;
}


int LMCEInsteonAPI::SendGroupOff(unsigned char group)
{
	unsigned char group_on[3] = {0x00};
	group_on[2]=group;
	insteon_send_group_msg (plc_handle,0x13,0x00,group_on);
	return 1;
}


int LMCEInsteonAPI::GetLocalRampRate(unsigned char* target)
{
	int retval = 0;
	if (plc_handle)
	{
		retval = insteon_read_byte(plc_handle,target, (unsigned short)0x0021);
	}
	return retval;
}

int LMCEInsteonAPI::SetLocalRampRate(unsigned char* target, unsigned char rate)
{
	int retval = 0;
	if (plc_handle)
	{
		insteon_write_byte(plc_handle, target, (unsigned short) 0x0021, rate);
	}
	return retval;
}

int LMCEInsteonAPI::LinkRemoteDevice(unsigned char* target, unsigned char group, 
		unsigned char on_level, unsigned char ramp_rate)
{
	insteon_set_remote_link(plc_handle, target, (unsigned char*)&plc_devid, group, on_level, ramp_rate);
	return 1;
}

int LMCEInsteonAPI::UnLinkRemoteDevice(unsigned char* target, unsigned char group, 
		unsigned char on_level, unsigned char ramp_rate)
{
	insteon_delete_remote_link(plc_handle, target, (unsigned char*)&plc_devid, group);
	return 1;
}

LMCEInsteonAPI::LMCEInsteonAPI()
{
	d = new Private(this);
	if(d == NULL)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Not enough memory to allocate LMCEInsteonAPI::Private");
	}
	GetPLCHandle();
	GetDevID();
}


LMCEInsteonAPI::~LMCEInsteonAPI()
{
	if (plc_handle != NULL)
	{
		ilib_close(plc_handle);
	}
}
