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
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "Logger.h"
#include "UpdateMedia.h"
#include "MediaState.h"
#include "FileUtils/file_utils.h"
#include "FileHandlerFactory.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <list>

#ifndef WIN32
#include <dirent.h>
#include <attr/attributes.h>
#endif

#include "pluto_main/Table_MediaType.h"
#include "pluto_main/Table_DeviceTemplate_MediaType.h"

#include "pluto_media/Table_File.h"
#include "pluto_media/Table_Picture_File.h"
#include "pluto_media/Table_Attribute.h"
#include "pluto_media/Table_Picture_Attribute.h"
#include "pluto_media/Table_File_Attribute.h"

#include "PlutoMediaFile.h"
#include "pluto_main/Table_Installation.h"
#include "pluto_main/Table_Device_DeviceData.h"
#include "pluto_main/Define_DeviceData.h"

#ifdef WIN32
#include <direct.h>
#include <conio.h>
#define chdir _chdir  // Why, Microsoft, why?
#define mkdir _mkdir  // Why, Microsoft, why?
#define stat64 _stat64
#else
#endif

#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

#include "FileStatusObserver.h"

#include "../include/version.h"
//#define  USE_DEVEL_DATABASES 

using namespace std;
using namespace DCE;

#ifdef WIN32
#include <signal.h>
#endif

namespace UpdateMediaSig
{
	static bool bSignalTrapCaught = false;
};

namespace UpdateMediaVars
{
	extern string sUPnPMountPoint;
	extern string sLocalUPnPServerName;
};

#include "DCE/DCEConfig.h"

void sigtrap_hook(int sig)
{
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Signal %d caught! Exiting...",  sig);
	UpdateMediaSig::bSignalTrapCaught = true;
}

UpdateMedia::UpdateMedia(string host, string user, string pass, int port,string sDirectory, bool bSyncFilesOnly)
{
	PlutoMediaFile::SetDefaultSyncMode(bSyncFilesOnly ? modeDbToFile : modeBoth);

#ifndef WIN32
	signal(SIGTRAP, &sigtrap_hook);
#endif

	ReadConfigFile();

	string sPlutoMediaDbName = "pluto_media";
	string sPlutoMainDbName = "pluto_main";

#ifdef USE_DEVEL_DATABASES
	sPlutoMediaDbName = "pluto_media_devel";
	sPlutoMainDbName = "pluto_main_devel";
#endif

	//connect to the databases
	m_pDatabase_pluto_media = new Database_pluto_media(LoggerWrapper::GetInstance());
	if( !m_pDatabase_pluto_media->Connect( host, user, pass, sPlutoMediaDbName, port ) )
	{
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Cannot connect to database!" );
		return;
	}

	m_pDatabase_pluto_main = new Database_pluto_main(LoggerWrapper::GetInstance());
	if( !m_pDatabase_pluto_main->Connect( host, user, pass, sPlutoMainDbName, port ) )
	{
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Cannot connect to database!" );
		return;
	}

	m_bAsDaemon = false;
	PlutoMediaIdentifier::Activate(m_pDatabase_pluto_main);
	LoadExtensions();

	m_sDirectory = FileUtils::ExcludeTrailingSlash(sDirectory);

	SetupInstallation();

	//load info about ModificationData, AttrCount, AttrDate, attributes, timestamp for all files
	MediaState::Instance().LoadDbInfo(m_pDatabase_pluto_media, FileUtils::ExcludeTrailingSlash(m_sDirectory));
}

UpdateMedia::UpdateMedia(Database_pluto_media *pDatabase_pluto_media, 
						 Database_pluto_main *pDatabase_pluto_main, string sDirectory) 
{
#ifndef WIN32
	signal(SIGTRAP, &sigtrap_hook);
#endif

	ReadConfigFile();

	//reusing connections
	m_pDatabase_pluto_main = pDatabase_pluto_main;
	m_pDatabase_pluto_media = pDatabase_pluto_media;

	SetupInstallation();

	m_bAsDaemon = true;
	m_sDirectory = FileUtils::ExcludeTrailingSlash(sDirectory);
}

UpdateMedia::~UpdateMedia()
{
	if(!m_bAsDaemon)
	{
		delete m_pDatabase_pluto_media;
		delete m_pDatabase_pluto_main;
	}
}

