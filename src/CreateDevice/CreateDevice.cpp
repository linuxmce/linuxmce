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
#include "PlutoUtils/ProcessUtils.h"
#include "PlutoUtils/DatabaseUtils.h"
#include "Logger.h"
#include "CreateDevice.h"

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

#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_Device.h"
#include "pluto_main/Table_Device_DeviceData.h"
#include "pluto_main/Table_DeviceData.h"
#include "pluto_main/Table_DeviceCategory.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Define_Distro.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"
#include "DCEConfig.h"

#define EXTERN_REVISION_DATE
#include "version.h"

// must be extern as the same is defined in Main.cpp 
extern DCEConfig dceConfig;

using namespace std;
using namespace DCE;

class CdPipe
{
public:
	int m_PK_Command_Input,m_PK_Command_Ouput,m_PK_Pipe;
	bool m_bToChild;
	CdPipe(int PK_Command_Input,int PK_Command_Ouput,int PK_Pipe,bool bToChild)
	{
		m_PK_Command_Input=PK_Command_Input;
		m_PK_Command_Ouput=PK_Command_Ouput;
		m_PK_Pipe=PK_Pipe;
		m_bToChild=bToChild;
	}
};

int CreateDevice::DoIt(int iPK_DHCPDevice,int iPK_DeviceTemplate,string sDescription,string sIPAddress,string sMacAddress,int PK_Device_ControlledVia,string sDeviceData,int iPK_Device_RelatedTo,int iPK_Room,bool *bReusedExistingDevice)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"CreateDevice::DoIt called with: iPK_DHCPDevice=%d; iPK_DeviceTemplate=%d"
		"; sIPAddress=%s; sMacAddress=%s; PK_Device_ControlledVia=%d" 
		"; sDeviceData=%s; iPK_Device_RelatedToendl=%d; Description=%s",
		iPK_DHCPDevice,iPK_DeviceTemplate,sIPAddress.c_str(),sMacAddress.c_str(),
		PK_Device_ControlledVia,sDeviceData.c_str(),iPK_Device_RelatedTo,sDescription.c_str());

	if( !m_bConnected )
	{
		cerr << "Cannot connect to database" << endl;
		return 0;
	}

	if( iPK_DHCPDevice )
	{
		string SQL = "SELECT FK_DeviceTemplate FROM DHCPDevice WHERE PK_DHCPDevice=" + StringUtils::itos(iPK_DHCPDevice);

		PlutoSqlResult result;
		DB_ROW row;
		if( ( result.r=db_wrapper_query_result( SQL ) ) && ( row=db_wrapper_fetch_row( result.r ) ) && NULL != row[0])
			iPK_DeviceTemplate = atoi(row[0]);
	}

	if( !iPK_DeviceTemplate )
	{
		cerr << "You did not specify the device.  You must use -c or -d" << endl;
		return 0;
	}


	if( m_iPK_Installation<1 )
	{
		PlutoSqlResult result;
		DB_ROW row;
		string SQL = "SELECT DISTINCT PK_Installation FROM Installation";
		if( ( result.r=db_wrapper_query_result( SQL ) ) && ( row=db_wrapper_fetch_row( result.r ) ) && result.r->row_count==1 )
			m_iPK_Installation = atoi(row[0]);
		else
		{
			cerr << "You did not specify the installation ID.  You must use -i" << endl;
			return 0;
		}
	}

	map<int,string> mapParametersAdded; // Keep track of what DeviceData we have added as we go through
	
	PlutoSqlResult result,result1,result1b,result2,result3,resultPackage,resultAutoAssign;
	DB_ROW row;

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"CreateDevice::DoIt querying packages for %d",iPK_DeviceTemplate);
	int iPK_DeviceCategory;
	string SQL = "SELECT FK_DeviceCategory,Description,ConfigureScript,IsPlugAndPlay,FK_Package,FK_InfraredGroup FROM DeviceTemplate WHERE PK_DeviceTemplate=" + StringUtils::itos(iPK_DeviceTemplate);
	if( ( result.r=db_wrapper_query_result( SQL ) ) && ( row=db_wrapper_fetch_row( result.r ) ) )
		iPK_DeviceCategory = atoi(row[0]);
	else
	{
		cerr << "Cannot find DeviceTemplate: " << iPK_DeviceTemplate << endl;
		return 0;
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"CreateDevice::DoIt getting i/r codes for %d",iPK_DeviceTemplate);
	if( row[5] && atoi(row[5]) )  // Only ask for the i/r codes if there is an infrared group
	{
		string sPK_DeviceTemplate = StringUtils::itos(iPK_DeviceTemplate);
		char * args[] = { (char *)("/usr/pluto/bin/WebDB_GetIR.sh"), (char *)("0"), (char *)(sPK_DeviceTemplate.c_str()), NULL };
		ProcessUtils::SpawnDaemon(args[0], args);
	}
	
	// Check if this device template has 'one per pc' set and ther's already one
	// Use a temporary PK_Device_ControlledVia_temp because in rare circumstances this can't be determined until after the device is created
	// because the controlling device doesn't exist.  In such cases we don't care because there can't be another such device if there's nothing
	// to control it
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"CreateDevice::DoIt Create device -- finding controlled via for %d",iPK_DeviceTemplate);
	int PK_Device_Existing,PK_Device_ControlledVia_temp = PK_Device_ControlledVia;
	if( !PK_Device_ControlledVia_temp )
		PK_Device_ControlledVia_temp=DatabaseUtils::FindControlledViaCandidate(this,0,iPK_DeviceTemplate,iPK_Device_RelatedTo,m_iPK_Installation);
	if( PK_Device_ControlledVia_temp && (PK_Device_Existing=DatabaseUtils::ViolatesDuplicateRules(this,PK_Device_ControlledVia_temp,iPK_DeviceTemplate))!=0 )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"CreateDevice::DoIt Not creating duplicate device template %d on %d, using %d",iPK_DeviceTemplate,PK_Device_ControlledVia_temp,PK_Device_Existing);
		if( bReusedExistingDevice )
			*bReusedExistingDevice = true;
		return PK_Device_Existing;
	}
		
	int iPK_Package = row[4] ? atoi(row[4]) : 0;

	if( PK_Device_ControlledVia )
		FixControlledViaIfEmbeddedIsMoreValid(iPK_DeviceTemplate,PK_Device_ControlledVia);

	SQL = "INSERT INTO Device(Description,FK_DeviceTemplate,FK_Installation,FK_Room,IPAddress,MACaddress,Status";
	if( PK_Device_ControlledVia )
		SQL += ",FK_Device_ControlledVia";
	SQL+=") VALUES('" + StringUtils::SQLEscape(sDescription.empty()==false ? sDescription : row[1]) + "'," + StringUtils::itos(iPK_DeviceTemplate) + "," + StringUtils::itos(m_iPK_Installation) + 
		"," + (iPK_Room ? StringUtils::itos(iPK_Room) : "NULL") + ",'" + sIPAddress + "','" + sMacAddress + "','" + (m_bDontCallConfigureScript ? "" : "**RUN_CONFIG**") + "'";
	if( PK_Device_ControlledVia )
		SQL += "," + StringUtils::itos(PK_Device_ControlledVia);
		
	SQL += ");";

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"CreateDevice::DoIt Ready to insert %s", SQL.c_str());

	int PK_Device = threaded_db_wrapper_query_withID(SQL);
	if( !PK_Device )
	{
		cerr << "Unable to add device" << endl;
		return 0;
	}

	if (sIPAddress == "auto")
	{
		char cmd[1024];
		snprintf(cmd, 1024, "/usr/pluto/bin/PlutoDHCP.sh -d %d -a", PK_Device);
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"CreateDevice::DoIt %s",cmd);
		system(cmd);
	}
	else if( sIPAddress.empty()==false )
	{
		char * args[] = { (char * )("/usr/pluto/bin/PlutoDHCP.sh"), NULL };
		ProcessUtils::SpawnDaemon(args[0], args);
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"CreateDevice::DoIt Inserted device: %d Package: %d installation: %d configure: %d",
		PK_Device,iPK_Package,m_iPK_Installation,(int) m_bDontCallConfigureScript);

	bool bIsOrbiter=false;
	// Loop through all the categories
	int iPK_DeviceCategory_Loop = iPK_DeviceCategory;
	while( iPK_DeviceCategory_Loop )
	{
		if( iPK_DeviceCategory_Loop==DEVICECATEGORY_Orbiter_CONST )
			bIsOrbiter=true;
		SQL = "SELECT FK_DeviceData,IK_DeviceData FROM DeviceCategory_DeviceData WHERE FK_DeviceCategory=" + StringUtils::itos(iPK_DeviceCategory_Loop);
		if( ( result1.r=db_wrapper_query_result( SQL ) ) )
		{
LoggerWrapper::GetInstance()->Write(LV_STATUS,"CreateDevice::DoIt Found %d rows with %s",(int) result1.r->row_count,SQL.c_str());
			while( (row=db_wrapper_fetch_row( result1.r )) )
			{
				mapParametersAdded[ atoi(row[0]) ] = row[1] ? row[1] : "";
LoggerWrapper::GetInstance()->Write(LV_STATUS,"CreateDevice::DoIt Added parameter from category %d %s",atoi(row[0]),mapParametersAdded[ atoi(row[0]) ].c_str());

				SQL = "INSERT INTO Device_DeviceData(FK_Device,FK_DeviceData,IK_DeviceData) VALUES(" + StringUtils::itos(PK_Device) + 
					"," + row[0] + ",'" + (row[1] ? StringUtils::SQLEscape( row[1] ) : string("")) + "');";
				threaded_db_wrapper_query(SQL,true);
			}
		}
		SQL = "SELECT FK_DeviceCategory_Parent FROM DeviceCategory WHERE PK_DeviceCategory=" + StringUtils::itos(iPK_DeviceCategory_Loop);
		if( ( result1b.r=db_wrapper_query_result( SQL ) ) && ( row=db_wrapper_fetch_row( result1b.r ) ) && row[0] )
			iPK_DeviceCategory_Loop = atoi(row[0]);
		else
			iPK_DeviceCategory_Loop = 0;
	}

	SQL = "SELECT FK_DeviceData,IK_DeviceData FROM DeviceTemplate_DeviceData JOIN DeviceData ON PK_DeviceData=FK_DeviceData WHERE FK_DeviceTemplate=" + StringUtils::itos(iPK_DeviceTemplate);
	if( ( result2.r=db_wrapper_query_result( SQL ) ) )
	{
LoggerWrapper::GetInstance()->Write(LV_STATUS,"CreateDevice::DoIt Found %d rows with %s",(int) result2.r->row_count,SQL.c_str());
		while( (row=db_wrapper_fetch_row( result2.r )) )
		{
			if( mapParametersAdded.find(atoi(row[0]))!=mapParametersAdded.end() )
			{
				SQL = "UPDATE Device_DeviceData SET IK_DeviceData='" + (row[1] ? StringUtils::SQLEscape( row[1] ) : string("")) + "' " +
					"WHERE FK_Device=" + StringUtils::itos(PK_Device) + " AND FK_DeviceData=" + row[0];
			}
			else
			{
				mapParametersAdded[ atoi(row[0]) ] = row[1] ? row[1] : "";
				SQL = "INSERT INTO Device_DeviceData(FK_Device,FK_DeviceData,IK_DeviceData) VALUES(" + StringUtils::itos(PK_Device) + 
					"," + row[0] + ",'" + (row[1] ? StringUtils::SQLEscape( row[1] ) : string("")) + "');";
			}
LoggerWrapper::GetInstance()->Write(LV_STATUS,"CreateDevice::DoIt Executing %s",SQL.c_str());
			threaded_db_wrapper_query(SQL,true);
		}
	}

	if( iPK_DHCPDevice )
	{
		SQL = "SELECT FK_DeviceData,IK_DeviceData FROM DHCPDevice_DeviceData WHERE FK_DHCPDevice=" + StringUtils::itos(iPK_DHCPDevice);
		if( ( result3.r=db_wrapper_query_result( SQL ) ) )
		{
LoggerWrapper::GetInstance()->Write(LV_STATUS,"CreateDevice::DoIt Found %d rows with %s",(int) result3.r->row_count,SQL.c_str());
			while( (row=db_wrapper_fetch_row( result3.r )) )
			{
				if( mapParametersAdded.find(atoi(row[0]))!=mapParametersAdded.end() )
				{
						SQL = "UPDATE Device_DeviceData SET IK_DeviceData='" + (row[1] ? StringUtils::SQLEscape( row[1] ) : string("")) + "' " +
						"WHERE FK_Device=" + StringUtils::itos(PK_Device) + " AND FK_DeviceData=" + row[0];
				}
				else
				{
					mapParametersAdded[ atoi(row[0]) ] = row[1] ? row[1] : "";
					SQL = "INSERT INTO Device_DeviceData(FK_Device,FK_DeviceData,IK_DeviceData) VALUES(" + StringUtils::itos(PK_Device) + 
						"," + row[0] + ",'" + (row[1] ? StringUtils::SQLEscape( row[1] ) : string("")) + "');";
				}
				threaded_db_wrapper_query(SQL,true);
			}
		}
	}

	// Within the device relation are information about embedded devices and pipes.  Store them in variables, and add them after we've got the controlled via
	bool bEmbedded=false;
	list<CdPipe *> listCdPipe;
	if( sDeviceData.size() )
	{
		string::size_type pos=0;
		while(pos<sDeviceData.size())
		{
			string sPK_DeviceData = StringUtils::Tokenize(sDeviceData,"|",pos);
			if( sPK_DeviceData=="E" )
				bEmbedded=true;
			else if( sPK_DeviceData=="P" )
			{
				int PK_Command_Input = atoi(StringUtils::Tokenize(sDeviceData,"|",pos).c_str());
				int PK_Command_Ouput = atoi(StringUtils::Tokenize(sDeviceData,"|",pos).c_str());
				int PK_Pipe = atoi(StringUtils::Tokenize(sDeviceData,"|",pos).c_str());
				bool bToChild = atoi(StringUtils::Tokenize(sDeviceData,"|",pos).c_str())==1;
				listCdPipe.push_back(new CdPipe(PK_Command_Input,PK_Command_Ouput,PK_Pipe,bToChild));
			}
			else
			{
				int PK_DeviceData = atoi(sPK_DeviceData.c_str());
				string sValue = StringUtils::Tokenize(sDeviceData,"|",pos);
				AssignDeviceData(PK_Device,PK_DeviceData,sValue);

				if( PK_DeviceData==DEVICEDATA_sPK_Device_Relations_For_Creat_CONST )
				{
					vector<string> vectDevices;
					StringUtils::Tokenize(sValue,"\t",vectDevices);
					for(vector<string>::iterator it=vectDevices.begin();it!=vectDevices.end();++it)
						CreateRelation(PK_Device,atoi(it->c_str()));
				}
				else if( PK_DeviceData==DEVICEDATA_Description_CONST && sValue.empty()==false )
				{
					string sSQL = "UPDATE Device SET Description='" + StringUtils::SQLEscape(sValue) + "' WHERE PK_Device=" + StringUtils::itos(PK_Device);
					threaded_db_wrapper_query(sSQL);
				}
			}
		}
	}

    // Now we've got to see if we have any children we need to create automatically, first by the category
	CreateChildrenByCategory(PK_Device,iPK_DeviceCategory);
	CreateChildrenByTemplate(PK_Device,iPK_DeviceTemplate);
	// Create the related devices now before finding the controlled via candidate
	// Since we may be controlled Via a device we create as a relation
	list<int> listSisterDevices;
	ConfirmRelations(PK_Device,iPK_Device_RelatedTo,&listSisterDevices);

	int PK_Distro=-1;
	if( iPK_DeviceCategory==DEVICECATEGORY_Core_CONST )
	{
		// Get Distro and OS from Device
		string SQL = "SELECT IK_DeviceData as FK_Distro, FK_OperatingSystem FROM Device LEFT JOIN Device_DeviceData ON FK_Device=PK_Device LEFT JOIN Distro ON IK_DeviceData=PK_Distro WHERE PK_Device=" + StringUtils::itos(PK_Device) + " AND FK_DeviceData=7";

		PlutoSqlResult osdistro;
		if( ( osdistro.r=db_wrapper_query_result ( SQL ) ) && ( row=db_wrapper_fetch_row( osdistro.r ) ) )
		{
			PK_Distro=atoi( row[0] );
			string SQL = string("SELECT FK_DeviceTemplate, FK_OperatingSystem, FK_Distro FROM InstallWizard RIGHT JOIN InstallWizard_Distro ON FK_InstallWizard=PK_InstallWizard WHERE InstallWizard.Default=1 AND InstallWizard.Step=5 AND ((FK_Distro=") + row[0] + ") OR (FK_OperatingSystem=" + row[1] + " AND FK_Distro IS NULL) OR (FK_OperatingSystem IS NULL AND FK_Distro IS NULL))";
			PlutoSqlResult result_child_dev;
			if( ( result_child_dev.r=db_wrapper_query_result( SQL ) ) )
			{
				while( (row=db_wrapper_fetch_row( result_child_dev.r )) )
				{
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"CreateDevice::DoIt Found install wizard %d",atoi(row[0]));
					DoIt(0,atoi(row[0]),"","","",0,"",PK_Device);
				}
			}
		}
	}
	else if( iPK_DeviceCategory==DEVICECATEGORY_Media_Director_CONST )
	{
		// Get Distro and OS from Device
                string SQL = "SELECT IK_DeviceData as FK_Distro, FK_OperatingSystem, FK_DeviceTemplate FROM Device LEFT JOIN Device_DeviceData ON FK_Device=PK_Device LEFT JOIN Distro ON IK_DeviceData=PK_Distro WHERE FK_DeviceData=" TOSTRING(DEVICEDATA_PK_Distro_CONST) " AND PK_Device=" + StringUtils::itos(PK_Device);

		LoggerWrapper::GetInstance()->Write(LV_STATUS,"CreateDevice::DoIt -- %s",SQL.c_str());

                PlutoSqlResult osdistro;
		if( ( osdistro.r=db_wrapper_query_result ( SQL ) ) && ( row=db_wrapper_fetch_row( osdistro.r ) ) )
		{
			PK_Distro=atoi( row[0] );
			string SQL = string("SELECT FK_DeviceTemplate, FK_OperatingSystem, FK_Distro FROM InstallWizard RIGHT JOIN InstallWizard_Distro ON FK_InstallWizard=PK_InstallWizard WHERE InstallWizard.Default=1 AND InstallWizard.Step=6 AND ((FK_Distro=") + row[0] + ") OR (FK_OperatingSystem=" + row[1] + " AND FK_Distro IS NULL) OR (FK_OperatingSystem IS NULL AND FK_Distro IS NULL)) AND (FK_DeviceTemplate_Required IN (" + row[2] + ") OR FK_DeviceTemplate_Required IS NULL)";
			PlutoSqlResult result_child_dev;
			if( ( result_child_dev.r=db_wrapper_query_result( SQL ) ) )
			{
				while( (row=db_wrapper_fetch_row( result_child_dev.r )) )
				{
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"CreateDevice::DoIt Found install wizard %d",atoi(row[0]));
					DoIt(0,atoi(row[0]),"","","",0,"",PK_Device);
				}
			}
		}
		if ( ( !PK_Device_ControlledVia ) && ( PK_Distro != -1 ) && ( PK_Distro != DISTRO_Raspian_Wheezy_CONST ) )
		{
			// See what PVR software this uses
			string SQL = "SELECT IK_DeviceData FROM Device_DeviceData JOIN Device ON FK_Device=PK_Device WHERE FK_DeviceData=" TOSTRING(DEVICEDATA_TV_Standard_CONST) 
				" AND FK_DeviceTemplate=" TOSTRING(DEVICETEMPLATE_Generic_PC_as_Core_CONST) " AND FK_Installation=" + StringUtils::itos(m_iPK_Installation);

			char cPVR; //='M'; // Default to MythTV
			PlutoSqlResult result_pvr;
			if( ( result_pvr.r=db_wrapper_query_result( SQL ) ) && (row=db_wrapper_fetch_row( result_pvr.r )) && row[0] )
			{
				cPVR = row[0][0];
				if( cPVR=='V' )
					DoIt(0,DEVICETEMPLATE_VDR_CONST,"","","",0,"",PK_Device);
				else if ( cPVR=='M' )
					DoIt(0,DEVICETEMPLATE_MythTV_Player_CONST,"","","",0,"",PK_Device);
			}
		}
	}

	// If we weren't given a controlled via, try to find an appropriate one
	// Do this last since the controlled via may be something we added above
	if( !PK_Device_ControlledVia )
	{
		PK_Device_ControlledVia=DatabaseUtils::FindControlledViaCandidate(this,PK_Device,iPK_DeviceTemplate,iPK_Device_RelatedTo,m_iPK_Installation);
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"CreateDevice::DoIt Searching for controlled via returned %d",PK_Device_ControlledVia);
		if( PK_Device_ControlledVia)
		{
			SQL = "UPDATE Device SET FK_Device_ControlledVia=" + StringUtils::itos(PK_Device_ControlledVia) +
				" WHERE PK_Device=" + StringUtils::itos(PK_Device);
			if( threaded_db_wrapper_query(SQL)<0 )
			{
				cout << "Error updating device controlled via" << endl;
				return 0;
			}
		}
		else if( iPK_Device_RelatedTo )
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"CreateDevice::DoIt was given a iPK_Device_RelatedTo %d but can't find a candidate",iPK_Device_RelatedTo);
	}

	// Fix any sister devices that didn't already have the same controlled via
	if( PK_Device_ControlledVia )
	{
		for(list<int>::iterator it=listSisterDevices.begin();it!=listSisterDevices.end();++it)
		{
			int PK_Device = *it;
			SQL = "UPDATE Device SET FK_Device_ControlledVia=" + StringUtils::itos(PK_Device_ControlledVia) +
				" WHERE PK_Device=" + StringUtils::itos(PK_Device);
			if( threaded_db_wrapper_query(SQL)<0 )
			{
				cout << "Error updating device controlled via" << endl;
				return 0;
			}
		}
	}

	// See if auto assign to parent's room is specified
	SQL = "SELECT IK_DeviceData FROM DeviceTemplate_DeviceData WHERE FK_DeviceTemplate=" + StringUtils::itos(iPK_DeviceTemplate) + " AND FK_DeviceData=" TOSTRING(DEVICEDATA_Autoassign_to_parents_room_CONST) " AND IK_DeviceData=1";
	if( ( resultAutoAssign.r=db_wrapper_query_result( SQL ) ) && ( row=db_wrapper_fetch_row( resultAutoAssign.r ) ) )
	{
		SQL = "UPDATE Device JOIN Device AS Parent ON Device.FK_Device_ControlledVia=Parent.PK_Device SET Device.FK_Room=Parent.FK_Room WHERE Device.PK_Device=" + StringUtils::itos(PK_Device);
		threaded_db_wrapper_query(SQL);
	}

	// If this is an orbiter, and it's not an on-screen orbiter, which will get orbitergen'd after a/v wizard, then regen it now
	if( bIsOrbiter && PK_Device_ControlledVia==0 )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"CreateDevice::DoIt it's an orbiter, time to generate");
		string SQL = "SELECT PK_Device FROM Device WHERE FK_Installation=" + StringUtils::itos(m_iPK_Installation) + " AND FK_DeviceTemplate=" + StringUtils::itos(DEVICETEMPLATE_Orbiter_Plugin_CONST);
		PlutoSqlResult result;
		DB_ROW row;
		if( ( result.r=db_wrapper_query_result( SQL ) ) && ( row=db_wrapper_fetch_row( result.r ) ) )
		{
			int iPK_Device_OP = atoi(row[0]);
			string sCmd = "/usr/pluto/bin/MessageSend localhost 0 " + StringUtils::itos(iPK_Device_OP) + " 1 " +
				StringUtils::itos(COMMAND_Regen_Orbiter_CONST) + " " + StringUtils::itos(COMMANDPARAMETER_PK_Device_CONST) + " " + StringUtils::itos(PK_Device);
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"CreateDevice::DoIt Calling %s",sCmd.c_str());
			system(sCmd.c_str());
		}
		else
			cerr << "No Orbiter plugin" << endl;
	}

	if( bEmbedded && PK_Device_ControlledVia )
	{
		string sSQL = "UPDATE Device SET FK_Device_RouteTo=" + StringUtils::itos(PK_Device_ControlledVia) + " WHERE PK_Device=" + StringUtils::itos(PK_Device);
		threaded_db_wrapper_query(sSQL);
	}

	for(list<CdPipe *>::iterator it=listCdPipe.begin();it!=listCdPipe.end();++it)
	{
		CdPipe *pCdPipe = *it;
		string SQL = "INSERT INTO Device_Device_Pipe(FK_Device_To,FK_Device_From,FK_Pipe,FK_Command_Input,FK_Command_Output) VALUES(";
		if( pCdPipe->m_bToChild )
			SQL += StringUtils::itos(PK_Device) + "," + StringUtils::itos(PK_Device_ControlledVia);
		else
			SQL += StringUtils::itos(PK_Device_ControlledVia) + "," + StringUtils::itos(PK_Device);
		SQL += string(",") + 
			(pCdPipe->m_PK_Pipe ? StringUtils::itos(pCdPipe->m_PK_Pipe) : "NULL") + "," + 
			(pCdPipe->m_PK_Command_Input ? StringUtils::itos(pCdPipe->m_PK_Command_Input) : "NULL") + "," + 
			(pCdPipe->m_PK_Command_Ouput ? StringUtils::itos(pCdPipe->m_PK_Command_Ouput) : "NULL") + ")";
		threaded_db_wrapper_query(SQL.c_str(),true); // If it's a device previously created the record may already exist

		if( pCdPipe->m_PK_Command_Input )
		{
			SQL = "SELECT Description FROM Command WHERE PK_Command=" + StringUtils::itos(pCdPipe->m_PK_Command_Input);
			PlutoSqlResult result;
			DB_ROW row;
			if( ( result.r=db_wrapper_query_result( SQL ) ) && ( row=db_wrapper_fetch_row( result.r ) ) && NULL != row[0])
			{
				string sLastDescription=row[0];
				SQL = "UPDATE Device SET Description = concat(Description,'/" + StringUtils::SQLEscape(sLastDescription) + "') WHERE PK_Device=" + StringUtils::itos(PK_Device);
				threaded_db_wrapper_query(SQL.c_str());
			}
		}
	}

	if( iPK_Package && !m_bDontInstallPackages )
	{
		int PK_Device_TopMost=DatabaseUtils::GetTopMostDevice(this,PK_Device);
		if( PK_Device_TopMost==dceConfig.m_iPK_Device_Computer )
		{
			SQL = "SELECT Name FROM Package_Source RIGHT JOIN Package_Source_Compat AS PSC ON FK_Package_Source=PK_Package_Source LEFT JOIN RepositorySource ON FK_RepositorySource=PK_RepositorySource WHERE FK_RepositoryType=1 and FK_Package=" + StringUtils::itos(iPK_Package) + " AND PSC.FK_Distro=" + StringUtils::itos(PK_Distro);
			PlutoSqlResult result;
			if( ( result.r=db_wrapper_query_result( SQL ) ) && ( row=db_wrapper_fetch_row( result.r ) ) )
			{
				if( m_bInstallPackagesInBackground )
				{
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"CreateDevice::DoIt Executing /usr/pluto/bin/InstallNewDevice.sh %d %s",
						PK_Device,(row[0] ? row[0] : ""));
					string sPK_Device = StringUtils::itos(PK_Device);
					char * args[] = { (char *)("/usr/pluto/bin/InstallNewDevice.sh"), (char *)(sPK_Device.c_str()), (char *)(row[0] ? row[0] : ""), NULL };
					ProcessUtils::SpawnDaemon(args[0], args);
				}
				else
				{
					string sCmd = "/usr/pluto/bin/InstallNewDevice.sh " + StringUtils::itos(PK_Device) + " \"" + row[0] + "\"";
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"CreateDevice::DoIt Executing %s",sCmd.c_str());
					system(sCmd.c_str());
				}
			}
			else
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"CreateDevice::DoIt No installation info for package %d",iPK_Package);
		}
		else
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"CreateDevice::DoIt Not installing package %d because %d top most is not %d",iPK_Package,PK_Device_TopMost,dceConfig.m_iPK_Device_Computer);
	}
	else
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"CreateDevice::DoIt Not installing package %d",iPK_Package);


	return PK_Device;
}

