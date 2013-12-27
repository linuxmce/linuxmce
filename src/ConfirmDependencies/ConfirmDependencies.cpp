/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "DCEConfig.h"
#include "Logger.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <list>

#ifdef WIN32
#include <conio.h>
#endif

#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_Device.h"
#include "pluto_main/Table_Device_DeviceData.h"
#include "pluto_main/Table_Device_QuickStart.h"
#include "pluto_main/Table_QuickStartTemplate.h"
#include "pluto_main/Table_DeviceData.h"
#include "pluto_main/Table_DeviceCategory.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Table_RepositorySource.h"
#include "pluto_main/Table_RepositoryType.h"
#include "pluto_main/Table_RepositorySource_URL.h"
#include "pluto_main/Table_Installation_RepositorySource_URL.h"
#include "pluto_main/Table_Package.h"
#include "pluto_main/Table_Package_Device.h"
#include "pluto_main/Table_Distro.h"
#include "pluto_main/Table_Installation.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "pluto_main/Table_Directory.h"
#include "pluto_main/Table_Package_Source_Compat.h"
#include "pluto_main/Table_Package_Source.h"
#include "pluto_main/Table_Package_Package.h"
#include "pluto_main/Table_Package_Directory.h"
#include "pluto_main/Table_Package_Directory_File.h"
#include "pluto_main/Table_Package_Compat.h"

#include "CreateDevice/CreateDevice.h"

#include "version.h"

using namespace std;
using namespace DCE;



int iPK_Device=0;
class CreateDevice *g_pCreateDevice;

bool bSummary=false,bInteractive=true,g_bSkipConfirmRelations=false,g_bSkipInstallCheck=false;
map<int,bool> g_mapPackages_AlreadyChecked; // Don't check each package more than once

class PackageInfo
{
public:
	Row_Package_Source_Compat *m_pRow_Package_Source_Compat;
	Row_Package_Source *m_pRow_Package_Source;
	Row_RepositorySource_URL *m_pRow_RepositorySource_URL;
	bool m_bMustBuild,m_bAlreadyInstalled;

	string m_sBinaryExecutiblesPathPath, m_sSourceIncludesPath, m_sSourceImplementationPath, m_sBinaryLibraryPath, m_sConfiguration;
	vector<Row_Package_Directory_File *> m_vectRow_Package_Directory_File_BinaryExecutibles, m_vectRow_Package_Directory_File_SourceIncludes,
		m_vectRow_Package_Directory_File_SourceImplementation,m_vectRow_Package_Directory_File_BinaryLibrary,
		m_vectRow_Package_Directory_File_Configuration,m_vectRow_Package_Directory_File_CompiledOutput;

	// If this package is SVN or CVS, we may need to do svn co's on multiple directories
	vector<Row_Package_Directory *> m_vectAltSourceImplementation;

	Row_Package_Directory *m_pRow_Package_Directory_Compiled_Output;


	vector<PackageInfo *> m_vectPackageInfo; // Some alternates in case this one fails

	PackageInfo(Row_Package_Source_Compat *pRow_Package_Source_Compat,
		Row_Package_Source *pRow_Package_Source, Row_RepositorySource_URL *pRow_RepositorySource_URL,
		bool bMustBuild)
	{
		if( pRow_Package_Source->FK_Package_get()==280 )
		{
			int k=2;
		}
		m_pRow_Package_Source_Compat=pRow_Package_Source_Compat;
		m_pRow_Package_Source=pRow_Package_Source;
		m_pRow_Package_Source=pRow_Package_Source;
		m_pRow_RepositorySource_URL=pRow_RepositorySource_URL;
		m_bMustBuild=bMustBuild;
		m_pRow_Package_Directory_Compiled_Output=NULL;
		if( !g_bSkipInstallCheck )
		{
			Row_Package_Device *pRow_Package_Device = pRow_Package_Source_Compat->Table_Package_Source_Compat_get()->Database_pluto_main_get()->Package_Device_get()->GetRow(pRow_Package_Source->FK_Package_get(),iPK_Device);
			m_bAlreadyInstalled = (pRow_Package_Device!=NULL);
		}
		else
			m_bAlreadyInstalled = false;
	}
};

void CheckDevice(Row_Device *pRow_Device,bool bSourceCode);
void CheckDeviceLoop(Row_Device *pRow_Device,bool bDevelopment);
void CheckPackage(Row_Package *pRow_Package,Row_Device *pRow_Device,bool bDevelopment,Row_Distro *pRow_Distro,bool bMustBuildFromSource, int depth = 1);
PackageInfo *MakePackageInfo(Row_Package_Source_Compat *pRow_Package_Source_Compat,bool bMustBuildFromSource);
void VerifyFiles(PackageInfo *pPackageInfo,vector<Row_Package_Directory_File *> &vectRow_Package_Directory_File,string Path);
Row_Package_Directory *GetDirectory(Row_Package *pRow_Package,int PK_Directory);
void InstallPackage(PackageInfo *pPackageInfo, bool bElse = false);
Row_Package_Source_Compat *FindPreferredSource(vector<Row_Package_Source_Compat *> vectRow_Package_Source_Compat,Row_Device *pRow_Device);
void AddAlternateSources(vector<Row_Package_Source_Compat *> &vectRow_Package_Source_Compat,PackageInfo *pPackageInfo);

string sCommand;
int iPK_Distro=-1;
Row_Installation *pRow_Installation;
list<PackageInfo *> listPackageInfo;  // We need a list so we can keep them in the right order
DCEConfig dceConfig;
Row_Distro *pRow_Distro=NULL;
map<int,Row_Package *> m_mapReportedErrors;
vector<Row_Device *> m_vectDevicesNeedingConfigure,m_vectDevicesNeedingIR;
int iPK_Orbiter=0;

string GetCommand()
{
	cout << "What would you like to do?" << endl
		<< "In the future you can also make your selections on the command line using the command shown in parenthesis." << endl
		<< "1.  View required packages for a device, grouped by child devices. (view)" << endl
		<< "2.  Status, shows the same information as view, but also physically scans the disk for the files to see whether the packages are installed. (status)" << endl
		<< "3.  Just list all the packages in a single list (list)" << endl
		<< "4.  List all the packages in a single list with full information (listall)" << endl
		<< "5.	Output an install script for the packages (install)" << endl
		<< "6.	Output a script that will build any packages that were not available as binaries. (build)" << endl
		<< "7.	Output a script that will build all packages, even if they were available as source. (buildall)" << endl
		<< endl << "Q.  Quit" << endl;

	char c=(char) getch();

	switch(c)
	{
	case '1':
		return "view";
	case '2':
		return "status";
	case '3':
		return "list";
	case '4':
		return "listall";
	case '5':
		return "build";
	case '6':
		return "buildall";
	case 'Q':
	case 'q':
		exit(1);
	}
	return "";
}

void PrintCmd(int argc, char * argv[])
{
	cout << "#";
	for(int optnum=0;optnum<argc;++optnum)
	{
		if( argv[optnum][0]=='-' && argv[optnum][1]=='p' )
			optnum++;
		else
			cout << " " << argv[optnum];
	}

	cout << endl;
}

