/*
 Datagrid_Plugin
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to 
 the terms of the Pluto Public License, available at: 
 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, 
 but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. 
 
 See the Pluto Public License for more details.
 
 */

//<-dceag-d-b->
#include "Datagrid_Plugin.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "DCE/DataGrid.h"

//<-dceag-const-b->! custom
Datagrid_Plugin::Datagrid_Plugin( int DeviceID, string ServerAddress, bool bConnectEventHandler, bool bLocalMode, class Router *pRouter )
 : Datagrid_Plugin_Command( DeviceID, ServerAddress, bConnectEventHandler, bLocalMode, pRouter ), 
 m_DataGridMutex( "DataGrid" )
//<-dceag-const-e->
{
 m_DataGridMutex.Init( NULL );
}

//<-dceag-getconfig-b->
bool Datagrid_Plugin::GetConfig()
{
	if( !Datagrid_Plugin_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->
	return true;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Datagrid_Plugin::~Datagrid_Plugin()
//<-dceag-dest-e->
{
	{
		PLUTO_SAFETY_LOCK( s, m_DataGridMutex );
		for(map<int, DatagridGeneratorCallBackMap *>::iterator it = m_mapDataGridGeneratorCallBack.begin();it != m_mapDataGridGeneratorCallBack.end();++it)
		{
			for(DatagridGeneratorCallBackMap::iterator it2 = it->second->begin(); it2 != it->second->end();++it2)
				delete it2->second;
			delete it->second;
		}
		m_mapDataGridGeneratorCallBack.clear();

		for(map<string, class DataGridTable *>::iterator itd = m_DataGrids.begin(); itd != m_DataGrids.end(); ++itd)
			delete itd->second;
		m_DataGrids.clear();
	}

	pthread_mutex_destroy(&m_DataGridMutex.mutex);
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Datagrid_Plugin::Register()
//<-dceag-reg-e->
{
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
void Datagrid_Plugin::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void Datagrid_Plugin::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

void Datagrid_Plugin::RegisterDatagridGenerator( class DataGridGeneratorCallBack *pCallBack, int PK_DataGrid, int PK_DeviceTemplate )
{
	map<int, DatagridGeneratorCallBackMap *>::iterator it = m_mapDataGridGeneratorCallBack.find( PK_DataGrid );
	if( it==m_mapDataGridGeneratorCallBack.end() )
	{
		DatagridGeneratorCallBackMap *pDatagridGeneratorCallBackMap = new DatagridGeneratorCallBackMap();
		(*pDatagridGeneratorCallBackMap)[PK_DeviceTemplate] = pCallBack;
		m_mapDataGridGeneratorCallBack[PK_DataGrid] = pDatagridGeneratorCallBackMap;
		return;
	}
	DatagridGeneratorCallBackMap *pDatagridGeneratorCallBackMap = it->second;
	(*pDatagridGeneratorCallBackMap)[PK_DeviceTemplate] = pCallBack;
}


/*

 COMMANDS TO IMPLEMENT

*/

//<-dceag-c34-b->

	/** @brief COMMAND: #34 - Request Datagrid Contents */
	/** The orbiters use this to request the contents of a datagrid, so they can display it for the user.  Before you can request a grid, the command Populate Datagrid must be called, passing in the information required to populate the grid as well a "GridID", */
		/** @param #10 ID */
			/** For debugging purposes if problems arise with a request not being filled, or a grid not populated when it should be.  If the Orbiter specified an ID when requesting the grid or populating it, the Datagrid plug-in will log the ID and status so the develope */
		/** @param #15 DataGrid ID */
			/** A unique identifier for the grid that was passed to populate grid. */
		/** @param #19 Data */
			/** The binary contents of the grid. */
		/** @param #32 Row */
			/** The starting row.   This is both an 'in' and 'out'.  If sSeek is not empty, the grid will ignore the value passed in here, and seek to that row instead.  It will also return the value of the row in this variable so the requestor will know the current row. */
		/** @param #33 Column */
			/** The starting column.  This is both an in and an out so the datagrid can forcibly move the current row/column on the orbiter. */
		/** @param #34 Row count */
			/** How many rows to fetch */
		/** @param #35 Column count */
			/** How many columns to fetch. */
		/** @param #36 Keep Row Header */
			/** True means the first row (0) is locked and will always be at the top as a header. */
		/** @param #37 Keep Column Header */
			/** True means the first column(0) is locked and will always be at the top as a header. */
		/** @param #49 Add Up-Down Arrows */
			/** If to add up/down arrows to the returned datagrid when datagrid doesn't fully fit. */
		/** @param #73 Seek */
			/** If a value is specified, the grid will seek to the first row matching this value.  The search is case insensitive.  The value of "Row" will be ignored, and the new top row will be returned as the out value of "Row".  If offset is specified, the comparisso */
		/** @param #74 Offset */
			/** See the "Seek" parameter for an explanation. */

void Datagrid_Plugin::CMD_Request_Datagrid_Contents(string sID,string sDataGrid_ID,int iRow_count,int iColumn_count,bool bKeep_Row_Header,bool bKeep_Column_Header,bool bAdd_UpDown_Arrows,string sSeek,int iOffset,char **pData,int *iData_Size,int *iRow,int *iColumn,string &sCMD_Result,Message *pMessage)
//<-dceag-c34-e->
{
	*iData_Size=0;
	*pData=NULL;

#ifdef DEBUG
	clock_t cStart=clock(); 
	LoggerWrapper::GetInstance()->Write( LV_DATAGRID, "Requesting grid: %s", sDataGrid_ID.c_str() );
#endif
	PLUTO_SAFETY_LOCK( s, m_DataGridMutex );

	DataGridTable *pDataGridTable=NULL;
	DataGridMap::iterator dg = m_DataGrids.find( sDataGrid_ID );
	if ( dg == m_DataGrids.end() )
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "Requesting data from unknown grid %s", sDataGrid_ID.c_str() );
		return;
	}

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write( LV_DATAGRID, "Found grid: %s", sDataGrid_ID.c_str() );
#endif

		pDataGridTable = ( *dg ).second;

		if( pDataGridTable->m_bRePopulateEachTimeRequested )
		{
			int iPK_Variable,Width,Height;
			string sValue_To_Assign;
			bool bIsSuccessful;
			s.Release();
			CMD_Populate_Datagrid(sID,sDataGrid_ID,pDataGridTable->m_iPK_Datagrid,pDataGridTable->m_sOptions,0,  // DEviceTemplate 0 -- TODO, what if it's needed
				&iPK_Variable,&sValue_To_Assign,&bIsSuccessful,&Width,&Height);
			s.Relock();
			dg = m_DataGrids.find( sDataGrid_ID );
			if ( dg == m_DataGrids.end() )
			{
				LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Requesting twice data from unknown grid %s", sDataGrid_ID.c_str() );
				return;
			}
			pDataGridTable = ( *dg ).second;
		}

		bool bValue = false;
		if( sSeek.length() && sSeek[0]=='~' )
		{
			bValue=true;
			sSeek = sSeek.substr(1);
		}
		if( sSeek.length() )
		{
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write( LV_DATAGRID, "it has a seek value grid: %s rows: %d seek %s offset: %d bValue: %d", 
				  sDataGrid_ID.c_str(),(int) pDataGridTable->GetRows(),sSeek.c_str(), iOffset, (int) bValue );
#endif
			sSeek = StringUtils::ToUpper(sSeek);
			// We need to do a seek for this value
			int dgrow;

			bool bFound=false;
			// Scroll through here until we find the row right after
			for(dgrow=0;dgrow<pDataGridTable->GetRows();++dgrow)
			{
				DataGridCell *pCell = pDataGridTable->GetData(iOffset,dgrow);
				if( !pCell )
				{
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot seek because there's an empty cell");
					break;
				}
				string CellText = bValue ? StringUtils::ToUpper(pCell->m_Value ? pCell->m_Value : "") : StringUtils::ToUpper(pCell->m_Text ? pCell->m_Text : "");
				if( CellText.length()==0 )
				{
					LoggerWrapper::GetInstance()->Write(LV_WARNING,"Request grid, we're seeking on a column that has empty cells ioffset: %d seek: %s id: %s",iOffset,sSeek.c_str(),sDataGrid_ID.c_str());
					continue;
				}

				while(CellText.size() && CellText[0]=='~')
				{
					string::size_type nexttilde = CellText.find('~',1);
					if( nexttilde!=string::npos )
						CellText = CellText.substr(nexttilde+1);
					else
						CellText = CellText.substr(1);

					LoggerWrapper::GetInstance()->Write(LV_STATUS,"Stripping leading ~ pos %d now %s",
						nexttilde,CellText.c_str());
				};

				while(CellText.size() && CellText[0]=='`')
				{
					string::size_type nexttilde = CellText.find('`',1);
					if( nexttilde!=string::npos )
						CellText = CellText.substr(nexttilde+1);
					else
						CellText = CellText.substr(1);

					LoggerWrapper::GetInstance()->Write(LV_STATUS,"Stripping leading ` pos %d now %s",
						nexttilde,CellText.c_str());
				};

				// If we're looking for a specific ID, we need to match it.  Otherwise we're looking for 
				// a value and will assume anything past the value is okay.
				if( bValue==false && CellText>sSeek )
				{
					bFound=true;
					break;
				}
				if( bValue==true && CellText==sSeek )
				{
					bFound=true;
					break;
				}
			}

			// If we didn't find anything, and we're seeking for a particular value, don't set iRow, just stay at the top
			if( bFound || bValue==false )
			{
				*iRow=dgrow;

				// Since we now cache pages of the grid in advance, we don't want to jump directly to the given row
				// since that means the page breaks won't line up and any existing cached pages won't be valid.  So
				// Seek to the starting row of the given page
				int iPage = *iRow / iRow_count;
				*iRow = iPage * iRow_count;
			}
#ifdef DEBUG
LoggerWrapper::GetInstance()->Write( LV_DATAGRID, "Seek row %d",*iRow);
#endif
		}
		pDataGridTable->m_bKeepColumnHeader = bKeep_Column_Header;
		pDataGridTable->m_bKeepRowHeader = bKeep_Row_Header;
#ifdef DEBUG
LoggerWrapper::GetInstance()->Write( LV_DATAGRID, "ready to call todata: %s ", sDataGrid_ID.c_str() );
#endif

		pDataGridTable->ToData( sDataGrid_ID, *iData_Size, *pData, iColumn, iRow, iColumn_count, iRow_count );
#ifdef DEBUG
		clock_t cStop = clock();
		LoggerWrapper::GetInstance()->Write( LV_DATAGRID, "Sending datagrid %s, size: %d, cols: %d, rows: %d %d ms", 
			sDataGrid_ID.c_str(), *iData_Size, iColumn_count, iRow_count, (int) (cStop-cStart) );
#endif
}

//<-dceag-c35-b->

	/** @brief COMMAND: #35 - Populate Datagrid */
	/** Before an orbiter can request the contents of a datagrid, the datagrid plug-in must first populate it.  This is how the orbiter tells the plug-in to create and populate a datagrid, referenced by the unique ID passed in with this command. */
		/** @param #4 PK_Variable */
			/** The populate grid can optionally return a variable number to assign a value into.  For example, the current path in the file grid. */
		/** @param #5 Value To Assign */
			/** The value to assign into the variable. */
		/** @param #10 ID */
			/** For debugging purposes if problems arise with a request not being filled, or a grid not populated when it should be.  If the Orbiter specified an ID when requesting the grid or populating it, the Datagrid plug-in will log the ID and status so the develope */
		/** @param #15 DataGrid ID */
			/** A unique ID for this instance of the grid that will be passed with the Request Datagrid Contents command. */
		/** @param #38 PK_DataGrid */
			/** Which grid should be populated */
		/** @param #39 Options */
			/** The options are specific the type of grid (PK_Datagrid).  These are not pre-defined.  The grid generator and orbiter must both pass the options in the correct format for the type of grid. */
		/** @param #40 IsSuccessful */
			/** Returns false if the grid could not be populated.  Perhaps there was no registered datagrid generator. */
		/** @param #44 PK_DeviceTemplate */
			/** If more than 1 plugin registered to handle this grid, this parameter will be used to match teh right one */
		/** @param #60 Width */
			/** The width of the grid, in columns, if the width is determined at populate time, such as a file grid.  If the whole size of the grid is unknown, such as the EPG grid, this should be 0. */
		/** @param #61 Height */
			/** The height of the grid, in rows, if the heightis determined at populate time, such as a file grid.  If the whole size of the grid is unknown, such as the EPG grid, this should be 0. */

void Datagrid_Plugin::CMD_Populate_Datagrid(string sID,string sDataGrid_ID,int iPK_DataGrid,string sOptions,int iPK_DeviceTemplate,int *iPK_Variable,string *sValue_To_Assign,bool *bIsSuccessful,int *iWidth,int *iHeight,string &sCMD_Result,Message *pMessage)
//<-dceag-c35-e->
{
	*bIsSuccessful=false; // assume we're not succesful
	PLUTO_SAFETY_LOCK( s, m_DataGridMutex );

#ifdef DEBUG
	clock_t cStart=clock(); // move this to within #debug
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "About to populate grid: %d %s", iPK_DataGrid, sDataGrid_ID.c_str() );
#endif

	DataGridGeneratorCallBack *pCB = GetCallBack(iPK_DataGrid,iPK_DeviceTemplate);
	DataGridTable *pDataGridTable=NULL;

	if( !pCB )
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"Datagrid_Plugin::CMD_Populate_Datagrid map is empty??");
	else
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "About to call member function: %d %s", iPK_DataGrid, sDataGrid_ID.c_str() );
#endif
		pDataGridTable = CALL_MEMBER_FN( *pCB->m_pDataGridGeneratorPlugIn, pCB->m_pDCEDataGridGeneratorFn ) ( sDataGrid_ID, sOptions, NULL, iPK_Variable, sValue_To_Assign, pMessage );
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Called datagrid populate function for grid: %d %s", iPK_DataGrid, sDataGrid_ID.c_str() );
#endif

		if( pCB->m_bRePopulateEachTimeRequested && pDataGridTable )
		{
			pDataGridTable->m_iPK_Datagrid=iPK_DataGrid;
			pDataGridTable->m_sOptions=sOptions;
			pDataGridTable->m_bRePopulateEachTimeRequested=true;
		}
	}

	if( !pDataGridTable )
	{
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Cannot populate grid: %s", sDataGrid_ID.c_str() );
		pDataGridTable = new DataGridTable();
		DataGridCell *pCell;
		pCell = new DataGridCell( "Datagrid: " + sDataGrid_ID, "" );
		pDataGridTable->SetData( 0, 0, pCell );
		pCell = new DataGridCell( "Type: " + StringUtils::itos( iPK_DataGrid ), "" );
		pDataGridTable->SetData( 0, 1, pCell );
		pCell = new DataGridCell( "is invalid", "" );
		pDataGridTable->SetData( 0, 2, pCell );
		*bIsSuccessful=false; // it didn't work
	}
	else
		*bIsSuccessful=true; // it worked

	*iWidth=pDataGridTable->GetCols();
	*iHeight=pDataGridTable->GetRows();
	DataGridMap::iterator it=m_DataGrids.find(sDataGrid_ID);
	if( it!=m_DataGrids.end() )
		delete it->second;
	m_DataGrids[sDataGrid_ID]=pDataGridTable;
#ifdef DEBUG
	clock_t cStop = clock();
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Datagrid_Plugin::CMD_Populate_Datagrid Returning from populate grid successful? %s %d ms size %d", (*bIsSuccessful ? "Y" : "N"), (int) (cStop-cStart), (int) pDataGridTable->m_MemoryDataTable.size() );
#endif
	return;
}


//<-dceag-sample-b->!

//<-dceag-createinst-b->!
