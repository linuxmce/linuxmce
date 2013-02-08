/*
 File_Grids_Plugin

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com

 Phone: +1 (877) 758-8648

 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */

//<-dceag-d-b->
#include "File_Grids_Plugin.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "FileListGrid.h"
#include "FileListOps.h"
#include "DeviceData_Router.h"
#include "DCE/DataGrid.h"
#include "DCERouter.h"
#include "Datagrid_Plugin/Datagrid_Plugin.h"
#include "Media_Plugin/Media_Plugin.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_DesignObjParameter.h"
#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "pluto_main/Table_MediaType.h"
#include "pluto_main/Define_DataGrid.h"
#include "pluto_main/Define_Variable.h"

#include "FileListOps.h"

#ifndef WIN32
    #include <dirent.h>
    #ifndef __APPLE_CC__
        #include <attr/attributes.h>
    #endif
#endif


//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
File_Grids_Plugin::File_Grids_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: File_Grids_Plugin_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
	m_pDatabase_pluto_main=NULL;
}

//<-dceag-getconfig-b->
bool File_Grids_Plugin::GetConfig()
{
	if( !File_Grids_Plugin_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	m_pDatabase_pluto_main = new Database_pluto_main(LoggerWrapper::GetInstance());
	if(!m_pDatabase_pluto_main->Connect(m_pRouter->sDBHost_get(),m_pRouter->sDBUser_get(),m_pRouter->sDBPassword_get(),m_pRouter->sDBName_get(),m_pRouter->iDBPort_get()) )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Cannot connect to database!");
		m_bQuit_set(true);
		return false;
	}
	return true;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
File_Grids_Plugin::~File_Grids_Plugin()
//<-dceag-dest-e->
{
	delete m_pDatabase_pluto_main;
	
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool File_Grids_Plugin::Register()
//<-dceag-reg-e->
{
	m_pDatagrid_Plugin=( Datagrid_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Datagrid_Plugin_CONST);
	m_pMedia_Plugin=( Media_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Media_Plugin_CONST);
	if( !m_pDatagrid_Plugin || !m_pMedia_Plugin )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find sister plugins to file grids");
		return false;
	}

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&File_Grids_Plugin::FileList))
		,DATAGRID_Directory_Listing_CONST,PK_DeviceTemplate_get());


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
void File_Grids_Plugin::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void File_Grids_Plugin::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}


