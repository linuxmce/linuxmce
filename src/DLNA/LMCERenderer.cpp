/*
     Copyright (C) 2012 LinuxMCE
     www.linuxmce.org

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "DCE/Logger.h"
#include "LMCERenderer.h"
#include <HUpnpAv/HAvDeviceModelCreator>
#include <HUpnpCore/HServerDevice>
#include <HUpnpCore/HDeviceInfo>
#include <HUpnpAv/HDeviceCapabilities>
#include <HUpnpAv/HRendererConnectionInfo>
#include <HUpnpAv/HStorageMedium>
#include <HUpnpAv/HAbstractMediaRendererDevice>
#include <HUpnpAv/HAbstractRenderingControlService>
#include <HUpnpAv/HChannel>
#include <HUpnpAv/HProtocolInfo>
#include <HUpnpAv/../../src/mediarenderer/hconnectionmanager_sinkservice_p.h>
#include <HUpnpAv/HTransportStatus>
#include <HUpnpAv/HTransportSettings>
#include <HUpnpAv/HRecordQualityMode>

#include <QtCore/QUrl>
#include <QtCore/QSet>
#include <QtCore/QHashNode>

#include "Gen_Devices/AllCommandsRequests.h"
#include "pluto_main/Define_MediaType.h"
#include "pluto_main/Define_DeviceTemplate.h"
#include "DLNA.h"
#include "DLNACallBack.h"

using namespace DCE;
using namespace Herqq::Upnp;

LMCERenderer::LMCERenderer(QObject* parent, DLNA* pDLNA) :
	HRendererConnection(parent)
{
	HAbstractMediaRendererDevice* pDevice = dynamic_cast<HAbstractMediaRendererDevice*>(parent->parent());
	if (pDevice != NULL)
	{
		LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERenderer() found HAbstractMediaRendererDevice as parent.parent()");
		HDeviceInfo info = pDevice->info();
		QString serial = info.serialNumber();

		LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERenderer() serialNumber(PK_EntertainArea) = %s", serial.toStdString().c_str());
		m_PK_EntArea = atoi(serial.toStdString().c_str());
	} else {
		LoggerWrapper::GetInstance ()->Write (LV_CRITICAL, "LMCERenderer() No PK_EntertainArea set - this renderer won't do anything");
		m_PK_EntArea = 0;
	}

	// Use EntArea for everyone... Then the LMCERenderer can figure out what to do, and return a failed staus at its discresion
	m_pDLNA = pDLNA;


	// TODO: Handle different media types... Audio goes to Xine, but pictures should go to the picture player.
	// Squeezeboxes are simpler as they only do audio, but networked_video_players probably does audio,video and picture
	// Load all rooms, entertainment areas and the devices in them
	// need to find out what we can play in our EA
	// load ea - device information,

	HRendererConnectionInfo *info = writableInfo();
	info->setVolume(HChannel::Master, 50);
	info->setTransportStatus(HTransportStatus::OK);
	
	// TODO: hack: implement our own ConnectionManager
	HConnectionManagerSinkService* s = dynamic_cast<HConnectionManagerSinkService*>(pDevice->connectionManager());
	HProtocolInfos piList;
/*

http-get:*:audio/mpeg:*,
http-get:*:audio/L16;rate=8000;channels=1:*,
http-get:*:audio/L16;rate=8000;channels=2:*,
http-get:*:audio/L16;rate=11025;channels=1:*,http-get:*:audio/L16;rate=11025;channels=2:*,http-get:*:audio/L16;rate=12000;channels=1:*,http-get:*:audio/L16;rate=12000;channels=2:*,http-get:*:audio/L16;rate=16000;channels=1:*,http-get:*:audio/L16;rate=16000;channels=2:*,http-get:*:audio/L16;rate=22050;channels=1:*,http-get:*:audio/L16;rate=22050;channels=2:*,http-get:*:audio/L16;rate=24000;channels=1:*,http-get:*:audio/L16;rate=24000;channels=2:*,http-get:*:audio/L16;rate=32000;channels=1:*,
http-get:*:audio/L16;rate=32000;channels=2:*,
http-get:*:audio/L16;rate=44100;channels=1:*,
http-get:*:audio/L16;rate=44100;channels=2:*,http-get:*:audio/L16;rate=48000;channels=1:*,http-get:*:audio/L16;rate=48000;channels=2:*,
http-get:*:audio/vnd.dlna.adts:*,
http-get:*:audio/vnd.dlna.adts:*,
http-get:*:audio/mp4:*,
http-get:*:audio/mp4:*,
http-get:*:audio/mp4:*,
http-get:*:audio/x-ms-wma:*,
http-get:*:audio/x-ms-wma:*,
http-get:*:audio/x-ms-wma:*,
http-get:*:application/ogg:*,
http-get:*:audio/x-flac:*

 */
	piList.append(HProtocolInfo("http-get","*","audio/mpeg","*"));
	piList.append(HProtocolInfo("http-get","*","audio/mp4","*"));
	piList.append(HProtocolInfo("http-get","*","audio/vnd.dlna.adts","*"));
	piList.append(HProtocolInfo("http-get","*","audio/x-ms-wma","*"));
	piList.append(HProtocolInfo("http-get","*","audio/application/ogg","*"));
	piList.append(HProtocolInfo("http-get","*","audio/x-flac","*"));

	//	s->setSinkProtocolInfo(piList);

