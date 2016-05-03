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
#include "PlutoMediaFile.h"

#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/Other.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <algorithm>

#include "pluto_main/Table_MediaType.h"
#include "pluto_main/Table_DeviceTemplate_MediaType.h"
#include "pluto_main/Table_Installation.h"

#include "pluto_media/Table_File.h"
#include "pluto_media/Table_Picture_File.h"
#include "pluto_media/Table_Picture.h"
#include "pluto_media/Table_Attribute.h"
#include "pluto_media/Table_LongAttribute.h"
#include "pluto_media/Table_Picture_Attribute.h"
#include "pluto_media/Table_File_Attribute.h"
#include "pluto_media/Table_Bookmark.h"
#include "pluto_media/Define_AttributeType.h"
#include "pluto_main/Define_MediaType.h"

#include "Media_Plugin/MediaAttributes_LowLevel.h"

#include "PlutoMediaAttributes.h"
#include "MediaIdentifier.h"

#include "FileUtils/file_utils.h"

#include "MediaState.h"

#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

using namespace UpdateMediaFileUtils;
using namespace std;
using namespace DCE;

#define MAX_PICTURES 20

#include "pluto_media/Database_pluto_media.h"
//-----------------------------------------------------------------------------------------------------
char *MediaSyncModeStr[] =
{
	"modeNone",
	"modeDbToFile",
	"modeFileToDb",
	"modeBoth"
};
//-----------------------------------------------------------------------------------------------------
//
//  PlutoMediaFile class
//
//-----------------------------------------------------------------------------------------------------
MediaSyncMode PlutoMediaFile::m_DefaultMediaSyncMode = modeNone;
bool PlutoMediaFile::m_bNewFilesAdded = false;
string PlutoMediaFile::m_sDVDKeysCache = "/home/.dvdcss";
//-----------------------------------------------------------------------------------------------------
PlutoMediaFile::PlutoMediaFile(Database_pluto_media *pDatabase_pluto_media, int PK_Installation, 
	string sDirectory, string sFile, GenericFileHandler *pFileHandler) : 
	m_MediaSyncMode(modeNone), m_pPlutoMediaAttributes(NULL)
{
	//initializations
    m_pDatabase_pluto_media = pDatabase_pluto_media;
    m_sDirectory = sDirectory;
    m_sFile = sFile;
	m_nOurInstallationID = PK_Installation;
	m_nPK_MediaType = 0;
	m_bNewFileToDb = false;
	m_spFileHandler.reset(pFileHandler);

	string sFilePath = sDirectory + "/" + sFile;
	m_bIsDir = UpdateMediaFileUtils::IsDirectory(sFilePath.c_str());

	LoggerWrapper::GetInstance()->Write(LV_WARNING, "# PlutoMediaFile STARTED: dir %s file %s", 
		m_sDirectory.c_str(), m_sFile.c_str());

	LoadPlutoAttributes();

	if(NULL == m_pPlutoMediaAttributes)
		m_pPlutoMediaAttributes = new PlutoMediaAttributes();

#ifdef UPDATEMEDIA_STATUS
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Processing path %s, file %s. "
		"Found %d attributes, %d long attributes in file", 
		m_sDirectory.c_str(), m_sFile.c_str(), m_pPlutoMediaAttributes->m_mapAttributes.size(),
		m_pPlutoMediaAttributes->m_mapLongAttributes.size());
#endif
}
//-----------------------------------------------------------------------------------------------------
PlutoMediaFile::~PlutoMediaFile()
{
	if(m_MediaSyncMode == modeDbToFile || m_MediaSyncMode == modeBoth)
	{
		//Save everything in file
		SavePlutoAttributes();

	}

	if(m_MediaSyncMode == modeFileToDb || m_MediaSyncMode == modeBoth)
	{
		//Save everything in db
		SaveEveryThingToDb();
	}

	AssignPlutoDevice(NULL);
	AssignPlutoUser(NULL);

	if(StringUtils::ToLower(FileUtils::FindExtension(m_sFile)) == "dvd")
		CacheDVDKeys();

	if(NULL != m_pPlutoMediaAttributes)
	{
		if(!m_pPlutoMediaAttributes->m_nFileID)
			m_pPlutoMediaAttributes->m_nFileID = GetFileIDFromDB();

		if(m_pPlutoMediaAttributes->m_nFileID == 0)
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Got to sync file, but it's not in the db %s/%s!",
				m_sDirectory.c_str(), m_sFile.c_str());
			return;
		}

		MediaState::Instance().FileSynchronized(m_pDatabase_pluto_media, m_sDirectory, m_sFile, 
			m_pPlutoMediaAttributes->m_nFileID);
	}

	delete m_pPlutoMediaAttributes;

#ifdef UPDATEMEDIA_STATUS
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "# PlutoMediaFile ENDED: dir %s file %s, sync mode %s", 
		m_sDirectory.c_str(), m_sFile.c_str(), MediaSyncModeStr[m_MediaSyncMode]);
#endif
}
//-----------------------------------------------------------------------------------------------------
string PlutoMediaFile::AdjustLongAttributeForDisplay(string sText)
{	
	const size_t cnMaxSize = 30;
	return sText.length() > cnMaxSize ? (sText.substr(cnMaxSize) + "...").c_str() : sText.c_str();
}
//-----------------------------------------------------------------------------------------------------
int PlutoMediaFile::HandleFileNotInDatabase(int PK_MediaType)
{
	m_nPK_MediaType = PK_MediaType;

#ifdef UPDATEMEDIA_STATUS
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "HandleFileNotInDatabase media type %d", PK_MediaType);
#endif
	int PK_File=0;
	// See if the same INode is already in the database
	int INode = FileUtils::GetInode(m_sDirectory + "/" + m_sFile);
	if( INode>0 )
	{
		vector<Row_File *> vectRow_File;
		// Attempt to match against m_sDirectory to not cross match files, then check against INode only (file has moved).
		m_pDatabase_pluto_media->File_get()->GetRows("INode=" + StringUtils::itos(INode) + " AND Path='" + m_sDirectory + "'",&vectRow_File);
		if ( !vectRow_File.size() )
		{
			m_pDatabase_pluto_media->File_get()->GetRows("INode=" + StringUtils::itos(INode),&vectRow_File);
		}
		if( vectRow_File.size() )  // Should only be 1
		{
			Row_File *pRow_File = vectRow_File[0];
			pRow_File->Reload();

			string sMidMD5File = FileUtils::GetMidFileChecksum(m_sDirectory + "/" + m_sFile);

			//reuse the file only if it's the same file (midmd5 is the same)
			if(pRow_File->MD5_get() == sMidMD5File || pRow_File->MD5_isNull())
			{
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "Reusing record PK_File %d for %s/%s ? Md5 db %s, md5 file %s", pRow_File->PK_File_get(), m_sDirectory.c_str(), m_sFile.c_str(), pRow_File->MD5_get().c_str(), sMidMD5File.c_str());

				PK_File=pRow_File->PK_File_get();
				pRow_File->Ignore_set(0);  // This could be a re-used INode
				pRow_File->IsNew_set(1); // This could be a re-used INode
				pRow_File->Path_set(m_sDirectory);
				pRow_File->Filename_set(m_sFile);
				pRow_File->Source_set(m_spFileHandler->GetFileSourceForDB());
				pRow_File->Missing_set(0);
				pRow_File->Table_File_get()->Commit();

#ifdef UPDATEMEDIA_STATUS
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "PlutoMediaFile::HandleFileNotInDatabase %s/%s N db-attr: %d Inode: %d size %d mt %d/%d, md5 %s", 
					m_sDirectory.c_str(), m_sFile.c_str(), pRow_File->PK_File_get(), INode, (int) vectRow_File.size(), PK_MediaType, pRow_File->EK_MediaType_get(),
					pRow_File->MD5_get().c_str());
#endif
			}
			else
			{
#ifdef UPDATEMEDIA_STATUS
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "Won't reuse file record %d from db because md5 is different", pRow_File->PK_File_get());
#endif
				m_pPlutoMediaAttributes->m_nFileID = 0; 
				m_pPlutoMediaAttributes->m_nInstallationID = 0;
			}
		}
	}

	//make sure it's not already in the database
	if(!PK_File)
		PK_File = GetFileIDFromDB();

#ifdef UPDATEMEDIA_STATUS
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "%s/%s not IN db-attr: %d INode: %d", m_sDirectory.c_str(), m_sFile.c_str(), PK_File, INode);
#endif
    // Is it a media file?
    if(!PK_MediaType)
	{
        PK_MediaType = PlutoMediaIdentifier::Identify(m_sDirectory + "/" + m_sFile);
		m_nPK_MediaType = PK_MediaType;
	}
	
	// HACKOMAT - The VDR information files do not have an extension (since 1.7.4)
	if ( m_sFile == "info")
	{
#ifdef UPDATEMEDIA_DEBUG
        	LoggerWrapper::GetInstance()->Write(LV_DEBUG, "VDR file %s/%s is now defined as video file", m_sDirectory.c_str(), m_sFile.c_str());
#endif
	        m_nPK_MediaType = 5;
	        PK_MediaType = 5;
        }

	if(PK_MediaType == 0)
	{
		if(m_bIsDir)
		{
			string sFullPath = FileUtils::IncludeTrailingSlash(m_sDirectory) + m_sFile;
			string sBasePath = "/home/public/data/";
			if(sFullPath.find(sBasePath + "videos") == 0)
				PK_MediaType = MEDIATYPE_pluto_StoredVideo_CONST;
			else if(sFullPath.find(sBasePath + "audio") == 0)
				PK_MediaType = MEDIATYPE_pluto_StoredAudio_CONST;
			else if(sFullPath.find(sBasePath + "VDRTV") == 0)
				PK_MediaType = MEDIATYPE_pluto_StoredVideo_CONST;
			else if(sFullPath.find(sBasePath + "pictures") == 0)
				PK_MediaType = MEDIATYPE_pluto_Pictures_CONST;
			else if(sFullPath.find(sBasePath + "documents") == 0)
				PK_MediaType = MEDIATYPE_misc_DocViewer_CONST;
			else if(sFullPath.find(sBasePath + "games") == 0)
				PK_MediaType = MEDIATYPE_lmce_Game_CONST;
			else if(sFullPath.find(sBasePath + "pvr") == 0)
				PK_MediaType = MEDIATYPE_pluto_StoredVideo_CONST;
			else
				PK_MediaType = MEDIATYPE_misc_DocViewer_CONST;
		}
	}