class DataGridTable * File_Grids_Plugin::FileList(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage)
{
#ifdef DEBUG
LoggerWrapper::GetInstance()->Write(LV_WARNING,"Starting File list");
#endif
	int iMaxColumns = atoi( pMessage->m_mapParameters[COMMANDPARAMETER_Width_CONST].c_str() );
	int PK_DataGrid = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_PK_DataGrid_CONST].c_str());
	FileListGrid *pDataGrid = new FileListGrid(m_pDatagrid_Plugin,m_pMedia_Plugin);
	DataGridCell *pCell;

	string::size_type pos2 = 0;
	StringUtils::Tokenize(GridID, "_", pos2);
	int PK_Controller = atoi(StringUtils::Tokenize(GridID, "_", pos2).c_str());

	// extract individual parameters
	string::size_type pos=0;

	// Paths can be a comma separated list of Paths.  The grid will be the contents
	// of all paths merged into 1 grid
	string Paths = StringUtils::Tokenize(Parms, "\n", pos);

	// A comma separated list of file extensions.  Blank means all files
	string Extensions = StringUtils::Tokenize(Parms, "\n", pos);

	// Because the orbiter cannot execute different actions for files than directories, it will pass an "Actions"
	// parameter that will tell the file grid generator what type of command to attach to file entries.
	// P = "play this file", which is a normal start media command
	// S = "show the play button", which is a normal start media command
	string Actions = StringUtils::Tokenize(Parms, "\n", pos);

	bool bSortByDate = StringUtils::Tokenize(Parms, "\n", pos)=="1";

	// This grid is initially called by the orbiter without the iDirNumber and sSubDirectory
	// When the grid creates cells for sub-directories, it will create an action that re-populates,
	// like a recursive function, and indicate which of the directories it recursed and what path
	// here.  When it repopulates itself again, it will this time populate only the contents of the
	// Path[iDirNumber] + sSubDirectory.  It will also add a 'parent' button that that drops
	// 1 path off the sSubDirectory and repopulates itself
	int iDirNumber = atoi(StringUtils::Tokenize(Parms, "\n", pos).c_str());
	string sSubDirectory = StringUtils::Tokenize(Parms, "\n", pos);
	if( sSubDirectory.length()==1 && (sSubDirectory[0]=='/' || sSubDirectory[0]=='\\') )
		sSubDirectory = "";

	int iRow=0;
	int iColumn=0;
	if( sSubDirectory.length() )
	{
		string sParent = FileUtils::BasePath(sSubDirectory) + "/";
		string newParams = Paths + "\n" + Extensions + "\n" + Actions + "\n" + (bSortByDate ? "1" : "0")
			+ "\n" + StringUtils::itos(iDirNumber)+ "\n" + sParent;
		DCE::CMD_NOREP_Populate_Datagrid_DT CMDPDG(PK_Controller, DEVICETEMPLATE_Datagrid_Plugin_CONST, BL_SameHouse,
			"", GridID, DATAGRID_Directory_Listing_CONST, newParams, 0);

		pCell = new DataGridCell("~S21~<-- Back (..) - " + FileUtils::FilenameWithoutPath(sSubDirectory), sSubDirectory);
		pCell->m_pMessage = new Message(CMDPDG.m_pMessage);
		if( PK_DataGrid==DATAGRID_Media_Browser_CONST )
		{
			pDataGrid->SetData(iColumn, iRow, pCell);
			INCREMENT_ROW_COLUMN
		}
		else
		{
			pCell->m_Colspan = 6;
			pDataGrid->SetData(1, iRow, pCell);

			pCell = new DataGridCell("", "");
			pCell->m_pMessage = new Message(CMDPDG.m_pMessage);
			pDataGrid->SetData(0, iRow++, pCell);
		}

		delete CMDPDG.m_pMessage;
		CMDPDG.m_pMessage = NULL;

		pDataGrid->m_vectFileInfo.push_back(new FileListInfo(true,Paths,true));
	}

	bool bMoviesFolder = Paths.substr(0, Paths.find("\t")) == "/home/public/data/videos";
		
	//Jukeboxes special hack
	vector<Row_Device *> vectRow_Device;
	if(bMoviesFolder)
	{
		//this is the movies top level folder; we'll add at the beginning an entry for each jukeboxe from current installation
		m_pDatabase_pluto_main->Device_get()->GetRows(
			"FK_DeviceTemplate = " + StringUtils::ltos(DEVICETEMPLATE_Powerfile_C200_CONST) + " AND "  + 
			"FK_Installation = " + StringUtils::ltos(m_pRouter->iPK_Installation_get()), 
			&vectRow_Device);	
		
		if(!sSubDirectory.length()) 
		{
			for(vector<Row_Device *>::iterator iPowerFile = vectRow_Device.begin(); iPowerFile != vectRow_Device.end(); iPowerFile++)
			{
				Row_Device *pRow_Device = *iPowerFile;
				string sItemName = "Jukebox: " + pRow_Device->Description_get();
				FileListInfo *pFileListInfo = new FileListInfo(true, sItemName, false);
		
				pCell = new DataGridCell("~S2~" + sItemName, sItemName);

				DataGridCell *pCellPicture = new DataGridCell("", sItemName);
					
				string newParams = Paths + "\n" + Extensions + "\n" + Actions + "\n" + (bSortByDate ? "1" : "0")
					+ "\n" + StringUtils::itos(iRow)+ "\n" + sItemName;
				DCE::CMD_NOREP_Populate_Datagrid_DT CMDPDG(PK_Controller, DEVICETEMPLATE_Datagrid_Plugin_CONST, BL_SameHouse,
					"", GridID, DATAGRID_Directory_Listing_CONST, newParams, 0);
				pCell->m_pMessage = new Message(CMDPDG.m_pMessage);
				pCellPicture->m_pMessage = new Message(CMDPDG.m_pMessage);			
				delete CMDPDG.m_pMessage;
				CMDPDG.m_pMessage = NULL;
				
				if( PK_DataGrid==DATAGRID_Media_Browser_CONST )
				{
					pDataGrid->SetData(iColumn, iRow, pCell);
					INCREMENT_ROW_COLUMN
				}
				else
				{
					pDataGrid->SetData(0, iRow, pCellPicture);	
					pCell->m_Colspan = 6;
					pDataGrid->SetData(1, iRow++, pCell);
				}
				pDataGrid->m_vectFileInfo.push_back(pFileListInfo);
			}
		}
		else if( StringUtils::StartsWith(sSubDirectory,"Jukebox: ") )
		{
			//not a top level folder
			for(vector<Row_Device *>::iterator iPowerFile = vectRow_Device.begin(); iPowerFile != vectRow_Device.end(); iPowerFile++)
			{
				Row_Device *pRow_Device = *iPowerFile;
				
				if("Jukebox: " + pRow_Device->Description_get() == sSubDirectory)
				{
					//we are in a "jukebox"; let's show its movies
					string sStatus;
					CMD_Get_Jukebox_Status CMD_Get_Jukebox_Status_(m_dwPK_Device, pRow_Device->PK_Device_get(), ""/*force=no*/, &sStatus);
					
					if(!SendCommand(CMD_Get_Jukebox_Status_))
					{
						FileListInfo *pFileListInfo = new FileListInfo(true, "", false);

						pCell = new DataGridCell("Unable to communicate with Powerfile '" + pRow_Device->Description_get() + "'", "");
						if( PK_DataGrid==DATAGRID_Media_Browser_CONST )
						{
							pDataGrid->SetData(iColumn, iRow, pCell);
							INCREMENT_ROW_COLUMN
						}
						else
						{
							pCell->m_Colspan = 6;
							pDataGrid->SetData(1, iRow, pCell);						
							pDataGrid->SetData(0, iRow++, new DataGridCell("", ""));
						}

						pDataGrid->m_vectFileInfo.push_back(pFileListInfo);					
						return pDataGrid;
					}
					
					vector<string> vectElems;
					StringUtils::Tokenize(sStatus, ",", vectElems);
					
					for(vector<string>::iterator it = vectElems.begin(); it != vectElems.end(); it++)
					{
						string sElem = *it;
						if(sElem.length() > 0 && sElem[0] == 'S')
						{
							string sSlotIndex = sElem.substr(1, sElem.find('=', 0) - 1);
							FileListInfo *pFileListInfo = new FileListInfo(true, sSlotIndex + ". " + "Movie", false);
							pCell = new DataGridCell("~S2~not identified", "not identified");
							
							if( Actions.length() )
							{
								if( Actions.find('P')!=string::npos )
								{
									// The Orbiter wants us to attach an action to files too
									DCE::CMD_Play_Disk cmd(m_dwPK_Device, pRow_Device->PK_Device_get(), atoi(sSlotIndex.c_str()));
									pCell->m_pMessage = new Message(cmd.m_pMessage);
									delete cmd.m_pMessage;
									cmd.m_pMessage = NULL;
								}
								else if( Actions.find('S')!=string::npos )
								{
									// The Orbiter wants us to attach an action to files too
									DCE::CMD_Show_Object cmd(PK_Controller, PK_Controller,
										StringUtils::itos(DESIGNOBJ_butPreviewFileList_CONST), 0, "", "", "1");
									pCell->m_pMessage = new Message(cmd.m_pMessage);
									delete cmd.m_pMessage;
									cmd.m_pMessage = NULL;
								}						
							}
							
							if( PK_DataGrid==DATAGRID_Media_Browser_CONST )
							{
								pDataGrid->SetData(iColumn, iRow, pCell);
								INCREMENT_ROW_COLUMN
							}
							else
							{
								pDataGrid->SetData(0, iRow, new DataGridCell("", ""));
								pCell->m_Colspan = 6;
								pDataGrid->SetData(1, iRow++, pCell);
							}
							pDataGrid->m_vectFileInfo.push_back(pFileListInfo);							
						}
					}
				}
			}
			
			return pDataGrid;
		}	
	}

	int PK_MediaType=0;
	bool bFoldersOnly = false;
	if( Extensions.length()>3 && Extensions.substr(0,3)=="~MT" )
	{
		PK_MediaType = atoi(Extensions.substr(3).c_str());

		if(PK_MediaType == -1) //folders only
		{
			bFoldersOnly = true;
		}
		else
		{
			Row_MediaType *pRow_MediaType=m_pDatabase_pluto_main->MediaType_get()->GetRow(PK_MediaType);
			if( !pRow_MediaType )
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find Media Type: %d",PK_MediaType);
				return NULL;
			}
			Extensions = pRow_MediaType->Extensions_get();
			// Include stored dvd's and stored cd's
			if( PK_MediaType==MEDIATYPE_pluto_StoredVideo_CONST )
			{
				pRow_MediaType=m_pDatabase_pluto_main->MediaType_get()->GetRow(MEDIATYPE_pluto_DVD_CONST);
				if( pRow_MediaType && pRow_MediaType->Extensions_get().size() )
					Extensions += (Extensions.size() ? "," : "") + pRow_MediaType->Extensions_get();
			}
			else if( PK_MediaType==MEDIATYPE_pluto_StoredAudio_CONST )
			{
				pRow_MediaType=m_pDatabase_pluto_main->MediaType_get()->GetRow(MEDIATYPE_pluto_CD_CONST);
				if( pRow_MediaType && pRow_MediaType->Extensions_get().size() )
					Extensions += (Extensions.size() ? "," : "") + pRow_MediaType->Extensions_get();
			}
		}
	}
	
	string PathsToScan;
	(*iPK_Variable) = VARIABLE_Path_CONST;
	if( sSubDirectory.length() )
	{
		pos=0;
		for(int i=0;i<=iDirNumber;++i)
			PathsToScan = StringUtils::Tokenize(Paths, "\t", pos);
		PathsToScan += "/" + sSubDirectory;
		(*sValue_To_Assign) = PathsToScan;
	}
	else
	{
		PathsToScan = Paths;
		(*sValue_To_Assign) = PathsToScan;
	}

	list<FileDetails *> listFileDetails;
	GetDirContents(listFileDetails,PathsToScan,Extensions);

	if( bSortByDate )
		listFileDetails.sort(FileDateComparer);
	else
		listFileDetails.sort(FileNameComparer);	
			
	for (list<FileDetails *>::iterator i = listFileDetails.begin(); i != listFileDetails.end(); i++)
	{
		FileDetails *pFileDetails = *i;

		if(bFoldersOnly && !pFileDetails->m_bIsDir)
			continue;

		FileListInfo *flInfo;
		flInfo = new FileListInfo(pFileDetails->m_bIsDir,pFileDetails->m_sBaseName + pFileDetails->m_sFileName,false);

		DataGridCell *pCellPicture = new DataGridCell("", pFileDetails->m_sBaseName + pFileDetails->m_sFileName);
        pCell = new DataGridCell((pFileDetails->m_bIsDir ? "~S2~" : "") + pFileDetails->m_sFileName + " " + pFileDetails->m_sDescription, pFileDetails->m_sBaseName + pFileDetails->m_sFileName);

		if (pFileDetails->m_bIsDir && PK_MediaType==MEDIATYPE_pluto_DVD_CONST)
		{
			PlutoSqlResult result_set;
			string sSQL = "SELECT PK_File from `File` where Path='" + StringUtils::SQLEscape(pFileDetails->m_sBaseName) + "' AND Filename='" + StringUtils::SQLEscape(pFileDetails->m_sFileName) + "' and IsDirectory=0";
			if( (result_set.r=m_pMedia_Plugin->m_pDatabase_pluto_media->db_wrapper_query_result(sSQL)) && result_set.r->row_count )
				pFileDetails->m_bIsDir=false;
		}

		if (pFileDetails->m_bIsDir)
		{
			string newParams = Paths + "\n" + Extensions + "\n" + Actions + "\n" + (bSortByDate ? "1" : "0")
				+ "\n" + StringUtils::itos(pFileDetails->m_iDirNumber)+ "\n" + sSubDirectory + pFileDetails->m_sFileName + "/";
			DCE::CMD_NOREP_Populate_Datagrid_DT CMDPDG(PK_Controller, DEVICETEMPLATE_Datagrid_Plugin_CONST, BL_SameHouse,
				"", GridID, DATAGRID_Directory_Listing_CONST, newParams, 0);
			pCell->m_pMessage = new Message(CMDPDG.m_pMessage);
			pCellPicture->m_pMessage = new Message(CMDPDG.m_pMessage);
			delete CMDPDG.m_pMessage;
			CMDPDG.m_pMessage = NULL;
		}
		else if( Actions.length() )
		{
			if( Actions.find('P')!=string::npos )
			{
				// The Orbiter wants us to attach an action to files too
				DCE::CMD_MH_Play_Media_Cat cmd(PK_Controller, DEVICECATEGORY_Media_Plugins_CONST, false, BL_SameHouse,
					0 /* any device */,pFileDetails->m_sBaseName + pFileDetails->m_sFileName,
					0 /* whatever media type the file is */,0 /* any master device */,"" /* current entertain area */,
					false /* resume */, 0 /* repeat */, 0 /* bqueue TODO aaronspecial */, 0 /* bByPass_Event TODO aaronspecial */, 0 /* bDont_Setup_Av TODO aaronspecial */);
				pCell->m_pMessage = new Message(cmd.m_pMessage);
				pCellPicture->m_pMessage = new Message(cmd.m_pMessage);
				delete cmd.m_pMessage;
				cmd.m_pMessage = NULL;
			}
			else if( Actions.find('S')!=string::npos )
			{
				// The Orbiter wants us to attach an action to files too
				DCE::CMD_Show_Object cmd(PK_Controller, PK_Controller, StringUtils::itos(DESIGNOBJ_butPreviewFileList_CONST), 0, "", "", "1");
				pCell->m_pMessage = new Message(cmd.m_pMessage);
				pCellPicture->m_pMessage = new Message(cmd.m_pMessage);
				delete cmd.m_pMessage; 
				cmd.m_pMessage = NULL;
			}
		}
		delete pFileDetails; // We won't need it anymore and it was allocated on the heap

		if( PK_DataGrid==DATAGRID_Media_Browser_CONST )
		{
			pDataGrid->SetData(iColumn, iRow, pCell);
			INCREMENT_ROW_COLUMN
		}
		else
		{
			pCell->m_Colspan = 6;
			pDataGrid->SetData(0, iRow, pCellPicture);
			pDataGrid->SetData(1, iRow++, pCell);
		}
		pDataGrid->m_vectFileInfo.push_back(flInfo);
	}

	return pDataGrid;
}



