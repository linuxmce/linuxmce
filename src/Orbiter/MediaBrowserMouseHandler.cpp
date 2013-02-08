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
#include "MediaBrowserMouseHandler.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/ProcessUtils.h"
#include "PlutoUtils/Other.h"
#include "MouseGovernor.h"
#include "MouseIterator.h"
#include "OrbiterRenderer.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "pluto_main/Define_Button.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_Screen.h"
#include "pluto_main/Define_Text.h"
#include "DataGrid.h"
#include "ScreenHistory.h"
#include "DataGridRenderer.h"

using namespace DCE;

MediaBrowserMouseHandler::MediaBrowserMouseHandler(DesignObj_Orbiter *pObj,string sOptions,MouseBehavior *pMouseBehavior)
	: MouseHandler(pObj,sOptions,pMouseBehavior)
{
	m_pObj_ListGrid=m_pObj_PicGrid=NULL;
	m_pObj_Sort=m_pObj_More=m_pObj_CoverArtPopup=NULL;

	if( !m_pObj )
		return; // Shouldn't happen

	DesignObj_DataList::iterator iHao;

	for( iHao=pObj->m_ChildObjects.begin(  ); iHao != pObj->m_ChildObjects.end(  ); ++iHao )
	{
		DesignObj_Orbiter *pDesignObj_Orbiter=( DesignObj_Orbiter * )*iHao;
		if( pDesignObj_Orbiter->m_iBaseObjectID==DESIGNOBJ_dgFileList2_CONST )
			m_pObj_ListGrid = (DesignObj_DataGrid *) pDesignObj_Orbiter;
		else if( pDesignObj_Orbiter->m_iBaseObjectID==DESIGNOBJ_dgFileList2_Pics_CONST )
			m_pObj_PicGrid = (DesignObj_DataGrid *) pDesignObj_Orbiter;
		else if( pDesignObj_Orbiter->m_iBaseObjectID==DESIGNOBJ_butFBSF_Show_Sort_CONST )
			m_pObj_Sort = pDesignObj_Orbiter;
		else if( pDesignObj_Orbiter->m_iBaseObjectID==DESIGNOBJ_butFBSF_Show_More_CONST )
			m_pObj_More = pDesignObj_Orbiter;
	}
	if( !m_pObj_ListGrid || !m_pObj_PicGrid || !m_pObj_Sort || !m_pObj_More )
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MediaBrowserMouseHandler::MediaBrowserMouseHandler");
	else
	{
		m_pObj_ListGrid->m_iHighlightedRow=0;
		m_pObj_PicGrid->m_iHighlightedRow=m_pObj_PicGrid->m_iHighlightedColumn=0;
		m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted_set(m_pObj_ListGrid);
		m_pMouseBehavior->m_pOrbiter->Renderer()->DoHighlightObject();
		m_rSortFilterMenu = PlutoRectangle::Union(m_pObj_Sort->m_rPosition, m_pObj_More->m_rPosition );
	}
	m_pObj_CoverArtPopup=m_pMouseBehavior->m_pOrbiter->FindObject(DESIGNOBJ_popCoverArt_CONST);

	m_pDatagridMouseHandlerHelper = new DatagridMouseHandlerHelper(this,m_pObj_ListGrid->m_rPosition.Left(),m_pObj_ListGrid->m_rPosition.Right());
	m_pHorizMenuMouseHandler = new HorizMenuMouseHandler(pObj,sOptions,pMouseBehavior,true);
}

MediaBrowserMouseHandler::~MediaBrowserMouseHandler()
{
	delete m_pDatagridMouseHandlerHelper;
	delete m_pHorizMenuMouseHandler;
}

void MediaBrowserMouseHandler::Start()
{
#ifdef DEBUG
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "MediaBrowserMouseHandler::Start()");
#endif
	m_pMouseBehavior->SetMouseCursorStyle(MouseBehavior::mcs_AnyDirection);
	m_LastRow=-1;
	m_pDatagridMouseHandlerHelper->Start(m_pObj_ListGrid,5,1,m_pMouseBehavior->m_pOrbiter->m_iImageHeight-1);
}

