/*
     Copyright (C) 2013 LinuxMCE

     www.linuxmce.org


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#include "Bluetooth_Adapter.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

void * StartAdapterRunloop(void * arg)
{
  DCE::Bluetooth_Adapter *pBluetooth_Adapter = (DCE::Bluetooth_Adapter *) arg;
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
  pBluetooth_Adapter->AdapterRunloop();
  return NULL;
}

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Bluetooth_Adapter::Bluetooth_Adapter(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Bluetooth_Adapter_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Bluetooth_Adapter::~Bluetooth_Adapter()
//<-dceag-dest-e->
{
	
}

//<-dceag-getconfig-b->
bool Bluetooth_Adapter::GetConfig()
{
	if( !Bluetooth_Adapter_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated
	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Bluetooth_Adapter::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->!

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void Bluetooth_Adapter::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void Bluetooth_Adapter::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN COMMAND";
}

//<-dceag-sample-b->!

/**
 * CreateChildren - use to spawn the adapter runloop
 */

void Bluetooth_Adapter::CreateChildren()
{
  LoggerWrapper::GetInstance()->Write(LV_WARNING,"Bluetooth_Adapter::CreateChildren() - starting adapter runloop.");
  if (pthread_create(&m_Adapter_Runloop_Thread, NULL, StartAdapterRunloop, (void *) this))
    {
      m_Adapter_Runloop_Thread = (pthread_t)NULL;
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Bluetooth_Adapter::CreateChildren() - failed to create Adapter Runloop Thread");
      m_bQuit_set(true);
      exit(1);
    }
}

/**
 * The adapter runloop itself.
 */
void Bluetooth_Adapter::AdapterRunloop()
{
    inquiry_info *ii = NULL;
    int max_rsp, num_rsp;
    int dev_id, sock, len, flags;
    int i;
    char addr[19] = { 0 };
    char name[248] = { 0 };

  while (!m_bQuit_get())
    {
      dev_id = hci_get_route(NULL);
      sock = hci_open_dev( dev_id );
      if (dev_id < 0 || sock < 0) {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Bluetooth_Adapter::AdapterRunloop() - Could not open socket to adapter.");
	break;
      }
      len = 8;
      max_rsp = 255;
      flags = IREQ_CACHE_FLUSH;

      ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));
      
      num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
      if (num_rsp < 0)
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Bluetooth_Adapter::AdapterRunloop() - num_resp error.");

      for (i = 0; i < num_rsp; i++) {
        ba2str(&(ii+i)->bdaddr, addr);
        memset(name, 0, sizeof(name));
        if (hci_read_remote_name(sock, &(ii+i)->bdaddr, sizeof(name),
				 name, 0) < 0)
	  strcpy(name, "[unknown]");
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Bluetooth_Adapter::AdapterRunloop() - Found device (%s): %s",addr,name);
      }
      
      free(ii);

      close(sock);
      
      usleep(15000000);
    }
}

/*

	COMMANDS TO IMPLEMENT

*/

//<-dceag-c1143-b->

	/** @brief COMMAND: #1143 - Bluetooth Pair Device */
	/** Pair Bluetooth Device to Media Director */
		/** @param #47 Mac address */
			/** Bluetooth MAC Address */

void Bluetooth_Adapter::CMD_Bluetooth_Pair_Device(string sMac_address,string &sCMD_Result,Message *pMessage)
//<-dceag-c1143-e->
{
	cout << "Need to implement command #1143 - Bluetooth Pair Device" << endl;
	cout << "Parm #47 - Mac_address=" << sMac_address << endl;
}

//<-dceag-c1144-b->

	/** @brief COMMAND: #1144 - Bluetooth Connect Device */
	/** Connect Bluetooth Device to Media Director kernel. */
		/** @param #47 Mac address */
			/** Bluetooth MAC Address */

void Bluetooth_Adapter::CMD_Bluetooth_Connect_Device(string sMac_address,string &sCMD_Result,Message *pMessage)
//<-dceag-c1144-e->
{
	cout << "Need to implement command #1144 - Bluetooth Connect Device" << endl;
	cout << "Parm #47 - Mac_address=" << sMac_address << endl;
}

//<-dceag-c1145-b->

	/** @brief COMMAND: #1145 - Bluetooth Disconnect Device */
	/** Disconnect Bluetooth Device from Media Director kernel */
		/** @param #47 Mac address */
			/** Bluetooth MAC Address */

void Bluetooth_Adapter::CMD_Bluetooth_Disconnect_Device(string sMac_address,string &sCMD_Result,Message *pMessage)
//<-dceag-c1145-e->
{
	cout << "Need to implement command #1145 - Bluetooth Disconnect Device" << endl;
	cout << "Parm #47 - Mac_address=" << sMac_address << endl;
}


