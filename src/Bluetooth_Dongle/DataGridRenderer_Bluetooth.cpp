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
#include "DataGridRenderer_Bluetooth.h"
#include "Bluetooth_Dongle.h"

#include "../DCE/Logger.h"
#include "../Orbiter/Orbiter.h"
#include "../Orbiter/OrbiterRenderer.h"
#include "../Orbiter/DesignObj_DataGrid.h"
#include "../DCE/DataGrid.h"

#include "../BD/BDCommandProcessor.h"
#include "../VIPShared/BD_CP_ShowImage.h"
#include "../VIPShared/BD_CP_ShowList.h"
#include "../VIPShared/BD_CP_CaptureKeyboard.h"
#include "../VIPShared/BD_CP_SimulateEvent.h"
#include "../VIPShared/PlutoPhoneCommands.h"
#include "../Bluetooth_Dongle/OrbiterBluetooth.h"

DataGridRenderer_Bluetooth::DataGridRenderer_Bluetooth(DesignObj_Orbiter *pOwner) : DataGridRenderer(pOwner) 
{
}

DataGridRenderer_Bluetooth::~DataGridRenderer_Bluetooth(void)
{
}

void DataGridRenderer_Bluetooth::RenderObject(DesignObj_Orbiter *pObj_Screen, PlutoPoint point)
{
	OrbiterBluetooth *pOrbiterBluetooth = dynamic_cast<OrbiterBluetooth *>(m_pObj_Owner->m_pOrbiter);
	if(NULL == pOrbiterBluetooth || NULL == m_pObj_Owner_DataGrid)
		return;

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Extraoptions in grid: %s", m_pObj_Owner_DataGrid->m_sExtraInfo.c_str());
#endif

#if (defined(PROFILING))
	clock_t clkStart = clock();
#endif

	//sending datagrid off command
	list<string> listGridClean;
	listGridClean.clear();
	BD_CP_ShowList *pBD_CP_ShowList_Off = new BD_CP_ShowList(0, 0, 0, 0, 0, listGridClean, false, false);
	if( pOrbiterBluetooth->m_pBDCommandProcessor )
		pOrbiterBluetooth->m_pBDCommandProcessor->AddCommand(pBD_CP_ShowList_Off);

	int iSelectedColumn = m_pObj_Owner_DataGrid->m_iInitialColNum;
	bool bUsePhoneGrid = false;

	//if 'c' - column  extraoption is specified, we'll send to phone the specified column
	size_t sPos;
	if((sPos = m_pObj_Owner_DataGrid->m_sExtraInfo.find( 'c' )) != string::npos)
	{
		bUsePhoneGrid=true;
		if(sPos + 1 < m_pObj_Owner_DataGrid->m_sExtraInfo.size())
			iSelectedColumn = m_pObj_Owner_DataGrid->m_sExtraInfo[sPos + 1] - '0';
	}

	//if 'F' option is specified, we'll let the base the render the grid
	if(m_pObj_Owner_DataGrid->m_sExtraInfo.find( 'F' ) != string::npos) 
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "OrbiterBluetooth: I won't render this grid on the phone");
#endif
		DataGridRenderer::RenderObject(pObj_Screen);
		return;
	}

	m_pObj_Owner_DataGrid->m_MaxRow = 0; //get all rows

	string Unused;
	m_pObj_Owner_DataGrid->PrepareRenderDataGrid(Unused);

	DataGridTable *pDataGridTable = m_pObj_Owner_DataGrid->DataGridTable_Get();
	if(pDataGridTable)
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Got to render a datagrid with %d columns", pDataGridTable->m_ColumnCount);
#endif
		if(pDataGridTable->m_ColumnCount == 1)//we can render on column datagrid
			bUsePhoneGrid = true;

		if(!bUsePhoneGrid)
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "OrbiterBluetooth: I won't render this grid on the phone");
			m_pObj_Owner_DataGrid->RenderObject(pObj_Screen);
			return;
		}

		m_pObj_Owner_DataGrid->m_MaxRow = pDataGridTable->getTotalRowCount();

		int x       = m_pObj_Owner_DataGrid->m_rPosition.X;
		int y       = m_pObj_Owner_DataGrid->m_rPosition.Y;
		int Width   = m_pObj_Owner_DataGrid->m_rPosition.Width;
		int Height  = m_pObj_Owner_DataGrid->m_rPosition.Height;
		list<string> listGrid;
		bool bSendSelectedOnMove = false; 

		//if 'T' extraoptions is specified, then when user presses up/down buttons, PlutoMO will send a SelectedItem command
		if(m_pObj_Owner_DataGrid->m_sExtraInfo.find( 'T' ) != string::npos)
			bSendSelectedOnMove = true;

		bool bTurnOn = true;

		string sCurrentSelected = pOrbiterBluetooth->m_mapVariable[atoi(m_pObj_Owner_DataGrid->sSelVariable.c_str())];
		int iHighlightedRow = m_pObj_Owner_DataGrid->m_iHighlightedRow >= 0 ? m_pObj_Owner_DataGrid->m_iHighlightedRow : atoi(sCurrentSelected.c_str());

#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "About to send BD_CP_ShowList command, column %d, turnon %d, items count %d, selected item %d, send 'selected item' %d",
			iSelectedColumn, bTurnOn, pDataGridTable->getTotalRowCount(), iHighlightedRow, (int)bSendSelectedOnMove);
#endif

		for(int i = 0; i < pDataGridTable->getTotalRowCount(); i++)
		{
			DataGridCell * pCell = pDataGridTable->GetData(iSelectedColumn, i);
			string sItem = pCell != NULL ? pCell->GetText() : "<empty>";
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Item %d : '%s'. Ignoring esc seq for now...", i, sItem.c_str());
#endif

			//ignoring esc seq
			size_t nPos = sItem.find("~S");
			if(nPos != string::npos)
			{
				size_t nPosNext = sItem.find("~", nPos + 1);
				if(nPosNext != string::npos)
					sItem = sItem.substr(nPosNext + 1);
			}            

#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Item %d : '%s'.", i, sItem.c_str());
#endif

			listGrid.push_back(sItem);
		}

		BD_CP_ShowList *pBD_CP_ShowList = new BD_CP_ShowList(x, y, Width, Height, iHighlightedRow, listGrid, 
			bSendSelectedOnMove, bTurnOn);

		if( pOrbiterBluetooth->m_pBDCommandProcessor )
		{
			pOrbiterBluetooth->m_pBDCommandProcessor->AddCommand(pBD_CP_ShowList);
			pOrbiterBluetooth->m_bShowListSent = true;
		}

		listGrid.clear();
	}

	return;
}
