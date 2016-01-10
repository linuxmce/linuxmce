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
#include <dbus/dbus.h>
#include <stdio.h>
#include <hal/libhal.h>
#include <stdlib.h>
#include <string.h>
#include <glib-2.0/glib.h>
#include <dbus/dbus-glib-lowlevel.h>
#include "DCE/Logger.h"
#include "DCE/Message.h"
#include "PlutoUtils/StringUtils.h"
#include "DCE/DeviceData_Impl.h"

/*
 
 CreateDevice createDevice(m_pRouter->iPK_Installation_get(),m_pRouter->sDBHost_get(),m_pRouter->sDBUser_get(),m_pRouter->sDBPassword_get(),m_pRouter->sDBName_get(),m_pRouter->iDBPort_get()); 
 createDevice.m_bInstallPackagesInBackground = true; 
 *iPK_Device = createDevice.DoIt(iPK_DHCPDevice,iPK_DeviceTemplate,sIP_Address,sMac_address,iPK_Device_ControlledVia,sData,iPK_Device_Related,iPK_Room > 0 ? iPK_Room : 0);

 */
// -DDBUS_API_SUBJECT_TO_CHANGE -I/usr/include/hal -I/usr/include/dbus-1.0 -I/usr/lib/dbus-1.0/include

#define ZWAVE_PRODUCT_ID 0xf2d0
#define ZWAVE_VENDOR_ID 0x0403

#define PLUTO_ZWAVE_TEMPLATE_ID 1754
#define PLUTO_CORE_DEVICE_ID 1

#define PLUTO_ZWAVE_TEMPLATE_ID_STR "1754"
#define PLUTO_CORE_DEVICE_ID_STR "1"

char *hostname = NULL;

using namespace DCE;

;


void mainloop_integration (LibHalContext *ctx, DBusConnection * dbus_connection)
{
	dbus_connection_setup_with_g_main (dbus_connection, NULL);
}

void sendMessage(char *params[], int count, string &returnValue)
{
	Event_Impl *pEvent = new Event_Impl(DEVICEID_MESSAGESEND, 0, hostname);
	if(pEvent != NULL)
	{			
		for(int i=0;true;++i) // Wait up to 30 seconds
		{
			pEvent->m_pClientSocket->SendString("READY");
			string sResponse;
			if( !pEvent->m_pClientSocket->ReceiveString(sResponse,5) )
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot communicate with router");
				throw(new string("Cannot communicate with router"));
			}
			if( sResponse=="YES" )
				break;
			else if( sResponse=="NO" )
			{
				if( i>5 )
				{
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Router not ready after 30 seconds.  Aborting....");
					throw(new string("Router not ready after 30 seconds.  Aborting...."));
				}
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"DCERouter still loading.  Waiting 5 seconds");
				Sleep(5000);
			}
			else
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Router gave unknown response to ready request %s",sResponse.c_str());
				throw(new string("Router gave unknown response to ready request"));
			}
		}

		Message *pMsg=new Message(13, params);
		if(pMsg != NULL)
		{
			eExpectedResponse ExpectedResponse = pMsg->m_eExpectedResponse;
			if( ExpectedResponse == ER_ReplyMessage )
			{
				// There are out parameters, we need to get a message back in return
				Message *pResponse = pEvent->SendReceiveMessage( pMsg );
				if( !pResponse || pResponse->m_dwID != 0 )
				{
					if(pResponse)
					{
						delete pResponse;
						pResponse = NULL;
					}								
					LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Failed to send message" );
				}
				else
				{
					for( map<long, string>::iterator it=pResponse->m_mapParameters.begin();it!=pResponse->m_mapParameters.end();++it)
					{
						cout << (*it).first << ":" << (*it).second << endl;
						returnValue = (*it).second;
					}
				}
				delete pResponse;
				pResponse = NULL;
			}
			else
			{
				LoggerWrapper::GetInstance()->Write(LV_DEBUG, "message should have out parameters (PK_Device (int))");
			}
		}
		else
		{
			throw(new string("pMsg == NULL"));
		}
	}
	else
		throw(new string("pEvent == NULL"));
}