int main(int argc, char *argv[])
{
#ifndef WIN32
	LoggerWrapper::SetType(LT_LOGGER_FILE,"/var/log/pluto/ConfirmDependencies.log");
#endif

	bool bError=false,bIncludeDisklessMD=true,bSourceCode=false; // An error parsing the command line
	iPK_Device = dceConfig.m_iPK_Device_Computer;
	char c;

	for(int optnum=1;optnum<argc;++optnum)
	{
		if( argv[optnum][0]!='-' )
		{
			sCommand = argv[optnum];
			if( optnum<argc-1 )
			{
				cout << argv[optnum] << " isn't preceded by a -, but the command must be the last parameter" << endl;
				bError=true;	// The command must be the last thing on the line
			}
			break;
		}

		c=argv[optnum][1];
		char * cPointer;
		switch (c)
		{
		case 'h':
			if (++optnum != argc)
				dceConfig.m_sDBHost = argv[optnum];
			else
				bError = true;
			break;
		case 'u':
			if (++optnum != argc)
				dceConfig.m_sDBUser = argv[optnum];
			else
				bError = true;
			break;
		case 'p':
			if (++optnum != argc)
				dceConfig.m_sDBPassword = argv[optnum];
			else
				bError = true;
			break;
		case 'D':
			if (++optnum != argc)
				dceConfig.m_sDBName = argv[optnum];
			else
				bError = true;
			break;
		case 'O':
			if (++optnum != argc)
				iPK_Orbiter = atoi(argv[optnum]);
			else
				bError = true;
			break;
		case 'P':
			if (++optnum != argc)
			{
				dceConfig.m_iDBPort = strtol(argv[optnum], &cPointer, 10);
				bError = cPointer == argv[optnum];
			}
			else
			{
				bError = true;
			}
			break;
		case 'd':
			if (++optnum != argc)
			{
				iPK_Device = strtol(argv[optnum], &cPointer, 10);
				bError = cPointer == argv[optnum];
			}
			else
			{
				bError = true;
			}
			break;
		case 'o':
			if (++optnum != argc)
			{
				iPK_Distro = strtol(argv[optnum], &cPointer, 10);
				bError = cPointer == argv[optnum];
			}
			else
			{
				bError = true;
			}
			break;
		case 'i':
			bIncludeDisklessMD = false;
			break;
		case 'r':
			g_bSkipConfirmRelations = true;
			g_bSkipInstallCheck = true; // maintains existing functionality
			break;
		case 'f':
			g_bSkipInstallCheck = true;
			break;
		case 'n':
			bInteractive = false;
			break;
		case 's':
			bSummary = true;
			break;
		case 'c':
			bSourceCode = true;
			break;
		default:
			cout << "Unknown: " << argv[optnum] << endl;
			bError=true;
			break;
		};
	}

	if ( bError)
	{
		PrintCmd(argc, argv);
		cout << "ConfirmDependencies, v." << VERSION << endl
			<< "Usage: ConfirmDependencies [-h hostname] [-u username] [-p password] [-D database] [-P mysql port] [-O orbiter id] [-d DeviceID] [-o distro id] [-f] [-r] [-n] [-c] [-s] command" << endl
			<< "Commands" <<endl
			<< "view        -- View required packages for a device, grouped by child devices. (view)" << endl
			<< "status      -- Same as view, but also physically scans the disk for the files" << endl
			<< "               to see whether the packages are installed. (status)" << endl
			<< "list        -- List all the packages in a single list (list)" << endl
			<< "listall     -- List all the packages in a single list with full information (listall)" << endl
			<< "install     -- Output an install script for the packages (install)" << endl
			<< "build       -- Output a script that will build any packages that were not available as binaries. (build)" << endl
			<< "buildall    -- Output a script that will build all packages, even if they were available as source. (buildall)" << endl
			<< endl
			<< "Options" << endl
			<< "hostname    -- address or DNS of database host, default is `dcerouter`" << endl
			<< "username    -- username for database connection" << endl
			<< "password    -- password for database connection, default is `` (empty)" << endl
			<< "database    -- database name.  default is pluto_main" << endl
			<< "port        -- port for database connection, default is 3306" << endl
			<< "orbiter id  -- orbiter id to send onscreen messages" << endl
			<< "device id   -- top device id of the tree to verify" << endl
			<< "distro id   -- distribution id to verify against" << endl
			<< "-f          -- force install, skips checking if package is install and configured" << endl
			<< "               (useful for repairing installations)" << endl
//			<< "-i          -- do not parse DisklessMDs on core [currently unused, DisklessMDs are not parsed]" << endl
			<< "-r          -- skips relationship checking (avoids checking for required plugins due to MD devices)" << endl
			<< "               (implies -f for backwards compatibility)" << endl
			<< "-n          -- no prompts, errors will be sent to a log file without asking the user to continue" << endl
			<< "-c          -- include all source code packages" << endl
			<< "-s          -- summary" << endl;

		exit(1);
	}

	while( sCommand.length()==0 )
		sCommand=GetCommand();

	sCommand=StringUtils::ToLower(sCommand);

	if (sCommand == "install")
	{
		cout << "#!/bin/bash" << endl;
		cout << "error=0" << endl;
		cout << ". /usr/pluto/bin/SQL_Ops.sh" << endl;
		cout << "if /usr/pluto/bin/CheckInternetConnection.sh; then" << endl;
		cout << "\texport InternetConnection=InternetConnection" << endl;
		cout << "else" << endl;
		cout << "\texport InternetConnection=NoInternetConnection" << endl;
		cout << "fi" << endl;
	}
	PrintCmd(argc, argv);

	Database_pluto_main database_pluto_main(LoggerWrapper::GetInstance());
	if(!database_pluto_main.Connect(&dceConfig))
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Cannot connect to database!");
		cout << "# ERROR: Cannot connect to database!" << endl;
		return 0;
	}

	Row_Device *pRow_Device = database_pluto_main.Device_get()->GetRow(iPK_Device);
	if( !pRow_Device )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Cannot get device information!");
		cout << "# ERROR: Cannot get device information!" << endl;
		exit(1);
	}

	pRow_Installation = pRow_Device->FK_Installation_getrow();

	if( !g_bSkipConfirmRelations )
	{
		CreateDevice createDevice(pRow_Installation->PK_Installation_get(),dceConfig.m_sDBHost,dceConfig.m_sDBUser,dceConfig.m_sDBPassword,dceConfig.m_sDBName,dceConfig.m_iDBPort);
		createDevice.ConfirmRelations(pRow_Device->PK_Device_get(),0,NULL,true);

		if( pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_get()==DEVICECATEGORY_Core_CONST )
		{
			// For each of the media directors we also want to see if extra devices (like plugins) are needed on the core
			vector<Row_Device *> vectRow_Device_MD;
			database_pluto_main.Device_get()->GetRows("JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_DeviceCategory=" + StringUtils::itos(DEVICECATEGORY_Media_Director_CONST),&vectRow_Device_MD);
			for(size_t s=0;s<vectRow_Device_MD.size();++s)
				createDevice.ConfirmRelations(vectRow_Device_MD[s]->PK_Device_get(),0,NULL,true,true);
		}
	}

	CheckDevice(pRow_Device,bSourceCode);
	/*
	if( bIncludeDisklessMD )
	{
	if( pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_get()==DEVICECATEGORY_Core_CONST )
	{
	// If this is a core, find all other devices in this installation that are media directors and which are set to diskless boot
	vector<Row_Device *> vectAllDevices;
	pRow_Device->FK_Installation_getrow()->Device_FK_Installation_getrows(&vectAllDevices);
	for(size_t s=0;s<vectAllDevices.size();++s)
	{
	Row_Device *pRow_Device = vectAllDevices[s];
	if( pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_get()==DEVICECATEGORY_Media_Director_CONST )
	{
	Row_Device_DeviceData *pRow_Device_DeviceData = database_pluto_main.Device_DeviceData_get()->GetRow(pRow_Device->PK_Device_get(),DEVICEDATA_Diskless_Boot_CONST);
	if( pRow_Device_DeviceData && pRow_Device_DeviceData->IK_DeviceData_get()=="1" )
	CheckDevice(pRow_Device,bSourceCode);
	}
	}
	}
	}
	*/
	if( !pRow_Distro )
		return 0;

	if( sCommand=="install" )
	{
		if( iPK_Orbiter )
			cout << "/usr/pluto/bin/MessageSend dcerouter 0 " << iPK_Orbiter << " 1 809 70 install 182 NONE 9 'Installing software.  Please wait...'" << endl;
		
		cout << "echo -e '\tInstalling software.  Please wait...' >> /var/log/pluto/LaunchManager.progress.log" << endl;
		
		list<PackageInfo *>::iterator it;
		for(it=listPackageInfo.begin(); it!=listPackageInfo.end(); ++it)
		{
			PackageInfo *pPackageInfo = *it;
			if( pPackageInfo->m_bAlreadyInstalled )
			{
				cout << "\t# PK_Package: " << pPackageInfo->m_pRow_Package_Source->FK_Package_get() << endl;
				cout << "\t# Rep. type: " << pPackageInfo->m_pRow_Package_Source->FK_RepositorySource_getrow()->FK_RepositoryType_get() << endl;
				cout << "\t# Package: " << pPackageInfo->m_pRow_Package_Source->FK_Package_getrow()->Description_get()
					<< " Type: " << pPackageInfo->m_pRow_Package_Source->FK_RepositorySource_getrow()->FK_RepositoryType_getrow()->Description_get() << endl;
				cout << "\t# ****** SKIPPING --- PACKAGE ALREADY INSTALLED ******" << endl;
				continue;
			}

			cout << "ok=0" << endl;
			cout << "while [ \"$ok\" -eq 0 ]; do" << endl;
			cout << "\tdpkg --configure -a --force-confold" << endl;
			//			cout << endl << "-----------------------------------------------------" << endl;
			cout << "\t# PK_Package: " << pPackageInfo->m_pRow_Package_Source->FK_Package_get() << endl;
			cout << "\t# Rep. type: " << pPackageInfo->m_pRow_Package_Source->FK_RepositorySource_getrow()->FK_RepositoryType_get() << endl;
			cout << "\t# Package: " << pPackageInfo->m_pRow_Package_Source->FK_Package_getrow()->Description_get()
				<< " Type: " << pPackageInfo->m_pRow_Package_Source->FK_RepositorySource_getrow()->FK_RepositoryType_getrow()->Description_get() << endl;

			if( iPK_Orbiter )
				cout << "/usr/pluto/bin/MessageSend dcerouter 0 " << iPK_Orbiter << " 1 809 70 install 182 NONE 9 'Downloading " << pPackageInfo->m_pRow_Package_Source->FK_Package_getrow()->Description_get() << "...'" << endl;
			
			cout << "echo -e '\tDownloading " << pPackageInfo->m_pRow_Package_Source->FK_Package_getrow()->Description_get() << "...' >> /var/log/pluto/LaunchManager.progress.log" << endl;

			/*
			on package 277, it tried the targz before the DEB
			makerelease isn't building all and isn't updating the versions
			*/
			InstallPackage(pPackageInfo);

			if( iPK_Orbiter )
				cout << "/usr/pluto/bin/MessageSend dcerouter 0 " << iPK_Orbiter << " 1 809 70 install 182 5 9 'Downloading " << pPackageInfo->m_pRow_Package_Source->FK_Package_getrow()->Description_get() << " done'" << endl;
			
			cout << "echo -e '\tDownloading " << pPackageInfo->m_pRow_Package_Source->FK_Package_getrow()->Description_get() << " done' >> /var/log/pluto/LaunchManager.progress.log" << endl;

			/* AB 1/26/05 - for the moment I'm going to comment this out so we don't keep trying alternate sources.  If one source failes, it really is a failure and we should stop
			if (pPackageInfo->m_pRow_Package_Source->FK_RepositorySource_getrow()->FK_RepositoryType_get() != REPOSITORYTYPE_PACKAGE_CONST)
			{
			for(size_t s=0;s<pPackageInfo->m_vectPackageInfo.size();++s)
			{
			PackageInfo *pPackageInfoAlt = pPackageInfo->m_vectPackageInfo[s];
			cout << "\t# PK_Package: " << pPackageInfoAlt->m_pRow_Package_Source->FK_Package_get() << endl;
			cout << "\t# Rep. type: " << pPackageInfoAlt->m_pRow_Package_Source->FK_RepositorySource_getrow()->FK_RepositoryType_get() << endl;
			cout << "\t# Package: " << pPackageInfoAlt->m_pRow_Package_Source->FK_Package_getrow()->Description_get()
			<< " Type: " << pPackageInfoAlt->m_pRow_Package_Source->FK_RepositorySource_getrow()->FK_RepositoryType_getrow()->Description_get() << endl;
			InstallPackage(pPackageInfoAlt, true);
			}
			}
			*/
			cout << "\telse" << endl;
			//			cout << "\techo '**ERROR** Unable to get package " << pPackageInfo->m_pRow_Package_Source->FK_Package_getrow()->Description_get() << "'" << endl;


#define EOL "\n"
			if( iPK_Orbiter )
				cout << "/usr/pluto/bin/MessageSend dcerouter 0 " << iPK_Orbiter << " 1 809 70 install 182 5 9 'Failed to get " << pPackageInfo->m_pRow_Package_Source->FK_Package_getrow()->Description_get() << "'" << endl;
			
			cout << "echo -e '\tFailed to get " << pPackageInfo->m_pRow_Package_Source->FK_Package_getrow()->Description_get() << "' >> /var/log/pluto/LaunchManager.progress.log" << endl;

			cout <<
				"\t\techo \"***************************************************\"" EOL
				"\t\techo \"***ERROR*** Processing of package '" + pPackageInfo->m_pRow_Package_Source->FK_Package_getrow()->Description_get() + "' failed\"" EOL
				//"\t\techo \"I tried every known option to find this software. " EOL
				//"\t\tYou will need\"" EOL
				//"\t\techo \"to get it manually.  Press any key to continue.\"" EOL
				"\t\techo \"***************************************************\"" EOL;

			if( bInteractive )
			{
				cout << "\t\techo -n 'Do you want to try again? [Y/n]: '" EOL
					"\t\tread answer" EOL
					"\t\tif [ \"$answer\" == n -o \"$answer\" == N ]; then" EOL
					"\t\t\techo '*** Leaving package uninstalled'" EOL
					"\t\t\tok=1" EOL
					"\t\t\terror=1" EOL
					"\t\tfi" EOL
					;
			}
			else
			{
				cout << "\t\techo \"01 Error processing package " + pPackageInfo->m_pRow_Package_Source->FK_Package_getrow()->Description_get() + "\" >> /var/log/pluto/ConfirmDependencies.log" EOL
					"\t\tok=1" EOL
					"\t\terror=1" EOL
					;
			}

			cout << "\tfi" << endl;
			cout << "done" << endl;
		}
		for(size_t s=0;s<m_vectDevicesNeedingConfigure.size();++s)
		{
			Row_Device *pRow_Device = m_vectDevicesNeedingConfigure[s];
			if( pRow_Device->FK_DeviceTemplate_getrow()->ConfigureScript_get().size() )
			{
				if( iPK_Orbiter )
					cout << "/usr/pluto/bin/MessageSend dcerouter 0 " << iPK_Orbiter << " 1 809 70 install 182 NONE 9 'Configuring " << pRow_Device->Description_get() << "...'" << endl;
				
				cout << "echo -e '\tConfiguring " << pRow_Device->Description_get() << "...' >> /var/log/pluto/LaunchManager.progress.log" << endl;

				cout << "#Running configure script" << endl;
				cout << "/usr/pluto/bin/" << pRow_Device->FK_DeviceTemplate_getrow()->ConfigureScript_get()
					<< " -d " + StringUtils::itos(pRow_Device->PK_Device_get()) << " -i \"" << pRow_Device->IPaddress_get()
					<< "\" -m \"" + pRow_Device->MACaddress_get() + "\"" << endl;

				cout << "if [[ $? -eq 0 ]]; then" << endl
					<< "\tQ=\"UPDATE Device SET Status='',psc_mod=psc_mod WHERE PK_Device='" << pRow_Device->PK_Device_get() << "'\"" << endl
					<< "\tRunSQL \"$Q\"" << endl
					<< "fi" << endl;

				if( iPK_Orbiter )
					cout << "/usr/pluto/bin/MessageSend dcerouter 0 " << iPK_Orbiter << " 1 809 70 install 182 5 9 'Configuring " << pRow_Device->Description_get() << " done.'" << endl;
				
				cout << "echo -e '\tConfiguring " << pRow_Device->Description_get() << " done.' >> /var/log/pluto/LaunchManager.progress.log" << endl;
			}
			else
			{
				cout << "#No configure script" << endl;
				cout << "\tQ=\"UPDATE Device SET Status='',psc_mod=psc_mod WHERE PK_Device='" << pRow_Device->PK_Device_get() << "'\"" << endl
					 << "\tRunSQL \"$Q\"" << endl;
			}

			cout << "#Cleared RUN_CONFIG flag" << endl;
		}
		for(size_t s=0;s<m_vectDevicesNeedingIR.size();++s)
		{
			Row_Device *pRow_Device = m_vectDevicesNeedingIR[s];
			cout << "#Running I/R script" << endl;
			cout << "/usr/pluto/bin/WebDB_GetIR.sh 0 " << pRow_Device->FK_DeviceTemplate_get() << endl;
		}
		database_pluto_main.Device_get()->Commit();

		for(it=listPackageInfo.begin(); it!=listPackageInfo.end(); ++it)
		{
			PackageInfo *pPackageInfo = *it;
			VerifyFiles(pPackageInfo,pPackageInfo->m_vectRow_Package_Directory_File_BinaryExecutibles,pPackageInfo->m_sBinaryExecutiblesPathPath);
			VerifyFiles(pPackageInfo,pPackageInfo->m_vectRow_Package_Directory_File_SourceIncludes,pPackageInfo->m_sSourceIncludesPath);
			VerifyFiles(pPackageInfo,pPackageInfo->m_vectRow_Package_Directory_File_SourceImplementation,pPackageInfo->m_sSourceImplementationPath);
			VerifyFiles(pPackageInfo,pPackageInfo->m_vectRow_Package_Directory_File_BinaryLibrary,pPackageInfo->m_sBinaryLibraryPath);
			VerifyFiles(pPackageInfo,pPackageInfo->m_vectRow_Package_Directory_File_Configuration,pPackageInfo->m_sConfiguration);
		}

		cout <<
			"if [[ \"$error\" -ne 0 ]]; then" EOL
			"\techo \"Note: Some packages failed to install\"" EOL
			"fi" EOL
			;

		if( iPK_Orbiter )
			cout << "/usr/pluto/bin/MessageSend dcerouter 0 " << iPK_Orbiter << " 1 809 70 install 182 5 9 'Installing software finished.'" << endl;
		
		cout << "echo -e '\tInstalling software finished.' >> /var/log/pluto/LaunchManager.progress.log" << endl;

		cout << "rm -f /usr/pluto/install/.notdone" << endl;
		cout << "echo \"*************************\"" << endl;
		cout << "echo \"*************************\"" << endl;
		cout << "echo \"Installation complete.\"" << endl;
		cout << "echo \"*************************\"" << endl;
		cout << "echo \"*************************\"" << endl;
	}
	else if( sCommand=="build" || sCommand=="buildall" )
	{
		list<PackageInfo *>::iterator it;
		for(it=listPackageInfo.begin(); it!=listPackageInfo.end(); ++it)
		{
			PackageInfo *pPackageInfo = *it;
			string s=pPackageInfo->m_pRow_Package_Source->FK_Package_getrow()->Description_get();
			if( s=="Pluto Standard Plugins Source" )
			{
				int k=2;
			}
			if( pPackageInfo->m_pRow_Package_Directory_Compiled_Output==NULL ||
				pPackageInfo->m_pRow_Package_Source->FK_Package_getrow()->IsSource_get()==0 || (!pPackageInfo->m_bMustBuild && sCommand!="buildall")  )
				continue;

			vector<Row_Package_Directory *> vectRow_Package_Directory_Compiled;
			pPackageInfo->m_pRow_Package_Directory_Compiled_Output->Table_Package_Directory_get()->GetRows(
				"FK_Package=" + StringUtils::itos(pPackageInfo->m_pRow_Package_Directory_Compiled_Output->FK_Package_get()) +
				" AND FK_Directory=" + StringUtils::itos(DIRECTORY_Compiled_Output_CONST),&vectRow_Package_Directory_Compiled);
			for(size_t s=0;s<vectRow_Package_Directory_Compiled.size();++s)
			{
				Row_Package_Directory *pRow_Package_Directory = vectRow_Package_Directory_Compiled[s];

				vector<Row_Package_Directory_File *> vectRow_Package_Directory_File;
				pRow_Package_Directory->Package_Directory_File_FK_Package_Directory_getrows(&vectRow_Package_Directory_File);

				if( vectRow_Package_Directory_File.size()==0 )
					continue;
				cout << "pushd " << pRow_Package_Directory->InputPath_get() << endl;

				for(size_t s=0;s<vectRow_Package_Directory_File.size();++s)
				{
					Row_Package_Directory_File *pRow_Package_Directory_File = vectRow_Package_Directory_File[s];

					if( pRow_Package_Directory_File->FK_Distro_get()==pRow_Distro->PK_Distro_get() ||
						pRow_Package_Directory_File->FK_OperatingSystem_get()==pRow_Distro->FK_OperatingSystem_get() ||
						(pRow_Package_Directory_File->FK_Distro_isNull() && pRow_Package_Directory_File->FK_OperatingSystem_isNull()) )
					{
						cout << pRow_Package_Directory_File->MakeCommand_get() << endl;
						cout << "if [ $? -ne 0 ]; then" << endl;
						cout << "\techo \"** Failed to build package '" << pPackageInfo->m_pRow_Package_Source->FK_Package_getrow()->Description_get() << "'\"" << endl;
						cout << "\texit 1" << endl;
						cout << "fi" << endl;
					}
				}
				cout << "popd" << endl;
			}
		}
	}
	else if( sCommand=="list" )
	{
		list<PackageInfo *>::iterator it;
		for(it=listPackageInfo.begin(); it!=listPackageInfo.end(); ++it)
		{
			PackageInfo *pPackageInfo = *it;
			cout << pPackageInfo->m_pRow_Package_Source->FK_Package_get() << "|" << pPackageInfo->m_pRow_Package_Source->FK_Package_getrow()->Description_get() << endl;
		}
	}
	else if(sCommand == "listall")
	{
		list<PackageInfo *>::iterator it;
		for(it=listPackageInfo.begin(); it!=listPackageInfo.end(); ++it)
		{
			PackageInfo *pPackageInfo = *it;

			cout << 
				pPackageInfo->m_pRow_Package_Source->FK_Package_get() << "|" << 
				pPackageInfo->m_pRow_Package_Source->Name_get() << "|" <<
				pPackageInfo->m_pRow_RepositorySource_URL->URL_get() << "|" <<
				pPackageInfo->m_pRow_Package_Source_Compat->Comments_get() << "|" <<
				pPackageInfo->m_pRow_Package_Source->Repository_get() << "|" <<
				pPackageInfo->m_pRow_Package_Source->FK_RepositorySource_getrow()->FK_RepositoryType_get() << "|" <<
				pPackageInfo->m_pRow_Package_Source->Version_get() << "|" <<
				pPackageInfo->m_sBinaryExecutiblesPathPath << "|" <<
				pPackageInfo->m_sSourceIncludesPath << "|" <<
				pPackageInfo->m_sSourceImplementationPath << "|" <<
				pPackageInfo->m_sBinaryLibraryPath << "|" <<
				pPackageInfo->m_sConfiguration << "|" <<
				pPackageInfo->m_pRow_RepositorySource_URL->Username_get() << "|" <<
				pPackageInfo->m_pRow_RepositorySource_URL->Password_get() << "|" <<
				pPackageInfo->m_pRow_Package_Source->Parms_get() << "|" <<
				pPackageInfo->m_pRow_Package_Source->FK_Package_getrow()->Description_get() << endl;

			if( pPackageInfo->m_pRow_Package_Source->FK_RepositorySource_getrow()->FK_RepositoryType_get()==REPOSITORYTYPE_CVS_CONST ||
				pPackageInfo->m_pRow_Package_Source->FK_RepositorySource_getrow()->FK_RepositoryType_get()==REPOSITORYTYPE_Subversion_SVN_CONST )
			{
				for(size_t s=0;s<pPackageInfo->m_vectAltSourceImplementation.size();++s)
				{
					Row_Package_Directory *pRow_Package_Directory = pPackageInfo->m_vectAltSourceImplementation[s];
					// Same thing as above but with alternate source implementation
					cout << 
						pPackageInfo->m_pRow_Package_Source->FK_Package_get() << "|" << 
						pPackageInfo->m_pRow_Package_Source->Name_get() << "|" <<
						pPackageInfo->m_pRow_RepositorySource_URL->URL_get() << "|" <<
						pPackageInfo->m_pRow_Package_Source_Compat->Comments_get() << "|" <<
						pPackageInfo->m_pRow_Package_Source->Repository_get() << "|" <<
						pPackageInfo->m_pRow_Package_Source->FK_RepositorySource_getrow()->FK_RepositoryType_get() << "|" <<
						pPackageInfo->m_pRow_Package_Source->Version_get() << "|" <<
						pPackageInfo->m_sBinaryExecutiblesPathPath << "|" <<
						pPackageInfo->m_sSourceIncludesPath << "|" <<
						pRow_Package_Directory->Path_get() << "|" <<
						pPackageInfo->m_sBinaryLibraryPath << "|" <<
						pPackageInfo->m_sConfiguration << "|" <<
						pPackageInfo->m_pRow_RepositorySource_URL->Username_get() << "|" <<
						pPackageInfo->m_pRow_RepositorySource_URL->Password_get() << "|" <<
						pPackageInfo->m_pRow_Package_Source->Parms_get() << "|" <<
						pPackageInfo->m_pRow_Package_Source->FK_Package_getrow()->Description_get() << endl;
				}
			}
		}
	}
	// The ConfirmDependencies program specific to this distro will be given the following parameters:
	// 1. Repository Source -- A URL, such as http://ftp.debian.org
	// 2. Repository -- such as Woody, or Stable
	// 3. Type -- 1: Package	2: Link to source  3: Archive
	// 4. Version
	// And then the following, which are directories or files and indicate where this program will expect to find the file
	// The ConfirmDependencies may choose to ignore these, such as with packages, since they define their own locations
	// 5. Binary Executibles
	// 6. Source Includes
	// 7. Source Implementation
	// 8. Binary Library
	// 9. Configuration
	/*

	if( pRow_RepositorySource_URL )
	cout << " \"" << pRow_RepositorySource_URL->URL_get() << "\"";
	else
	cout << " \"\"";

	cout << " \"" << pRow_Package_Source->Repository_get() << "\""
	<< " \"" << pRow_Package_Source->FK_RepositoryType_get() << "\""
	<< " \"" << pRow_Package_Source->Version_get() << "\"";


	}
	}
	*/
	return 0;
}