#ifdef UPDATEMEDIA_STATUS
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Media Type is: %d, is folder %d", PK_MediaType, m_bIsDir);
#endif
	if(!PK_File)
    {
		if(PK_MediaType || m_bIsDir)
            return AddFileToDatabase(PK_MediaType);
        else
        {
#ifdef UPDATEMEDIA_STATUS
            LoggerWrapper::GetInstance()->Write(LV_STATUS, "not a media file");
#endif
            return 0;
        }
    }
    else
    {
#ifdef UPDATEMEDIA_STATUS
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Reusing file, updating media type to %d", PK_MediaType);
#endif

        Row_File *pRow_File = m_pDatabase_pluto_media->File_get()->GetRow(PK_File);
        if( !pRow_File )
        {
            SetFileAttribute(0);  // The file doesn't exist.  Shouldn't really happen
#ifdef UPDATEMEDIA_STATUS
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Huh??  not in db now");
#endif
            return 0;
        }
        // it was in the database, but it's been moved to a different directory or renamed
		pRow_File->Reload();
        pRow_File->Path_set(m_sDirectory);
        pRow_File->Filename_set(m_sFile);
		pRow_File->EK_MediaType_set(PK_MediaType);
		pRow_File->IsDirectory_set(m_bIsDir);
		pRow_File->Source_set(m_spFileHandler->GetFileSourceForDB());
		pRow_File->Missing_set(0);
		AssignPlutoUser(pRow_File);
		AssignPlutoDevice(pRow_File);
		pRow_File->Table_File_get()->Commit();

		m_pPlutoMediaAttributes->m_nFileID = PK_File;
    }

	return PK_File;
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::SaveEveryThingToDb()
{
#ifdef UPDATEMEDIA_STATUS
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "# SyncDbAttributes: ready to sync db with attributes found in attribute file");
#endif
	// Is it a media file?
	if(!m_nPK_MediaType)
		m_nPK_MediaType = PlutoMediaIdentifier::Identify(m_sDirectory + "/" + m_sFile);
	
	//not a media file
	if(!m_nPK_MediaType)
		return;

	SaveStartPosition();
	SaveShortAttributesInDb(false);
	SaveLongAttributesInDb(false);
	SaveCoverarts();
	SaveBookmarkPictures();
	SaveMiscInfo();

	UpdateMd5Field();
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::SaveStartPosition()
{
	//save/update start position in the database, if needed
	if(m_pPlutoMediaAttributes->m_sStartPosition != "")
	{
		vector<Row_Bookmark *> vectBookmarks;
		m_pDatabase_pluto_media->Bookmark_get()->GetRows("Description = 'START' AND IsAutoResume = 1 "
			"AND FK_File = " + StringUtils::ltos(m_pPlutoMediaAttributes->m_nFileID), &vectBookmarks);

		if(vectBookmarks.size() > 0)
		{
			Row_Bookmark *pRow_Bookmark = vectBookmarks[0];
			if(pRow_Bookmark->Position_get() != m_pPlutoMediaAttributes->m_sStartPosition)
			{
#ifdef UPDATEMEDIA_STATUS
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "# SyncDbAttributes: got different start position. Updating to %s...",
					m_pPlutoMediaAttributes->m_sStartPosition.c_str());
#endif
				pRow_Bookmark->Position_set(m_pPlutoMediaAttributes->m_sStartPosition);
				pRow_Bookmark->Table_Bookmark_get()->Commit();
			}
			else
			{
#ifdef UPDATEMEDIA_STATUS
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "# SyncDbAttributes: already got same start position. Ignoring...");
#endif
			}
		}
		else
		{
#ifdef UPDATEMEDIA_STATUS
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "# SyncDbAttributes: No start position in the database for this file. "
				"Adding %s start position.", m_pPlutoMediaAttributes->m_sStartPosition.c_str());
#endif
			Row_Bookmark *pRow_Bookmark_New = m_pDatabase_pluto_media->Bookmark_get()->AddRow();
			pRow_Bookmark_New->Position_set(m_pPlutoMediaAttributes->m_sStartPosition);
			pRow_Bookmark_New->Description_set("START");
			pRow_Bookmark_New->FK_File_set(m_pPlutoMediaAttributes->m_nFileID);
			pRow_Bookmark_New->EK_MediaType_set(m_nPK_MediaType);
			pRow_Bookmark_New->IsAutoResume_set(1);
			m_pDatabase_pluto_media->Bookmark_get()->Commit();
		}
	}
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::SaveShortAttributesInDb(bool bAddAllToDb)
{
	if(!m_pPlutoMediaAttributes->m_nFileID)
		m_pPlutoMediaAttributes->m_nFileID = GetFileIDFromDB();

	int PK_File = m_pPlutoMediaAttributes->m_nFileID;

	if(PK_File == 0)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Got to add short attributes to file, but it's not in the db %s/%s!",
			m_sDirectory.c_str(), m_sFile.c_str());
		return;
	}

	MapPlutoMediaAttributes mapPlutoMediaAttributes;

	if(!bAddAllToDb)
	{
		string SQL = 
			"SELECT PK_Attribute,FK_AttributeType, Name, Track, Section "
			"FROM Attribute JOIN File_Attribute ON PK_Attribute = FK_Attribute "
			"WHERE FK_File = " + StringUtils::ltos(PK_File);

		PlutoSqlResult result;
		DB_ROW row;
		if((result.r = m_pDatabase_pluto_media->db_wrapper_query_result(SQL)))
		{
			while((row = db_wrapper_fetch_row(result.r)) && NULL != row[0] && NULL != row[1])
			{
				int nPK_Attribute = atoi(row[0]);
				int nFK_AttributeType = atoi(row[1]);
				string sName = row[2];
				int nTrack = NULL != row[3] ? atoi(row[3]) : 0;
				int nSection = NULL != row[4] ? atoi(row[4]) : 0;

				mapPlutoMediaAttributes.insert(
					std::make_pair(
					nFK_AttributeType, 
					new PlutoMediaAttribute(nPK_Attribute, nFK_AttributeType, sName, nTrack, nSection)
					)
					);
			}
		}
	}

#ifdef UPDATEMEDIA_STATUS
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "# SyncDbAttributes: attributes is db %d, total %d",
		mapPlutoMediaAttributes.size(), m_pPlutoMediaAttributes->m_mapAttributes.size());
