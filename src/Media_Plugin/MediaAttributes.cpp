/*
 MediaAttributes

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com

 Phone: +1 (877) 758-8648

 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */

#include "PlutoUtils/CommonIncludes.h"
#include "MediaAttributes.h"
#include "Logger.h"

#ifndef WIN32
#include <dirent.h>
#include <attr/attributes.h>
#endif

// pluto_media related database access
#include "pluto_main/Table_MediaType.h"

#include "pluto_media/Table_Attribute.h"
#include "pluto_media/Table_File_Attribute.h"
#include "pluto_media/Table_MediaType_AttributeType.h"
#include "pluto_media/Table_File.h"
#include "pluto_media/Table_Bookmark.h"
#include "pluto_media/Table_Playlist.h"
#include "pluto_media/Table_PlaylistEntry.h"
#include "pluto_media/Table_Disc.h"
#include "pluto_media/Table_Picture_Disc.h"
#include "pluto_media/Table_Disc_Attribute.h"
#include "pluto_media/Table_Picture.h"
#include "pluto_media/Table_Picture_Attribute.h"
#include "pluto_media/Table_Picture_File.h"

#include "MediaFile.h"
#include "MediaStream.h"

using namespace DCE;

MediaAttributes::MediaAttributes(string host, string user, string pass, string db_name, int port, int nPK_Installation)
{
	m_nPK_Installation = nPK_Installation;
	m_pMediaAttributes_LowLevel=NULL;
    m_pDatabase_pluto_media = new Database_pluto_media(LoggerWrapper::GetInstance());
    if( !m_pDatabase_pluto_media->Connect( host, user, pass, db_name, port  ) )
    {
        LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Cannot connect to database!" );
        throw "Cannot connect";
    }
	m_pMediaAttributes_LowLevel = new MediaAttributes_LowLevel(m_pDatabase_pluto_media);
}

MediaAttributes::~MediaAttributes()
{
	delete m_pDatabase_pluto_media;
	delete m_pMediaAttributes_LowLevel;
}

bool MediaAttributes::SavePlaylist(deque<MediaFile *> &dequeMediaFile, int iPK_Users, int &iPK_Playlist, string sPlaylistName )
{
	Row_Playlist *pRow_Playlist=NULL;
	
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "MediaAttributes::SavePlaylist, user %d, pk_playlist %d",
		iPK_Users, iPK_Playlist);

	if( iPK_Playlist )
		pRow_Playlist = m_pDatabase_pluto_media->Playlist_get()->GetRow(iPK_Playlist);
	else
	{
		//search for <name, user> pair

		vector<Row_Playlist *> vectRow_Playlist;
		m_pDatabase_pluto_media->Playlist_get()->GetRows(
			"WHERE Name = '" + StringUtils::SQLEscape(sPlaylistName) + "' AND "
			"EK_User = " + StringUtils::ltos(iPK_Users),
			&vectRow_Playlist);

		if(vectRow_Playlist.empty())
		{
			pRow_Playlist = m_pDatabase_pluto_media->Playlist_get()->AddRow();
		}
		else
		{
			pRow_Playlist = vectRow_Playlist[0];
		}
	}

	if( !pRow_Playlist )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Tried to save to an invalid playlist");
		return false;
	}

    pRow_Playlist->Name_set(sPlaylistName);
	pRow_Playlist->EK_User_set(iPK_Users);

LoggerWrapper::GetInstance()->Write(LV_WARNING, "pl3 = %s %s",sPlaylistName.c_str(),pRow_Playlist->Name_get().c_str());

    if ( !pRow_Playlist->Table_Playlist_get()->Commit(true,true) )
    {
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot save/create playlist with ID %d error: %s",iPK_Playlist,
	        m_pDatabase_pluto_media->m_sLastDBError.c_str());

		return false;
    }

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "MediaAttributes::SavePlaylist, user %d, pk_playlist new %d",
		iPK_Users, iPK_Playlist);

    iPK_Playlist = pRow_Playlist->PK_Playlist_get();

    if( iPK_Playlist )
	{
		// We'll just re-write the whole thing out again anyway
	    vector<Row_PlaylistEntry*> vectRow_PlaylistEntry;
		pRow_Playlist->PlaylistEntry_FK_Playlist_getrows(&vectRow_PlaylistEntry);
#ifdef DEBUG
LoggerWrapper::GetInstance()->Write(LV_STATUS,"Deleting %d rows from old playlist",(int) vectRow_PlaylistEntry.size());
#endif
		for(size_t s=0;s<vectRow_PlaylistEntry.size();++s)
			vectRow_PlaylistEntry[s]->Delete();
	}