void CheckDevice(Row_Device *pRow_Device,bool bSourceCode)
{
	Database_pluto_main *pDatabase_pluto_main = pRow_Device->Table_Device_get()->Database_pluto_main_get();
	bool bDevelopment = bSourceCode;
	if( !bDevelopment )
	{
		Row_Device_DeviceData *pRow_Device_DeviceData = pDatabase_pluto_main->Device_DeviceData_get()->GetRow(pRow_Device->PK_Device_get(),DEVICEDATA_Development_CONST);
		bDevelopment = pRow_Device_DeviceData && pRow_Device_DeviceData->IK_DeviceData_get()=="1";
	}
	if( iPK_Distro==-1 )
	{
		Row_Device_DeviceData *pRow_Device_DeviceData = pDatabase_pluto_main->Device_DeviceData_get()->GetRow(pRow_Device->PK_Device_get(),DEVICEDATA_PK_Distro_CONST);
                cout << "# No -o specified for distro.  Found device data?" << (pRow_Device_DeviceData ? "Y" : "N") << endl;
		if( pRow_Device_DeviceData )
			iPK_Distro = atoi(pRow_Device_DeviceData->IK_DeviceData_get().c_str());
		else if( dceConfig.m_iPK_Distro )
			iPK_Distro = dceConfig.m_iPK_Distro;
	}

	pRow_Distro = pDatabase_pluto_main->Distro_get()->GetRow(iPK_Distro);
	if( !pRow_Distro )
		pRow_Distro = pDatabase_pluto_main->Distro_get()->GetRow(1);

        cout << "# Using Distro: " << pRow_Distro->PK_Distro_get() << endl;
	
	CheckDeviceLoop(pRow_Device,bDevelopment);
}