void UpdateMedia::ReadConfigFile()
{
	ifstream fUpdateMediaConfig;
	string sConfFileValue;
	string::size_type sPosBeginIdx, sPosEndIdx;
	string sConfVar, sConfVal;

	sPosBeginIdx=0;
	sPosEndIdx=0;

	fUpdateMediaConfig.open("/etc/UpdateMedia.conf");
	if(fUpdateMediaConfig.is_open())
	{
		while(!fUpdateMediaConfig.eof())
		{
			getline(fUpdateMediaConfig, sConfFileValue);
			if(sConfFileValue.empty());
			else
			{
				sPosEndIdx=sConfFileValue.find_first_of("=");
				sConfVar=sConfFileValue.substr(sPosBeginIdx, sPosEndIdx);
				sPosBeginIdx = sPosEndIdx + 1;
				sPosEndIdx=sConfFileValue.length();
				sConfVal=sConfFileValue.substr(sPosBeginIdx, sPosEndIdx);

				if(sConfVar=="SyncId3Files")
				{
					if(sConfVal=="false" || sConfVal=="0")
					{
						PlutoMediaFile::SetDefaultSyncMode(modeFileToDb);
					}	
				}
			}
		}
		fUpdateMediaConfig.close();
	}
}

void UpdateMedia::SetupInstallation()
{
	m_nPK_Installation = 0;
	vector<Row_Installation *> vectRow_Installation;
	m_pDatabase_pluto_main->Installation_get()->GetRows("1=1", &vectRow_Installation);
	if(vectRow_Installation.size() > 0)
		m_nPK_Installation = vectRow_Installation[0]->PK_Installation_get();
}

void UpdateMedia::LoadExtensions()
{
	m_sExtensions = PlutoMediaIdentifier::GetExtensions();
}

void UpdateMedia::DoIt()
{
	if( !m_pDatabase_pluto_media->m_bConnected )
	{
		cerr << "Cannot connect to database" << endl;
		exit(1);
	}

	ReadDirectory(m_sDirectory);
	SyncDbWithDirectory(m_sDirectory); //mark missing/not-missing files, recursively
}

int UpdateMedia::ReadDirectory(string sDirectory)
{
	if( sDirectory.size()==0 || StringUtils::StartsWith(sDirectory,"/home/public/data/samples") )
		return 0;

	// Strip any trailing /
	sDirectory = FileUtils::ExcludeTrailingSlash(sDirectory);

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "UpdateMedia::ReadDirectory %s", sDirectory.c_str());

	if(IsLockedFolder(sDirectory))
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Skipped locked folder: %s", sDirectory.c_str());
		return 0;
	}

	if(HasSpecialFolderParent(sDirectory))
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Skipped directory - is a subfolder for a special folder: %s", sDirectory.c_str());
		return 0;
	}

	if(!ScanFiles(sDirectory))
		return 0;

	FolderType folder_type = ftNormal;
	if(!ScanSubfolders(sDirectory, folder_type))
		return 0;

    return SetupDirectory(sDirectory, folder_type);
}

