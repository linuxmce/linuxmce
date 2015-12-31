/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#ifndef MediaFile_h
#define MediaFile_h

#include "MediaTitle.h"
#include "MediaSection.h"
#include "MediaAttributes_LowLevel.h"
#include "pluto_media/Table_Bookmark.h"
#include "pluto_media/Table_File.h"
#include "pluto_media/Table_File_Attribute.h"
#include "pluto_media/Define_AttributeType.h"

class MediaFile
{
public:
	MediaFile(MediaAttributes_LowLevel *pMediaAttributes_LowLevel,int dwPK_File,string sFullyQualifiedFile)	{
		m_dwPK_File=dwPK_File; m_sPath=FileUtils::BasePath(sFullyQualifiedFile); m_sFilename=FileUtils::FilenameWithoutPath(sFullyQualifiedFile);
		m_dwPK_Bookmark=m_dwPK_Disk=m_dwPK_Device_Disk_Drive=0;
		m_Slot=0;
		m_dwDuration=0;
		m_tTimeout=0;
		m_dwPK_MediaType=m_dwPK_CommandGroup_Start=m_dwPK_CommandGroup_Stop=0;
		m_sExtension=StringUtils::ToUpper(FileUtils::FindExtension(sFullyQualifiedFile));
		CheckForStartPosition(pMediaAttributes_LowLevel);
		m_bWaitingForJukebox=false;

		m_iTrack = atoi(GetAttributeValue(pMediaAttributes_LowLevel,ATTRIBUTETYPE_Track_CONST).c_str());
		m_sTitle = GetAttributeValue(pMediaAttributes_LowLevel, ATTRIBUTETYPE_Title_CONST);
		m_sAlbum = GetAttributeValue(pMediaAttributes_LowLevel, ATTRIBUTETYPE_Album_CONST);
		m_sEpisode = GetAttributeValue(pMediaAttributes_LowLevel, ATTRIBUTETYPE_Episode_CONST);
		m_sSystem = GetAttributeValue(pMediaAttributes_LowLevel, ATTRIBUTETYPE_System_CONST);
	}

	MediaFile(int PK_Disk,int PK_Device_Disk_Drive,int Slot)	{
		m_dwPK_Disk=PK_Disk;
		m_dwPK_Device_Disk_Drive=PK_Device_Disk_Drive;
		m_Slot=Slot;
		m_dwPK_File=0;
		m_dwPK_Bookmark=0;
		m_dwDuration=0;
		m_tTimeout=0;
		m_iTrack=0;
		m_dwPK_MediaType=m_dwPK_CommandGroup_Start=m_dwPK_CommandGroup_Stop=0;
		m_bWaitingForJukebox=false;
	}

	MediaFile(string sMRL)	{
		m_sFilename=sMRL;
		m_dwPK_File=m_dwPK_Disk=m_dwPK_Device_Disk_Drive=0;
		m_Slot=0;
		m_dwPK_Bookmark=0;
		m_dwDuration=0;
		m_tTimeout=0;
		m_iTrack=0;
		m_dwPK_MediaType=m_dwPK_CommandGroup_Start=m_dwPK_CommandGroup_Stop=0;
		m_sExtension=StringUtils::ToUpper(FileUtils::FindExtension(sMRL));
		m_bWaitingForJukebox=false;
		if( StringUtils::StartsWith(sMRL,"cdda:/",true) )
		{
			// It's a cd.  Set m_iTrack if there's a trailing track number
			string sTrack = FileUtils::FilenameWithoutPath(sMRL);
			if( StringUtils::itos(atoi(sTrack.c_str()))==sTrack )  // It's purely numeric
				m_iTrack = atoi(sTrack.c_str());
		}
	}

	MediaFile(MediaAttributes_LowLevel *pMediaAttributes_LowLevel, string sFullyQualifiedFile) {
		m_sPath=FileUtils::BasePath(sFullyQualifiedFile); m_sFilename=FileUtils::FilenameWithoutPath(sFullyQualifiedFile);
		m_dwPK_File=pMediaAttributes_LowLevel->GetFileIDFromFilePath(sFullyQualifiedFile);
		m_dwPK_Disk=m_dwPK_Device_Disk_Drive=0;
		m_Slot=0;
		m_dwPK_Bookmark=0;
		m_dwDuration=0;
		m_tTimeout=0;
		m_dwPK_MediaType=m_dwPK_CommandGroup_Start=m_dwPK_CommandGroup_Stop=0;
		m_sExtension=StringUtils::ToUpper(FileUtils::FindExtension(sFullyQualifiedFile));
		CheckForStartPosition(pMediaAttributes_LowLevel);
		m_bWaitingForJukebox=false;

		m_iTrack = atoi(GetAttributeValue(pMediaAttributes_LowLevel,ATTRIBUTETYPE_Track_CONST).c_str());
		m_sTitle = GetAttributeValue(pMediaAttributes_LowLevel, ATTRIBUTETYPE_Title_CONST);
		m_sAlbum = GetAttributeValue(pMediaAttributes_LowLevel, ATTRIBUTETYPE_Album_CONST);
		m_sEpisode = GetAttributeValue(pMediaAttributes_LowLevel, ATTRIBUTETYPE_Episode_CONST);
		m_sSystem = GetAttributeValue(pMediaAttributes_LowLevel, ATTRIBUTETYPE_System_CONST);
	}

