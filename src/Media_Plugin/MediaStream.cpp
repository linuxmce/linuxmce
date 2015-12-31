/*
 MediaHandlerInfo

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com

 Phone: +1 (877) 758-8648

 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */

#include "PlutoUtils/CommonIncludes.h"
#include "MediaHandlerInfo.h"
#include "MediaHandlerBase.h"
#include "EntertainArea.h"
#include "Media_Plugin.h"

#include "DCERouter.h"
#include "DeviceData_Router.h"
#include "Orbiter_Plugin/Orbiter_Plugin.h"
#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_MediaType.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "DataGrid.h"

#include "pluto_media/Database_pluto_media.h"
#include "pluto_media/Table_Attribute.h"
#include "pluto_media/Define_AttributeType.h"
#include "pluto_media/Table_Attribute_Settings.h"
#include "pluto_media/Table_Attribute.h"
#include "pluto_media/Table_Bookmark.h"
#include "pluto_media/Table_File_Attribute.h"
#include "pluto_media/Table_File.h"
#include "pluto_media/Table_Disc.h"
#include "pluto_media/Table_DiscLocation.h"
#include "pluto_media/Table_Picture_Disc.h"
#include "pluto_media/Table_Disc_Attribute.h"
#include "pluto_media/Table_Playlist.h"
#include "pluto_media/Table_Picture.h"
#include "pluto_media/Table_Picture_File.h"
#include "pluto_media/Table_Picture_Attribute.h"
#include "pluto_media/Table_AttributeType.h"
#include "pluto_media/Table_MediaType_AttributeType.h"
#include "pluto_media/Table_MediaProvider.h"
#include "pluto_media/Table_ProviderSource.h"
#include "pluto_media/Table_LongAttribute.h"
#include "pluto_media/Table_RipStatus.h"
#include "pluto_main/Table_MediaType_DesignObj.h"
#include "pluto_main/Table_DeviceTemplate_MediaType_DesignObj.h"

RemoteControlSet::RemoteControlSet(Row_MediaType_DesignObj *pRow_MediaType_DesignObj)
{
	m_iPK_Screen_Remote=pRow_MediaType_DesignObj->FK_Screen_get();
	m_iPK_DesignObj_Remote_Popup=pRow_MediaType_DesignObj->FK_DesignObj_Popup_get();
	m_iPK_Screen_FileList=pRow_MediaType_DesignObj->FK_Screen_FileList_get();
    m_iPK_Screen_OSD=pRow_MediaType_DesignObj->FK_Screen_OSD_get();
	m_iPK_Screen_Alt_Remote=pRow_MediaType_DesignObj->FK_Screen_Alt_get();
	m_iPK_Screen_Alt_OSD=pRow_MediaType_DesignObj->FK_Screen_Alt_OSD_get();
	m_iPK_Screen_OSD_Speed=pRow_MediaType_DesignObj->FK_Screen_OSD_Speed_get();
	m_iPK_Screen_OSD_Track=pRow_MediaType_DesignObj->FK_Screen_OSD_Track_get();
}

RemoteControlSet::RemoteControlSet(Row_DeviceTemplate_MediaType_DesignObj *pRow_DeviceTemplate_MediaType_DesignObj)
{
	m_iPK_Screen_Remote=pRow_DeviceTemplate_MediaType_DesignObj->FK_Screen_get();
	m_iPK_DesignObj_Remote_Popup=pRow_DeviceTemplate_MediaType_DesignObj->FK_DesignObj_Popup_get();
	m_iPK_Screen_FileList=pRow_DeviceTemplate_MediaType_DesignObj->FK_Screen_FileList_get();
    m_iPK_Screen_OSD=pRow_DeviceTemplate_MediaType_DesignObj->FK_Screen_OSD_get();
	m_iPK_Screen_Alt_Remote=pRow_DeviceTemplate_MediaType_DesignObj->FK_Screen_Alt_get();
	m_iPK_Screen_Alt_OSD=pRow_DeviceTemplate_MediaType_DesignObj->FK_Screen_Alt_OSD_get();
	m_iPK_Screen_OSD_Speed=pRow_DeviceTemplate_MediaType_DesignObj->FK_Screen_OSD_Speed_get();
	m_iPK_Screen_OSD_Track=pRow_DeviceTemplate_MediaType_DesignObj->FK_Screen_OSD_Track_get();
}

MediaStream::MediaStream( class MediaHandlerInfo *pMediaHandlerInfo, int iPK_MediaProvider, MediaDevice *pMediaDevice, int PK_Users, enum SourceType sourceType, int iStreamID )
{
	m_dwPK_Device_Remote=0;
	m_dwPK_Disc=m_discid=0;
	m_iTrackOrSectionOrChannel=-1;
    m_iStreamID = iStreamID;
    m_pMediaHandlerInfo=pMediaHandlerInfo;
	m_IdentifiedPriority=0;
	m_iPK_MediaProvider=iPK_MediaProvider;
	m_bPlugInWillSetDescription=false;
	m_bIdentifiedDisc=false;
	m_bContainsTitlesOrSections=true;

    m_iPK_Users=PK_Users;
    m_eSourceType=sourceType;
    m_bResume=m_bPlaying=false;
    m_pOH_Orbiter_StartedMedia=NULL;
    m_pPictureData=NULL;
    m_iPictureSize=0;
    m_iOrder=0;
    m_iDequeMediaFile_Pos=0; m_iDequeMediaTitle_Pos=m_iDequeMediaSection_Pos=-1;

	m_iPK_Playlist=0;
    m_sPlaylistName="";
	m_bIsMovable = true; // by default all the media can move but not always.
	m_tTime = time(NULL);
	m_tTime_Parked = 0;
	m_iPK_MediaType = pMediaHandlerInfo->m_PK_MediaType;
	m_iRepeat = 0;
	m_bStopped = false;
	m_bUseAltScreens = false;
	m_bPlaybackStarted = false;
    m_pMediaDevice_Source=pMediaDevice;
	m_pDevice_CaptureCard=NULL;

    if ( m_pMediaHandlerInfo ) // If this stream is a "valid stream only"
        m_pMediaHandlerInfo->m_pMediaHandlerBase->m_pMedia_Plugin->m_mapMediaStream[m_iStreamID] = this;

    if( !m_pMediaDevice_Source || !m_pMediaHandlerInfo )
        LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Media stream is invalid because of NULL pointers! %p %p", m_pMediaDevice_Source, m_pMediaHandlerInfo);

	m_pMediaDevice_Source->m_iLastPlaybackSpeed = 1000;
LoggerWrapper::GetInstance()->Write( LV_STATUS, "MediaStream::MediaStream %p on stream id: %d type %d", this, m_iStreamID, m_iPK_MediaType );
LoggerWrapper::GetInstance()->Write( LV_STATUS, "Mediastream mapEntArea size %d", m_mapEntertainArea.size( ) );
LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"debug_stream_end MediaStream::MediaStream c1 %d/%p source %d",
m_iStreamID,this,pMediaDevice->m_pDeviceData_Router->m_dwPK_Device);
}

