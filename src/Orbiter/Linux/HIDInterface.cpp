/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
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
#include "DCE/Logger.h"
#include "../Orbiter.h"
#include "HIDInterface.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "../OrbiterFileBrowser.h"
#include "../PlutoUtils/ProcessUtils.h"

#include <stdio.h>
#include <string.h>

void *ProcessHIDEvents(void *p)
{
	PlutoHIDInterface *pPlutoHIDInterface = (PlutoHIDInterface *) p;
	pPlutoHIDInterface->ProcessHIDEvents();
	return NULL;
}

PlutoHIDInterface::PlutoHIDInterface(Orbiter *pOrbiter) : m_HIDMutex("HID")
{
	m_pOrbiter=pOrbiter; 
	m_p_usb_dev_handle=NULL; 
	m_bRunning=false;
	m_HIDMutex.Init(&m_pOrbiter->m_MutexAttr);
	m_iRemoteID=m_iPK_Device_Remote=0;
	m_iHoldingDownButton=0;
	m_MouseStartStop=mssNone;
	m_iRepeat=0;
	m_tLastButtonPress=0;
	m_iLastButtonPress=0;
}

/**
 * A hack to remove the modules so that the detection loop will work.
 */
void PlutoHIDInterface::modules_remove()
{
//	system("rmmod hid_gyration");
//	system("rmmod usbhid");
}

void PlutoHIDInterface::modules_reload()
{
//	system("modprobe usbhid");
//	system("modprobe hid_gyration");
}

void PlutoHIDInterface::ProcessHIDEvents()
{
	struct usb_bus *busses;
	usb_set_debug(255);

	usb_init();
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"PlutoHIDInterface::ProcessHIDEvents starting");
#endif

	while(!m_pOrbiter->m_bQuit_get())  // an outer loop so this will retry a connect if the remote is removed and reconnected
	{
		modules_remove();
		usb_find_busses();
		usb_find_devices();

		busses = usb_get_busses();

		struct usb_bus *bus;
		//int c, i, a;

		/* ... */

		for (bus = busses; bus; bus = bus->next) 
		{
	#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"PlutoHIDInterface::ProcessHIDEvents bus %s",bus->dirname);
	#endif
			struct usb_device *dev;

			for (dev = bus->devices; dev; dev = dev->next) {

	#ifdef DEBUG
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"PlutoHIDInterface::ProcessHIDEvents %04x:%04x\n", dev->descriptor.idVendor, dev->descriptor.idProduct);
	#endif
				if ( (dev->descriptor.idVendor==0x0c16) && (dev->descriptor.idProduct==0x0006) )  // The gyration remote
				{
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"PlutoHIDInterface::ProcessHIDEvents device found!");

					m_p_usb_dev_handle = usb_open(dev);

					int res = 0;
					res = usb_claim_interface(m_p_usb_dev_handle, 1);
					if (res<0)
					{ 
						LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PlutoHIDInterface::ProcessHIDEvents claim interface: %i\n", res);
						perror("error: ");
						break;
					}

					char inPacket[6];
					int cnt=0;
					while(!m_pOrbiter->m_bQuit_get())
					{
						PLUTO_SAFETY_LOCK_ERRORSONLY(hm,m_HIDMutex);
						m_bRunning=true;
						res = usb_interrupt_read(m_p_usb_dev_handle, 0x82, inPacket, 6, 250);
						if (res<0&&res!=-110) break;
						if (res<=0)
						{
							if (cnt%100==20) 
								LoggerWrapper::GetInstance()->Write(LV_STATUS,"PlutoHIDInterface::ProcessHIDEvents %d .", cnt++);

							if( m_MouseStartStop!=mssNone )
							{
								PLUTO_SAFETY_LOCK(vm,m_pOrbiter->m_VariableMutex);
								if( m_MouseStartStop==mssStart )
									DoStartMouse();
								else
									DoStopMouse();
								m_MouseStartStop=mssNone;
							}

							hm.Release(); 
							modules_reload(); 
							usleep(10000);  // Give somebody else a chance to use this
							hm.Relock();
							cnt++;
						}
						else
						{
							unsigned char *pPtr = (unsigned char *) inPacket;
							LoggerWrapper::GetInstance()->Write(LV_STATUS,"PlutoHIDInterface::ProcessHIDEvents [READER] %04i.%03i: read bytes: %d %x.%x.%x.%x.%x.%x", 
								cnt/100, cnt%100, res, (int) pPtr[0],(int) pPtr[1],(int) pPtr[2],(int) pPtr[3],(int) pPtr[4],(int) pPtr[5]);

							if( !m_pOrbiter->m_pOrbiterFileBrowser_Collection )
								LoggerWrapper::GetInstance()->Write(LV_STATUS,"PlutoHIDInterface::ProcessHIDEvents skipping until Orbiter finishes startup");
							else if( res==6 && inPacket[0]==8 )  // It's for us
							{
								if( inPacket[1]==0x20 || inPacket[1]==0x26 )  // A bind request
									ProcessBindRequest(inPacket);
								else if( inPacket[1]==0x25 )  // A button
									ProcessHIDButton(inPacket);
							}
						}
					}

					LoggerWrapper::GetInstance()->Write(LV_STATUS,"PlutoHIDInterface::ProcessHIDEvents quit %d", (int) m_pOrbiter->m_bQuit_get());
					usb_release_interface(m_p_usb_dev_handle, 1);
					usb_close(m_p_usb_dev_handle);
				}
			}
		}

		if( !m_pOrbiter->m_bQuit_get() )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"PlutoHIDInterface::ProcessHIDEvents didn't get remote.  Sleep 2 seconds and try again");
			Sleep(2000);
		}
	}

	m_bRunning=false;
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"PlutoHIDInterface::ProcessHIDEvents Exiting");
}

