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
#include "EPGMouseHandler.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/ProcessUtils.h"
#include "PlutoUtils/Other.h"
#include "MouseGovernor.h"
#include "MouseIterator.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "pluto_main/Define_Button.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_Text.h"
#include "DataGrid.h"
#include "OrbiterRenderer.h"

using namespace DCE;

EPGMouseHandler::EPGMouseHandler(DesignObj_Orbiter *pObj,string sOptions,MouseBehavior *pMouseBehavior)
	: MouseHandler(pObj,sOptions,pMouseBehavior)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"MouseHandler const %p",this);
#endif
	m_pDatagridMouseHandlerHelper = new DatagridMouseHandlerHelper(this);
	m_pDesignObj_DataGrid_Active=NULL;
	m_pDesignObj_UpcomingChannels= (DesignObj_DataGrid *) m_pMouseBehavior->m_pOrbiter->FindObject(DESIGNOBJ_dgUpcomingShows_CONST);
	m_pDesignObj_UpcomingChannels->m_bCacheGrid=false;  // Since the grid isn't scrollable until the user moves to it, don't waste the time to cache it
	m_pDesignObjText_Synopsis=m_pDesignObj_UpcomingChannels ? m_pMouseBehavior->m_pOrbiter->FindText( (DesignObj_Orbiter *) m_pDesignObj_UpcomingChannels->m_pParentObject, TEXT_MediaSynopsis_CONST) : NULL;
	if( m_pDesignObjText_Synopsis )
		m_pDesignObjText_Synopsis->m_sText = "";
}

EPGMouseHandler::~EPGMouseHandler()
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"MouseHandler dest %p",this);
#endif
	delete m_pDatagridMouseHandlerHelper;
}

void EPGMouseHandler::Start()
{
	m_iRow_Last=-1;
	if( !m_pObj || m_pObj->m_ObjectType!=DESIGNOBJTYPE_Datagrid_CONST || !m_pDesignObj_UpcomingChannels || !m_pDesignObjText_Synopsis )
		return; // Shouldn't happen, this should be the volume control

	m_pObj_QuickNav = m_pMouseBehavior->m_pOrbiter->FindObject(DESIGNOBJ_grpPVR_UI2_Channels_CONST);
	m_pObj_Bookmark_Channel = m_pMouseBehavior->m_pOrbiter->FindObject(DESIGNOBJ_butUI2_Ch_Prev_Bookmark_Chan_CONST);
	m_pObj_Bookmark_Show = m_pMouseBehavior->m_pOrbiter->FindObject(DESIGNOBJ_butUI2_Ch_Prev_Bookmark_Show_CONST);
	m_pObj_Record_Once = m_pMouseBehavior->m_pOrbiter->FindObject(DESIGNOBJ_butUI2_Ch_Prev_Record_Once_CONST);
	m_pObj_Record_All = m_pMouseBehavior->m_pOrbiter->FindObject(DESIGNOBJ_butUI2_Ch_Prev_Record_All_CONST);

	m_pDesignObj_DataGrid_Active = (DesignObj_DataGrid *) m_pObj;

	PlutoRectangle rect(m_pDesignObj_DataGrid_Active->m_rPosition.X + m_pDesignObj_DataGrid_Active->m_pPopupPoint.X + m_pDesignObj_DataGrid_Active->m_rPosition.Width/2,
		m_pDesignObj_DataGrid_Active->m_rPosition.Y + m_pDesignObj_DataGrid_Active->m_pPopupPoint.Y,
		m_pDesignObj_DataGrid_Active->m_rPosition.Width / 2,m_pDesignObj_DataGrid_Active->m_rPosition.Height);
//	m_pMouseBehavior->ConstrainMouse(rect);

	m_pMouseBehavior->m_pMouseGovernor->SetBuffer(2000);

	m_pMouseBehavior->SetMouseCursorStyle(MouseBehavior::mcs_UpDown);
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"EPGMouseHandler::Start m_iTime_Last_Mouse_Up %d",(int) m_pMouseBehavior->m_iTime_Last_Mouse_Up);
#endif

	if( true ) // for now just do it the one way, the absolute seeking was too tough.  m_pMouseBehavior->m_iTime_Last_Mouse_Up )
	{
		m_bTapAndRelease=true;
		m_pMouseBehavior->SetMousePosition(m_pDesignObj_DataGrid_Active->m_rPosition.X + m_pDesignObj_DataGrid_Active->m_pPopupPoint.X + (m_pDesignObj_DataGrid_Active->m_rPosition.Width/2),
			m_pDesignObj_DataGrid_Active->m_rPosition.Y + m_pDesignObj_DataGrid_Active->m_pPopupPoint.Y + (m_pDesignObj_DataGrid_Active->m_rPosition.Height/2));
		m_pDatagridMouseHandlerHelper->Start(m_pDesignObj_DataGrid_Active,10,m_pDesignObj_DataGrid_Active->m_rPosition.Y,m_pDesignObj_DataGrid_Active->m_rPosition.Bottom());
	}
	else
	{
		m_bTapAndRelease=false;

		DataGridTable *pDataGridTable = m_pDesignObj_DataGrid_Active->DataGridTable_Get();
		if( pDataGridTable )
		{
			int Rows = pDataGridTable->GetRows();
			int CurrentRow = m_pDesignObj_DataGrid_Active->m_iHighlightedRow>0 ? m_pDesignObj_DataGrid_Active->m_iHighlightedRow : 0;
			double RowHeight = (double) m_pDesignObj_DataGrid_Active->m_rPosition.Height / Rows;
			int Y = int(CurrentRow * RowHeight + m_pDesignObj_DataGrid_Active->m_rPosition.Y + m_pDesignObj_DataGrid_Active->m_pPopupPoint.Y);
			int X = m_pDesignObj_DataGrid_Active->m_rPosition.X + m_pDesignObj_DataGrid_Active->m_pPopupPoint.X + (m_pDesignObj_DataGrid_Active->m_rPosition.Width/2);
			m_pMouseBehavior->SetMousePosition(X,Y);
		}
	}
	m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted_set(m_pDesignObj_DataGrid_Active);
}