void CreateDevice::CreateChildrenByCategory(int iPK_Device,int iPK_DeviceCategory)
{
	PlutoSqlResult result,result2;
	DB_ROW row;

	string SQL = "SELECT FK_DeviceTemplate,RerouteMessagesToParent,PK_DeviceTemplate_DeviceCategory_ControlledVia FROM DeviceTemplate_DeviceCategory_ControlledVia WHERE AutoCreateChildren=1 AND FK_DeviceCategory=" + StringUtils::itos(iPK_DeviceCategory);
	if( ( result.r=db_wrapper_query_result( SQL ) ) )
	{
		while( (row=db_wrapper_fetch_row( result.r )) )
		{
			CreateAutoCreateChildDevice(iPK_Device,atoi(row[0]),row[1] && atoi(row[1]),0,atoi(row[2]));
		}
	}

	SQL = "SELECT FK_DeviceCategory_Parent FROM DeviceCategory WHERE PK_DeviceCategory=" + StringUtils::itos(iPK_DeviceCategory);
	if( ( result2.r=db_wrapper_query_result( SQL ) ) && ( row=db_wrapper_fetch_row( result2.r ))!=NULL && row[0] )
		CreateChildrenByCategory( iPK_Device, atoi(row[0]) );
}

void CreateDevice::CreateChildrenByTemplate(int iPK_Device,int iPK_DeviceTemplate)
{
	PlutoSqlResult result;
	DB_ROW row;

	string SQL = "SELECT FK_DeviceTemplate,RerouteMessagesToParent,PK_DeviceTemplate_DeviceTemplate_ControlledVia FROM DeviceTemplate_DeviceTemplate_ControlledVia WHERE AutoCreateChildren=1 AND FK_DeviceTemplate_ControlledVia=" + StringUtils::itos(iPK_DeviceTemplate);
	if( ( result.r=db_wrapper_query_result( SQL ) ) )
	{
		while( (row=db_wrapper_fetch_row( result.r )) )
		{
			CreateAutoCreateChildDevice(iPK_Device,atoi(row[0]),row[1] && atoi(row[1]),atoi(row[2]),0);
		}
	}
}

