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
//<-dceag-d-b->
#include "Infrared_Plugin.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->


#ifdef WIN32
    #define WEXITSTATUS(w)  (((w) >> 8) & 0xff)
#else
	#include <unistd.h>
#endif

#include "Orbiter_Plugin/Orbiter_Plugin.h" 

#include "DCE/DataGrid.h"
#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_Device.h"
#include "pluto_main/Table_DeviceTemplate_DeviceData.h"
#include "pluto_main/Table_DeviceCategory.h"
#include "pluto_main/Table_InfraredGroup.h"
#include "pluto_main/Table_InfraredGroup_Command.h"
#include "pluto_main/Table_InfraredGroup_Command_Preferred.h"
#include "pluto_main/Table_DeviceTemplate_InfraredGroup.h"
#include "pluto_main/Table_Command.h"
#include "pluto_main/Table_DeviceTemplate_AV.h"
#include "pluto_main/Table_DeviceTemplate_Input.h"
#include "pluto_main/Table_Manufacturer.h"
#include "pluto_main/Table_RemoteMapping.h"
#include "pluto_main/Define_DataGrid.h"
#include "pluto_main/Define_Variable.h"
#include "pluto_main/Define_CommMethod.h"
#include "pluto_main/Define_CommandParameter.h"
#include "IR/IRDevice.h"
#include "Gen_Devices/AllScreens.h"

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Infrared_Plugin::Infrared_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Infrared_Plugin_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
	m_pDatabase_pluto_main = NULL;
}

//<-dceag-getconfig-b->
bool Infrared_Plugin::GetConfig()
{
	if( !Infrared_Plugin_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	m_pDatabase_pluto_main = new Database_pluto_main(LoggerWrapper::GetInstance());
	if(!m_pDatabase_pluto_main->Connect(m_pRouter->sDBHost_get(),m_pRouter->sDBUser_get(),m_pRouter->sDBPassword_get(),m_pRouter->sDBName_get(),m_pRouter->iDBPort_get()) )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Cannot connect to database!");
		m_bQuit_set(true);
		return false;
	}

	vector<Row_RemoteMapping *> vectRow_RemoteMapping;
	m_pDatabase_pluto_main->RemoteMapping_get()->GetRows("1=1",&vectRow_RemoteMapping);
	for(size_t s=0;s<vectRow_RemoteMapping.size();++s)
	{
		Row_RemoteMapping *pRow_RemoteMapping = vectRow_RemoteMapping[s];
		m_sRemoteMapping += pRow_RemoteMapping->ScreenType_get() + "\t" + pRow_RemoteMapping->RemoteLayout_get() + "\t"
			+ pRow_RemoteMapping->Mapping_get() + "\t";
	}
	return true;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Infrared_Plugin::~Infrared_Plugin()
//<-dceag-dest-e->
{
	delete m_pDatabase_pluto_main;
	
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Infrared_Plugin::Register()
//<-dceag-reg-e->
{
	m_pDatagrid_Plugin=( Datagrid_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Datagrid_Plugin_CONST);
	m_pOrbiter_Plugin=( Orbiter_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Orbiter_Plugin_CONST);
	if( !m_pDatagrid_Plugin || !m_pOrbiter_Plugin )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find sister plugins to ir plugin");
		return false;
	}

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&Infrared_Plugin::DevicesGrid)),
		DATAGRID_Devices_by_Room_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&Infrared_Plugin::CommandsGrid)),
		DATAGRID_Commmands_By_Device_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&Infrared_Plugin::Manufacturers)),
		DATAGRID_Manufacturers_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&Infrared_Plugin::IRGroupCategories)),
		DATAGRID_Infrared_Group_Categories_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&Infrared_Plugin::InfraredGroups)),
		DATAGRID_Infrared_Groups_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&Infrared_Plugin::InfraredCodes)),
		DATAGRID_Infrared_Codes_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&Infrared_Plugin::DeviceTemplateByMfrModel)),
		DATAGRID_Device_Template_by_Mfr_Mo_CONST,PK_DeviceTemplate_get());
/*
	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this, (DCEDataGridGeneratorFn) (&Infrared_Plugin::ConfirmIRCodes)), 
		DATAGRID_Confirm_IR_Codes_CONST,PK_DeviceTemplate_get());
*/
	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&Infrared_Plugin::AvailableInputs)),
		DATAGRID_Available_Inputs_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&Infrared_Plugin::AVDeviceTypes)),
		DATAGRID_AVDeviceTypes_CONST,PK_DeviceTemplate_get());

	return Connect(PK_DeviceTemplate_get()); 
}

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void Infrared_Plugin::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	sCMD_Result = "UNHANDLED CHILD";
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void Infrared_Plugin::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

class DataGridTable *Infrared_Plugin::DevicesGrid(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage)
{
	DataGridTable *pDataGrid = new DataGridTable();
	DataGridCell *pCell;

	int Type = atoi(Parms.c_str());
	int iRow=0;

    for(map<int,Room *>::const_iterator it=m_pRouter->m_mapRoom_get()->begin();it!=m_pRouter->m_mapRoom_get()->end();++it)
    {
		Room *pRoom = (*it).second;
		bool bFirst=true;

		for(list<class DeviceData_Router *>::iterator itD=pRoom->m_listDevices.begin();itD!=pRoom->m_listDevices.end();++itD)
		{
			class DeviceData_Router *pDeviceData_Router = *itD;
			if( Type==1 && !pDeviceData_Router->WithinCategory(DEVICECATEGORY_AV_CONST) )
				continue;

			pCell = new DataGridCell( (bFirst ? pRoom->m_sDescription + "\n  " : "  ") + pDeviceData_Router->m_sDescription,
				StringUtils::itos(pDeviceData_Router->m_dwPK_Device) );
			pDataGrid->SetData(0,iRow++,pCell);
			bFirst=false;
		}
	}