#endif
	// For albums we don't want to add a new one for each media file.  All media from the same album
	// should be grouped into a single album attribute.  However since different (Album Artists)[performers] could have
	// albums of the same name we need to make a pass first to find the (Album Artist)[performer] and skip the album,
	// and then another pass to add the album, this should prioritize Album Artist (if available) over performer
	int PK_Attribute_Album_Artist=0;
	int PK_Attribute_Performer=0;
	for(int iPass=0;iPass<2;++iPass)
	{
		//Save any new attributes in the database
		for(MapPlutoMediaAttributes::iterator it = m_pPlutoMediaAttributes->m_mapAttributes.begin(),
			end = m_pPlutoMediaAttributes->m_mapAttributes.end(); it != end; ++it)
		{
			PlutoMediaAttribute *pPlutoMediaAttribute = it->second;

			bool bAttributeAlreadyAdded = false;

			// If this runs, then only new fields are added to the DB.  Existing fields are not updated.
			if(!bAddAllToDb)
			{
				for(MapPlutoMediaAttributes::iterator itdb = mapPlutoMediaAttributes.begin(),
					enddb = mapPlutoMediaAttributes.end(); itdb != enddb; ++itdb)
				{
					PlutoMediaAttribute *pDBPlutoMediaAttribute = itdb->second;

					if(
						pPlutoMediaAttribute->m_nType == pDBPlutoMediaAttribute->m_nType		&&
						StringUtils::ToLower(pPlutoMediaAttribute->m_sName) == StringUtils::ToLower(pDBPlutoMediaAttribute->m_sName)		&&
						pPlutoMediaAttribute->m_nTrack == pDBPlutoMediaAttribute->m_nTrack		&&
						pPlutoMediaAttribute->m_nSection == pDBPlutoMediaAttribute->m_nSection
						)
					{
						// need to do a first pass for Album Artist, if not found use Performer
						if( pPlutoMediaAttribute->m_nType==ATTRIBUTETYPE_Album_Artist_CONST )
						{
							PK_Attribute_Album_Artist = pDBPlutoMediaAttribute->m_nPK_Attribute;
						}
						else if( pPlutoMediaAttribute->m_nType==ATTRIBUTETYPE_Performer_CONST )
						{
							PK_Attribute_Performer = pDBPlutoMediaAttribute->m_nPK_Attribute;
							if( !PK_Attribute_Album_Artist )
							{
								// If we don't have an Album Artist, choose the first Performer we come across
								PK_Attribute_Album_Artist = PK_Attribute_Performer;
							}
						}
						bAttributeAlreadyAdded = true;
						break;
					}
				}
			}

			// This has been altered to use Album Artist when available, otherwise the first performer in the list
			if(!bAttributeAlreadyAdded )
			{
				if(pPlutoMediaAttribute->m_nType > 0 && !StringUtils::OnlyWhiteSpace(pPlutoMediaAttribute->m_sName))
				{
					// Do not add album attribute on pass 0, only add album attribute on pass 1
					if( (iPass==0 && pPlutoMediaAttribute->m_nType==ATTRIBUTETYPE_Album_CONST) || (iPass==1 && pPlutoMediaAttribute->m_nType!=ATTRIBUTETYPE_Album_CONST) )
						continue; // See notes above

					// When getting the album attribute this will grab the specific album associated with this Album Artist/Performer
					// Request the current attributes' value as it exists in the DB right now.
					MediaAttributes_LowLevel mediaAttributes_LowLevel(m_pDatabase_pluto_media);
					Row_Attribute *pRow_Attribute = mediaAttributes_LowLevel.GetAttributeFromDescription(m_nPK_MediaType,
						pPlutoMediaAttribute->m_nType, pPlutoMediaAttribute->m_sName, PK_Attribute_Album_Artist /*PK_Attribute_Performer*/ );

					// Skip this attribute if it is invalid in anyway?  I'm unsure of exactly when this would return NULL.
					if( pRow_Attribute==NULL )
					{
						LoggerWrapper::GetInstance()->Write(LV_WARNING,"PlutoMediaFile::SyncDbAttributes attribute type %d/%s is empty",
							pPlutoMediaAttribute->m_nType, pPlutoMediaAttribute->m_sName.c_str());
						continue;
					}

					// Assigns current PK_Attribute = to value currently in the DB
					pPlutoMediaAttribute->m_nPK_Attribute = pRow_Attribute->PK_Attribute_get();

					// If this is the Album Artist or Peformer record the PK value
					if( pPlutoMediaAttribute->m_nType==ATTRIBUTETYPE_Album_Artist_CONST )
					{
						PK_Attribute_Album_Artist = pPlutoMediaAttribute->m_nPK_Attribute;
					}
					if( pPlutoMediaAttribute->m_nType==ATTRIBUTETYPE_Performer_CONST )
					{
						PK_Attribute_Performer = pPlutoMediaAttribute->m_nPK_Attribute;
						if( !PK_Attribute_Album_Artist )
						{
							// If we don't have an Album Artist, choose the first Performer we come across
							PK_Attribute_Album_Artist = PK_Attribute_Performer;
						}
					}

					//already in the database? if not, put it in the database
					if(NULL == m_pDatabase_pluto_media->File_Attribute_get()->GetRow(
						PK_File, pRow_Attribute->PK_Attribute_get(),
						pPlutoMediaAttribute->m_nTrack, pPlutoMediaAttribute->m_nSection)
						)
					{
						Row_File_Attribute *pRow_File_Attribute = m_pDatabase_pluto_media->File_Attribute_get()->AddRow();
						pRow_File_Attribute->FK_File_set(PK_File);
						pRow_File_Attribute->FK_Attribute_set(pRow_Attribute->PK_Attribute_get());
						pRow_File_Attribute->Section_set(pPlutoMediaAttribute->m_nSection);
						pRow_File_Attribute->Track_set(pPlutoMediaAttribute->m_nTrack);
						pRow_File_Attribute->Table_File_Attribute_get()->Commit();

#ifdef UPDATEMEDIA_STATUS
						LoggerWrapper::GetInstance()->Write(LV_STATUS, "# SyncDbAttributes: Adding attribute to database: "
							"for PK_File %d, AttrID %d, AttrType = %d with value %s, section %d, track %d", 
							PK_File, pRow_Attribute->PK_Attribute_get(), pPlutoMediaAttribute->m_nType,
							pPlutoMediaAttribute->m_sName.c_str(), pPlutoMediaAttribute->m_nSection, pPlutoMediaAttribute->m_nTrack); 
#endif
					}
				}
			}
		}
	}

	for(MapPlutoMediaAttributes::iterator itdb = mapPlutoMediaAttributes.begin(); itdb != mapPlutoMediaAttributes.end(); ++itdb)
		delete itdb->second;
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::SaveLongAttributesInDb(bool bAddAllToDb)
{
	if(!m_pPlutoMediaAttributes->m_nFileID)
		m_pPlutoMediaAttributes->m_nFileID = GetFileIDFromDB();

	int PK_File = m_pPlutoMediaAttributes->m_nFileID;


	if(PK_File == 0)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Got to add long attributes to file, but it's not in the db %s/%s!",
			m_sDirectory.c_str(), m_sFile.c_str());
		return;
	}


	MapPlutoMediaAttributes mapPlutoMediaAttributes;

	if(!bAddAllToDb)
	{
		string SQL = 
			"SELECT FK_AttributeType, Text "
			"FROM LongAttribute "
			"WHERE FK_File = " + StringUtils::ltos(PK_File);

		PlutoSqlResult result;
		DB_ROW row;
		if((result.r = m_pDatabase_pluto_media->db_wrapper_query_result(SQL)))
		{
			while((row = db_wrapper_fetch_row(result.r)) && NULL != row[0] && NULL != row[1])
			{
				int nFK_AttributeType = atoi(row[0]);
				string sName = row[1];

				mapPlutoMediaAttributes.insert(
					std::make_pair(
						nFK_AttributeType, 
						new PlutoMediaAttribute(0, nFK_AttributeType, sName)
					)
				);
			}
		}
	}

#ifdef UPDATEMEDIA_STATUS
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "# SyncDbAttributes: long attributes is db %d, total %d",
		mapPlutoMediaAttributes.size(), m_pPlutoMediaAttributes->m_mapLongAttributes.size());
#endif
	//Save any new long attributes in the database
	for(MapPlutoMediaAttributes::iterator it = m_pPlutoMediaAttributes->m_mapLongAttributes.begin(),
		end = m_pPlutoMediaAttributes->m_mapLongAttributes.end(); it != end; ++it)
	{
		PlutoMediaAttribute *pPlutoMediaAttribute = it->second;

		bool bAttributeAlreadyAdded = false;

		if(!bAddAllToDb)
		{
			for(MapPlutoMediaAttributes::iterator itdb = mapPlutoMediaAttributes.begin(),
				enddb = mapPlutoMediaAttributes.end(); itdb != enddb; ++itdb)
			{
				PlutoMediaAttribute *pDBPlutoMediaAttribute = itdb->second;

				if(
					pPlutoMediaAttribute->m_nType == pDBPlutoMediaAttribute->m_nType &&
					StringUtils::ToLower(pPlutoMediaAttribute->m_sName) == StringUtils::ToLower(pDBPlutoMediaAttribute->m_sName)
					)
				{
					bAttributeAlreadyAdded = true;
					break;
				}
			}
		}

		if(!bAttributeAlreadyAdded)
		{
			if(pPlutoMediaAttribute->m_nType > 0 && !StringUtils::OnlyWhiteSpace(pPlutoMediaAttribute->m_sName))
			{
				Row_LongAttribute *pRow_LongAttribute = m_pDatabase_pluto_media->LongAttribute_get()->AddRow();
				pRow_LongAttribute->FK_AttributeType_set(pPlutoMediaAttribute->m_nType);
				pRow_LongAttribute->FK_File_set(PK_File);
				pRow_LongAttribute->Text_set(pPlutoMediaAttribute->m_sName);
				pRow_LongAttribute->Table_LongAttribute_get()->Commit();

#ifdef UPDATEMEDIA_STATUS
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "# SyncDbAttributes: Adding long attribute to database: "
					"for PK_File %d, long AttrID %d, AttrType = %d with value %s", 
					PK_File, pRow_LongAttribute->PK_LongAttribute_get(), pPlutoMediaAttribute->m_nType,
					AdjustLongAttributeForDisplay(pPlutoMediaAttribute->m_sName).c_str()
				); 
#endif
			}
		}
	}

	for(MapPlutoMediaAttributes::iterator itdb = mapPlutoMediaAttributes.begin(); itdb != mapPlutoMediaAttributes.end(); ++itdb)
		delete itdb->second;
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::SaveBookmarkPictures()
{
	//got bookmarks in attribute file, but none in the database?
	if(!m_pPlutoMediaAttributes->m_listBookmarks.empty() && !NumberOfBookmarksFromDB())
	{
#ifdef UPDATEMEDIA_STATUS
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "SaveBookmarkPictures: no bookmarks to db. Adding those from file %d",
			m_pPlutoMediaAttributes->m_listBookmarks.size());
#endif

		int nCounter = 0;
		for(ListBookmarks::iterator it = m_pPlutoMediaAttributes->m_listBookmarks.begin(), 
			end = m_pPlutoMediaAttributes->m_listBookmarks.end(); it != end; ++it)
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "SaveBookmarkPictures: saving bookmark...");

			PlutoMediaBookmark *pBookmark = *it;

			int PK_Picture = 0;
			if(pBookmark->m_dataPicture.m_dwSize != 0 && pBookmark->m_dataPictureThumb.m_dwSize != 0)
			{
				Row_Picture *pRow_Picture = m_pDatabase_pluto_media->Picture_get()->AddRow();
				pRow_Picture->Extension_set("jpg");
				pRow_Picture->Table_Picture_get()->Commit();

				SavePicture(make_pair(pBookmark->m_dataPicture.m_dwSize, pBookmark->m_dataPicture.m_pBlock), pRow_Picture->PK_Picture_get());
				SavePicture(make_pair(pBookmark->m_dataPictureThumb.m_dwSize, pBookmark->m_dataPictureThumb.m_pBlock), pRow_Picture->PK_Picture_get(), true);

				PK_Picture = pRow_Picture->PK_Picture_get();
			}

			Row_Bookmark *pRow_Bookmark = m_pDatabase_pluto_media->Bookmark_get()->AddRow();
			pRow_Bookmark->Description_set(pBookmark->m_sDescription);
			pRow_Bookmark->Position_set(pBookmark->m_sPosition);
			pRow_Bookmark->FK_File_set(m_pPlutoMediaAttributes->m_nFileID);

			if(PK_Picture != 0)
				pRow_Bookmark->FK_Picture_set(PK_Picture);

			pRow_Bookmark->Table_Bookmark_get()->Commit();

			if(++nCounter > MAX_PICTURES)
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "SaveBookmarkPictures: too many bookmarks...");
				break;
			}
		}
	}
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::SaveMiscInfo()
{
#ifdef UPDATEMEDIA_STATUS
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "# SaveMiscInfo: FK_FileFormat %d, FK_MediaSubType %d", 
		m_pPlutoMediaAttributes->m_nFileFormat, m_pPlutoMediaAttributes->m_nMediaSubType);
