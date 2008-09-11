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
#include "DCEConfig.h"
#include "Logger.h"
#include "CreateDevice.h"
#include "UserUtils.h"

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
#else

#endif

#include "../include/version.h"

using namespace std;
using namespace DCE;
DCEConfig dceConfig;



#include "PlutoUtils/DatabaseUtils.h"

void ExportChildDevices(CreateDevice &createDevice,int iPK_Device_Controlled_Via,string sExportFile)
{
	FILE *file = fopen(sExportFile.c_str(),"wb");
	if( !file )
	{
		cerr << "Cannot open " << sExportFile << endl;
		exit(1);
	}

	string sSQL = "SELECT PK_Device,Device.Description,Room.Description,Room.FK_RoomType,FK_DeviceTemplate,IPaddress,MACaddress FROM Device LEFT JOIN Room ON FK_Room=PK_Room where FK_Device_ControlledVia=" + StringUtils::itos(iPK_Device_Controlled_Via);
	PlutoSqlResult result_child_devices;
	DB_ROW row_child_device;
	if( ( result_child_devices.r=createDevice.db_wrapper_query_result( sSQL ) ) )
	{
		while( row_child_device=db_wrapper_fetch_row( result_child_devices.r ) )
		{
			// output the FK_DeviceTemplate,Device.Description,Room.Description,Room.FK_RoomType,IP Address, Mac Address
			fprintf(file,"%s\t%s\t%s\t%s\t%s\t%s",
				row_child_device[4],
				StringUtils::Escape(row_child_device[1]).c_str(),
				row_child_device[2] ? StringUtils::Escape(row_child_device[2]).c_str() : "",
				row_child_device[3] ? row_child_device[3] : "",
				row_child_device[5] ? row_child_device[5] : "",
				row_child_device[6] ? row_child_device[6] : "");
			
			PlutoSqlResult result_device_data;
			DB_ROW row;
			sSQL = "SELECT FK_DeviceData,IK_DeviceData FROM Device_DeviceData WHERE FK_Device=" + string(row_child_device[0]);
			if( ( result_device_data.r=createDevice.db_wrapper_query_result( sSQL ) ) )
			{
				while( row=db_wrapper_fetch_row( result_device_data.r ) )
				{
					fprintf(file,"\t%s\t%s",
						row[0],
						row[1] ? StringUtils::Escape(row[1]).c_str() : "");
				}
			}
			fprintf(file,"\n");
		};
	}
	fclose(file);
}

void ImportChildDevices(CreateDevice &createDevice,int iPK_Device_Controlled_Via,string sExportFile)
{
	size_t size;
	char *pPtr = FileUtils::ReadFileIntoBuffer(sExportFile,size);
	if( !pPtr )
		return;

	vector<string> vectChildDevices;
	string s = pPtr;
	StringUtils::Tokenize(s,"\r\n",vectChildDevices);
	for(vector<string>::iterator it=vectChildDevices.begin();it!=vectChildDevices.end();++it)
	{
		s = *it;
		string::size_type pos=0;
		int PK_DeviceTemplate = atoi( StringUtils::Tokenize( s, "\t", pos ).c_str() );
		string sDescription = StringUtils::UnEscape(StringUtils::Tokenize( s, "\t", pos ));
		string sRoomName = StringUtils::UnEscape(StringUtils::Tokenize( s, "\t", pos ));
		string sFK_RoomType = StringUtils::Tokenize( s, "\t", pos );
		string sIPAddress = StringUtils::Tokenize( s, "\t", pos );
		string sMacAddress = StringUtils::Tokenize( s, "\t", pos );
		if( PK_DeviceTemplate )
		{
			int PK_Device=createDevice.DoIt(0,PK_DeviceTemplate,"",sIPAddress,sMacAddress,iPK_Device_Controlled_Via,"",0);
			if( PK_Device )
			{
				if( sRoomName.size() )
				{
					int PK_Room = DatabaseUtils::GetRoomByName(&createDevice, sRoomName, atoi(sFK_RoomType.c_str()));
					if( PK_Room )
						DatabaseUtils::SetDeviceInRoom(&createDevice,PK_Device,PK_Room);
				}

				while( pos<s.size() )
				{
					int PK_DeviceData = atoi( StringUtils::Tokenize( s, "\t", pos ).c_str() );
					string sIK_DeviceData = StringUtils::Tokenize( s, "\t", pos );
					DatabaseUtils::SetDeviceData(&createDevice, PK_Device, PK_DeviceData, StringUtils::UnEscape(sIK_DeviceData));
				}
			}
		}
	}
}