	return pDataGrid;
}

class SortedIRData
{
public:
	int PK_Command;
	int PK_CommandCategory;
	string sCommand_Description;
	string sCommandCategory_Description;
	string sIRData;

	SortedIRData(Command *pCommand,string IRData)
	{
		if( !pCommand )
			return; // Should never happen
		PK_Command = pCommand->m_dwPK_Command;
		sCommand_Description = pCommand->m_sDescription;
		PK_CommandCategory = pCommand->m_dwPK_CommandCategory;
		sCommandCategory_Description = pCommand->m_sCommandCategory_Description;
		sIRData=IRData;
	}
};

static bool SortedIRDataComparer(SortedIRData *x, SortedIRData *y)
{
	if( x->PK_CommandCategory!=y->PK_CommandCategory )
		return StringUtils::ToUpper(x->sCommandCategory_Description)<StringUtils::ToUpper(y->sCommandCategory_Description);
	return StringUtils::ToUpper(x->sCommand_Description)<StringUtils::ToUpper(y->sCommand_Description);
}

class DataGridTable *Infrared_Plugin::CommandsGrid(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage)
{
	DataGridTable *pDataGrid = new DataGridTable();
	DataGridCell *pCell;

	string::size_type pos=0;
	int PK_Device = atoi(StringUtils::Tokenize(Parms,",",pos).c_str());
	if( !PK_Device )
		return pDataGrid;

	int PK_Orbiter = atoi(StringUtils::Tokenize(Parms,",",pos).c_str());
	int PK_Text = atoi(StringUtils::Tokenize(Parms,",",pos).c_str());

	int iRow=0;
	IRDevice irDevice;
	GetInfraredCodes(PK_Device,irDevice);
	DeviceData_Router *pDevice = m_pRouter->m_mapDeviceData_Router_Find(PK_Device);

	list<SortedIRData *> listSortedIRData;
	for(map<int,string>::iterator it=irDevice.m_mapCodes.begin();it!=irDevice.m_mapCodes.end();++it)
		listSortedIRData.push_back( new SortedIRData(m_pRouter->m_mapCommand_Find(it->first),it->second) );

	for(size_t s=0;s<irDevice.m_vectCommands_WithoutCodes.size();++s)
		listSortedIRData.push_back( new SortedIRData(m_pRouter->m_mapCommand_Find(irDevice.m_vectCommands_WithoutCodes[s]),"") );

	listSortedIRData.sort(SortedIRDataComparer);

	int PK_CommandCategory=0;
	for(list<SortedIRData *>::iterator it=listSortedIRData.begin();it!=listSortedIRData.end();++it)
	{
		SortedIRData *pSortedIRData = *it;

		if( pSortedIRData->sIRData.size()==0 )
			pSortedIRData->sCommand_Description += " (no data)";

		if( PK_CommandCategory==pSortedIRData->PK_CommandCategory )
			pCell = new DataGridCell( pSortedIRData->sCommand_Description, StringUtils::itos(pSortedIRData->PK_Command));
		else
			pCell = new DataGridCell( "CATEGORY: " + pSortedIRData->sCommandCategory_Description + "\n" + pSortedIRData->sCommand_Description, StringUtils::itos(pSortedIRData->PK_Command));
		PK_CommandCategory=pSortedIRData->PK_CommandCategory;

		pCell->m_Colspan = 4;
		if( irDevice.m_bImplementsDCE )
		{
			pCell->m_pMessage = new Message(PK_Orbiter,PK_Device,
				PRIORITY_NORMAL,MESSAGETYPE_COMMAND,pSortedIRData->PK_Command,0);
			pDataGrid->SetData(0,iRow++,pCell);
		}
		else
		{
			pCell->m_pMessage = new Message(PK_Orbiter,irDevice.m_PK_Device_ControlledVia,
				PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_Send_Code_CONST,1,
				COMMANDPARAMETER_Text_CONST,pSortedIRData->sIRData.c_str());
			DCE::CMD_Set_Text CMD_Set_Text(PK_Orbiter,PK_Orbiter,"","",PK_Text);
			pCell->m_pMessage->m_vectExtraMessages.push_back( CMD_Set_Text.m_pMessage );
			pDataGrid->SetData(0,iRow,pCell);

			pCell = new DataGridCell( "learn","" );
			DCE::CMD_Learn_IR CMD_Learn_IR(PK_Orbiter,irDevice.m_PK_Device_ControlledVia,PK_Device,"1",PK_Text,pSortedIRData->PK_Command);
			pCell->m_pMessage = CMD_Learn_IR.m_pMessage;
			DCE::CMD_Set_Text CMD_Set_Text2(PK_Orbiter,PK_Orbiter,"","Start learning...",PK_Text);
			pCell->m_pMessage->m_vectExtraMessages.push_back( CMD_Set_Text2.m_pMessage );

			pDataGrid->SetData(4,iRow++,pCell);
		}

		delete pSortedIRData;
	}

	return pDataGrid;
}

class DataGridTable *Infrared_Plugin::InfraredCodes(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage)
{
	DataGridTable *pDataGrid = new DataGridTable();
	DataGridCell *pCell;

	if( Parms.length()==0 )
		return pDataGrid;

