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
	
	DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Initialization...");

	DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Open serial port %s",device);
	
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
	
	//Initiate a request for IM Info
	mybuf[0] = 0x02;
	mybuf[1] = COMMAND_GET_IM_INFO;
	sendFunction( mybuf , 2, 0, 0); 
	
	return NULL;

}

InsteonAPI::InsteonAPI::InsteonAPI(void* myInsteon)
{
	this->myInsteon = myInsteon;
	DCEcallback = static_cast<DCE::Insteon_PLM_DCE*>(myInsteon);
}

void InsteonAPI::InsteonAPI::receiveFunction()
{
	DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"receiveFunction started");
	long timer;
	long idletimer;
	char retbuf[1];
	char mybuf2[1024];
	size_t len2;
	while (1) {
	
		// read one byte from the serial port
		//We expect all messages coming from the PLM to begin with 0x02
		mybuf2[0]=0;
		len2 = ReadSerialStringEx(serialPort,mybuf2, 1, 100);
			
		if ((len2 == 1) && (mybuf2[0] == 0x02))
		{
			len2 += ReadSerialStringEx(serialPort,mybuf2+1, 1, 100);	
			
			switch(mybuf2[1])
			{
					//Check for a version request reply
					case COMMAND_GET_IM_INFO:
					len2 +=ReadSerialStringEx(serialPort,mybuf2+2,7,100);
					
					plmIDHiByte  = mybuf2[2];
					plmIDMidByte = mybuf2[3];
					plmIDLoByte  = mybuf2[4];
					
					plmDevCat 	 	= mybuf2[5];
					plmDevSubCat	= mybuf2[6];
					plmFirmwareRev 	= mybuf2[7];
				break;
			}
		} else {
			// nothing received, let's see if there is a job to send
			pthread_mutex_lock (&mutexSendQueue);
			if (InsteonSendQueue.size()>0) 
			{
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
		for (i = 0; i < length;i++)
		{
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
	sdBuffer[1] = COMMAND_SEND_SD_ED_MSG;
	
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
	
	
