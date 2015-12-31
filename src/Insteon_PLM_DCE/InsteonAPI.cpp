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

#include "Generic_Serial_Device/IOUtils.h"

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

	callbackpool = 1;
	await_ack = 0;
	
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
	//mybuf[0] = IM_CMD_START;
	mybuf[0] = CMD_60_GET_IM_INFO;
	sendFunction( mybuf , 1, 0, 0); 
	DCE::LoggerWrapper::GetInstance()->Write(LV_INSTEON,"Get IM config");
	//Get the IM config
	mybuf[0] = CMD_73_GET_IM_CONFIG;
	sendFunction( mybuf , 1, 0, 0);
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
	//mybuf[0] = IM_CMD_START;
	mybuf[0] = CMD_69_GET_FIRST_ALL_LINK_RECORD;
	DCE::LoggerWrapper::GetInstance()->Write(LV_INSTEON,"Start getting ALL-Link database");
	sendFunction( mybuf , 1, 0, 0);
}

void InsteonAPI::InsteonAPI::receiveFunction()
{
	DCE::LoggerWrapper::GetInstance()->Write(LV_INSTEON,"receiveFunction started");
	long timer;
	long idletimer;
	char mybuf[1024]; // Buffer to hold commands to send
	char mybuf2[1024]; // Buffer to hold received data
	size_t len2;
	while (1) {
	
		// read one byte from the serial port
		//We expect all messages coming from the PLM to begin with 0x02
		mybuf2[0]=0;
		len2 = ReadSerialStringEx(serialPort,mybuf2, 1, 100);
			
		if ((len2 == 1) && (mybuf2[0] == IM_02_CMD_START)) // we received a message
		{
			len2 += ReadSerialStringEx(serialPort,mybuf2+1, 1, 100);	
//			DCE::LoggerWrapper::GetInstance()->Write(LV_INSTEON,"Received command: %X",mybuf2[1]);
			// commands are varible lenth and the ACK/NAK is at the end, so we have to figure out what it is
			// and read the whole thing to know if it was successfull or not
			switch(mybuf2[1])
			{
				case CMD_60_GET_IM_INFO:
				len2 +=ReadSerialStringEx(serialPort,mybuf2+2,7,100);
				
				plmIDHiByte  = mybuf2[2];
				plmIDMidByte = mybuf2[3];
				plmIDLoByte  = mybuf2[4];
				
				plmDevCat 	= mybuf2[5];
				plmDevSubCat	= mybuf2[6];
				plmFirmwareRev 	= mybuf2[7];

				DCE::LoggerWrapper::GetInstance()->Write(LV_INSTEON,"Got IM info %s; Address: %X.%X.%X Cat: %X SubCat: %X Firmware: %X",DCE::IOUtils::FormatHexBuffer(mybuf2,8).c_str(),plmIDHiByte,plmIDMidByte,plmIDLoByte,plmDevCat,plmDevSubCat,plmFirmwareRev);
				sendNextCommand();
				break;

				case CMD_73_GET_IM_CONFIG:
				len2 +=ReadSerialStringEx(serialPort,mybuf2+2,4,100);
				DCE::LoggerWrapper::GetInstance()->Write(LV_INSTEON,"Got IM config: %x",mybuf2[2]);
				if (!(mybuf2[2] & CFG_40_BIT_MONITOR_MODE)) {
					// IM is not in monitor mode, but needs to be
					//mybuf[0] = IM_CMD_START;
					mybuf[0] = CMD_6B_SET_IM_CONFIG;
					mybuf[1] = mybuf2[2] | CFG_40_BIT_MONITOR_MODE;
					DCE::LoggerWrapper::GetInstance()->Write(LV_INSTEON,"Setting IM config: %x",mybuf2[2] | CFG_40_BIT_MONITOR_MODE);
					sendFunction( mybuf , 2, 0, 0);
				}
				sendNextCommand();
				break;
				case CMD_57_GOT_ALL_LINK_RECORD:
				len2 +=ReadSerialStringEx(serialPort,mybuf2+2,8,100);
				DCE::LoggerWrapper::GetInstance()->Write(LV_INSTEON,"Got ALL-Link record; Address: %X.%X.%X (%s)",mybuf2[4],mybuf2[5],mybuf2[6],DCE::IOUtils::FormatHexBuffer(mybuf2,10).c_str());
				// we now need to query the device to see what it is
				//mybuf[0] = IM_CMD_START;
				mybuf[0] = CMD_62_SEND_SD_ED_MSG;
				mybuf[1] = mybuf2[4];
				mybuf[2] = mybuf2[5];
				mybuf[3] = mybuf2[6];
				mybuf[4] = 0x0f;
				mybuf[5] = 0x0d;
				mybuf[6] = 0x00;
				sendFunction( mybuf , 7, 0, 0); 
/*				mybuf[4] = 0x1F;
				mybuf[5] = 0x03;
				mybuf[6] = 0x00;
				mybuf[7] = 0x00;
				mybuf[8] = 0x00;
				mybuf[9] = 0x00;
				mybuf[10] = 0x00;
				mybuf[11] = 0x00;
				mybuf[12] = 0x00;
				mybuf[13] = 0x00;
				mybuf[14] = 0x00;
				mybuf[15] = 0x00;
				mybuf[16] = 0x00;
				mybuf[17] = 0x00;
				mybuf[18] = 0x00;
				mybuf[19] = 0x00;
				mybuf[20] = 0x00;
				sendFunction( mybuf , 21, 0, 0); */
				//sendNextCommand();
				break;
				case CMD_69_GET_FIRST_ALL_LINK_RECORD:
					len2 += ReadSerialStringEx(serialPort,mybuf2+2, 1, 100);
					DCE::LoggerWrapper::GetInstance()->Write(LV_INSTEON,"->Received CMD %X response: %s",mybuf2[1],DCE::IOUtils::FormatHexBuffer(mybuf2,3).c_str());
					sendNextCommand();
				break;
				case CMD_51_ED_MSG_RECV:
					len2 += ReadSerialStringEx(serialPort,mybuf2+2, 22, 100);
					DCE::LoggerWrapper::GetInstance()->Write(LV_INSTEON,"Received extended message: %x (%s) %d",mybuf2[2],DCE::IOUtils::FormatHexBuffer(mybuf2,len2).c_str(),len2);
					switch(mybuf2[2]){

					}
//					sendNextCommand();
//				mybuf[1] = CMD_GET_NEXT_ALL_LINK_RECORD;
//				sendFunction( mybuf , 2, 0, 0);
				break;
				case CMD_50_SD_MSG_RECV:
				len2 +=ReadSerialStringEx(serialPort,mybuf2+2,9,100);
				DCE::LoggerWrapper::GetInstance()->Write(LV_INSTEON,"Received SD command - From: %X.%X.%X To: %X.%X.%X Flags: %X Cmd1: %X Cmd2: %X (%s) (read %d bytes)",mybuf2[2],mybuf2[3],mybuf2[4],mybuf2[5],mybuf2[6],mybuf2[7],mybuf2[8],mybuf2[9],mybuf2[10],DCE::IOUtils::FormatHexBuffer(mybuf2,11).c_str(),len2);
				switch(mybuf2[9]){
				case CMD_0D_SD_GET_VERSION:
					if (mybuf2[10] & 1) {
						DCE::LoggerWrapper::GetInstance()->Write(LV_INSTEON,"  Device is i2 ");
					} else {
						DCE::LoggerWrapper::GetInstance()->Write(LV_INSTEON,"  Device is i1");
					}
					mybuf[0] = CMD_62_SEND_SD_ED_MSG;
                                mybuf[1] = mybuf2[4];
                                mybuf[2] = mybuf2[5];
                                mybuf[3] = mybuf2[6];
                                mybuf[4] = 0x0f;
                                mybuf[5] = 0x10;
                                mybuf[6] = 0x00;
                                sendFunction( mybuf , 7, 0, 0);
					sendNextCommand();
					break;
				}
				break;
				case CMD_62_SEND_SD_ED_MSG:
				len2 +=ReadSerialStringEx(serialPort,mybuf2+2,7,100);
				//DCE::LoggerWrapper::GetInstance()->Write(LV_INSTEON,"Previous command status: %X",mybuf2[8]);
				if (mybuf2[5] & 16) {
					// extended message
					len2 +=ReadSerialStringEx(serialPort,mybuf2+9,14,100);
				}
				DCE::LoggerWrapper::GetInstance()->Write(LV_INSTEON,"->Received CMD %1X Response. Address: %X.%X.%X (%s) (read %d bytes)",mybuf2[6],mybuf2[2],mybuf2[3],mybuf2[4],DCE::IOUtils::FormatHexBuffer(mybuf2,len2).c_str(),len2);
				//if (mybuf2[8] == 0x15) {
				//	DCE::LoggerWrapper::GetInstance()->Write(LV_INSTEON,"Previous command failed");
				//}
				break;
				case CMD_6A_GET_NEXT_ALL_LINK_RECORD:
				len2 +=ReadSerialStringEx(serialPort,mybuf2+2,1,100);
				if (mybuf2[2] == 0x06) {
					DCE::LoggerWrapper::GetInstance()->Write(LV_INSTEON,"ALL-Link database done downloading");
				}
				break;
				default:
				// unhandled command. assume its SD and read in some data to display
				len2 +=ReadSerialStringEx(serialPort,mybuf2+2,7,100);
				DCE::LoggerWrapper::GetInstance()->Write(LV_INSTEON,"Received unhandled command: %s (read %d bytes)",DCE::IOUtils::FormatHexBuffer(mybuf2,len2).c_str(),len2);
			}
		} else {
			// nothing received, let's see if there is a job to send
			pthread_mutex_lock (&mutexSendQueue);
			if (InsteonSendQueue.size()>0) {
				if ( await_ack != 1 && await_callback == 0) {
					await_callback = (unsigned int) InsteonSendQueue.front()->callbackid;

DCE::LoggerWrapper::GetInstance()->Write(LV_INSTEON, "<-Sending job %p (cb %i) - %s (%d jobs in queue)",InsteonSendQueue.front(),await_callback,DCE::IOUtils::FormatHexAsciiBuffer(InsteonSendQueue.front()->buffer, InsteonSendQueue.front()->len,"31").c_str(),InsteonSendQueue.size());
					callback_type = (unsigned int) InsteonSendQueue.front()->callback_type;
//					DCE::LoggerWrapper::GetInstance()->Write(LV_INSTEON,"Sending command: %x", InsteonSendQueue.front()->buffer[1]);
					//Send Job
					WriteSerialStringEx(serialPort,InsteonSendQueue.front()->buffer, InsteonSendQueue.front()->len);
					//Pop if we only need to send it once (add logic)
					InsteonSendQueue.front()->sendcount++;
					await_ack = 1;
					timer=0;
				} else {
					timer++;
					if (timer > 30 && await_callback != 0) {
						DCE::LoggerWrapper::GetInstance()->Write(LV_WARNING, "No callback received: await_callback: %i timer: %i",await_callback,timer);
						timer=0;
						// resend, we got no final callback
						await_ack = 0;
						await_callback = 0;
						if (InsteonSendQueue.front()->sendcount > 2) {
							DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "ERROR: Dropping command, no callback received after three resends");
							dropSendQueueJob();
						}
					}
				}
			} else {
				idletimer++;
			}
			pthread_mutex_unlock (&mutexSendQueue);
		}
	}
}