	string::size_type pos=0;
	string sPK_Device = StringUtils::Tokenize(Parms,",",pos);
	string sPK_InfraredGroup = StringUtils::Tokenize(Parms,",",pos);

	Row_InfraredGroup *pRow_InfraredGroup = m_pDatabase_pluto_main->InfraredGroup_get()->GetRow( atoi(sPK_InfraredGroup.c_str()) );
	if( !pRow_InfraredGroup )
		return pDataGrid;

	Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow( atoi(sPK_Device.c_str()) );
	if( !pRow_Device )
		return pDataGrid;

	vector<Row_InfraredGroup_Command *> vectRow_InfraredGroup_Command;
	pRow_InfraredGroup->InfraredGroup_Command_FK_InfraredGroup_getrows(&vectRow_InfraredGroup_Command);

	if( vectRow_InfraredGroup_Command.size()==0 )
	{
		pCell = new DataGridCell( "No commands","" );
		pDataGrid->SetData(0,0,pCell);
	}

	for(size_t s=0;s<vectRow_InfraredGroup_Command.size();++s)
	{
		Row_InfraredGroup_Command *pRow_InfraredGroup_Command = vectRow_InfraredGroup_Command[s];
		pCell = new DataGridCell( pRow_InfraredGroup_Command->FK_Command_getrow()->Description_get(),"" );
		DCE::CMD_Send_Code CMD_Send_Code(m_dwPK_Device,pRow_Device->PK_Device_get(),pRow_InfraredGroup_Command->IRData_get());
		pCell->m_pMessage = CMD_Send_Code.m_pMessage;
		pDataGrid->SetData(0,s,pCell);
	}
	return pDataGrid;
}

class DataGridTable *Infrared_Plugin::AvailableInputs(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage)
{
	DataGridTable *pDataGrid = new DataGridTable();
	DataGridCell *pCell;
	
	int PK_DeviceTemplate=atoi(Parms.c_str());
	vector<Row_DeviceTemplate_Input *> vectRow_DeviceTemplate_Input;
	m_pDatabase_pluto_main->DeviceTemplate_Input_get()->GetRows("FK_DeviceTemplate=" + StringUtils::itos(PK_DeviceTemplate),
		&vectRow_DeviceTemplate_Input);

	int iRow=0,iColumn=0;
	int iMaxColumns = atoi( pMessage->m_mapParameters[COMMANDPARAMETER_Width_CONST].c_str() );

	for(size_t s=0;s<vectRow_DeviceTemplate_Input.size();++s)
	{
		Row_DeviceTemplate_Input *pRow_DeviceTemplate_Input = vectRow_DeviceTemplate_Input[s];
		pCell = new DataGridCell( pRow_DeviceTemplate_Input->FK_Command_getrow()->Description_get(),StringUtils::itos(pRow_DeviceTemplate_Input->FK_Command_get()) );
		/*		This means if the user already selected another value, it always defaults to hdmi/dvi when they return to the screen
		if( pRow_DeviceTemplate_Input->FK_Command_get()==COMMAND_DVI_CONST || pRow_DeviceTemplate_Input->FK_Command_get()==COMMAND_HDMI_CONST )
		{
			*iPK_Variable=VARIABLE_Misc_Data_4_CONST;
			*sValue_To_Assign=StringUtils::itos(pRow_DeviceTemplate_Input->FK_Command_get());
		}
		*/

		pDataGrid->SetData(iColumn,iRow,pCell);
		iColumn++;
		if( iColumn>=iMaxColumns )
		{
			iColumn=0;
			iRow++;
		}
	}

	return pDataGrid;
}

class DataGridTable *Infrared_Plugin::DeviceTemplateByMfrModel(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage)
{
	DataGridTable *pDataGrid = new DataGridTable();
	DataGridCell *pCell;
	Row_Device *pRow_Device = NULL;  // The initially selected manufacturer can be passed in as a device
	
	string::size_type pos=0;
	int PK_DeviceCategory=atoi(StringUtils::Tokenize(Parms,",",pos).c_str());
	int PK_Manufacturer=atoi(StringUtils::Tokenize(Parms,",",pos).c_str());
	Parms = StringUtils::Tokenize(Parms,",",pos); // Just get whatever is left over the user typed in as a search value
	int PK_DeviceTemplate=0;

	string sql = 
		"SELECT PK_DeviceTemplate, DeviceTemplate.Description, DeviceTemplate_DeviceCategory_ControlledVia.FK_DeviceCategory "
		"FROM DeviceTemplate "
		"LEFT JOIN DeviceTemplate_DeviceCategory_ControlledVia ON FK_DeviceTemplate = PK_DeviceTemplate "
		"WHERE DeviceTemplate.FK_DeviceCategory = " + StringUtils::itos(PK_DeviceCategory) + " AND " + 
		"FK_Manufacturer = " + StringUtils::itos(PK_Manufacturer);

	PlutoSqlResult result;
	DB_ROW row;

	map<string, pair<string, string> > mapDevices;	// devicetemplate <-> pair(description, controlledviacategory)
	string sInfraredInterfaceCategory = StringUtils::ltos(DEVICECATEGORY_Infrared_Interface_CONST);