bool MediaStream::isMovable()
{
	return m_bIsMovable;
}

void MediaStream::setIsMovable(bool bIsMovable)
{
	m_bIsMovable = bIsMovable;
}

void MediaStream::SetPlaylistPosition(int position)
{
LoggerWrapper::GetInstance()->Write(LV_STATUS,"SetPlaylistPosition with position %d dequepos %d size %d",
	position,m_iDequeMediaFile_Pos,(int) m_dequeMediaFile.size());

	if ( (unsigned int)position == m_iDequeMediaFile_Pos )
		return;

	if ( position < 0 )
		m_iDequeMediaFile_Pos = m_dequeMediaFile.size() - 1;
	else
		m_iDequeMediaFile_Pos = position;

    if ( m_iDequeMediaFile_Pos >= m_dequeMediaFile.size() )
        m_iDequeMediaFile_Pos = 0;
}


void MediaStream::ChangePositionInPlaylist(int iHowMuch)
{
LoggerWrapper::GetInstance()->Write(LV_STATUS,"ChangePositionInPlaylist with position %d how much %d",
	m_iDequeMediaFile_Pos,iHowMuch);

	SetPlaylistPosition(m_iDequeMediaFile_Pos + iHowMuch);
}

string MediaStream::GetFilenameToPlay(string sNoFilesFileName )
{
LoggerWrapper::GetInstance()->Write(LV_STATUS,"GetFilenameToPlay called with size: %d pos: %d",
	(int) m_dequeMediaFile.size(), m_iDequeMediaFile_Pos);

    if ( m_dequeMediaFile.size() == 0 )
        return sNoFilesFileName;

    // ensure the playlist position is accurate
    SetPlaylistPosition(m_iDequeMediaFile_Pos);

    return GetCurrentMediaFile()->FullyQualifiedFile();
}

void MediaStream::DumpPlaylist()
{
    deque<MediaFile *>::iterator itPlaylist;

    itPlaylist = m_dequeMediaFile.begin();
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Position: %d", m_iDequeMediaFile_Pos);
    while ( itPlaylist != m_dequeMediaFile.end() )
    {
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "File%c%d: %s",
                    (m_iDequeMediaFile_Pos == (unsigned int)(itPlaylist - m_dequeMediaFile.begin())) ? '*' : ' ',
                    itPlaylist - m_dequeMediaFile.begin(),
                    (*itPlaylist)->FullyQualifiedFile().c_str());
        itPlaylist++;
    }
}

void MediaStream::ClearPlaylist()
{
    m_iDequeMediaFile_Pos = 0;
    MediaAttributes_LowLevel::PurgeDequeMediaFile(m_dequeMediaFile);
    m_dequeMediaFile.clear();
}

bool MediaStream::CanPlayMore()
{
    LoggerWrapper::GetInstance()->Write(LV_WARNING, "HaveMoreInQueue: position %d, size: %d, result %d", m_iDequeMediaFile_Pos, m_dequeMediaFile.size(), m_iDequeMediaFile_Pos < (m_dequeMediaFile.size() - 1));
    DumpPlaylist();

    return m_iDequeMediaFile_Pos < (m_dequeMediaFile.size() - 1);
}

MediaStream::~MediaStream( )
{
LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"debug_stream_end MediaStream::~MediaStream c1 %d/%p",
m_iStreamID,this);


	LoggerWrapper::GetInstance()->Write(LV_STATUS,"MediaStream::~MediaStream %p %d mhi %p",this,m_iStreamID,m_pMediaHandlerInfo); 
	for(size_t s=0;s<m_dequeMediaSection.size();++s)
		if( m_dequeMediaSection[s] )
			delete m_dequeMediaSection[s];

	for(size_t s=0;s<m_dequeMediaTitle.size();++s)
		if( m_dequeMediaTitle[s] )
			delete m_dequeMediaTitle[s];

	for(deque<MediaFile *>::iterator it = m_dequeMediaFile.begin(); it != m_dequeMediaFile.end(); ++it)
		delete *it;
	m_dequeMediaFile.clear();

	if ( m_pMediaHandlerInfo )
	{
		size_t sizebefore = m_pMediaHandlerInfo->m_pMediaHandlerBase->m_pMedia_Plugin->m_mapMediaStream.size();
        m_pMediaHandlerInfo->m_pMediaHandlerBase->m_pMedia_Plugin->m_mapMediaStream_Remove( m_iStreamID );
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"MediaStream::~MediaStream size %d - %d",(int) sizebefore,(int) m_pMediaHandlerInfo->m_pMediaHandlerBase->m_pMedia_Plugin->m_mapMediaStream.size());
	}
    ClearPlaylist();

	for(map<int,class OldStreamInfo *>::iterator it = m_mapOldStreamInfo.begin(); it != m_mapOldStreamInfo.end(); ++it)
		delete it->second;
	m_mapOldStreamInfo.clear();

	delete[] m_pPictureData;
	m_pPictureData = NULL;
}

bool MediaStream::ProcessJumpPosition(string sJumpSpecification)
{
    switch ( sJumpSpecification[0] )
    {
        case '-':
            ChangePositionInPlaylist(-atoi(sJumpSpecification.substr(1).c_str()));
            break;
        case '+':
            ChangePositionInPlaylist(atoi(sJumpSpecification.substr(1).c_str()));
            break;
        default:
            SetPlaylistPosition(atoi(sJumpSpecification.c_str()));
            break;
    }

	return true;
}