void myDeviceAdded(LibHalContext * ctx, const char * udi)
{

	static char last_udi[2048];

	gchar *bus = hal_device_get_property_string (ctx, udi, "info.bus");
	if(strcmp(bus, "usb_device") == 0 && strlen(bus) == strlen("usb_device"))
	{
		strcpy(last_udi, udi); 
		//hal_device_print (ctx, udi);
		int usb_device_product_id = hal_device_get_property_int(ctx, udi, "usb_device.product_id");
		int usb_device_vendor_id = hal_device_get_property_int(ctx, udi, "usb_device.vendor_id");
		
		if(usb_device_product_id == ZWAVE_PRODUCT_ID && usb_device_vendor_id == ZWAVE_VENDOR_ID)
		{
			gchar *product = hal_device_get_property_string (ctx, udi, "info.product");
			gchar *vendor = hal_device_get_property_string (ctx, udi, "info.vendor");
			gchar *serial = hal_device_get_property_string (ctx, udi, "usb_device.serial");
			gchar *info_udi = hal_device_get_property_string (ctx, udi, "info.udi");
			gchar *sysfs_path = hal_device_get_property_string (ctx, udi, "usb_device.linux.sysfs_path");
			
			// TODO: check for null strings
			
			char buffer[4096];
			
			
			printf("%s | %s | %s | %s | %s | %s\n", bus, product, vendor, serial, info_udi, sysfs_path);
			if(hostname == NULL)
			{
				hostname = (char *)malloc(10);
				if(hostname != NULL)
					strcpy(hostname, "localhost");
			}
			/*./MessageSend 10.0.0.87 -targetType template -o 0 27 1 718 44 1754 156 1 57 -1
				0:OK
				2:30
			 * */
			snprintf( buffer, sizeof(buffer),
				"MessageSend %s -targetType template -o 0 27 1 718 44 %d 156 %d 57 -1 109 154|%s|37|%s",
				hostname,
				PLUTO_ZWAVE_TEMPLATE_ID, 
				PLUTO_CORE_DEVICE_ID,
				info_udi,
				sysfs_path );
			printf("%s\n", buffer);
//				system(buffer);
			try
			{
				
				bool isNewDevice = true;
				//find out if it is a new device by calling the command 
				//corresponding to the void General_Info_Plugin::Get_iPK_DeviceFromUID(string UID, string deviceType, string &returnValue)
				//function from General_Info_Plugin
				if(isNewDevice)
				{
					char *params[15];
					snprintf(buffer, sizeof(buffer), "154|%s", info_udi);

					params[0]  =	"-targetType";
					params[1]  =	"template";
					params[2]  =	"-o";
					params[3]  =	"0"; // from:
					params[4]  =	"27";// to: DEVICETEMPLATE_General_Info_Plugin_CONST
					params[5]  =	"1"; //command
					params[6]  =	"718";
					params[7]  =	"44";
					params[8]  =	PLUTO_ZWAVE_TEMPLATE_ID_STR;
					params[9]  =	"156";
					params[10] =	PLUTO_CORE_DEVICE_ID_STR;
					params[11] =	"57";
					params[12] =	"-1";
					params[13] =	"109"; // extra param for setting the data devices
					params[14] =	buffer;
					string response;
					
//					sendMessage(params, 15, response);
				}
			}
			catch(string ex)
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "exception thrown: %s", ex.c_str());
			}

			g_free (product);
			product = NULL;
			g_free (vendor);
			vendor = NULL;
			g_free (serial);
			serial = NULL;
			g_free (info_udi);
			info_udi = NULL;
			g_free (sysfs_path);
			sysfs_path = NULL;
		}
	}

		
//	if(/*strncmp(last_udi, udi, strlen(last_udi)) == 0 && */strncmp(&udi[strlen(udi) - 10], "usb-serial", 10) == 0)
//	{
//	}
	g_free (bus);
}