#endif
	Row_File *pRow_File = m_pDatabase_pluto_media->File_get()->GetRow(m_pPlutoMediaAttributes->m_nFileID);
	if(NULL != pRow_File)
	{
		pRow_File->Reload();

		if(m_pPlutoMediaAttributes->m_nFileFormat != 0)
			pRow_File->FK_FileFormat_set(m_pPlutoMediaAttributes->m_nFileFormat);
		else
			pRow_File->FK_FileFormat_setNull(true);

		if(m_pPlutoMediaAttributes->m_nMediaSubType != 0)
			pRow_File->FK_MediaSubType_set(m_pPlutoMediaAttributes->m_nMediaSubType);
		else
			pRow_File->FK_MediaSubType_setNull(true);

		pRow_File->Table_File_get()->Commit();
	}
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::SaveCoverarts()
{
	list<string> listMd5Sums;
	GenerateMd5SumsCoverartsFromDb(listMd5Sums);

#ifdef UPDATEMEDIA_STATUS
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "SaveCoverarts: got %d coverarts in db. "
		"Merging with %d from file", listMd5Sums.size(), m_pPlutoMediaAttributes->m_mapCoverarts.size());
#endif
	MapPictures::iterator it = m_pPlutoMediaAttributes->m_mapCoverarts.begin();
	MapPictures::iterator end = m_pPlutoMediaAttributes->m_mapCoverarts.end();

	for( ; it != end ; ++it)
	{
		string sMd5Sum = FileUtils::FileChecksum(it->second, it->first);

		if(listMd5Sums.end() == std::find(listMd5Sums.begin(), listMd5Sums.end(), sMd5Sum))
		{
#ifdef UPDATEMEDIA_STATUS
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "SaveCoverarts: adding picture with size %d, md5sum %s to db",
				it->first, sMd5Sum.c_str());
#endif
			Row_Picture *pRow_Picture = m_pDatabase_pluto_media->Picture_get()->AddRow();
			pRow_Picture->Extension_set("jpg");
			pRow_Picture->Table_Picture_get()->Commit();

			Row_Picture_File *pRow_Picture_File = m_pDatabase_pluto_media->Picture_File_get()->AddRow();
			pRow_Picture_File->FK_File_set(m_pPlutoMediaAttributes->m_nFileID);
			pRow_Picture_File->FK_Picture_set(pRow_Picture->PK_Picture_get());
			pRow_Picture_File->Table_Picture_File_get()->Commit();

			SavePicture(make_pair(it->first, it->second), pRow_Picture->PK_Picture_get());
			SavePicture(make_pair(it->first, it->second), pRow_Picture->PK_Picture_get(), true);
		}
		else
		{
#ifdef UPDATEMEDIA_STATUS
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "SaveCoverarts: not adding picture with size %d, md5sum %s"
				" since is already in db", it->first, sMd5Sum.c_str());
#endif
		}
	}
}
//-----------------------------------------------------------------------------------------------------
int PlutoMediaFile::AddFileToDatabase(int PK_MediaType)
{
	m_nPK_MediaType = PK_MediaType;
	Row_File *pRow_File = NULL;

	//We'll have to be sure first that we won't create duplicates; check first is there are any
	//rows we can reuse, marked as 'Missing' (deleted before because the file was removed/moved)
	//However, this should never happen. Added this code temporary to trace a possible logic flaw.
	vector<Row_File *> vectRow_File;
	m_pDatabase_pluto_media->File_get()->GetRows(
		"Path = '" + StringUtils::SQLEscape(FileUtils::ExcludeTrailingSlash(m_sDirectory)) + 
		"' AND Filename = '" + StringUtils::SQLEscape(m_sFile) + "' AND Missing = 1", &vectRow_File);

	//Any luck to reuse a missing file record?
	if(vectRow_File.size())
	{
#ifdef UPDATEMEDIA_STATUS
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "PlutoMediaFile::AddFileToDatabase -> there is already "
			"a record in the database marked as missing for %s - %s file", m_sDirectory.c_str(), m_sFile.c_str());
#endif
		//Get the first record and reuse it
		pRow_File = vectRow_File[0];
		pRow_File->Reload();
		pRow_File->Missing_set(0);
		pRow_File->EK_MediaType_set(PK_MediaType);
		AssignPlutoUser(pRow_File);
		pRow_File->Table_File_get()->Commit();

		//This will get a vector with desired File_Attribute's rows
		vector<Row_File_Attribute *> vectRow_File_Attribute;
		m_pDatabase_pluto_media->File_Attribute_get()->GetRows("FK_File = " + StringUtils::ltos(pRow_File->PK_File_get()),
			&vectRow_File_Attribute);

		//Iterate through the vector and mark all the rows as deleted
		for(vector<Row_File_Attribute *>::iterator it = vectRow_File_Attribute.begin();
			it != vectRow_File_Attribute.end(); ++it
		)
		{
			Row_File_Attribute *pRow_File_Attribute = *it;
			pRow_File_Attribute->Delete();
		}

		//If there are indeed rows to be deleted, commit the changes
		if(vectRow_File_Attribute.size())
			m_pDatabase_pluto_media->File_Attribute_get()->Commit();

#ifdef UPDATEMEDIA_STATUS
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "PlutoMediaFile::AddFileToDatabase -> reusing PK_File %d. "
			"Deleted children records.", pRow_File->PK_File_get());
#endif
	}
	else
	{
		//No record to reuse; we'll create a new one
		pRow_File = m_pDatabase_pluto_media->File_get()->AddRow();
		pRow_File->DateAdded_set(StringUtils::SQLDateTime(time(NULL)));
		pRow_File->Path_set(FileUtils::ExcludeTrailingSlash(m_sDirectory));
		pRow_File->Filename_set(FileUtils::ExcludeTrailingSlash(m_sFile));
		pRow_File->IsDirectory_set(m_bIsDir);
		pRow_File->EK_MediaType_set(PK_MediaType);
		pRow_File->Source_set(m_spFileHandler->GetFileSourceForDB());
		AssignPlutoUser(pRow_File);
		pRow_File->INode_set( FileUtils::GetInode( pRow_File->Path_get() + "/" + pRow_File->Filename_get() ) );
		pRow_File->Missing_set(0);
		pRow_File->Table_File_get()->Commit();

#ifdef UPDATEMEDIA_STATUS
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "PlutoMediaFile::AddFileToDatabase new %s PK_File %d Inode %d",
			(pRow_File->Path_get() + "/" + pRow_File->Filename_get()).c_str(), pRow_File->PK_File_get(), pRow_File->INode_get() );
#endif
	}

	//These are our installation and our file
	m_pPlutoMediaAttributes->m_nInstallationID = m_nOurInstallationID;
	m_pPlutoMediaAttributes->m_nFileID = pRow_File->PK_File_get();

	SaveShortAttributesInDb(true);
	SaveLongAttributesInDb(true);

#ifdef UPDATEMEDIA_STATUS
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "PlutoMediaFile::AddFileToDatabase picture url %s",
		m_pPlutoMediaAttributes->m_sPictureUrl.c_str());
#endif
	// Got a picture url? Let's download it if we don't have already coverarts saved
	if(m_pPlutoMediaAttributes->m_mapCoverarts.empty())
	{
		if(m_pPlutoMediaAttributes->m_sPictureUrl != "")
		{
			//It's a "new" file, but we know the picture url
			//we'll download the picture and record in Picture table
			MediaAttributes_LowLevel mediaAttributes_LowLevel(m_pDatabase_pluto_media);
			Row_Picture *pRow_Picture = mediaAttributes_LowLevel.AddPicture(NULL, 0, 
				FileUtils::FindExtension(m_pPlutoMediaAttributes->m_sPictureUrl), 
				m_pPlutoMediaAttributes->m_sPictureUrl
			);

			if(pRow_Picture)
			{
				m_pPlutoMediaAttributes->m_nPictureID = pRow_Picture->PK_Picture_get();

				Row_Picture_File *pRow_Picture_File = m_pDatabase_pluto_media->Picture_File_get()->AddRow();
				pRow_Picture_File->FK_File_set(pRow_File->PK_File_get());
				pRow_Picture_File->FK_Picture_set(pRow_Picture->PK_Picture_get());
				pRow_Picture_File->Table_Picture_File_get()->Commit();

#ifdef UPDATEMEDIA_STATUS
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "PlutoMediaFile::AddFileToDatabase Added picture to file: PK_File %d, PK_Picture %d",
					pRow_File->PK_File_get(), m_pPlutoMediaAttributes->m_nPictureID);
#endif
			}
			else
			{
#ifdef UPDATEMEDIA_STATUS
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "PlutoMediaFile::AddFileToDatabase Failed to add picture to file: PK_File %d, picture url: %s",
					pRow_File->PK_File_get(), m_pPlutoMediaAttributes->m_sPictureUrl.c_str());
#endif
			}
		}
	}

#ifdef UPDATEMEDIA_STATUS
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Added %s/%s to db with PK_File = %d", m_sDirectory.c_str(), m_sFile.c_str(),
		pRow_File->PK_File_get());