void MediaStream::DeleteEntryFromPlaylist(int position)
{
	if ( position >= 0 && position < m_dequeMediaFile.size() )
		m_dequeMediaFile.erase(m_dequeMediaFile.begin() + position);

	DumpPlaylist();
}

void MediaStream::MoveEntryInPlaylist(int position, int displacement)
{
	if ( position >= 0 && position < m_dequeMediaFile.size() &&
		 position + displacement >= 0 && position + displacement < m_dequeMediaFile.size() )
	{
		MediaFile *pTargetFile = m_dequeMediaFile[position];
		m_dequeMediaFile[position] = m_dequeMediaFile[position + displacement];
		m_dequeMediaFile[position + displacement ] = pTargetFile;

		if ( m_iDequeMediaFile_Pos == position )
			m_iDequeMediaFile_Pos += displacement;
	}

	DumpPlaylist();
}

bool MediaStream::ContainsVideo()
{
	return m_iPK_MediaType==MEDIATYPE_pluto_LiveTV_CONST ||
		m_iPK_MediaType==MEDIATYPE_pluto_DVD_CONST ||
		m_iPK_MediaType==MEDIATYPE_pluto_StoredVideo_CONST ||
		m_iPK_MediaType==MEDIATYPE_pluto_Pictures_CONST ||
		m_iPK_MediaType==MEDIATYPE_np_LiveTV_CONST ||
		m_iPK_MediaType==MEDIATYPE_np_DVD_CONST ||
		m_iPK_MediaType==MEDIATYPE_np_VideoTape_CONST ||
		m_iPK_MediaType==MEDIATYPE_np_LaserDisc_CONST ||
		m_iPK_MediaType==MEDIATYPE_np_Game_CONST ||
		m_iPK_MediaType==MEDIATYPE_np_BluRay_CONST ||
		m_iPK_MediaType==MEDIATYPE_pluto_HDDVD_CONST ||
		m_iPK_MediaType==MEDIATYPE_pluto_BD_CONST ||
		m_iPK_MediaType==MEDIATYPE_np_NetworkMedia_CONST ||
		m_iPK_MediaType==MEDIATYPE_np_XBOX360_CONST ||
		m_iPK_MediaType==MEDIATYPE_np_PVR_CONST ||
		m_iPK_MediaType==MEDIATYPE_np_Playstation_CONST;
}


bool MediaStream::OrbiterIsOSD(int PK_Orbiter,EntertainArea **ppEntertainArea)
{
	for(map<int, class EntertainArea *>::iterator it=m_mapEntertainArea.begin();it!=m_mapEntertainArea.end();++it)
	{
		EntertainArea *pEntertainArea = it->second;
		if( pEntertainArea->m_pOH_Orbiter_OSD && pEntertainArea->m_pOH_Orbiter_OSD->m_pDeviceData_Router->m_dwPK_Device==PK_Orbiter )
		{
			if( ppEntertainArea )
				(*ppEntertainArea) = pEntertainArea;
			return true;
		}
	}
	return false;
}

string MediaStream::GetAllOSD()
{
	string s;
	for( map<int, class EntertainArea *>::iterator it=m_mapEntertainArea.begin();it!=m_mapEntertainArea.end();++it)
	{
		EntertainArea *pEntertainArea = it->second;
		if( pEntertainArea->m_pMediaDevice_ActiveDest && 
			pEntertainArea->m_pMediaDevice_ActiveDest->m_pOH_Orbiter_OSD )
		{
			if( s.size() )
				s += ",";
			s += StringUtils::itos(pEntertainArea->m_pMediaDevice_ActiveDest->m_pOH_Orbiter_OSD->m_pDeviceData_Router->m_dwPK_Device);
		}
	}
	return s;
}

void MediaStream::GetRenderDevices(map<int, MediaDevice *> *pmapMediaDevices)
{
	for(map<int, class EntertainArea *>::iterator it=m_mapEntertainArea.begin();it!=m_mapEntertainArea.end();++it)
	{
		EntertainArea *pEntertainArea = it->second;
		if( pEntertainArea->m_pMediaDevice_ActiveDest )
			(*pmapMediaDevices)[ pEntertainArea->m_pMediaDevice_ActiveDest->m_pDeviceData_Router->m_dwPK_Device ] = pEntertainArea->m_pMediaDevice_ActiveDest;
	}
}