void UpdateMedia::UpdateSearchTokens()
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Updating search tokens...");

	// Find the naximum token
	string sSQL = "SELECT max(psc_mod) FROM Attribute";
	PlutoSqlResult result_max;
	DB_ROW row_max = NULL;
	if( (result_max.r = m_pDatabase_pluto_media->db_wrapper_query_result(sSQL)) )
		row_max = db_wrapper_fetch_row( result_max.r );
	
	string SQL = "DELETE FROM SearchToken_Attribute";
	m_pDatabase_pluto_media->threaded_db_wrapper_query(SQL);

	SQL = "SELECT PK_Attribute,Name FROM Attribute";

	PlutoSqlResult allresult,result;
	DB_ROW row,row2;
	if( ( allresult.r=m_pDatabase_pluto_media->db_wrapper_query_result( SQL ) ) )
	{
		while( ( row=db_wrapper_fetch_row( allresult.r ) ) )
		{
			if(m_bAsDaemon)
				Sleep(40);

			string sName = row[1];
			string::size_type pos=0;
			while( pos<sName.size() )
			{
				string Token=StringUtils::UpperAZ09Only( StringUtils::Tokenize( sName, " ", pos ) );
				if( Token.length( )==0 )
					continue;
				SQL = "SELECT PK_SearchToken FROM SearchToken WHERE Token='" +
					StringUtils::SQLEscape( Token ) + "'";
				int PK_SearchToken=0;
				if( ( result.r=m_pDatabase_pluto_media->db_wrapper_query_result( SQL ) ) && ( row2=db_wrapper_fetch_row( result.r ) ) )
				{
					PK_SearchToken = atoi( row2[0] );
				}
				else
				{
					SQL = "INSERT INTO SearchToken( Token ) VALUES( '" +
						StringUtils::SQLEscape( Token ) + "' )";
					PK_SearchToken = m_pDatabase_pluto_media->threaded_db_wrapper_query_withID( SQL );
				}
				if( PK_SearchToken )
				{
					SQL = "INSERT INTO SearchToken_Attribute(FK_SearchToken,FK_Attribute) VALUES( " +
						StringUtils::itos( PK_SearchToken ) + ", " +
						row[0] + " )";
					m_pDatabase_pluto_media->threaded_db_wrapper_query( SQL, true );
				}
			}
		}
	}

	if(NULL != row_max && NULL != row_max[0])
	{
		time_t tLastUpdate = StringUtils::SQLDateTime(row_max[0]);
		if( tLastUpdate )
		{
			DCEConfig dce_config;
			dce_config.AddString("LastSearchTokenUpdate",StringUtils::itos(static_cast<int>(tLastUpdate)));
			dce_config.WriteSettings();
		}
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "UpdateMedia::UpdateSearchTokens Update search tokens last: %d / %s",tLastUpdate,row_max[0]);
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Update search tokens ended.");
}

void UpdateMedia::UpdateThumbnails()
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Updating thumbs...");

	string SQL = "SELECT PK_Picture,Extension FROM Picture";
	PlutoSqlResult result;
	DB_ROW row;
	if( ( result.r=m_pDatabase_pluto_media->db_wrapper_query_result( SQL ) ) )
	{
		while( ( row=db_wrapper_fetch_row( result.r ) ) )
		{
#ifdef WIN32
			__time64_t tModTime=0,tTnModTime=0;
			struct __stat64 dirEntryStat;
#else
			time_t tModTime=0,tTnModTime=0;
			struct stat64 dirEntryStat;
#endif

			if ( stat64((string("/home/mediapics/") + row[0] + "." + row[1]).c_str(), &dirEntryStat) == 0 )
				tModTime = dirEntryStat.st_mtime;

			if ( stat64((string("/home/mediapics/") + row[0] + "_tn." + row[1]).c_str(), &dirEntryStat) == 0 )
				tTnModTime = dirEntryStat.st_mtime;

			if( tModTime>tTnModTime )
			{
				if(m_bAsDaemon)
					Sleep(40);

				string Cmd = "convert -scale 256x256 -antialias /home/mediapics/" + string(row[0])  + "." + row[1] +
					" /home/mediapics/" + row[0] + "_tn." + row[1];
				int result;
				if( ( result=system( Cmd.c_str( ) ) )!=0 )
					LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Thumbnail picture %s returned %d", Cmd.c_str( ), result );

				LoggerWrapper::GetInstance()->Write(LV_STATUS, "Thumbs pic #%s updated!", row[0]);
			}
			else if( tModTime==0 && tTnModTime==0 )
			{
				if(m_bAsDaemon)
					Sleep(40);

				LoggerWrapper::GetInstance()->Write(LV_STATUS, "Picture %s is missing. Deleting all references", row[0]);

				m_pDatabase_pluto_media->threaded_db_wrapper_query("DELETE FROM Picture WHERE PK_Picture=" + string(row[0]));
				m_pDatabase_pluto_media->threaded_db_wrapper_query("DELETE FROM Picture_Attribute WHERE FK_Picture=" + string(row[0]));
				m_pDatabase_pluto_media->threaded_db_wrapper_query("DELETE FROM Picture_Disc WHERE FK_Picture=" + string(row[0]));
				m_pDatabase_pluto_media->threaded_db_wrapper_query("DELETE FROM Picture_File WHERE FK_Picture=" + string(row[0]));
				m_pDatabase_pluto_media->threaded_db_wrapper_query("UPDATE Bookmark SET FK_Picture=NULL WHERE FK_Picture=" + string(row[0]));

				LoggerWrapper::GetInstance()->Write(LV_STATUS, "Thumbs pic #%s deleted!", row[0]);
			}


		}
	}
}