void CheckDeviceLoop(Row_Device *pRow_Device,bool bDevelopment)
{
	if( pRow_Device->Status_get()=="**RUN_CONFIG**" )
		m_vectDevicesNeedingConfigure.push_back(pRow_Device);
	else if( pRow_Device->Status_get()=="**RUN_GET_IR**" )
		m_vectDevicesNeedingIR.push_back(pRow_Device);

	if( pRow_Device->FK_DeviceTemplate_getrow()->FK_Package_isNull() )
	{
		if( sCommand!="line" )
			cout << "#Device: " << pRow_Device->Description_get() << " (#" << pRow_Device->FK_DeviceTemplate_get() << " " << pRow_Device->FK_DeviceTemplate_getrow()->Description_get() << ") doesn't require any software." << endl;
	}
	else
	{
		if (sCommand == "install" || sCommand == "view")
		{
			cout << "#Device: " << pRow_Device->Description_get() << " (#" << pRow_Device->FK_DeviceTemplate_get() << " " << pRow_Device->FK_DeviceTemplate_getrow()->Description_get() << ") requires software" << endl;
		}
		string PkgName = pRow_Device->FK_DeviceTemplate_getrow()->FK_Package_getrow()->Description_get();
		CheckPackage(pRow_Device->FK_DeviceTemplate_getrow()->FK_Package_getrow(),pRow_Device,bDevelopment,pRow_Distro,false);
		Row_DeviceCategory *pRow_DeviceCategory = pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_getrow();
		if( pRow_DeviceCategory && (pRow_DeviceCategory->PK_DeviceCategory_get()==DEVICECATEGORY_Media_Director_CONST ||
			pRow_DeviceCategory->FK_DeviceCategory_Parent_get()==DEVICECATEGORY_Media_Director_CONST) )
		{
			// This is a media director, check the QuickStart tables too
			vector<Row_Device_QuickStart *> vectRow_Device_QuickStart;
			pRow_Device->Device_QuickStart_FK_Device_getrows(&vectRow_Device_QuickStart);
			for(size_t s=0;s<vectRow_Device_QuickStart.size();++s)
			{
				Row_Device_QuickStart *pRow_Device_QuickStart = vectRow_Device_QuickStart[s];
				Row_QuickStartTemplate *pRow_QuickStartTemplate = pRow_Device_QuickStart->FK_QuickStartTemplate_getrow();
				if( pRow_QuickStartTemplate && pRow_QuickStartTemplate->FK_Package_get() )
				{
					CheckPackage(pRow_QuickStartTemplate->FK_Package_getrow(),pRow_Device,bDevelopment,pRow_Distro,false);
				}
			}
		}
	}

	vector<Row_Device *> vectRow_Device;
	pRow_Device->Device_FK_Device_ControlledVia_getrows(&vectRow_Device);
	for(size_t s=0;s<vectRow_Device.size();++s)
		CheckDeviceLoop( vectRow_Device[s],bDevelopment );
}

