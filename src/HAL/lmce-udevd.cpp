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


#include "udi-helper.h"
#include <libudev.h>
#include "lmce-udevd.h"

#include "HAL.h"

#include "DCE/Logger.h"
#include "DCE/Message.h"
#include "DCE/DeviceData_Impl.h"
#include "DCE/DCEConfig.h"

#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/ProcessUtils.h"

#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"
#include "Gen_Devices/AllCommandsRequests.h"

#define UNKNOWN_COMM_METHOD  11
#define USB_COMM_METHOD       4
#define PCI_COMM_METHOD       8

using namespace DCE;

using namespace std;

std::map<unsigned int, int> LmceUdevD::templatesMap;
HAL *LmceUdevD::HalDevice = NULL;
long LmceUdevD::pnpDeviceID = 0;
//GMainLoop * LmceUdevD::loop = NULL;
bool LmceUdevD::threadShutdown = false;
bool LmceUdevD::udevdrunning = false;
std::string LmceUdevD::moreInfo;

std::map<std::string, std::string> LmceUdevD::devicesMap;

LmceUdevD::LmceUdevD()
{
}

/*
// not going to use glib integration, need our own monitoring thread perhaps
void PlutoHalD::mainloop_integration (LibHalContext *ctx, DBusConnection * dbus_connection)
{
	dbus_connection_setup_with_g_main (dbus_connection, NULL);
}
*/

void LmceUdevD::getPortIdentification(string portFromBus, string& portID)
{
	size_t startPos = portFromBus.find("pci");

	if ( startPos == string::npos )
	{
		startPos = portFromBus.find("platform");
	}

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
		LoggerWrapper::GetInstance()->Write(LV_DEBUG, "))))))) port ID = %s", portID.c_str());
	}
}

void LmceUdevD::getParentId(struct udev * ctx, struct udev_device * dev,  // const char * udi,
							const char * sub, const char * type,
							const char * tag, const char * value,
							std::string & parentId, std::string & parentTagValue)
{
	// NOTES: this is asking to get device's parent's udi
	struct udev_device *parent_dev = NULL; // = udev_device_get_parent(dev);
	if ( sub != NULL )
		parent_dev = udev_device_get_parent_with_subsystem_devtype(dev, sub, type);
	else
		parent_dev = udev_device_get_parent(dev);

	if( parent_dev != NULL )
	{
		const char *parent = udi_helper_compute_udi(parent_dev);

		if( parent != NULL )
		{
			// NOTES: this is asking to get a parent with specific parameters or sysattr values that match
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "getParentId :: id found");
			string tagValue;
			if ( value != NULL )
				tagValue = value;

			// returns NULL if not found
			const char *udevTag = ( udev_device_has_property(parent_dev, tag) ? udev_device_get_property_value (parent_dev, tag) : udev_device_get_sysattr_value (parent_dev, tag) );

			LoggerWrapper::GetInstance()->Write(LV_DEBUG, "------- getParentId = %s || %s", udevTag, tagValue.c_str());

			if( udevTag != NULL && ( (value == NULL && udevTag != NULL ) || tagValue == udevTag) )
			{
				if ( parent != NULL )
					parentId = parent;
				if ( udevTag != NULL )
					parentTagValue = udevTag;
			}
			else
			{
				getParentId(ctx, parent_dev, sub, type, tag, value, parentId, parentTagValue);
			}
		}
	}
}

void LmceUdevD::getChildId(struct udev * ctx, struct udev_device * dev,
							const char * sub, const char * tag, const char * value,
							std::string & childId)
{
	struct udev_enumerate *enumerate;
	struct udev_list_entry *devices, *dev_list_entry;

	const char *syspath = udev_device_get_syspath(dev);
	const char *subsystem = udev_device_get_subsystem(dev);
	const char *devtype = udev_device_get_devtype(dev);

	enumerate = udev_enumerate_new(ctx);

	if ( sub != NULL )
	{
		udev_enumerate_add_match_subsystem(enumerate, sub);
	}

	udev_enumerate_scan_devices(enumerate);
	devices = udev_enumerate_get_list_entry(enumerate);

	udev_list_entry_foreach(dev_list_entry, devices) {
		const char *path = udev_list_entry_get_name(dev_list_entry);
		struct udev_device *d = udev_device_new_from_syspath(ctx, path);

		struct udev_device *parent_d = NULL; // = udev_device_get_parent(d);
		if ( sub != NULL )
		{
			parent_d = udev_device_get_parent_with_subsystem_devtype(d, subsystem, devtype);
		}
		else
		{
			parent_d = udev_device_get_parent(d);
		}

		if (parent_d != NULL)
		{
			const char *parent_syspath = udev_device_get_syspath(parent_d);

			if (0 == strcmp(syspath, parent_syspath))
			{
				// matched on subsystem with no tag to find
				if( tag == NULL )
				{
					childId = "found";
					LoggerWrapper::GetInstance()->Write(LV_DEBUG, "------- getChildId found %s || %s", syspath, sub);
					udev_device_unref(d);
					udev_enumerate_unref(enumerate);
					return;
				}

				string tagValue;
				if ( value != NULL)
					tagValue = value;
				const char *udevTag = ( udev_device_has_property(d, tag) ? udev_device_get_property_value (d, tag) : udev_device_get_sysattr_value (d, tag) );

				if ( udevTag != NULL && tagValue == udevTag)
				{
					const char *child = udi_helper_compute_udi(d);
					if (child == NULL)
						child = udi_helper_compute_udi(parent_d);
					LoggerWrapper::GetInstance()->Write(LV_DEBUG, "------- getChildId = %s || %s", child, udevTag);


					if (child != NULL)
						childId = child;
					udev_device_unref(d);
					udev_enumerate_unref(enumerate);
					return;
				}
				else
				{
					getChildId(ctx, d, sub, tag, value, childId);
				}
			}
		}
		udev_device_unref(d);
	}
	udev_enumerate_unref(enumerate);
}