/*

	COMMANDS TO IMPLEMENT

*/


//<-dceag-sample-b->!


/*
if( PK_Device!=0 )
	{
		DeviceData_Router *pDevice = m_pRouter->m_mapDeviceData_Router_Find(PK_Device);
		if( !pDevice )
			return NULL;

		// Help the controller out since we could only attach 1 message to the datagrid
		// we weren't able to set it's device variable
		if( ptrController )
			ptrController->SetVariable(VARIABLE_PK_DEVICE_CONST,StringUtils::itos(PK_Device).c_str());

		IRInformation *pIRInformation = m_mapIRInformation_Find(pDevice->m_dwPK_Device);
		for(size_t i=0;i<=pDevice->m_iNumberCommands;++i)
		{
			Command *pCommand = m_pRouter->mapCommand_Find(pDevice->m_piCommands[i]);
			if( !pCommand )
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"There's a null in the pDevice->m_mapCommand");
				continue;
			}

			if( pCommand->m_iPK_Command==COMMAND_AV_INPUT_SELECT_CONST && pIRInformation  && pIRInformation ->m_listToggleInputs.size()==0 )
				continue;
			if( pCommand->m_iPK_Command==COMMAND_AV_DSP_MODE_CONST && pIRInformation && pIRInformation->m_listToggleDSPModes.size()==0 )
				continue;
			if( (pCommand->m_iPK_Command==COMMAND_GEN_OFF_CONST || pCommand->m_iPK_Command==COMMAND_GEN_ON_CONST) && pIRInformation && pIRInformation->m_bTogglePower )
				continue;

			int PK_Command = pCommand->m_iPK_Command;
			string Description = pCommand->m_sDescription;

			pCell = new DataGridCell(Description,"");
			pCell->m_Colspan = 7;
			pCell->m_pMessage = new Message(PK_Controller,PK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND, PK_Command, 0);
			pDataGrid->SetData(0,iRow,pCell);

			pCell = new DataGridCell("Learn","");
			pCell->m_Colspan = 2;
			pCell->m_pMessage = new Message(PK_Controller,PK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND, COMMAND_LEARN_IR_CONST, 1, COMMANDPARAMETER_ID_CONST, StringUtils::itos(PK_Command).c_str());
			pDataGrid->SetData(7,iRow,pCell);

			++iRow;
		}

		map<int,int>::iterator itIODSP;
		for(itIODSP=pDevice->m_mapInputs.begin();itIODSP!=pDevice->m_mapInputs.end();++itIODSP)
		{
			Command *pCommand = m_pRouter->m_mapCommand[(*itIODSP).second];
			if( !pCommand )
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"There's a null in the pDevice->m_mapInput for non-existant action: %d (%d)",(*itIODSP).second,(*itIODSP).first);
			else
			{
				if( pIRInformation && pIRInformation->m_listToggleInputs.size()!=0 )
				{
					bool bToggleInput=false;
					list<int>::iterator it;
					for(it=pIRInformation->m_listToggleInputs.begin();it!=pIRInformation->m_listToggleInputs.end();++it)
					{
						if( (*it)==(*itIODSP).first )
						{
							bToggleInput=true;
							break;
						}
					}
					if( bToggleInput )
						continue;
				}
				pCell = new DataGridCell("Input: " + pCommand->m_sDescription,"");
				pCell->m_Colspan = 7;
				pCell->m_pMessage = new Message(PK_Controller,PK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND, pCommand->m_iPK_Command, 0);
				pDataGrid->SetData(0,iRow,pCell);

				pCell = new DataGridCell("Learn","");
				pCell->m_Colspan = 2;
				pCell->m_pMessage = new Message(PK_Controller,PK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND, COMMAND_LEARN_IR_CONST, 1, COMMANDPARAMETER_ID_CONST, StringUtils::itos(pCommand->m_iPK_Command).c_str());
				pDataGrid->SetData(7,iRow,pCell);

				++iRow;
			}
		}

		for(itIODSP=pDevice->m_mapOutputs.begin();itIODSP!=pDevice->m_mapOutputs.end();++itIODSP)
		{
			Command *pCommand = m_pRouter->m_mapCommand[(*itIODSP).second];
			if( !pCommand )
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"There's a null in the pDevice->m_mapOutput for non-existant action: %d (%d)",(*itIODSP).second,(*itIODSP).first);
			else
			{
				pCell = new DataGridCell("Output: " + pCommand->m_sDescription,"");
				pCell->m_Colspan = 8;
				pCell->m_pMessage = new Message(PK_Controller,PK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND, pCommand->m_iPK_Command, 0);
				pDataGrid->SetData(0,iRow,pCell);

				pCell = new DataGridCell("Learn","");
				pCell->m_Colspan = 2;
				pCell->m_pMessage = new Message(PK_Controller,PK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND, COMMAND_LEARN_IR_CONST, 1, COMMANDPARAMETER_ID_CONST, StringUtils::itos(pCommand->m_iPK_Command).c_str());
				pDataGrid->SetData(7,iRow,pCell);

				++iRow;
			}
		}

		for(itIODSP=pDevice->m_mapDSPModes.begin();itIODSP!=pDevice->m_mapDSPModes.end();++itIODSP)
		{
			Command *pCommand = m_pRouter->m_mapCommand[(*itIODSP).second];
			if( !pCommand )
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"There's a null in the pDevice->m_mapDSPMode for non-existant action: %d (%d)",(*itIODSP).second,(*itIODSP).first);
			else
			{
				if( pIRInformation && pIRInformation->m_listToggleDSPModes.size()!=0 )
				{
					list<int>::iterator it;
					for(it=pIRInformation->m_listToggleDSPModes.begin();it!=pIRInformation->m_listToggleDSPModes.end();++it)
					{
						if( (*it)==(*itIODSP).second )
							continue;  // This is a toggle input
					}
				}

				pCell = new DataGridCell("DSPMode: " + pCommand->m_sDescription,"");
				pCell->m_Colspan = 8;
				pCell->m_pMessage = new Message(PK_Controller,PK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND, pCommand->m_iPK_Command, 0);
				pDataGrid->SetData(0,iRow,pCell);

				pCell = new DataGridCell("Learn","");
				pCell->m_Colspan = 2;
				pCell->m_pMessage = new Message(PK_Controller,PK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND, COMMAND_LEARN_IR_CONST, 1, COMMANDPARAMETER_ID_CONST, StringUtils::itos(pCommand->m_iPK_Command).c_str());
				pDataGrid->SetData(7,iRow,pCell);

				++iRow;
			}
		}
		return pDataGrid;
	}

	*/
//<-dceag-createinst-b->!
