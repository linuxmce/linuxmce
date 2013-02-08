/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#include "DCEConfig.h"
#include "Logger.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <list>
#include "../LIRC_DCE/IRReceiverBase.h"
#include "../PlutoUtils/ProcessUtils.h"
#include <sys/ioctl.h>
#include <fcntl.h>

#define  VERSION "<=version=>"

using namespace std;
using namespace DCE;

#define HAVE_STDBOOL_H
#define AVWizard_Port	28949
#define AVWizard_Host	"localhost"

extern "C" 
{
		#include <hid.h>
		#include <usb.h>
}

bool g_bQuit=false;

void sig_int(int sig)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Got signal");
	g_bQuit=true;
}

bool ProcessBindRequest(usb_dev_handle *p_usb_dev_handle,char *inPacket)
{
	unsigned char *pSerialNumber = (unsigned char *) inPacket; // Unsigned so it's not negative numbers
	char sSerialNumber[30];
	sprintf(sSerialNumber,"%x.%x.%x.%x",(int) pSerialNumber[2],(int) pSerialNumber[3],(int) pSerialNumber[4],(int) pSerialNumber[5]);
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"got a bind request for %s",sSerialNumber);

	char write_packet[5];
	write_packet[0]=8;
	write_packet[1]=0x20;
	write_packet[2]=(char) 0xff;
	write_packet[3]=0;
	int ctrl = usb_control_msg(p_usb_dev_handle, 0x21, 0x9, 8+(0x03<<8) /*int value*/, 1 /* int index */, write_packet, 4, 250);
	if (ctrl<0)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"usb_control_msg %d\n",(int) ctrl);
		perror("error: ");
		return false;
	}

	char * args[] = { "/usr/bin/beep", "-f","2000","-n","-f","1800","-n","-f","1600","-n","-f","1800","-n","-f","2000", NULL };
	ProcessUtils::SpawnDaemon(args[0], args, false);

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"wrote message %d",ctrl);
	return true;
}

void StartAVWizard()
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"WatchGyroRemote StartAVWizard");
	DCEConfig dceConfig;
	dceConfig.AddString("AVWizardOverride","1");
	dceConfig.WriteSettings();
	system("beep -f 2000");
}

void CheckShiftState();