#endif
	m_bNewFileToDb = true;
	m_bNewFilesAdded = true;

	if( PK_MediaType==MEDIATYPE_pluto_Pictures_CONST && !m_bIsDir)
	{
		string Output = m_sDirectory + "/" + m_sFile;
		StringUtils::Replace(&Output,"\"","\\\"");
		string sCmd = "convert \"" + Output + "\" -scale 256x256 -antialias \"jpeg:" + Output + ".tnj\"";
#ifdef UPDATEMEDIA_STATUS
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Creating thumbnail %s",sCmd.c_str());
#endif
		system(sCmd.c_str());
	}

    return pRow_File->PK_File_get();
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::SetMediaType(int PK_File, int PK_MediaType)
{
	if(0 != PK_File && m_nPK_MediaType != PK_MediaType)
	{
		m_nPK_MediaType = PK_MediaType;

		Row_File *pRow_File = m_pDatabase_pluto_media->File_get()->GetRow(PK_File);
		pRow_File->Reload();
		pRow_File->EK_MediaType_set(PK_MediaType);
		m_pDatabase_pluto_media->File_get()->Commit();

#ifdef UPDATEMEDIA_STATUS
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "File %d has now media type %d",
			PK_File, PK_MediaType);
#endif
	}
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::AssignPlutoDevice(Row_File *pRow_File)
{
	bool bCommitNeeded = (NULL == pRow_File);

	if(NULL == pRow_File)
		pRow_File = m_pDatabase_pluto_media->File_get()->GetRow(m_pPlutoMediaAttributes->m_nFileID);

	if(NULL != pRow_File)
	{
		map<int, int> mapMountedDevices;
		list<string> listFiles;
		FileUtils::FindDirectories(listFiles, "/mnt/device", false, true);
		MapMountedDevicesToPlutoDevices(listFiles, mapMountedDevices);
		int nEK_Device = PlutoDeviceForFile(m_sDirectory + "/" + m_sFile, mapMountedDevices);

#ifdef UPDATEMEDIA_STATUS
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "File %s/%s on pluto device %d", m_sDirectory.c_str(), m_sFile.c_str(), nEK_Device);
#endif
		if(nEK_Device != 0)
			pRow_File->EK_Device_set(nEK_Device);
		else
			pRow_File->EK_Device_setNull(true);

		if(bCommitNeeded)
			pRow_File->Table_File_get()->Commit();
	}
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::AssignPlutoUser(Row_File *pRow_File)
{
	bool bCommitNeeded = (NULL == pRow_File);

	if(NULL == pRow_File)
		pRow_File = m_pDatabase_pluto_media->File_get()->GetRow(m_pPlutoMediaAttributes->m_nFileID);

	if(NULL != pRow_File)
	{
		int nEK_Users_Private = GetOwnerForPath(m_sDirectory);

#ifdef UPDATEMEDIA_STATUS
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "File %s/%s assigned to user %d", m_sDirectory.c_str(), m_sFile.c_str(), nEK_Users_Private);
#endif
		if(nEK_Users_Private != 0)
			pRow_File->EK_Users_Private_set(nEK_Users_Private);
		else
			pRow_File->EK_Users_Private_setNull(true);

		if(bCommitNeeded)
			pRow_File->Table_File_get()->Commit();
	}
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::UpdateMd5Field()
{
	string sMidMd5 = FileUtils::GetMidFileChecksum(m_sDirectory + "/" + m_sFile);

	Row_File *pRow_File = m_pDatabase_pluto_media->File_get()->GetRow(m_pPlutoMediaAttributes->m_nFileID);
	if(NULL != pRow_File)
	{
		pRow_File->Reload();

#ifdef UPDATEMEDIA_STATUS
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "File %s/%s, PK_File %d - midmd5 is %s", 
			m_sDirectory.c_str(), m_sFile.c_str(), m_pPlutoMediaAttributes->m_nFileID, sMidMd5.c_str());
#endif
		pRow_File->MD5_set(sMidMd5);
		pRow_File->Table_File_get()->Commit();
	}
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::SetFileAttribute(int PK_File)
{
#ifdef UPDATEMEDIA_STATUS
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "SetFileAttribute %s/%s %d", m_sDirectory.c_str(), m_sFile.c_str(), PK_File);
#endif
	//make sure it's our installation and file
	m_pPlutoMediaAttributes->m_nInstallationID = m_nOurInstallationID;
	m_pPlutoMediaAttributes->m_nFileID = PK_File;

	LoadEverythingFromDb();
}
//-----------------------------------------------------------------------------------------------------
int PlutoMediaFile::GetFileAttribute()
{
	if(m_pPlutoMediaAttributes->m_nInstallationID == m_nOurInstallationID && m_pPlutoMediaAttributes->m_nFileID != 0)
	{
		//same installation, same file; however, it's a good record?
		Row_File *pRow_File = m_pDatabase_pluto_media->File_get()->GetRow(m_pPlutoMediaAttributes->m_nFileID);
		if(NULL != pRow_File && pRow_File->Filename_get() == m_sFile)
			return pRow_File->PK_File_get();
#ifdef UPDATEMEDIA_STATUS
		else

			LoggerWrapper::GetInstance()->Write(LV_STATUS, "GetFileAttribute %s/%s installation %d/%d id %d doesn't match database %s", 
				m_sDirectory.c_str(), m_sFile.c_str(),
				m_pPlutoMediaAttributes->m_nInstallationID, m_nOurInstallationID, m_pPlutoMediaAttributes->m_nFileID,
				pRow_File ? pRow_File->Filename_get().c_str() : "*NULL*");
#endif
	}

#ifdef UPDATEMEDIA_STATUS
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "GetFileAttribute %s/%s not found installation %d/%d id %d", 
		m_sDirectory.c_str(), m_sFile.c_str(),
		m_pPlutoMediaAttributes->m_nInstallationID, m_nOurInstallationID, m_pPlutoMediaAttributes->m_nFileID);
#endif
    return 0;
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::SetPicAttribute(int PK_Picture, string sPictureUrl)
{
#ifdef UPDATEMEDIA_STATUS
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "SetPicAttribute %s/%s PK_Picture %d", m_sDirectory.c_str(), m_sFile.c_str(),
		PK_Picture);
#endif
	//set the right picture id and picture url
	m_pPlutoMediaAttributes->m_nPictureID = PK_Picture;
	m_pPlutoMediaAttributes->m_sPictureUrl = sPictureUrl;
}
//-----------------------------------------------------------------------------------------------------
int PlutoMediaFile::GetFileIDFromDB()
{
	vector<Row_File *> vectRow_File;
	m_pDatabase_pluto_media->File_get()->GetRows("Path='" + StringUtils::SQLEscape(m_sDirectory) + 
		"' AND Filename='" + StringUtils::SQLEscape(m_sFile) + "' AND Missing = 0", &vectRow_File);

	if(!vectRow_File.empty())
		return vectRow_File[0]->PK_File_get();
	else
		return 0;  // Can do nothing.  This isn't in the database
}
//-----------------------------------------------------------------------------------------------------
int PlutoMediaFile::GetPicAttribute(int PK_File)
{
	//got the file in the database?
    if(!PK_File)
	{
		PK_File = GetFileIDFromDB();
		m_pPlutoMediaAttributes->m_nFileID = PK_File;
	}

	if(PK_File == 0)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Got to add picture to file, but it's not in the db %s/%s!",
			m_sDirectory.c_str(), m_sFile.c_str());
		return 0;
	}

#ifdef UPDATEMEDIA_STATUS
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "# GetPicAttribute: file %d", PK_File);
#endif
	//got any picture associated with the file?
    vector<Row_Picture_File *> vectPicture_File;
    m_pDatabase_pluto_media->Picture_File_get()->GetRows("FK_File=" + StringUtils::itos(PK_File),&vectPicture_File);
#ifdef UPDATEMEDIA_STATUS
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Found %d pics for file", vectPicture_File.size());
#endif
    if( vectPicture_File.size() )
    {
		long PK_Picture = vectPicture_File[0]->FK_Picture_get();
		Row_Picture *pRow_Picture = m_pDatabase_pluto_media->Picture_get()->GetRow(PK_Picture);
		string sPictureUrl = NULL != pRow_Picture ? pRow_Picture->URL_get() : "";

#ifdef UPDATEMEDIA_STATUS
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "# GetPicAttribute: got picture %d associated to file %d", PK_Picture, PK_File);
#endif
		SetPicAttribute(PK_Picture, sPictureUrl);
		return PK_Picture;  // The first pic for this directory
    }

    //Does one of the attributes have a picture?
    vector<Row_Picture_Attribute *> vectPicture_Attribute;
    m_pDatabase_pluto_media->Picture_Attribute_get()->GetRows(
        "JOIN File_Attribute ON Picture_Attribute.FK_Attribute=File_Attribute.FK_Attribute "
        " JOIN Attribute ON Picture_Attribute.FK_Attribute=Attribute.PK_Attribute "
        " JOIN AttributeType ON Attribute.FK_AttributeType=AttributeType.PK_AttributeType "
        " WHERE FK_File=" + 
        StringUtils::itos(PK_File) +
        " ORDER BY `PicPriority`",&vectPicture_Attribute);

#ifdef UPDATEMEDIA_STATUS
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Found %d pics for attribute", (int) vectPicture_Attribute.size());
#endif
    if( vectPicture_Attribute.size())
    {
		long PK_Picture = vectPicture_Attribute[0]->FK_Picture_get();
		Row_Picture *pRow_Picture = m_pDatabase_pluto_media->Picture_get()->GetRow(PK_Picture);
		string sPictureUrl = NULL != pRow_Picture ? pRow_Picture->URL_get() : "";
#ifdef UPDATEMEDIA_STATUS
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "# GetPicAttribute: got picture %d associated to attribute %d", PK_Picture, 
			vectPicture_Attribute[0]->FK_Attribute_get());