void MediaBrowserMouseHandler::Stop()
{
#ifdef DEBUG
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "MediaBrowserMouseHandler::Stop()");
#endif
	m_pDatagridMouseHandlerHelper->Stop();
}

bool MediaBrowserMouseHandler::ButtonDown(int PK_Button)
{
	if( m_pMouseBehavior->m_pOrbiter->m_pScreenHistory_Current->GetObj()->m_iBaseObjectID!=DESIGNOBJ_popFileList_CONST )
		return false; // Keep processing
	if( m_pDatagridMouseHandlerHelper->CapturingMouse() )
	{
		if( PK_Button==BUTTON_Mouse_2_CONST )
		{
			m_pDatagridMouseHandlerHelper->ReleaseRelative();
			return true;
		}
	}

	return false; // Keep processing
}

bool MediaBrowserMouseHandler::ButtonUp(int PK_Button)
{
	if( m_pMouseBehavior->m_pOrbiter->m_pScreenHistory_Current->GetObj()->m_iBaseObjectID!=DESIGNOBJ_popFileList_CONST )
		return false; // Keep processing

	if( PK_Button==BUTTON_Mouse_7_CONST && m_bTapAndRelease==false && m_bStartedMovement )
	{
//		return false;
	}

	// If we're moving left and right to the filter options
	if( PK_Button==BUTTON_Left_Arrow_CONST || PK_Button==BUTTON_Right_Arrow_CONST || m_pHorizMenuMouseHandler->m_pObj_ActiveMenuPad )
		return m_pHorizMenuMouseHandler->ButtonUp(PK_Button);

	return false; // Keep processing
}


void MediaBrowserMouseHandler::Move(int X,int Y,int PK_Direction)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "MediaBrowserMouseHandler::Move(%d, %d, %d)", X, Y, PK_Direction);
#endif

	if( m_pMouseBehavior->m_pOrbiter->m_pScreenHistory_Current->GetObj()->m_iBaseObjectID!=DESIGNOBJ_popFileList_CONST )
		return;
	if( !m_pObj_ListGrid || !m_pObj_PicGrid )
		return;  // Shouldn't happen

    if (m_pDatagridMouseHandlerHelper->Move(X,Y,PK_Direction))
		return;

	PLUTO_SAFETY_LOCK( cm, m_pMouseBehavior->m_pOrbiter->m_ScreenMutex );  // Protect the highlighed object
	DesignObj_DataGrid *pObj_ListGrid_Active=NULL;
	if( m_rSortFilterMenu.Contains(X,Y) || m_pHorizMenuMouseHandler->m_pObj_ActiveMenuPad )
	{
		m_pHorizMenuMouseHandler->Move(X,Y,PK_Direction);
		return;
	}
	else if( m_pObj_ListGrid->m_rPosition.Contains(X,Y) )
		pObj_ListGrid_Active = m_pObj_ListGrid;
	else if( m_pObj_PicGrid->m_rPosition.Contains(X,Y) )
		pObj_ListGrid_Active = m_pObj_PicGrid;
	else
	{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "MediaBrowserMouseHandler::Move(%d, %d, %d) in sort filter or menu pad %p", X, Y, PK_Direction,m_pHorizMenuMouseHandler->m_pObj_ActiveMenuPad);
#endif
		return;
	}

	PlutoRectangle rect;
	m_pMouseBehavior->m_pOrbiter->GetDataGridHighlightCellCoordinates(pObj_ListGrid_Active,rect);
	if( pObj_ListGrid_Active->m_iHighlightedColumn!=-1 && pObj_ListGrid_Active->m_iHighlightedRow!=-1 && rect.Contains(X,Y) )
	{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "MediaBrowserMouseHandler::Move(%d, %d, %d) use hasn't left cell", X, Y, PK_Direction);