void UpdateMedia::SyncDbWithDirectory(string sDirectory)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Sync'ing db with directory...");

	string sSql =
		"SELECT PK_File, Path, Filename, Missing, EK_Device, pluto_main.Device_DeviceData.IK_DeviceData "
		"FROM pluto_media.File "
		"LEFT JOIN pluto_main.Device_DeviceData ON pluto_main.Device_DeviceData.FK_Device = pluto_media.File.EK_Device "
		"AND FK_DeviceData = " + StringUtils::ltos(DEVICEDATA_Online_CONST) + " "
		"WHERE pluto_media.File.Path LIKE '" + StringUtils::SQLEscape(FileUtils::ExcludeTrailingSlash(sDirectory), true) + "/%' OR "
		"pluto_media.File.Path = '" + StringUtils::SQLEscape(FileUtils::ExcludeTrailingSlash(sDirectory)) + "'";

	enum SqlFields
	{
		sfFileID,
		sfPath,
		sfFilename,
		sfMissing,
		sfDeviceID,
		sfOnline
	};

	bool bRowsAffected = false;
	DB_ROW row;
	PlutoSqlResult allresult;
	if(NULL != (allresult.r = m_pDatabase_pluto_media->db_wrapper_query_result(sSql)))
	{
		while((row = db_wrapper_fetch_row(allresult.r)))
		{
			if(
				NULL != row && NULL != row[sfFileID] &&  NULL != row[sfPath] && 
				NULL != row[sfFilename] && NULL != row[sfMissing]
				)
				{
					int nFileID = atoi(row[sfFileID]);
					string sPath = row[sfPath];
					string sFilename = row[sfFilename];
					bool bDbFileMissing = atoi(row[sfMissing]) != 0;
					bool bDeviceOnline = NULL != row[sfOnline] ? atoi(row[sfOnline]) != 0 : true;
					//int nDeviceID = NULL != row[sfDeviceID] ? atoi(row[sfDeviceID]) : 0;

					string sFilePath = sPath + "/" + sFilename;

					if(bDeviceOnline)
					{
						bool bFileIsMissing = !FileUtils::FileExists(sFilePath);

						if(bFileIsMissing && !bDbFileMissing) 
						{
							bRowsAffected = true;
							string sUpdateSql = "UPDATE File SET Missing = 1 WHERE PK_File = " + StringUtils::ltos(nFileID);
							m_pDatabase_pluto_media->threaded_db_wrapper_query(sUpdateSql);
							LoggerWrapper::GetInstance()->Write(LV_STATUS, "Marking record as missing in database: %s", sFilePath.c_str());

							if(m_bAsDaemon)
								Sleep(50);
						}
						else if(!bFileIsMissing && bDbFileMissing)
						{
							bRowsAffected = true;
							string sUpdateSql = "UPDATE File SET Missing = 0 WHERE PK_File = " + StringUtils::ltos(nFileID);
							m_pDatabase_pluto_media->threaded_db_wrapper_query(sUpdateSql);
							LoggerWrapper::GetInstance()->Write(LV_STATUS, "Marking record as NOT missing in database: %s", sFilePath.c_str());

							if(m_bAsDaemon)
								Sleep(50);
						}
					}
					else
						LoggerWrapper::GetInstance()->Write(LV_STATUS, "Device is offline, skipping the file %s", sFilePath.c_str());
				}
		}
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "DB sync'd with directory!");
}