// linux.sysfs_path = '/sys/class/tty/ttyUSB2' => linux.sysfs_path = '/sys/devices/pci0000:00/0000:00:0b.0/usb1/1-2'
// readlink /sys/class/tty/ttyUSB2/device
// ../../../devices/pci0000:00/0000:00:0b.0/usb1/1-2/1-2:1.0/ttyUSB2
void LmceUdevD::getSerialParent(const char * sysfs, std::string & parentSysfs)
{
	parentSysfs = "";
	string parentPath;

	/* In 0810 the sysfs path works as the symlink value did in 0710
	 * Using the symlink here actually breaks things, and some devices don't even have this symlink
	 * The sysfs path itself is what we used to get in 0710 from the symlink
	 */
	parentPath = sysfs;
	size_t iFind1 = parentPath.find("pci");

	if ( iFind1 == string::npos )
	{
		iFind1 = parentPath.find("platform");
	}

	size_t iFind2 = parentPath.rfind(":");

	LoggerWrapper::GetInstance()->Write(LV_DEBUG, "+++++++ getSerialParent %s: parent=%s, iFind1=%d, iFind2=%d", sysfs, parentPath.c_str(), iFind1, iFind2);

	if( iFind1 != string::npos && iFind2 != string::npos && iFind1 < iFind2 )
	{
		parentSysfs = "/sys/devices/" + parentPath.substr(iFind1, iFind2 - iFind1);
		size_t iFind3 = parentSysfs.rfind("/");
		if( iFind3 != string::npos )
		{
			parentSysfs = parentSysfs.substr(0, iFind3);
		}
		LoggerWrapper::GetInstance()->Write(LV_DEBUG, "XXXXX getSerialPort: parentSysfs=%s", parentSysfs.c_str());
	}
	else
	{
		LoggerWrapper::GetInstance()->Write
			(LV_DEBUG, "+++++++ getSerialParent error = %s\nPath = %s", sysfs, parentPath.c_str());
	}
}

