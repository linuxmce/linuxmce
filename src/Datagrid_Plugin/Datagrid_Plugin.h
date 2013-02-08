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
/**
 *
 * @file Datagrid_Plugin.h
 * @brief header file for the DataGridGeneratorPlugIn, DataGridGeneratorCallBack classes
 *
 */
 
//<-dceag-d-b->
#ifndef Datagrid_Plugin_h
#define Datagrid_Plugin_h

//	DCE Implemenation for #9 Datagrid Plug-in

#include "Gen_Devices/Datagrid_PluginBase.h"
//<-dceag-d-e->

namespace DCE
{
class Message;

/** This plug-in has it's own plug-in architecture as well to keep track of datagrid generator */
typedef map<string, class DataGridTable *> DataGridMap;
typedef map<int, class DataGridGeneratorCallBack *> DatagridGeneratorCallBackMap;
class DataGridGeneratorPlugIn {
};

/**
 * If a non-null pointer is returned, the framework will remove any existing datagrid by this name and replace it with this one.
 */
 
typedef class DataGridTable *( DataGridGeneratorPlugIn::*DCEDataGridGeneratorFn )( string GridID, string Parms, void *pExtraData
				, int *iPK_Variable, string *sValue_To_Assign, Message *pMessage );

/** Contains a pointer to the datagrid callback */

class DataGridGeneratorCallBack
{
public:
	class DataGridGeneratorPlugIn *m_pDataGridGeneratorPlugIn;
	DCEDataGridGeneratorFn m_pDCEDataGridGeneratorFn;
	bool m_bRePopulateEachTimeRequested; // If true, the grid will be repopulated each time it's requested

	/**
	 * @note There is a reason why the pointer to DataGridGeneratorPlugIn must be passed in as void. If the class being passed in doesn't
	 * derive from DataGridGeneratorPlugIn as the first parent, the "this" pointer gets offset when it's passed in. Then when the callback is
	 * made, it no longer pointer to the right place, and "this" ends up pointing to an incorrect memory table. The solution is to pass
	 * in the plug-ins as void * and then convert back. See the sample app in voidptr.cpp within this project to demonstrate it.
	
	 * DataGridGeneratorCallBack( class DataGridGeneratorPlugIn *pDataGridGeneratorPlugIn, DCEDataGridGeneratorFn pDCEDataGridGeneratorFn )   <-- this won't work
	 */

	/** @brief constructor */
	
	DataGridGeneratorCallBack( void *pDataGridGeneratorPlugIn, DCEDataGridGeneratorFn pDCEDataGridGeneratorFn, bool bRePopulateEachTimeRequested=false )
	{
		m_bRePopulateEachTimeRequested=bRePopulateEachTimeRequested;
		m_pDataGridGeneratorPlugIn=( class DataGridGeneratorPlugIn * ) pDataGridGeneratorPlugIn;
		m_pDCEDataGridGeneratorFn=pDCEDataGridGeneratorFn;
	}
};

//<-dceag-decl-b->!
	
	/**
	 * @brief documenatation
	 * @todo complete documentation
	 */
	 
	class Datagrid_Plugin : public Datagrid_Plugin_Command
	{
//<-dceag-decl-e->
//<-dceag-const-b->
public:
		// Constructors/Destructor
		Datagrid_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Datagrid_Plugin();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

	/** Private member variables */
	
	pluto_pthread_mutex_t m_DataGridMutex;
	DataGridMap m_DataGrids;
	map<int, DatagridGeneratorCallBackMap *> m_mapDataGridGeneratorCallBack;

	/** Private methods */
public:
	/** Public member variables */

	void RegisterDatagridGenerator( class DataGridGeneratorCallBack *pCallBack, int PK_DataGrid, int PK_DeviceTemplate );

	DataGridGeneratorCallBack *GetCallBack(int iPK_DataGrid, int PK_DeviceTemplate)
	{
		DataGridGeneratorCallBack *pCB=NULL;
		map<int, DatagridGeneratorCallBackMap *>::iterator itGridCB = m_mapDataGridGeneratorCallBack.find( iPK_DataGrid );
		if( itGridCB != m_mapDataGridGeneratorCallBack.end() )
		{
			if( itGridCB->second->size()==1 )
				pCB = itGridCB->second->begin()->second;
			else
			{
				DatagridGeneratorCallBackMap *pDatagridGeneratorCallBackMap = itGridCB->second;
				DatagridGeneratorCallBackMap::iterator itCB;
				if( (itCB=pDatagridGeneratorCallBackMap->find(PK_DeviceTemplate))==pDatagridGeneratorCallBackMap->end() )
				{
					if( (itCB=pDatagridGeneratorCallBackMap->find(0))==pDatagridGeneratorCallBackMap->end() )
					{
						LoggerWrapper::GetInstance()->Write(LV_WARNING,"Multiple handlers for datagrid %d and none matching.  Using the first",iPK_DataGrid);
						pCB = pDatagridGeneratorCallBackMap->begin()->second;
					}
					else
						pCB = itCB->second;
				}
				else
					pCB = itCB->second;
			}
		}
		return pCB;
	}

	DataGridTable *DataGridTable_get( string sDataGridID )
	{
		DataGridMap::iterator dg = m_DataGrids.find( sDataGridID );
		if ( dg == m_DataGrids.end() )
			return NULL;
		return dg->second;
	}


//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/


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

	virtual void CMD_Request_Datagrid_Contents(string sID,string sDataGrid_ID,int iRow_count,int iColumn_count,bool bKeep_Row_Header,bool bKeep_Column_Header,bool bAdd_UpDown_Arrows,string sSeek,int iOffset,char **pData,int *iData_Size,int *iRow,int *iColumn) { string sCMD_Result; CMD_Request_Datagrid_Contents(sID.c_str(),sDataGrid_ID.c_str(),iRow_count,iColumn_count,bKeep_Row_Header,bKeep_Column_Header,bAdd_UpDown_Arrows,sSeek.c_str(),iOffset,pData,iData_Size,iRow,iColumn,sCMD_Result,NULL);};
	virtual void CMD_Request_Datagrid_Contents(string sID,string sDataGrid_ID,int iRow_count,int iColumn_count,bool bKeep_Row_Header,bool bKeep_Column_Header,bool bAdd_UpDown_Arrows,string sSeek,int iOffset,char **pData,int *iData_Size,int *iRow,int *iColumn,string &sCMD_Result,Message *pMessage);


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

	virtual void CMD_Populate_Datagrid(string sID,string sDataGrid_ID,int iPK_DataGrid,string sOptions,int iPK_DeviceTemplate,int *iPK_Variable,string *sValue_To_Assign,bool *bIsSuccessful,int *iWidth,int *iHeight) { string sCMD_Result; CMD_Populate_Datagrid(sID.c_str(),sDataGrid_ID.c_str(),iPK_DataGrid,sOptions.c_str(),iPK_DeviceTemplate,iPK_Variable,sValue_To_Assign,bIsSuccessful,iWidth,iHeight,sCMD_Result,NULL);};
	virtual void CMD_Populate_Datagrid(string sID,string sDataGrid_ID,int iPK_DataGrid,string sOptions,int iPK_DeviceTemplate,int *iPK_Variable,string *sValue_To_Assign,bool *bIsSuccessful,int *iWidth,int *iHeight,string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->
};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!