	if( ( result.r = m_pDatabase_pluto_main->db_wrapper_query_result( sql ) ) )
	{
		while( ( row=db_wrapper_fetch_row( result.r ) ) )
		{
			string sPK_DeviceTemplate = row[0];
			string sDescription = row[1];
			string sControlledViaCategory = (NULL != row[2] ? row[2] : "");
			

			map<string, pair<string, string> >::iterator it = mapDevices.find(sPK_DeviceTemplate);
			if(it == mapDevices.end())
				mapDevices.insert(make_pair(sPK_DeviceTemplate, make_pair(sDescription, sControlledViaCategory)));
			else
				it->second.second = it->second.second == sInfraredInterfaceCategory ? it->second.second : sControlledViaCategory;
		}
	}

	int RowCount = 0;
	for(map<string, pair<string, string> >::iterator it = mapDevices.begin(), end = mapDevices.end(); it != end; ++it)
	{
		string sPK_DeviceTemplate = it->first;
		string sDescription = it->second.first;
		string sTextColor = (atoi(it->second.second.c_str()) == DEVICECATEGORY_Infrared_Interface_CONST ? "~S22~" : "");		

		if(!PK_DeviceTemplate && Parms.size() && StringUtils::StartsWith(sDescription, Parms, true))
			PK_DeviceTemplate = atoi(sDescription.c_str());

		pCell = new DataGridCell(sTextColor + sDescription, sPK_DeviceTemplate);
		pDataGrid->SetData(0, RowCount++, pCell);
	}

	if( PK_DeviceTemplate )
		*sValue_To_Assign = StringUtils::itos(PK_DeviceTemplate);

	return pDataGrid;
}

class DataGridTable *Infrared_Plugin::Manufacturers(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage)
{
	DataGridTable *pDataGrid = new DataGridTable();
	DataGridCell *pCell;
	Row_Device *pRow_Device = NULL;  // The initially selected manufacturer can be passed in as a device

	string::size_type pos=0;
	string sPK_DeviceCategory = StringUtils::Tokenize(Parms,",",pos);
	Parms = StringUtils::Tokenize(Parms,",",pos); // Just get whatever is left over the user typed in as a search value

	if( Parms.length() )  // If it's not a device, it won't be reset, and we will consider it a search value
	{
		pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow(atoi(Parms.c_str()));
		if( pRow_Device )
		{
			*iPK_Variable = VARIABLE_Misc_Data_1_CONST;
			*sValue_To_Assign = StringUtils::itos(pRow_Device->FK_DeviceTemplate_getrow()->FK_Manufacturer_get());
			Parms="";
		}
	}

	int PK_Manufacturer=0;
	string sWhereCondition;
	if(sPK_DeviceCategory != "" && atoi(sPK_DeviceCategory.c_str()) > 0)
		sWhereCondition = "JOIN DeviceTemplate ON PK_Manufacturer = FK_Manufacturer "
			"WHERE FK_DeviceCategory = " + sPK_DeviceCategory + " "
			"ORDER BY Manufacturer.Description";
	else
		sWhereCondition = "1=1 ORDER BY Manufacturer.Description";

	vector<Row_Manufacturer *> vectRow_Manufacturer;
	m_pDatabase_pluto_main->Manufacturer_get()->GetRows(sWhereCondition, &vectRow_Manufacturer);
	long PK_LastManufacturer = 0;
	int nIndex = 0;
	for(size_t s=0;s<vectRow_Manufacturer.size();++s)
	{
		Row_Manufacturer *pRow_Manufacturer = vectRow_Manufacturer[s];

		//ignore duplicates - force distinct
		if(PK_LastManufacturer != pRow_Manufacturer->PK_Manufacturer_get())
		{
			if( !PK_Manufacturer && Parms.size() && StringUtils::StartsWith(pRow_Manufacturer->Description_get(),Parms,true) )
				PK_Manufacturer = pRow_Manufacturer->PK_Manufacturer_get();
			pCell = new DataGridCell( pRow_Manufacturer->Description_get(),StringUtils::itos(pRow_Manufacturer->PK_Manufacturer_get()) );
			pDataGrid->SetData(0, nIndex++, pCell);

			PK_LastManufacturer = pRow_Manufacturer->PK_Manufacturer_get();
		}
	}

	if( PK_Manufacturer )
		*sValue_To_Assign = StringUtils::itos(PK_Manufacturer);

	return pDataGrid;
}

class DataGridTable *Infrared_Plugin::InfraredGroups(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage)
{
	DataGridTable *pDataGrid = new DataGridTable();
	DataGridCell *pCell;

	string::size_type pos=0;
	string sPK_Manufacturer = StringUtils::Tokenize(Parms,",",pos);
	string sPK_DeviceCategory = StringUtils::Tokenize(Parms,",",pos);

	if( !sPK_DeviceCategory.length() )
	{
		if( !sPK_Manufacturer.length() )
			return pDataGrid;
		// If we got in only 1 number, it's really a device
		Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow(atoi(sPK_Manufacturer.substr(1).c_str()));
		if( !pRow_Device )
			return pDataGrid;
		sPK_Manufacturer = StringUtils::itos(pRow_Device->FK_DeviceTemplate_getrow()->FK_Manufacturer_get());
		sPK_DeviceCategory = StringUtils::itos(pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_get());
	}

	vector<Row_InfraredGroup *> vectRow_InfraredGroup;
	m_pDatabase_pluto_main->InfraredGroup_get()->GetRows("FK_Manufacturer=" + sPK_Manufacturer + " AND FK_DeviceCategory=" + sPK_DeviceCategory,&vectRow_InfraredGroup);

	for(size_t s=0;s<vectRow_InfraredGroup.size();++s)
	{
		Row_InfraredGroup *pRow_InfraredGroup = vectRow_InfraredGroup[s];
		pCell = new DataGridCell( pRow_InfraredGroup->Description_get(), StringUtils::itos(pRow_InfraredGroup->PK_InfraredGroup_get()) );
        pDataGrid->SetData( 0, s, pCell );
	}