void LmceUdevD::myDeviceAdded(struct udev *ctx, struct udev_device *dev)
{
	if( ctx == NULL || dev == NULL )
	{
		// error
		return;
	}

	// the udi is in the map after this call to compute.
	const char *udi = udi_helper_compute_udi(dev);

	const char *syspath = udev_device_get_syspath(dev);
	const char *subsystem = udev_device_get_subsystem(dev);
	const char *devtype = udev_device_get_devtype(dev);

	struct udev_device *parent_dev = udev_device_get_parent(dev);
	const char *parent_subsystem = NULL;
	const char *parent_devtype = NULL;
	if (parent_dev != NULL)
	{
		parent_subsystem = udev_device_get_subsystem(parent_dev);
		parent_devtype = udev_device_get_devtype(parent_dev);
	}

	LoggerWrapper::GetInstance()->Write(LV_DEBUG, "init udi = %s", udi);
	LoggerWrapper::GetInstance()->Write(LV_DEBUG, "XXXXX udi: %s", udi);
	LoggerWrapper::GetInstance()->Write(LV_DEBUG, "XXXXX subsystem: %s", subsystem);
	LoggerWrapper::GetInstance()->Write(LV_DEBUG, "XXXXX devtype: %s", devtype);
	LoggerWrapper::GetInstance()->Write(LV_DEBUG, "XXXXX parent subsystem: %s, devtype: %s", parent_subsystem, parent_devtype);

        int device_vendor_id = 0;
        int device_product_id = 0;
        int subsys_device_vendor_id = 0;
        int subsys_device_product_id = 0;
	int iBusType;
	string sysfsPath;

	getProductVendorId(
		ctx, dev,
		&device_product_id, &device_vendor_id,
		&subsys_device_product_id, &subsys_device_vendor_id,
		&iBusType, sysfsPath );

	string deviceData;
	char buffer[64];
	const char *parent_udi = NULL;

	if ( iBusType == PCI_COMM_METHOD || iBusType == UNKNOWN_COMM_METHOD )
	{
		if( !sysfsPath.empty() )
		{
			LoggerWrapper::GetInstance()->Write(LV_DEBUG, "=== PCI %s", sysfsPath.c_str() );
			deviceData += StringUtils::itos( DEVICEDATA_Location_on_PCI_bus_CONST );
			deviceData += "|";
			deviceData += sysfsPath;
		}

		snprintf(buffer, sizeof(buffer), "%08x%08x",
				(unsigned int) ((device_vendor_id & 0xffff) << 16) | (device_product_id & 0xffff),
				(unsigned int) ((subsys_device_vendor_id & 0xffff) << 16) | (subsys_device_product_id & 0xffff));
	}
	else if ( iBusType == USB_COMM_METHOD )
	{
		snprintf(buffer, sizeof(buffer), "%08x", (unsigned int) ((device_vendor_id & 0xffff) << 16) | (device_product_id & 0xffff));
	}
	else
	{
		snprintf(buffer, sizeof(buffer), "%08x%08x",
				(unsigned int) ((device_vendor_id & 0xffff) << 16) | (device_product_id & 0xffff),
				(unsigned int) ((subsys_device_vendor_id & 0xffff) << 16) | (subsys_device_product_id & 0xffff));
	}

	parent_udi = udi_helper_compute_parent_udi(dev);
	LoggerWrapper::GetInstance()->Write(LV_DEBUG, "=== parent_udi %s", parent_udi );

	string child;
	if( 0 == strcmp(subsystem, "usb") && devtype != NULL && 0 == strcmp(devtype, "usb_device") && strlen(devtype) == strlen("usb_device")
			&& parent_devtype != NULL && 0 == strcmp(parent_devtype, "usb_device") && strlen(parent_devtype) == strlen("usb_device") )
	{
		// We ignore devices with child subsystems usb-serial, block and bluetooth so they can be handled seperately with 'category' defined.
		// We do need to add devices whose parent usb_device is not the same, to distinguish child device in a hub vs a hub
		// Or if we have a USB device that has a usb_device as a child, ignore it as it's a hub
		string usbChild;
		getChildId(ctx, dev, "usb", NULL, NULL, usbChild);

		if (child.empty())
		{
			getChildId(ctx, dev, "scsi_host", NULL, NULL, child);
		}
		if (child.empty())
		{
			getChildId(ctx, dev, "sound", NULL, NULL, child);
		}
//		if (child.empty())
//		{
//			getChildId(ctx, dev, "usb-serial", NULL, NULL, child);
//		}
		if (child.empty())
		{
			getChildId(ctx, dev, "video4linux", NULL, NULL, child);
		}
		if (child.empty())
		{
			getChildId(ctx, dev, "bluetooth", NULL, NULL, child);
		}
		if (child.empty())
		{
			getChildId(ctx, dev, "net", NULL, NULL, child);
		}
		if (child.empty())
		{
			getChildId(ctx, dev, "input", "ID_INPUT_JOYSTICK", "1", child);
		}
//		if (child.empty())
//		{
//			getChildId(ctx, dev, "tty", NULL, NULL, child);
//		}
		if (child.empty())
		{
			getChildId(ctx, dev, "dvb", NULL, NULL, child);
		}

		if (usbChild.empty() || (!usbChild.empty() && child.empty()))
		{
			// This is a usb device that we will not recoginize by any other subsystem
			LoggerWrapper::GetInstance()->Write(LV_DEBUG, "+++++++ USB device added %s = %s", buffer, udi);

			devicesMap[ (std::string) syspath ] = (std::string) udi;
			HalDevice->EVENT_Device_Detected("", "", "", 0, buffer, iBusType /*USB_COMM_METHOD*/, 0, udi, "", "" /*category*/, HalDevice->m_sSignature_get());

			LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Finished firing event for %s", buffer);
		}
		child.clear();
	}

	else if( 0 == strcmp(subsystem, "usb-serial") && strlen(subsystem) == strlen("usb-serial") ) // category == serial
	{
		LoggerWrapper::GetInstance()->Write(LV_DEBUG, "+++++++ SRL device added %s = %s", buffer, parent_udi);

		const char *serial_port = udev_device_get_syspath(dev);
		string portID;
		getPortIdentification(string(serial_port), portID);

		if( !deviceData.empty() )
		{
			deviceData += "|";
		}

		deviceData += "37"; // TODO: Constant? StringUtils::itos( DEVICEDATA_COM_Port_on_PC_CONST );
		deviceData += "|";
		deviceData += portID;

		devicesMap[ (std::string) syspath ] = (std::string) parent_udi;
		HalDevice->EVENT_Device_Detected("", "", "", 0, buffer, iBusType /*USB_COMM_METHOD*/, 0, parent_udi, deviceData.c_str() /*"37|" + portID*/, "serial" /*category*/, HalDevice->m_sSignature_get());

		LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Finished firing event for %s", buffer);
	}

	else if( 0 == strcmp(subsystem, "bluetooth") && strlen(subsystem) == strlen("bluetooth")  // category == bluetooth_hci
			&& 0 == strcmp(devtype, "host") && strlen(devtype) == strlen("host") )
	{
		LoggerWrapper::GetInstance()->Write(LV_DEBUG, "+++++++ BTH device added %s = %s", buffer, parent_udi);

		devicesMap[ (std::string) syspath ] = (std::string) parent_udi;
		HalDevice->EVENT_Device_Detected("", "", "", 0, buffer, 4, 0, parent_udi, "", "bluetooth_hci" /*category*/, HalDevice->m_sSignature_get());

		LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Finished firing event for %s", buffer);
	}

	else if( udi != NULL && 0 == strcmp(subsystem, "input") && strlen(subsystem) == strlen("input") )
	{
		const char *joystick = udev_device_get_property_value(dev, "ID_INPUT_JOYSTICK");
		if ( joystick != NULL ) // category == joystick
		{
			LoggerWrapper::GetInstance()->Write(LV_DEBUG, "+++++++ JOY device added %s = %s", buffer, parent_udi);

			devicesMap[ (std::string) syspath ] = (std::string) parent_udi;
			HalDevice->EVENT_Device_Detected("", "", "", 0, buffer, 4, 0, parent_udi, "", "joystick" /*category*/, HalDevice->m_sSignature_get());

			LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Finished firing event for %s", buffer);
		}
	}

	else if( 0 == strcmp(subsystem, "block") && strlen(subsystem) == strlen("block") )
	{
		if( udi != NULL && NULL != strstr(udi, "volume") && iBusType == USB_COMM_METHOD) // category == volume
		{
			// TODO is_disc ??

			// udisks: need to use udisks for volume management, versus udev for device management
			const char *fstype = udev_device_get_property_value(dev, "volume.fstype");
			if( fstype != NULL )
			{
				if( !deviceData.empty() )
				{
					deviceData += "|";
				}

				deviceData += StringUtils::itos( DEVICEDATA_Filesystem_CONST );
				deviceData += "|";
				deviceData += fstype;
			}

			const char *blockdevice = udev_device_get_sysattr_value(dev, "dev");
			if( blockdevice != NULL )
			{
				if( !deviceData.empty() )
				{
					deviceData += "|";
				}

				deviceData += StringUtils::itos( DEVICEDATA_Block_Device_CONST );
				deviceData += "|";
				deviceData += blockdevice;
			}

			LoggerWrapper::GetInstance()->Write(LV_DEBUG, "+++++++ USB Volume added %s = %s", buffer, udi);

			devicesMap[ (std::string) syspath ] = (std::string) udi;
			HalDevice->EVENT_Device_Detected("", "", "", 0, buffer, iBusType, 0, udi, deviceData.c_str(), "volume" /*category*/, HalDevice->m_sSignature_get());

			LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Finished firing event for %s", buffer);
		}

		else if ( udi != NULL && NULL != strstr(udi, "storage") ) // category == storage
		{
			// this section seems to deal only with detecting cdrom storage devices (disk_drives)
			const char *cdrom = udev_device_get_property_value(dev, "ID_CDROM");

			if( cdrom != NULL ) // category == cdrom
			{
				LoggerWrapper::GetInstance()->Write(LV_DEBUG, "+++++++ CDR device added %s = %s", buffer, udi);

				const char *blockdevice =  udev_device_get_property_value(dev, "DEVNAME");
				if( blockdevice != NULL )
				{
					if( !deviceData.empty() )
					{
						deviceData += "|";
					}

					deviceData += StringUtils::itos( DEVICEDATA_Drive_CONST );
					deviceData += "|";
					deviceData += blockdevice;
				}

				devicesMap[ (std::string) syspath ] = (std::string) udi;
				HalDevice->EVENT_Device_Detected("", "", "", 0, buffer, iBusType, 0, udi, deviceData.c_str(), "cdrom" /*category*/, HalDevice->m_sSignature_get());

				LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Finished firing event for %s", buffer);
			}
		}
	}

	else if( 0 == strcmp(subsystem, "video4linux") && strlen(subsystem) == strlen("video4linux") ) // category == video4linux
	{
		if ( udi[strlen(udi)-2] != '_' )
		{
			LoggerWrapper::GetInstance()->Write(LV_DEBUG, ")))))))* V4L device added = %s\nSP=%s\nid: %d||%d\n",
				udi, parent_udi,
				device_product_id, device_vendor_id);

			LoggerWrapper::GetInstance()->Write(LV_DEBUG, "+++++++ V4L device added %s = %s", buffer, udi);

			const char *blockdevice =  udev_device_get_devnode(dev);
			if( blockdevice != NULL )
			{
				if( !deviceData.empty() )
				{
					deviceData += "|";
				}

				deviceData += StringUtils::itos( DEVICEDATA_Block_Device_CONST );
				deviceData += "|";
				deviceData += blockdevice;
			}

			devicesMap[ (std::string) syspath ] = (std::string) udi;
			HalDevice->EVENT_Device_Detected("", "", "", 0, buffer, iBusType, 0, udi, deviceData.c_str(), "video4linux" /*category*/, HalDevice->m_sSignature_get());

			LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Finished firing event for %s", buffer);
		}
	}

	else if( 0 == strcmp(subsystem, "net") && strlen(subsystem) == strlen("net") ) // category == video4linux
	{
		LoggerWrapper::GetInstance()->Write(LV_DEBUG, "+++++++ NET device added %s = %s", buffer, udi);

		devicesMap[ (std::string) syspath ] = (std::string) udi;
		HalDevice->EVENT_Device_Detected("", "", "", 0, buffer, iBusType, 0, udi, deviceData.c_str(), "net" /*category*/, HalDevice->m_sSignature_get());

		LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Finished firing event for %s", buffer);
	}

	else if( 0 == strcmp(subsystem, "sound") && strlen(subsystem) == strlen("sound") ) // category == sound
	{
		if  ( NULL == parent_subsystem || ( NULL != parent_subsystem && 0 != strcmp(parent_subsystem, "sound") )) // don't add card sub devices
		{
			LoggerWrapper::GetInstance()->Write(LV_DEBUG, "+++++++ SND device added %s = %s", buffer, udi);

			devicesMap[ (std::string) syspath ] = (std::string) udi;
			HalDevice->EVENT_Device_Detected("", "", "", 0, buffer, iBusType, 0, udi, deviceData.c_str(), "sound" /*category*/, HalDevice->m_sSignature_get());

			LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Finished firing event for %s", buffer);
		}
	}

	else if( 0 == strcmp(subsystem, "scsi_host") && strlen(subsystem) == strlen("scsi_host") ) // category == sound
	{
		LoggerWrapper::GetInstance()->Write(LV_DEBUG, "+++++++ SCH device added %s = %s", buffer, udi);

		devicesMap[ (std::string) syspath ] = (std::string) udi;
		HalDevice->EVENT_Device_Detected("", "", "", 0, buffer, iBusType, 0, udi, deviceData.c_str(), "scsi_host" /*"category*/, HalDevice->m_sSignature_get());

		LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Finished firing event for %s", buffer);
	}

	else if( strcmp(subsystem, "pci") == 0 && strlen(subsystem) == strlen("pci") )
	{
		if (child.empty())
		{
			getChildId(ctx, dev, "scsi_host", NULL, NULL, child);
		}
		if (child.empty())
		{
			getChildId(ctx, dev, "sound", NULL, NULL, child);
		}
		if (child.empty())
		{
			getChildId(ctx, dev, "video4linux", NULL, NULL, child);
		}
		if (child.empty())
		{
			getChildId(ctx, dev, "net", NULL, NULL, child);
		}
		if (child.empty())
		{
			getChildId(ctx, dev, "dvb", NULL, NULL, child);
		}

		if ( parent_subsystem != NULL && child.empty() )
		{
			LoggerWrapper::GetInstance()->Write(LV_DEBUG, "+++++++ PCI device added %s = %s", buffer, udi);

			devicesMap[ (std::string) syspath ] = (std::string) udi;
			HalDevice->EVENT_Device_Detected("", "", "", 0, buffer, PCI_COMM_METHOD /*iBusType*/, 0, udi, deviceData.c_str(), "" /*category*/, HalDevice->m_sSignature_get());

			LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Finished firing event for %s", buffer);
		}
	}

	else if ( strcmp(subsystem, "dvb") == 0 && strlen(subsystem) == strlen("dvb") )
	{
		if ( strstr(udi, "frontend") != NULL )
		{
			LoggerWrapper::GetInstance()->Write(LV_DEBUG, "+++++++ DVB device added %s = %s", buffer, udi);

			devicesMap[ (std::string) syspath ] = (std::string) udi;
			HalDevice->EVENT_Device_Detected("", "", "", 0, buffer, iBusType, 0, udi, deviceData.c_str(), "dvb" /*category*/, HalDevice->m_sSignature_get());

			LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Finished firing event for %s",buffer);
		}
	}

	else if ( strcmp(subsystem, "tty") == 0 && strlen(subsystem) == strlen("tty") )
	{
		if ( udev_device_get_parent_with_subsystem_devtype(dev, "usb-serial", NULL ) == NULL
				 && udev_device_get_parent_with_subsystem_devtype(dev, "usb", NULL) != NULL )
		{
			LoggerWrapper::GetInstance()->Write(LV_DEBUG, "+++++++ TTY device added %s = %s", buffer, udi);

			const char *serial_port = udev_device_get_syspath(dev);
			string portID;
			getPortIdentification(string(serial_port), portID);

			if( !deviceData.empty() )
			{
				deviceData += "|";
			}

			deviceData += "37"; // TODO: Constant? StringUtils::itos( DEVICEDATA_COM_Port_on_PC_CONST );
			deviceData += "|";
			deviceData += portID;

			devicesMap[ (std::string) syspath ] = (std::string) udi;
			HalDevice->EVENT_Device_Detected("", "", "", 0, buffer, USB_COMM_METHOD /*iBusType*/, 0, udi, deviceData.c_str(), "serial" /*category*/, HalDevice->m_sSignature_get());

			LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Finished firing event for %s",buffer);
		}
	}

	else if ( strcmp(subsystem, "usb") == 0 && strlen(subsystem) == strlen("usb") )
	{
		if ( udi != NULL && devtype == NULL )
		{
			LoggerWrapper::GetInstance()->Write(LV_DEBUG, "+++++++ USB dongle added %s = %s", buffer, udi);

			devicesMap[ (std::string) syspath ] = (std::string) udi;
			HalDevice->EVENT_Device_Detected("", "", "", 0, buffer, USB_COMM_METHOD /*iBusType*/, 0, udi, deviceData.c_str(), "" /*category*/, HalDevice->m_sSignature_get());

			LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Finished firing event for %s",buffer);
		}
	}

	else if ( subsystem != NULL && ( (strcmp(subsystem, "firewire") == 0 && strlen(subsystem) == strlen("firewire"))
			|| (strcmp(subsystem, "ieee1394") == 0 && strlen(subsystem) == strlen("ieee1394")) ) )
	{
		if( !moreInfo.empty() )
		{
			if( !deviceData.empty() )
			{
				deviceData += "|";
			}

			deviceData += StringUtils::itos( DEVICEDATA_Firewire_ID_CONST );
			deviceData += "|";
			deviceData += moreInfo.c_str();
		}

		LoggerWrapper::GetInstance()->Write(LV_DEBUG, "+++++++ FIR device added %s = %s", moreInfo.c_str(), udi);

		devicesMap[ (std::string) syspath ] = (std::string) udi;
		HalDevice->EVENT_Device_Detected("", "", "", 0, buffer, iBusType, 0, udi, deviceData.c_str(), "ieee1394" /*category*/, HalDevice->m_sSignature_get());

		LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Finished firing event for %s",buffer);
	}

	else // "general" or "other" category - perhaps unknown
	{
		// ignore virtual devices and the following subsystems (categories) for now.
		if ( NULL != udi && NULL != subsystem
				&& NULL == strstr(syspath, "/virtual/")
				&& 0 != strcmp(subsystem, "acpi")
				&& 0 != strcmp(subsystem, "dvb")
				&& 0 != strcmp(subsystem, "hid")
				&& 0 != strcmp(subsystem, "hidraw")
				&& 0 != strcmp(subsystem, "platform")
				&& 0 != strcmp(subsystem, "pnp")
				&& 0 != strcmp(subsystem, "scsi")
				&& 0 != strcmp(subsystem, "scsi_generic")
				&& 0 != strcmp(subsystem, "serio")
				&& 0 != strcmp(subsystem, "tty")
				)
		{
			const char *category = udev_device_get_subsystem(dev);

			LoggerWrapper::GetInstance()->Write(LV_DEBUG, "+++++++ GEN added category = %s || UID = %s", category, udi);

			devicesMap[ (std::string) syspath ] = (std::string) udi;
			HalDevice->EVENT_Device_Detected("", "", "", 0, buffer, iBusType, 0, udi, deviceData.c_str(), category, HalDevice->m_sSignature_get());

			LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Finished firing event for %s",buffer);
		}
	}
}