void EPGMouseHandler::Stop()
{
	m_pDatagridMouseHandlerHelper->Stop();
}

bool EPGMouseHandler::ButtonDown(int PK_Button)
{
	if( m_pDatagridMouseHandlerHelper->CapturingMouse() )
	{
		if( PK_Button==BUTTON_Mouse_2_CONST )
			m_pDatagridMouseHandlerHelper->ReleaseRelative();
		return true;
	}
	return false; // Keep processing
}

bool EPGMouseHandler::ButtonUp(int PK_Button)
{
	if( PK_Button==BUTTON_Mouse_6_CONST && !m_bTapAndRelease )  // The user was in press and hold mode, or tapped again after the menu appeared on screen
	{
		DCE::CMD_Change_Playback_Speed CMD_Change_Playback_Speed(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,0,1000,false);
		m_pMouseBehavior->m_pOrbiter->SendCommand(CMD_Change_Playback_Speed);
		m_pMouseBehavior->m_pOrbiter->SelectedObject(m_pObj,smNavigation);
		return false; // this is now invalid
	}
	return false; // Keep processing
}

void EPGMouseHandler::Move(int X,int Y,int PK_Direction)
{
	PLUTO_SAFETY_LOCK( cm, m_pMouseBehavior->m_pOrbiter->m_ScreenMutex );  // Always lock this before datagrid to prevent a deadlock
	PLUTO_SAFETY_LOCK( dng, m_pMouseBehavior->m_pOrbiter->m_DatagridMutex );

	if( !m_pObj || m_pObj->m_ObjectType!=DESIGNOBJTYPE_Datagrid_CONST )
		return; // Also shouldn't happen

	if( m_pDesignObj_DataGrid_Active && m_pDesignObj_DataGrid_Active->m_iBaseObjectID==DESIGNOBJ_dgTvChannels_UI2_CONST )
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"left grid X %d right %d", X, m_pDesignObj_DataGrid_Active->m_rPosition.Right());
	if( m_pDesignObj_DataGrid_Active && m_pDesignObj_DataGrid_Active->m_iBaseObjectID==DESIGNOBJ_dgUpcomingShows_CONST )
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"right grid X %d left %d", X, m_pDesignObj_DataGrid_Active->m_rPosition.X );

	if( m_pDesignObj_DataGrid_Active && m_pDesignObj_DataGrid_Active->m_iBaseObjectID==DESIGNOBJ_dgTvChannels_UI2_CONST && X>=m_pDesignObj_DataGrid_Active->m_rPosition.Right()-5 )
	{
		m_iRow_Last=-1;  // The last highlighted row on the right side

		// The user was in the channels list and moved to the right to the grid listing
		m_pDatagridMouseHandlerHelper->ReleaseRelative();

		int Row = (Y-m_pDesignObj_DataGrid_Active->m_rPosition.Y)/m_pDesignObj_DataGrid_Active->m_FixedRowHeight;
		DataGridTable *pDataGridTable = m_pDesignObj_DataGrid_Active->m_pDataGridTable_Current_get();
		if( !pDataGridTable )
			return;
		DataGridCell *pCell = pDataGridTable->GetData(0,Row + m_pDesignObj_DataGrid_Active->m_GridCurRow);
		if( pCell )
			m_pMouseBehavior->m_pOrbiter->CMD_Set_Variable( m_pDesignObj_DataGrid_Active->m_iPK_Variable, pCell->GetValue() );

		m_pDesignObj_DataGrid_Active = m_pDesignObj_UpcomingChannels;
		m_pDesignObj_UpcomingChannels->m_bCacheGrid=true;  // Now that this is the active grid, start caching future pages
		PlutoRectangle rect(m_pDesignObj_DataGrid_Active->m_rPosition.X + m_pDesignObj_DataGrid_Active->m_pPopupPoint.X,
			m_pDesignObj_DataGrid_Active->m_rPosition.Y + m_pDesignObj_DataGrid_Active->m_pPopupPoint.Y,
			m_pDesignObj_DataGrid_Active->m_rPosition.Width / 2,m_pDesignObj_DataGrid_Active->m_rPosition.Height);

		m_pMouseBehavior->SetMousePosition(rect.Right(),Y);

//		m_pMouseBehavior->ConstrainMouse(rect);
		m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted_set(m_pDesignObj_DataGrid_Active);
		m_pMouseBehavior->m_pOrbiter->CMD_Refresh( m_pDesignObj_DataGrid_Active->m_sGridID );
		m_pDatagridMouseHandlerHelper->Start(m_pDesignObj_DataGrid_Active,10,m_pDesignObj_DataGrid_Active->m_rPosition.Y,m_pDesignObj_DataGrid_Active->m_rPosition.Bottom());
	}
	else if( m_pDesignObj_DataGrid_Active && m_pDesignObj_DataGrid_Active->m_iBaseObjectID==DESIGNOBJ_dgUpcomingShows_CONST && X<=m_pDesignObj_DataGrid_Active->m_rPosition.X+5 )
	{
		m_iRow_Last=-1;  // The last highlighted row on the right side

		// The user was in the upcoming shows grid and moved to the left
		m_pDatagridMouseHandlerHelper->ReleaseRelative();
		m_pDesignObj_DataGrid_Active = (DesignObj_DataGrid *) m_pObj;
		m_pDesignObj_UpcomingChannels->m_bCacheGrid=false;  // Since the grid isn't scrollable until the user moves to it, don't waste the time to cache it
		PlutoRectangle rect(m_pDesignObj_DataGrid_Active->m_rPosition.X + m_pDesignObj_DataGrid_Active->m_pPopupPoint.X + m_pDesignObj_DataGrid_Active->m_rPosition.Width/2,
			m_pDesignObj_DataGrid_Active->m_rPosition.Y + m_pDesignObj_DataGrid_Active->m_pPopupPoint.Y,
			m_pDesignObj_DataGrid_Active->m_rPosition.Width / 2,m_pDesignObj_DataGrid_Active->m_rPosition.Height);
		m_pMouseBehavior->SetMousePosition(rect.X,Y);
//		m_pMouseBehavior->ConstrainMouse(rect);
		m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted_set(m_pDesignObj_DataGrid_Active);
		m_pMouseBehavior->m_pOrbiter->CMD_Refresh( "" );
		m_pDatagridMouseHandlerHelper->Start(m_pDesignObj_DataGrid_Active,10,m_pDesignObj_DataGrid_Active->m_rPosition.Y,m_pDesignObj_DataGrid_Active->m_rPosition.Bottom());
		m_pObj_QuickNav->m_bHidden=true;
	}

    if (m_bTapAndRelease && m_pDatagridMouseHandlerHelper->Move(X,Y,PK_Direction))
		return;

	//DesignObj_DataGrid *pObj_Grid = (DesignObj_DataGrid *) m_pObj;

	if( m_pDesignObj_DataGrid_Active && m_pDesignObj_DataGrid_Active->m_iBaseObjectID==DESIGNOBJ_dgUpcomingShows_CONST && X<m_pDesignObj_DataGrid_Active->m_rPosition.Right() )
	{
		DataGridTable *pDataGridTable = m_pDesignObj_DataGrid_Active ? m_pDesignObj_DataGrid_Active->m_pDataGridTable_Current_get() : NULL;
		if( pDataGridTable )
		{
			int Row = (Y-m_pDesignObj_DataGrid_Active->m_rPosition.Y)/m_pDesignObj_DataGrid_Active->m_FixedRowHeight;
			if( Row!=m_iRow_Last )
			{
				NeedToRender render( m_pMouseBehavior->m_pOrbiter, "Move on grid" );
				m_pDesignObj_DataGrid_Active->m_iHighlightedRow=Row;
				m_iRow_Last = Row;
//				m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted_set(m_pDesignObj_DataGrid_Active);
				m_pMouseBehavior->m_pOrbiter->Renderer()->DoHighlightObject();
				DataGridCell *pCell = pDataGridTable->GetData(0,Row + m_pDesignObj_DataGrid_Active->m_GridCurRow );

				if( pCell )
				{
					m_pDesignObjText_Synopsis->m_sText = pCell->m_mapAttributes_Find("Synopsis");
					m_pMouseBehavior->m_pOrbiter->Renderer()->RenderObjectAsync((DesignObj_Orbiter *) m_pDesignObj_DataGrid_Active->m_pParentObject);
					m_pObj_Bookmark_Show->m_GraphicToDisplay_set( "EPMH1", pCell->m_mapAttributes_Find("Favorite").empty()==false ? GRAPHIC_SELECTED : GRAPHIC_NORMAL );
					m_pObj_Record_Once->m_GraphicToDisplay_set( "EPMH3", pCell->m_mapAttributes_Find("recording")=="O" ? GRAPHIC_SELECTED : GRAPHIC_NORMAL );
					m_pObj_Record_All->m_GraphicToDisplay_set( "EPMH4", pCell->m_mapAttributes_Find("recording")=="C" ? GRAPHIC_SELECTED : GRAPHIC_NORMAL );
					DataGridTable *pDataGridTable_Channel = m_pObj ? ((DesignObj_DataGrid *) m_pObj)->m_pDataGridTable_Current_get() : NULL;
					if( pDataGridTable_Channel )
					{
						DataGridCell *pCell = pDataGridTable_Channel->GetData(0,((DesignObj_DataGrid *) m_pObj)->m_iHighlightedRow + ((DesignObj_DataGrid *) m_pObj)->m_GridCurRow );
						if( pCell )
							m_pObj_Bookmark_Channel->m_GraphicToDisplay_set( "EPMH2", pCell->m_mapAttributes_Find("Favorite").empty()==false ? GRAPHIC_SELECTED : GRAPHIC_NORMAL );
					}
				}
			}
		}
		if( m_pObj_QuickNav->m_bHidden )
		{
			m_pObj_QuickNav->m_bHidden=false;
			m_pMouseBehavior->m_pOrbiter->Renderer()->RenderObjectAsync(m_pObj_QuickNav);
		}
	}
	if( m_pDesignObj_DataGrid_Active && m_pDesignObj_DataGrid_Active->m_iBaseObjectID==DESIGNOBJ_dgTvChannels_UI2_CONST )
	{
		DataGridTable *pDataGridTable = m_pDesignObj_DataGrid_Active ? m_pDesignObj_DataGrid_Active->m_pDataGridTable_Current_get() : NULL;
		if( pDataGridTable )
		{
			int Row = (Y-m_pDesignObj_DataGrid_Active->m_rPosition.Y)/m_pDesignObj_DataGrid_Active->m_FixedRowHeight;
			if( Row!=m_iRow_Last )
			{
				NeedToRender render( m_pMouseBehavior->m_pOrbiter, "Move on grid" );
				m_pDesignObj_DataGrid_Active->m_iHighlightedRow=Row;
				m_iRow_Last = Row;
//				m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted_set(m_pDesignObj_DataGrid_Active);

				DataGridTable *pDataGridTable = m_pDesignObj_DataGrid_Active->m_pDataGridTable_Current_get();
				if( !pDataGridTable )
					return;
				DataGridCell *pCell = pDataGridTable->GetData(0,Row + m_pDesignObj_DataGrid_Active->m_GridCurRow);
				if( pCell )
				{
					m_pMouseBehavior->m_pOrbiter->CMD_Set_Variable( m_pDesignObj_DataGrid_Active->m_iPK_Variable, pCell->GetValue() + string(",1") );  // The ,1 means don't bother fetching all 2 weeks of guide data, just 1 page
					m_pDesignObjText_Synopsis->m_sText = pCell->m_mapAttributes_Find("Source");
				}

				m_pMouseBehavior->m_pOrbiter->CMD_Refresh( m_pDesignObj_UpcomingChannels->m_sGridID );

				m_pMouseBehavior->m_pOrbiter->Renderer()->DoHighlightObject();
			}
		}
	}
}
