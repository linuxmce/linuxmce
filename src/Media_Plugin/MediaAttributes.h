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
/**
 *
 * @file MediaAttributes.h
 * @brief header file for the MediaAttribute, MediaPicture, MediaAttributes class
 *
 */

#ifndef MEDIAATTRIBUTES_H
#define MEDIAATTRIBUTES_H

#include <deque>
using namespace std;

#include "pluto_media/Database_pluto_media.h"
#include "pluto_media/Table_PlaylistEntry.h"
#include "pluto_media/Table_Attribute.h"
#include "MediaSection.h"
#include "MediaAttributes_LowLevel.h"

/**
 * @brief Contains a media file
 */

namespace DCE
{
	class MediaStream;
};

class MediaFile;
class Row_Attribute;
class Row_Disc;
class Row_Picture;

/* For table: MediaType_AttributeType, Identifier means:
1=this attribute should be included when identifying the media.  For music, song, artist
2=this attribute should be included when identifying a collection of the media.  For music, album
3=this attribute is worth mentioning, but don't include it as a primary identifier (genre, etc.)

CombineAsOne, if 1, then if there are 2 files with the same attribute they will share the same entry in
the attribute table.  If 0, there will be 2 entries in the attribute table with the same value
*/

/**
 * @brief documentation
 * @todo ask
 */

class MediaAttributes
{
private:
    Database_pluto_media *m_pDatabase_pluto_media;
	int m_nPK_Installation;

public:
	MediaAttributes_LowLevel *m_pMediaAttributes_LowLevel;

	/** @brief constructor */
    MediaAttributes(string host, string user, string pass, string db_name, int port, int nPK_Installation);
    ~MediaAttributes();

	bool SavePlaylist(deque<MediaFile *> &dequeMediaFile, int iPK_Users, int &iPK_Playlist, string sPlaylistName );
	int LoadPlaylist(int iPK_Playlist, deque<MediaFile *> &dequeMediaFile, string &sPlaylistName);

	int AddIdentifiedDiscToDB(string sIdentifiedDisc,MediaStream *pMediaStream);
	bool IsDiscAlreadyIdentified(string sIdentifiedDisc,MediaStream *pMediaStream);
	void AddAttributeToStream(MediaStream *pMediaStream,Row_Attribute *pRow_Attribute,int File,int Track,int Section);
	void LoadStreamAttributes(MediaStream *pMediaStream);
	void LoadStreamAttributesForDisc(MediaStream *pMediaStream);
	void UpdateAttributeCache();
};

#endif