void LmceUdevD::myDeviceNewCapability(struct udev * ctx, const char * udi, const char *capability)
{
/*
	if( udi == NULL || ctx == NULL || capability == NULL )
	{
		// error
		return;
	}

	gchar *serial_port = libhal_device_get_property_string (ctx, udi, "linux.sysfs_path", NULL);
	if(serial_port != NULL)
	{
		gchar *parent = libhal_device_get_property_string (ctx, libhal_device_get_property_string(ctx, udi, "info.parent", NULL), "info.parent", NULL);
		gchar *info_udi = libhal_device_get_property_string (ctx, parent, "info.udi", NULL);
		int usb_device_product_id = libhal_device_get_property_int(ctx, parent, "usb_device.product_id", NULL);
		int usb_device_vendor_id = libhal_device_get_property_int(ctx, parent, "usb_device.vendor_id", NULL);

		LoggerWrapper::GetInstance()->Write(LV_DEBUG, "udi = %s parent = %s capability = %s serial port = %s\n", udi, parent, capability, serial_port);

		map<unsigned int, int>::iterator it;
		it = templatesMap.find( (unsigned int) ((usb_device_vendor_id & 0xffff) << 16) | (usb_device_product_id & 0xffff) );
		if( it != templatesMap.end() )
		{
			string portID;
			getPortIdentification(string(serial_port), portID);

			LoggerWrapper::GetInstance()->Write(LV_DEBUG, "NewCapability udi = %s serial port = %s port id = \n", udi, serial_port, portID.c_str());
			string responseCreate;

			char buffer[64];
			snprintf(buffer, sizeof(buffer), "%08x", (*it).first);
			HalDevice->EVENT_Device_Detected("", "", "", 0, buffer, 4, 0, info_udi, "37|" + portID, "", HalDevice->m_sSignature_get());
			LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Finished firing event for %s",buffer);
		}

		g_free (parent);
		parent = NULL;
		g_free (info_udi);
		info_udi = NULL;
	}

	g_free (serial_port);
	serial_port = NULL;
*/
}