#ifdef DEBUG
LoggerWrapper::GetInstance()->Write(LV_STATUS,"Save playlist id %d with %d rows",iPK_Playlist,(int) dequeMediaFile.size());
#endif

	string sFiles;
	for(size_t s=0;s<dequeMediaFile.size();++s)
	{
		MediaFile *pMediaFile = dequeMediaFile[s];
		Row_PlaylistEntry *pRow_PlaylistEntry = m_pDatabase_pluto_media->PlaylistEntry_get()->AddRow();
		pRow_PlaylistEntry->FK_Playlist_set( iPK_Playlist );
		if( pMediaFile->m_dwPK_File )
		{
			pRow_PlaylistEntry->FK_File_set(pMediaFile->m_dwPK_File);
			if( sFiles.size() )
				sFiles += ",";
			sFiles += StringUtils::itos(pMediaFile->m_dwPK_File);
		}
		pRow_PlaylistEntry->Path_set(pMediaFile->m_sPath);
		pRow_PlaylistEntry->Filename_set(pMediaFile->m_sFilename);
		pRow_PlaylistEntry->Order_set(s);
	}

	if( pRow_Playlist->FK_Picture_isNull() && sFiles.empty()==false )
	{
		vector<Row_Picture_File *> vectRow_Picture_File;
		m_pDatabase_pluto_media->Picture_File_get()->GetRows("FK_File IN (" + sFiles + ") LIMIT 1",&vectRow_Picture_File);
		if( vectRow_Picture_File.size() )
			pRow_Playlist->FK_Picture_set( vectRow_Picture_File[0]->FK_Picture_get() );

	}
    if( !m_pDatabase_pluto_media->PlaylistEntry_get()->Commit(true,true) || !m_pDatabase_pluto_media->Playlist_get()->Commit(true,true) )
    {
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Could not save the playlist error: %s",m_pDatabase_pluto_media->m_sLastDBError.c_str());
        return false;
    }

    return true;
}

int MediaAttributes::LoadPlaylist(int iPK_Playlist, deque<MediaFile *> &dequeMediaFile, string &sPlaylistName)
{
    Row_Playlist *pRow_Playlist = m_pDatabase_pluto_media->Playlist_get()->GetRow(iPK_Playlist);
    if ( !pRow_Playlist )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find playlist: %d",iPK_Playlist);
		return false;
	}

    m_pMediaAttributes_LowLevel->PurgeDequeMediaFile(dequeMediaFile);
    vector<Row_PlaylistEntry *> vectRow_PlaylistEntry;
	pRow_Playlist->PlaylistEntry_FK_Playlist_getrows(&vectRow_PlaylistEntry);
    for( size_t s=0;s<vectRow_PlaylistEntry.size();++s )
      dequeMediaFile.push_back(new MediaFile(m_pMediaAttributes_LowLevel,vectRow_PlaylistEntry[s]));

    return (int) dequeMediaFile.size();
}

void MediaAttributes::AddAttributeToStream(MediaStream *pMediaStream,Row_Attribute *pRow_Attribute,int File,int Title_Track,int Section)
{
	if( !pRow_Attribute )
		return;
	int PK_AttributeType=pRow_Attribute->FK_AttributeType_get();
	// For CD's, the tracks are represented as files
LoggerWrapper::GetInstance()->Write(LV_STATUS,"MediaAttributes::AddAttributeToStream %p %d",pRow_Attribute,(pRow_Attribute ? pRow_Attribute->PK_Attribute_get() : 0));
	if( File )
	{
		if( File>0 && File<=pMediaStream->m_dequeMediaFile.size() )
		{
			MediaFile *pMediaFile = pMediaStream->m_dequeMediaFile[File-1];
			if( Title_Track )
			{
				while( Title_Track>pMediaFile->m_dequeMediaTitle.size() )
					pMediaFile->m_dequeMediaTitle.push_back(new MediaTitle());
				MediaTitle *pMediaTitle=pMediaFile->m_dequeMediaTitle[Title_Track-1];
				if( Section==0 )
					pMediaTitle->m_mapPK_Attribute[PK_AttributeType].push_back(pRow_Attribute->PK_Attribute_get());
				else
				{
					MediaSection *pMediaSection=m_pMediaAttributes_LowLevel->GetMediaSection(&pMediaTitle->m_dequeMediaSection,Section);
					pMediaSection->m_mapPK_Attribute[PK_AttributeType].push_back(pRow_Attribute->PK_Attribute_get());
				}
			}
			else if( Section==0 )
				pMediaFile->m_mapPK_Attribute[PK_AttributeType].push_back(pRow_Attribute->PK_Attribute_get());
			else
				m_pMediaAttributes_LowLevel->GetMediaSection(&pMediaFile->m_dequeMediaSection,Section)->m_mapPK_Attribute[PK_AttributeType].push_back(pRow_Attribute->PK_Attribute_get());
		}
	}
	else if( Title_Track ) // For DVD's Track is the title
	{
		while( Title_Track>pMediaStream->m_dequeMediaTitle.size() )
			pMediaStream->m_dequeMediaTitle.push_back(new MediaTitle());
		MediaTitle *pMediaTitle=pMediaStream->m_dequeMediaTitle[Title_Track-1];
		if( Section==0 )
			pMediaTitle->m_mapPK_Attribute[PK_AttributeType].push_back(pRow_Attribute->PK_Attribute_get());
		else
		{
			MediaSection *pMediaSection=m_pMediaAttributes_LowLevel->GetMediaSection(&pMediaTitle->m_dequeMediaSection,Section);
			pMediaSection->m_mapPK_Attribute[PK_AttributeType].push_back(pRow_Attribute->PK_Attribute_get());
		}
	}
	else if( Section==0 )
		pMediaStream->m_mapPK_Attribute[PK_AttributeType].push_back(pRow_Attribute->PK_Attribute_get());
	else
		m_pMediaAttributes_LowLevel->GetMediaSection(&pMediaStream->m_dequeMediaSection,Section)->m_mapPK_Attribute[PK_AttributeType].push_back(pRow_Attribute->PK_Attribute_get());
}

