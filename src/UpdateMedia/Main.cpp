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
#include "PlutoUtils/DatabaseUtils.h"
#include "DCEConfig.h"
#include "Logger.h"
#include "UpdateMedia.h"
#include "PlutoUtils/MultiThreadIncludes.h"

#include "FileStatusObserver.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <list>

#ifdef WIN32
#include <direct.h>
#include <conio.h>
#define chdir _chdir  // Why, Microsoft, why?
#define mkdir _mkdir  // Why, Microsoft, why?
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#endif

#include "inotify/FileNotifier.h"
#include "PlutoMediaFile.h"
#include "PlutoMediaFile.h"
#include "pluto_media/Define_AttributeType.h"
#include "pluto_media/Table_Attribute.h"

#include "DCE/Message.h"
#include "DCE/DeviceData_Impl.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_MediaType.h"

#include "Media_Plugin/MediaAttributes_LowLevel.h"

#define  VERSION "<=version=>"

using namespace std;
using namespace DCE;

namespace UpdateMediaVars
{
    bool bError, bUpdateThumbnails, bUpdateSearchTokens, bRunAsDaemon;
    string sDirectory;
    string sUPnPMountPoint;
    string sLocalUPnPServerName;
	bool bSyncFilesOnly;
	vector<string> vectModifiedFolders;

    Database_pluto_media *g_pDatabase_pluto_media = NULL;

    pluto_pthread_mutex_t g_ConnectionMutex("connections");
	pluto_pthread_mutex_t g_FoldersListMutex("folders list");
	pthread_cond_t g_ActionCond;
}

using namespace UpdateMediaVars;

void UpdateAlbumsForPhotos()
{
	MediaAttributes_LowLevel mediaAttributes_LowLevel(g_pDatabase_pluto_media);

	string sSqlAlbums = "SELECT DISTINCT Path FROM File where EK_MediaType=" TOSTRING(MEDIATYPE_pluto_Pictures_CONST) " AND Missing=0 and IsDirectory=0";

	PlutoSqlResult result;
	DB_ROW row;
	if((result.r = g_pDatabase_pluto_media->db_wrapper_query_result(sSqlAlbums)))
	{
		while((row = db_wrapper_fetch_row(result.r)))
		{
			Row_Attribute *pRow_Attribute = mediaAttributes_LowLevel.GetAttributeFromDescription(MEDIATYPE_pluto_Pictures_CONST,
				ATTRIBUTETYPE_Album_CONST, FileUtils::FilenameWithoutPath(row[0]));

			if( pRow_Attribute )
			{
				string sSQL = "REPLACE INTO File_Attribute(FK_File,FK_Attribute) SELECT PK_File," + StringUtils::itos(pRow_Attribute->PK_Attribute_get()) + 
					" FROM File where Path='" + row[0] + "' AND EK_MediaType=" TOSTRING(MEDIATYPE_pluto_Pictures_CONST) " AND Missing=0 and IsDirectory=0";
				int iResult = g_pDatabase_pluto_media->threaded_db_wrapper_query(sSQL);
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "Updated PhotoAlbum %d records: %s", iResult, sSQL.c_str());
			}
		}
	}
}

