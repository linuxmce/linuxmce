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
#ifndef __PLUTO_MEDIA_FILE_H__
#define __PLUTO_MEDIA_FILE_H__

#include <memory>
#include <string>
#include <map>
#include <list>
using namespace std;
//-----------------------------------------------------------------------------------------------------
class Database_pluto_media;
class Database_pluto_main;
class PlutoMediaAttributes;
class Row_File;
//-----------------------------------------------------------------------------------------------------
#include "GenericFileHandler.h"
//-----------------------------------------------------------------------------------------------------
enum MediaSyncMode
{
	modeNone,
	modeDbToFile,
	modeFileToDb,
	modeBoth
};
//-----------------------------------------------------------------------------------------------------
extern char const *MediaSyncModeStr[];
//-----------------------------------------------------------------------------------------------------
//
//  PlutoMediaFile class
//
//-----------------------------------------------------------------------------------------------------
class PlutoMediaFile
{
private:

	MediaSyncMode m_MediaSyncMode;
	PlutoMediaAttributes *m_pPlutoMediaAttributes;

	list<pair<char *, size_t> > m_listPicturesForTags;

    string m_sDirectory;
    string m_sFile;
	int m_nOurInstallationID;
	int m_nPK_MediaType;
	bool m_bIsDir;
	bool m_bNewFileToDb;

	Database_pluto_media *m_pDatabase_pluto_media;
	
	static MediaSyncMode m_DefaultMediaSyncMode;
	static bool m_bNewFilesAdded;
	static string m_sDVDKeysCache;

	auto_ptr<GenericFileHandler> m_spFileHandler;

	//file with attributes operations - load:
	void LoadPlutoAttributes();
	//file with attributes operations - save:
	void SavePlutoAttributes();

	//db operations - load:
	void LoadStartPosition();
	void LoadShortAttributes();
	void LoadLongAttributes();
	void LoadCoverarts();
	void LoadBookmarkPictures();
	void LoadMiscInfo();
	void LoadEverythingFromDb();
	//db operations - save:
	void SaveStartPosition();
	void SaveShortAttributesInDb(bool bAddAllToDb);
	void SaveLongAttributesInDb(bool bAddAllToDb);
	void SaveCoverarts();
	void SaveBookmarkPictures();
	void SaveMiscInfo();
	void SaveEveryThingToDb();
	void CacheDVDKeys();

	int NumberOfBookmarksFromDB();
	void GenerateMd5SumsCoverartsFromDb(list<string>& listMd5Sums);

	//misc helpers
	int AddFileToDatabase(int PK_MediaType);
	void AssignPlutoDevice(Row_File *pRow_File);
	void AssignPlutoUser(Row_File *pRow_File);
	int GetOwnerForPath(string sPath);
	int GetFileIDFromDB();
	string AdjustLongAttributeForDisplay(string sText);
	void UpdateMd5Field();

	//pictures
	pair<unsigned long, char *> LoadPicture(int nPictureId, bool bThumb = false);
	void SavePicture(pair<unsigned long, char *> pairPicture, int nPictureId, bool bThumb = false);
	void MergePictures();

public:
    PlutoMediaFile(Database_pluto_media *pDatabase_pluto_media, int PK_Installation,
        string sDirectory, string sFile,
		GenericFileHandler *pFileHandler /*= new GenericFileHandler()*/);
    ~PlutoMediaFile();

	//synchronization mode:
	void SetSyncMode(MediaSyncMode mode) { m_MediaSyncMode = mode; }
	static void SetDefaultSyncMode(MediaSyncMode mode) { m_DefaultMediaSyncMode = mode; }
	static MediaSyncMode GetDefaultSyncMode() { return m_DefaultMediaSyncMode; }

	//new file added status
	static bool NewFilesAdded() { return m_bNewFilesAdded; }
	static void ResetNewFilesAddedStatus() { m_bNewFilesAdded = false; }

	static void DVDKeysCacheSetup(string sValue) { if(!sValue.empty()) m_sDVDKeysCache = sValue; }

	void SetFileAttribute(int PK_File);
    int GetFileAttribute();
    void SetPicAttribute(int PK_Picture, string sPictureUrl);
    int GetPicAttribute(int PK_File);

	void SetMediaType(int PK_File, int PK_MediaType);

	int HandleFileNotInDatabase(int PK_MediaType = 0);
	void RenameAttribute(int Attribute_Type, string sOldValue, string sNewValue);
};
//-----------------------------------------------------------------------------------------------------
//
//  PlutoMediaIdentifier class
//
//-----------------------------------------------------------------------------------------------------
class PlutoMediaIdentifier
{
private:
    static map<string,int> m_mapExtensions;

    virtual ~PlutoMediaIdentifier() = 0; //don't allow instances
public:
    static void Activate(Database_pluto_main *pDatabase_pluto_main);
    static int Identify(string sFilename);
	static string GetExtensions(); //tab delimited
};
//-----------------------------------------------------------------------------------------------------

#endif
