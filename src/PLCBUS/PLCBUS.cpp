/*
     Copyright (C) 2010 Harald Klein <hari@vt100.at>

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#include "PLCBUS.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "PlutoUtils/LinuxSerialUSB.h"
#include "Generic_Serial_Device/IOUtils.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <termios.h>
#include <stdio.h>

int reprq = 0;

extern "C" void *start( void* );
void *start( void *p ) {
	PLCBUS *base = static_cast<PLCBUS*>(p);
	base->receiveFunction();
	return NULL;
}

int serial_write (int dev,uint8_t pnt[],int len)
{
        int res;

        res = write (dev,pnt,len);
        if (res != len) {
                return (-1);
        }
        tcflush(dev, TCIOFLUSH);

        return (len);
}

int serial_read (int dev,uint8_t pnt[],int len,long timeout)
{
        int bytes = 0;
        int total = 0;
        struct timeval tv;
        fd_set fs;

        while (total < len) {
                FD_ZERO (&fs);
                FD_SET (dev,&fs);
                tv.tv_sec = 0;
                tv.tv_usec = 500000;
                // tv.tv_usec = 100000;
                // bytes = select (FD_SETSIZE,&fs,NULL,NULL,&tv);
                bytes = select (dev+1,&fs,NULL,NULL,&tv);

                // 0 bytes or error
                if( bytes <= 0 )
                {
                       return total;
                }

                bytes = read (dev,pnt+total,len-total);
                total += bytes;
        }
	return total;
}


//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
PLCBUS::PLCBUS(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: PLCBUS_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
PLCBUS::PLCBUS(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: PLCBUS_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
}

//<-dceag-dest-b->
PLCBUS::~PLCBUS()
//<-dceag-dest-e->
{
	
}

//<-dceag-getconfig-b->
bool PLCBUS::GetConfig()
{
	if( !PLCBUS_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated


	if (DATA_Get_3_Phase()) {
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"3phase enabled");
		reprq = 64;
	}
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Open port: %s", TranslateSerialUSB(DATA_Get_COM_Port_on_PC()).c_str());
	fd = open(TranslateSerialUSB(DATA_Get_COM_Port_on_PC()).c_str(), O_RDWR);

	// B9600 8n1
	struct termios tio;
	tcgetattr(fd, &tio);
	tio.c_cflag = CS8 | CLOCAL | CREAD;
	tio.c_cc[VMIN] = 1;
	tio.c_cc[VTIME] = 0;
	tio.c_lflag = 0;
	tio.c_iflag = IGNBRK;
	tio.c_oflag = 0;
	cfsetispeed(&tio, B9600);
	cfsetospeed(&tio, B9600);
	tcflush(fd, TCIFLUSH);
	tcsetattr(fd,TCSANOW,&tio);

	pthread_mutex_init(&mutexSendQueue, NULL);
	static pthread_t readThread;
	pthread_create(&readThread, NULL, start, (void*)this);

	for (int i=0;i<5;i++) {
		PLCBUSJob *myjob = new PLCBUSJob;
		myjob->sendcount=0;
		myjob->homeunit=(i <<4);
		myjob->usercode=0;
		myjob->data1=0;
		myjob->data2=0;
		myjob->command=1;
		LoggerWrapper::GetInstance()->Write(LV_DEBUG,"Adding get all ID command to queue...");
		pthread_mutex_lock (&mutexSendQueue);
		PLCBUSSendQueue.push_back(myjob);
		pthread_mutex_unlock (&mutexSendQueue);
	}

	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool PLCBUS::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
PLCBUS_Command *Create_PLCBUS(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new PLCBUS(pPrimaryDeviceCommand, pData, pEvent, pRouter);
}
//<-dceag-createinst-e->

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void PLCBUS::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	string addr = pDeviceData_Impl->m_mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST);
	int house = addr.substr(0,1).c_str()[0]-65;
	int unit = atoi(addr.substr(1,2).c_str())-1;
	// LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"home %i unit %i",house, unit);

	PLCBUSJob *myjob = new PLCBUSJob;
	myjob->sendcount=0;
	myjob->homeunit=(house <<4) + unit;
	myjob->usercode=0;
	myjob->data1=0;
	myjob->data2=0;

	switch (pMessage->m_dwID) {
		case COMMAND_Generic_On_CONST:
			LoggerWrapper::GetInstance()->Write(LV_DEBUG,"COMMAND_Generic_On_CONST received for child %s",addr.c_str());
			myjob->command=192;
			sCMD_Result = "OK";
			break;
		;;
		case COMMAND_Generic_Off_CONST:
			LoggerWrapper::GetInstance()->Write(LV_DEBUG,"COMMAND_Generic_Off_CONST received for child %s",addr.c_str());
			myjob->command=193;
			sCMD_Result = "OK";
			break;
		;;
		case COMMAND_Set_Level_CONST:
			LoggerWrapper::GetInstance()->Write(LV_DEBUG,"COMMAND_Set_Level_CONST received for child %s",addr.c_str()); 
			myjob->command=184;
			myjob->data1 = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Level_CONST].c_str());
			sCMD_Result = "OK";
			break;
		;;
		default:
			sCMD_Result = "UNHANDLED CHILD";
			break;
		;;
	}

	LoggerWrapper::GetInstance()->Write(LV_DEBUG,"Adding command to queue...");
	pthread_mutex_lock (&mutexSendQueue);
	PLCBUSSendQueue.push_back(myjob);
	pthread_mutex_unlock (&mutexSendQueue);

}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void PLCBUS::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN COMMAND";
}

//<-dceag-sample-b->!
//<-dceag-sample-e->

/*

	COMMANDS TO IMPLEMENT

*/