void MediaStream::UpdateDescriptions(bool bAllFiles,MediaFile *pMediaFile_In)
{

	if( m_bPlugInWillSetDescription )
		return;

	//TODO refactor this code: we need to use LiveRadio media type instead and set m_bPlugInWillSetDescription=true
	//HACK if description starts from "INTERNET_RADIO:", cut the prefix and return
	if ( StringUtils::StartsWith(m_sMediaDescription, "INTERNET_RADIO:") )
	{
	    StringUtils::Replace(&m_sMediaDescription, "INTERNET_RADIO:", "");
	    return;
	}

	m_sMediaDescription=""; m_sSectionDescription="";
	Media_Plugin *pMedia_Plugin = m_pMediaHandlerInfo->m_pMediaHandlerBase->m_pMedia_Plugin;

	if( m_iPK_MediaType==MEDIATYPE_pluto_StoredAudio_CONST || m_iPK_MediaType==MEDIATYPE_pluto_CD_CONST || (m_iPK_MediaType==MEDIATYPE_pluto_StoredVideo_CONST && !m_bContainsTitlesOrSections) )
	{
		MediaFile *pMediaFile=pMediaFile_In;
		if( bAllFiles )
		{
			for(size_t s=0;s<m_dequeMediaFile.size();++s)
				UpdateDescriptions(false,m_dequeMediaFile[s]);
			return;
		}
		else if( !pMediaFile )
			pMediaFile = GetCurrentMediaFile();

		Row_Attribute *pRow_Attribute_Album=NULL,*pRow_Attribute_Performer=NULL,*pRow_Attribute_Title=NULL,*pRow_Attribute_Episode=NULL;

		// First try to find attributes for the particular song, otherwise look in the collection
		if( pMediaFile )
		{
			if( !pRow_Attribute_Performer )
			{
				list_int *listPK_Attribute = pMediaFile->m_mapPK_Attribute_Find(ATTRIBUTETYPE_Performer_CONST);
				if( listPK_Attribute && listPK_Attribute->size() )
					pRow_Attribute_Performer = pMedia_Plugin->m_pDatabase_pluto_media->Attribute_get()->GetRow(*(listPK_Attribute->begin()));
			}
			if( !pRow_Attribute_Album )
			{
				list_int *listPK_Attribute = pMediaFile->m_mapPK_Attribute_Find(ATTRIBUTETYPE_Album_CONST);
				if( listPK_Attribute && listPK_Attribute->size() )
					pRow_Attribute_Album = pMedia_Plugin->m_pDatabase_pluto_media->Attribute_get()->GetRow(*(listPK_Attribute->begin()));
			}
			if( !pRow_Attribute_Title )
			{
				list_int *listPK_Attribute = pMediaFile->m_mapPK_Attribute_Find(ATTRIBUTETYPE_Title_CONST);
				if( listPK_Attribute && listPK_Attribute->size() )
					pRow_Attribute_Title = pMedia_Plugin->m_pDatabase_pluto_media->Attribute_get()->GetRow(*(listPK_Attribute->begin()));
			}

			if( !pRow_Attribute_Episode )
			{
				list_int *listPK_Attribute = pMediaFile->m_mapPK_Attribute_Find(ATTRIBUTETYPE_Episode_CONST);
				if( listPK_Attribute && listPK_Attribute->size() )
					pRow_Attribute_Episode = pMedia_Plugin->m_pDatabase_pluto_media->Attribute_get()->GetRow(*(listPK_Attribute->begin()));
			}

		}

		if( !pRow_Attribute_Performer )
		{
			list_int *listPK_Attribute = m_mapPK_Attribute_Find(ATTRIBUTETYPE_Performer_CONST);
			if( listPK_Attribute && listPK_Attribute->size() )
				pRow_Attribute_Performer = pMedia_Plugin->m_pDatabase_pluto_media->Attribute_get()->GetRow(*(listPK_Attribute->begin()));
		}
		if( !pRow_Attribute_Album )
		{
			list_int *listPK_Attribute = m_mapPK_Attribute_Find(ATTRIBUTETYPE_Album_CONST);
			if( listPK_Attribute && listPK_Attribute->size() )
				pRow_Attribute_Album = pMedia_Plugin->m_pDatabase_pluto_media->Attribute_get()->GetRow(*(listPK_Attribute->begin()));
		}
		if( !pRow_Attribute_Title )
		{
			list_int *listPK_Attribute = m_mapPK_Attribute_Find(ATTRIBUTETYPE_Title_CONST);
			if( listPK_Attribute && listPK_Attribute->size() )
				pRow_Attribute_Title = pMedia_Plugin->m_pDatabase_pluto_media->Attribute_get()->GetRow(*(listPK_Attribute->begin()));
		}
		if( !pRow_Attribute_Episode )
		{
			list_int *listPK_Attribute = m_mapPK_Attribute_Find(ATTRIBUTETYPE_Episode_CONST);
			if( listPK_Attribute && listPK_Attribute->size() )
				pRow_Attribute_Title = pMedia_Plugin->m_pDatabase_pluto_media->Attribute_get()->GetRow(*(listPK_Attribute->begin()));
		}
		
		if( (pRow_Attribute_Performer || pRow_Attribute_Album) && m_iPK_MediaType!=MEDIATYPE_pluto_StoredVideo_CONST )
		{
			if( pRow_Attribute_Performer )
			{
				m_sMediaDescription = pRow_Attribute_Performer->Name_get();
				if( pRow_Attribute_Album )
					m_sMediaDescription += "\n" + pRow_Attribute_Album->Name_get();
			}
			else
				m_sMediaDescription = pRow_Attribute_Album->Name_get();

			if( pRow_Attribute_Title )
				m_sSectionDescription = pRow_Attribute_Title->Name_get();
			else
				m_sSectionDescription = pMediaFile->m_sFilename;

			if( pMediaFile )
				pMediaFile->m_sDescription = m_sSectionDescription;

                       if( pMediaFile )
                        {
                                vector<Row_LongAttribute *> vectRow_LongAttribute;
                                pMedia_Plugin->m_pDatabase_pluto_media->LongAttribute_get()->GetRows("FK_File=" + StringUtils::itos(pMediaFile->m_dwPK_File) + " AND FK_AttributeType=" TOSTRING(ATTRIBUTETYPE_Synopsis_CONST),&vectRow_LongAttribute);
                                if( vectRow_LongAttribute.size() )
                                        {
                                                string sSynopsis = vectRow_LongAttribute[0]->Text_get();
                                                StringUtils::Replace(&sSynopsis,"\t","");
                                                m_sMediaSynopsis = sSynopsis;

                                        }
                        }

		}
		else if( pRow_Attribute_Title )
				m_sMediaDescription = pRow_Attribute_Title->Name_get();
		else if( pMediaFile && pMediaFile->m_sFilename.size()>6 && pMediaFile->m_sFilename.substr(0,6)=="cdda:/" )
			m_sMediaDescription = pMediaFile->m_sFilename.substr(6);
		else if( pMediaFile )
		{
			m_sMediaDescription = pMediaFile->m_sFilename;
		}

		if( pMediaFile && pMediaFile->m_sDescription.size()==0 )
			pMediaFile->m_sDescription = pMediaFile->m_sFilename;

		if( m_sMediaDescription.size()==0 )
		{
			if( m_iPK_MediaType==MEDIATYPE_pluto_CD_CONST || !pMediaFile || pMediaFile->m_sFilename.size()==0 )
				m_sMediaDescription = "<%=T" + StringUtils::itos(TEXT_Unknown_Disc_CONST) + "%>";
			else
				m_sMediaDescription = pMediaFile->m_sFilename;
		}

		// Let's see if putting it down here makes a dent. 
		if( (pRow_Attribute_Title) && m_iPK_MediaType==MEDIATYPE_pluto_StoredVideo_CONST )
		{
			m_sMediaDescription = pRow_Attribute_Title->Name_get();

			if (pRow_Attribute_Episode) {
				m_sSectionDescription = pRow_Attribute_Episode->Name_get();			
			} else {
				m_sSectionDescription = m_sMediaDescription;
			}

		}

	}
	else if( m_iPK_MediaType==MEDIATYPE_pluto_DVD_CONST )
	{
		MediaTitle *pMediaTitle=NULL;
		MediaSection *pMediaSection=NULL;
		if( m_iDequeMediaTitle_Pos>=0 && m_iDequeMediaTitle_Pos<m_dequeMediaTitle.size() )
			pMediaTitle = m_dequeMediaTitle[m_iDequeMediaTitle_Pos];
		if( pMediaTitle && m_iDequeMediaSection_Pos>=0 && m_iDequeMediaSection_Pos<pMediaTitle->m_dequeMediaSection.size() )
			pMediaSection=pMediaTitle->m_dequeMediaSection[m_iDequeMediaSection_Pos];
		else if( m_iDequeMediaSection_Pos>=0 && m_iDequeMediaSection_Pos<m_dequeMediaSection.size() )
			pMediaSection=m_dequeMediaSection[m_iDequeMediaSection_Pos];

		Row_Attribute *pRow_Attribute_Title=NULL,*pRow_Attribute_Chapter=NULL;

		if( !pRow_Attribute_Title && pMediaTitle )
		{
			list_int *listPK_Attribute = pMediaTitle->m_mapPK_Attribute_Find(ATTRIBUTETYPE_Title_CONST);
			if( listPK_Attribute && listPK_Attribute->size() )
				pRow_Attribute_Title = pMedia_Plugin->m_pDatabase_pluto_media->Attribute_get()->GetRow(*(listPK_Attribute->begin()));
		}
		if( !pRow_Attribute_Title )
		{
			list_int *listPK_Attribute = m_mapPK_Attribute_Find(ATTRIBUTETYPE_Title_CONST);
			if( listPK_Attribute && listPK_Attribute->size() )
				pRow_Attribute_Title = pMedia_Plugin->m_pDatabase_pluto_media->Attribute_get()->GetRow(*(listPK_Attribute->begin()));
		}
		if( !pRow_Attribute_Chapter )
		{
			list_int *listPK_Attribute = m_mapPK_Attribute_Find(ATTRIBUTETYPE_Chapter_CONST);
			if( listPK_Attribute && listPK_Attribute->size() )
				pRow_Attribute_Chapter = pMedia_Plugin->m_pDatabase_pluto_media->Attribute_get()->GetRow(*(listPK_Attribute->begin()));
		}

		if( pRow_Attribute_Title )
			m_sMediaDescription = pRow_Attribute_Title->Name_get();
		if( pRow_Attribute_Chapter )
			m_sSectionDescription = pRow_Attribute_Chapter->Name_get();
		else if( m_iDequeMediaTitle_Pos!=-1 && m_iDequeMediaSection_Pos!=-1 )
			m_sSectionDescription =  "Chapter: " + StringUtils::itos(m_iDequeMediaSection_Pos+1) + 
				"\nTitle: " + StringUtils::itos(m_iDequeMediaTitle_Pos+1);
		else
			m_sSectionDescription = "";

		if( m_sMediaDescription.size()==0 )
		{
			MediaFile *pMediaFile=NULL;
			if( !pMediaFile )
				pMediaFile = GetCurrentMediaFile();

			if( pMediaFile && !StringUtils::StartsWith(pMediaFile->m_sFilename,"/dev/") )
				m_sMediaDescription = pMediaFile->m_sFilename;
			else
				m_sMediaDescription = "<%=T" + StringUtils::itos(TEXT_Unknown_Disc_CONST) + "%>";
		}

		{
			MediaFile *pMediaFile=NULL;
			if ( !pMediaFile )
				pMediaFile = GetCurrentMediaFile();
	                if( pMediaFile && (pMediaFile->m_dwPK_File > 0))
	                {
	                        vector<Row_LongAttribute *> vectRow_LongAttribute;
	                        pMedia_Plugin->m_pDatabase_pluto_media->LongAttribute_get()->GetRows("FK_File=" + StringUtils::itos(pMediaFile->m_dwPK_File) + " AND FK_AttributeType=" TOSTRING(ATTRIBUTETYPE_Synopsis_CONST),&vectRow_LongAttribute);
	                        if( vectRow_LongAttribute.size() )
      	                          {
        	                                string sSynopsis = vectRow_LongAttribute[0]->Text_get();
                	                        StringUtils::Replace(&sSynopsis,"\t","");
                        	                m_sMediaSynopsis = sSynopsis;
                                  }
                	}
		}

	}
	else if( m_iPK_MediaType==MEDIATYPE_lmce_StreamedAudio_CONST )
	{
		// Use channel name as description
		MediaFile *pMediaFile=pMediaFile_In;
		if( bAllFiles )
		{
			for(size_t s=0;s<m_dequeMediaFile.size();++s)
				UpdateDescriptions(false,m_dequeMediaFile[s]);
			return;
		}
		else if( !pMediaFile )
			pMediaFile = GetCurrentMediaFile();

		Row_Attribute *pRow_Attribute_Channel=NULL;
		if ( pMediaFile )
		{
			list_int *listPK_Attribute = pMediaFile->m_mapPK_Attribute_Find(ATTRIBUTETYPE_Channel_CONST);
			if( listPK_Attribute && listPK_Attribute->size() )
				pRow_Attribute_Channel = pMedia_Plugin->m_pDatabase_pluto_media->Attribute_get()->GetRow(*(listPK_Attribute->begin()));
			if( pRow_Attribute_Channel != NULL)
			{
				m_sMediaDescription = pRow_Attribute_Channel->Name_get();
				pMediaFile->m_sDescription = pRow_Attribute_Channel->Name_get();
			}
		}
	}

	if( m_sMediaDescription.size()==0 )
	{
		MediaFile *pMediaFile = GetCurrentMediaFile();

		if( pMediaFile )
			m_sMediaDescription = pMediaFile->m_sFilename;
		else
		{
			DeviceData_Router *pDeviceData_Router = m_pMediaDevice_Source->m_pDeviceData_Router;
			if( pDeviceData_Router->m_pDevice_RouteTo )  // this may be an embedded device, if so show the parent's description
				pDeviceData_Router = pDeviceData_Router->m_pDevice_RouteTo;

			Row_MediaType *pRow_MediaType = m_pMediaHandlerInfo->m_pMediaHandlerBase->m_pMedia_Plugin->m_pDatabase_pluto_main->MediaType_get()->GetRow(m_iPK_MediaType);
			m_sMediaDescription = pDeviceData_Router->m_sDescription;
			if( pRow_MediaType )
				m_sMediaDescription += "(" + pRow_MediaType->Description_get() + ")";
		}
	}

	{
		MediaFile *pMediaFile = NULL;
		pMediaFile = GetCurrentMediaFile();
		if( pMediaFile )
		{
			vector<Row_LongAttribute *> vectRow_LongAttribute;
			pMedia_Plugin->m_pDatabase_pluto_media->LongAttribute_get()->GetRows("FK_File=" + StringUtils::itos(pMediaFile->m_dwPK_File) + " AND FK_AttributeType=" TOSTRING(ATTRIBUTETYPE_Synopsis_CONST),&vectRow_LongAttribute);
			if( vectRow_LongAttribute.size() )
			{
				string sSynopsis = vectRow_LongAttribute[0]->Text_get();
				StringUtils::Replace(&sSynopsis,"\t","");
				m_sMediaSynopsis = sSynopsis;
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"BLARG Synopsis: %s",m_sMediaSynopsis.c_str());
                	}

		}
	}
}

