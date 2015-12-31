/*
     Copyright (C) 2012 LinuxMCE
     www.linuxmce.org

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "DCE/Logger.h"
#include "AlarmManager.h"
#include "LMCERenderer.h"

#include <HUpnpAv/HAvDeviceModelCreator>
#include <HUpnpCore/HServerDevice>
#include <HUpnpCore/HDeviceInfo>
#include <HUpnpAv/HDeviceCapabilities>
#include <HUpnpAv/HRendererConnectionInfo>
#include <HUpnpAv/HStorageMedium>
#include <HUpnpAv/HAbstractMediaRendererDevice>
#include <HUpnpAv/HAbstractRenderingControlService>
#include <HUpnpAv/HAbstractConnectionManagerService>
#include <HUpnpAv/HConnectionManagerInfo>
#include <HUpnpAv/HChannel>
#include <HUpnpAv/HItem>
#include <HUpnpAv/HMusicTrack>
#include <HUpnpAv/HPersonWithRole>
#include <HUpnpAv/HCdsDidlLiteSerializer>
#include <HUpnpAv/HMediaInfo>
#include <HUpnpAv/HProtocolInfo>
#include <HUpnpAv/HConnectionInfo>
#include <HUpnpAv/HTransportState>
#include <HUpnpAv/HTransportStatus>
#include <HUpnpAv/HPlayMode>
#include <HUpnpAv/HTransportSettings>
#include <HUpnpAv/HRecordQualityMode>
#include <HUpnpAv/HDuration>
#include "../../external/hupnp/hupnp_av/src/mediarenderer/hconnectionmanager_sinkservice_p.h"
#include <QtCore/QUrl>
#include <QtCore/QTime>
#include <QtCore/QSet>
#include <QtCore/QHashNode>

#include "Gen_Devices/AllCommandsRequests.h"
#include "pluto_main/Define_MediaType.h"
#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_EventParameter.h"
#include "DLNA.h"
#include "DLNACallBack.h"
#include "EntertainArea.h"

using namespace DCE;
using namespace Herqq::Upnp;

LMCERenderer::LMCERenderer(HAbstractConnectionManagerService *cmService, QObject* parent, DLNA* pDLNA) :
	HRendererConnection(parent), m_iMediaType(0)
{

	m_pDLNA = pDLNA;
	m_iStreamID = 0;

	HAbstractMediaRendererDevice* pDevice = dynamic_cast<HAbstractMediaRendererDevice*>(cmService->parentDevice());
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
		return;
	}

	// TODO: Handle different media types... Audio goes to Xine, but pictures should go to the picture player.
	// Squeezeboxes are simpler as they only do audio, but networked_video_players probably does audio,video and picture
	// Load all rooms, entertainment areas and the devices in them
	// need to find out what we can play in our EA
	// load ea - device information,

	HRendererConnectionInfo *info = writableRendererConnectionInfo();
	info->setVolume(HChannel::Master, 50);
	info->setTransportStatus(HTransportStatus::OK);
	// When we start up, assume the DCERouter also has just started, so nothing is playing.
	info->setTransportState(HTransportState::NoMediaPresent);
	info->setPlaybackStorageMedium(HStorageMedium::None);
	info->setNumberOfTracks(0);
	info->setCurrentTrack(0);

	// TODO: hack: implement our own ConnectionManager ?
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
	//piList.append(HProtocolInfo("http-get","*","audio/vnd.dlna.adts","*"));
	//piList.append(HProtocolInfo("http-get","*","audio/x-ms-wma","*"));
	//piList.append(HProtocolInfo("http-get","*","audio/application/ogg","*"));
	piList.append(HProtocolInfo("http-get","*","audio/x-flac","*"));
	//piList.append(HProtocolInfo("http-get","*","audio/*","*"));
	piList.append(HProtocolInfo("http-get","*","video/*","*"));
	piList.append(HProtocolInfo("http-get","*","image/*","*"));

	
		s->setSinkProtocolInfo(piList);

/*	LoggerWrapper::GetInstance ()->Write (LV_CRITICAL, "LMCERenderer() getCapabilitied %s", info->deviceCapabilities().playMedia().count()); */