void AddUnknownAlbum()
{
	MediaAttributes_LowLevel mediaAttributes_LowLevel(g_pDatabase_pluto_media);

	Row_Attribute *pRow_Attribute = mediaAttributes_LowLevel.GetAttributeFromDescription(MEDIATYPE_pluto_StoredAudio_CONST,
		ATTRIBUTETYPE_Album_CONST, "Unknown Album");  // Add the unknown attribute for songs without an album

	int iResult1=0, iResult2=0;

	if( pRow_Attribute )
		iResult1=g_pDatabase_pluto_media->threaded_db_wrapper_query(
			"REPLACE INTO File_Attribute(FK_File,FK_Attribute) "
				"SELECT PK_File," + StringUtils::itos(pRow_Attribute->PK_Attribute_get()) + " FROM File "
				"LEFT JOIN File_Attribute ON FK_File=PK_File AND FK_AttributeType=" TOSTRING(ATTRIBUTETYPE_Album_CONST) " "
				"WHERE EK_MediaType=" TOSTRING(MEDIATYPE_pluto_StoredAudio_CONST) " AND FK_AttributeType IS NULL" );

	pRow_Attribute = mediaAttributes_LowLevel.GetAttributeFromDescription(MEDIATYPE_pluto_StoredAudio_CONST,
		ATTRIBUTETYPE_Performer_CONST, "Unknown Performer");  // Add the unknown attribute for songs without a performer

	if( pRow_Attribute )
		iResult2=g_pDatabase_pluto_media->threaded_db_wrapper_query(
			"REPLACE INTO File_Attribute(FK_File,FK_Attribute) "
				"SELECT PK_File," + StringUtils::itos(pRow_Attribute->PK_Attribute_get()) + " FROM File "
				"LEFT JOIN File_Attribute ON FK_File=PK_File AND FK_AttributeType=" TOSTRING(ATTRIBUTETYPE_Performer_CONST) " "
				"WHERE EK_MediaType=" TOSTRING(MEDIATYPE_pluto_StoredAudio_CONST) " AND FK_AttributeType IS NULL" );
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "AddUnknownAlbum %d/%d records", iResult1, iResult2);
}

void RemoveDuplicatedAttributes()
{
	string sSqlDuplicatedAttributes = 
		"SELECT Name, FK_AttributeType, PK_Attribute FROM Attribute\n"
		"GROUP BY Name, FK_AttributeType\n"
		"HAVING Count(PK_Attribute) > 1";

	enum DuplicatedAttributesFields
	{
		dafName,
		dafAttributerType,
		dafAttribute
	};

	LoggerWrapper::GetInstance()->Write(LV_WARNING, "DISABLED -- Removing duplicated attributes..."); 

	PlutoSqlResult result;
	DB_ROW row;
	if((result.r = g_pDatabase_pluto_media->db_wrapper_query_result(sSqlDuplicatedAttributes)))
	{
		while((row = db_wrapper_fetch_row(result.r)))
		{
			if(NULL != row[dafName] && NULL != row[dafAttributerType] && NULL != row[dafAttribute])
			{
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "DISABLED -- Found duplicated attribute %s, pk %s, type %s. Won't touch it.",
					row[dafName], row[dafAttribute], row[dafAttributerType]);

				//char *AffectedTables[] =
				//{
				//	"File_Attribute", 
				//	"Attribute_Settings", 
				//	"CoverArtScan", 
				//	"Disc_Attribute", 
				//	"Download_Attribute", 
				//	"LongAttribute", 
				//	"Picture_Attribute", 
				//	"SearchToken_Attribute"
				//};

				//for(int i = 0; i < sizeof(AffectedTables) / sizeof(char *); ++i)
				//{
				//	string sSqlMoveRowsFromDuplicates = 
				//		"UPDATE Attribute JOIN File_Attribute ON FK_Attribute = PK_Attribute\n"
				//		"SET FK_Attribute = " + string(row[dafAttribute]) + "\n"
				//		"WHERE FK_AttributeType = " + string(row[dafAttributerType]) + "\n"
				//		"AND Name = '" + StringUtils::SQLEscape(row[dafName]) + "'\n"
				//		"AND PK_Attribute <> " + row[dafAttribute];
				//	g_pDatabase_pluto_media->threaded_db_wrapper_query(sSqlMoveRowsFromDuplicates);
				//}

				//string sSqlDeleteDuplicates = 
				//	"DELETE FROM Attribute\n"
				//	"WHERE FK_AttributeType = " + string(row[dafAttributerType]) + "\n"
				//	"AND Name = '" + StringUtils::SQLEscape(row[dafName]) + "'\n"
				//	"AND PK_Attribute <> " + row[dafAttribute];

    //            int nAffectedRecords = g_pDatabase_pluto_media->threaded_db_wrapper_query(sSqlDeleteDuplicates);
				//if(nAffectedRecords == -1)
				//{
				//	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed to remove duplicated attributes! Query: %s",
				//		sSqlDeleteDuplicates.c_str());
				//}
				//else
				//{
				//	LoggerWrapper::GetInstance()->Write(LV_WARNING, "Removed %d duplicated attributes for '%s'", 
				//		nAffectedRecords, row[dafName]); 
				//}
			}
		}
	}

	LoggerWrapper::GetInstance()->Write(LV_WARNING, "Finished removing duplicated attributes!"); 
}