	return pDataGrid;
}

class DataGridTable *Infrared_Plugin::IRGroupCategories(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage)
{
	DataGridTable *pDataGrid = new DataGridTable();
	DataGridCell *pCell;	

	if( Parms.length()==0 )
		return pDataGrid;

	Row_Device *pRow_Device = NULL;
	// The first time the populate is called when the screen appears we don't know the category yet.  The initial options for this
	// grid pass in "D" + the device number, and we fill in the category automatically.  After that we're passed in the manufacturer
	if( Parms.length()>1 && Parms[0]=='D' )
	{
		pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow(atoi(Parms.substr(1).c_str()));
		if( pRow_Device )
		{
			*iPK_Variable = VARIABLE_Misc_Data_2_CONST;
			*sValue_To_Assign = StringUtils::itos(pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_get());
			Parms = StringUtils::itos(pRow_Device->FK_DeviceTemplate_getrow()->FK_Manufacturer_get());  // We're going to expect this to be the manufacturer
		}
	}

	string sql = "SELECT DISTINCT PK_DeviceCategory,DeviceCategory.Description FROM InfraredGroup JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory WHERE FK_Manufacturer=" + Parms;

	PlutoSqlResult result;
    DB_ROW row;
    int RowCount=0;

    if( ( result.r=m_pDatabase_pluto_main->db_wrapper_query_result( sql ) ) )
    {
        while( ( row=db_wrapper_fetch_row( result.r ) ) )
        {
            pCell = new DataGridCell( row[1], row[0] );
            pDataGrid->SetData( 0, RowCount++, pCell );
		}
	}
	return pDataGrid;
}

//<-dceag-sample-b->!

/*

	COMMANDS TO IMPLEMENT

*/

//<-dceag-c188-b->

	/** @brief COMMAND: #188 - Get Infrared Codes */
	/** Retrieves all the infrared codes for a given device. */
		/** @param #2 PK_Device */
			/** The device to retrieve the infrared codes for. */
		/** @param #19 Data */
			/** A serialized map(int,string) with the commands/codes. */

void Infrared_Plugin::CMD_Get_Infrared_Codes(int iPK_Device,char **pData,int *iData_Size,string &sCMD_Result,Message *pMessage)
//<-dceag-c188-e->
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"start infrared codes");
	IRDevice irDevice;
	GetInfraredCodes(iPK_Device,irDevice);
	irDevice.SerializeWrite();
	*iData_Size = irDevice.m_dwAllocatedSize;
	*pData = irDevice.m_pcDataBlock;
	sCMD_Result = "OK";
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"endinfrared codes");
}

void Infrared_Plugin::GetInfraredCodes(int iPK_Device,IRDevice &irDevice,bool bNoIRData)
{
	int i;
	size_t Count = 0;
	Table_InfraredGroup_Command * pTable_InfraredGroup_Command = m_pDatabase_pluto_main->InfraredGroup_Command_get();

	vector<Row_InfraredGroup_Command *> vectRow_InfraredGroup_Command;

	Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow(iPK_Device);
	if( !pRow_Device )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Infrared_Plugin::GetInfraredCodes invalid device");
		return;
	}
	pRow_Device->Reload();  // Get the latest so the user doesn't need to do a quick reload router
	Row_DeviceTemplate *pRow_DeviceTemplate = pRow_Device->FK_DeviceTemplate_getrow();
	if( !pRow_DeviceTemplate )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Infrared_Plugin::GetInfraredCodes invalid device template");
		return;
	}
	pRow_DeviceTemplate->Reload();

	irDevice.m_PK_Device_ControlledVia = pRow_Device->FK_Device_ControlledVia_get();

	Row_DeviceTemplate_AV *pRow_DeviceTemplate_AV = m_pDatabase_pluto_main->DeviceTemplate_AV_get()->GetRow(pRow_Device->FK_DeviceTemplate_get());
	if( pRow_DeviceTemplate_AV )
	{
		pRow_DeviceTemplate_AV->Reload();
		irDevice.m_bTogglePower=pRow_DeviceTemplate_AV->TogglePower_get()==1;
		irDevice.m_iToggleDSP=pRow_DeviceTemplate_AV->ToggleDSP_get();
		irDevice.m_iToggleInput=pRow_DeviceTemplate_AV->ToggleInput_get();
		irDevice.m_iToggleOutput=pRow_DeviceTemplate_AV->ToggleOutput_get();
		irDevice.m_iPowerDelay=pRow_DeviceTemplate_AV->IR_PowerDelay_get();
		irDevice.m_iModeDelay=pRow_DeviceTemplate_AV->IR_ModeDelay_get();
		irDevice.m_iDigitDelay=pRow_DeviceTemplate_AV->DigitDelay_get();
		irDevice.m_sNumericEntry=pRow_DeviceTemplate_AV->NumericEntry_get();
		if( pRow_DeviceTemplate_AV->RepeatIR_get() )
			irDevice.m_iRepeatIR = pRow_DeviceTemplate_AV->RepeatIR_get();
		if( pRow_DeviceTemplate_AV->RepeatVolume_get() )
			irDevice.m_iRepeatVolume = pRow_DeviceTemplate_AV->RepeatVolume_get();
	}

	irDevice.m_bUsesIR = (pRow_DeviceTemplate->FK_CommMethod_get()==COMMMETHOD_Infrared_CONST);
	irDevice.m_bImplementsDCE = pRow_DeviceTemplate->ImplementsDCE_get()==1;

	m_pDatabase_pluto_main->InfraredGroup_Command_get()->GetRows(INFRAREDGROUP_COMMAND_FK_INFRAREDGROUP_FIELD "=" + StringUtils::itos(pRow_DeviceTemplate->FK_InfraredGroup_get()),&vectRow_InfraredGroup_Command);

	map<int,bool> mapCommandsWithoutCodes; // First store them here, then double check before we actually add them
	vector<Row_InfraredGroup_Command *>::iterator it_vRIGC;
	for (it_vRIGC = vectRow_InfraredGroup_Command.begin(); it_vRIGC != vectRow_InfraredGroup_Command.end(); it_vRIGC++)
	{
		Row_InfraredGroup_Command * pRow_InfraredGroup_Command = * it_vRIGC;
		pRow_InfraredGroup_Command->Reload();  // Be sure we have the latest in case the user is debugging stuff
		if( pRow_InfraredGroup_Command->IRData_get().size()==0 || StringUtils::OnlyWhiteSpace(pRow_InfraredGroup_Command->IRData_get().c_str()) )
			mapCommandsWithoutCodes[pRow_InfraredGroup_Command->FK_Command_get()]=true;
		else
			irDevice.m_mapCodes[pRow_InfraredGroup_Command->FK_Command_get()] = pRow_InfraredGroup_Command->IRData_get();
	}

	for(map<int,bool>::iterator it=mapCommandsWithoutCodes.begin();it!=mapCommandsWithoutCodes.end();++it)
		if( irDevice.m_mapCodes.find(it->first)==irDevice.m_mapCodes.end() )
			irDevice.m_vectCommands_WithoutCodes.push_back(it->first);

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Infrared_Plugin::GetInfraredCodes %d records %d cmds without codes %d codes",
		(int) vectRow_InfraredGroup_Command.size(), (int) irDevice.m_vectCommands_WithoutCodes.size(), (int) irDevice.m_mapCodes.size() );
}
//<-dceag-c250-b->

	/** @brief COMMAND: #250 - Store Infrared Code */
	/** Store a learned infrared code for a "Device" + "Command" pair */
		/** @param #2 PK_Device */
			/** Device this code was learned for */
		/** @param #5 Value To Assign */
			/** IR code in Philips pronto format */
		/** @param #71 PK_Command_Input */
			/** Command this code launches */