void LmceUdevD::myDeviceRemoved(struct udev * ctx, struct udev_device * dev)
{
	if( ctx != NULL && dev != NULL )
	{
		// get sysfspath
		const char *syspath = udev_device_get_syspath(dev);
		const char *udi = udi_helper_get_key_by_value(syspath);
		const char *subsystem = udev_device_get_subsystem(dev);
		const char *devtype = udev_device_get_devtype(dev);

		if (udi != NULL && syspath != NULL)
		{
			if (devicesMap.find( (std::string) syspath) != devicesMap.end() )
			{
				string sUDI = devicesMap[ syspath ];
				LoggerWrapper::GetInstance()->Write(LV_DEBUG, "+++++++ ------- Removed %s/%s = %s", subsystem, devtype, sUDI.c_str() );

				if (udi != NULL)
					HalDevice->EVENT_Device_Removed("", "", 0, "", 0, "", 4, 0, sUDI.c_str(), "", "");

				LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Finished firing event");

				devicesMap.erase( (std::string) syspath);
			}
			udi_helper_remove_device(NULL, syspath);
		}
	}
	else
	{
		// error
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "error on HAL passing null arguments to remove");
	}
}

/***/
void LmceUdevD::initialize(struct udev * ctx)
{
	struct udev_list_entry *devices, *dev_list_entry;
	struct udev_enumerate *enumerate;

	enumerate = udev_enumerate_new(ctx);

	// enumerate all devices in sysfs
	udev_enumerate_scan_devices(enumerate);
	devices = udev_enumerate_get_list_entry(enumerate);

	//get all template IDs and producer_id vendor_id
	udev_list_entry_foreach(dev_list_entry, devices) {
		const char *syspath = NULL;
		struct udev_device *dev;

		syspath = udev_list_entry_get_name(dev_list_entry);
		dev = udev_device_new_from_syspath(ctx, syspath);

		myDeviceAdded(ctx, dev);

		udev_device_unref(dev);
	}

	HalDevice->EVENT_Done_Detecting_Devices( HalDevice->m_sSignature_get() );
}


