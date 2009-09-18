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
#include "Tongdy_RS485.h"
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
#include <stdint.h>
#include <termios.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/file.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

int serial_read (int dev,unsigned char pnt[],int len,long timeout)
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
                bytes = select (dev+1,&fs,NULL,NULL,&tv);
                if( bytes <= 0 )
                {
                       return total;
                }

                bytes = read (dev,pnt+total,len-total);
                total += bytes;
        }
	return total;
}

int serial_write (int dev,unsigned char pnt[],int len)
{
        int res;

        res = write (dev,pnt,len);
        if (res != len) {
                        exit (-1);
                return (ERR_TIMEOUT);
        }
        tcflush(dev, TCIOFLUSH);

        return (0);
}

extern "C" void *start( void* );
void *start( void *p )
{
	DCE::Tongdy_RS485 *base = static_cast<DCE::Tongdy_RS485*>(p);
	base->receiveFunction();
	return NULL;
}


//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Tongdy_RS485::Tongdy_RS485(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Tongdy_RS485_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
Tongdy_RS485::Tongdy_RS485(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: Tongdy_RS485_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
}

//<-dceag-dest-b->
Tongdy_RS485::~Tongdy_RS485()
//<-dceag-dest-e->
{
	
}

//<-dceag-getconfig-b->
bool Tongdy_RS485::GetConfig()
{
	if( !Tongdy_RS485_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated
	//
	string serialport = TranslateSerialUSB(DATA_Get_COM_Port_on_PC());
	
	if ((port = open(serialport.c_str(), O_RDWR | O_NOCTTY)) < 0) return (ERR_OPEN);	
	struct termios portterm;

        if (!isatty(port)) {
                close(port);
                return (ERR_OPEN);
        }
        if (flock(port, LOCK_EX | LOCK_NB) < 0) {
                close(port);
                return (ERR_FLOCK);
        }
        portterm.c_cflag = CS8 | CREAD | CLOCAL;

        portterm.c_cc[VMIN] = 1; 
        portterm.c_cc[VTIME] = 0;

        cfsetispeed(&portterm, BAUDRATE);
        cfsetospeed(&portterm, BAUDRATE);

        portterm.c_lflag = 0;

        portterm.c_iflag = IGNBRK;
        portterm.c_oflag = 0;


        tcflush(port, TCIOFLUSH);
        if (tcsetattr(port, TCSANOW, &portterm) < 0) {
                close(port);
                return (ERR_STTY);
        }
        usleep (1000);  
	tcflush(port, TCIOFLUSH);

	static pthread_t readThread;
	pthread_create(&readThread, NULL, start, (void*)this);

	return true;
//
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Tongdy_RS485::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
Tongdy_RS485_Command *Create_Tongdy_RS485(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new Tongdy_RS485(pPrimaryDeviceCommand, pData, pEvent, pRouter);
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
void Tongdy_RS485::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void Tongdy_RS485::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN COMMAND";
}

//<-dceag-sample-b->
/*		**** SAMPLE ILLUSTRATING HOW TO USE THE BASE CLASSES ****

**** IF YOU DON'T WANT DCEGENERATOR TO KEEP PUTTING THIS AUTO-GENERATED SECTION ****
**** ADD AN ! AFTER THE BEGINNING OF THE AUTO-GENERATE TAG, LIKE //<=dceag-sample-b->! ****
Without the !, everything between <=dceag-sometag-b-> and <=dceag-sometag-e->
will be replaced by DCEGenerator each time it is run with the normal merge selection.
The above blocks are actually <- not <=.  We don't want a substitution here

void Tongdy_RS485::SomeFunction()
{
	// If this is going to be loaded into the router as a plug-in, you can implement: 	virtual bool Register();
	// to do all your registration, such as creating message interceptors

	// If you use an IDE with auto-complete, after you type DCE:: it should give you a list of all
	// commands and requests, including the parameters.  See "AllCommandsRequests.h"

	// Examples:
	
	// Send a specific the "CMD_Simulate_Mouse_Click" command, which takes an X and Y parameter.  We'll use 55,77 for X and Y.
	DCE::CMD_Simulate_Mouse_Click CMD_Simulate_Mouse_Click(m_dwPK_Device,OrbiterID,55,77);
	SendCommand(CMD_Simulate_Mouse_Click);

	// Send the message to orbiters 32898 and 27283 (ie a device list, hence the _DL)
	// And we want a response, which will be "OK" if the command was successfull
	string sResponse;
	DCE::CMD_Simulate_Mouse_Click_DL CMD_Simulate_Mouse_Click_DL(m_dwPK_Device,"32898,27283",55,77)
	SendCommand(CMD_Simulate_Mouse_Click_DL,&sResponse);

	// Send the message to all orbiters within the house, which is all devices with the category DEVICECATEGORY_Orbiter_CONST (see pluto_main/Define_DeviceCategory.h)
	// Note the _Cat for category
	DCE::CMD_Simulate_Mouse_Click_Cat CMD_Simulate_Mouse_Click_Cat(m_dwPK_Device,DEVICECATEGORY_Orbiter_CONST,true,BL_SameHouse,55,77)
    SendCommand(CMD_Simulate_Mouse_Click_Cat);

	// Send the message to all "DeviceTemplate_Orbiter_CONST" devices within the room (see pluto_main/Define_DeviceTemplate.h)
	// Note the _DT.
	DCE::CMD_Simulate_Mouse_Click_DT CMD_Simulate_Mouse_Click_DT(m_dwPK_Device,DeviceTemplate_Orbiter_CONST,true,BL_SameRoom,55,77);
	SendCommand(CMD_Simulate_Mouse_Click_DT);

	// This command has a normal string parameter, but also an int as an out parameter
	int iValue;
	DCE::CMD_Get_Signal_Strength CMD_Get_Signal_Strength(m_dwDeviceID, DestDevice, sMac_address,&iValue);
	// This send command will wait for the destination device to respond since there is
	// an out parameter
	SendCommand(CMD_Get_Signal_Strength);  

	// This time we don't care about the out parameter.  We just want the command to 
	// get through, and don't want to wait for the round trip.  The out parameter, iValue,
	// will not get set
	SendCommandNoResponse(CMD_Get_Signal_Strength);  

	// This command has an out parameter of a data block.  Any parameter that is a binary
	// data block is a pair of int and char *
	// We'll also want to see the response, so we'll pass a string for that too

	int iFileSize;
	char *pFileContents
	string sResponse;
	DCE::CMD_Request_File CMD_Request_File(m_dwDeviceID, DestDevice, "filename",&pFileContents,&iFileSize,&sResponse);
	SendCommand(CMD_Request_File);

	// If the device processed the command (in this case retrieved the file),
	// sResponse will be "OK", and iFileSize will be the size of the file
	// and pFileContents will be the file contents.  **NOTE**  We are responsible
	// free deleting pFileContents.


	// To access our data and events below, you can type this-> if your IDE supports auto complete to see all the data and events you can access

	// Get our IP address from our data
	string sIP = DATA_Get_IP_Address();

	// Set our data "Filename" to "myfile"
	DATA_Set_Filename("myfile");

	// Fire the "Finished with file" event, which takes no parameters
	EVENT_Finished_with_file();
	// Fire the "Touch or click" which takes an X and Y parameter
	EVENT_Touch_or_click(10,150);
}
*/
//<-dceag-sample-e->

/*

	COMMANDS TO IMPLEMENT

*/

void Tongdy_RS485::decodeFrame(unsigned char *buf, int length) {

	int debug = 1;
	int verbose = 1;
	int i;

	if(debug) {
		for(i=0;i<length;i++) {
			printf("0x%x ",buf[i]);
		}
		printf("\n");
	}

	if ( length!=20) {
		printf("ERROR in frame, size invalid!\n");
	} else {
		if ((buf[0]!=0xff) &&(buf[19]!=0x80)) {
			printf("ERROR in frame, SOF/EOF error!\n");
		} else {
			// frame looks good
			//
			char tmp_value[2048];
			int child_device_id =26;
			// m_pEvent->SendMessage( new Message(pChildDevice->m_dwPK_Device,
			sprintf(tmp_value, "%d", buf[4]/2);
			m_pEvent->SendMessage( new Message(child_device_id,
				DEVICEID_EVENTMANAGER,
				PRIORITY_NORMAL,
				MESSAGETYPE_EVENT,
				EVENT_Temperature_Changed_CONST, 1, 
				EVENTPARAMETER_Value_CONST, tmp_value)
			);
			sprintf(tmp_value, "%d", buf[9]*100+buf[10]);
			m_pEvent->SendMessage( new Message(child_device_id,
				DEVICEID_EVENTMANAGER,
				PRIORITY_NORMAL,
				MESSAGETYPE_EVENT,
				EVENT_CO2_Level_Changed_CONST,
				1,
				EVENTPARAMETER_Value_CONST,
				tmp_value)
			);
			sprintf(tmp_value, "%d", buf[6]);
			m_pEvent->SendMessage( new Message(child_device_id,
				DEVICEID_EVENTMANAGER,
				PRIORITY_NORMAL,
				MESSAGETYPE_EVENT,
				EVENT_Humidity_Changed_CONST,
				1,
				EVENTPARAMETER_Value_CONST,
				tmp_value)
			);
			sprintf(tmp_value, "%d", buf[13]*100 + buf[14]);
			m_pEvent->SendMessage( new Message(child_device_id,
				DEVICEID_EVENTMANAGER,
				PRIORITY_NORMAL,
				MESSAGETYPE_EVENT,
				EVENT_Brightness_Changed_CONST,
				1,
				EVENTPARAMETER_Value_CONST,
				tmp_value)
			);

			char query[4096];	
				// LV_STATUS
			DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Node: %i Cmd: 0x%x T:%f/%f H:%i/%i CO2:%i/%i",buf[1],buf[2],buf[4]/2,buf[3]/2,buf[6],buf[5],buf[9]*100 + buf[10],buf[7]*100 + buf[8]);
			if (verbose) {	
		//		printf("Node: %i,\n",buf[1]);
		//		printf("Intercepted command: 0x%x;\n",buf[2]);
		//		printf("Temp act: %i degC setpoint: %i degC;\n",buf[4]/2,buf[3]/2);
		//		printf("Humidity act: %i setpoint: %i;\n",buf[6],buf[5]);
		//		printf("CO2 act: %i ppm setpoint: %i ppm;\n",buf[9]*100 + buf[10],buf[7]*100 + buf[8]);
				printf("Photosensitive act: %i setpoint: %i;\n",buf[13]*100 + buf[14],buf[11]*100 + buf[12]);
				printf("Relay state: 1 [%s] 2 [%s] 3 [%s];\n",
					buf[17] & 2 ? "on" : "off",
					buf[17] & 8 ? "on" : "off",
					buf[17] & 32 ? "on" : "off"
				);
				printf("Montor state: %s (%s), %s and %s, %s mode\n",
					buf[18] & 1 ? "On" : "Off",
					buf[18] & 2 ? "locked" : "unlocked",
					buf[18] & 8 ? "cool" : "heat",
					buf[18] & 16 ? "dehumidify" : "humidify",
					buf[18] & 32 ? "night" : "day"
				);
			}
		}
	}

}

void *Tongdy_RS485::receiveFunction() {
	unsigned char buf[6];
	unsigned char bufr[20];

	DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"receiveFunction started");


	buf[0] = 0xff; // header
	buf[1] = 0; // address
	buf[2] = 0; // command
	buf[3] = 0; // param 1
	buf[4] = 0; // param 2
	buf[5] = 0x80; // frame tail

	while(1) {
		serial_write(port,(unsigned char*)buf,6);
		DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"polling devices..");
		if (serial_read(port, bufr, 20,5)==20) {
			decodeFrame(bufr,20);
		}
		sleep(15);
	}

}
