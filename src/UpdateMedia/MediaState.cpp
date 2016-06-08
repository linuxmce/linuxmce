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
#include "MediaState.h"

#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

#include "../PlutoUtils/StringUtils.h"
#include "../pluto_media/Database_pluto_media.h"
#include "../pluto_media/Table_File.h"
#include "FileUtils/file_utils.h"
#include "DCE/Logger.h"
using namespace DCE;

#include "FileHandlerFactory.h"
//-----------------------------------------------------------------------------------------------------
MediaState MediaState::m_instance;
//-----------------------------------------------------------------------------------------------------
void MediaState::LoadDbInfo(Database_pluto_media *pDatabase_pluto_media, string sRootDirectory)
{
	string sWhere = "1 = 1 AND (";

	vector<string> vectFolders;
	StringUtils::Tokenize(sRootDirectory, "|", vectFolders);
	for(vector<string>::iterator it = vectFolders.begin(); it != vectFolders.end(); ++it)
	{
		string sFolder = *it;

		if(it != vectFolders.begin())
			sWhere += " OR ";

		sWhere += 
			"Path LIKE '" + StringUtils::SQLEscape(FileUtils::ExcludeTrailingSlash(sFolder), true) + "%' "
			"OR (Path = '" + StringUtils::SQLEscape(FileUtils::ExcludeTrailingSlash(FileUtils::BasePath(sFolder))) + "' AND "
			"Filename = '" + StringUtils::SQLEscape(FileUtils::FilenameWithoutPath(sFolder)) + "' ) ";
	}

	sWhere += " ) ";

	string sSql =
		"SELECT \n"
		"  File.PK_File, \n"
		"  File.Path, \n"
		"  File.Filename, \n"
		"  File.INode, \n"
		"  s.CurrentDbAttrDate, \n"
		"  s.CurrentDbAttrCount, \n"
		"  s.HasAttributes, \n"
		"  File.AttrDate AS OldDbAttrDate, \n"
		"  File.AttrCount AS OldDbAttrCount, \n"
		"  File.ModificationDate AS OldFileDate, \n"
		"  File.Source \n"
		"FROM \n"
		"  ( \n"
		"    SELECT \n"
		"      PK_File, \n"
		"      greatest( \n"
		"        (SELECT IFNULL( MAX( Bookmark.psc_mod), CAST('0000-00-00 00:00:00' AS DATE)) FROM Bookmark WHERE FK_File=PK_File), \n"
		"        (SELECT IFNULL( MAX( Attribute.psc_mod), CAST('0000-00-00 00:00:00' AS DATE)) FROM Attribute RIGHT JOIN File_Attribute ON PK_Attribute=FK_Attribute WHERE FK_File=PK_File), \n"
		"        (SELECT IFNULL( MAX( LongAttribute.psc_mod), CAST('0000-00-00 00:00:00' AS DATE)) FROM LongAttribute WHERE FK_File=PK_File), \n"
		"        (SELECT IFNULL( MAX( Picture_File.psc_mod), CAST('0000-00-00 00:00:00' AS DATE)) FROM Picture_File WHERE FK_File=PK_File) \n"
		"      ) AS CurrentDbAttrDate, \n"
		"      (select count(*) from Bookmark where FK_File=PK_File) as BOOKMARKCOUNT, \n"
		"      (select count(*) from File_Attribute where FK_File=PK_File) as ACOUNT, \n"
		"      (select count(*) from LongAttribute where FK_File=PK_File) as LONGATTRCOUNT, \n"
		"      (select count(*) from Picture_File where FK_File=PK_File) as IMGCOUNT, \n"
		"      ( \n"
		"        (1000000 * (select BOOKMARKCOUNT)) + \n"
		"        (10000 * (select ACOUNT)) + \n"
		"        (100 * (select LONGATTRCOUNT)) + \n"
		"        (select IMGCOUNT) \n"
		"      ) as CurrentDbAttrCount, \n"
		"      ( \n"
		"        (select ACOUNT) + \n"
		"        (select LONGATTRCOUNT) \n"
		"      ) as HasAttributes \n"
		"    FROM \n"
		"      File \n"
		"    LEFT JOIN Bookmark ON Bookmark.FK_File = PK_File \n"
		"    LEFT JOIN File_Attribute ON File_Attribute.FK_File = PK_File \n"
		"    LEFT JOIN Attribute ON File_Attribute.FK_Attribute = PK_Attribute \n"
		"    LEFT JOIN LongAttribute ON LongAttribute.FK_File = PK_File \n"
		"    LEFT JOIN Picture_File ON Picture_File.FK_File = PK_File \n"
		"    WHERE " + sWhere + " AND Missing = 0 \n"
		"    GROUP BY PK_File \n"
		"  ) s INNER JOIN File ON s.PK_File=File.PK_File \n";

	enum SqlFields
	{
		sfFileID,
		sfPath,
		sfFilename,
		sfINode,
		sfCurrentDbAttrDate,
		sfCurrentDbAttrCount,
		sfHasAttributes,
		sfOldDbAttrDate,
		sfOldDbAttrCount,
		sfOldFileDate,
		sfSource
	};

	DB_ROW row;
	PlutoSqlResult allresult;

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "MediaState::LoadDbInfo ready to run big query");

	//do you want to debug this big query? uncomment this:
	//cout << endl << endl << sSql << endl << endl;

	if(NULL != (allresult.r = pDatabase_pluto_media->db_wrapper_query_result(sSql)))
	{
		LoggerWrapper::GetInstance()->Write(LV_MEDIA, "MediaState::LoadDbInfo got %d rows <- this is erroneously reporting 0", allresult.r->row_count);

		//reset current state, load fresh info
		m_mapMediaState.clear();

		while((row = db_wrapper_fetch_row(allresult.r)))
		{
			if(NULL != row && NULL != row[sfFileID] && NULL != row[sfPath] && NULL != row[sfFilename])
			{
				int nFileID = atoi(row[sfFileID]);
				string sPath = row[sfPath];
				string sFilename = row[sfFilename];
				int nInode = NULL != row[sfINode] ? atoi(row[sfINode]) : 0;

				string sCurrentDbAttrDate = NULL != row[sfCurrentDbAttrDate] ? row[sfCurrentDbAttrDate] : string();
				int sCurrentDbAttrCount = NULL != row[sfCurrentDbAttrCount] ? atoi(row[sfCurrentDbAttrCount]) : 0;
				string sOldDbAttrDate = NULL != row[sfOldDbAttrDate] ? row[sfOldDbAttrDate] : string();
				int sOldDbAttrCount = NULL != row[sfOldDbAttrCount] ? atoi(row[sfOldDbAttrCount]) : 0;
				string sOldFileDate = NULL != row[sfOldFileDate] ? row[sfOldFileDate] : string();
				bool bHasAttributes = NULL != row[sfHasAttributes] ? atoi(row[sfHasAttributes]) > 0 : false;
				char cSource = NULL != row[sfSource] ? row[sfSource][0] : 0;

				sOldDbAttrDate = (sOldDbAttrDate.empty() ? sCurrentDbAttrDate : sOldDbAttrDate);
				sOldDbAttrCount = (0 ? sCurrentDbAttrCount : sOldDbAttrCount);

				m_mapMediaState[make_pair(sPath, sFilename)] = MediaItemState(nFileID, sPath, sFilename, nInode,
					sCurrentDbAttrDate, sCurrentDbAttrCount, 
					sOldDbAttrDate, sOldDbAttrCount, sOldFileDate, bHasAttributes, cSource);
			}
		}
	}	
}
//-----------------------------------------------------------------------------------------------------
MediaSyncMode MediaState::SyncModeNeeded(string sDirectory, string sFile)
{
	MediaSyncMode sync_mode = modeNone;
	bool bNeedToUpdateDb = false;
	bool bNeedtoUpdateFile = false;

	string sCurrentFileDate = ReadMediaFileInfo(sDirectory, sFile);

	MapMediaState::iterator it = m_mapMediaState.find(make_pair(sDirectory, sFile));
	if(it != m_mapMediaState.end())
	{
		MediaItemState item = it->second;

		//is this our file or not source defined?
		if(item.m_cSource != 'F' && item.m_cSource != 'P' && item.m_cSource != 0)
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Not a valid file to get sync mode on.");
			return modeNone;
		}

LoggerWrapper::GetInstance()->Write(LV_MEDIA, "%s/%s: old file date %s, current file date %s", 
sDirectory.c_str(), sFile.c_str(), 
item.m_sOldFileDate.c_str(), sCurrentFileDate.c_str());

		if(StringUtils::SQLDateTime(item.m_sOldFileDate) != StringUtils::SQLDateTime(sCurrentFileDate))
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Need to update db for %s/%s: old file date %s, current file date %s",
				sDirectory.c_str(), sFile.c_str(), 
				item.m_sOldFileDate.c_str(), sCurrentFileDate.c_str());

			bNeedToUpdateDb = true;
		}

		string sCurrentFullFilename = sDirectory + "/" + sFile;

		if(
			(
				item.m_sCurrentDbAttrCount != item.m_sOldDbAttrCount || 
				StringUtils::SQLDateTime(item.m_sCurrentDbAttrDate) != StringUtils::SQLDateTime(item.m_sOldDbAttrDate)
			)
			//&&
			//!UpdateMediaFileUtils::IsDirectory(sCurrentFullFilename.c_str())
		)
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Need to update file for %s/%s: "
				"old attr count %d, current attr count %d, "
				"old attr date %s, current attr date %s",
				sDirectory.c_str(), sFile.c_str(), 
				item.m_sOldDbAttrCount, item.m_sCurrentDbAttrCount,
				item.m_sOldDbAttrDate.c_str(), item.m_sCurrentDbAttrDate.c_str()); 

			bNeedtoUpdateFile = true;
		}

		auto_ptr<GenericFileHandler> spFileHandler(FileHandlerFactory::CreateFileHandler(sDirectory, sFile));

		if(
			!spFileHandler->FileAttributeExists() &&
			item.m_sCurrentDbAttrDate != "" && item.m_bHasAttributes &&
			StringUtils::SQLDateTime(item.m_sCurrentDbAttrDate) > StringUtils::SQLDateTime(sCurrentFileDate)
			//&&
			//!UpdateMediaFileUtils::IsDirectory(sCurrentFullFilename.c_str())
		)
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Need to update file for %s/%s: "
				"current attr date %s, file date %s, has attr %d",
				sDirectory.c_str(), sFile.c_str(), 
			        item.m_sCurrentDbAttrDate.c_str(), sCurrentFileDate.c_str(), item.m_bHasAttributes); 

			bNeedtoUpdateFile = true;
		}

		if(bNeedToUpdateDb)
			if(bNeedtoUpdateFile)
				sync_mode = modeBoth;
			else 
				sync_mode = modeFileToDb;
		else
			if(bNeedtoUpdateFile)
				sync_mode = modeDbToFile;
			else
				sync_mode = modeNone;
	}
	else
	{
		//check if the inode already exists
		int INode = FileUtils::GetInode(sDirectory + "/" + sFile);
		
        for(MapMediaState::iterator it = m_mapMediaState.begin(), end = m_mapMediaState.end(); it != end; ++it)
		{
            if(it->second.m_nINode == INode && (sDirectory != it->second.m_sPath || sFile != it->second.m_sFilename))
            {
				string sDBFullFilename = it->second.m_sPath + "/" + it->second.m_sFilename;
				string sCurrentFullFilename = sDirectory + "/" + sFile;

				//double check that the file has that inode
				int INode_DBFile = FileUtils::GetInode(sDBFullFilename);
				int nDeviceID_DBFile = UpdateMediaFileUtils::GetDeviceID(sDBFullFilename.c_str());
				int nDeviceID_CurrentFile = UpdateMediaFileUtils::GetDeviceID(sCurrentFullFilename.c_str());

                if(INode_DBFile == INode && nDeviceID_DBFile == nDeviceID_CurrentFile && FileUtils::FileExists(sDBFullFilename))
                {
					LoggerWrapper::GetInstance()->Write(LV_MEDIA, "File already in DB: modeNone for inode %d and device id %d: %s vs %s", 
						INode, nDeviceID_DBFile, sCurrentFullFilename.c_str(), sDBFullFilename.c_str());

                    return modeNone;
                }
            }
		}

		LoggerWrapper::GetInstance()->Write(LV_MEDIA, "Need to update file because it's not in the database %s/%s",
			sDirectory.c_str(), sFile.c_str());

		sync_mode = modeBoth;
	}

	return sync_mode;
}
//-----------------------------------------------------------------------------------------------------
void MediaState::FileSynchronized(Database_pluto_media *pDatabase_pluto_media, string sDirectory, string sFile, int nFileID)
{
	MapMediaState::iterator it = m_mapMediaState.find(make_pair(sDirectory, sFile));
	MediaItemState item;
	if(it != m_mapMediaState.end())
		item = it->second;
	else
		item = LoadDbInfoForFile(pDatabase_pluto_media, nFileID);

	if(nFileID)
	{
		string sFileTimestamp = ReadMediaFileInfo(sDirectory, sFile);
		item.m_sOldFileDate = sFileTimestamp;
		item.m_sOldDbAttrCount = item.m_sCurrentDbAttrCount;
		item.m_sOldDbAttrDate = item.m_sCurrentDbAttrDate;

		Row_File *pRow_File = pDatabase_pluto_media->File_get()->GetRow(nFileID);
		pRow_File->Reload();
		pRow_File->ModificationDate_set(sFileTimestamp);
		pRow_File->AttrCount_set(item.m_sCurrentDbAttrCount);
		pRow_File->AttrDate_set(item.m_sCurrentDbAttrDate);
		pDatabase_pluto_media->File_get()->Commit();

		m_mapMediaState[make_pair(sDirectory, sFile)] = item;
	}
}
//-----------------------------------------------------------------------------------------------------
string MediaState::ReadMediaFileInfo(string sDirectory, string sFile)
{
	string sFilePath = sDirectory + "/" + sFile;

#ifdef WIN32
	struct __stat64 buf;
#else
	struct stat64 buf;
#endif

#ifdef WIN32
	if(!_stat64(sFilePath.c_str(), &buf))
#else
	if(!stat64(sFilePath.c_str(), &buf))
#endif
	{
		return StringUtils::SQLDateTime((time_t)buf.st_mtime);
	}

	return "";
}
//-----------------------------------------------------------------------------------------------------
MediaItemState MediaState::LoadDbInfoForFile(Database_pluto_media *pDatabase_pluto_media, int nFileID)
{
	string sSql =
		"SELECT \n"
		"  File.PK_File, \n"
		"  File.Path, \n"
		"  File.Filename, \n"
		"  File.INode, \n"
		"  s.CurrentDbAttrDate, \n"
		"  s.CurrentDbAttrCount, \n"
		"  s.HasAttributes, \n"
		"  File.AttrDate AS OldDbAttrDate, \n"
		"  File.AttrCount AS OldDbAttrCount, \n"
		"  File.ModificationDate AS OldFileDate, \n"
		"  File.Source \n"
		"FROM \n"
		"  ( \n"
		"    SELECT \n"
		"      PK_File, \n"
		"      greatest( \n"
		"        (SELECT IFNULL( MAX( Bookmark.psc_mod), CAST('0000-00-00 00:00:00' AS DATE)) FROM Bookmark WHERE FK_File=PK_File), \n"
		"        (SELECT IFNULL( MAX( Attribute.psc_mod), CAST('0000-00-00 00:00:00' AS DATE)) FROM Attribute RIGHT JOIN File_Attribute ON PK_Attribute=FK_Attribute WHERE FK_File=PK_File), \n"
		"        (SELECT IFNULL( MAX( LongAttribute.psc_mod), CAST('0000-00-00 00:00:00' AS DATE)) FROM LongAttribute WHERE FK_File=PK_File), \n"
		"        (SELECT IFNULL( MAX( Picture_File.psc_mod), CAST('0000-00-00 00:00:00' AS DATE)) FROM Picture_File WHERE FK_File=PK_File) \n"
		"      ) AS CurrentDbAttrDate, \n"
		"      (select count(*) from Bookmark where FK_File=PK_File) as BOOKMARKCOUNT, \n"
		"      (select count(*) from File_Attribute where FK_File=PK_File) as ACOUNT, \n"
		"      (select count(*) from LongAttribute where FK_File=PK_File) as LONGATTRCOUNT, \n"
		"      (select count(*) from Picture_File where FK_File=PK_File) as IMGCOUNT, \n"
		"      ( \n"
		"        (1000000 * (select BOOKMARKCOUNT)) + \n"
		"        (10000 * (select ACOUNT)) + \n"
		"        (100 * (select LONGATTRCOUNT)) + \n"
		"        (select IMGCOUNT) \n"
		"      ) as CurrentDbAttrCount, \n"
		"      ( \n"
		"        (select ACOUNT) + \n"
		"        (select LONGATTRCOUNT) \n"
		"      ) as HasAttributes \n"
		"    FROM \n"
		"      File \n"
		"    LEFT JOIN Bookmark ON Bookmark.FK_File = PK_File \n"
		"    LEFT JOIN File_Attribute ON File_Attribute.FK_File = PK_File \n"
		"    LEFT JOIN Attribute ON File_Attribute.FK_Attribute = PK_Attribute \n"
		"    LEFT JOIN LongAttribute ON LongAttribute.FK_File = PK_File \n"
		"    LEFT JOIN Picture_File ON Picture_File.FK_File = PK_File \n"
		"    WHERE PK_File = " + StringUtils::ltos(nFileID) + " AND Missing = 0 \n"
		"  ) s INNER JOIN File ON s.PK_File=File.PK_File \n";

	enum SqlFields
	{
		sfFileID,
		sfPath,
		sfFilename,
		sfINode,
		sfCurrentDbAttrDate,
		sfCurrentDbAttrCount,
		sfHasAttributes,
		sfOldDbAttrDate,
		sfOldDbAttrCount,
		sfOldFileDate,
		sfSource
	};

	DB_ROW row;
	PlutoSqlResult allresult;

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "MediaState::LoadDbInfoForFile ready to run big query on FileID: %d", nFileID );

	if(NULL != (allresult.r = pDatabase_pluto_media->db_wrapper_query_result(sSql)))
	{
		LoggerWrapper::GetInstance()->Write(LV_MEDIA, "MediaState::LoadDbInfoForFile got %d rows <- this is erroneously reporting 0", allresult.r->row_count);

		if(NULL != (row = db_wrapper_fetch_row(allresult.r)))
		{
			if(NULL != row[sfFileID] && NULL != row[sfPath] && NULL != row[sfFilename])
			{
				int nFileID = atoi(row[sfFileID]);
				string sPath = row[sfPath];
				string sFilename = row[sfFilename];
				int nInode = NULL != row[sfINode] ? atoi(row[sfINode]) : 0;

				string sCurrentDbAttrDate = NULL != row[sfCurrentDbAttrDate] ? row[sfCurrentDbAttrDate] : string();
				int sCurrentDbAttrCount = NULL != row[sfCurrentDbAttrCount] ? atoi(row[sfCurrentDbAttrCount]) : 0;
				string sOldDbAttrDate = NULL != row[sfOldDbAttrDate] ? row[sfOldDbAttrDate] : string();
				int sOldDbAttrCount = NULL != row[sfOldDbAttrCount] ? atoi(row[sfOldDbAttrCount]) : 0;
				string sOldFileDate = NULL != row[sfOldFileDate] ? row[sfOldFileDate] : string();
				bool bHasAttributes = NULL != row[sfHasAttributes] ? atoi(row[sfHasAttributes]) > 0 : false;
				char cSource = NULL != row[sfSource] ? row[sfSource][0] : 0;

				return MediaItemState(nFileID, sPath, sFilename, nInode,
					sCurrentDbAttrDate, sCurrentDbAttrCount, sOldDbAttrDate, 
					sOldDbAttrCount, sOldFileDate, bHasAttributes, cSource);
			}
		}
	}

	return MediaItemState();
}
//-----------------------------------------------------------------------------------------------------
int MediaState::FileId(string sDirectory, string sFile)
{
	MapMediaState::iterator it = m_mapMediaState.find(make_pair(sDirectory, sFile));
	if(it != m_mapMediaState.end())
		return it->second.m_nFileID;

	return 0;
}
//-----------------------------------------------------------------------------------------------------