bool InsteonAPI::InsteonAPI::sendFunction(char *buffer, size_t length, int type, bool response)
{
	
	InsteonJob *newJob = new InsteonJob;
	unsigned int i = 0, index = 0;
	
	
	newJob->await_response = response && (type == REQUEST);
	newJob->sendcount = 0;

	newJob->buffer[index++] = IM_02_CMD_START;
//	newJob->buffer[index++] = length + 2 + (response ? 1 : 0);
//	newJob->buffer[index++] = type == RESPONSE ? RESPONSE : REQUEST;
	//Fill in remainder of command		
	for (i = 0; i < length;i++) {
		newJob->buffer[index++] = buffer[i];
	}
	newJob->len = length+1;// + 4 + (response ? 1 : 0);
	pthread_mutex_lock (&mutexSendQueue);
	if (response) {
		if (callbackpool>255 || callbackpool==0) { callbackpool=1; }
		newJob->buffer[index++] = callbackpool;
		newJob->callbackid = callbackpool++;
		newJob->callback_type = (unsigned int)newJob->buffer[3];
	} else {
		newJob->callbackid = 0;
		newJob->callback_type = 0;
	}
	newJob->buffer[index] = checksum(newJob->buffer+1,length+2+( response ? 1 : 0) );
	InsteonSendQueue.push_back(newJob);
	pthread_mutex_unlock (&mutexSendQueue);
	return true;
}