#endif
        SetPicAttribute(PK_Picture, sPictureUrl);
        return PK_Picture;  // The first pic for this directory
    }

	if(m_pPlutoMediaAttributes->m_mapCoverarts.empty())
	{
		if(m_pPlutoMediaAttributes->m_sPictureUrl != "")
		{
#ifdef UPDATEMEDIA_STATUS
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "# GetPicAttribute: got no picture in the db, but got picture URL %s",
				m_pPlutoMediaAttributes->m_sPictureUrl.c_str());
#endif
			//It's a "new" file, but we know the picture url
			//we'll download the picture and record in Picture table
			MediaAttributes_LowLevel mediaAttributes_LowLevel(m_pDatabase_pluto_media);
			Row_Picture *pRow_Picture = mediaAttributes_LowLevel.AddPicture(NULL, 0, 
				FileUtils::FindExtension(m_pPlutoMediaAttributes->m_sPictureUrl), 
				m_pPlutoMediaAttributes->m_sPictureUrl
			);

			if(NULL != pRow_Picture)
			{
				m_pPlutoMediaAttributes->m_nPictureID = pRow_Picture->PK_Picture_get();

				Row_Picture_File *pRow_Picture_File = m_pDatabase_pluto_media->Picture_File_get()->AddRow();
				pRow_Picture_File->FK_File_set(PK_File);
				pRow_Picture_File->FK_Picture_set(pRow_Picture->PK_Picture_get());
				pRow_Picture_File->Table_Picture_File_get()->Commit();

#ifdef UPDATEMEDIA_STATUS
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "PlutoMediaFile::GetPicAttribute Added picture to file: PK_File %d, PK_Picture %d",
					PK_File, m_pPlutoMediaAttributes->m_nPictureID);

#endif
				return m_pPlutoMediaAttributes->m_nPictureID;
			}
			else
			{
#ifdef UPDATEMEDIA_STATUS
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "Failed to add picture to file: PK_File %d, picture url: %s",
					PK_File, m_pPlutoMediaAttributes->m_sPictureUrl.c_str());
#endif
			}
		}
	}

#ifdef UPDATEMEDIA_STATUS
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "# GetPicAttribute: got no picture for file %d", PK_File);
#endif
    return 0;
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::SavePlutoAttributes()
{
	if(m_MediaSyncMode == modeFileToDb)
	{
#ifdef UPDATEMEDIA_STATUS
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "# NOT saving attributes in attribute files. Read only mode!");
#endif
	}
	else
	{
		GetPicAttribute(m_pPlutoMediaAttributes->m_nFileID);
		m_spFileHandler->SaveAttributes(m_pPlutoMediaAttributes);
	}
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::CacheDVDKeys()
{
	string sKeysArchiveFile = m_sDirectory + "/" + m_sFile + ".keys.tar.gz";

	if(FileUtils::FileExists(sKeysArchiveFile))
	{
		if(!FileUtils::DirExists(m_sDVDKeysCache))
			FileUtils::MakeDir(m_sDVDKeysCache);

		string sCMD = "/bin/tar zx -C \"" + m_sDVDKeysCache + "\" -f \"" + sKeysArchiveFile + "\"";
		int nResult = 0;
		if((nResult = system(sCMD.c_str())) != 0)
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed to cache dvd keys: %s, result %d", 
				sCMD.c_str(), nResult);
		}
		else
		{
#ifdef UPDATEMEDIA_STATUS
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Successfuly cached dvd keys from %s to %s", 
				sKeysArchiveFile.c_str(), m_sDVDKeysCache.c_str());
#endif
		}
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Couldn't find %s file for this dvd",
			sKeysArchiveFile.c_str());
	}
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::LoadPlutoAttributes()
{
	m_pPlutoMediaAttributes = new PlutoMediaAttributes();
	m_spFileHandler->LoadAttributes(m_pPlutoMediaAttributes, m_listPicturesForTags);

	//merge the pictures from external tags with those from our tags
	MergePictures();
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::MergePictures()
{
#ifdef UPDATEMEDIA_STATUS
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "MergePictures: merging %d pictures from our tag with %d pictures from PIC tags",
		m_pPlutoMediaAttributes->m_mapCoverarts.size(), m_listPicturesForTags.size());
#endif
	//get the list with md5sums for coverarts and also remove the duplicates
	list<string> listMD5SumsCoverarts;
	m_pPlutoMediaAttributes->GenerateMd5SumsForCoverarts(listMD5SumsCoverarts, true);

	for(list<pair<char *, size_t> >::iterator itp = m_listPicturesForTags.begin(); itp != m_listPicturesForTags.end(); ++itp)
	{
		char *pPictureData = itp->first;
		size_t nPictureSize = itp->second;
		string sMd5Sum = FileUtils::FileChecksum(pPictureData, nPictureSize);

		if(listMD5SumsCoverarts.end() != std::find(listMD5SumsCoverarts.begin(), listMD5SumsCoverarts.end(), sMd5Sum))
		{
			delete [] pPictureData;
		}
		else
		{
#ifdef UPDATEMEDIA_STATUS
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "LoadPlutoAttributes: loading picture with size %d from PIC tag - md5sum %s",
				nPictureSize, sMd5Sum.c_str());
#endif
			m_pPlutoMediaAttributes->m_mapCoverarts.insert(make_pair(static_cast<unsigned long>(nPictureSize), pPictureData));
		}
	}
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::LoadEverythingFromDb()
{
	LoadStartPosition();
	LoadShortAttributes();
	LoadLongAttributes();
	LoadCoverarts();
	LoadBookmarkPictures();
	LoadMiscInfo();
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::LoadStartPosition()
{
	//load start position from the database
	vector<Row_Bookmark *> vectBookmarks;
	m_pDatabase_pluto_media->Bookmark_get()->GetRows("Description = 'START' AND IsAutoResume = 1 "
		"AND FK_File = " + StringUtils::ltos(m_pPlutoMediaAttributes->m_nFileID), &vectBookmarks);

	if(vectBookmarks.size() > 0)
	{
		Row_Bookmark *pRow_Bookmark = vectBookmarks[0];	
		m_pPlutoMediaAttributes->m_sStartPosition = pRow_Bookmark->Position_get();

#ifdef UPDATEMEDIA_STATUS
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "# LoadPlutoAttributes: read start position from db: %s",
			m_pPlutoMediaAttributes->m_sStartPosition.c_str());
#endif
	}
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::LoadShortAttributes()
{
	PlutoSqlResult result;
	DB_ROW row;
	string SQL = 
		"SELECT PK_Attribute,FK_AttributeType, Name, Track, Section "
		"FROM Attribute JOIN File_Attribute ON PK_Attribute = FK_Attribute "
		"WHERE FK_File = " + StringUtils::ltos(m_pPlutoMediaAttributes->m_nFileID);

	MapPlutoMediaAttributes mapDBAttributes;

	if((result.r = m_pDatabase_pluto_media->db_wrapper_query_result(SQL)))
	{
		while((row = db_wrapper_fetch_row(result.r)) && NULL != row[0] && NULL != row[1])
		{
			int nPK_Attribute = atoi(row[0]);
			int nFK_AttributeType = atoi(row[1]);
			string sName = row[2];
			int nTrack = NULL != row[3] ? atoi(row[3]) : 0;
			int nSection = NULL != row[4] ? atoi(row[4]) : 0;

#ifdef UPDATEMEDIA_STATUS
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Attr in db : type %d\t\tvalue %s", nFK_AttributeType, sName.c_str());
#endif
			bool bAttributeAlreadyAdded = false;

			if(m_MediaSyncMode == modeDbToFile) 
			{
				mapDBAttributes.insert(std::make_pair(nFK_AttributeType, 
					new PlutoMediaAttribute(nPK_Attribute,nFK_AttributeType, sName, nTrack, nSection)));
			}

			for(MapPlutoMediaAttributes::iterator it = m_pPlutoMediaAttributes->m_mapAttributes.begin(),
				end = m_pPlutoMediaAttributes->m_mapAttributes.end(); it != end; ++it)
			{
				PlutoMediaAttribute *pPlutoMediaAttribute = it->second;
				if(
					pPlutoMediaAttribute->m_nType == nFK_AttributeType	&&
					pPlutoMediaAttribute->m_sName == sName				&&
					pPlutoMediaAttribute->m_nTrack == nTrack			&&
					pPlutoMediaAttribute->m_nSection == nSection
					)
				{
					bAttributeAlreadyAdded = true;
					break;
				}
			}

			if(!bAttributeAlreadyAdded)
			{
#ifdef UPDATEMEDIA_STATUS
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "# LoadPlutoAttributes: (from db) new attributes to add in attribute file -  type %d, values %s",
					nFK_AttributeType, sName.c_str());
#endif
				m_pPlutoMediaAttributes->m_mapAttributes.insert(
					std::make_pair(
						nFK_AttributeType, 
						new PlutoMediaAttribute(nPK_Attribute, nFK_AttributeType, sName, nTrack, nSection)
					)
				);
			}
		}
	}

	if(m_MediaSyncMode == modeDbToFile)
	{
		//we need to check here if any attribute was deleted from website in order to delete it from 
		//file's attribute too.
		for(MapPlutoMediaAttributes::iterator it = m_pPlutoMediaAttributes->m_mapAttributes.begin(),
			end = m_pPlutoMediaAttributes->m_mapAttributes.end(); it != end;)
		{
			PlutoMediaAttribute *pPlutoMediaAttribute_File = it->second;
			bool bAttributeFoundInDB = false;

#ifdef UPDATEMEDIA_STATUS
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Attr in file : type %d\t\tvalue %s", 
				pPlutoMediaAttribute_File->m_nType, pPlutoMediaAttribute_File->m_sName.c_str());
#endif
			for(MapPlutoMediaAttributes::iterator it_DbAttr = mapDBAttributes.begin(), end_DbAttr = mapDBAttributes.end(); 
				it_DbAttr != end_DbAttr; ++it_DbAttr)
			{
				PlutoMediaAttribute *pPlutoMediaAttribute_DB = it_DbAttr->second;

				if(*pPlutoMediaAttribute_File == *pPlutoMediaAttribute_DB)
				{
					bAttributeFoundInDB = true;
					break;
				}
			}

			if(!bAttributeFoundInDB)
			{
				m_spFileHandler->RemoveAttribute(pPlutoMediaAttribute_File->m_nType, pPlutoMediaAttribute_File->m_sName, m_pPlutoMediaAttributes);
				m_pPlutoMediaAttributes->m_mapAttributes.erase(it++);
			}
			else
				++it;
		}

#ifdef UPDATEMEDIA_STATUS
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "After sync'ing file %d, attr in db: %d, attr in file: %d", 
			m_pPlutoMediaAttributes->m_nFileID, mapDBAttributes.size(), m_pPlutoMediaAttributes->m_mapAttributes.size());
