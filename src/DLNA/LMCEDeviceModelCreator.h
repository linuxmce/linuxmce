/*
     Copyright (C) 2012 LinuxMCE
     www.linuxmce.org

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#ifndef LMCEDeviceModelCreator_h
#define LMCEDeviceModelCreator_h

#include <HUpnpAv/HAvDeviceModelCreator>
#include <HUpnpAv/HMediaRendererInfo>
//#include <HUpnpAv/HMediaRendererDevice>
//#include <HUpnp/HServerDevice>
#include <HUpnpCore/HDeviceInfo>

#include "LMCERenderer.h"
#include "DLNACallBack.h"

using namespace Herqq::Upnp;
namespace DCE {
class LMCEDeviceModelCreator : public HAvDeviceModelCreator {
	public:

/*	HServerDevice* createDevice(const HDeviceInfo& info) {
		if (info.deviceType().compare(
			    HMediaRendererInfo::supportedDeviceType(), HResourceType::Inclusive))
		{
			if (h_ptr->m_mediaRendererConf && h_ptr->m_mediaRendererConf->isValid())
			{
				return new HMediaRendererDevice(*h_ptr->m_mediaRendererConf);
				}
		} else {
		return HAvDeviceModelCreator::createDevice(info);
		}
		}*/
};
}
#endif