void CreateDevice::CreateAutoCreateChildDevice(int iPK_Device_Parent,int PK_DeviceTemplate,bool bRerouteMessagesToParent,int PK_DeviceTemplate_DeviceTemplate_ControlledVia,int PK_DeviceTemplate_DeviceCategory_ControlledVia)
{
	DB_ROW row;
	string SQL;
	int PK_Device = DoIt(0,PK_DeviceTemplate,"","","",iPK_Device_Parent);
	if( bRerouteMessagesToParent )
	{
		// Need to reroute messages to parent
		SQL = "UPDATE Device SET FK_Device_RouteTo=" + StringUtils::itos(iPK_Device_Parent) + " WHERE PK_Device=" + StringUtils::itos(PK_Device);
		threaded_db_wrapper_query(SQL.c_str());
	}

	PlutoSqlResult result;
	if( PK_DeviceTemplate_DeviceTemplate_ControlledVia )
		SQL = "SELECT FK_Pipe,FK_Command_Input,FK_Command_Output,ToChild,Command.Description FROM DeviceTemplate_DeviceTemplate_ControlledVia_Pipe LEFT JOIN Command ON FK_Command_Input=PK_Command WHERE FK_DeviceTemplate_DeviceTemplate_ControlledVia=" + StringUtils::itos(PK_DeviceTemplate_DeviceTemplate_ControlledVia);
	else
		SQL = "SELECT FK_Pipe,FK_Command_Input,FK_Command_Output,ToChild,Command.Description FROM DeviceTemplate_DeviceCategory_ControlledVia_Pipe LEFT JOIN Command ON FK_Command_Input=PK_Command WHERE FK_DeviceTemplate_DeviceCategory_ControlledVia=" + StringUtils::itos(PK_DeviceTemplate_DeviceCategory_ControlledVia);

	if( ( result.r=db_wrapper_query_result( SQL ) ) )
	{
		string sLastDescription;
		while( (row=db_wrapper_fetch_row( result.r )) )
		{
			SQL = "INSERT INTO Device_Device_Pipe(FK_Device_To,FK_Device_From,FK_Pipe,FK_Command_Input,FK_Command_Output) VALUES(";
			if( row[3] && atoi(row[3]) )
				SQL += StringUtils::itos(PK_Device) + "," + StringUtils::itos(iPK_Device_Parent);
			else
				SQL += StringUtils::itos(iPK_Device_Parent) + "," + StringUtils::itos(PK_Device);
			SQL += string(",") + (row[0] ? row[0] : "NULL") + "," + (row[1] ? row[1] : "NULL")  + "," + (row[2] ? row[2] : "NULL") + ")";
			threaded_db_wrapper_query(SQL.c_str(),true); // If it's a device previously created the record may already exist

			if( row[4] && sLastDescription!=row[4] ) // We've got a description for the input
			{
				sLastDescription=row[4];
				SQL = "UPDATE Device SET Description = concat(Description,'/" + StringUtils::SQLEscape(sLastDescription) + "') WHERE PK_Device=" + StringUtils::itos(PK_Device);
				threaded_db_wrapper_query(SQL.c_str());
			}
		}
	}
}