//	HConnectionInfo cinfo;
//	HAbstractConnectionManagerService *cmService2 = pDevice->connectionManager();
/*	qint32 retval = cmService->getCurrentConnectionInfo(0, &cinfo);
	if (retval == UpnpSuccess) 
	{
		cinfo.setDirection(HConnectionManagerInfo::DirectionInput);
		cinfo.setStatus(HConnectionManagerInfo::StatusOk);

		LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERenderer() HConnectionInfo.isValid() = %d", cinfo.isValid());
		LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERenderer() HConnectionInfo.protocolInfo().isValid() = %d, %s", cinfo.protocolInfo().isValid(), cinfo.protocolInfo().toString().toStdString().c_str());
	}else{
		LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERenderer() FAILED, adding new connection info");
		HProtocolInfo pinfo("http-get:*:*:*");
		HConnectionInfo connectionInfo(0, pinfo);
		//TODO: wrong, not necessarily id=0 on these services if implementing PrepareConnection:
		connectionInfo.setAvTransportId(0);
		connectionInfo.setRcsId(0);

		connectionInfo.setDirection(HConnectionManagerInfo::DirectionInput);
		connectionInfo.setStatus(HConnectionManagerInfo::StatusOk);
		s->addConnection(connectionInfo);
	}
*/
	writableRendererConnectionInfo()->setCurrentPlayMode(HPlayMode::Normal);

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

	EntertainArea* pEntertainArea = m_pDLNA->FindEntertainArea(m_PK_EntArea);
	if (pEntertainArea != NULL) {
		LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERenderer() assigning pRenderer (%d) to m_PK_EntArea = %d(addr: %d)", this, m_PK_EntArea, pEntertainArea);
		pEntertainArea->m_pRenderer = this;
	}
}

LMCERenderer::~LMCERenderer()
{
	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERenderer~() deleting renderer");
	EntertainArea* pEntertainArea = m_pDLNA->FindEntertainArea(m_PK_EntArea);
	if (pEntertainArea != NULL) {
		pEntertainArea->m_pRenderer = NULL;
	}
}

void LMCERenderer::finalizeInit()
{
//	connectionInfo()->setDirection(HConnectionManagerInfo::DirectionInput);
	setConnectionStatus(HConnectionManagerInfo::StatusOk);
	//DID NOT WORK: 	QString format("http-get:*:audio/*:*");
	//	setContentFormat(format);
}

int LMCERenderer::GetPK_EntertainArea() 
{
	return m_PK_EntArea;
}

int LMCERenderer::GetStreamID()
{
	return m_iStreamID;
}

void LMCERenderer::MediaCommandIntercepted(Message *pMessage, long PK_Device)
{
	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERenderer::MediaCommandIntercepted() m_PK_EntArea = %d, this = %d", m_PK_EntArea, this);
	// TODO: FIXME: create signals/slots to avoid threading issues
	if (pMessage->m_dwMessage_Type == MESSAGETYPE_COMMAND && 
	    (pMessage->m_dwID == COMMAND_Start_Streaming_CONST ||
	     pMessage->m_dwID == COMMAND_Play_Media_CONST))
	{
		m_dwPK_Device_Playing = PK_Device;
		string sStreamID = pMessage->m_mapParameters[COMMANDPARAMETER_StreamID_CONST];
		int streamID = atoi(sStreamID.c_str());
		string sMRL = pMessage->m_mapParameters[COMMANDPARAMETER_MediaURL_CONST];
		LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERenderer::MediaCommandIntercepted() Play Media/Start Streaming, streamID = %d", streamID);
		if (streamID > 0) {
			m_iStreamID = streamID;
			m_Url.setUrl(QString(sMRL.c_str()));
			HRendererConnectionInfo *info = writableRendererConnectionInfo();
			info->setCurrentResourceUri(m_Url);
			info->setTransportState(HTransportState::Playing);
			info->setCurrentPlayMode(HPlayMode::Normal);
			// TODO depends on mediatype
			info->setCurrentMediaCategory(HMediaInfo::TrackAware);
			// TODO: set number of tracks from playlist
			// TODO: check URL/source first
			info->setPlaybackStorageMedium(HStorageMedium::HDD);
			UpdateNowPlaying();
		}

	} else if ((pMessage->m_dwMessage_Type == MESSAGETYPE_COMMAND && pMessage->m_dwID == COMMAND_Stop_Streaming_CONST) ||
		   (pMessage->m_dwMessage_Type == MESSAGETYPE_COMMAND && pMessage->m_dwID == COMMAND_Stop_Media_CONST))
	{
		string sStreamID = pMessage->m_mapParameters[COMMANDPARAMETER_StreamID_CONST];
		int streamID = atoi(sStreamID.c_str());
		LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERenderer::MediaCommandIntercepted() Playback Completed/Stop Media/Stop Streaming, streamID = %d", streamID);
		if (streamID > 0 && m_iStreamID == streamID) {
			PlayBackCompleted();
		}
	} else if ((pMessage->m_dwMessage_Type == MESSAGETYPE_EVENT && pMessage->m_dwID == EVENT_Media_Position_Changed_CONST) ||
		   (pMessage->m_dwMessage_Type == MESSAGETYPE_EVENT && pMessage->m_dwID == EVENT_Playback_Completed_CONST))
	{
		string sStreamID = pMessage->m_mapParameters[EVENTPARAMETER_Stream_ID_CONST];
		int streamID = atoi(sStreamID.c_str());
		string sMRL = pMessage->m_mapParameters[EVENTPARAMETER_MRL_CONST];

		LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERenderer::MediaCommandIntercepted() Playback related Event, streamID = %d", streamID);
		if (streamID > 0 && m_iStreamID == streamID) {
			// Make sure that the playback completed event is related to the current URL, if not ignore it
			// (we sometimes can get the playback completed event from the previous file after the next file already has started playing)
			if (pMessage->m_dwID == EVENT_Playback_Completed_CONST && m_Url.toString() == sMRL.c_str())
				PlayBackCompleted();
			else {
				int MediaType = atoi(pMessage->m_mapParameters[EVENTPARAMETER_FK_MediaType_CONST].c_str());
				string currentTime = pMessage->m_mapParameters[EVENTPARAMETER_Current_Time_CONST];
				string totalTime = pMessage->m_mapParameters[EVENTPARAMETER_DateTime_CONST];
				string mrl = pMessage->m_mapParameters[EVENTPARAMETER_MRL_CONST];
				int Speed = atoi(pMessage->m_mapParameters[EVENTPARAMETER_Speed_CONST].c_str());
				string mediaID = pMessage->m_mapParameters[EVENTPARAMETER_ID_CONST];

				MediaPositionChanged(MediaType, currentTime, totalTime, mrl, mediaID, Speed);
			}
		}
	}
}