#endif
		return; // Nothing to do.  User is still on same cell
	}

	// A very lean location of the highlighted row/cell
	// This worked before, but now that we're using pre-rendered cells which are placed in OrbiterGen, there's a rounding
	// issue becaues pObj_ListGrid_Active->m_FixedRowHeight doesn't have the fractional amount and can get off
	// int Row = (Y-pObj_ListGrid_Active->m_rPosition.Y)/(pObj_ListGrid_Active->m_FixedRowHeight+1); // +1 because there's a blank pixel under each one
	// So use this method when there's a DesignObj for the cell
	int Row = int((Y-pObj_ListGrid_Active->m_rPosition.Y)/((float) pObj_ListGrid_Active->m_rPosition.Height / pObj_ListGrid_Active->m_MaxRow));

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "MediaBrowserMouseHandler::Move(%d, %d, %d) row %d active %p=%p", X, Y, PK_Direction,Row,pObj_ListGrid_Active,m_pObj_PicGrid);
#endif
	if( pObj_ListGrid_Active==m_pObj_PicGrid )
	{
		int Col = (X-m_pObj_PicGrid->m_rPosition.X)/m_pObj_PicGrid->m_FixedColumnWidth;
		m_pObj_PicGrid->m_iHighlightedColumn=Col;
		m_pObj_PicGrid->m_iHighlightedRow=Row;
		m_pObj_ListGrid->m_iHighlightedRow = Row * m_pObj_PicGrid->m_MaxCol + Col;
	}
	else
	{
		m_pObj_ListGrid->m_iHighlightedRow = Row;
		Row = Row / m_pObj_PicGrid->m_MaxCol;
		int Col = m_pObj_ListGrid->m_iHighlightedRow - (Row * m_pObj_PicGrid->m_MaxCol);
		m_pObj_PicGrid->m_iHighlightedColumn=Col;
		m_pObj_PicGrid->m_iHighlightedRow=Row;
	}

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "MediaBrowserMouseHandler::Move(%d, %d, %d) list grid row now %d", X, Y, PK_Direction,m_pObj_ListGrid->m_iHighlightedRow);
#endif
	if( m_pObj_ListGrid->m_iHighlightedRow!=m_LastRow )
	{
		m_LastRow=m_pObj_ListGrid->m_iHighlightedRow;
		// This takes care of the list grid
		NeedToRender render( m_pMouseBehavior->m_pOrbiter, "Move on grid" );
		ShowCoverArtPopup();

		DataGridCell *pCell = NULL;
		DataGridTable *pDataGridTable = m_pObj_ListGrid->DataGridTable_Get();
		if( pDataGridTable )
		{
			pCell = pDataGridTable->GetData(0,m_pObj_ListGrid->m_iHighlightedRow + m_pObj_ListGrid->m_GridCurRow );
			if( pCell && pCell->m_Text )
				m_pMouseBehavior->m_pOrbiter->CMD_Set_Text( TOSTRING(DESIGNOBJ_objLongFileName_CONST), pCell->m_Text, TEXT_STATUS_CONST );
		}

		m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted_set(m_pObj_ListGrid);
		m_pMouseBehavior->m_pOrbiter->Renderer()->DoHighlightObject();
	}
}