bool UpdateMedia::ScanFiles(string sDirectory)
{
	// Build a list of the files on disk, and a map of those in the database
	list<string> listFilesOnDisk;
	FileUtils::FindFiles(listFilesOnDisk,sDirectory,m_sExtensions,false,false,0,"");

	if(!listFilesOnDisk.empty())
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "UpdateMedia::ScanFiles dir %s: files found: %d", 
			sDirectory.c_str(), listFilesOnDisk.size());	

	// Now start matching them up
	for(list<string>::iterator it=listFilesOnDisk.begin();it!=listFilesOnDisk.end();++it)
	{
		if(UpdateMediaSig::bSignalTrapCaught)
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "SIGTERM received. Exiting...");
			return false;
		}

		string sFile = *it;
		if(AnyReasonToSkip(sDirectory, sFile))
			continue;

		if(FileStatusObserver::IsFileOpen(sDirectory + "/" + sFile))
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "File %s/%s is opened for writting. Adding to file status observer's thread...",
				sDirectory.c_str(), sFile.c_str());

			FileStatusObserver::Instance().Observe(sDirectory + "/" + sFile);
			continue;
		}

		if(m_bAsDaemon)
			Sleep(1);

		MediaSyncMode sync_mode = MediaState::Instance().SyncModeNeeded(sDirectory, sFile);
		if(sync_mode == modeNone)
			continue;

		GenericFileHandler *pFileHandler = FileHandlerFactory::CreateFileHandler(sDirectory, sFile);

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Sync mode for %s/%s: %s", sDirectory.c_str(), sFile.c_str(), MediaSyncModeStr[sync_mode]); 
		PlutoMediaFile PlutoMediaFile_(m_pDatabase_pluto_media, m_nPK_Installation, sDirectory, sFile, pFileHandler);

		//adjust sync mode
		if(PlutoMediaFile::GetDefaultSyncMode() == modeFileToDb && sync_mode == modeBoth)
			sync_mode = modeFileToDb;
		if(PlutoMediaFile::GetDefaultSyncMode() == modeDbToFile && sync_mode == modeBoth)
			sync_mode = modeDbToFile;
		PlutoMediaFile_.SetSyncMode(sync_mode);

		// Is it in the database?
		int PK_File=0;
		if(!AlreadyInDatabase(sDirectory, sFile))
		{
			PK_File = PlutoMediaFile_.HandleFileNotInDatabase();
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"UpdateMedia::ReadDirectory PlutoMediaFile_.HandleFileNotInDatabase %d",PK_File);
			if(!PK_File)
			{
				if(m_bAsDaemon)
					Sleep(10);

				continue; // Nothing to do
			}

			PlutoMediaFile_.SetFileAttribute(PK_File);
		}	
		else
		{
			// It's in the database already.  Be sure the attribute is set
			PK_File = GetFileID(sDirectory, sFile);

			if(PK_File != 0)
			{
				PlutoMediaFile_.SetFileAttribute(PK_File);  
			}
			else
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "The file exists in the db, but I can't find it's PK_File!!!");
			}
			
		}

		if(m_bAsDaemon)
			Sleep(300);


		int PK_Picture = 0;
		if(PK_File != 0)
		{
			PlutoMediaFile_.GetPicAttribute(PK_File);
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"UpdateMedia::ReadDirectory PK_File is %d, won't sync pictures", PK_File);
		}

		LoggerWrapper::GetInstance()->Write(LV_STATUS,"UpdateMedia::ReadDirectory File %d Picture %d",PK_File,PK_Picture);

		if(PK_Picture && PK_File)
		{
			string sSql="SELECT Attribute.* FROM Attribute"
				" JOIN File_Attribute ON FK_Attribute=PK_Attribute"
				" JOIN File ON FK_File=PK_File"
				" JOIN MediaType_AttributeType ON Attribute.FK_AttributeType=MediaType_AttributeType.FK_AttributeType"
				" AND MediaType_AttributeType.EK_MediaType=File.EK_MediaType"
				" WHERE PK_File=" + StringUtils::itos(PK_File) + " AND Identifier=1";
			vector<Row_Attribute *> vectRow_Attribute;
			m_pDatabase_pluto_media->Attribute_get()->GetRows(sSql,&vectRow_Attribute);
			for(size_t s=0;s<vectRow_Attribute.size();++s)
			{
				Row_Attribute *pRow_Attribute = vectRow_Attribute[s];
				Row_Picture_Attribute *pRow_Picture_Attribute = m_pDatabase_pluto_media->Picture_Attribute_get()->GetRow(PK_Picture,pRow_Attribute->PK_Attribute_get());
				if( !pRow_Picture_Attribute )
				{
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"UpdateMedia::ReadDirectory Adding Picture_Attribute File %d Picture %d attr %d size %d %s",PK_File,PK_Picture,pRow_Attribute->PK_Attribute_get(),(int) vectRow_Attribute.size(),sSql.c_str());
					pRow_Picture_Attribute = m_pDatabase_pluto_media->Picture_Attribute_get()->AddRow();
					pRow_Picture_Attribute->FK_Picture_set(PK_Picture);
					pRow_Picture_Attribute->FK_Attribute_set(pRow_Attribute->PK_Attribute_get());
					m_pDatabase_pluto_media->Picture_Attribute_get()->Commit();
				}
			}
		}
	}

	return true;
}