string MediaStream::GetEntAreasWithout(EntertainArea *pEntertainArea)
{
	string sResult;
	for(map<int, class EntertainArea *>::iterator it=m_mapEntertainArea.begin();it!=m_mapEntertainArea.end();++it)
	{
		EntertainArea *pEA = it->second;
		if( pEA!=pEntertainArea )
			sResult += StringUtils::itos(pEA->m_iPK_EntertainArea) + ",";
	}

	return sResult;
}

string MediaStream::GetEntAreasWithout(map<int,class EntertainArea *> *p_mapEntertainArea)
{
	string sResult;
	for(map<int, class EntertainArea *>::iterator it=m_mapEntertainArea.begin();it!=m_mapEntertainArea.end();++it)
	{
		EntertainArea *pEA = it->second;
		if( !p_mapEntertainArea || p_mapEntertainArea->find(pEA->m_iPK_EntertainArea)==p_mapEntertainArea->end() )
			sResult += StringUtils::itos(pEA->m_iPK_EntertainArea) + ",";
	}

	return sResult;
}

int MediaStream::GetRemoteControlScreen(int PK_Orbiter)
{
	int PK_MediaType=m_iPK_MediaType;
	if( m_iPK_MediaType==MEDIATYPE_pluto_StoredVideo_CONST && m_bContainsTitlesOrSections )
		PK_MediaType = MEDIATYPE_pluto_DVD_CONST;

	Media_Plugin *pMedia_Plugin = m_pMediaHandlerInfo->m_pMediaHandlerBase->m_pMedia_Plugin;
	RemoteControlSet *pRemoteControlSet = pMedia_Plugin->PickRemoteControlMap(
		PK_Orbiter,
		m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_DeviceTemplate,
		PK_MediaType);
	if( !pRemoteControlSet )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MediaStream::GetRemoteControlScreen Cannot find remote controls for Orbiter %d",PK_Orbiter);
		return 0;
	}
	m_mapRemoteControlSet[PK_Orbiter]=pRemoteControlSet;
	EntertainArea *pEntertainArea_OSD=NULL;
	bool bIsOSD=OrbiterIsOSD(PK_Orbiter,&pEntertainArea_OSD);

	if( m_bUseAltScreens )
	{
		if( bIsOSD && pRemoteControlSet->m_iPK_Screen_Alt_OSD )
			return pRemoteControlSet->m_iPK_Screen_Alt_OSD;
		else if( pRemoteControlSet->m_iPK_Screen_Alt_Remote )
			return pRemoteControlSet->m_iPK_Screen_Alt_Remote;
	}

	if( bIsOSD && pRemoteControlSet->m_iPK_Screen_OSD )
		return pRemoteControlSet->m_iPK_Screen_OSD;
	else
		return pRemoteControlSet->m_iPK_Screen_Remote;
}