int main(int argc, char *argv[])
{
	LoggerWrapper::SetType(LT_LOGGER_FILE,"/var/log/pluto/WatchGyro.log");
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"WatchGyroRemote starting");

	signal(SIGINT, sig_int);
	signal(SIGTERM, sig_int);

	usb_dev_handle *p_usb_dev_handle;
	struct usb_bus *busses;
	usb_set_debug(255);

	IRReceiverBase irReceiverBase(NULL);

	while(!g_bQuit)
	{
		usb_init();
		usb_find_busses();
		usb_find_devices();

		busses = usb_get_busses();

		struct usb_bus *bus;
		//int c, i, a;

		/* ... */

		for (bus = busses; bus; bus = bus->next) 
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"WatchGyroRemote bus %s",bus->dirname);
			struct usb_device *dev;

			for (dev = bus->devices; dev; dev = dev->next) {

				LoggerWrapper::GetInstance()->Write(LV_STATUS,"WatchGyroRemote %04x:%04x\n", dev->descriptor.idVendor, dev->descriptor.idProduct);
				if ( (dev->descriptor.idVendor==0x0c16) && (dev->descriptor.idProduct==0x0006) )  // The gyration remote
				{
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"WatchGyroRemote device found!");

					p_usb_dev_handle = usb_open(dev);

					int res = 0;
					res = usb_claim_interface(p_usb_dev_handle, 1);
					if (res<0)
					{ 
						LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"WatchGyroRemote claim interface: %i\n", res);
						perror("error: ");
						break;
					}

					char inPacket[6];
					int cnt=0;
					while(!g_bQuit)
					{
						res = usb_interrupt_read(p_usb_dev_handle, 0x82, inPacket, 6, 250);
						if (res<0&&res!=-110) break;

						CheckShiftState();

						if (res<=0)
						{
							if (cnt%100==20) 
								LoggerWrapper::GetInstance()->Write(LV_STATUS,"WatchGyroRemote %d .", cnt++);
							cnt++;
						}
						else
						{
							unsigned char *pPtr = (unsigned char *) inPacket;
							LoggerWrapper::GetInstance()->Write(LV_STATUS,"WatchGyroRemote [READER] %04i.%03i: read bytes: %d %x.%x.%x.%x.%x.%x", 
								cnt/100, cnt%100, res, (int) pPtr[0],(int) pPtr[1],(int) pPtr[2],(int) pPtr[3],(int) pPtr[4],(int) pPtr[5]);

							if( res==6 && inPacket[0]==8 )  // It's for us
							{
								if( inPacket[1]==0x20 || inPacket[1]==0x26 )  // A bind request
								{
									ProcessBindRequest(p_usb_dev_handle,inPacket);
									system("beep -f 200");
								}
								else if( inPacket[1]==0x25 )  // A button
								{
									LoggerWrapper::GetInstance()->Write(LV_STATUS,"WatchGyroRemote button %d %d %d %d",
										(int) inPacket[2],(int) inPacket[3],(int) inPacket[4],(int) inPacket[5]);
									if( inPacket[3]==-62 )  // The AV Menu button
									{
										LoggerWrapper::GetInstance()->Write(LV_STATUS,"WatchGyroRemote button -- activating AV Wizard");
										StartAVWizard();
									}
									else if( inPacket[3]==-99 )
									{
										LoggerWrapper::GetInstance()->Write(LV_STATUS,"WatchGyroRemote button -- sending chdown");
										irReceiverBase.ForceKeystroke("chdown", AVWizard_Host, AVWizard_Port);
									}
									else if( inPacket[3]==-100 )
									{
										LoggerWrapper::GetInstance()->Write(LV_STATUS,"WatchGyroRemote button -- sending chup");
										irReceiverBase.ForceKeystroke("chup", AVWizard_Host, AVWizard_Port);
									}
									else if( inPacket[3]==-22 )
									{
										LoggerWrapper::GetInstance()->Write(LV_STATUS,"WatchGyroRemote button -- sending voldown");
										irReceiverBase.ForceKeystroke("voldn", AVWizard_Host, AVWizard_Port);
									}
									else if( inPacket[3]==-23 )
									{
										LoggerWrapper::GetInstance()->Write(LV_STATUS,"WatchGyroRemote button -- sending vol up");
										irReceiverBase.ForceKeystroke("volup", AVWizard_Host, AVWizard_Port);
									}
									else if( inPacket[3]==-63 )
									{
										LoggerWrapper::GetInstance()->Write(LV_STATUS,"WatchGyroRemote button -- sending back");
										irReceiverBase.ForceKeystroke("back", AVWizard_Host, AVWizard_Port);
									}
								}
							}
						}
					}

					LoggerWrapper::GetInstance()->Write(LV_STATUS,"WatchGyroRemote");
					usb_release_interface(p_usb_dev_handle, 1);
					usb_close(p_usb_dev_handle);
				}
			}
		}
		if( !g_bQuit )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Didn't get remote.  Sleep 2 seconds and try again");
			CheckShiftState();
			Sleep(2000);
		}
	}
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Exiting");
	return 0;
}


void CheckShiftState()
{
	char c = 6;
	for(int i=0;i<10;++i)
	{
		char Console[10] = "/dev/ttyX";
                Console[8] = '0' + i;
                int fd = open(Console, O_RDONLY);
		if( !fd )
			continue;
                int iResult = ioctl(fd, TIOCLINUX, &c);
                close(fd);
		if( iResult==0 && c==1 )
			StartAVWizard();
	}
}
