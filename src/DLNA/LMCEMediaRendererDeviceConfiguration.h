/*
     Copyright (C) 2012 LinuxMCE
     www.linuxmce.org

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#ifndef LMCEMediaRendererDeviceConfiguration_h
#define LMCEMediaRendererDeviceConfiguration_h

#include <HUpnpAv/HMediaRendererDeviceConfiguration>

namespace DCE {
  class LMCEMediaRendererDeviceConfiguration: public HMediaRendererDeviceConfiguration {
	public:
		virtual void doClone(HClonable* target) const
		{
			cout << "LMCEMediaRendererDeviceConfiguration::doClone() " << target << "\n";
			LMCEMediaRendererDeviceConfiguration* conf =
				dynamic_cast<LMCEMediaRendererDeviceConfiguration*>(target);
				    
			if (!target)
			{
				return;
			}
			// Hack to create a new ConnectionManager class for each HMediaRendererDevice
			// (a ConnectionManager will only allow one connection with id 0, which is the default
			conf->setRendererConnectionManager(new LMCERendererConnectionManager((dynamic_cast<LMCERendererConnectionManager*>(rendererConnectionManager()))->GetDLNA()), true);
		}

		virtual LMCEMediaRendererDeviceConfiguration* newInstance() const
		{
			return new LMCEMediaRendererDeviceConfiguration();
		}
	};

}

#endif

