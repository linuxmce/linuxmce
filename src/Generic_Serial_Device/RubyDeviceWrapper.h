/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

/** @file RubyDeviceWrapper.h
For ?
*/
#ifndef DCERUBYDEVICEWRAPPER_H
#define DCERUBYDEVICEWRAPPER_H

#include <string>
#include <map>

/** @namespace DCE
The Data Commands and Events (DCE) namespace.
*/
namespace DCE {

/** @class RubyDeviceWrapper
For ???
*/
class RubyDeviceWrapper {
public:
    RubyDeviceWrapper();
    ~RubyDeviceWrapper();

public:
	int getDevId() {
		return devid_;
	}
	void setDevId(int devid) {
		devid_ = devid;
	}

	int getDevTemplId() {
		return devtemplid_;
	}
	void setDevTemplId(int devtemplid) {
		devtemplid_ = devtemplid;
	}

	const std::map<int, std::string>& getData() {
		return devdata_;
	}
	void setData(const std::map<int, std::string>& devdata) {
		devdata_ = devdata;
	}

	DCE::RubyDeviceWrapper* getParent() {
		return parent_;
	}

	void setParent(DCE::RubyDeviceWrapper* parent) {
           parent_ = parent;
	}

	std::map<int, DCE::RubyDeviceWrapper>& getChildDevices() {
		return childdevices_;
	}
	void setChildDevices(std::map<int, DCE::RubyDeviceWrapper>& devices) {
		childdevices_ = devices;
	}

	std::map<std::string, int>& getMapPortChannel_Device() {
		return mapPortChannel_Device_;
	}
	void setMapPortChannel_Device(std::map<std::string, int>& mapPC_D) {
		mapPortChannel_Device_ = mapPC_D;
	}

	std::map<int, std::string>& getMapDevice_PortChannel() {
		return mapDevice_PortChannel_;
	}
	void setMapDevice_PortChannel(std::map<int, std::string>& mapD_PC) {
		mapDevice_PortChannel_ = mapD_PC;
	}

public:
	RubyDeviceWrapper* FindChildDevice(int devid);

public:
	int devid_;
	int devtemplid_;
	std::map<int, std::string> devdata_;

	DCE::RubyDeviceWrapper* parent_;
	std::map<int, DCE::RubyDeviceWrapper> childdevices_; /*child device*/

	std::map<std::string, int> mapPortChannel_Device_;
	std::map<int, std::string> mapDevice_PortChannel_;
};

};

#endif