void myDeviceNewCapability(LibHalContext * ctx, const char * udi, const char *capability)
{
	gchar *serial_port = hal_device_get_property_string (ctx, udi, "serial.device");
	if(serial_port != NULL)
	{
		fflush(stdout);
		gchar *parent = hal_device_get_property_string (ctx, hal_device_get_property_string(ctx, udi, "info.parent"), "info.parent");
		printf("udi = %s parent = %s capability = %s serial port = %s\n", udi, parent, capability, serial_port);
		g_free (parent);
		parent = NULL;

	}
	g_free (serial_port);
	serial_port = NULL;
}



void myDeviceRemoved(LibHalContext * ctx, const char * udi)
{
	printf("removed device %s\n", udi);
}



void initialize(LibHalContext * ctx)
{
	int num_devices = 0;
	int i = 0;
	char **devices = hal_get_all_devices (ctx, &num_devices);
	gchar *bus = NULL;
	
	for(i = num_devices - 1; i >= 0 ; i--)
	{
		char *udi = devices[i];
		bus = hal_device_get_property_string (ctx, udi, "info.bus");
		if(strcmp(bus, "usb_device") == 0)
		{
			int usb_device_product_id = hal_device_get_property_int(ctx, udi, "usb_device.product_id");
			int usb_device_vendor_id = hal_device_get_property_int(ctx, udi, "usb_device.vendor_id");;
						
			if( (usb_device_product_id == ZWAVE_PRODUCT_ID && usb_device_vendor_id == ZWAVE_VENDOR_ID) ||
			    (usb_device_vendor_id == 0x045e && usb_device_product_id == 0x006d))
			{
					gchar *product = hal_device_get_property_string (ctx, udi, "info.product");
					gchar *vendor = hal_device_get_property_string (ctx, udi, "info.vendor");
					gchar *serial = hal_device_get_property_string (ctx, udi, "usb_device.serial");
					gchar *info_udi = hal_device_get_property_string (ctx, udi, "info.udi");
					gchar *sysfs_path = hal_device_get_property_string (ctx, udi, "usb_device.linux.sysfs_path");
					
					printf("%s | %s | %s | %s | %s | %s\n", bus, product, vendor, serial, info_udi, sysfs_path);
					//TODO complete this one
					system("CommandSend ");
					
					g_free (product);
					product = NULL;
					g_free (vendor);
					vendor = NULL;
					g_free (serial);
					serial = NULL;
					g_free (info_udi);
					info_udi = NULL;
					g_free(sysfs_path);
					sysfs_path = NULL;
			}
		}
		gchar *serial_port = hal_device_get_property_string (ctx, udi, "serial.device");
		if(serial_port != NULL)
		{
			printf("udi = %s serial port = %s\n", udi, serial_port);
			fflush(stdout);
		}
		g_free (serial_port);
		serial_port = NULL;

		g_free(bus);
		bus = NULL;
	}
}

int main(int argc, char* argv[])
{
	LibHalFunctions funcs;
	LibHalContext * ctx;
	GMainLoop *loop = NULL;
	
	LoggerWrapper::SetType(LT_LOGGER_FILE,"hal.log");
	
	if(argc >= 2)
	{
		hostname = (char *)malloc(strlen(argv[1]) + 1);
		if(hostname != NULL)
		{
			strcpy(hostname, argv[1]);
		}
	}
	else
		hostname = NULL;
	
	loop = g_main_loop_new (NULL, FALSE);
	
	funcs.main_loop_integration = mainloop_integration;
	funcs.device_added = myDeviceAdded;
	funcs.device_removed = myDeviceRemoved;
	funcs.device_new_capability = myDeviceNewCapability;
	funcs.device_lost_capability = NULL;
	funcs.device_property_modified = NULL;
	funcs.device_condition = NULL;
	
	ctx = hal_initialize(&funcs, 0);
	if (!ctx)
	{
		printf("NULL!\n");
		return 1;
	}
	initialize(ctx);
	
	hal_device_property_watch_all(ctx);

	g_main_loop_run(loop);
	
	hal_shutdown(ctx);
	return 0;
}
