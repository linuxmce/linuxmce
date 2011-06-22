#include <sys/time.h>
#include <sys/types.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <termios.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <sys/file.h>

#include <iostream>


#include "Serial.h"

#include "InsteonAPI.h"
#include "Insteon_PLM_DCE.h"

DCE::Insteon_PLM_DCE *DCEcallback;

extern "C" void *start( void* );

void *start( void *p )
{
	InsteonAPI::InsteonAPI *base = static_cast<InsteonAPI::InsteonAPI*>(p);
	base->receiveFunction();
	return NULL;
}

void* InsteonAPI::InsteonAPI::init(const char *device)
{
	char mybuf[1024];
	
	DCE::LoggerWrapper::GetInstance()->Write(LV_INSTEON, "Initialization...");

	DCE::LoggerWrapper::GetInstance()->Write(LV_INSTEON, "Open serial port %s",device);
	
	OpenSerialPortEx(device,&serialPort);

	/*Test Code for Dev Lamplinc	 
	mybuf[0]=0x02;
	mybuf[1]=0x62;
	mybuf[2]=0x0a;
	mybuf[3]=0xb9;
	mybuf[4]=0x94;
	mybuf[5]=0x05;
	mybuf[6]=0x11;
	mybuf[7]=0xFF;    
    WriteSerialStringEx(serialPort,mybuf,8);
	*/
	
	pthread_mutex_init(&mutexSendQueue, NULL);
	
	static pthread_t readThread;
	pthread_create(&readThread, NULL, start, (void*)this);
	
	DCE::LoggerWrapper::GetInstance()->Write(LV_INSTEON,"Get IM info");
	//Initiate a request for IM Info
	mybuf[0] = IM_CMD_START;
	mybuf[1] = CMD_GET_IM_INFO;
	sendFunction( mybuf , 2, 0, 0); 
	DCE::LoggerWrapper::GetInstance()->Write(LV_INSTEON,"Get IM config");
	//Get the IM config
	mybuf[1] = CMD_GET_IM_CONFIG;
	sendFunction( mybuf , 2, 0, 0);
	getDatabase();
	return NULL;

}

InsteonAPI::InsteonAPI::InsteonAPI(void* myInsteon)
{
	this->myInsteon = myInsteon;
	DCEcallback = static_cast<DCE::Insteon_PLM_DCE*>(myInsteon);
}

void InsteonAPI::InsteonAPI::getDatabase()
{
	char mybuf[2];
	mybuf[0] = IM_CMD_START;
	mybuf[1] = CMD_GET_FIRST_ALL_LINK_RECORD;
	DCE::LoggerWrapper::GetInstance()->Write(LV_INSTEON,"Start getting ALL-Link database");
	sendFunction( mybuf , 2, 0, 0);
}