void CreateDevice::ConfirmAllRelations()
{
	PlutoSqlResult result;
	DB_ROW row;

	string SQL = "SELECT PK_Device FROM Device WHERE FK_Installation=" + StringUtils::itos(m_iPK_Installation);
	if( ( result.r=db_wrapper_query_result( SQL ) ) )
	{
		while( (row=db_wrapper_fetch_row( result.r )) )
		{
			int PK_Device = atoi(row[0]);
			ConfirmRelations(PK_Device);
		}
	}
}

void CreateDevice::ConfirmRelations(int PK_Device,int iPK_Device_RelatedTo,list<int> *plistSisterDevices,bool bRecurseChildren,bool bOnlyAddDevicesOnCore)
{
	PlutoSqlResult result_dt,result_related,result_related2,result_related3,result_related4,result_related5;
	DB_ROW row,row3;

	int PK_Device_Core=0,PK_Device_DCERouter=0,PK_Device_Parent=0;
	string SQL = "SELECT PK_Device,FK_Device_ControlledVia FROM Device WHERE FK_DeviceTemplate=" + StringUtils::itos(DEVICETEMPLATE_DCERouter_CONST);
	if( (result_related4.r=db_wrapper_query_result(SQL)) && (row3=db_wrapper_fetch_row(result_related4.r)) )
	{
		PK_Device_DCERouter = row3[0] ? atoi(row3[0]) : 0;
		PK_Device_Core = row3[1] ? atoi(row3[1]) : 0;
	}

	SQL = "SELECT FK_Device_ControlledVia FROM Device WHERE PK_Device=" + StringUtils::itos(PK_Device);
	if( (result_related5.r=db_wrapper_query_result(SQL)) && (row3=db_wrapper_fetch_row(result_related5.r)) )
		PK_Device_Parent = row3[0] ? atoi(row3[0]) : 0;

	int PK_DeviceTemplate=0;
	SQL = "SELECT FK_DeviceTemplate FROM Device WHERE PK_Device=" + StringUtils::itos(PK_Device);
	if( (result_dt.r=db_wrapper_query_result(SQL)) && (row=db_wrapper_fetch_row(result_dt.r)) )
		PK_DeviceTemplate=atoi(row[0]);

	if( !PK_DeviceTemplate )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Invalid device template for device %d",PK_Device);
		return;
	}

	// See if this requires dragging any other types of devices
	SQL = "SELECT FK_DeviceTemplate_Related,Value FROM DeviceTemplate_DeviceTemplate_Related WHERE FK_DeviceTemplate=" + StringUtils::itos(PK_DeviceTemplate) + " ORDER BY FK_DeviceTemplate_Related";  // Use an order by so we can predict exactly in what order this will be done
	if( ( result_related.r=db_wrapper_query_result( SQL ) ) )
	{
LoggerWrapper::GetInstance()->Write(LV_STATUS,"Found result_related %d rows with %s",(int) result_related.r->row_count,SQL.c_str());
		while( (row=db_wrapper_fetch_row( result_related.r )) )
		{
			int iPK_DeviceTemplate_Related = atoi(row[0]);
			int iRelation= (row[1] ? atoi(row[1]) : 0);

			string sDeviceData;
			if( row[1] )
			{
				char *pDeviceData = strchr(row[1],'|');
				if( pDeviceData )
					sDeviceData = pDeviceData + 1;
			}

			string sPort;  // We can have multiple children of the same type, but with different ports
			string::size_type pos=0;
			while(pos<sDeviceData.size())
			{
				int PK_DeviceData = atoi( StringUtils::Tokenize( sDeviceData, "|", pos ).c_str() );
				string sValue = StringUtils::Tokenize( sDeviceData, "|", pos );
				if( PK_DeviceData==DEVICEDATA_PortChannel_Number_CONST )
				{
					sPort = sValue;
					break;
				}
			}

			// 1 = Sister device same controlled via
			// 2 = child of the core (dce router's parent)
			// 3 = plugin
			// 4 = anywhere on the same pc
			// 5 = my child
			if( (iRelation>=1 && iRelation<=3) || iRelation==5 )
			{
				// We need this device too.  See if we already have it
				if( iRelation==1 )
					SQL = "SELECT FK_DeviceTemplate FROM Device LEFT JOIN Device_DeviceData ON FK_Device=PK_Device AND FK_DeviceData=" TOSTRING(DEVICEDATA_PortChannel_Number_CONST) " WHERE FK_DeviceTemplate=" + StringUtils::itos(iPK_DeviceTemplate_Related) + " AND FK_Device_ControlledVia=" + StringUtils::itos(PK_Device_Parent);
				else if( iRelation==2 )
					SQL = "SELECT FK_DeviceTemplate FROM Device LEFT JOIN Device_DeviceData ON FK_Device=PK_Device AND FK_DeviceData=" TOSTRING(DEVICEDATA_PortChannel_Number_CONST) " WHERE FK_DeviceTemplate=" + StringUtils::itos(iPK_DeviceTemplate_Related) + " AND FK_Device_ControlledVia=" + StringUtils::itos(PK_Device_Core);
				else if( iRelation==3 )
					SQL = "SELECT FK_DeviceTemplate FROM Device LEFT JOIN Device_DeviceData ON FK_Device=PK_Device AND FK_DeviceData=" TOSTRING(DEVICEDATA_PortChannel_Number_CONST) " WHERE FK_DeviceTemplate=" + StringUtils::itos(iPK_DeviceTemplate_Related) + " AND FK_Device_ControlledVia=" + StringUtils::itos(PK_Device_DCERouter);
				else if( iRelation==5 )
					SQL = "SELECT FK_DeviceTemplate FROM Device LEFT JOIN Device_DeviceData ON FK_Device=PK_Device AND FK_DeviceData=" TOSTRING(DEVICEDATA_PortChannel_Number_CONST) " WHERE FK_DeviceTemplate=" + StringUtils::itos(iPK_DeviceTemplate_Related) + " AND FK_Device_ControlledVia=" + StringUtils::itos(PK_Device);

				if( sPort.empty()==false )
					SQL += " AND IK_DeviceData='" + sPort + "'";

				if( (result_related2.r=db_wrapper_query_result(SQL)) && result_related2.r->row_count>0 )
					continue;  // It's ok.  We got it

				// That device doesn't exist.  We need to create it
				if( iRelation==1 && !bOnlyAddDevicesOnCore )  // It's a sister device
				{
					bool bReusedExisting=false;
					int PK_Device = DoIt(0,iPK_DeviceTemplate_Related,"","","",PK_Device_Parent,sDeviceData,iPK_Device_RelatedTo,0,&bReusedExisting);
					if( bReusedExisting==false && PK_Device_Parent==0 && plistSisterDevices ) // The parent device isn't known yet because ConfirmRelations is called before FindControlledVia.
						plistSisterDevices->push_back(PK_Device);  // So add it to a map to process later
					/* The above is still susceptible to problems.  We call ConfirmRelations before finalizing the controlled via, since the controlled via may be
					created in ConfirmRelations.  So the controlled via may be 0.  Therefore the check above to see if the device already exists may report that 
					it doesn't, so we may create a second copy.  The best solution is to add the #162 Only One Per PC(bool) device data so it's only 
					created once, such as with the Windows XP MCE remote */
				}
				else if( iRelation==2 )  // It's a child of the core (ie DCERouter's parent)
					DoIt(0,iPK_DeviceTemplate_Related,"","","",PK_Device_Core,sDeviceData);
				else if( iRelation==3 )  // It's a plugin (ie child of DCERouter)
					DoIt(0,iPK_DeviceTemplate_Related,"","","",PK_Device_DCERouter,sDeviceData);
				else if( iRelation==5 )  // It's my child
					DoIt(0,iPK_DeviceTemplate_Related,"","","",PK_Device,sDeviceData);
			}
			else if( iRelation==4 )  // Put it anywhere on this PC
			{
				map<int,pair<int,int> > mapDeviceTree;
				DatabaseUtils::GetAllDevicesInTree(this,PK_Device,mapDeviceTree);
				if( iPK_Device_RelatedTo )
					DatabaseUtils::GetAllDevicesInTree(this,PK_Device,mapDeviceTree);  // Add the related devices as well

				bool bFound=false;
				for(map<int,pair<int,int> >::iterator it=mapDeviceTree.begin();it!=mapDeviceTree.end();++it)
				{
					if( it->second.first==iPK_DeviceTemplate_Related )
					{
						bFound=true;
						break;
					}
				}
				if( bFound )
					continue;

				DoIt(0,iPK_DeviceTemplate_Related,"","","",0,sDeviceData,PK_Device);
			}
		}
	}

	if( bRecurseChildren )
	{
		PlutoSqlResult result_child_dev;
		SQL = "SELECT PK_Device FROM Device WHERE FK_Device_ControlledVia=" + StringUtils::itos(PK_Device);
		if( ( result_child_dev.r=db_wrapper_query_result( SQL ) ) )
		{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Found result_child_dev %d rows with %s",(int) result_child_dev.r->row_count,SQL.c_str());
			while( (row=db_wrapper_fetch_row( result_child_dev.r )) )
				ConfirmRelations(atoi(row[0]),iPK_Device_RelatedTo,NULL,true);
		}
	}
}