bool InsteonAPI::InsteonAPI::setLightOn (char* id, unsigned char level)
{
	char cmd[128];
	
	cmd[0] = CMD_11_SD_LIGHT_ON;
	cmd[1] = level; 
	sendSDMsg(id, cmd);
	return 0;
}

bool InsteonAPI::InsteonAPI::setLightOff (char* id)
{
	char cmd[128];
	
	cmd[0] = CMD_13_SD_LIGHT_OFF;
	cmd[1] = 0x00; 
	sendSDMsg(id, cmd);
	return 0;
}

bool InsteonAPI::InsteonAPI::sendSDMsg(char* id, char* msg)
{
	char sdBuffer[128];
	
	//First Byte of Command to PLM is always 0x02
//	sdBuffer[0] = IM_02_CMD_START;
	
	//For a Standard-Length Message send, the second byte is 0x62
	sdBuffer[0] = CMD_62_SEND_SD_ED_MSG;
	
	//Fill in Target Address
	for (int i = 0; i < 3; i++)
	{
		sdBuffer[i+1] = id[i];
	}
	
	//set to Direct Message, SD length, 3 resends
	sdBuffer[4] = 0x0F;
	
	sdBuffer[5] = msg[0];
	sdBuffer[6] = msg[1];
	
	sendFunction(sdBuffer,7,0,0);
	return 0;
}
	
char InsteonAPI::InsteonAPI::checksum(char* buf, int len)
{
return 0;
}

void InsteonAPI::InsteonAPI::dropSendQueueJob()
{
	// pthread_mutex_lock (&mutexSendQueue);
	InsteonSendQueue.pop_front();
	// pthread_mutex_unlock (&mutexSendQueue);
	// await_callback = 0;
	dropped_jobs++;
	if( dropped_jobs >= 6 ) {
		// If you get this error, take a look at Trac ticket #874
		DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "ERROR: Three dropped commands in a row, soft-resetting controller");
		pthread_mutex_unlock (&mutexSendQueue);
		//zwSoftReset();
		pthread_mutex_lock (&mutexSendQueue);
		dropped_jobs = 0;
	}
}

void InsteonAPI::InsteonAPI::sendNextCommand()
{
	pthread_mutex_lock (&mutexSendQueue);
	InsteonSendQueue.pop_front();
	pthread_mutex_unlock (&mutexSendQueue);
	await_ack = 0;
	await_callback = 0;
	dropped_jobs = 0;
}
	
