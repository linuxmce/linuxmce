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
#include "Crystal_Fontz_USBRS232.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "LowLevel.h"
#include "../VFD_LCD/CoreMenuStructure.h"

void *CF_Thread(void *p)
{
	Crystal_Fontz_USBRS232 *pCrystal_Fontz_USBRS232 = (Crystal_Fontz_USBRS232 *) p;
	pCrystal_Fontz_USBRS232->m_bCF_ThreadRunning=true;
	pCrystal_Fontz_USBRS232->RunCFThread();
	pCrystal_Fontz_USBRS232->m_bCF_ThreadRunning=false;
	return NULL;
}

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Crystal_Fontz_USBRS232::Crystal_Fontz_USBRS232(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Crystal_Fontz_USBRS232_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
	, VFD_LCD_Base(20,4,20),m_CF_Mutex("CF"),m_CF_SendMutex("CFSend")
{
	m_bCF_ThreadRunning=false;
	m_pSerialPort=NULL;
	m_pCOMMAND_PACKET_LastResponse=NULL;
	m_CF_Mutex.Init(NULL);
	m_CF_SendMutex.Init(NULL);
}


//<-dceag-dest-b->
Crystal_Fontz_USBRS232::~Crystal_Fontz_USBRS232()
//<-dceag-dest-e->
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Waiting for Incoming data thread to exit");
	m_bQuit_set(true);
	while( m_bCF_ThreadRunning )
		Sleep(100);
	delete m_pSerialPort;
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Incoming data thread exited...");
}

//<-dceag-getconfig-b->
bool Crystal_Fontz_USBRS232::GetConfig()
{
	if( !Crystal_Fontz_USBRS232_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	m_pSerialPort = new CSerialPort("COM3", 115200, epbsN81);

	pthread_t t;
	pthread_create(&t, NULL, CF_Thread, (void*)this);

	SetMenuStructure( new CoreMenuStructure(this) );

//	GetFirmware();

	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Crystal_Fontz_USBRS232::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

//<-dceag-createinst-b->!
//<-dceag-const2-b->!

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void Crystal_Fontz_USBRS232::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	sCMD_Result = "UNHANDLED CHILD";
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void Crystal_Fontz_USBRS232::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

//<-dceag-sample-b->!


/*

	COMMANDS TO IMPLEMENT

*/

string Crystal_Fontz_USBRS232::GetFirmware()
{
    COMMAND_PACKET command_packet,response_packet;
    command_packet.command=1;
    command_packet.data_length=0;
	if( send_packet(&command_packet,response_packet) )
	{
		int k=2;
	}
	return "";
}
extern FILE *filer;

bool Crystal_Fontz_USBRS232::send_packet(COMMAND_PACKET *pCOMMAND_PACKET,COMMAND_PACKET &pCOMMAND_PACKET_Response)
{
LoggerWrapper::GetInstance()->Write(LV_STATUS,"Going to send packet");
	PLUTO_SAFETY_LOCK(sm,m_CF_SendMutex);
	PLUTO_SAFETY_LOCK(cf,m_CF_Mutex);
	delete m_pCOMMAND_PACKET_LastResponse;
	m_pCOMMAND_PACKET_LastResponse=NULL;
	for(int iRetries=0;iRetries<5;iRetries++)
	{
		ubyte
			i;
		unsigned char
			buffer[80];
		int
			index;
		index=0;
		buffer[index++]=pCOMMAND_PACKET->command;
		buffer[index++]=pCOMMAND_PACKET->data_length;
		for(i=0;i<pCOMMAND_PACKET->data_length;i++)
			buffer[index++]=pCOMMAND_PACKET->data[i];
		//Set the CRC
		pCOMMAND_PACKET->CRC.as_word=
			get_crc((ubyte *)pCOMMAND_PACKET,pCOMMAND_PACKET->data_length+2);
		buffer[index++]=pCOMMAND_PACKET->CRC.as_bytes[0];
		buffer[index++]=pCOMMAND_PACKET->CRC.as_bytes[1];

		m_pSerialPort->Write((char *)buffer,index);

		for(int iWaitForAck=0;iWaitForAck<1000;++iWaitForAck)
		{
			cf.Release();
			Sleep(50);
			cf.Relock();
			if( m_pCOMMAND_PACKET_LastResponse )
			{
LoggerWrapper::GetInstance()->Write(LV_STATUS,"Got response to packet");
				pCOMMAND_PACKET_Response=*m_pCOMMAND_PACKET_LastResponse;
				return true;
			}
		}

		// No ack after 500ms, try sending the command again by looping back
	}
fwrite("\nno response to packet\n",string("\nno response to packet\n").size(),1,filer);
LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"No response to packet");
	return false; // Failed after 5 attempts
}

void Crystal_Fontz_USBRS232::RunCFThread()
{
	COMMAND_PACKET command_packet;
	while( !m_bQuit_get())
	{
		if( m_pSerialPort->Read((char *) &command_packet,1,5000)==0 )
			continue; // 250 ms delay means when the thread is supposed to quit, there be at max 250ms before it does

		if( MAX_COMMAND < (0x3F & command_packet.command) )
		{
fwrite("\ngarbagecommand\n",string("\ngarbagecommand\n").size(),1,filer);
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"Getting garbage command %d",(int) command_packet.command);
			continue;
		}

		if( m_pSerialPort->Read((char *) &command_packet.data_length,1,250)==0 || command_packet.data_length>MAX_DATA_LENGTH )
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"***garbage data testing again %d",(int) command_packet.data_length);
fwrite("\ngarbage data length\n",string("\ngarbage data length\n").size(),1,filer);
			command_packet.command = command_packet.data_length;
			if( MAX_COMMAND < (0x3F & command_packet.command) || m_pSerialPort->Read((char *) &command_packet.data_length,1,250)==0 || command_packet.data_length>MAX_DATA_LENGTH )
			{
				LoggerWrapper::GetInstance()->Write(LV_WARNING,"Getting garbage data length %d",(int) command_packet.data_length);
				continue;
			}
LoggerWrapper::GetInstance()->Write(LV_WARNING,"trying to recover");
fwrite("\ntrying to recover\n",string("\ntrying to recover\n").size(),1,filer);
		}

		if( m_pSerialPort->Read((char *) &command_packet.data,command_packet.data_length,250)!= command_packet.data_length )
		{
fwrite("\ngarbage data\n",string("\ngarbage data\n").size(),1,filer);
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"Getting garbage data");
			continue;
		}

		if( m_pSerialPort->Read((char *) &command_packet.CRC.as_bytes,2,250)!= 2 )
		{
fwrite("\nno crc\n",string("\nno crc\n").size(),1,filer);
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"Couldn't get CRC");
			continue;
		}
		if( command_packet.CRC.as_word!=
			 get_crc((ubyte *)&command_packet,command_packet.data_length+2))
		{
fwrite("\nbad crc\n",string("\nbad crc\n").size(),1,filer);
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"CRC doesn't match");
			continue;
		}
