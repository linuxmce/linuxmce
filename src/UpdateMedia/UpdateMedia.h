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
#ifndef UpdateMedia_h
#define UpdateMedia_h

#include "pluto_media/Database_pluto_media.h"
#include "pluto_main/Database_pluto_main.h"
#include "MediaState.h"

class UpdateMedia 
{
	enum FolderType
	{
		ftNormal,
		ftDVD,
		ftHDDVD,
		ftBluRay
	};

	string m_sDirectory,m_sExtensions;
	bool m_bAsDaemon;
	int m_nPK_Installation;
	void ReadConfigFile();

	//Helper method
	void SetupInstallation();

	bool ScanFiles(string sDirectory);
	bool ScanSubfolders(string sDirectory, FolderType& folder_type);
	int SetupDirectory(string sDirectory, FolderType folder_type);

	bool AnyReasonToSkip(string sDirectory, string sFile);
	bool IsLockedFolder(string sDirectory);
	bool HasSpecialFolderParent(string sDirectory);
	bool AlreadyInDatabase(string sDirectory, string sFile);
	int GetFileID(string sDirectory, string sFile);

public:
	Database_pluto_media *m_pDatabase_pluto_media ;
	Database_pluto_main *m_pDatabase_pluto_main;

	UpdateMedia(string host, string user, string pass, int port,string sDirectory,bool bSyncFilesOnly);
	UpdateMedia(Database_pluto_media *pDatabase_pluto_media, Database_pluto_main *pDatabase_pluto_main, 
		string sDirectory);
	~UpdateMedia();

	void DoIt();

	int ReadDirectory(string Directory); // Returns the picture for the directory

	void UpdateSearchTokens();
	void UpdateThumbnails();
	void SyncDbWithDirectory(string sDirectory);
	void LoadExtensions();
};
#endif