bool MediaStream::StreamingRequired()
{
	// The key is to determine if streaming is going to be necessary, or if all the active destinations are the same
	// as the sources.  The source can be a capture card
	for(map<int,EntertainArea *>::iterator it=m_mapEntertainArea.begin();it!=m_mapEntertainArea.end();++it)
	{
		EntertainArea *pEntertainArea = it->second;
		if( !pEntertainArea->m_pMediaDevice_ActiveDest )
			return true;
		if( pEntertainArea->m_pMediaDevice_ActiveDest->m_pDeviceData_Router->WithinCategory(DEVICECATEGORY_Output_Zone_CONST) )
			continue; // It's a special output zone, so this is ok.  AlternatePipes will find a route, we're not streaming
		if( m_pDevice_CaptureCard && m_pDevice_CaptureCard->m_dwPK_Device==pEntertainArea->m_pMediaDevice_ActiveDest->m_pDeviceData_Router->m_dwPK_Device )
			continue; // this is ok
		if( pEntertainArea->m_pMediaDevice_ActiveDest!=m_pMediaDevice_Source )
			return true;  // We're streaming from a source to a different destination
	}

	return false;
}

void MediaStream::LoadDefaultAvSettings()
{
	// Clear the values since if this was a playlist there may be values from a prior media file
	m_sVideoSettings="";
	m_sAudioSettings="";
	m_sCommands="";

	map<int,int> mapAttributes;  // Get all attributes, arranged by priority mapAttributes.first=PK_Attribute, mapAttributes.second=Priority

	/* Priorities for determing with video/audio settings to use:
	10=specific for the current title (highest)
	20=other attribute for the current title
	30=genre for the current title
	*/

	MergeAttributes(mapAttributes,m_mapPK_Attribute);
	LoadDefaultAvSettings(m_dequeMediaSection,mapAttributes);
	LoadDefaultAvSettings(m_dequeMediaTitle,mapAttributes);
	MediaFile *pMediaFile = GetCurrentMediaFile();
	if( pMediaFile )
		LoadDefaultAvSettings(pMediaFile,mapAttributes);

	if( mapAttributes.empty() ) 
		return;

	string sAllAttributes;
	for(map<int,int>::iterator it=mapAttributes.begin();it!=mapAttributes.end();++it)
	{
		if( sAllAttributes.empty()==false )
			sAllAttributes += ",";
		sAllAttributes += StringUtils::itos(it->first);
	}

	string sSQL = "FK_Attribute IN (" + sAllAttributes + ")";
	vector<Row_Attribute_Settings *> vectRow_Attribute_Settings;
	int iPriorityAudioSetting=999,iPriorityVideoSetting=999,iPriorityCommand=999;  // Get the one with the lowest priority
	m_pMediaHandlerInfo->m_pMediaHandlerBase->m_pMedia_Plugin->m_pDatabase_pluto_media->Attribute_Settings_get()->GetRows(sSQL,&vectRow_Attribute_Settings);
	for(vector<Row_Attribute_Settings *>::iterator it=vectRow_Attribute_Settings.begin();it!=vectRow_Attribute_Settings.end();++it)
	{
		Row_Attribute_Settings *pRow_Attribute_Settings = *it;
        if( pRow_Attribute_Settings->AudioSetting_get().empty()==false && mapAttributes[pRow_Attribute_Settings->FK_Attribute_get()]<iPriorityAudioSetting )
		{
			iPriorityAudioSetting=mapAttributes[pRow_Attribute_Settings->FK_Attribute_get()];
			m_sAudioSettings=pRow_Attribute_Settings->AudioSetting_get();
		}
        if( pRow_Attribute_Settings->VideoSetting_get().empty()==false && mapAttributes[pRow_Attribute_Settings->FK_Attribute_get()]<iPriorityVideoSetting )
		{
			iPriorityVideoSetting=mapAttributes[pRow_Attribute_Settings->FK_Attribute_get()];
			m_sVideoSettings=pRow_Attribute_Settings->VideoSetting_get();
		}
        if( pRow_Attribute_Settings->Commands_get().empty()==false && mapAttributes[pRow_Attribute_Settings->FK_Attribute_get()]<iPriorityCommand )
		{
			iPriorityCommand=mapAttributes[pRow_Attribute_Settings->FK_Attribute_get()];
			m_sCommands=pRow_Attribute_Settings->Commands_get();
		}
	}
}