	MediaFile(MediaFile *pMediaFile_Copy) {
		m_dwPK_File=pMediaFile_Copy->m_dwPK_File;
		m_dwPK_Disk=pMediaFile_Copy->m_dwPK_Disk;
		m_Slot=pMediaFile_Copy->m_Slot;
		m_dwPK_Device_Disk_Drive=pMediaFile_Copy->m_dwPK_Device_Disk_Drive;
		m_sPath=pMediaFile_Copy->m_sPath;
		m_sFilename=pMediaFile_Copy->m_sFilename;
		m_sDescription=pMediaFile_Copy->m_sDescription;
		m_sStartPosition=pMediaFile_Copy->m_sStartPosition;
		m_sExtension=pMediaFile_Copy->m_sExtension;
		m_dwPK_CommandGroup_Start=pMediaFile_Copy->m_dwPK_CommandGroup_Start;
		m_dwPK_CommandGroup_Stop=pMediaFile_Copy->m_dwPK_CommandGroup_Stop;
		m_dwPK_MediaType=pMediaFile_Copy->m_dwPK_MediaType;
		m_dwPK_Bookmark=pMediaFile_Copy->m_dwPK_Bookmark;
		m_dwDuration=pMediaFile_Copy->m_dwDuration;
		m_tTimeout=pMediaFile_Copy->m_tTimeout;
		m_iTrack=pMediaFile_Copy->m_iTrack;
		m_bWaitingForJukebox=pMediaFile_Copy->m_bWaitingForJukebox;
		m_sTitle=pMediaFile_Copy->m_sTitle;
		m_sAlbum=pMediaFile_Copy->m_sAlbum;
		m_sEpisode=pMediaFile_Copy->m_sEpisode;
		m_sSystem=pMediaFile_Copy->m_sSystem;
	}


	MediaFile(MediaAttributes_LowLevel *pMediaAttributes_LowLevel,Row_PlaylistEntry *pRow_PlaylistEntry) {
		m_dwPK_File=pRow_PlaylistEntry->FK_File_get();
		m_dwPK_Disk=m_dwPK_Device_Disk_Drive=0;
		m_Slot=0;
		m_dwPK_Bookmark=pRow_PlaylistEntry->FK_Bookmark_get();
		m_dwDuration=pRow_PlaylistEntry->Duration_get();
		m_tTimeout=0;
		m_iTrack=0;
		m_dwPK_MediaType=0;
		m_bWaitingForJukebox=false;
		m_sPath=pRow_PlaylistEntry->Path_get();
		m_sFilename=pRow_PlaylistEntry->Filename_get();
		m_dwPK_CommandGroup_Start=pRow_PlaylistEntry->EK_CommandGroup_Start_get();
		m_dwPK_CommandGroup_Stop=pRow_PlaylistEntry->EK_CommandGroup_Stop_get();
		m_sTitle = GetAttributeValue(pMediaAttributes_LowLevel, ATTRIBUTETYPE_Title_CONST);
		m_sEpisode = GetAttributeValue(pMediaAttributes_LowLevel, ATTRIBUTETYPE_Episode_CONST);
		m_sSystem = GetAttributeValue(pMediaAttributes_LowLevel, ATTRIBUTETYPE_System_CONST);
		if( m_dwPK_Bookmark )
		{
			Row_Bookmark *pRow_Bookmark = pRow_PlaylistEntry->Table_PlaylistEntry_get()->Database_pluto_media_get()->Bookmark_get()->GetRow(m_dwPK_Bookmark);
			if( pRow_Bookmark )
			{
				Row_File *pRow_File = pRow_Bookmark->FK_File_getrow();
				if( pRow_File )
				{
					m_dwPK_File = pRow_File->PK_File_get();
					m_sPath = pRow_File->Path_get();
					m_sFilename = pRow_File->Filename_get();
					m_sStartPosition = pRow_Bookmark->Position_get();
				}
			}
		}	

	}

	~MediaFile() {
		for(size_t s=0;s<m_dequeMediaSection.size();++s)
			if( m_dequeMediaSection[s] )
				delete m_dequeMediaSection[s];
	}