void *UpdateMediaThread(void *)
{
	UpdateAlbumsForPhotos();
	DatabaseUtils::SyncMediaAttributes(g_pDatabase_pluto_media);
	DatabaseUtils::UpdateAttributeCount(g_pDatabase_pluto_media);
	RemoveDuplicatedAttributes();
	AddUnknownAlbum();

	PlutoMediaIdentifier::Activate();

	vector<string> vsUPnPDevices;
	
	while(true)
	{
		//load info about ModificationData, AttrCount, AttrDate, attributes, timestamp for all files
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Loading fresh data from db...");
		MediaState::Instance().LoadDbInfo(g_pDatabase_pluto_media, UpdateMediaVars::sDirectory);
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Loaded fresh data from db");

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Worker thread: \"I'm wake!\"");        
		
		//UPnP changes: as UPnP mount share doesn't work with inotify (?? 
		//at least I don't see it firing any expected events, we are manually 
		//checking contents of 'devices' list for any changes
		//FIXME if UPnP server went down and up with new content within 2 mins sleep
		//changes in it's contents can go unnoticed
		if (!sUPnPMountPoint.empty())
		{
			vector<string> vsNewDevices;
			FileUtils::ReadFileIntoVector(sUPnPMountPoint+"/devices", vsNewDevices);
			sort(vsNewDevices.begin(), vsNewDevices.end());
			
			vector<string> vsChanges;
			set_symmetric_difference(vsNewDevices.begin(), vsNewDevices.end(), vsUPnPDevices.begin(), vsUPnPDevices.end(), back_inserter(vsChanges));
			
			if (!vsChanges.empty())
			{
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "UPnP mount point devices list changed, adding %s for processing", sUPnPMountPoint.c_str());	
				vsUPnPDevices = vsNewDevices;
				vectModifiedFolders.push_back(sUPnPMountPoint);
			}
			else
			{
				//TODO process list if devices list appear to be same - see fixme note above
			}
		}		

		PLUTO_SAFETY_LOCK(flm, g_FoldersListMutex);
		while(vectModifiedFolders.size())
		{
			string sItem = vectModifiedFolders.front();
			flm.Release();

			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Folder to process: %s", sItem.c_str());	
			PLUTO_SAFETY_LOCK(cm, g_ConnectionMutex );
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Synchronizing '%s'...", sItem.c_str());	

			PlutoMediaFile::ResetNewFilesAddedStatus();
			PlutoMediaFile::ResetAttributesUpdated();
				
			UpdateMedia engine(g_pDatabase_pluto_media, sItem);
			engine.LoadExtensions();
			engine.DoIt();

			if( bUpdateSearchTokens )
				engine.UpdateSearchTokens();

			if( bUpdateThumbnails )
				engine.UpdateThumbnails();

			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Synchronized '%s'. new files %d attributes updated %d",
				sItem.c_str(), (int) PlutoMediaFile::NewFilesAdded(), PlutoMediaFile::AttributesUpdated() );

			if(PlutoMediaFile::NewFilesAdded() || PlutoMediaFile::AttributesUpdated() )
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "New files were added to db for '%s'.", sItem.c_str());

				UpdateAlbumsForPhotos();
				DatabaseUtils::SyncMediaAttributes(g_pDatabase_pluto_media);
				DatabaseUtils::UpdateAttributeCount(g_pDatabase_pluto_media);

				LoggerWrapper::GetInstance()->Write(LV_STATUS, "Sending \"Check for new files\" command to Media_Plugin...");
				Event_Impl *pEvent = new Event_Impl(DEVICEID_MESSAGESEND, 0, "dcerouter");
				Message* pMessage = new Message(0, DEVICETEMPLATE_Media_Plugin_CONST, BL_SameHouse, 
					MESSAGETYPE_COMMAND, PRIORITY_NORMAL, 
					COMMAND_Check_For_New_Files_CONST, 0);
				pEvent->SendMessage(pMessage);
				delete pEvent;
				pEvent = NULL;

				RemoveDuplicatedAttributes();
				AddUnknownAlbum();

				LoggerWrapper::GetInstance()->Write(LV_STATUS, "Command \"Check for new files\" sent!");
			}

			flm.Relock();
			vectModifiedFolders.erase(vectModifiedFolders.begin());
		}

		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Nothing to process, sleeping 2 minute...");        
		timespec abstime;
		abstime.tv_sec = (long) (time(NULL) + 120);  //2 minutes
		abstime.tv_nsec = 0;
		flm.TimedCondWait(abstime);		
	}
}