void CheckPackage(Row_Package *pRow_Package,Row_Device *pRow_Device,bool bDevelopment,Row_Distro *pRow_Distro,bool bMustBuildFromSource, int depth)
{
	Database_pluto_main *pDatabase_pluto_main = pRow_Distro->Table_Distro_get()->Database_pluto_main_get();
	vector<Row_Package_Compat *> vectRow_Package_Compat;
	pRow_Package->Package_Compat_FK_Package_getrows(&vectRow_Package_Compat);
	bool bFound=false;
	for(size_t s=0;s<vectRow_Package_Compat.size();++s)
	{
		Row_Package_Compat *pRow_Package_Compat = vectRow_Package_Compat[s];
		if( pRow_Package_Compat->FK_Distro_get()==pRow_Distro->PK_Distro_get() ||
			(pRow_Package_Compat->FK_Distro_isNull() && pRow_Package_Compat->FK_OperatingSystem_get()==pRow_Distro->FK_OperatingSystem_get()) ||
			(pRow_Package_Compat->FK_Distro_isNull() && pRow_Package_Compat->FK_OperatingSystem_isNull()) )
		{
			bFound=true;
			break;
		}
	}

	string PkgName = pRow_Package->Description_get();

	if( !bFound )
	{
		cout << "# Skipping package " << PkgName << " because it is not compatible with this distro" << endl;
		return;
	}

	cout << "#";
	for (int i = 0; i < depth; i++)
		cout << ".";
	cout << PkgName << endl;

	// Start with the dependencies first, since we want the lowest dependency, and then the top one
	vector<Row_Package_Package *> vectRow_Package_Package;
	pRow_Package->Package_Package_FK_Package_getrows(&vectRow_Package_Package);
	for(size_t s=0;s<vectRow_Package_Package.size();++s)
	{
		Row_Package_Package *pRow_Package_Package = vectRow_Package_Package[s];

		// Skip this package if it's for development only and this is not a development box
		if( pRow_Package_Package->OnlyToBuild_get()==1 && !bDevelopment || g_mapPackages_AlreadyChecked[pRow_Package_Package->FK_Package_DependsOn_get()])
			continue;

		g_mapPackages_AlreadyChecked[pRow_Package_Package->FK_Package_DependsOn_get()]=true;

		// Development = false because we only assume the first level of packages are intended to be built
		// from source
		CheckPackage(pRow_Package_Package->FK_Package_DependsOn_getrow(),pRow_Device,bDevelopment,pRow_Distro,false, depth + 1);
	}

	if( bDevelopment && !pRow_Package->FK_Package_Sourcecode_isNull() )
	{
		CheckPackage(pRow_Package->FK_Package_Sourcecode_getrow(),pRow_Device,bDevelopment,pRow_Distro,false, depth + 1);
	}

	// If the command is not 'view' or 'status', we don't need to list each package over and over again within
	// each of the dependencies.  For these commands, we just create a single list of all the packages, so if
	// we already got the info for this package, we don't need to do it again
	if(	sCommand!="view" && sCommand!="status" )
	{
		list<PackageInfo *>::iterator it;
		for(it=listPackageInfo.begin(); it!=listPackageInfo.end(); ++it)
		{
			PackageInfo *pPackageInfo = *it;
			if( pPackageInfo->m_pRow_Package_Source->FK_Package_get()==pRow_Package->PK_Package_get() )
				return;
		}
	}

	if( pRow_Package->PK_Package_get()==280 )
	{
		int k=2;
	}

	// If we find a source specific for our distro, that does not require a build from source code,
	// that will be considered a perfect match, and added here.
	vector<Row_Package_Source_Compat *> vectRow_Package_Source_Compat_Match;
	// If we find a source that is compatible with our operating system, but not this distro of it,
	// and doesn't require a build from source, we'll set add it here
	vector<Row_Package_Source_Compat *> vectRow_Package_Source_Compat_OS;

	// All sources that are compatible will be put here, so we have some alternates if we can't install from the 
	// primary
	vector<Row_Package_Source_Compat *> vectRow_Package_Source_Compat;

	vector<Row_Package_Source *> vectRow_Package_Source;
	pRow_Package->Package_Source_FK_Package_getrows(&vectRow_Package_Source);
	for(size_t s=0;s<vectRow_Package_Source.size();++s)
	{
		Row_Package_Source *pRow_Package_Source = vectRow_Package_Source[s];
		vector<Row_Package_Source_Compat *> vectRPSC;
		pRow_Package_Source->Package_Source_Compat_FK_Package_Source_getrows(&vectRPSC);

		// We've got 3 lists we can put this in.  We'll put it in whatever one is the best
		// We'll update the following when it's a match, and take the best.  The exception is the 
		// source code.  In that case we're not going to look for a source that is specific to 
		// our distribution because then we would always choose to take from a package before SVN, for example,
		// if that's what the user prefers.  So if it's source, just put them all in the "compat"
		// vector and we'll pick the user's preference
		Row_Package_Source_Compat *pRPSC_Match=NULL,*pRPSC_OS=NULL,*pRPSC_Comp=NULL;

		for(size_t t=0;t<vectRPSC.size();++t)
		{
			Row_Package_Source_Compat *pRPSC = vectRPSC[t];
			if( pRPSC->FK_Distro_get()==pRow_Distro->PK_Distro_get() && !pRPSC->MustBuildFromSource_get() )
				pRPSC_Match=pRPSC; // This is considered the perfect match
			else if( pRPSC->FK_Distro_isNull() && pRPSC->FK_OperatingSystem_get()==pRow_Distro->FK_OperatingSystem_get() && !pRPSC->MustBuildFromSource_get() )
				pRPSC_OS=pRPSC; // It matched the OS, so we'll use it if we don't find an exact match for the distro

			if( (pRPSC->FK_Distro_isNull() || pRow_Distro->PK_Distro_get()==pRPSC->FK_Distro_get()) && 
				(pRPSC->FK_OperatingSystem_isNull() || pRow_Distro->FK_OperatingSystem_get()==pRPSC->FK_OperatingSystem_get()) )
				pRPSC_Comp=pRPSC;
		}
		if( pRow_Package->IsSource_get()==1 && pRPSC_Comp )  // See comments above
			vectRow_Package_Source_Compat.push_back( pRPSC_Comp );
		else if( pRPSC_Match )
			vectRow_Package_Source_Compat_Match.push_back(pRPSC_Match); // This is considered the perfect match
		else if( pRPSC_OS )
			vectRow_Package_Source_Compat_OS.push_back(pRPSC_OS); // It matched the OS, so we'll use it if we don't find an exact match for the distro
		else if( pRPSC_Comp )
			vectRow_Package_Source_Compat.push_back( pRPSC_Comp );
	}

	// This will be the preferred source
	Row_Package_Source_Compat *pRow_Package_Source_Compat_Preferred=NULL;

	// The installation table will contain the users preferred repository type for getting this package
	// The FindPreferredSource function will search through the vect we pass, and if it finds a source that
	// matches the preferred type, it will return it.  Otherwise it will just return the first
	// entry in the vector.  We will give preference first to sources that were an exact match on our
	// distro, then sources that at least match the operating system, and lastly all other compatible
	// sources, which likely require building from source
	if( vectRow_Package_Source_Compat_Match.size() )
		pRow_Package_Source_Compat_Preferred = FindPreferredSource(vectRow_Package_Source_Compat_Match,pRow_Device);
	else if( vectRow_Package_Source_Compat_OS.size() )
		pRow_Package_Source_Compat_Preferred = FindPreferredSource(vectRow_Package_Source_Compat_OS,pRow_Device);
	else if( vectRow_Package_Source_Compat.size() )
		pRow_Package_Source_Compat_Preferred = FindPreferredSource(vectRow_Package_Source_Compat,pRow_Device);

	if( sCommand=="view" || sCommand=="status" )
		cout << "PROGRAM: " << pRow_Package->Description_get() << endl;

	if( !pRow_Package_Source_Compat_Preferred )
	{
		if( !pRow_Package->FK_Package_Sourcecode_isNull() )
		{
			// We can only build this package from source.  The binaries are not available
			CheckPackage(pRow_Package->FK_Package_Sourcecode_getrow(),pRow_Device,bDevelopment,pRow_Distro,true);
			return;
		}
		else
		{
			if( m_mapReportedErrors.find(pRow_Package->PK_Package_get())==m_mapReportedErrors.end() )
			{
				m_mapReportedErrors[pRow_Package->PK_Package_get()]=pRow_Package;
				if( sCommand=="list" )
					cout << pRow_Package->PK_Package_get() << "|" << "*ERROR* " << pRow_Package->Description_get() << " not found for this distro" << endl;
				else
					//cout << "#*ERROR* " << pRow_Package->Description_get() << " not found for this distro" << endl;
					cout << "#" << pRow_Package->PK_Package_get() << "," << pRow_Package->Description_get() << "," <<
					"#*ERROR* " << pRow_Package->Description_get() << " not found for this distro" << endl;
			}
			return;
		}
	}

	// Get the info for the primary match
	PackageInfo *pPackageInfo = MakePackageInfo(pRow_Package_Source_Compat_Preferred,bMustBuildFromSource);
	if( !pPackageInfo  )
		return;
	listPackageInfo.push_back( pPackageInfo );

	// This will add the alternate sources from the given vects to pPackageInfo
	AddAlternateSources(vectRow_Package_Source_Compat_Match,pPackageInfo);
	AddAlternateSources(vectRow_Package_Source_Compat_OS,pPackageInfo);
	AddAlternateSources(vectRow_Package_Source_Compat,pPackageInfo);
}

