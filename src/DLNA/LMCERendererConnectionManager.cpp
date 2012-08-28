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

HRendererConnection* LMCERendererConnectionManager::doCreate (const QString &contentFormat, qint32 connectionId)
{
	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERendererConnectionManager::doCreate() start, connectionId = %d", connectionId);
	LMCERenderer *pRenderer = new LMCERenderer(this, m_pDLNA);
	return pRenderer;
}

bool LMCERendererConnectionManager::connectionComplete ( qint32  connectionId) 
{
	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERendererConnectionManager::connectionComplete() , connectionId = %d", connectionId);
	HRendererConnectionManager::connectionComplete(connectionId);
}

DLNA* LMCERendererConnectionManager::GetDLNA()
{
	return m_pDLNA;
}
