/*

 Copyright (C) 2015 Jason Murduck.

 This file is licenced under GPL even if this whole program is not.
 Whatever needs to be said to make that happen should go here.

 */

#include "udev_helper.h"

#include "PlutoUtils/LinuxSerialUSB.h"

using namespace std;

// Required enum to determine data from the provided map.
enum FieldName
{
	fnVendorModelID,
	fnPK_DeviceTemplate,
	fnDescription,
	fnCommandLine,
	fnName
};

// Borrowed from existing lmce source files.
void UdevHelper::getPortIdentification(string portFromBus, string& portID)
{
	size_t startPos = portFromBus.find("pci");

	if ( startPos == string::npos)
		startPos = portFromBus.find("platform");

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
	}
}


void UdevHelper::InternalFindDevice(struct udev * ctx, const std::map<unsigned long, vector<string> >& mapDevices)
{
	struct udev_list_entry *devices, *dev_list_entry;
	struct udev_enumerate *enumerate;

	enumerate = udev_enumerate_new(ctx);
	udev_enumerate_add_match_subsystem(enumerate, "tty");
	udev_enumerate_scan_devices(enumerate);

	devices = udev_enumerate_get_list_entry(enumerate);

	//get all template IDs and producer_id vendor_id
	udev_list_entry_foreach(dev_list_entry, devices)
	{
		const char *syspath = udev_list_entry_get_name(dev_list_entry);
		struct udev_device *dev = udev_device_new_from_syspath(ctx, syspath);
		struct udev_device *parent_dev = udev_device_get_parent_with_subsystem_devtype(dev, "usb", "usb_device");

		if ( parent_dev != NULL )
		{
			const char *prod_Id = (udev_device_get_sysattr_value(parent_dev,"idProduct") != NULL ? udev_device_get_sysattr_value(parent_dev,"idProduct") : udev_device_get_sysattr_value(parent_dev,"device"));
			const char *vendor_Id = (udev_device_get_sysattr_value(parent_dev,"idVendor") != NULL ? udev_device_get_sysattr_value(parent_dev,"idVendor") : udev_device_get_sysattr_value(parent_dev,"vendor"));

			//cerr << "searching for serial device: " << vendor_Id << ":" << prod_Id << " - " << syspath << endl;

			int usb_device_product_id = ( prod_Id == NULL ? -1 : strtol(prod_Id, NULL, 16) );
			int usb_device_vendor_id = ( vendor_Id == NULL ? -1 : strtol(vendor_Id, NULL, 16) );

			unsigned long uiSerialID = (unsigned long) ((usb_device_vendor_id & 0xffff) << 16) | (usb_device_product_id & 0xffff);
			//cerr << "searching for serial in map: " << uiSerialID << endl;

			map<unsigned long, vector<string> >::const_iterator it = mapDevices.find(uiSerialID);
			if( it != mapDevices.end() )
			{
				const char *serialDevice = udev_device_get_devnode(dev);

//				const char *serial_port = udev_device_get_syspath(dev);
//				string portID;
//				getPortIdentification(string(serial_port), portID);

				cout << serialDevice << "|"
				     << it->second[fnPK_DeviceTemplate] << "|"
				     << it->second[fnDescription] << "|"
				     << it->second[fnName] << endl;
			}
		}
	}
}

void* UdevHelper::FindDevice(const map<unsigned long, vector<string> >& mapDevices)
{
	struct udev * ctx = udev_new();
        if ( ctx == NULL )
        {
                cerr << "udev context is NULL!" << endl;
                return NULL;
        }

	InternalFindDevice(ctx, mapDevices);

	return NULL;
}