void PLCBUS::receiveFunction() {

	uint8_t buf[1024];
	uint8_t bufr[1024];


	int timer = 0;

	while (1) { 
		timer++;
		for (int i=0;i<1020;i++) {
			bufr[i]=0;
		}
		int len = serial_read(fd, bufr, 9, 6);
				// search for SOF
		if (len > 0) {
			DCE::LoggerWrapper::GetInstance()->Write(LV_RECEIVE_DATA, DCE::IOUtils::FormatHexAsciiBuffer((const char*)bufr, len,"33").c_str());
			if (len!=9) continue;
		}
		// for (int i=0;i<len;i++) printf("%x\n",bufr[i]);

		int i=0;
		while ((bufr[i] != 0x2) && (i < len)) { 
			i++;
		}
		if (bufr[i] == 0x2) {
			// SOF found
			i++;
			if (bufr[i] == 6) { // found plcbus frame
				int tmpusercode = bufr[i+1];	
				int tmphomeunit = bufr[i+2];	
				int tmpcommand = bufr[i+3];	
				int tmpdata1 = bufr[i+4];	
				int tmpdata2 = bufr[i+5];	
				int rxtxswitch = bufr[i+6];
				if (rxtxswitch & 32) { // received ack
					pthread_mutex_lock (&mutexSendQueue);
					if (PLCBUSSendQueue.size() > 0 ) {
						if (PLCBUSSendQueue.front()->homeunit == tmphomeunit) {
							LoggerWrapper::GetInstance()->Write(LV_DEBUG,"Response from same id as command on send queue, removing command (sendcount: %i)",PLCBUSSendQueue.front()->sendcount);
							PLCBUSJob *myjob = PLCBUSSendQueue.front();
							PLCBUSSendQueue.pop_front();
							delete myjob;
						}
					}
					pthread_mutex_unlock (&mutexSendQueue);
				} else if (rxtxswitch & 64) { // received ID feedback signal
					pthread_mutex_lock (&mutexSendQueue);
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Received ID feedback signal for home %c, removing command (sendcount: %i)",65+(tmphomeunit >> 4),PLCBUSSendQueue.front()->sendcount);
					PLCBUSJob *myjob = PLCBUSSendQueue.front();
					PLCBUSSendQueue.pop_front();
					delete myjob;
					pthread_mutex_unlock (&mutexSendQueue);
					for (int i=0;i<8;i++) {
						if (tmpdata2 & 1<<i) {
							LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Found Unit %c%i",65+(tmphomeunit >> 4),i+1);
						}
					}
					for (int i=0;i<8;i++) {
						if (tmpdata1 & 1<<i) {
							LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Found Unit %c%i",65+(tmphomeunit >> 4),i+9);
						}
					}
				} else if (rxtxswitch & 0x1c) {
					// received bus copy
					LoggerWrapper::GetInstance()->Write(LV_DEBUG,"frame seen on bus");
					continue;
				}
	
			}
		} //...

		pthread_mutex_lock (&mutexSendQueue);
		if (PLCBUSSendQueue.size() > 0 ) {
			int commandlength = 8;
			buf[0]=0x2; // STX
			buf[1]=5; // LEN
			buf[2]=PLCBUSSendQueue.front()->usercode; // USERCODE
			buf[3]=PLCBUSSendQueue.front()->homeunit;
			buf[6]=0;
			switch(PLCBUSSendQueue.front()->command) {
				case 192:
					buf[4]=0x02 | 32 | reprq; // ON + ACK_PULSE
					break;
				case 193:
					buf[4]=0x03 | 32 | reprq; // OFF + ACK_PULSE
					break;
				case 184:
					buf[4]=0x0c | 32 | reprq; // PRESETDIM + ACK_PULSE
					buf[6]=0x3; // dim rate
					break;
				case 1:
					buf[4]=0x1c;
					break;
				default:
					buf[4]=0x02 | 32 | reprq;
			}
			buf[5]=PLCBUSSendQueue.front()->data1;
			buf[7]=0x03; // ETX

			LoggerWrapper::GetInstance()->Write(LV_DEBUG,"Send Queue Size: %i",PLCBUSSendQueue.size());
			DCE::LoggerWrapper::GetInstance()->Write(LV_SEND_DATA, "Sending job %p - %s",PLCBUSSendQueue.front(),DCE::IOUtils::FormatHexAsciiBuffer((const char*)buf, 8,"31").c_str());

			PLCBUSSendQueue.front()->sendcount++;
			serial_write(fd,(uint8_t*)buf,commandlength);

			if (PLCBUSSendQueue.front()->sendcount > 3) {
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Sendcount exceeded, this was the last sent attempt, removing job...");
				PLCBUSSendQueue.pop_front();
			}
			// for(int i=0;i<commandlength;i++) {
		 //		printf("0x%x ",(unsigned char) buf[i]);
		 //	}
		 //	printf("\n");
		}
		pthread_mutex_unlock (&mutexSendQueue);

	}
}