void CreateDevice::AssignDeviceData(int PK_Device,int PK_DeviceData,string sValue)
{
	string SQL = "SELECT FK_Device FROM Device_DeviceData WHERE FK_Device="
		+ StringUtils::itos(PK_Device) + " AND FK_DeviceData=" + StringUtils::itos(PK_DeviceData);

	PlutoSqlResult result1;
	result1.r=db_wrapper_query_result( SQL );
	if( !result1.r || result1.r->row_count==0 )
		threaded_db_wrapper_query("INSERT INTO Device_DeviceData(FK_Device,FK_DeviceData) "
			"VALUES(" + StringUtils::itos(PK_Device) + "," + StringUtils::itos(PK_DeviceData) + ");",true);

	threaded_db_wrapper_query("UPDATE Device_DeviceData SET IK_DeviceData='"
		+ StringUtils::SQLEscape(sValue) + "' WHERE "
		"FK_Device=" + StringUtils::itos(PK_Device) + 
		" AND FK_DeviceData=" + StringUtils::itos(PK_DeviceData) );
}

void CreateDevice::CreateRelation(int PK_Device,int PK_Device_Related)
{
	string SQL = "SELECT FK_Device FROM Device_Device_Related WHERE FK_Device="
		+ StringUtils::itos(PK_Device) + " AND FK_Device_Related=" + StringUtils::itos(PK_Device_Related);

	PlutoSqlResult result1;
	result1.r=db_wrapper_query_result( SQL );
	if( !result1.r || result1.r->row_count==0 )
		threaded_db_wrapper_query("INSERT INTO Device_Device_Related(FK_Device,FK_Device_Related) "
			"VALUES(" + StringUtils::itos(PK_Device) + "," + StringUtils::itos(PK_Device_Related) + ");",true);
}

void CreateDevice::FixControlledViaIfEmbeddedIsMoreValid(int PK_DeviceTemplate,int &PK_Device_ControlledVia)
{
	// 1. If the controlled via is a valid choice, return without doing anything.
	if( DatabaseUtils::IsValidControlledVia(this,PK_DeviceTemplate,PK_Device_ControlledVia) )
		return;

	// 2. Check if there are embedded devices that are valid options
	string sSQL = "SELECT PK_Device FROM Device WHERE FK_Device_RouteTo=" + StringUtils::itos(PK_Device_ControlledVia);
	PlutoSqlResult result3;
	result3.r=db_wrapper_query_result( sSQL );
	if( result3.r )
	{
		DB_ROW row;
		while( (row=db_wrapper_fetch_row( result3.r )) )
		{
			// If this is a valid choice use it instead
			int PK_Device = atoi(row[0]);
			if( PK_Device && DatabaseUtils::IsValidControlledVia(this,PK_DeviceTemplate,PK_Device) )
			{
				PK_Device_ControlledVia = PK_Device;
				return;
			}
		}
	}
}

