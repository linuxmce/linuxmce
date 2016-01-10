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

#include "hald_helper.h"
#include <iostream>

#include "PlutoUtils/LinuxSerialUSB.h"

using namespace std;
DBusError HalDHelper::halError;

enum FieldName
{
	fnVendorModelID,
	fnPK_DeviceTemplate,
	fnDescription,
	fnCommandLine,
	fnName 
};

void HalDHelper::getPortIdentification(string portFromBus, string& portID)
{
	size_t startPos = portFromBus.find("pci");
	size_t usbPos = portFromBus.find("/usb");
	size_t minus = portFromBus.rfind("-");
	size_t colon = portFromBus.rfind(":");

	if( startPos != string::npos && usbPos != string::npos && usbPos > startPos &&
		minus    != string::npos && colon  != string::npos && colon > minus )
	{
		// Razvan G. 
		// /sys/devices/pci0000:00/0000:00:02.0/usb1/1-4/1-4.5/1-4.5:1.0 => pci0000:00/0000:00:02.0+4.5
		portID = portFromBus.substr(startPos, usbPos - startPos);
		portID += "+";
		portID += portFromBus.substr(minus + 1, colon - minus - 1);
		
		portID = TranslateSerialUSB( portID );
		//LoggerWrapper::GetInstance()->Write(LV_DEBUG, "port ID = %s\n", portID.c_str());
	}
}

/***/
void HalDHelper::InternalFindDevice(LibHalContext * ctx, const std::map<unsigned long, vector<string> >& mapDevices)
{
	int num_devices = 0;
	char **devices = libhal_get_all_devices (ctx, &num_devices, NULL);
	gchar *bus = NULL;
	gchar *category = NULL;

	//get all template IDs and producer_id vendor_id
	for(int i = num_devices - 1; i >= 0 ; i--)
	{
		char *udi = devices[i];
		
		//cerr << "init udi = " << udi << endl;
		
		bus = libhal_device_get_property_string (ctx, udi, "info.bus", NULL);
		category = libhal_device_get_property_string (ctx, udi, "info.category", NULL);
		
		if( category != NULL && 0 == strcmp(category, "serial") && strlen(category) == strlen("serial") )
		{
			gchar *parent = libhal_device_get_property_string (ctx, libhal_device_get_property_string(ctx, udi, "info.parent", NULL), "info.parent", NULL);
			gchar *info_udi = libhal_device_get_property_string (ctx, parent, "info.udi", NULL);
			int usb_device_product_id = libhal_device_get_property_int(ctx, parent, "usb_device.product_id", NULL);
			int usb_device_vendor_id = libhal_device_get_property_int(ctx, parent, "usb_device.vendor_id", NULL);

			unsigned long uiSerialID = (unsigned long) ((usb_device_vendor_id & 0xffff) << 16) | (usb_device_product_id & 0xffff);
			//cerr << "searching for serial in map: " << uiSerialID << endl;

			map<unsigned long, vector<string> >::const_iterator it = mapDevices.find(uiSerialID);
			if( it != mapDevices.end() )
			{
				gchar *serialDevice = libhal_device_get_property_string (ctx, udi, "serial.device", NULL);
				gchar *serial_port = libhal_device_get_property_string (ctx, libhal_device_get_property_string(ctx, udi, "info.parent", NULL), "linux.sysfs_path", NULL);
				if(serial_port != NULL && serialDevice != NULL)
				{
//					string portID;
//					getPortIdentification(string(serial_port), portID);
					cout << serialDevice << "|" 
						<< it->second[fnPK_DeviceTemplate] << "|" 
						<< it->second[fnDescription] << "|" 
						<< it->second[fnName] << endl; 
				}
				
				g_free (serialDevice);
				serialDevice = NULL;
				g_free (serial_port);
				serial_port = NULL;
			}
			
			g_free (parent);
			parent = NULL;
			g_free (info_udi);
			info_udi = NULL;
		}
		
		g_free(bus);
		bus = NULL;
		g_free(category);
		category = NULL;
	}
}

void* HalDHelper::FindDevice(const map<unsigned long, vector<string> >& mapDevices)
{
	dbus_error_init(&halError);
	DBusConnection * halConnection = dbus_bus_get(DBUS_BUS_SYSTEM, &halError);
	if( halConnection == NULL )
	{
		cerr << "DBusConnection is NULL!" << endl;
		return NULL;
	}
	
	LibHalContext * ctx = libhal_ctx_new();
	if ( ctx == NULL )
	{
		cerr << "CTX is NULL!" << endl;
		return NULL;
	}
	
	libhal_ctx_set_dbus_connection(ctx, halConnection);
	
	if( !libhal_ctx_init(ctx, &halError) )
	{
		cerr << "CTX initialization failed!" << endl; 
		dbus_error_free(&halError);
		libhal_ctx_free(ctx);
		return NULL;
	}
	
	InternalFindDevice(ctx, mapDevices);
	
	libhal_device_property_watch_all(ctx, &halError);

	if( dbus_error_is_set (&halError) )
		dbus_error_free (&halError);
	
	dbus_error_init(&halError);
	
	libhal_ctx_shutdown(ctx, &halError);
	libhal_ctx_free(ctx);
	
	return NULL;
}