void LMCERenderer::MediaPositionChanged(int MediaType, string currentTime, string totalTime, string mrl, string mediaID, int speed)
{
	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERenderer::MediaPositionChanged() currentTime = %s", currentTime.c_str());
	HRendererConnectionInfo *info = writableRendererConnectionInfo();
	writableRendererConnectionInfo()->setRelativeTimePosition(HDuration(QString(currentTime.c_str())));
	writableRendererConnectionInfo()->setAbsoluteTimePosition(HDuration(QString(currentTime.c_str())));
	writableRendererConnectionInfo()->setCurrentTrackDuration(HDuration(QString(totalTime.c_str())));
	// TODO: media = all tracks, track = this track
	writableRendererConnectionInfo()->setCurrentMediaDuration(HDuration(QString(totalTime.c_str())));
	// TODO: use UPnP accessible URL (http-something)
	writableRendererConnectionInfo()->setCurrentTrackUri(QString(mrl.c_str()));
	info->setTransportPlaySpeed(QString(StringUtils::itos(speed/1000).c_str()));
}

void LMCERenderer::UpdateNowPlaying()
{	
	HItem *item = NULL;
	// Get data for current media from media plugin
	string attributes;
	map<string, string> attrMap;
	CMD_Get_Attributes_For_Media_DT CMD_Get_Attributes_For_Media_DT(m_pDLNA->m_dwPK_Device, DEVICETEMPLATE_Media_Plugin_CONST, BL_SameHouse, "", StringUtils::itos(m_PK_EntArea), &attributes);
	if (m_pDLNA->SendCommand(CMD_Get_Attributes_For_Media_DT)) {
		//A tab delimited list of attributes: Attribute type \t Name \t type .
		vector<string> attr;
		StringUtils::Tokenize(attributes, "\t", attr);
		// TODO: will not work if we get multiple entries of the same key (several ARTIST for instance)
		for (int i = 0; i < attr.size(); i = i + 2) {
			LoggerWrapper::GetInstance ()->Write (LV_WARNING, "LMCERenderer::UpdateNowPlaying() attribute: %s : %s", attr[i].c_str(), attr[i+1].c_str());			
			attrMap.insert(pair<string,string>(attr[i], attr[i+1]));
		}

		HMusicTrack *mitem = new HMusicTrack(QString(attrMap["TITLE"].c_str()), QString(""));
		QList<HPersonWithRole> artists;
		artists.append(HPersonWithRole(""));
		mitem->setArtists(artists);
		mitem->setAlbums(QStringList(attrMap["ALBUM"].c_str()));
		item = mitem;
	}

	// Set info in renderer
	if (item != NULL) {
		HCdsDidlLiteSerializer serializer;
		QString xml = serializer.serializeToXml(*item);
		writableRendererConnectionInfo()->setCurrentResourceMetadata(xml);
		writableRendererConnectionInfo()->setCurrentTrackMetadata(xml);
	}
	delete item;
}