void LmceUdevD::getProductVendorId(	struct udev * ctx, struct udev_device *dev,
									int * prodId, int * vendorId,
									int * subsysProdId, int * subsysVendorId,
									int * busType , string & sysfsPath )
{
	moreInfo = "";
//LoggerWrapper::GetInstance()->Write(LV_DEBUG, " ID-1");
	const char *prod_Id = (udev_device_get_sysattr_value(dev,"idProduct") != NULL ? udev_device_get_sysattr_value(dev,"idProduct") : udev_device_get_sysattr_value(dev,"device"));
	const char *vendor_Id = (udev_device_get_sysattr_value(dev,"idVendor") != NULL ? udev_device_get_sysattr_value(dev,"idVendor") : udev_device_get_sysattr_value(dev,"vendor"));
	const char *subsysProd_Id = (udev_device_get_sysattr_value(dev,"subsystem_device"));
	const char *subsysVendor_Id = (udev_device_get_sysattr_value(dev,"subsystem_vendor"));

//LoggerWrapper::GetInstance()->Write(LV_DEBUG, " ID-2 - %i = %s, %s, %s, %s", *busType, prod_Id, vendor_Id, subsysProd_Id, subsysVendor_Id);
	*prodId = ( prod_Id == NULL ? -1 : strtol(prod_Id, NULL, 16) );
	*vendorId = ( vendor_Id == NULL ? -1 : strtol(vendor_Id, NULL, 16) );
	*subsysProdId = ( subsysProd_Id == NULL ? -1 : strtol(subsysProd_Id, NULL, 16) );
	*subsysVendorId = ( subsysVendor_Id == NULL ? -1 : strtol(subsysVendor_Id, NULL, 16) );

//LoggerWrapper::GetInstance()->Write(LV_DEBUG, " ID-3 - %i = %i, %i, %i, %i", *busType, *prodId, *vendorId, *subsysProdId, *subsysVendorId);

	const char *sysfs_path = udev_device_get_syspath(dev);
	if( sysfs_path != NULL )
	{
		sysfsPath = sysfs_path;
		size_t startPos = sysfsPath.find("pci");
		if( startPos != string::npos )
		{
			sysfsPath = sysfsPath.substr(startPos);
		}
	}

	if( *prodId == -1 || *vendorId == -1 )
	{
		struct udev_device *parent = udev_device_get_parent(dev);
		if( parent != NULL )
		{
			getProductVendorId(ctx, parent, prodId, vendorId, subsysProdId, subsysVendorId, busType, sysfsPath);
		}
	}

	const char *bus = udev_device_get_property_value(dev, "ID_BUS");

	*busType = UNKNOWN_COMM_METHOD;
	if ( udev_device_get_parent_with_subsystem_devtype(dev, "firewire", NULL) != NULL)
	{
		 char tempBuffer[32];// 14+1 should be enough
		 snprintf(tempBuffer, sizeof(tempBuffer), "%s", udev_device_get_sysattr_value(dev, "guid") );
		 moreInfo = tempBuffer;
 		*busType = UNKNOWN_COMM_METHOD;
	}
	else if ( udev_device_get_parent_with_subsystem_devtype(dev, "ieee1394", NULL) != NULL)
	{
		 char tempBuffer[32];// 14+1 should be enough
		 snprintf(tempBuffer, sizeof(tempBuffer), "%s", udev_device_get_sysattr_value(dev, "guid") );
		 moreInfo = tempBuffer;
		*busType = UNKNOWN_COMM_METHOD;
	}
	else if ( udev_device_get_parent_with_subsystem_devtype(dev, "usb", "usb_device") != NULL)
	{
		*busType = USB_COMM_METHOD;
	}
	else if ( (bus != NULL && 0 == strcmp(bus, "usb")) || udev_device_get_parent_with_subsystem_devtype(dev, "usb", NULL) != NULL)
	{
		*busType = USB_COMM_METHOD;
	}
	else if ( udev_device_get_parent_with_subsystem_devtype(dev, "scsi", NULL) != NULL)
	{
		*busType = 10;
	}
	else if ( (bus != NULL && 0 == strcmp(bus, "pci")) || udev_device_get_parent_with_subsystem_devtype(dev, "pci", NULL) != NULL)
	{
		*busType = PCI_COMM_METHOD;
	}
}