bool UpdateMedia::ScanSubfolders(string sDirectory, FolderType& folder_type)
{
	list<string> listSubDirectories;

#ifdef WIN32
	FileUtils::FindDirectories(listSubDirectories,sDirectory,false,true,0,"");
#else
	map<u_int64_t,bool> mapInodes;
	FileUtils::FindDirectories(listSubDirectories,sDirectory,false,true,0,"", &mapInodes);
#endif

	if(!listSubDirectories.empty())
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "UpdateMedia::ScanSubfolders dir %s: subdirs found: %d", 
			sDirectory.c_str(), listSubDirectories.size());	

	//is this a special folder?
	for(list<string>::iterator it=listSubDirectories.begin();it!=listSubDirectories.end();++it)
	{
		string sSubDir = *it;

		//is sDirectory a ripped dvd ?
		if( 
			StringUtils::ToUpper(FileUtils::FilenameWithoutPath(sSubDir))=="VIDEO_TS" || 
			StringUtils::ToUpper(FileUtils::FilenameWithoutPath(sSubDir))=="AUDIO_TS" 
			)
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "'%s' is a ripped dvd", sDirectory.c_str());
			folder_type = ftDVD;
			break;
		}

		//HD?
		if(StringUtils::ToUpper(FileUtils::FilenameWithoutPath(sSubDir))=="HVDVD_TS")
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "'%s' is a HDDVD", sDirectory.c_str());
			folder_type = ftHDDVD;
			break;
		}

		//BluRay?
		if(StringUtils::ToUpper(FileUtils::FilenameWithoutPath(sSubDir))=="BDMV")
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "'%s' is a BDMV", sDirectory.c_str());
			folder_type = ftBluRay;
			break;
		}
	}

	if(folder_type != ftNormal)
		return true;

	for(list<string>::iterator it=listSubDirectories.begin();it!=listSubDirectories.end();++it)
	{
		string sSubDir = *it;

		// UPnP changes: skipping .metadata .debug and _search subfolders
		// also skipping folders starting from "MythTV AV Media Server"
		if ( StringUtils::StartsWith(sSubDir, UpdateMediaVars::sUPnPMountPoint+"/") && ( StringUtils::EndsWith(sSubDir,"/.debug") || StringUtils::EndsWith(sSubDir,"/.metadata") || StringUtils::EndsWith(sSubDir,"/_search") ) )
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "The directory %s is UPnP service dir. We'll skip it!", sSubDir.c_str());
			continue;
		}
		
		if ( StringUtils::StartsWith(sSubDir, UpdateMediaVars::sUPnPMountPoint+"/") && (sSubDir.find("MythTV AV Media Server")!=string::npos) )
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "The directory %s is MythTV UPnP dir. We'll skip it!", sSubDir.c_str());
			continue;
		}
		
		if ( !UpdateMediaVars::sLocalUPnPServerName.empty() && StringUtils::StartsWith(sSubDir, UpdateMediaVars::sUPnPMountPoint+"/" + UpdateMediaVars::sLocalUPnPServerName))
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "The directory %s is LinuxMCE UPnP dir. We'll skip it!", sSubDir.c_str());
			continue;
		}
		
		ReadDirectory(sSubDir);
	}

	return true;
}