void LMCERenderer::PlayBackCompleted()
{
	m_iStreamID = 0;
	m_dwPK_Device_Playing = 0;
	writableRendererConnectionInfo()->setTransportState(HTransportState::NoMediaPresent);
	writableRendererConnectionInfo()->setPlaybackStorageMedium(HStorageMedium::None);
	writableRendererConnectionInfo()->setNumberOfTracks(0);
	writableRendererConnectionInfo()->setCurrentTrack(0);
	writableRendererConnectionInfo()->setCurrentTrackDuration(HDuration());
	writableRendererConnectionInfo()->setCurrentMediaDuration(HDuration());
	writableRendererConnectionInfo()->setCurrentMediaCategory(HMediaInfo::NoMedia);
	writableRendererConnectionInfo()->setCurrentTrackUri(QUrl());
	writableRendererConnectionInfo()->setCurrentTrackMetadata(QString());
	writableRendererConnectionInfo()->setCurrentResourceUri(QUrl());
	writableRendererConnectionInfo()->setCurrentResourceMetadata(QString());
	writableRendererConnectionInfo()->setRelativeTimePosition(HDuration());
	m_Time = QTime();
	m_Url.clear();
}

int LMCERenderer::GetMediaType(HObject *metadata, QUrl& url)
{
	int mediaType = 0;
	if (metadata != NULL)
	{
		if (metadata->type() == HObject::AudioItem || metadata->type() == HObject::MusicTrack) {
			LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERenderer::GetMediaType() metadata->title = %s", metadata->title().toStdString().c_str());
			mediaType = MEDIATYPE_lmce_StreamedAudio_CONST;
		} else if (metadata->type() == HObject::ImageItem) {
			mediaType = MEDIATYPE_pluto_Pictures_CONST;
		} else if (metadata->type() == HObject::VideoItem) {
			mediaType = MEDIATYPE_lmce_StreamedVideo_CONST;
		}
		LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERenderer::GetMediaType() metadata->type = %d, mediaType = %d", metadata->type(), mediaType);
	} else {
		LoggerWrapper::GetInstance ()->Write (LV_WARNING, "LMCERenderer::GetMediaType() no metadata provided by media server");
		// If the upnp media server does not provide metadata, guess media type from url extension
		if (StringUtils::EndsWith(url.toString().toStdString(), "flac") ||
		    StringUtils::EndsWith(url.toString().toStdString(), "flc") ||
		    StringUtils::EndsWith(url.toString().toStdString(), "mp3") ||
            StringUtils::EndsWith(url.toString().toStdString(), "ogg") ||
            StringUtils::EndsWith(url.toString().toStdString(), "wav"))
		{
			mediaType = MEDIATYPE_lmce_StreamedAudio_CONST;
        } else if (StringUtils::EndsWith(url.toString().toStdString(), "jpg"))
        {
            mediaType = MEDIATYPE_pluto_Pictures_CONST;
        } else if (StringUtils::EndsWith(url.toString().toStdString(), "mpg"))
        {
            mediaType = MEDIATYPE_lmce_StreamedVideo_CONST;
        }
	}
	return mediaType;
}

qint32 LMCERenderer::doPlay(const QString& speed)
{
	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERenderer::doPlay() start, speed = %s", speed.toStdString().c_str());
    if (m_iStreamID > 0)
    {
        LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERenderer::doPlay() with existing stream, performing resume play instead");
        return doPause();
    } else {
        LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERenderer::doPlay() start, m_Url = %s", m_Url.toString().toStdString().c_str());
        if (m_iMediaType > 0)
        {
            CMD_MH_Play_Media_DT CMD_MH_Play_Media_DT(m_pDLNA->m_dwPK_Device, DEVICETEMPLATE_Media_Plugin_CONST, BL_SameHouse,
                                  0,m_Url.toString().toStdString(),m_iMediaType,
                                  0,StringUtils::itos(m_PK_EntArea),false,0,false,false,false);
            if (m_pDLNA->SendCommand(CMD_MH_Play_Media_DT)) {
                writableRendererConnectionInfo()->setTransportState(HTransportState::Transitioning);

                // TODO not all media are track-aware
                writableRendererConnectionInfo()->setCurrentMediaCategory(HMediaInfo::TrackAware);

                LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERenderer::doPlay() successful");
                return UpnpSuccess;
            }
        }
        LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERenderer::doPlay() failed! no media type set.");
    }
    return UpnpActionFailed;
}

