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
// Solved by object hiearchy :
// HMediaRendererConnection -> LMCERendererConnectionManager -> LMCERenderer

	LMCEMediaRendererDeviceConfiguration mrConf;
	//  	HMediaRendererDeviceConfiguration mrConf;
	LMCERendererConnectionManager* pRcm = new LMCERendererConnectionManager(m_pDLNA);
	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "DLNAEngine() pRcm = %d", (unsigned int)pRcm);

	mrConf.setRendererConnectionManager(pRcm, true);
	creator.setMediaRendererConfiguration(mrConf);

	hostConf.setDeviceModelCreator(creator);


	// Write a xml file representing a DLNA device for each entertainment areas
	map<int, EntertainArea*> *pMapEntertainAreas = m_pDLNA->GetEntertainAreas();
	string path = "./xml/lmceDlnaDevice_";
	string templateFile = "./xml/lmceDlnaDevice_tmpl.xml";
	string data;
	FileUtils::ReadTextFile(templateFile, data);
	for(map<int,EntertainArea *>::iterator it=pMapEntertainAreas->begin();it!=pMapEntertainAreas->end();++it)
	{
		int PK_EntArea = (*it).first;
		LoggerWrapper::GetInstance ()->Write (LV_STATUS, "DLNAEngine() writing device description xml file for ent area %d", (unsigned int)PK_EntArea);
		string file = path + StringUtils::itos(PK_EntArea) + ".xml";
		QString deviceFile(file.c_str());
		
		string sConfigData;
		sConfigData += data;
 
		// Replace the template tags
		sConfigData=StringUtils::Replace(sConfigData, "%SERIALNUMBER%", StringUtils::itos(PK_EntArea));
		sConfigData=StringUtils::Replace(sConfigData, "%DEVICE_NAME%", "LinuxMCE " + (*it).second->m_sDescription);
		sConfigData=StringUtils::Replace(sConfigData, "%UUID%", "3a75ea42-1a88-7e1d-30aa-4b87ae459" + StringUtils::itos(100+PK_EntArea));

		FileUtils::WriteTextFile(file, sConfigData);

		HDeviceConfiguration deviceConf;
		deviceConf.setPathToDeviceDescription(deviceFile);
		deviceConf.setCacheControlMaxAge(1800);
		hostConf.add(deviceConf);

	}

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