#endif
		//cleanup
		for(MapPlutoMediaAttributes::iterator it = mapDBAttributes.begin(), end = mapDBAttributes.end(); it != end; ++it)
		{
			PlutoMediaAttribute *pPlutoMediaAttribute = it->second;
			delete pPlutoMediaAttribute;
		}
		mapDBAttributes.clear();
	}
#ifdef UPDATEMEDIA_STATUS
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "# LoadPlutoAttributes: pluto attributes merged with those from database %d", 
		m_pPlutoMediaAttributes->m_mapAttributes.size());
#endif
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::LoadLongAttributes()
{
	PlutoSqlResult result;
	DB_ROW row;
	string SQL = 
		"SELECT FK_AttributeType, Text "
		"FROM LongAttribute "
		"WHERE FK_File = " + StringUtils::ltos(m_pPlutoMediaAttributes->m_nFileID);

	MapPlutoMediaAttributes mapDbLongAttributes;

	if((result.r = m_pDatabase_pluto_media->db_wrapper_query_result(SQL)))
	{
		while((row = db_wrapper_fetch_row(result.r)) && NULL != row[0] && NULL != row[1])
		{
			int nFK_AttributeType = atoi(row[0]);
			string sName = row[1];

#ifdef UPDATEMEDIA_STATUS
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Long attribute in db : type %d\t\tvalue %s", nFK_AttributeType, AdjustLongAttributeForDisplay(sName).c_str());
#endif
			bool bAttributeAlreadyAdded = false;
			if(m_MediaSyncMode == modeDbToFile) 
				mapDbLongAttributes.insert(std::make_pair(nFK_AttributeType, new PlutoMediaAttribute(0, nFK_AttributeType, sName)));

			for(MapPlutoMediaAttributes::iterator it = m_pPlutoMediaAttributes->m_mapLongAttributes.begin(),
				end = m_pPlutoMediaAttributes->m_mapLongAttributes.end(); it != end; ++it)
			{
				PlutoMediaAttribute *pPlutoMediaAttribute = it->second;
				if(pPlutoMediaAttribute->m_nType == nFK_AttributeType && pPlutoMediaAttribute->m_sName == sName)
				{
					bAttributeAlreadyAdded = true;
					break;
				}
			}

			if(!bAttributeAlreadyAdded)
			{
#ifdef UPDATEMEDIA_STATUS
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "# LoadPlutoAttributes: (from db) new long attributes to add in attribute file -  type %d, values %s",
					nFK_AttributeType, AdjustLongAttributeForDisplay(sName).c_str());
#endif
				m_pPlutoMediaAttributes->m_mapLongAttributes.insert(
					std::make_pair(nFK_AttributeType, new PlutoMediaAttribute(0, nFK_AttributeType, sName))
				);
			}
		}
	}

	if(m_MediaSyncMode == modeDbToFile)
	{
		//we need to check here if any long attribute was deleted from website in order to delete it from 
		//file's attribute too.
		for(MapPlutoMediaAttributes::iterator it = m_pPlutoMediaAttributes->m_mapLongAttributes.begin(),
			end = m_pPlutoMediaAttributes->m_mapLongAttributes.end(); it != end;)
		{
			PlutoMediaAttribute *pPlutoMediaAttribute_File = it->second;
			bool bAttributeFoundInDB = false;

#ifdef UPDATEMEDIA_STATUS
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Long attr in file : type %d\t\tvalue %s", 
				pPlutoMediaAttribute_File->m_nType, AdjustLongAttributeForDisplay(pPlutoMediaAttribute_File->m_sName).c_str());
#endif
			for(MapPlutoMediaAttributes::iterator it_DbAttr = mapDbLongAttributes.begin(), end_DbAttr = mapDbLongAttributes.end(); 
				it_DbAttr != end_DbAttr; ++it_DbAttr)
			{
				PlutoMediaAttribute *pPlutoMediaAttribute_DB = it_DbAttr->second;

				if(*pPlutoMediaAttribute_File == *pPlutoMediaAttribute_DB)
				{
					bAttributeFoundInDB = true;
					break;
				}
			}

			if(!bAttributeFoundInDB)
			{
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "Removing attribute from file %d, value %s, since it was remove from the site.",
					pPlutoMediaAttribute_File->m_nType, AdjustLongAttributeForDisplay(pPlutoMediaAttribute_File->m_sName).c_str());

				m_spFileHandler->RemoveAttribute(pPlutoMediaAttribute_File->m_nType, pPlutoMediaAttribute_File->m_sName, m_pPlutoMediaAttributes);
				m_pPlutoMediaAttributes->m_mapLongAttributes.erase(it++);
			}
			else
				++it;
		}

#ifdef UPDATEMEDIA_STATUS
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "After sync'ing file %d, long attr in db: %d, long attr in file: %d", 
			m_pPlutoMediaAttributes->m_nFileID, mapDbLongAttributes.size(), m_pPlutoMediaAttributes->m_mapLongAttributes.size());
#endif
		//cleanup
		for(MapPlutoMediaAttributes::iterator it = mapDbLongAttributes.begin(), end = mapDbLongAttributes.end(); it != end; ++it)
		{
			PlutoMediaAttribute *pPlutoMediaAttribute = it->second;
			delete pPlutoMediaAttribute;
		}
		mapDbLongAttributes.clear();
	}

#ifdef UPDATEMEDIA_STATUS
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "# LoadPlutoAttributes: pluto long attributes merged with those from database %d", 
		m_pPlutoMediaAttributes->m_mapLongAttributes.size());
#endif
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::LoadBookmarkPictures()
{
	int nNumBookmarksFromDB = NumberOfBookmarksFromDB();

	//keep them in sync. clean bookmarks from file and use those from db if it's not a new file
	if(!m_bNewFileToDb)
	{
#ifdef UPDATEMEDIA_STATUS
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "LoadBookmarkPictures: clear bookmarks from file."
			"We'll use those from db: %d", nNumBookmarksFromDB);
		m_pPlutoMediaAttributes->ClearBookmarks();
#endif
	}

	PlutoSqlResult result;
	DB_ROW row;
	string SQL = 
		"SELECT FK_Picture, Description, Position FROM Bookmark "
		"WHERE FK_Picture IS NOT NULL AND "
		"FK_File = " + StringUtils::ltos(m_pPlutoMediaAttributes->m_nFileID);

	if((result.r = m_pDatabase_pluto_media->db_wrapper_query_result(SQL)))
	{
		int nCounter = 0;
		while((row = db_wrapper_fetch_row(result.r)) && NULL != row[0] && NULL != row[1] && NULL != row[2])
		{
			int nFK_Picture = atoi(row[0]);
			string sDescription(row[1]);
			string sPosition(row[2]);

			//skip start position
			if(sDescription == "START")
				continue;
#ifdef UPDATEMEDIA_STATUS
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "LoadBookmarkPictures: Adding bookmark desc %s, pos %s", sDescription.c_str(),
				sPosition.c_str());
#endif
			pair<unsigned long, char *> picture_data = LoadPicture(nFK_Picture);
			pair<unsigned long, char *> thumb_picture_data = LoadPicture(nFK_Picture, true);

			PlutoMediaBookmark *pBookmark = new PlutoMediaBookmark(sDescription, sPosition);
			pBookmark->m_dataPicture.m_dwSize = picture_data.first;
			pBookmark->m_dataPicture.m_pBlock = picture_data.second;
			pBookmark->m_dataPictureThumb.m_dwSize = thumb_picture_data.first;
			pBookmark->m_dataPictureThumb.m_pBlock = thumb_picture_data.second;

			m_pPlutoMediaAttributes->m_listBookmarks.push_back(pBookmark);

			if(++nCounter > MAX_PICTURES)
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "LoadBookmarkPictures : too many bookmarks...");
				break;
			}
		}
	}
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::LoadMiscInfo()
{
	PlutoSqlResult result;
	DB_ROW row;
	string SQL = 
		"SELECT FK_MediaSubType, FK_FileFormat FROM File WHERE "
		"PK_File = " + StringUtils::ltos(m_pPlutoMediaAttributes->m_nFileID);

	if((result.r = m_pDatabase_pluto_media->db_wrapper_query_result(SQL)))
	{
		while((row = db_wrapper_fetch_row(result.r)) && NULL != row[0] && NULL != row[1])
		{
			m_pPlutoMediaAttributes->m_nMediaSubType = atoi(row[0]);
			m_pPlutoMediaAttributes->m_nFileFormat = atoi(row[1]);
		}
	}

#ifdef UPDATEMEDIA_STATUS
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "# SaveMiscInfo: FK_FileFormat %d, FK_MediaSubType %d", 
		m_pPlutoMediaAttributes->m_nFileFormat, m_pPlutoMediaAttributes->m_nMediaSubType);
#endif
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::LoadCoverarts()
{
	//get the list with md5sums for coverarts and also remove the duplicates
	list<string> listMD5SumsCoverarts;
	m_pPlutoMediaAttributes->GenerateMd5SumsForCoverarts(listMD5SumsCoverarts, true);

#ifdef UPDATEMEDIA_STATUS
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "LoadCoverarts: loaded %d unique pictures from file",
		listMD5SumsCoverarts.size());