void Infrared_Plugin::CMD_Store_Infrared_Code(int iPK_Device,string sValue_To_Assign,int iPK_Command_Input,string &sCMD_Result,Message *pMessage)
//<-dceag-c250-e->
{
	Table_InfraredGroup_Command * pTable_InfraredGroup_Command = m_pDatabase_pluto_main->InfraredGroup_Command_get();
	Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow(iPK_Device);
	if( !pRow_Device )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Infrared_Plugin::CMD_Store_Infrared_Code Device %d is invalid",iPK_Device);
		return;
	}

	Row_DeviceTemplate *pRow_DeviceTemplate = pRow_Device->FK_DeviceTemplate_getrow();
	Row_InfraredGroup *pRow_InfraredGroup;
	if( !pRow_DeviceTemplate || (pRow_InfraredGroup = pRow_DeviceTemplate->FK_InfraredGroup_getrow())==NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Infrared_Plugin::CMD_Store_Infrared_Code Device %d has no dt %p or no i/r group",iPK_Device,pRow_DeviceTemplate);
		return;
	}

	vector<Row_InfraredGroup_Command *> vectRow_InfraredGroup_Command;
	pTable_InfraredGroup_Command->GetRows("FK_InfraredGroup=" + StringUtils::itos(pRow_InfraredGroup->PK_InfraredGroup_get()) +
		" AND FK_Command=" + StringUtils::itos(iPK_Command_Input), &vectRow_InfraredGroup_Command);
LoggerWrapper::GetInstance()->Write(LV_STATUS,"StoreIR Code.  Found %d rows for this already",(int) vectRow_InfraredGroup_Command.size());
	bool bNew = false;
	Row_InfraredGroup_Command *pRow_InfraredGroup_Command=NULL;
	if (vectRow_InfraredGroup_Command.size() == 0)
	{
		// we found no existing entry for the Command+Device pair; adding a new one
		pRow_InfraredGroup_Command = pTable_InfraredGroup_Command->AddRow();
		pRow_InfraredGroup_Command->FK_InfraredGroup_set(pRow_InfraredGroup->PK_InfraredGroup_get());
		pRow_InfraredGroup_Command->FK_Command_set(iPK_Command_Input);
		pRow_InfraredGroup_Command->IRData_set(sValue_To_Assign);
		bNew = true;
	}
	else
	{
		// we found an entry; updating it
		pRow_InfraredGroup_Command = vectRow_InfraredGroup_Command[0];
		pRow_InfraredGroup_Command->IRData_set(sValue_To_Assign);
	}
	pTable_InfraredGroup_Command->Commit();
LoggerWrapper::GetInstance()->Write(LV_STATUS,"In the database as PK_IRG_C %d",pRow_InfraredGroup_Command->PK_InfraredGroup_Command_get());

}
//<-dceag-createinst-b->!
//<-dceag-c276-b->

	/** @brief COMMAND: #276 - Add GC100 */
	/** Add a GC100 Device */