void MediaStream::LoadDefaultAvSettings(MediaFile *pMediaFile,map<int,int> &mapAttributes)
{
	MergeAttributes(mapAttributes,pMediaFile->m_mapPK_Attribute);
	LoadDefaultAvSettings(pMediaFile->m_dequeMediaSection,mapAttributes);
	LoadDefaultAvSettings(pMediaFile->m_dequeMediaTitle,mapAttributes);
}

void MediaStream::LoadDefaultAvSettings(deque<MediaSection *> &dequeMediaSection,map<int,int> &mapAttributes)
{
	for(deque<MediaSection *>::iterator it=dequeMediaSection.begin();it!=dequeMediaSection.end();++it)
	{
		MediaSection *pMediaSection = *it;
		MergeAttributes(mapAttributes,pMediaSection->m_mapPK_Attribute);
	}
}

void MediaStream::LoadDefaultAvSettings(deque<MediaTitle *> &dequeMediaTitle,map<int,int> &mapAttributes)
{
	for(deque<MediaTitle *>::iterator it=dequeMediaTitle.begin();it!=dequeMediaTitle.end();++it)
	{
		MediaTitle *pMediaTitle = *it;
		MergeAttributes(mapAttributes,pMediaTitle->m_mapPK_Attribute);
	}
}

void MediaStream::MergeAttributes(map<int,int> &mapAttributes,map< int,list_int > &mapPK_Attribute)
{
	for(map< int,list_int >::iterator itMA=mapPK_Attribute.begin();itMA!=mapPK_Attribute.end();++itMA)
	{
		for(list_int::iterator itLI=itMA->second.begin();itLI!=itMA->second.end();++itLI)
		{
			if( itMA->first==ATTRIBUTETYPE_Title_CONST || itMA->first==ATTRIBUTETYPE_Album_CONST )
				mapAttributes[*itLI]=10;
			else if( itMA->first==ATTRIBUTETYPE_Genre_CONST )
				mapAttributes[*itLI]=30;
			else
				mapAttributes[*itLI]=20;
		}
	}
}

string MediaStream::GetTargets(int PK_DeviceTemplate)
{
	string sTargets;
	for(map<int, class EntertainArea *>::iterator it=m_mapEntertainArea.begin();it!=m_mapEntertainArea.end();++it)
	{
		EntertainArea *pEntertainArea = it->second;
		ListMediaDevice *pListMediaDevice = pEntertainArea->m_mapMediaDeviceByTemplate_Find(PK_DeviceTemplate);
		if( pListMediaDevice && pListMediaDevice->size() )
		{
			MediaDevice *pMediaDevice = *(pListMediaDevice->begin());
			if( sTargets.empty()==false )
				sTargets += ",";
			sTargets += StringUtils::itos( pMediaDevice->m_pDeviceData_Router->m_dwPK_Device );
		}
	}
	return sTargets;
}

// This sends the set now playing command to an orbiter.  If pMessage is passed, it adds the command without sending it
void MediaStream::SetNowPlaying( OH_Orbiter *pOH_Orbiter, bool bRefreshScreen, bool bGotoRemote, Message *pMessage )
{
	Media_Plugin *pMedia_Plugin = m_pMediaHandlerInfo ? m_pMediaHandlerInfo->m_pMediaHandlerBase->m_pMedia_Plugin : NULL;
	if( !pMedia_Plugin )  // Don't this this will ever happen
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MediaStream::SetNowPlaying no media plugin");
		return;
	}
	StreamWasUpdated();

LoggerWrapper::GetInstance()->Write(LV_STATUS,"MediaStream::SetNowPlaying stream %p refresh %d"
					,this,(int) bRefreshScreen);
	string sRemotes;
	RemoteControlSet *pRemoteControlSet = NULL;
	// As a temporary measure we don't have a method for making certain stored video clips use a different
	// set of menu options than others, and yet stored dvd's need menu, subtitle, etc., options that normal
	// media doesn't.  We should comed up with a 'stream capabilities' function that allows us to add playback 
	// options on the fly, but until then, if it's a stored video file, and it has titles/sections, use the dvd's menu options