/*	LoggerWrapper::GetInstance ()->Write (LV_CRITICAL, "LMCERenderer() getCapabilitied %s", info->deviceCapabilities().playMedia().count()); */
	HDeviceCapabilities caps;
	QSet<HStorageMedium> playMediaSet;
	playMediaSet.insert(HStorageMedium::Network);
	playMediaSet.insert(HStorageMedium::HDD);
	caps.setPlayMedia(playMediaSet);

	QSet<HStorageMedium> recMediaSet;
	recMediaSet.insert(HStorageMedium::NotImplemented);
	caps.setRecordMedia(recMediaSet);

	info->setDeviceCapabilities(caps);

	QSet<HRecordQualityMode> rqmSet;
	rqmSet.insert(HRecordQualityMode::NotImplemented);
	info->setPossibleRecordQualityModes(rqmSet);

	info->transportSettings().setRecordQualityMode(HRecordQualityMode::NotImplemented);

}

LMCERenderer::~LMCERenderer()
{

}

qint32 LMCERenderer::doPlay(const QString& speed)
{
	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERenderer::doPlay() start, speed = %s", speed.toStdString().c_str());
	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERenderer::doPlay() start, m_Url = %s", m_Url->toString().toStdString().c_str());

	CMD_MH_Play_Media_DT CMD_MH_Play_Media_DT(0, DEVICETEMPLATE_Media_Plugin_CONST, BL_SameHouse,
						  0,m_Url->toString().toStdString(),MEDIATYPE_lmce_StreamedAudio_CONST,
						  0,StringUtils::itos(m_PK_EntArea),false,0,false,false,false);
	if (m_pDLNA->SendCommand(CMD_MH_Play_Media_DT)) {
		LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERenderer::doPlay() successful");
		return UpnpSuccess;
	}

}

qint32 LMCERenderer::doStop()
{
	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERenderer::doStop() start");
	CMD_MH_Stop_Media_DT CMD_MH_Stop_Media_DT(0, DEVICETEMPLATE_Media_Plugin_CONST,  BL_SameHouse,
						  0,0,0,StringUtils::itos(m_PK_EntArea),false);
	if (m_pDLNA->SendCommand(CMD_MH_Stop_Media_DT)) {
		LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERenderer::doStop() successful");
		return UpnpSuccess;
	}
}

qint32 LMCERenderer::doPause()
{
	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERenderer::doPause() start");
		return UpnpSuccess;

	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERenderer::doPause() end");
}

qint32 LMCERenderer::doSeek(const HSeekInfo& seekInfo)
{
		return UpnpSuccess;
	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERenderer::doSeek() start");

	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERenderer::doSeek() end");
}

qint32 LMCERenderer::doNext()
{
	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERenderer::doNext() start");
		return UpnpSuccess;

	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERenderer::doNext() end");
}

qint32 LMCERenderer::doPrevious()
{
	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERenderer::doPrevious() start");
		return UpnpSuccess;

	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERenderer::doPrevious() end");
}

qint32 LMCERenderer::doSetResource ( const QUrl &  resourceUri, HObject *  cdsMetadata)
{
	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERenderer::doSetResource() start: resourceUri = %s", resourceUri.toString().toStdString().c_str());
	// TODO: check cdsMetadata->type()
	m_Url = new QUrl(resourceUri);
	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERenderer::doSetResource() end");
	return UpnpSuccess;
}

qint32 LMCERenderer::doSetNextResource ( const QUrl &  resourceUri, HObject *  cdsMetadata)
{
	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERenderer::doSetNextResource() start: resourceUri = %s", resourceUri.toString().toStdString().c_str() );

		return UpnpSuccess;

	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERenderer::doSetNextResource() end");
}

qint32 LMCERenderer::doSelectPreset ( const QString &  presetName)
{
	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERenderer::doSelectPreset() start: presetName = %s", presetName.toStdString().c_str());

		return UpnpSuccess;

	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERenderer::doSelectPreset() end");
}