bool PlutoHIDInterface::ProcessBindRequest(char *inPacket)
{
	unsigned char *pSerialNumber = (unsigned char *) inPacket; // Unsigned so it's not negative numbers
	char sSerialNumber[30];
	sprintf(sSerialNumber,"%x.%x.%x.%x",(int) pSerialNumber[2],(int) pSerialNumber[3],(int) pSerialNumber[4],(int) pSerialNumber[5]);
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"PlutoHIDInterface::ProcessBindRequest got a bind request for %s",sSerialNumber);

	pair<int,int> ipair = m_pOrbiter->m_pOrbiterFileBrowser_Collection->m_mapRemoteControls_Find(sSerialNumber);
	int PK_Device=ipair.first,RemoteID=ipair.second;
	
	if( RemoteID==0 )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"PlutoHIDInterface::ProcessBindRequest requesting remote ID...");
		DCE::CMD_Get_Remote_ID CMD_Get_Remote_ID(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_OrbiterPlugIn,sSerialNumber,&PK_Device,&RemoteID);
		if( !m_pOrbiter->SendCommand(CMD_Get_Remote_ID) || PK_Device==0 || RemoteID==0 )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PlutoHIDInterface::ProcessBindRequest failed to get RemoteID answer from orbiter plugin");
			RemoteID=255; // A bogus number
		}
		m_pOrbiter->m_pOrbiterFileBrowser_Collection->m_mapRemoteID_Device[ RemoteID ] = PK_Device;
		m_pOrbiter->m_pOrbiterFileBrowser_Collection->m_mapRemoteControls[ sSerialNumber ] = make_pair<int,int> (PK_Device,RemoteID);
	}
	else
		PK_Device=m_pOrbiter->m_pOrbiterFileBrowser_Collection->m_mapRemoteID_Device_Find(RemoteID);

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"PlutoHIDInterface::ProcessBindRequest we are remote %d device %d",RemoteID,PK_Device);
	char write_packet[5];
	write_packet[0]=8;
	write_packet[1]=0x20;
	write_packet[2]=(char) RemoteID;
	write_packet[3]=0;
	int ctrl = usb_control_msg(m_p_usb_dev_handle, 0x21, 0x9, 8+(0x03<<8) /*int value*/, 1 /* int index */, write_packet, 4, 250);
	if (ctrl<0)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PlutoHIDInterface::ProcessBindRequest  usb_control_msg %d\n",(int) ctrl);
		perror("error: ");
		return false;
	}

	// Make a beep so the user knows we connected
	char * args[] = { "/usr/bin/beep", "-f","2000","-n","-f","1800","-n","-f","1600","-n","-f","1800","-n","-f","2000", NULL };
	ProcessUtils::SpawnDaemon(args[0], args, false);

	m_pOrbiter->CMD_Display_Alert("Remote " + StringUtils::itos(PK_Device) + " connected","connectremote","3",interuptAlways);
	m_pOrbiter->GotActivity(0);  // In case the tv is off or the screen saver
	SetActiveRemote(RemoteID,inPacket[1]==0x26);
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"PlutoHIDInterface::ProcessBindRequest remote %d PK_Device %d follow: %d",RemoteID,PK_Device,(int) inPacket[1]==0x26);
	return true;
}

bool PlutoHIDInterface::Rebind()
{
	char write_packet[5];
	write_packet[0]=8;
	write_packet[1]=0x51;
	write_packet[2]=0xff;
	write_packet[3]=0;
	int ctrl = usb_control_msg(m_p_usb_dev_handle, 0x21, 0x9, 8+(0x03<<8) /*int value*/, 1 /* int index */, write_packet, 4, 250);
	if (ctrl<0)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PlutoHIDInterface::Rebind  usb_control_msg %d\n",(int) ctrl);
		perror("error: ");
		return false;
	}
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"PlutoHIDInterface::Rebind wrote message %d",ctrl);
	return true;
}