LoggerWrapper::GetInstance()->Write(LV_STATUS,"MediaStream::SetNowPlaying type %d containstitles %d",m_iPK_MediaType,(int) m_bContainsTitlesOrSections);
	if( m_iPK_MediaType==MEDIATYPE_pluto_StoredVideo_CONST && m_bContainsTitlesOrSections )
	{
		pRemoteControlSet = pMedia_Plugin->GetRemoteControlSet(pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,this,MEDIATYPE_pluto_DVD_CONST);
LoggerWrapper::GetInstance()->Write(LV_STATUS,"MediaStream::SetNowPlaying pRemoteControlSet_dvd %p",pRemoteControlSet);
	}
	else
		pRemoteControlSet = pMedia_Plugin->GetRemoteControlSet(pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,this);
	if( !pRemoteControlSet )
		return;

LoggerWrapper::GetInstance()->Write(LV_STATUS,"MediaStream::SetNowPlaying use alt screens %d alt osd %d alt remote %d",
				(int) m_bUseAltScreens,pRemoteControlSet->m_iPK_Screen_Alt_OSD,pRemoteControlSet->m_iPK_Screen_Alt_Remote);

	sRemotes = StringUtils::itos(m_bUseAltScreens && pRemoteControlSet->m_iPK_Screen_Alt_Remote ? pRemoteControlSet->m_iPK_Screen_Alt_Remote : pRemoteControlSet->m_iPK_Screen_Remote) + ","
		+ StringUtils::itos(pRemoteControlSet->m_iPK_DesignObj_Remote_Popup) + ","   // ON UI2 the leftmost popup menu on the main menu
		+ StringUtils::itos(pRemoteControlSet->m_iPK_Screen_FileList) + ","
		+ StringUtils::itos(m_bUseAltScreens && pRemoteControlSet->m_iPK_Screen_Alt_OSD ? pRemoteControlSet->m_iPK_Screen_Alt_OSD : pRemoteControlSet->m_iPK_Screen_OSD) + ","
		+ StringUtils::itos(pRemoteControlSet->m_iPK_Screen_OSD_Speed) + ","
		+ StringUtils::itos(pRemoteControlSet->m_iPK_Screen_OSD_Track);

	EntertainArea *pEntertainArea_OSD=NULL;

	int PK_Device_Source=0,iDequeMediaFile=0;

	OrbiterIsOSD(pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,&pEntertainArea_OSD);
	int PK_Screen = GetRemoteControlScreen(pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device);
	PK_Device_Source = m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device;
	if( m_iTrackOrSectionOrChannel==-1 )
		iDequeMediaFile = m_iDequeMediaFile_Pos;
	else
		iDequeMediaFile = m_iTrackOrSectionOrChannel;

	string sMediaDevices;

	if( m_pMediaDevice_Source->m_bCaptureCardActive )
	{
		// If we're using a capture card then the audio & video should go to the output device in this entertainment area, not the 
		// one the capture card is connected to
		MediaDevice *pMediaDevice = pOH_Orbiter->m_pEntertainArea ? pOH_Orbiter->m_pEntertainArea->m_pMediaDevice_ActiveDest : NULL;
		sMediaDevices = StringUtils::itos(m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device)
			+ "," + (pMediaDevice && pMediaDevice->m_pDevice_Video ? StringUtils::itos(pMediaDevice->m_pDevice_Video->m_dwPK_Device) : "")
			+ "," + (pMediaDevice && pMediaDevice->m_pDevice_Audio ? StringUtils::itos(pMediaDevice->m_pDevice_Audio->m_dwPK_Device) : "")
			+ "," + (m_pMediaDevice_Source->m_pDevice_CaptureCard && m_pMediaDevice_Source->m_bCaptureCardActive ? StringUtils::itos(m_pMediaDevice_Source->m_pDevice_CaptureCard->m_dwPK_Device) : "");
	}
	else
		sMediaDevices = StringUtils::itos(m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device)
			+ "," + (m_pMediaDevice_Source->m_pDevice_Video ? StringUtils::itos(m_pMediaDevice_Source->m_pDevice_Video->m_dwPK_Device) : "")
			+ "," + (m_pMediaDevice_Source->m_pDevice_Audio ? StringUtils::itos(m_pMediaDevice_Source->m_pDevice_Audio->m_dwPK_Device) : "")
			+ "," + (m_pMediaDevice_Source->m_pDevice_CaptureCard && m_pMediaDevice_Source->m_bCaptureCardActive ? StringUtils::itos(m_pMediaDevice_Source->m_pDevice_CaptureCard->m_dwPK_Device) : "");

	if( m_pMediaDevice_Source->m_pDevice_Audio && m_pMediaDevice_Source->m_pDevice_Audio->m_mapParameters_Find(DEVICEDATA_Discrete_Volume_CONST)=="1" )
		sMediaDevices += ",1";
	else
		sMediaDevices += ",0";

	sMediaDevices += ContainsVideo() ? ",1" : ",0";
	sMediaDevices += pEntertainArea_OSD && pEntertainArea_OSD->m_bViewingLiveAVPath ? ",1" : ",0";

	DCE::CMD_Set_Now_Playing CMD_Set_Now_Playing( pMedia_Plugin->m_dwPK_Device, pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device, 
		sRemotes, m_sMediaDescription, m_sSectionDescription, 
		m_iPK_MediaType, m_iStreamID_get(), iDequeMediaFile, m_sAppName, 
		sMediaDevices, bRefreshScreen);

	if( pMessage )
	{
		pMessage->m_vectExtraMessages.push_back(CMD_Set_Now_Playing.m_pMessage);
		if( bGotoRemote )
		{
			DCE::CMD_Goto_Screen CMD_Goto_Screen(pMedia_Plugin->m_dwPK_Device,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,"",PK_Screen,interuptAlways,true,false);
			pMessage->m_vectExtraMessages.push_back(CMD_Goto_Screen.m_pMessage);
		}
	}
	else
	{
		if( bGotoRemote )
		{
			DCE::CMD_Goto_Screen CMD_Goto_Screen(pMedia_Plugin->m_dwPK_Device,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,"",PK_Screen,interuptAlways,true,false);
			CMD_Set_Now_Playing.m_pMessage->m_vectExtraMessages.push_back(CMD_Goto_Screen.m_pMessage);
		}
		pMedia_Plugin->SendCommand( CMD_Set_Now_Playing );
	}
}