	deque<MediaSection *> m_dequeMediaSection;
	deque<MediaTitle *> m_dequeMediaTitle;
	map< int,list_int > m_mapPK_Attribute;  /** An external media identification script may set attributes here, PK_AttributeType=PK_Attribute */
    list_int *m_mapPK_Attribute_Find(int PK_AttributeType) { map<int,list_int >::iterator it = m_mapPK_Attribute.find(PK_AttributeType); return it==m_mapPK_Attribute.end() ? NULL : &((*it).second); }
	int m_dwPK_File,m_dwPK_Disk,m_dwPK_MediaType,m_dwPK_Device_Disk_Drive;
	int m_Slot; // For Jukeboxes, the source slot.  0 means not specified
	bool m_bWaitingForJukebox; // if true, the disk is in a jukebox and we're waiting for the jukebox to move it to a drive before we can continue

	unsigned long m_dwPK_Bookmark,  // The bookmark to play instead of a file.  comes from Playlist_Entry.File
		m_dwDuration, // If Playlist_Entry has a value, this file will only play for this many seconds
		m_dwPK_CommandGroup_Start,m_dwPK_CommandGroup_Stop; // If specified in Playlist_Entry, these will be executed when the file starts and stops
	time_t m_tTimeout;
	int m_iTrack; // If this is a cd, the track to play
	string m_sPath,m_sFilename,m_sDescription,m_sExtension,m_sTitle,m_sAlbum,m_sEpisode,m_sSystem;
	string m_sStartPosition; /** Where to start the media the first time.  As soon as the media has begun MediaPlugin will reset this */

	string FullyQualifiedFile() {
		if( m_sPath.size() )
			return m_sPath + "/" + m_sFilename; 
		else
			return m_sFilename; 
	}
	string HumanReadableFullyQualifiedFile() {
		if( m_sPath.size()==0 )
			return m_sFilename; 

		string::size_type pos=1;
		string sHome = StringUtils::Tokenize(m_sPath,"/",pos);
		string sPublic = StringUtils::Tokenize(m_sPath,"/",pos);
		string sData = StringUtils::Tokenize(m_sPath,"/",pos);
		if( sHome!="home" || sData!="data" )
			return FullyQualifiedFile(); // It's a format we don't recognize

		string sResult;
		if( sPublic=="public" )
			sResult = "PUBLIC : ";
		else if ( sPublic.find("user_")==0 )
			sResult = "PRIVATE: ";
		else
			sResult = sPublic + "/";

		if( pos < m_sPath.size() )
			sResult += m_sPath.substr(pos) + "/";
		return sResult + m_sFilename;
	}

	void CheckForStartPosition(MediaAttributes_LowLevel *pMediaAttributes_LowLevel)
	{
		if( m_dwPK_File )
		{
			vector<Row_Bookmark *> m_vectRow_Bookmark;
			pMediaAttributes_LowLevel->m_pDatabase_pluto_media->Bookmark_get()->GetRows(BOOKMARK_FK_FILE_FIELD "=" + StringUtils::itos(m_dwPK_File) + " AND " BOOKMARK_DESCRIPTION_FIELD " ='START' AND " BOOKMARK_ISAUTORESUME_FIELD "=1",&m_vectRow_Bookmark);
			if( m_vectRow_Bookmark.size() )
			{
				Row_Bookmark *pRow_Bookmark = m_vectRow_Bookmark[0];
				m_sStartPosition = pRow_Bookmark->Position_get();
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"MediaFile::MediaFile File %d has start position %s", m_dwPK_File, m_sStartPosition.c_str());
			}
		}
	}

	string GetAttributeValue(MediaAttributes_LowLevel *pMediaAttributes_LowLevel, int nAttributeType)
	{
		if( m_dwPK_File )
		{
		  map<pair<int, int>, string>::iterator it = pMediaAttributes_LowLevel->m_mapAttributeCache.find(make_pair(m_dwPK_File, nAttributeType));
		  if (it!=pMediaAttributes_LowLevel->m_mapAttributeCache.end()) // cache hit
		    {
		      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"WWWW GetAttributeValue - Cache Hit - %s",it->second.c_str());
		      return it->second;
		    }
		  else // cache miss
		    {
		      vector<Row_Attribute *> vectRow_Attribute;
		      pMediaAttributes_LowLevel->m_pDatabase_pluto_media->Attribute_get()->GetRows(
												   "JOIN File_Attribute ON FK_Attribute = PK_Attribute "
												   "WHERE FK_File = " + StringUtils::ltos(m_dwPK_File) + " AND "
												   "FK_AttributeType = " + StringUtils::ltos(nAttributeType),
												   &vectRow_Attribute
												   );
		      if(!vectRow_Attribute.empty())
			{
			  Row_Attribute *pRow_Attribute = vectRow_Attribute[0];
			  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"WWWW GetAttributeValue - Cache Miss - %s",pRow_Attribute->Name_get().c_str());
			  return pRow_Attribute->Name_get();
			}
		      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"WWWW GetAttributeValue - NULL Value!");
		      return "";
		    }
		}
		return ""; // We need one here now, because of the cache. -tschak
	}
};

void operator+= (deque<MediaFile *> &dTarget, deque<MediaFile *> &dAdditional);

#endif
