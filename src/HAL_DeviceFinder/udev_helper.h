/*

 Copyright (C) 2015 Jason Murduck.

 This file is licenced under GPL even if this whole program is not.
 Whatever needs to be said to make that happen should go here.

 */
#ifndef __UDEV_HELPER_H__
#define __UDEV_HELPER_H__

#include <map>
#include <string>
#include <vector>

#include <libudev.h>

using namespace std;

class UdevHelper
{
	private:

		static void getPortIdentification(string portFromBus, string& portID);
		static void InternalFindDevice(struct udev * ctx, const map<unsigned long, vector<string> >& mapDevices);

	public:

		static void* FindDevice(const map<unsigned long, vector<string> >& mapDevices);
};

#endif /*__UDEV_HELPER_H__*/