SYSTEMTIME lt;
    timeval tv;
::GetLocalTime( &lt );

/** @todo Need to fill tv */
tv.tv_sec = (long)time( NULL );
tv.tv_usec = lt.wMilliseconds * 1000;
    struct tm *t = localtime((time_t *)&tv.tv_sec);
    char acBuff[50];
    double dwSec = (double)(tv.tv_usec/1E6) + t->tm_sec;

		// It's good!
		if( command_packet.command & 0x40 )
		{
LoggerWrapper::GetInstance()->Write(LV_STATUS,"resp packet");
fprintf(filer,"\ngood resp packet %d:%02d:%06.3f\n",(int)t->tm_hour, (int)t->tm_min, dwSec );
fclose(filer);
filer=fopen("C:\\serialread.cpp","ab");
			// It's a response
			PLUTO_SAFETY_LOCK(cf,m_CF_Mutex);
			if( m_pCOMMAND_PACKET_LastResponse )
				delete m_pCOMMAND_PACKET_LastResponse;
			m_pCOMMAND_PACKET_LastResponse=new COMMAND_PACKET(command_packet);
		}
		else if( command_packet.command & 0x80 )
		{
LoggerWrapper::GetInstance()->Write(LV_STATUS,"inc packet");
fprintf(filer,"\ngood inc packet %d:%02d:%06.3f\n",(int)t->tm_hour, (int)t->tm_min, dwSec );
fclose(filer);
filer=fopen("C:\\serialread.cpp","ab");
			HandleIncomingPacket(&command_packet);
		}
	}
}

void Crystal_Fontz_USBRS232::HandleIncomingPacket(COMMAND_PACKET *pcommand_packet)
{
	if( pcommand_packet->command==0x80 )
	{
		switch( pcommand_packet->data[0] )
		{
		case 7:
			Up();
			break;
		case 8:
			Down();
			break;
		case 9:
			Left();
			break;
		case 10:
			Right();
			break;
		case 11:
			Enter();
			break;
		case 12:
			Home();
			break;
		}
	}
}

void Crystal_Fontz_USBRS232::DoUpdateDisplay(vector<string> *vectString)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Updating Display with %d lines: %s",vectString->size(),vectString->size() ? (*vectString)[0].c_str() : "");
	for(size_t s=0;s<m_iNumLines;++s)
	{
		string str = s<vectString->size() ? (*vectString)[s] : "";
		COMMAND_PACKET command_packet,response_packet;
		command_packet.command=31;
		command_packet.data_length=2+m_iNumVisibleColumns;
		command_packet.data[0]=0;
		command_packet.data[1]=s;
		if( str.size() )
			memcpy(&command_packet.data[2],str.c_str(),
			       min(m_iNumVisibleColumns,(int)str.size()));
		if( str.size()<m_iNumVisibleColumns )
			memcpy((void *) &command_packet.data[2+str.size()],"                                                                                                    ",m_iNumVisibleColumns-str.size());
		if( !send_packet(&command_packet,response_packet) )
		{
			int k=2;
		}
	}
}
