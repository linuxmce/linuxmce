/*
     Copyright (C) 2012 LinuxMCE
     www.linuxmce.org

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "LMCERendererConnectionManager.h"
#include <HUpnpAv/HRendererConnection>
#include "DCE/Logger.h"

using namespace DCE;

LMCERendererConnectionManager::LMCERendererConnectionManager(DLNA* pDLNA)
{
	m_pDLNA = pDLNA;
	m_iCount = 0;
}

LMCERendererConnectionManager::~LMCERendererConnectionManager()
{

}

HRendererConnection* LMCERendererConnectionManager::doCreate (HAbstractConnectionManagerService* cmService, HConnectionInfo* connectionInfo)
{
	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERendererConnectionManager::doCreate() start");
	LMCERenderer *pRenderer = new LMCERenderer(cmService, this, m_pDLNA);
	return pRenderer;
}

DLNA* LMCERendererConnectionManager::GetDLNA()
{
	return m_pDLNA;
}
