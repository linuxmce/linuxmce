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
#include "AEt_EMC2000.h"
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
                tv.tv_usec = 100000;
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
AEt_EMC2000::AEt_EMC2000(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: AEt_EMC2000_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
AEt_EMC2000::AEt_EMC2000(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: AEt_EMC2000_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
}

//<-dceag-dest-b->
AEt_EMC2000::~AEt_EMC2000()
//<-dceag-dest-e->
{
	
}

//<-dceag-getconfig-b->
bool AEt_EMC2000::GetConfig()
{
	if( !AEt_EMC2000_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated

	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Open port: %s", TranslateSerialUSB(DATA_Get_COM_Port_on_PC()).c_str());
	fd = open(TranslateSerialUSB(DATA_Get_COM_Port_on_PC()).c_str(), O_RDWR);

	// B19200 8n1
	struct termios tio;
	tcgetattr(fd, &tio);
	tio.c_cflag = CS8 | CLOCAL | CREAD;
	cfsetispeed(&tio, B19200);
	cfsetospeed(&tio, B19200);
	tcflush(fd, TCIFLUSH);
	tcsetattr(fd,TCSANOW,&tio);

	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool AEt_EMC2000::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
AEt_EMC2000_Command *Create_AEt_EMC2000(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new AEt_EMC2000(pPrimaryDeviceCommand, pData, pEvent, pRouter);
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
void AEt_EMC2000::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{

	uint8_t buf[1024];
	uint8_t bufr[1024];

	buf[0] = 0x2; // header
	buf[1] = '0'; // instnr
	buf[2] = '3'; // instnr
	buf[3] = '9'; // instnr
	buf[4] = '9'; // instnr
	buf[5] = '0'; // reglernr
	buf[6] = '1'; // reglernr
	//buf[7] = 'I'; // TA
	buf[7] = 'Z'; // TA
	buf[8] = '2'; // ETX
	buf[9] = '2'; // ETX
	buf[10] = 0x3; // ETX
	buf[11] = 0xd; // RET


	int address = atoi(pDeviceData_Impl->m_mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST).c_str());
	buf[8]=48+address;

	switch (pMessage->m_dwID) {
		case COMMAND_Generic_On_CONST:
			sCMD_Result = "OK";
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"COMMAND_Generic_On_CONST received for child %i",address);
			buf[9] = '2'; // ETX
			break;
		;;
		case COMMAND_Generic_Off_CONST:
			sCMD_Result = "OK";
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"COMMAND_Generic_Off_CONST received for child %i",address);
			buf[9] = '0'; // ETX
			break;
		;;
		case COMMAND_Set_Level_CONST:
			sCMD_Result = "OK";
			break;
		;;
		default:
			sCMD_Result = "UNHANDLED CHILD";
			break;
		;;
	}
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Sending buffer...");
	serial_write(fd,(uint8_t*)buf,12);
	int len = serial_read(fd, bufr, 1020,3);
	for(int i=0;i<len;i++) {
		printf("%c",(unsigned char) bufr[i]);
	}
	len = serial_read(fd, bufr, 1020,7);
	for(int i=0;i<len;i++) {
		printf("%c",(unsigned char) bufr[i]);
	}
	printf("\n");

	
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void AEt_EMC2000::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN COMMAND";
}

//<-dceag-sample-b->!
//<-dceag-sample-e->

/*

	COMMANDS TO IMPLEMENT

*/


