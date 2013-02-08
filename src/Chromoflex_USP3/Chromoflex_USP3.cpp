/*
     Copyright (C) 2009 Harald Klein <hari@vt100.at>

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#include "Chromoflex_USP3.h"
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <termios.h>
#include <stdio.h>

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Chromoflex_USP3::Chromoflex_USP3(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Chromoflex_USP3_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
Chromoflex_USP3::Chromoflex_USP3(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: Chromoflex_USP3_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
}

//<-dceag-dest-b->
Chromoflex_USP3::~Chromoflex_USP3()
//<-dceag-dest-e->
{
	
}

//<-dceag-getconfig-b->
bool Chromoflex_USP3::GetConfig()
{
	if( !Chromoflex_USP3_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Open port: %s", TranslateSerialUSB(DATA_Get_COM_Port_on_PC()).c_str());
	fd = open(TranslateSerialUSB(DATA_Get_COM_Port_on_PC()).c_str(), O_RDWR);
	unsigned char buf[1024];

	increment=1;
	speed=1;

	// init crc
	usp_crc = 0xffff;

	// disable any programs on the units
	buf[0]=0xca; // preamble
	buf[1]=0x00; // broadcast
	buf[2]=0x00; // broadcast
	buf[3]=0x00; // broadcast
	buf[4]=0x00; // length 
	buf[5]=0x02; // length
	buf[6]=0x7e; // 7e == write register
	buf[7]=18; // register addr
	buf[8]=0x01; // disable internal programs
	for (int i = 0; i < 9; i++) process_crc(buf[i]);
	buf[9] = (usp_crc >> 8);
	buf[10] = (usp_crc & 0xff);

	// setup B9600 8N1 first
	struct termios tio;
	tcgetattr(fd, &tio);
	tio.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
	tcflush(fd, TCIFLUSH);
	tcsetattr(fd,TCSANOW,&tio);

	if (write (fd, buf, 11) == 11) {
		return true;
	} else {
		return false;
	}
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Chromoflex_USP3::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
Chromoflex_USP3_Command *Create_Chromoflex_USP3(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new Chromoflex_USP3(pPrimaryDeviceCommand, pData, pEvent, pRouter);
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
void Chromoflex_USP3::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	int red = 0;
	int green = 0;
	int blue = 0;
	unsigned char buf[1024];

	int level = 0;

	int address = atoi(pDeviceData_Impl->m_mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST).c_str());

	switch (pMessage->m_dwID) {
		case COMMAND_Generic_On_CONST:
			red = 255; green = 255; blue = 255;
			break;
		;;
		case COMMAND_Generic_Off_CONST:
			red = 0; green = 0; blue = 0;
			break;
		;;
		case COMMAND_Set_Level_CONST:
			level = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Level_CONST].c_str());
			red = green = blue = (int) ( 255.0 * level / 100 );
			break;
		;;
		case COMMAND_Set_Color_RGB_CONST:
			red = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Red_Level_CONST].c_str());
			green = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Green_Level_CONST].c_str());
			blue = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Blue_Level_CONST].c_str());
			break;
		;;
		default:
			break;
		;;
	}
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL," Setting RGB: %d %d %d",red,green,blue);

	// assemble frame
	buf[0]=0xca; // preamble
	buf[1]=0x00; // broadcast
	buf[2]=0x00; // broadcast
	buf[3]=0x00; // broadcast
	buf[4]=0x00; // length 
	buf[5]=0x08; // length
	buf[6]=0x7e; // 7e == effect color
	buf[7]=0x04; // register addr
	buf[8]=red; // R
	buf[9]=green; // G
	buf[10]=blue; // B
	buf[11]=0x00; // X
	buf[12]=increment; // reg 8 - red increment
	buf[13]=increment; // reg 9 - green increment
	buf[14]=increment; // reg 10 - blue increment

	// calc crc16
	usp_crc = 0xffff;
	for (int i = 0; i < 15; i++) process_crc(buf[i]);

	buf[15] = (usp_crc >> 8);
	buf[16] = (usp_crc & 0xff);

	if (write (fd, buf, 17) != 17) {
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Write error: %s", strerror(errno ));
		exit(-1);
	}

	// TODO: handle multiple childs, we just broadcast for now
	sCMD_Result = "OK";
	//	sCMD_Result = "UNHANDLED CHILD";
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void Chromoflex_USP3::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN COMMAND";
}

//<-dceag-sample-b->!
//<-dceag-sample-e->

/*

	COMMANDS TO IMPLEMENT

*/


void Chromoflex_USP3::process_crc(unsigned char ucData) {
      int i;
      usp_crc^=ucData;
      for(i=0;i<8;i++){ // Process each Bit
             if(usp_crc&1){ usp_crc >>=1; usp_crc^=0xA001;}
             else{          usp_crc >>=1; }
      }

}