Row_Package_Source_Compat *FindPreferredSource(vector<Row_Package_Source_Compat *> vectRow_Package_Source_Compat,Row_Device *pRow_Device)
{
	// Try to find a repository source that matches the preferred type for retrieving source/binaries for this installation
	for(size_t s=0;s<vectRow_Package_Source_Compat.size();++s)
	{
		Row_Package_Source_Compat *pRow_Package_Source_Compat=vectRow_Package_Source_Compat[s];

		if( pRow_Package_Source_Compat->FK_Package_Source_getrow()->FK_Package_getrow()->IsSource_get()==1 )
		{
			// Assume the user wants svn for source code if there's no preference in the database
			if( pRow_Device->FK_Installation_getrow()->FK_RepositoryType_Source_isNull() && 
				pRow_Package_Source_Compat->FK_Package_Source_getrow()->FK_RepositorySource_getrow()->FK_RepositoryType_get() == REPOSITORYTYPE_Subversion_SVN_CONST )
				return pRow_Package_Source_Compat;

			if( pRow_Package_Source_Compat->FK_Package_Source_getrow()->FK_RepositorySource_getrow()->FK_RepositoryType_get() ==
				pRow_Device->FK_Installation_getrow()->FK_RepositoryType_Source_get() )
				return pRow_Package_Source_Compat;
		}
		else
		{
			// Assume the user wants packages if there's no preference in the database
			if( pRow_Device->FK_Installation_getrow()->FK_RepositoryType_Source_isNull() && 
				pRow_Package_Source_Compat->FK_Package_Source_getrow()->FK_RepositorySource_getrow()->FK_RepositoryType_get() == REPOSITORYTYPE_PACKAGE_CONST )
				return pRow_Package_Source_Compat;

			if( pRow_Package_Source_Compat->FK_Package_Source_getrow()->FK_RepositorySource_getrow()->FK_RepositoryType_get() ==
				pRow_Device->FK_Installation_getrow()->FK_RepositoryType_Binaries_get() )
				return pRow_Package_Source_Compat;
		}
	}

	// Otherwise return the first.  We'll be trying all the others anyway if it fails.  The preferred only determines our first choice
	return vectRow_Package_Source_Compat[0];
}

