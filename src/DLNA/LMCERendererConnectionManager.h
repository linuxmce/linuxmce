/*
     Copyright (C) 2012 LinuxMCE
     www.linuxmce.org

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#ifndef LMCERendererConnectionManager_h
#define LMCERendererConnectionManager_h

#include <HUpnpAv/HRendererConnectionManager>
#include <HUpnpAv/HAbstractConnectionManagerService>

#include "LMCERenderer.h"
#include "DLNA.h"

using namespace Herqq::Upnp;

class LMCERendererConnectionManager : public HRendererConnectionManager {
private:
	DLNA* m_pDLNA;
	int m_iCount;
public:
	LMCERendererConnectionManager(DLNA* pDLNA);
	virtual ~LMCERendererConnectionManager();

	virtual HRendererConnection * doCreate (HAbstractConnectionManagerService* cmService, HConnectionInfo* connectionInfo);
	virtual DLNA* GetDLNA();
};
#endif
