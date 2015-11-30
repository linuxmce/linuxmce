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
#include <HUpnpAv/HFileSystemDataSource>
#include <HUpnpAv/HMediaServerDeviceConfiguration>
#include <HUpnpAv/HContentDirectoryServiceConfiguration>
#include <HUpnpAv/HRootDir>

#include <QtCore/QDir>
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
	//TODO: delete files ?
	delete m_pDeviceHost;
}

int DLNAEngine::Init()
{
	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "DLNAEngine::Start() start");

	Herqq::Upnp::SetLoggingLevel(Information);

	m_pDeviceHost = new HDeviceHost(this);

	HDeviceHostConfiguration hostConf;
	QList<QHostAddress> list;
	// TODO: get internal network address
	string sCoreIP = m_pDLNA->GetIpAddress(1);
	LoggerWrapper::GetInstance ()->Write (LV_WARNING, "DLNAEngine() binding DLNA devices to IP %s", sCoreIP.c_str());
	list.append(QHostAddress(sCoreIP.c_str()));
	hostConf.setNetworkAddressesToUse(list);

	// Device model creator - common to all devices
	HAvDeviceModelCreator creator;

	// Set up Media Renderer devices (if enabled)
	if ( m_pDLNA->m_bEnableMediaRenderer ) {
		LoggerWrapper::GetInstance ()->Write (LV_WARNING, "DLNAEngine() setting up Media Renderers");

		// Set up device creator and configuration manager
		HMediaRendererDeviceConfiguration mrConf;
		LMCERendererConnectionManager* pRcm = new LMCERendererConnectionManager(m_pDLNA);

		mrConf.setRendererConnectionManager(pRcm, true);
		creator.setMediaRendererConfiguration(mrConf);

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
			
			// Add device to host configuration
			HDeviceConfiguration deviceConf;
			deviceConf.setPathToDeviceDescription(deviceFile);
			deviceConf.setCacheControlMaxAge(1800);
			hostConf.add(deviceConf);
		}
	}

	// Set up Media Server (if enabled)
	if ( m_pDLNA->m_bEnableMediaServer ) {
		LoggerWrapper::GetInstance ()->Write (LV_WARNING, "DLNAEngine() setting up Media Server");

		HFileSystemDataSourceConfiguration datasourceConfig;
		datasourceConfig.addRootDir(HRootDir(QDir("/home/public/data/audio/"), HRootDir::RecursiveScan, HRootDir::WatchForChanges));
		datasourceConfig.addRootDir(HRootDir(QDir("/home/public/data/videos/"), HRootDir::RecursiveScan, HRootDir::WatchForChanges));
		datasourceConfig.addRootDir(HRootDir(QDir("/home/public/data/pictures/"), HRootDir::RecursiveScan, HRootDir::WatchForChanges));
		HFileSystemDataSource* m_datasource = new HFileSystemDataSource(datasourceConfig);
		
		HContentDirectoryServiceConfiguration cdsConfig;
		cdsConfig.setDataSource(m_datasource, false);
		
		HMediaServerDeviceConfiguration mediaServerConfig;
		mediaServerConfig.setContentDirectoryConfiguration(cdsConfig);
		creator.setMediaServerConfiguration(mediaServerConfig);

		HDeviceConfiguration config;
		config.setPathToDeviceDescription("./xml/lmceDlnaMediaServer.xml");
		config.setCacheControlMaxAge(180);

		hostConf.add(config);

	}

	hostConf.setDeviceModelCreator(creator);

	// Set up Control Points (if enabled)
	if ( m_pDLNA->m_bEnableControlPoints ) {
//		LoggerWrapper::GetInstance ()->Write (LV_WARNING, "DLNAEngine() starting Control Points");

		// TODO:
	}

	LoggerWrapper::GetInstance ()->Write (LV_WARNING, "DLNAEngine() initing m_pDeviceHost");
	if (!m_pDeviceHost->init(hostConf)) 
	{
		LoggerWrapper::GetInstance ()->Write (LV_CRITICAL, "DLNAEngine::Run(): devicehost init failed:");
		LoggerWrapper::GetInstance ()->Write (LV_CRITICAL, "DLNAEngine::Run(): Error: %s", m_pDeviceHost->errorDescription().toStdString().c_str());
	}
	
	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "DLNAEngine::Run(): rootDevices: %d", m_pDeviceHost->rootDevices().size());
	for (int i = 0; i < m_pDeviceHost->rootDevices().size(); i++)
	{
		LoggerWrapper::GetInstance ()->Write (LV_STATUS, "isStarted %i", m_pDeviceHost->isStarted());
	}

	return 0;
}