void Infrared_Plugin::CMD_Add_GC100(string &sCMD_Result,Message *pMessage)
//<-dceag-c276-e->
{
	string Command;
	char line[45];
	int returned, size, exit_status;
	FILE *fp;

	Command = "/usr/pluto/bin/gc100-conf.pl";
	int iPK_Device_Orbiter = pMessage->m_dwPK_Device_From;

	SCREEN_DialogGC100Error SCREEN_DialogGC100Error(m_dwPK_Device, iPK_Device_Orbiter, "Finding GC100", "1");
	SendCommand(SCREEN_DialogGC100Error);

	returned = system(Command.c_str());
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Find gc100 returned %d",returned);
	exit_status=WEXITSTATUS(returned);
	if ( returned == -1) {
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Failed Spawning configure script");
		//m_pOrbiter_Plugin->DisplayMessageOnOrbiter(iPK_Device_Orbiter,"GC100 Failed, Spwning config script",false,30);
		DCE::SCREEN_DialogGC100Error SCREEN_DialogGC100Error(m_dwPK_Device, iPK_Device_Orbiter, "GC100 Failed, Spawning config script", "0");
		SendCommand(SCREEN_DialogGC100Error);
	} else if( returned == 0) {
		size_t s;
		const char *ptr = FileUtils::ReadFileIntoBuffer("/var/log/pluto/gc100-conf.log",s);
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "The configure script returned with success.  log %d bytes: %s",s,ptr ? ptr : "NULL");
// The script will fire the new pp		m_pOrbiter_Plugin->DisplayMessageOnOrbiter(iPK_Device_Orbiter,"GC100 added with success",false,30);
	} else if( exit_status == 1) {
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "GC100 as default not found");
		//m_pOrbiter_Plugin->DisplayMessageOnOrbiter(iPK_Device_Orbiter,"GC100 Not Found as factory default",false,30);
		DCE::SCREEN_DialogGC100Error SCREEN_DialogGC100Error(m_dwPK_Device, iPK_Device_Orbiter, "GC100 Not Found as factory default", "0");
		SendCommand(SCREEN_DialogGC100Error);
	} else if( exit_status == 2) {
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "GC100 already exist in the databse");
		//m_pOrbiter_Plugin->DisplayMessageOnOrbiter(iPK_Device_Orbiter,"GC100 Allready exists",false,30);
		DCE::SCREEN_DialogGC100Error SCREEN_DialogGC100Error(m_dwPK_Device, iPK_Device_Orbiter, "GC100 Allready exists", "0");
		SendCommand(SCREEN_DialogGC100Error);

	} else if( exit_status == 3) {
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "GC100 config did not found instalation number");
		//m_pOrbiter_Plugin->DisplayMessageOnOrbiter(iPK_Device_Orbiter,"GC100 Failed, invalid instalation number",false,30);
		DCE::SCREEN_DialogGC100Error SCREEN_DialogGC100Error(m_dwPK_Device, iPK_Device_Orbiter, "GC100 Failed, invalid instalation number", "0");
		SendCommand(SCREEN_DialogGC100Error);

	} else if( exit_status == 4) {
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "GC100 config did not found Template number");
		//m_pOrbiter_Plugin->DisplayMessageOnOrbiter(iPK_Device_Orbiter,"GC100 Failed, invalid Template number",false,30);
		DCE::SCREEN_DialogGC100Error SCREEN_DialogGC100Error(m_dwPK_Device, iPK_Device_Orbiter, "GC100 Failed, invalid Template number", "0");
		SendCommand(SCREEN_DialogGC100Error);
	} else {
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "The config script returned weird error %d",exit_status);
		//m_pOrbiter_Plugin->DisplayMessageOnOrbiter(iPK_Device_Orbiter,"GC100 Failed. "
        //    "Please make sure that the device was reseted to factory settings (response " + 
        //    StringUtils::itos(exit_status) + ")",false,30);
		string sMessage = "GC100 Failed. "
			"Please make sure that the device was reseted to factory settings (response " + 
			StringUtils::itos(exit_status) + ")";
		DCE::SCREEN_DialogGC100Error SCREEN_DialogGC100Error(m_dwPK_Device, iPK_Device_Orbiter, sMessage, "0");
		SendCommand(SCREEN_DialogGC100Error);
	}
}

class DataGridTable *Infrared_Plugin::AVDeviceTypes(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage)
{
	DataGridTable *pDataGrid = new DataGridTable();
	DataGridCell *pCell;
	Row_Device *pRow_Device = NULL;  // The initially selected manufacturer can be passed in as a device

	string::size_type pos=0;
	Parms = StringUtils::Tokenize(Parms,",",pos); // Just get whatever is left over the user typed in as a search value

	int PK_DeviceCategory=0;
	vector<Row_DeviceCategory *> vectRow_DeviceCategory;
	m_pDatabase_pluto_main->DeviceCategory_get()->GetRows(
		"FK_DeviceCategory_Parent = " + StringUtils::itos(DEVICECATEGORY_AV_CONST) + 
		" ORDER BY Description",&vectRow_DeviceCategory);
	for(size_t s=0;s<vectRow_DeviceCategory.size();++s)
	{
		Row_DeviceCategory *pRow_DeviceCategory = vectRow_DeviceCategory[s];
		if( !PK_DeviceCategory && Parms.size() && StringUtils::StartsWith(pRow_DeviceCategory->Description_get(),Parms,true) )
			PK_DeviceCategory = pRow_DeviceCategory->PK_DeviceCategory_get();
		pCell = new DataGridCell( pRow_DeviceCategory->Description_get(),StringUtils::itos(pRow_DeviceCategory->PK_DeviceCategory_get()) );
		pDataGrid->SetData(0,s,pCell);
	}