void AddAlternateSources(vector<Row_Package_Source_Compat *> &vectRow_Package_Source_Compat,PackageInfo *pPackageInfo)
{
	// See if we can find some alternates if the primary fails
	for(size_t s=0;s<vectRow_Package_Source_Compat.size();++s)
	{
		Row_Package_Source_Compat *pRow_Package_Source_Compat = vectRow_Package_Source_Compat[s];
		if( pRow_Package_Source_Compat!=pPackageInfo->m_pRow_Package_Source_Compat )
		{
			// This isn't our primary, add this to the list of alternates
			PackageInfo *pPackageInfo_local = MakePackageInfo(pRow_Package_Source_Compat,pRow_Package_Source_Compat->MustBuildFromSource_get()==1);
			if( pPackageInfo_local )
				pPackageInfo->m_vectPackageInfo.push_back( pPackageInfo_local );
		}
	}
}

PackageInfo *MakePackageInfo(Row_Package_Source_Compat *pRow_Package_Source_Compat,bool bMustBuildFromSource)
{
	if (pRow_Package_Source_Compat == NULL)
	{
		cout << "# Got NULL" << endl;
		return NULL;
	}
	if( pRow_Package_Source_Compat->FK_Package_Source_getrow()->FK_Package_get()==135 )
	{
		int k=2;
	}

	Database_pluto_main *pDatabase_pluto_main = pRow_Package_Source_Compat->Table_Package_Source_Compat_get()->Database_pluto_main_get();
	Row_Package *pRow_Package = pRow_Package_Source_Compat->FK_Package_Source_getrow()->FK_Package_getrow();

	// First find the available repository sources
	Row_Package_Source *pRow_Package_Source = pRow_Package_Source_Compat->FK_Package_Source_getrow();
	Row_RepositorySource *pRow_RepositorySource = pRow_Package_Source->FK_RepositorySource_getrow();
	Row_RepositorySource_URL *pRow_RepositorySource_URL=NULL;

	//	if( !pRow_Package_Source_Compat_Match->FK_Package_Source_getrow()->FK_RepositorySource_isNull() )
	//	{
	vector<Row_RepositorySource_URL *> vectRow_RepositorySource_URL;
	pRow_Package_Source->FK_RepositorySource_getrow()->RepositorySource_URL_FK_RepositorySource_getrows(&vectRow_RepositorySource_URL);
	Row_RepositorySource_URL *pRow_RepositorySource_URL_Country=NULL;
	for(size_t t=0;t<vectRow_RepositorySource_URL.size();++t)
	{
		Row_RepositorySource_URL *pRow_RepositorySource_URL_test = vectRow_RepositorySource_URL[t];

		// If this repository is specifically mentioned in the Installation_RepositorySource_URL table, we'll use it.
		Row_Installation_RepositorySource_URL *pInstallation_RepositorySource_URL = 
			pDatabase_pluto_main->Installation_RepositorySource_URL_get()->GetRow(
			pRow_Installation->PK_Installation_get(),
			pRow_RepositorySource_URL_test->PK_RepositorySource_URL_get());

		if( pInstallation_RepositorySource_URL )
		{
			pRow_RepositorySource_URL=pRow_RepositorySource_URL_test;
			break;
		}

		// If this source is at least in the same country we'll consider it a tentative match while we keep looking for an exact match
		if( pRow_Installation->FK_Country_get()==pRow_RepositorySource_URL_test->FK_Country_get() )
			pRow_RepositorySource_URL_Country = pRow_RepositorySource_URL_test;  
	}
	if( !pRow_RepositorySource_URL )
	{
		// We didn't find an exact match, see if we found one in the same country
		if( pRow_RepositorySource_URL_Country )
			pRow_RepositorySource_URL=pRow_RepositorySource_URL_Country;
		// If not, just take the first one
		else if( vectRow_RepositorySource_URL.size() )
			pRow_RepositorySource_URL = vectRow_RepositorySource_URL[0];
	}
	//	}

	if( !pRow_RepositorySource_URL )
	{
		cout << "# PK_Package=" << pRow_Package->PK_Package_get() << "," << pRow_Package->Description_get() << ","
			<< "**ERROR** Cannot find a URL for Repository Source: " 
			<< pRow_RepositorySource->Description_get() << endl;
		return NULL;
	}

	PackageInfo *pPackageInfo = new PackageInfo(pRow_Package_Source_Compat,pRow_Package_Source,pRow_RepositorySource_URL,bMustBuildFromSource);

	Row_Package_Directory *pRow_Package_Directory;

	pRow_Package_Directory = GetDirectory(pRow_Package,DIRECTORY_Binary_Executables_CONST);
	if( pRow_Package_Directory )
	{
		pPackageInfo->m_sBinaryExecutiblesPathPath = pRow_Package_Directory->Path_get();
		pRow_Package_Directory->Package_Directory_File_FK_Package_Directory_getrows(&pPackageInfo->m_vectRow_Package_Directory_File_BinaryExecutibles);
	}

	pRow_Package_Directory = GetDirectory(pRow_Package,DIRECTORY_Source_Includes_CONST);
	if( pRow_Package_Directory )
	{
		pPackageInfo->m_sSourceIncludesPath = pRow_Package_Directory->Path_get();
		pRow_Package_Directory->Package_Directory_File_FK_Package_Directory_getrows(&pPackageInfo->m_vectRow_Package_Directory_File_SourceIncludes);
	}

	pRow_Package_Directory = GetDirectory(pRow_Package,DIRECTORY_Source_Implementation_CONST);
	if( pRow_Package_Directory )
	{
		// For source implementation we may be doing a SVN or CVS checkout.  In that case we will need to do this repeatedly 
		// for all source implementation directories--not just the primary.  Go through the directories again and find
		// any other directories that we may have missed and add them to the vectAlt
		vector<Row_Package_Directory *> vectRow_Package_Directory;
		pRow_Package->Package_Directory_FK_Package_getrows(&vectRow_Package_Directory);
		for(size_t s=0;s<vectRow_Package_Directory.size();++s)
		{
			Row_Package_Directory *pRow_Package_Directory_SI = vectRow_Package_Directory[s];
			if( pRow_Package_Directory_SI->FK_Directory_get()!=DIRECTORY_Source_Implementation_CONST || pRow_Package_Directory_SI==pRow_Package_Directory )
				continue;
			if( pRow_Package_Directory_SI->FK_Distro_get() == pRow_Distro->PK_Distro_get() ||
				pRow_Package_Directory_SI->FK_OperatingSystem_get() == pRow_Distro->FK_OperatingSystem_get() ||
				( pRow_Package_Directory_SI->FK_Distro_isNull() && pRow_Package_Directory_SI->FK_OperatingSystem_isNull() ) )
				pPackageInfo->m_vectAltSourceImplementation.push_back(pRow_Package_Directory_SI);
		}

		pPackageInfo->m_sSourceImplementationPath = pRow_Package_Directory->Path_get();
		pRow_Package_Directory->Package_Directory_File_FK_Package_Directory_getrows(&pPackageInfo->m_vectRow_Package_Directory_File_SourceImplementation);
	}

	pRow_Package_Directory = GetDirectory(pRow_Package,DIRECTORY_Compiled_Output_CONST);
	if( pRow_Package_Directory )
	{
		pPackageInfo->m_pRow_Package_Directory_Compiled_Output = pRow_Package_Directory;
		pRow_Package_Directory->Package_Directory_File_FK_Package_Directory_getrows(&pPackageInfo->m_vectRow_Package_Directory_File_CompiledOutput);
	}


	pRow_Package_Directory = GetDirectory(pRow_Package,DIRECTORY_Binary_Library_CONST);
	if( pRow_Package_Directory )
	{
		pPackageInfo->m_sBinaryLibraryPath = pRow_Package_Directory->Path_get();
		pRow_Package_Directory->Package_Directory_File_FK_Package_Directory_getrows(&pPackageInfo->m_vectRow_Package_Directory_File_BinaryLibrary);
	}

	pRow_Package_Directory = GetDirectory(pRow_Package,DIRECTORY_Configuration_CONST);
	if( pRow_Package_Directory )
	{
		pPackageInfo->m_sConfiguration = pRow_Package_Directory->Path_get();
		pRow_Package_Directory->Package_Directory_File_FK_Package_Directory_getrows(&pPackageInfo->m_vectRow_Package_Directory_File_Configuration);
	}

	if( sCommand=="view" || sCommand=="status" )
	{
		cout << "\tFrom: " << pRow_RepositorySource->Description_get() << "(" << pRow_Package_Source->FK_RepositorySource_getrow()->FK_RepositoryType_getrow()->Description_get() << "\tVersion: " << pRow_Package_Source->Version_get() << endl;
		cout << "\tRepository: " <<  pRow_Package_Source->Repository_get();
		if( pRow_RepositorySource_URL )
			cout << "\tAt: " << pRow_RepositorySource_URL->URL_get() << endl;
	}
	return pPackageInfo;
}