int UpdateMedia::SetupDirectory(string sDirectory, FolderType folder_type)
{
	auto_ptr<PlutoMediaFile> spPlutoMediaParentFolder;
	spPlutoMediaParentFolder.reset();

	string sBaseDirectory = FileUtils::BasePath(sDirectory);
	string sDirectoryName = FileUtils::FilenameWithoutPath(sDirectory);
	MediaSyncMode dir_sync_mode = MediaState::Instance().SyncModeNeeded(sBaseDirectory, sDirectoryName);

	if(m_bAsDaemon)
		Sleep(10);

	if(folder_type != ftNormal)
	{
		//always sync dvd folders
		dir_sync_mode = modeBoth;
	}

	if(dir_sync_mode == modeNone)
		return 0;

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Sync mode for dir %s/%s: %s", sBaseDirectory.c_str(), sDirectoryName.c_str(), MediaSyncModeStr[dir_sync_mode]); 

	if(dir_sync_mode != modeNone)
	{
		GenericFileHandler *pFileHandler = FileHandlerFactory::CreateFileHandler(sBaseDirectory, sDirectoryName);
		spPlutoMediaParentFolder.reset(new PlutoMediaFile(m_pDatabase_pluto_media, m_nPK_Installation, sBaseDirectory, sDirectoryName, pFileHandler));

		//adjust sync mode
		if(PlutoMediaFile::GetDefaultSyncMode() == modeFileToDb && dir_sync_mode == modeBoth)
			dir_sync_mode = modeFileToDb;
		if(PlutoMediaFile::GetDefaultSyncMode() == modeDbToFile && dir_sync_mode == modeBoth)
			dir_sync_mode = modeDbToFile;
		spPlutoMediaParentFolder->SetSyncMode(dir_sync_mode);
	}

	if(NULL != spPlutoMediaParentFolder.get())
	{
		int PK_File = 0;

		if(folder_type == ftNormal)
		{
			if(!AlreadyInDatabase(FileUtils::BasePath(sDirectory), FileUtils::FilenameWithoutPath(sDirectory)))
			{
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "Adding parent folder to db: %s PlutoMediaParentFolder.HandleFileNotInDatabase", sDirectory.c_str());
				PK_File = spPlutoMediaParentFolder->HandleFileNotInDatabase(0);
			}
			else
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "Parent folder already in the database: %s", sDirectory.c_str());
				PK_File = GetFileID(FileUtils::BasePath(sDirectory), FileUtils::FilenameWithoutPath(sDirectory));
				spPlutoMediaParentFolder->SetFileAttribute(PK_File);
			}
		}
		else
		{
			int nMediaType = folder_type ==  ftDVD ? MEDIATYPE_pluto_DVD_CONST :
					folder_type == ftHDDVD ? MEDIATYPE_pluto_HDDVD_CONST : MEDIATYPE_pluto_BD_CONST;

			// Add this directory like it were a file
			if(!AlreadyInDatabase(FileUtils::BasePath(sDirectory), FileUtils::FilenameWithoutPath(sDirectory)))
			{
				PK_File = spPlutoMediaParentFolder->HandleFileNotInDatabase(nMediaType);
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"UpdateMedia::ReadDirectory media type %d PlutoMediaFile_.HandleFileNotInDatabase %d",nMediaType, PK_File);
				Row_File *pRow_File = m_pDatabase_pluto_media->File_get()->GetRow(PK_File);
				pRow_File->Reload();
				pRow_File->IsDirectory_set(false);
				m_pDatabase_pluto_media->File_get()->Commit();

				spPlutoMediaParentFolder->SetFileAttribute(PK_File);
			}
			else
			{
				PK_File = GetFileID(FileUtils::BasePath(sDirectory), FileUtils::FilenameWithoutPath(sDirectory));
				spPlutoMediaParentFolder->SetFileAttribute(PK_File);
				spPlutoMediaParentFolder->SetMediaType(PK_File, nMediaType);
			}
		}

		int PK_Picture_Directory = 0;

		// Whatever was the first picture we found will be the one for this directory
		if(PK_File)
			PK_Picture_Directory = spPlutoMediaParentFolder->GetPicAttribute(PK_File);
		else
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"UpdateMedia::ReadDirectory PK_File is %d, won't sync pictures", PK_File);

		if(PK_Picture_Directory)
			spPlutoMediaParentFolder->SetPicAttribute(PK_Picture_Directory, "");

		return PK_Picture_Directory;
	}

	return 0;
}