#endif
	if(m_MediaSyncMode == modeDbToFile)
	{
#ifdef UPDATEMEDIA_STATUS
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "LoadCoverarts: only the db has changed, so we'll purge coverarts from file and use only those from db");
#endif
		m_pPlutoMediaAttributes->ClearCoverarts();
		listMD5SumsCoverarts.clear();
	}

	PlutoSqlResult result;
	DB_ROW row;
	string SQL = 
		"SELECT FK_Picture FROM Picture_File "
		"WHERE FK_File = " + StringUtils::ltos(m_pPlutoMediaAttributes->m_nFileID);

	if((result.r = m_pDatabase_pluto_media->db_wrapper_query_result(SQL)))
	{
		while((row = db_wrapper_fetch_row(result.r)) && NULL != row[0])
		{
			int nFK_Picture = atoi(row[0]);

			pair<unsigned long, char *> pairPicture = LoadPicture(nFK_Picture);
			string sMd5Sum = FileUtils::FileChecksum(pairPicture.second, pairPicture.first);

			//picture already in file attributes?
			if(listMD5SumsCoverarts.end() != std::find(listMD5SumsCoverarts.begin(), listMD5SumsCoverarts.end(), sMd5Sum))
			{
#ifdef UPDATEMEDIA_STATUS
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "LoadCoverarts: not loading picture with size %d from database, "
					"because we already have it in file - md5sum %s", 
					pairPicture.first, sMd5Sum.c_str());
#endif
				delete [] pairPicture.second;
			}
			else
			{
				//we don't have it, add it to list
				listMD5SumsCoverarts.push_back(sMd5Sum);

#ifdef UPDATEMEDIA_STATUS
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "LoadCoverarts: Adding coverart, picture id %d, "
					"size %d, md5sum %s", nFK_Picture, pairPicture.first, sMd5Sum.c_str());

#endif
				m_pPlutoMediaAttributes->m_mapCoverarts.insert(pairPicture);
			}
		}
	}
}
//-----------------------------------------------------------------------------------------------------
pair<unsigned long, char *> PlutoMediaFile::LoadPicture(int nPictureId, bool bThumb/* = false*/)
{
	string sPictureFile = "/home/mediapics/" + StringUtils::ltos(nPictureId) + (bThumb ? "_tn" : "") + ".jpg";

	size_t nSize = 0;
	char *pData = FileUtils::ReadFileIntoBuffer(sPictureFile, nSize);
	return make_pair(static_cast<unsigned long>(nSize), pData);
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::SavePicture(pair<unsigned long, char *> pairPicture, int nPictureId, bool bThumb/* = false*/)
{
	string sPictureFile = "/home/mediapics/" + StringUtils::ltos(nPictureId) + (bThumb ? "_tn" : "") + ".jpg";

	if(bThumb)
	{
		string sMd5Sum = FileUtils::FileChecksum(pairPicture.second, pairPicture.first);

		//(re)gen thumb
		string sTempPictureName = "picture-" + sMd5Sum + ".jpg";
		string sTempPictureNameThumb = "picture-" + sMd5Sum + "_tn.jpg";

		if(FileUtils::WriteBufferIntoFile(sTempPictureName, pairPicture.second, pairPicture.first))
		{
			string Cmd = "convert -scale 256x256 -antialias " + sTempPictureName + " " + sTempPictureNameThumb;
			int result;
			if((result = system(Cmd.c_str())) != 0)
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Thumbnail picture %s returned %d", Cmd.c_str(), result);
			}
			else
			{
				size_t nPictureThumbSize = 0;
				char *pPictureThumbData = FileUtils::ReadFileIntoBuffer(sTempPictureNameThumb, nPictureThumbSize);

				if(NULL != pPictureThumbData && nPictureThumbSize > 0)
				{
#ifdef UPDATEMEDIA_STATUS
					LoggerWrapper::GetInstance()->Write(LV_STATUS, "Thumbnail picture created with %s and returned %d", Cmd.c_str(), result);
#endif
					FileUtils::WriteBufferIntoFile(sPictureFile, pPictureThumbData, nPictureThumbSize);

					FileUtils::DelFile(sTempPictureName);
					FileUtils::DelFile(sTempPictureNameThumb);

					delete [] pPictureThumbData;
				}
				else
				{
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Cannot read image from disk: %s", sTempPictureNameThumb.c_str());
				}				
			}
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Cannot save image to disk: %s", sTempPictureName.c_str());
		}
	}
	else
	{
		FileUtils::WriteBufferIntoFile(sPictureFile, pairPicture.second, pairPicture.first);
	}
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::RenameAttribute(int Attribute_Type, string sOldValue, string sNewValue)
{
#ifdef UPDATEMEDIA_STATUS
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "# RenameAttribute : need to rename attribute type %d, value %s with %s", 
		Attribute_Type, sOldValue.c_str(), sNewValue.c_str());
#endif
	for(MapPlutoMediaAttributes::iterator it = m_pPlutoMediaAttributes->m_mapAttributes.begin(), 
		end = m_pPlutoMediaAttributes->m_mapAttributes.end(); it != end; ++it)
	{
		if(it->first == Attribute_Type && it->second->m_sName == sOldValue)
		{
			it->second->m_sName = sOldValue;
			break;
		}
	}
}
//-----------------------------------------------------------------------------------------------------
int PlutoMediaFile::GetOwnerForPath(string sPath)
{
	// like this: /home/user_xxx/data

	int nEK_Users_Private = 0;
	const string csHomeUserPath = "/home/user_";

	if(sPath.find(csHomeUserPath) == 0)
	{
		string sRemainingPath = sPath.substr(csHomeUserPath.length());
		string sUserId = sRemainingPath.substr(0, sRemainingPath.find("/"));
		nEK_Users_Private = atoi(sUserId.c_str());
	}

	return nEK_Users_Private;
}
//-----------------------------------------------------------------------------------------------------
int PlutoMediaFile::NumberOfBookmarksFromDB()
{
	PlutoSqlResult result_count;
	DB_ROW row_count;
	string SQLCount = 
		"SELECT Count(*) FROM Bookmark "
		"WHERE FK_Picture IS NOT NULL AND "
		"FK_File = " + StringUtils::ltos(m_pPlutoMediaAttributes->m_nFileID);

	int nNumberOfBookmarksFromDB = 0;
    if((result_count.r = m_pDatabase_pluto_media->db_wrapper_query_result(SQLCount)) && (row_count = db_wrapper_fetch_row(result_count.r)) && NULL != row_count[0])
		nNumberOfBookmarksFromDB = atoi(row_count[0]);

	return nNumberOfBookmarksFromDB;
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::GenerateMd5SumsCoverartsFromDb(list<string>& listMd5Sums)
{
	vector<Row_Picture_File *> vectPictures;
	m_pDatabase_pluto_media->Picture_File_get()->GetRows(
		"FK_File = " + StringUtils::ltos(m_pPlutoMediaAttributes->m_nFileID), 
		&vectPictures);

	for(vector<Row_Picture_File *>::iterator it = vectPictures.begin(); it != vectPictures.end(); ++it)
	{
		Row_Picture_File *pRow_Picture_File = *it;

		string sPictureFile = "/home/mediapics/" + StringUtils::ltos(pRow_Picture_File->FK_Picture_get()) + ".jpg";
		
		if(FileUtils::FileExists(sPictureFile))
		{
			string sMd5Sum = FileUtils::FileChecksum(sPictureFile);
			listMd5Sums.push_back(sMd5Sum);
		}
	}
}
//-----------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------
//
//  PlutoMediaIdentifier class
//
//-----------------------------------------------------------------------------------------------------
map<string,int> PlutoMediaIdentifier::m_mapExtensions;
//-----------------------------------------------------------------------------------------------------
/*static*/ void PlutoMediaIdentifier::Activate(Database_pluto_main *pDatabase_pluto_main)
{
	if(m_mapExtensions.size()) //activate once
		return;

#ifdef UPDATEMEDIA_STATUS
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Activating Pluto Media Identifier...");
#endif
    vector<Row_MediaType *> vectRow_MediaType;
    pDatabase_pluto_main->MediaType_get()->GetRows("1=1",&vectRow_MediaType);
    for(size_t s=0;s<vectRow_MediaType.size();++s)
    {
        Row_MediaType *pRow_MediaType = vectRow_MediaType[s];
        string::size_type pos=0;
        string sExtensions = pRow_MediaType->Extensions_get();
        while(pos < sExtensions.size() )
			m_mapExtensions[StringUtils::ToLower(StringUtils::Tokenize(sExtensions,",",pos))] = pRow_MediaType->PK_MediaType_get();
    }

    vector<Row_DeviceTemplate_MediaType *> vectRow_DeviceTemplate_MediaType;
    pDatabase_pluto_main->DeviceTemplate_MediaType_get()->GetRows("1=1",&vectRow_DeviceTemplate_MediaType);
    for(size_t s=0;s<vectRow_DeviceTemplate_MediaType.size();++s)
    {
        Row_DeviceTemplate_MediaType *pRow_DeviceTemplate_MediaType = vectRow_DeviceTemplate_MediaType[s];
        string::size_type pos=0;
        string sExtensions = pRow_DeviceTemplate_MediaType->Extensions_get();
        while(pos < sExtensions.size() )
            m_mapExtensions[StringUtils::ToLower(StringUtils::Tokenize(sExtensions,",",pos))] = pRow_DeviceTemplate_MediaType->FK_MediaType_get();
    }

#ifdef UPDATEMEDIA_STATUS
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Pluto Media Identifier activated. Extensions %d", m_mapExtensions.size());
#endif
}
//-----------------------------------------------------------------------------------------------------
/*static*/ string PlutoMediaIdentifier::GetExtensions()
{
	string sResult;
	for(map<string,int>::iterator it = m_mapExtensions.begin(); it != m_mapExtensions.end(); ++it)
		sResult += "*." + it->first + "\t";

	return sResult;
}
//-----------------------------------------------------------------------------------------------------
/*static*/ int PlutoMediaIdentifier::Identify(string sFilename)
{
    //we'll try first to identify the media based on the extension
    //if we don't find any extention to match it, we'll use MediaIdentifier which uses the magic from files
	string sExtension = FileUtils::FindExtension(sFilename);

	if(sExtension.empty() && (sFilename != "info") )
		return 0;

    map<string, int>::iterator it = m_mapExtensions.find(StringUtils::ToLower(sExtension));
    return it != m_mapExtensions.end() ? it->second : 0 /*MediaIdentifier::GetFileMediaType(sFilename)*/;
}
//-----------------------------------------------------------------------------------------------------