void MediaBrowserMouseHandler::ShowCoverArtPopup()
{
	if( !m_pObj_CoverArtPopup )
		return; // Shouldn't happen

	int X=0,Y=0;
	if( m_pObj_PicGrid->m_iHighlightedColumn )
		X = m_pObj_PicGrid->m_rPosition.X + (m_pObj_PicGrid->m_iHighlightedColumn*m_pObj_PicGrid->m_FixedColumnWidth) - ((m_pObj_CoverArtPopup->m_rPosition.Width - m_pObj_PicGrid->m_FixedColumnWidth)/2);
	if( m_pObj_PicGrid->m_iHighlightedRow )
		Y = m_pObj_PicGrid->m_rPosition.Y + (m_pObj_PicGrid->m_iHighlightedRow*m_pObj_PicGrid->m_FixedRowHeight) - ((m_pObj_CoverArtPopup->m_rPosition.Height - m_pObj_PicGrid->m_FirstRowHeight)/2);

	if( X + m_pObj_CoverArtPopup->m_rPosition.Width > m_pObj_PicGrid->m_rPosition.Right() )
		X = m_pObj_PicGrid->m_rPosition.Right() - m_pObj_CoverArtPopup->m_rPosition.Width;
	else if( X < m_pObj_PicGrid->m_rPosition.X )
		X = m_pObj_PicGrid->m_rPosition.X;
	if( Y + m_pObj_CoverArtPopup->m_rPosition.Height > m_pObj_PicGrid->m_rPosition.Bottom() )
		Y = m_pObj_PicGrid->m_rPosition.Bottom() - m_pObj_CoverArtPopup->m_rPosition.Height;
	else if( Y < m_pObj_PicGrid->m_rPosition.Y )
		Y = m_pObj_PicGrid->m_rPosition.Y;

	DataGridCell *pCell = NULL;
	DataGridTable *pDataGridTable = m_pObj_PicGrid->DataGridTable_Get();
	if( pDataGridTable )
		pCell = pDataGridTable->GetData(m_pObj_PicGrid->m_iHighlightedColumn + m_pObj_PicGrid->m_GridCurCol,m_pObj_PicGrid->m_iHighlightedRow + m_pObj_PicGrid->m_GridCurRow );

	PLUTO_SAFETY_LOCK(M, m_pMouseBehavior->m_pOrbiter->m_VariableMutex );

	DesignObj_Orbiter *pObjCoverartImageObj = m_pMouseBehavior->m_pOrbiter->FindObject(m_pObj_CoverArtPopup->m_ObjectID + "." TOSTRING(DESIGNOBJ_objCDCover_CONST));
	if(NULL != pObjCoverartImageObj)
		m_pMouseBehavior->m_pOrbiter->Renderer()->DestroyGraphic(pObjCoverartImageObj->GenerateObjectHash(m_pObj_CoverArtPopup->m_pPopupPoint, false));

	if( pCell && pCell->m_pGraphic && pCell->m_pGraphic->m_pGraphicData)
	{
		char *pDup = new char[pCell->m_pGraphic->m_GraphicLength];
		memcpy(pDup,pCell->m_pGraphic->m_pGraphicData,pCell->m_pGraphic->m_GraphicLength);
		M.Release();

		m_pMouseBehavior->m_pOrbiter->CMD_Update_Object_Image(m_pObj_CoverArtPopup->m_ObjectID + "." TOSTRING(DESIGNOBJ_objCDCover_CONST),"jpg",pDup,int(pCell->m_pGraphic->m_GraphicLength),"0");
		m_pMouseBehavior->m_pOrbiter->CMD_Show_Popup(m_pObj_CoverArtPopup->m_ObjectID,X,Y,"","coverart",false,false);

		delete [] pDup;
	}
	else if( pCell && pCell->m_pGraphicData )
	{
		char *pDup = new char[pCell->m_GraphicLength];
		memcpy(pDup,pCell->m_pGraphicData,pCell->m_GraphicLength);
		M.Release();
		m_pMouseBehavior->m_pOrbiter->CMD_Update_Object_Image(m_pObj_CoverArtPopup->m_ObjectID + "." TOSTRING(DESIGNOBJ_objCDCover_CONST),"jpg",pDup,int(pCell->m_GraphicLength),"0");
		m_pMouseBehavior->m_pOrbiter->CMD_Show_Popup(m_pObj_CoverArtPopup->m_ObjectID,X,Y,"","coverart",false,false);
		delete [] pDup;
	}
	else
	{
		M.Release();
		m_pMouseBehavior->m_pOrbiter->CMD_Remove_Popup("","coverart");
	}
}