void* LmceUdevD::startUp(void *device)
{
/*
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "LmceUdevD::startUp  Waiting 10 seconds to let GSD devices start first and disable any invalid ports");
	Sleep(10000);
*/

	struct udev * ctx = NULL;
	struct udev_monitor *mon = NULL;
	int fd;

	udevdrunning = true;
	LoggerWrapper::GetInstance()->Write(LV_DEBUG, "LmceUdevD::startUp pthread_create ############ Start ");

	if( device == NULL )
	{
		// error
		udevdrunning = false;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "LmceUdevD::startUp device == NULL ");
		return NULL;
	}

	HalDevice = (HAL *)device;
	if( HalDevice->m_pData == NULL )
	{
		// error
		udevdrunning = false;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "LmceUdevD::startUp HalDevice->m_pData == NULL ");
		return NULL;
	}

	// initialize the udi helper
	udi_helper_map_table_new();

	ctx = udev_new();
	if ( ctx == NULL )
	{
		udevdrunning = false;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "LmceUdevD::startUp udev CTX is NULL!\n");
		return NULL;
	}

	// create the udev monitor
	mon = udev_monitor_new_from_netlink(ctx, "udev");
	if ( mon == NULL )
	{
		udevdrunning = false;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "LmceUdevD::startUp udev mon is NULL!\n");
		return NULL;
	}

	// Turn on the udev event monitor for all events
        udev_monitor_enable_receiving(mon);

	// Get the file descriptor (fd) for the monitor.  This fd will get passed to select() for non-blocking access to monitor events
        fd = udev_monitor_get_fd(mon);

	// enumerate all existing devices
	initialize(ctx);

	while (!threadShutdown)
	{
		/* Set up the call to select(). In this case, select() will
		   only operate on a single file descriptor, the one
		   associated with our udev_monitor. Note that the timeval
		   object is set to 0, which will cause select() to not
		   block. */
		fd_set fds;
		struct timeval tv;
		int ret;

		FD_ZERO(&fds);
		FD_SET(fd, &fds);
		tv.tv_sec = 0;
		tv.tv_usec = 0;

		ret = select(fd+1, &fds, NULL, NULL, &tv);

		/* Check if our file descriptor has received data. */
		if (ret > 0 && FD_ISSET(fd, &fds)) {
			usleep(1000); // 1ms - wait for children to show in udev?
			/* Make the call to receive the device.
			   select() ensured that this will not block. */
			struct udev_device *dev = udev_monitor_receive_device(mon);

			if (dev != NULL)
			{
				const char *action = udev_device_get_action(dev);
				const char *subsystem = udev_device_get_subsystem(dev);
				const char *devtype = udev_device_get_devtype(dev);
				const char *syspath = udev_device_get_syspath(dev);

			if ( 0 != strcmp(subsystem) )
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "LmceUdevD::Monitor EVENT %s: %s, %s, %s", 
					action,
					subsystem,
					devtype,
					syspath );

				if (action != NULL)
				{
					if ( 0 == strcmp(action, "add") && strlen(action) == strlen("add") )
					{
						myDeviceAdded(ctx, dev);
					}
					else if ( 0 == strcmp(action, "remove") && strlen(action) == strlen("remove") )
					{
						myDeviceRemoved(ctx, dev);
					}
					//else if ( 0 == strcmp(action, "change") && strlen(action) == strlen("change") )
					//{
					//	myDeviceNewCapability(ctx, dev);
					//}
				}
	                        else
				{
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "LmceUdevD::Monitor receive_device() returned NULL action");
				}
			}
                        else
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "LmceUdevD::Monitor receive_device() returned NULL device");
			}
		}
		else
		{
			usleep(1000000); // 1sec
		}
	}

	udev_monitor_unref (mon);

	// TODO: clear the udi cache.
	// udi_helper_map_table_freeall();

	LoggerWrapper::GetInstance()->Write(LV_DEBUG, "LmceUdevD::startUp  ############ END -----------  running=%d",(int) udevdrunning);

	udevdrunning = false;
	threadShutdown = false;
	return NULL;
}

bool LmceUdevD::shutDown()
{

	threadShutdown = true;

	LoggerWrapper::GetInstance()->Write(LV_DEBUG, "############ LmceUdevD shutDown waiting for thread running=%d",(int) udevdrunning);
	time_t timeout = time(NULL) + 15;
	while( udevdrunning && time(NULL) < timeout )
	{
		usleep(10);
	}
	LoggerWrapper::GetInstance()->Write(LV_DEBUG, "############ LmceUdevD shutDown thread exited or timed out: running=%d",(int) udevdrunning);
	return udevdrunning==false;
}