void OnModify(list<string> &listFiles) 
{
	for(list<string>::iterator it = listFiles.begin(); it != listFiles.end(); it++)
	{
		string sItem = *it;

		struct stat st;
		if(0 != stat(sItem.c_str(), &st))
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "stat failed for %s. We'll try to parent!", sItem.c_str());
			
			size_t nPos = sItem.rfind("/");
			if(nPos != string::npos)
				sItem = sItem.substr(0, nPos);

			if(0 != stat(sItem.c_str(), &st))
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "stat failed for %s. We'll skip it!", sItem.c_str());
				continue;
			}
		}

		if(!(st.st_mode & S_IFDIR))
		{
			//this is a file, we need the folder from where is the file
			size_t nPos = sItem.rfind("/");
			if(nPos != string::npos)
				sItem = sItem.substr(0, nPos);
		}

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "New folder %s to sync...", sItem.c_str());        
		PLUTO_SAFETY_LOCK(flm, g_FoldersListMutex);

		bool bFound = false;
		for(vector<string>::iterator it = vectModifiedFolders.begin(); it != vectModifiedFolders.end(); it++)
		{
			if(*it == sItem)
			{
				bFound = true;
				break;
			}
		}

		if(!bFound)
		{
			vectModifiedFolders.push_back(sItem);
			pthread_cond_broadcast(&g_ActionCond);
		}
	}
}

