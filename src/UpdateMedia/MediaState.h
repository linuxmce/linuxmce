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
#ifndef __MEDIA_STATE_H__
#define __MEDIA_STATE_H__
//-----------------------------------------------------------------------------------------------------
#include <string>
#include <map>
using namespace std;
//-----------------------------------------------------------------------------------------------------
#include "PlutoMediaFile.h"
//-----------------------------------------------------------------------------------------------------
struct MediaItemState
{
	int m_nFileID;
	string m_sPath;
	string m_sFilename;
	int m_nINode;
	string m_sCurrentDbAttrDate;
	int m_sCurrentDbAttrCount;
	string m_sOldDbAttrDate;
	int m_sOldDbAttrCount;
	string m_sOldFileDate;
	bool m_bHasAttributes;
	char m_cSource;

	MediaItemState(int nFileID, string sPath, string sFilename, int nINode, string sCurrentDbAttrDate,
		int sCurrentDbAttrCount, string sOldDbAttrDate, int sOldDbAttrCount, string sOldFileDate,
		bool bHasAttributes, char cSource
		) :
		m_nFileID(nFileID),
		m_sPath(sPath),
		m_sFilename(sFilename),
		m_nINode(nINode),
		m_sCurrentDbAttrDate(sCurrentDbAttrDate),
		m_sCurrentDbAttrCount(sCurrentDbAttrCount),
		m_sOldDbAttrDate(sOldDbAttrDate),
		m_sOldDbAttrCount(sOldDbAttrCount),
		m_sOldFileDate(sOldFileDate),
		m_bHasAttributes(bHasAttributes),
		m_cSource(cSource)
	{
	}

	MediaItemState() : m_nFileID(0), m_nINode(0), m_sCurrentDbAttrCount(0), 
		m_sOldDbAttrCount(0), m_bHasAttributes(false), m_cSource(0)
	{
	}
};
//-----------------------------------------------------------------------------------------------------
typedef map<pair<string /*path*/, string /*filename*/>, MediaItemState > MapMediaState;
//-----------------------------------------------------------------------------------------------------
class Database_pluto_media;
//-----------------------------------------------------------------------------------------------------
class MediaState
{
	MapMediaState m_mapMediaState;

	static MediaState m_instance;

	//helpers
	string ReadMediaFileInfo(string sDirectory, string sFile);
	MediaItemState LoadDbInfoForFile(Database_pluto_media *pDatabase_pluto_media, int nFileID);

public:

	static MediaState& Instance() { return m_instance; }

	void LoadDbInfo(Database_pluto_media *pDatabase_pluto_media, string sRootDirectory);
	MediaSyncMode SyncModeNeeded(string sDirectory, string sFile);

	void FileSynchronized(Database_pluto_media *pDatabase_pluto_media, string sDirectory, string sFile, int nFileID);
	int FileId(string sDirectory, string sFile);
};
//-----------------------------------------------------------------------------------------------------
#endif