void InsteonAPI::InsteonAPI::receiveFunction()
{
	DCE::LoggerWrapper::GetInstance()->Write(LV_INSTEON,"receiveFunction started");
	char mybuf[1024]; // Buffer to hold commands to send
	char mybuf2[1024]; // Buffer to hold received data
	size_t len2;
	while (1) {
	
		// read one byte from the serial port
		//We expect all messages coming from the PLM to begin with 0x02
		mybuf2[0]=0;
		len2 = ReadSerialStringEx(serialPort,mybuf2, 1, 100);
			
		if ((len2 == 1) && (mybuf2[0] == IM_CMD_START))
		{
			len2 += ReadSerialStringEx(serialPort,mybuf2+1, 1, 100);	
			DCE::LoggerWrapper::GetInstance()->Write(LV_INSTEON,"Received command: %x",mybuf2[1]);
			switch(mybuf2[1])
			{
				case CMD_GET_IM_INFO:
				len2 +=ReadSerialStringEx(serialPort,mybuf2+2,7,100);
				
				plmIDHiByte  = mybuf2[2];
				plmIDMidByte = mybuf2[3];
				plmIDLoByte  = mybuf2[4];
				
				plmDevCat 	= mybuf2[5];
				plmDevSubCat	= mybuf2[6];
				plmFirmwareRev 	= mybuf2[7];

				DCE::LoggerWrapper::GetInstance()->Write(LV_INSTEON,"Got IM info; Address: %x.%x.%x Cat: %x SubCat: %x Firmware: %x",plmIDHiByte,plmIDMidByte,plmIDLoByte,plmDevCat,plmDevSubCat,plmFirmwareRev);
				break;

				case CMD_GET_IM_CONFIG:
				len2 +=ReadSerialStringEx(serialPort,mybuf2+2,4,100);
				DCE::LoggerWrapper::GetInstance()->Write(LV_INSTEON,"Got IM config: %x",mybuf2[2]);
				if (!(mybuf2[2] & CFG_BIT_MONITOR_MODE)) {
					// IM is not in monitor mode, but needs to be
					mybuf[0] = IM_CMD_START;
					mybuf[1] = CMD_SET_IM_CONFIG;
					mybuf[2] = mybuf2[2] | CFG_BIT_MONITOR_MODE;
					DCE::LoggerWrapper::GetInstance()->Write(LV_INSTEON,"Setting IM config: %x",mybuf2[2] | CFG_BIT_MONITOR_MODE);
					sendFunction( mybuf , 3, 0, 0);
				}
				break;
				case CMD_GOT_ALL_LINK_RECORD:
				len2 +=ReadSerialStringEx(serialPort,mybuf2+2,8,100);
				DCE::LoggerWrapper::GetInstance()->Write(LV_INSTEON,"Got ALL-Link record; Address: %x.%x.%x",mybuf2[4],mybuf2[5],mybuf2[6]);
				// we now need to query the device to see what it is
				mybuf[0] = IM_CMD_START;
				mybuf[1] = CMD_SEND_SD_ED_MSG;
				mybuf[2] = mybuf2[4];
				mybuf[3] = mybuf2[5];
				mybuf[4] = mybuf2[6];
				mybuf[5] = 0x0F;
				mybuf[6] = CMD_SD_PROD_DATA_REQ;
				mybuf[7] = 0x00;
				sendFunction( mybuf , 8, 0, 0);
				break;
				case CMD_ED_MSG_RECV:
					len2 += ReadSerialStringEx(serialPort,mybuf2+1, 1, 100);
					DCE::LoggerWrapper::GetInstance()->Write(LV_INSTEON,"Received extended message: %x",mybuf2[2]);
					switch(mybuf2[2]){

					}
//				mybuf[1] = CMD_GET_NEXT_ALL_LINK_RECORD;
//				sendFunction( mybuf , 2, 0, 0);
				break;
				case CMD_SD_MSG_RECV:
				len2 +=ReadSerialStringEx(serialPort,mybuf2+2,7,100);
				DCE::LoggerWrapper::GetInstance()->Write(LV_INSTEON,"Received SD command: From: %x.%x.%x To: %x.%x.%x Flags: %x Cmd1: %x Cmd2: %x",mybuf2[2],mybuf2[3],mybuf2[4],mybuf2[5],mybuf2[6],mybuf2[7],mybuf[8],mybuf2[9],mybuf2[10]);
				break;
				case CMD_SEND_SD_ED_MSG:
				len2 +=ReadSerialStringEx(serialPort,mybuf2+2,7,100);
				DCE::LoggerWrapper::GetInstance()->Write(LV_INSTEON,"Previous command status: %x",mybuf2[8]);
				DCE::LoggerWrapper::GetInstance()->Write(LV_INSTEON,"Cmd: %x Address: %x.%x.%x",mybuf2[6],mybuf2[2],mybuf2[3],mybuf2[4]);
				if (mybuf2[8] == 0x15) {
					DCE::LoggerWrapper::GetInstance()->Write(LV_INSTEON,"Previous command failed");
				}
				break;
				case CMD_GET_NEXT_ALL_LINK_RECORD:
				len2 +=ReadSerialStringEx(serialPort,mybuf2+2,1,100);
				if (mybuf2[2] == 0x06) {
					DCE::LoggerWrapper::GetInstance()->Write(LV_INSTEON,"ALL-Link database done downloading");
				}
				break;
			}
		} else {
			// nothing received, let's see if there is a job to send
			pthread_mutex_lock (&mutexSendQueue);
			if (InsteonSendQueue.size()>0) {
				DCE::LoggerWrapper::GetInstance()->Write(LV_INSTEON,"Sending command: %x", InsteonSendQueue.front()->buffer[1]);
				//Send Job
				WriteSerialStringEx(serialPort,InsteonSendQueue.front()->buffer, InsteonSendQueue.front()->len);
				//Pop if we only need to send it once (add logic)
				InsteonSendQueue.pop_front();
			}
			pthread_mutex_unlock (&mutexSendQueue);
		}
	}
}

bool InsteonAPI::InsteonAPI::sendFunction(char *buffer, size_t length, int type, bool response)
{
	
	InsteonJob *newJob = new InsteonJob;
	unsigned int i = 0;
	
	
	//Fill in remainder of command		
	for (i = 0; i < length;i++) {
		newJob->buffer[i ] = buffer[i];
	}
	newJob->len = length;
	pthread_mutex_lock (&mutexSendQueue);
	InsteonSendQueue.push_back(newJob);
	pthread_mutex_unlock (&mutexSendQueue);
	return true;
}

bool InsteonAPI::InsteonAPI::setLightOn (char* id, unsigned char level)
{
	char cmd[128];
	
	cmd[0] = CMD_SD_LIGHT_ON;
	cmd[1] = level; 
	sendSDMsg(id, cmd);
	return 0;
}

bool InsteonAPI::InsteonAPI::setLightOff (char* id)
{
	char cmd[128];
	
	cmd[0] = CMD_SD_LIGHT_OFF;
	cmd[1] = 0x00; 
	sendSDMsg(id, cmd);
	return 0;
}

bool InsteonAPI::InsteonAPI::sendSDMsg(char* id, char* msg)
{
	char sdBuffer[128];
	
	//First Byte of Command to PLM is always 0x02
	sdBuffer[0] = IM_CMD_START;
	
	//For a Standard-Length Message send, the second byte is 0x62
	sdBuffer[1] = CMD_SEND_SD_ED_MSG;
	
	//Fill in Target Address
	for (int i = 0; i < 3; i++)
	{
		sdBuffer[i+2] = id[i];
	}
	
	//set to Direct Message, SD length, 3 resends
	sdBuffer[5] = 0x0F;
	
	sdBuffer[6] = msg[0];
	sdBuffer[7] = msg[1];
	
	sendFunction(sdBuffer,8,0,0);
	return 0;
}
	
	
