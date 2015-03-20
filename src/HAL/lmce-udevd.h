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
#ifndef LMCE_UDEVD_H_
#define LMCE_UDEVD_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <map>
#include <string>

#include <libudev.h>
#include <glib-2.0/glib.h>

#include "udi-helper.h"

namespace DCE
{
	class HAL;
	class Message;
}

using namespace DCE;

class LmceUdevD
{
	private:
		static std::map<unsigned int, int> templatesMap;
		static DCE::HAL * HalDevice;
		static long pnpDeviceID;
		static GMainLoop * loop;
		static bool threadShutdown;
		static bool udevdrunning;
		static std::string moreInfo;
		static std::map<std::string, std::string> devicesMap;

	public:
		LmceUdevD();

		/***/
		 static void isUsbDevice(struct udev * ctx, struct udev_device ** dev);

		/***/
		static void* startUp(void * device);

		/***/
		static bool shutDown();

		/***/
		static void mainloop_integration (struct udev * ctx);

		/***/
//		static void myDeviceAdded(struct udev * ctx, const char * udi);
		static void myDeviceAdded(struct udev * ctx, struct udev_device * dev);

		/***/
		static void myDeviceNewCapability(struct udev * ctx, const char * udi, const char *capability);

		/***/
//		static void myDeviceRemoved(struct udev * ctx, const char * udi);
		static void myDeviceRemoved(struct udev * ctx, struct udev_device * dev);

		/***/
		static void getPortIdentification(std::string portFromBus, std::string& portID);

		/***/
		static void initialize(struct udev * ctx);

		/***/
		static void getProductVendorId(
			struct udev * ctx,
			struct udev_device * dev,
			int * prodId,
			int * vendorId,
			int * subsysProdId,
			int * subsysVendorId,
			int * busType,
			std::string & sysfsPath
		);

		/***/
		static void getParentId(struct udev * ctx, struct udev_device * dev,
								const char * sub, const char * type,
								const char * tag, const char * value,
								std::string & parentId, std::string & parentTagValue);

		/***/
		static void getChildId(struct udev * ctx, struct udev_device * dev,
								const char * sub, const char * tag, const char * value,
								std::string & childId);

		static void getSerialParent(const char * sysfs, std::string & parentSysfs);

};

#endif /*LMCE_UDEVD_H_*/