qint32 LMCERenderer::doStop()
{
	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERenderer::doStop() start");
	CMD_MH_Stop_Media_DT CMD_MH_Stop_Media_DT(m_pDLNA->m_dwPK_Device, DEVICETEMPLATE_Media_Plugin_CONST,  BL_SameHouse,
						  0,0,0,StringUtils::itos(m_PK_EntArea),false);
	if (m_pDLNA->SendCommand(CMD_MH_Stop_Media_DT)) {
		writableRendererConnectionInfo()->setTransportState(HTransportState::Stopped);

		LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERenderer::doStop() successful");
		return UpnpSuccess;
	}
}

qint32 LMCERenderer::doPause()
{
	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERenderer::doPause() start m_PK_EntArea = %d", m_PK_EntArea);
	if (m_iStreamID > 0)
	{
        CMD_Pause_Media_DT CMD_Pause_Media(m_pDLNA->m_dwPK_Device, DEVICETEMPLATE_Media_Plugin_CONST, BL_SameHouse, m_iStreamID);
        CMD_Pause_Media.m_pMessage->m_mapParameters[COMMANDPARAMETER_PK_EntertainArea_CONST] = StringUtils::itos(m_PK_EntArea);
        if (m_pDLNA->SendCommand(CMD_Pause_Media))
		{
			LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERenderer::doPause() successful");
			writableRendererConnectionInfo()->setTransportState(HTransportState::PausedPlayback);
			return UpnpSuccess;
		}
	}
	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERenderer::doPause() failed, no streamID");
	return UpnpActionFailed;
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
	CMD_Jump_Position_In_Playlist_DT CMD_Jump_Position_In_Playlist_DT(m_pDLNA->m_dwPK_Device, DEVICETEMPLATE_Media_Plugin_CONST,  BL_SameHouse, "+1", m_iStreamID);
	if (m_pDLNA->SendCommand(CMD_Jump_Position_In_Playlist_DT))
	{
		writableRendererConnectionInfo()->setTransportState(HTransportState::Transitioning);
		return UpnpSuccess;
	}
	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERenderer::doNext() Failed!");
}

qint32 LMCERenderer::doPrevious()
{
	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERenderer::doPrevious() start");
	CMD_Jump_Position_In_Playlist_DT CMD_Jump_Position_In_Playlist_DT(m_pDLNA->m_dwPK_Device, DEVICETEMPLATE_Media_Plugin_CONST,  BL_SameHouse, "-1", m_iStreamID);
	if (m_pDLNA->SendCommand(CMD_Jump_Position_In_Playlist_DT))
	{
		writableRendererConnectionInfo()->setTransportState(HTransportState::Transitioning);
		return UpnpSuccess;
	}
	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERenderer::doPrevious() Failed!");
}

qint32 LMCERenderer::doSetResource ( const QUrl &  resourceUri, HObject *  cdsMetadata)
{
	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERenderer::doSetResource() start: resourceUri = %s, cdsMetadata = %d", resourceUri.toString().toStdString().c_str(), cdsMetadata);
	m_Url = resourceUri;
	m_iMediaType = GetMediaType(cdsMetadata, m_Url);
	
	if (rendererConnectionInfo()->transportState() == HTransportState::NoMediaPresent)
		writableRendererConnectionInfo()->setTransportState(HTransportState::Stopped);
	m_Time = QTime();
	writableRendererConnectionInfo()->setRelativeTimePosition(HDuration());
	// TODO: fetch real number from media plugin
	writableRendererConnectionInfo()->setNumberOfTracks(1);
	writableRendererConnectionInfo()->setPlaybackStorageMedium(HStorageMedium::Network);

	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "LMCERenderer::doSetResource() end, mediatype = %d", m_iMediaType);
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

void LMCERenderer::AlarmCallback(int id, void* param)
{
}

