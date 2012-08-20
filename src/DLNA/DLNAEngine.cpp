/*
     Copyright (C) 2012 LinuxMCE
     www.linuxmce.org

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "DLNAEngine.h"
#include "LMCERendererConnectionManager.h"
#include <HUpnpCore/HDeviceHostConfiguration>
#include <HUpnpCore/HDeviceConfiguration>
#include <HUpnpCore/HServerDevice>
#include <HUpnpCore/HServerService>
#include <HUpnpAv/HMediaRendererDeviceConfiguration>

#include <QtNetwork/QHostAddress>
#include <QtCore/QList>

#include "DCE/Logger.h"

#include "LMCEDeviceModelCreator.h"
#include "LMCEMediaRendererDeviceConfiguration.h"
using namespace DCE;

DLNAEngine::DLNAEngine(DLNA* pDlna)
{
	m_pDLNA = pDlna;
}

DLNAEngine::~DLNAEngine()
{
	delete m_pDeviceHost;

}

int DLNAEngine::Init()
{
	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "DLNAEngine::Start() start");

	Herqq::Upnp::SetLoggingLevel(Warning);

	// Initialize hupnp with the xml file
	m_pDeviceHost = new HDeviceHost(this);

	HDeviceHostConfiguration hostConf;
	QList<QHostAddress> list;
	// TODO: get internal network address
	list.append(QHostAddress("192.168.0.50"));
	hostConf.setNetworkAddressesToUse(list);

	LMCEDeviceModelCreator creator;

// TODO: currently the HDeviceInfo object stops at the HAvDeviceModelCreator::createDevice, while we
// need to keep it all the way through to LMCERenderer
// maybe it can be solved with some clever sub-classing of, and implementation of
// HAvDeviceModelCreator::createDevice, HMediaRendererDevice::createMediaConnection, 

// HMediaRendererDevice is a subclass of HServerDevice, and that has the HDeviceInfo
// so how to get to it from the RendererConnection...?

// Is some of the creator or configuration classes cloned? if so, it could be that the parent-child hierarchy is as follows:
// HMediaRendererConnection -> LMCERendererConnectionManager -> LMCERenderer

	LMCEMediaRendererDeviceConfiguration mrConf;
	//  	HMediaRendererDeviceConfiguration mrConf;
	LMCERendererConnectionManager* pRcm = new LMCERendererConnectionManager(m_pDLNA);
	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "DLNAEngine() pRcm = %d", (unsigned int)pRcm);

	mrConf.setRendererConnectionManager(pRcm, true);
	creator.setMediaRendererConfiguration(mrConf);

	hostConf.setDeviceModelCreator(creator);


	// Write a xml file describing all possible renderers(xine, squeezeboxes, etc)
	// One per MD : xine for audio/video, picture_player for pictures
	// Also one for varoius networked players
	// DEVICECATEGORY_Media_Players_CONST, DEVICECATEGORY_Network_Audio_Players_CONST, DEVICECATEGORY_Network_Video_Players_CONST
// maybe there is a way to get all available players in the system? MEdia_Plugin?
	QString deviceFile = "./xml/testdevice.xml";
	QString deviceFile2 = "./xml/testdevice2.xml";

	HDeviceConfiguration deviceConf;
	deviceConf.setPathToDeviceDescription(deviceFile);
	deviceConf.setCacheControlMaxAge(1800);
	hostConf.add(deviceConf);

	HDeviceConfiguration deviceConf2;
	deviceConf2.setPathToDeviceDescription(deviceFile2);
	hostConf.add(deviceConf2);

	if (!m_pDeviceHost->init(hostConf)) 
	{
		LoggerWrapper::GetInstance ()->Write (LV_STATUS, "DLNAEngine::Run(): devicehost init failed:");
		LoggerWrapper::GetInstance ()->Write (LV_STATUS, "DLNAEngine::Run(): Error: %s", m_pDeviceHost->errorDescription().toStdString().c_str());


	}
	
	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "DLNAEngine::Run(): rootDevices: %d", m_pDeviceHost->rootDevices().size());
	for (int i = 0; i < m_pDeviceHost->rootDevices().size(); i++)
	{
//		cout << m_pDeviceHost->rootDevices()[i]->description().toStdString().c_str();
		LoggerWrapper::GetInstance ()->Write (LV_STATUS, "isStarted %i", m_pDeviceHost->isStarted());
		
		for (int j = 0; j < m_pDeviceHost->rootDevices()[i]->services().size(); j++)
		{
//			cout << m_pDeviceHost->rootDevices()[i]->services()[j]->description().toStdString().c_str();
		}
	}


}