bool PlutoHIDInterface::DoStartMouse()
{
	PLUTO_SAFETY_LOCK(hm,m_HIDMutex);
	if( !m_bRunning )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"PlutoHIDInterface::StartMouse m_bRunning==false");
		return false;

	}
	char write_packet[5];
	write_packet[0]=8;
	write_packet[1]=0x60;
	write_packet[2]=(char) m_iRemoteID;
	write_packet[3]=0;
	int ctrl = usb_control_msg(m_p_usb_dev_handle, 0x21, 0x9, 8+(0x03<<8) /*int value*/, 1 /* int index */, write_packet, 4, 250);
	if (ctrl<0)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PlutoHIDInterface::StartMouse  usb_control_msg %d id %d\n",(int) ctrl,(int) m_iRemoteID);
		perror("error: ");
		return false;
	}
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"PlutoHIDInterface::StartMouse remote %d",m_iRemoteID);
#endif
	return true;
}

bool PlutoHIDInterface::DoStopMouse()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(hm,m_HIDMutex);
	if( !m_bRunning )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"PlutoHIDInterface::StopMouse m_bRunning==false");
		return false;

	}
	char write_packet[5];
	write_packet[0]=8;
	write_packet[1]=0x30;
	write_packet[2]=(char) m_iRemoteID;
	write_packet[3]=0;
	int ctrl = usb_control_msg(m_p_usb_dev_handle, 0x21, 0x9, 8+(0x03<<8) /*int value*/, 1 /* int index */, write_packet, 4, 250);
	if (ctrl<0)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PlutoHIDInterface::StopMouse  usb_control_msg %d id %d\n",(int) ctrl,(int) m_iRemoteID);
		perror("error: ");
		return false;
	}
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"PlutoHIDInterface::StopMouse remote %d",m_iRemoteID);
#endif
	return true;
}

bool PlutoHIDInterface::SetActiveRemote(int iRemoteID,bool bFollowMe)
{
	int PK_Device = m_pOrbiter->m_pOrbiterFileBrowser_Collection->m_mapRemoteID_Device_Find(iRemoteID);
	if( !PK_Device )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PlutoHIDInterface::SetActiveRemote Remote ID %d is unknown",iRemoteID);
		return false;
	}
	m_pOrbiter->CMD_Display_Alert("Remote " + StringUtils::itos(PK_Device) + " active","actremote","3",interuptAlways);
	m_iPK_Device_Remote = PK_Device;
	m_iRemoteID = iRemoteID;
	DCE::CMD_Set_Active_Remote CMD_Set_Active_Remote(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_OrbiterPlugIn,m_iPK_Device_Remote,
			bFollowMe,m_pOrbiter->m_dwPK_Device);
	m_pOrbiter->SendCommand(CMD_Set_Active_Remote);
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"PlutoHIDInterface::SetActiveRemote new remote %d device %d",m_iRemoteID,m_iPK_Device_Remote );
	return true;
}

void PlutoHIDInterface::LegacyBind()
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"PlutoHIDInterface::LegacyBind");
	PLUTO_SAFETY_LOCK_ERRORSONLY(hm,m_HIDMutex);
	if( !m_bRunning )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"PlutoHIDInterface::LegacyBind m_bRunning==false");
		return;

	}
	char write_packet[5];
	write_packet[0]=8;
	write_packet[1]=0x40;
	write_packet[2]=(char) m_iRemoteID;
	write_packet[3]=0;
	int ctrl = usb_control_msg(m_p_usb_dev_handle, 0x21, 0x9, 8+(0x03<<8) /*int value*/, 1 /* int index */, write_packet, 4, 250);
	if (ctrl<0)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PlutoHIDInterface::LegacyBind  usb_control_msg %d\n",(int) ctrl);
		perror("error: ");
		return;
	}
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"PlutoHIDInterface::LegacyBind done");
	return;

}