void VerifyFiles(PackageInfo *pPackageInfo,vector<Row_Package_Directory_File *> &vectRow_Package_Directory_File,string Path)
{
}

Row_Package_Directory *GetDirectory(Row_Package *pRow_Package,int PK_Directory)
{
	Row_Package_Directory *pRow_Package_Directory_OS=NULL,*pRow_Package_Directory_Empty=NULL;

	vector<Row_Package_Directory *> vectRow_Package_Directory;
	pRow_Package->Package_Directory_FK_Package_getrows(&vectRow_Package_Directory);
	for(size_t s=0;s<vectRow_Package_Directory.size();++s)
	{
		Row_Package_Directory *pRow_Package_Directory = vectRow_Package_Directory[s];
		if( pRow_Package_Directory->FK_Directory_get()!=PK_Directory )
			continue;
		if( pRow_Package_Directory->FK_Distro_get() == pRow_Distro->PK_Distro_get() )
			return pRow_Package_Directory;  // Perfect match
		else if( pRow_Package_Directory->FK_OperatingSystem_get() == pRow_Distro->FK_OperatingSystem_get() )
			pRow_Package_Directory_OS = pRow_Package_Directory; // We'll use this if we don't find an exact match on the distro
		else if( pRow_Package_Directory->FK_Distro_isNull() && pRow_Package_Directory->FK_OperatingSystem_isNull() )
			pRow_Package_Directory_Empty = pRow_Package_Directory;
	}

	// We didn't find an exact match, return an OS match if there is one
	if( pRow_Package_Directory_OS )
		return pRow_Package_Directory_OS;
	return pRow_Package_Directory_Empty;  // Will be NULL if there is no directory
}

void InstallPackage(PackageInfo *pPackageInfo, bool bElse)
{
	cout << "\t" << (bElse ? "el" : "") << "if /usr/pluto/install/" << pRow_Distro->Installer_get()
		<< " \"" << pPackageInfo->m_pRow_Package_Source->Name_get() << "\""
		<< " \"" << pPackageInfo->m_pRow_RepositorySource_URL->URL_get() << "\""
		<< " \"" << pPackageInfo->m_pRow_Package_Source->Repository_get() << "\""
		<< " \"" << pPackageInfo->m_pRow_Package_Source->FK_RepositorySource_getrow()->FK_RepositoryType_get() << "\""
		<< " \"" << pPackageInfo->m_pRow_Package_Source->Version_get() << "\"" 
		<< " \"" << pPackageInfo->m_sBinaryExecutiblesPathPath << "\"" 
		<< " \"" << pPackageInfo->m_sSourceIncludesPath << "\"" 
		<< " \"" << pPackageInfo->m_sSourceImplementationPath << "\"" 
		<< " \"" << pPackageInfo->m_sBinaryLibraryPath << "\"" 
		<< " \"" << pPackageInfo->m_sConfiguration << "\""
		<< " \"" << pPackageInfo->m_pRow_RepositorySource_URL->Username_get() << "\""
		<< " \"" << pPackageInfo->m_pRow_RepositorySource_URL->Password_get() << "\""
		<< " \"" << pPackageInfo->m_pRow_Package_Source->Parms_get() << "\""
		<< " \"" << pPackageInfo->m_pRow_Package_Source->FK_Package_get() << "\"";

	if( pPackageInfo->m_pRow_Package_Source->FK_RepositorySource_getrow()->FK_RepositoryType_get()==REPOSITORYTYPE_CVS_CONST ||
		pPackageInfo->m_pRow_Package_Source->FK_RepositorySource_getrow()->FK_RepositoryType_get()==REPOSITORYTYPE_Subversion_SVN_CONST )
	{
		for(size_t s=0;s<pPackageInfo->m_vectAltSourceImplementation.size();++s)
		{
			cout << " && /usr/pluto/install/" << pRow_Distro->Installer_get()
				<< " \"" << pPackageInfo->m_pRow_Package_Source->Name_get() << "\""
				<< " \"" << pPackageInfo->m_pRow_RepositorySource_URL->URL_get() << "\""
				<< " \"" << pPackageInfo->m_pRow_Package_Source->Repository_get() << "\""
				<< " \"" << pPackageInfo->m_pRow_Package_Source->FK_RepositorySource_getrow()->FK_RepositoryType_get() << "\""
				<< " \"" << pPackageInfo->m_pRow_Package_Source->Version_get() << "\"" 
				<< " \"" << pPackageInfo->m_sBinaryExecutiblesPathPath << "\"" 
				<< " \"" << pPackageInfo->m_sSourceIncludesPath << "\"" 
				<< " \"" << pPackageInfo->m_vectAltSourceImplementation[s]->Path_get() << "\"" 
				<< " \"" << pPackageInfo->m_sBinaryLibraryPath << "\"" 
				<< " \"" << pPackageInfo->m_sConfiguration << "\""
				<< " \"" << pPackageInfo->m_pRow_RepositorySource_URL->Username_get() << "\""
				<< " \"" << pPackageInfo->m_pRow_RepositorySource_URL->Password_get() << "\""
				<< " \"" << pPackageInfo->m_pRow_Package_Source->Parms_get() << "\"";
		}
	}
	cout << "; then"
		<< endl;
	cout << "\t\techo \"Confirmation of package " << pPackageInfo->m_pRow_Package_Source->FK_Package_get() << " '" << pPackageInfo->m_pRow_Package_Source->FK_Package_getrow()->Description_get() << "' went ok.\"" << endl;
	cout << "\t\tok=1" << endl;
}