bool UpdateMedia::AnyReasonToSkip(string sDirectory, string sFile)
{
	//not a media file or a directory
	if(!UpdateMediaFileUtils::IsDirectory((sDirectory + "/" + sFile).c_str()) && !PlutoMediaIdentifier::Identify(sDirectory + "/" + sFile))
		return true;

	//ignore id3 and lock files
	if(StringUtils::ToLower(FileUtils::FindExtension(sFile)) == "id3" || StringUtils::ToLower(FileUtils::FindExtension(sFile)) == "lock")
		return true;

	//ignore vdr's index and info files
	if(sFile == "info.vdr" || sFile == "index.vdr")
		return true;

	if(!FileUtils::FileExists(sDirectory + "/" + sFile)) //the file was just being deleted
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "The file %s/%s was just being deleted. We'll skip it!",
			sDirectory.c_str(), sFile.c_str());
		return true;
	}

	//is the lock file for folders (it ends with lock token) ?
	static const string csLockToken = ".folderlock";
	size_t pos = sFile.rfind(csLockToken);
	if(pos != string::npos && sFile.length() - pos == csLockToken.length())
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "The file %s/%s is the lock file for a folder!",
			sDirectory.c_str(), sFile.c_str());
		return true;
	}

	string sLockFile = sDirectory + "/" + sFile + ".lock";
	if(FileUtils::FileExists(sLockFile))
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Found lock file %s", sLockFile.c_str());

		time_t tFileTimestamp = FileUtils::GetLastModifiedDate(sLockFile);
		time_t tNow = time(NULL);

		// deleting locks if they are >2hours old
		if(tFileTimestamp == -1 || tNow - tFileTimestamp > 7200) //Media_Plugin has more the enough time to add the file in the database
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "The lock file %s is old (%d seconds). Deleting it!", sLockFile.c_str(), tNow - tFileTimestamp);
			FileUtils::DelFile(sLockFile);
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "The lock file %s is still active (%d seconds old). Ignoring the protected file!", sLockFile.c_str(), tNow - tFileTimestamp);
			return true;
		}
	}

	// UPnP-related changes - skipping service files
	// sUPnPMountPoint + "/devices"
	// sUPnPMountPoint + "/something.../" + ".status"
	if ( ((sDirectory == UpdateMediaVars::sUPnPMountPoint) && (sFile == "devices")) || (StringUtils::StartsWith(sDirectory, UpdateMediaVars::sUPnPMountPoint+"/") && (sFile == ".status")) )
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "The file %s/%s is UPnP service file. We'll skip it!",
					   sDirectory.c_str(), sFile.c_str());
		return true;
	}
		
	return false;
}

bool UpdateMedia::IsLockedFolder(string sDirectory)
{
	static const string csLockToken = ".folderlock";

	//top folder?
	if(sDirectory == "/" || sDirectory.empty())
		return false;

	//a lock folder exist for me or one of my parents ?
	return FileUtils::FileExists(sDirectory + csLockToken) ||
		IsLockedFolder(FileUtils::BasePath(sDirectory));
}

bool UpdateMedia::HasSpecialFolderParent(string sDirectory)
{
	//top folder?
	if(sDirectory == "/" || sDirectory.empty())
		return false;

	//subfolder for special folder?
	string sFolderName = StringUtils::ToUpper(FileUtils::FilenameWithoutPath(sDirectory));

	if(
		sFolderName == "VIDEO_TS" || sFolderName == "AUDIO_TS" ||
		sFolderName == "HVDVD_TS" || sFolderName == "BDMV"
	)
		return true;

	//what about my parent ?
	return HasSpecialFolderParent(FileUtils::BasePath(sDirectory));
}

bool UpdateMedia::AlreadyInDatabase(string sDirectory, string sFile)
{
	vector<Row_File *> vectRow_File;
	m_pDatabase_pluto_media->File_get()->GetRows("Path='" + StringUtils::SQLEscape(sDirectory) + 
		"' AND Filename='" + StringUtils::SQLEscape(sFile) + "'", &vectRow_File);

	return !vectRow_File.empty();
}

int UpdateMedia::GetFileID(string sDirectory, string sFile)
{
	 int nFileID = MediaState::Instance().FileId(sDirectory, sFile);

	 if(nFileID == 0)
	 {
		 LoggerWrapper::GetInstance()->Write(LV_STATUS, "Need to get the id from DB %s/%s since it's not saved in MediaState",
			 sDirectory.c_str(), sFile.c_str());
         
		 vector<Row_File *> vectRow_File;
		 m_pDatabase_pluto_media->File_get()->GetRows("Path='" + StringUtils::SQLEscape(sDirectory) + 
			 "' AND Filename='" + StringUtils::SQLEscape(sFile) + "'", &vectRow_File);

		 if(!vectRow_File.empty())
		 {
			 Row_File *pRow_File = vectRow_File[0];
			 return pRow_File->PK_File_get();
		 }
     }

	 if(nFileID == 0)
	 {
		 LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Something is very very wrong for file %s/%s",
			 sDirectory.c_str(), sFile.c_str());
	 }

	 return nFileID;
}