	if( PK_DeviceCategory )
		*sValue_To_Assign = StringUtils::itos(PK_DeviceCategory);

	return pDataGrid;
}

//<-dceag-c688-b->

	/** @brief COMMAND: #688 - Get Remote Control Mapping */
	/** Returns a list of all the commands and mapping information for i/r remotes */
		/** @param #5 Value To Assign */
			/** A list with ScreenType\tRemoteLayout\tMapping */

void Infrared_Plugin::CMD_Get_Remote_Control_Mapping(string *sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c688-e->
{
	(*sValue_To_Assign) = m_sRemoteMapping;
}

//<-dceag-c793-b->

	/** @brief COMMAND: #793 - Get Sibling Remotes */
	/** Return a list of remote remote IDs with remote data. */
		/** @param #5 Value To Assign */
			/** Tilde delimited list, Remote DeviceID, Remote Configuration Data */
		/** @param #209 PK_DeviceCategory */
			/** Device category to search for remotes */

void Infrared_Plugin::CMD_Get_Sibling_Remotes(int iPK_DeviceCategory,string *sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c793-e->
{
	// This is a special workaround so that the remote control can work on the first boot before it's added to the database
	// If the I/R receiver is running with this device id, that means nothing is in the database.  Just give it all remotes
	if( pMessage->m_dwPK_Device_From==DEVICEID_MESSAGESEND )
	{
		vector<Row_DeviceTemplate *> vectRow_DeviceTemplate;
		m_pDatabase_pluto_main->DeviceTemplate_get()->GetRows( DEVICETEMPLATE_FK_DEVICECATEGORY_FIELD "=" + StringUtils::itos(iPK_DeviceCategory), &vectRow_DeviceTemplate );
		for( vector<Row_DeviceTemplate *>::iterator it=vectRow_DeviceTemplate.begin();it!=vectRow_DeviceTemplate.end();++it )
		{
			Row_DeviceTemplate *pRow_DeviceTemplate = *it;
			Row_DeviceTemplate_DeviceData *pRow_DeviceTemplate_DeviceData_Remote = m_pDatabase_pluto_main->DeviceTemplate_DeviceData_get()->GetRow(pRow_DeviceTemplate->PK_DeviceTemplate_get(), DEVICEDATA_Remote_Layout_CONST);
			Row_DeviceTemplate_DeviceData *pRow_DeviceTemplate_DeviceData_Config = m_pDatabase_pluto_main->DeviceTemplate_DeviceData_get()->GetRow(pRow_DeviceTemplate->PK_DeviceTemplate_get(), DEVICEDATA_Configuration_CONST);
			string sRemoteLayout = pRow_DeviceTemplate_DeviceData_Remote ? pRow_DeviceTemplate_DeviceData_Remote->IK_DeviceData_get() : "";
			string sConfiguration = pRow_DeviceTemplate_DeviceData_Config ? pRow_DeviceTemplate_DeviceData_Config->IK_DeviceData_get() : "";

			if ( sRemoteLayout.size() && sConfiguration.size() )
			{
				if((*sValue_To_Assign).length() > 0)
					(*sValue_To_Assign)+="`";
				(*sValue_To_Assign)+=StringUtils::itos(pRow_DeviceTemplate->PK_DeviceTemplate_get())+"~"+sRemoteLayout+"~"+sConfiguration;
			}
		}
		return;
	}
	
	// Find all our sibblings that are remote controls 
	Table_Device * pTable_Device = m_pDatabase_pluto_main->Device_get();

	Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow(pMessage->m_dwPK_Device_From);
	if( !pRow_Device )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Get sibling remote called from device %d that's not in the database!", pMessage->m_dwPK_Device_From);
		return;
	}
	int FK_Device_ControlledVia = pRow_Device->FK_Device_ControlledVia_get();
	if (!FK_Device_ControlledVia)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Parent of calling device %d not found!", pMessage->m_dwPK_Device_From);
		return;
	}
	vector<Row_Device *> vectRow_Device;

	string sWhere = "FK_Device_ControlledVia = "+StringUtils::itos(FK_Device_ControlledVia);
	if (!m_pDatabase_pluto_main->Device_get()->GetRows(sWhere, &vectRow_Device))
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Device GetRows Failed, WHERE %s", sWhere.c_str());
		return;
	}
	(*sValue_To_Assign).empty();
	for(int i=0;i<vectRow_Device.size();i++)
	{
		if (vectRow_Device[i]->FK_DeviceTemplate_getrow()->FK_DeviceCategory_get() == iPK_DeviceCategory)
		{
			if ((*sValue_To_Assign).length() > 0)
				(*sValue_To_Assign)+="`";

			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Using remote %d %s",vectRow_Device[i]->PK_Device_get(), vectRow_Device[i]->Description_get().c_str());
			string sRemoteLayout = m_pDatabase_pluto_main->Device_DeviceData_get()->GetRow(vectRow_Device[i]->PK_Device_get(), DEVICEDATA_Remote_Layout_CONST)->IK_DeviceData_get();
			string sConfiguration = m_pDatabase_pluto_main->Device_DeviceData_get()->GetRow(vectRow_Device[i]->PK_Device_get(), DEVICEDATA_Configuration_CONST)->IK_DeviceData_get();
			(*sValue_To_Assign)+=StringUtils::itos(vectRow_Device[i]->PK_Device_get())+"~"+sRemoteLayout+"~"+sConfiguration;
		}
	}
}
//<-dceag-c794-b->