int main(int argc, char *argv[])
{
	//make sure we are logging everything
	LoggerWrapper::GetInstance()->LogAll();

	bError=false;
	bUpdateThumbnails=false;
	bUpdateSearchTokens=false;
	bRunAsDaemon=false;
	bSyncFilesOnly=false;
	sDirectory="/home/";

	string sDBHost;
	string sDBUser;
	string sDBPassword;
	int iDBPort;

	{
		DCEConfig dceConfig;
		dceConfig.m_sDBName="pluto_media";

		sDBHost = dceConfig.m_sDBHost;
		sDBUser = dceConfig.m_sDBUser;
		sDBPassword = dceConfig.m_sDBPassword;
		iDBPort = dceConfig.m_iDBPort;

		if(dceConfig.m_mapParameters_Exists("DVDKeysCache"))
		{
			string sDVDKeysCache = dceConfig.m_mapParameters_Find("DVDKeysCache");
			PlutoMediaFile::DVDKeysCacheSetup(sDVDKeysCache);
		}
	}

	char c;
	for(int optnum=1;optnum<argc;++optnum)
	{
		if( argv[optnum][0]!='-' )
		{
			cerr << "Unknown option " << argv[optnum] << endl;
			bError=true;
		}

		c=argv[optnum][1];
		switch (c)
		{
		case 'h':
			sDBHost = argv[++optnum];
			break;
		case 'u':
			sDBUser = argv[++optnum];
			break;
		case 'p':
			sDBPassword = argv[++optnum];
			break;
		case 'P':
			iDBPort = atoi(argv[++optnum]);
			break;
		case 'd':
			sDirectory = argv[++optnum];
			break;
		case 's':
			bUpdateSearchTokens=true;
			break;
		case 't':
			bUpdateThumbnails=true;
			break;
		case 'B':
			bRunAsDaemon=true;
			break;
		case 'w':
			bSyncFilesOnly = true;
			break;
		case 'U':
			sUPnPMountPoint = argv[++optnum];
			break;
		default:
			cout << "Unknown: " << argv[optnum] << endl;
			bError=true;
			break;
		};
	}

	if ( bError )
	{
		cout << "UpdateMedia, v." << VERSION << endl
			<< "Usage: UpdateMedia [-h hostname] [-u username] [-p password] [-D database] [-P mysql port]" << endl
			<< "[-d The list with directories, pipe delimited] [-U UPnP mount point to scan] [-s] [-t]" << endl
			<< "hostname    -- address or DNS of database host, default is `dce_router`" << endl
			<< "username    -- username for database connection" << endl
			<< "password    -- password for database connection, default is `` (empty)" << endl
			<< "database    -- database name.  default is pluto_main" << endl
			<< "-s          -- Update all search tokens" << endl
			<< "-t          -- Update all thumbnails" << endl
			<< "-w			-- Synchronize files only" << endl
			<< "-B          -- Run as daemon" << endl
			<< "Directory defaults to /home" << endl;

		exit(1);
	}

	// detecting local UPnP server name
	if (sUPnPMountPoint!="")
	{
		//FIXME add real detection here
		sLocalUPnPServerName = "LinuxMCE";
	}
	
	if(!bRunAsDaemon)
	{
		vector<string> vectFolders;
		StringUtils::Tokenize(sDirectory, "|", vectFolders);
		for(vector<string>::iterator it = vectFolders.begin(); it != vectFolders.end(); ++it)
		{
			string sFolder = *it;

			UpdateMedia UpdateMedia(sDBHost,sDBUser,sDBPassword,iDBPort,sFolder,bSyncFilesOnly);

			if(!sFolder.empty())
				UpdateMedia.DoIt();

			if( bUpdateSearchTokens )
				UpdateMedia.UpdateSearchTokens();

			if( bUpdateThumbnails )
				UpdateMedia.UpdateThumbnails();
		}
		
		// extra code to process UPnP mount point
		if (!sUPnPMountPoint.empty())
		{
			UpdateMedia UpdateMedia(sDBHost,sDBUser,sDBPassword,iDBPort,sUPnPMountPoint,bSyncFilesOnly);
			UpdateMedia.DoIt();

			//TODO check and uncomment if necessary
/*
			if( bUpdateSearchTokens )
				UpdateMedia.UpdateSearchTokens();

			if( bUpdateThumbnails )
				UpdateMedia.UpdateThumbnails();
*/
		}

		UpdateAlbumsForPhotos();
		DatabaseUtils::SyncMediaAttributes(g_pDatabase_pluto_media);
		DatabaseUtils::UpdateAttributeCount(g_pDatabase_pluto_media);
		RemoveDuplicatedAttributes();
		AddUnknownAlbum();
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Running as daemon... ");

		pthread_cond_init(&g_ActionCond, NULL);
        g_ConnectionMutex.Init(NULL);
		g_FoldersListMutex.Init(NULL, &g_ActionCond);

        string sPlutoMediaDbName = "pluto_media";
        string sPlutoMainDbName = "pluto_main";

#ifdef USE_DEVEL_DATABASES
        sPlutoMediaDbName = "pluto_media_devel";
        sPlutoMainDbName = "pluto_main_devel";
#endif

        //connect to the databases
        g_pDatabase_pluto_media = new Database_pluto_media(LoggerWrapper::GetInstance());
        if( !g_pDatabase_pluto_media->Connect(sDBHost,sDBUser,sDBPassword,sPlutoMediaDbName,iDBPort) )
        {
            LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Cannot connect to database!" );
            return 1;
        }

		FileNotifier fileNotifier(g_pDatabase_pluto_media);
		fileNotifier.RegisterCallbacks(OnModify, OnModify); //we'll use the same callback for OnCreate and OnDelete events

		vector<string> vectFolders;
		StringUtils::Tokenize(sDirectory, "|", vectFolders);
		for(vector<string>::iterator it = vectFolders.begin(); it != vectFolders.end(); ++it)
		{
	  		fileNotifier.Watch(*it);
			vectModifiedFolders.push_back(*it);
		}

		pthread_t UpdateMediaThreadId;
		pthread_create(&UpdateMediaThreadId, NULL, UpdateMediaThread, NULL);

		FileStatusObserver::Instance().SetFileNotifier(&fileNotifier);

		fileNotifier.Run();//it waits for worker thread to exit; the user must press CTRL+C to finish it

		FileStatusObserver::Instance().UnsetFileNotifier();

		delete g_pDatabase_pluto_media;
		g_pDatabase_pluto_media = NULL;
	}

	FileStatusObserver::Instance().Finalize();
	MutexTracking::Delete();
	
	return 0;
}