void MediaAttributes::LoadStreamAttributes(MediaStream *pMediaStream)
{
	MediaFile *pMediaFile_Current = pMediaStream->GetCurrentMediaFile();

	if( pMediaFile_Current && pMediaFile_Current->m_dwPK_Disk )
	{
		pMediaStream->m_dwPK_Disc = pMediaFile_Current->m_dwPK_Disk;
		pMediaStream->m_bIdentifiedDisc = true;
	}

	if( pMediaStream->m_dwPK_Disc )
	{
		LoadStreamAttributesForDisc(pMediaStream);
		return;
	}
	for(size_t s=0;s<pMediaStream->m_dequeMediaFile.size();++s)
	{
		MediaFile *pMediaFile = pMediaStream->m_dequeMediaFile[s];
		if( !pMediaFile->m_dwPK_File )
			continue;

		Row_File *pRow_File = m_pDatabase_pluto_media->File_get()->GetRow(pMediaFile->m_dwPK_File);
		if( !pRow_File )
			continue;

		vector<Row_File_Attribute *> vectRow_File_Attribute;
		pRow_File->File_Attribute_FK_File_getrows(&vectRow_File_Attribute);
		for(size_t sA=0;sA<vectRow_File_Attribute.size();++sA)
		{
			Row_File_Attribute *pRow_File_Attribute=vectRow_File_Attribute[sA];
			AddAttributeToStream(pMediaStream,pRow_File_Attribute->FK_Attribute_getrow(),
				pMediaStream->m_bContainsTitlesOrSections ? 0 : s+1,  // For DVD's and media with titles/sections, we don't use the file since the 1 file is just a stored image
				pMediaStream->m_iPK_MediaType==MEDIATYPE_pluto_StoredAudio_CONST ? 0 : pRow_File_Attribute->Track_get(),  // We don't use tracks now that it's ripped
				pRow_File_Attribute->Section_get());
		}
	}
}

void MediaAttributes::LoadStreamAttributesForDisc(MediaStream *pMediaStream)
{
	Row_Disc *pRow_Disc = m_pDatabase_pluto_media->Disc_get()->GetRow(pMediaStream->m_dwPK_Disc);
	if( !pRow_Disc )
		return;

	vector<Row_Disc_Attribute *> vectRow_Disc_Attribute;
	pRow_Disc->Disc_Attribute_FK_Disc_getrows(&vectRow_Disc_Attribute);
	for(size_t s=0;s<vectRow_Disc_Attribute.size();++s)
	{
		Row_Disc_Attribute *pRow_Disc_Attribute = vectRow_Disc_Attribute[s];
		Row_Attribute *pRow_Attribute = pRow_Disc_Attribute->FK_Attribute_getrow();
		if( pRow_Attribute ) // should always be true
		{
			if( pMediaStream->m_iPK_MediaType==MEDIATYPE_pluto_CD_CONST ) // Tracks are treated as files
			{
				// There are 2 ways this can happen.  The user can choose to play the whole cd, or just specific tracks from the browser grid
				// If the former is true, then there will be a MediaFile with m_iTrack for the track, and we should only add matching tracks
				bool bFoundMediaFilesWithTracks=false;
				for(int File = 0; File<pMediaStream->m_dequeMediaFile.size(); ++File)
				{
					MediaFile *pMediaFile = pMediaStream->m_dequeMediaFile[File];
					if( pMediaFile->m_iTrack )
					{
						bFoundMediaFilesWithTracks=true;
						if( pRow_Disc_Attribute->Track_get()==0 )
							AddAttributeToStream(pMediaStream,pRow_Attribute,0,0,pRow_Disc_Attribute->Section_get());
						else if( pMediaFile->m_iTrack == pRow_Disc_Attribute->Track_get() )
							AddAttributeToStream(pMediaStream,pRow_Attribute,File+1 /* track # means nothing now, it's the position in the deque we need */,0,pRow_Disc_Attribute->Section_get());
					}
				}
				if( !bFoundMediaFilesWithTracks )
					AddAttributeToStream(pMediaStream,pRow_Attribute,pRow_Disc_Attribute->Track_get(),0,pRow_Disc_Attribute->Section_get());
			}
			else
				AddAttributeToStream(pMediaStream,pRow_Attribute,0,pRow_Disc_Attribute->Track_get(),pRow_Disc_Attribute->Section_get());
		}
	}
}