bool PlutoHIDInterface::ProcessHIDButton(char *inPacket)
{
	unsigned char *p_Packet = (unsigned char *) inPacket;

	int iRemoteID = p_Packet[2];

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"PlutoHIDInterface::ProcessHIDButton for remote %d/%d %d %d %d %d",
		iRemoteID,m_iRemoteID,(int) p_Packet[2],(int) p_Packet[3],(int) p_Packet[4],(int) p_Packet[5]);

	if( iRemoteID==0xff )
	{
		m_pOrbiter->CMD_Display_Alert("Press Connect on your remote","mustconnectremote","5",interuptAlways);
		m_pOrbiter->GotActivity(0);  // In case the tv is off or the screen saver
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"PlutoHIDInterface::ProcessHIDButton Remote was bound while booting up and has ID ff.  Re-connecting...");
		Rebind();
	}
	else if( iRemoteID!=m_iRemoteID )
	{
		if( SetActiveRemote(iRemoteID,false)==false )
		{
			m_pOrbiter->CMD_Display_Alert("Press Connect on your remote","mustconnectremote","5",interuptAlways);
			m_pOrbiter->GotActivity(0);  // In case the tv is off or the screen saver
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PlutoHIDInterface::ProcessHIDButton user needs to connect first");
			Rebind();
		}
	}
	
	// If p_Packet[3]==0 then this is notifying us that the button was released.  If it's not, it's notifying us that a button was pressed
	// if m_iHoldingDownButton is !0, then we're already reporting a button as being depressed, so we need to fire a button up if the 
	// user is pressing another button
	if( m_iHoldingDownButton )
	{
		Orbiter::Event *pEvent = new Orbiter::Event;
		pEvent->type=Orbiter::Event::BUTTON_UP;
		pEvent->data.button.m_iPK_Button = 0;
		pEvent->data.button.m_iKeycode = m_iHoldingDownButton + 100000;
		pEvent->data.button.m_bSimulated = false;
		m_pOrbiter->CallMaintenanceInMiliseconds(0, &Orbiter::QueueEventForProcessing, pEvent, pe_NO, false );
		if( p_Packet[3]==0 )
		{
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"PlutoHIDInterface::ProcessHIDButton button %d was released",m_iHoldingDownButton);
#endif
			m_iHoldingDownButton=0;  // we're not holding down any buttons anymore
			return true;
		}
	}

	if( p_Packet[3]==0 )
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"PlutoHIDInterface::ProcessHIDButton got a button up with nothing down");
		return true; // Shouldn't get here because m_iHoldingDownButton should be non-null if we're getting a button up
	}

	// Special hacks to power down or reboot when you don't have a picture on the screen.  If the user presses power 4 times in
	// a row with 2 seconds or delay between each, and then presses vol up or ch up, that's a reboot or power off
	if( m_iLastButtonPress==p_Packet[3] && m_tLastButtonPress && time(NULL)-m_tLastButtonPress<=2 )
		m_iRepeat++;
	else
	{
		if( m_iLastButtonPress==130 && m_iRepeat>=3 && time(NULL)-m_tLastButtonPress<=2 && (p_Packet[3]==233 /*volup*/ || p_Packet[3]==156 /*chup*/ ) )
		{
			DCE::CMD_Halt_Device CMD_Halt_Device(m_pOrbiter->m_dwPK_Device, m_pOrbiter->m_dwPK_Device_GeneralInfoPlugIn,
				m_pOrbiter->m_dwPK_Device,p_Packet[3]==233 ? "R" : "H","");
			m_pOrbiter->SendCommand(CMD_Halt_Device);
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"PlutoHIDInterface::ProcessHIDButton %s",p_Packet[3]==233 ? "R"  /*volup*/ : "H" /*chup*/ );
			return true;
		}
		else
			m_iRepeat = 0;
	}

	m_iLastButtonPress=p_Packet[3];
	m_tLastButtonPress=time(NULL);
	m_iHoldingDownButton=p_Packet[3];

	Orbiter::Event *pEvent = new Orbiter::Event;
	pEvent->type=Orbiter::Event::BUTTON_DOWN;
	pEvent->data.button.m_bSimulated = false;
	pEvent->data.button.m_iPK_Button = 0;
	pEvent->data.button.m_iKeycode = m_iHoldingDownButton + 100000; // We're using especially high numbers, > 100000 for these special buttons
    
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"PlutoHIDInterface::ProcessHIDButton m_iHoldingDownButton %d keycode %d m_iRepeat %d m_tLastButtonPress %d", 
		m_iHoldingDownButton, pEvent->data.button.m_iKeycode, m_iRepeat, m_tLastButtonPress);

	m_pOrbiter->CallMaintenanceInMiliseconds(0, &Orbiter::QueueEventForProcessing, pEvent, pe_NO, false );
	return true;
}


/*
	Kirill's note:
	short note:
	usb_control_msg(m_p_usb_dev_handle, 0x21, 9, 8+(0x03<<8) , 1 , outPacket, 4, 250);
	you see here the combination:
	8+(0x03<<8) , 1

	this is from hid sources : reportID + (HID_RT_FEATURE<<8),   interfaceID
	seems, this comes directly from USB HID specs, so I decided to keep it
	its in sources of libhid , src/hid_exchange.c   - in function function hid_set_output_report
	just for reference
	yep, so now we know everything 
	the 0x21: USB_ENDPOINT_OUT + USB_TYPE_CLASS + USB_RECIP_INTERFACE
	09: HID_REPORT_SET
*/