int main(int argc, char *argv[])
{
	LoggerWrapper::SetType(LT_LOGGER_FILE,"/var/log/pluto/CreateDevice.log");

	int iPK_DeviceTemplate=0,iPK_DHCPDevice=0,iPK_Device_Controlled_Via=0,iPK_Device_RelatedTo=0;
	string sIPAddress,sMacAddress,sDeviceData,sUserName,sExportFile;
	bool bDontCallConfigureScript=false,bDontInstallPackages=false,bInstallPackagesInBackground=false,bExport=false,bConfirmRelations=false;

	bool bError=false;
	char c;

	for(int optnum=1;optnum<argc;++optnum)
	{
		c=argv[optnum][1];
		switch (c)
		{
		case 'e':
			sExportFile = argv[++optnum];
			bExport = true;
			break;
		case 'm':
			sExportFile = argv[++optnum];
			bExport = false;
			break;
		case 'h':
			dceConfig.m_sDBHost = argv[++optnum];
			break;
		case 'u':
			dceConfig.m_sDBUser = argv[++optnum];
			break;
		case 'p':
			dceConfig.m_sDBPassword = argv[++optnum];
			break;
		case 'D':
			dceConfig.m_sDBName = argv[++optnum];
			break;
		case 'P':
			dceConfig.m_iDBPort = atoi(argv[++optnum]);
			break;
		case 'c':
			iPK_DHCPDevice = atoi(argv[++optnum]);
			break;
		case 'C':
			iPK_Device_Controlled_Via = atoi(argv[++optnum]);
			break;
		case 'd':
			iPK_DeviceTemplate = atoi(argv[++optnum]);
			break;
		case 'i':
			dceConfig.m_iPK_Installation = atoi(argv[++optnum]);
			break;
		case 'I':
			sIPAddress = argv[++optnum];
			break;
		case 'M':
			sMacAddress = argv[++optnum];
			break;
		case 'n':
			bDontCallConfigureScript=true;
			break;
		case 'A':
			sDeviceData = argv[++optnum];
			break;
		case 'R':
			iPK_Device_RelatedTo = atoi(argv[++optnum]);
			break;
		case 'r':
			bConfirmRelations = true;
			break;
		case 'x':
			bDontInstallPackages = true;
			break;
		case 'b':
			bInstallPackagesInBackground = true;
			break;
		case 'U':
			sUserName = argv[++optnum];
			break;
		default:
			cout << "Unknown: " << argv[optnum] << endl;
			bError=true;
			break;
		};
	}

	if ( bError || (!iPK_DHCPDevice && !iPK_DeviceTemplate && sUserName.size()==0 && sExportFile.size()==0 && bConfirmRelations==false) )
	{
		cout << "CreateDevice, v." << VERSION << endl
			<< "Usage: CreateDevice [-h hostname] [-u username] [-p password] [-D database] [-P mysql port]" << endl
			<< "[-c PK_DHCPDevice] [-d PK_DeviceTemplate] [-i PK_Installation] [-I IPAddress/auto] [-M MacAddress]" << endl
			<< "[-C PK_Device_Controlled_Via] [-n don't call configure script]" << endl
			<< "[-A Default Device data tab delimited as PK_DeviceData\tValue\t...]" << endl
			<< "[-R PK_Device (related to if there are multiple possible controlled Via's)]" << endl
			<< "[-x don't call InstallNewDevice to add packages]" << endl
			<< "[-b Install packages in background]" << endl
			<< "[-U Username] CreateUser--not device" << endl
			<< "[-e Filename] Export all child devices of PK_Device_Controlled_Via to Filename" << endl
			<< "[-i Filename] Import devices in Filename as children of PK_Device_Controlled_Via" << endl
			<< "[-r] Don't create a new device, just confirm relations" << endl
			<< "hostname    -- address or DNS of database host, default is `dce_router`" << endl
			<< "username    -- username for database connection" << endl
			<< "password    -- password for database connection, default is `` (empty)" << endl
			<< "database    -- database name.  default is pluto_main" << endl;

		exit(1);
	}

	string args;
	for(int optnum=1;optnum<argc;++optnum)
		args += string(argv[optnum]) + " ";
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"Called with: %s",args.c_str());

	if( sUserName.size() )
	{
		DBHelper mySqlHelper(dceConfig.m_sDBHost,dceConfig.m_sDBUser,dceConfig.m_sDBPassword,dceConfig.m_sDBName,dceConfig.m_iDBPort);
		if( dceConfig.m_iPK_Installation<1 )
		{
			PlutoSqlResult result;
			DB_ROW row;
			string SQL = "SELECT DISTINCT PK_Installation FROM Installation";
			if( ( result.r=mySqlHelper.db_wrapper_query_result( SQL ) ) && ( row=db_wrapper_fetch_row( result.r ) ) && result.r->row_count==1 )
				dceConfig.m_iPK_Installation = atoi(row[0]);
		}
		UserUtils userUtils(&mySqlHelper,dceConfig.m_iPK_Installation);
		int PK_Users = userUtils.AddUser(sUserName);
		cout << PK_Users << endl;
		return 0;
	}

	CreateDevice createDevice(dceConfig.m_iPK_Installation,dceConfig.m_sDBHost,dceConfig.m_sDBUser,dceConfig.m_sDBPassword,dceConfig.m_sDBName,dceConfig.m_iDBPort);
	createDevice.m_bDontInstallPackages=bDontInstallPackages;
	createDevice.m_bDontCallConfigureScript=bDontCallConfigureScript;
	createDevice.m_bInstallPackagesInBackground=bInstallPackagesInBackground;

	if( sExportFile.size() )
	{
		if( !iPK_Device_Controlled_Via )
		{
			cerr << "Must specify controlled via for import/export" << endl;
			exit(1);
		}
		if( bExport )
			ExportChildDevices(createDevice,iPK_Device_Controlled_Via,sExportFile);
		else
			ImportChildDevices(createDevice,iPK_Device_Controlled_Via,sExportFile);
		return 0;
	}

	if( bConfirmRelations )
	{
		createDevice.ConfirmAllRelations();
		return 0;
	}

	int PK_Device=createDevice.DoIt(iPK_DHCPDevice,iPK_DeviceTemplate,"",sIPAddress,sMacAddress,iPK_Device_Controlled_Via,sDeviceData,iPK_Device_RelatedTo);
	if( PK_Device==0 )
	{
		cerr << "CreateDevice failed" << endl;
		exit(1);
	}
	
	cout << PK_Device << endl;

	return 0;
}

