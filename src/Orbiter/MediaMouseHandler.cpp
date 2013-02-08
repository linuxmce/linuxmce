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
#include "MediaMouseHandler.h"
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
#include "DataGrid.h"
#include "OrbiterRenderer.h"

using namespace DCE;

MediaMouseHandler::MediaMouseHandler(DesignObj_Orbiter *pObj,string sOptions,MouseBehavior *pMouseBehavior)
	: MouseHandler(pObj,sOptions,pMouseBehavior)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"MouseHandler const %p",this);
#endif
	m_pDatagridMouseHandlerHelper = new DatagridMouseHandlerHelper(this);
}

MediaMouseHandler::~MediaMouseHandler()
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"MouseHandler dest %p",this);
#endif
	delete m_pDatagridMouseHandlerHelper;
}

void MediaMouseHandler::Start()
{
	if( !m_pObj || m_pObj->m_ObjectType!=DESIGNOBJTYPE_Datagrid_CONST )
		return; // Shouldn't happen, this should be the volume control

	PlutoRectangle rect(m_pObj->m_rPosition.X + m_pObj->m_pPopupPoint.X + m_pObj->m_rPosition.Width/2,
		m_pObj->m_rPosition.Y + m_pObj->m_pPopupPoint.Y,
		1,m_pObj->m_rPosition.Height);
	m_pMouseBehavior->ConstrainMouse(rect);

	DesignObj_DataGrid *pObj_Grid = (DesignObj_DataGrid *) m_pObj;

	m_pMouseBehavior->m_pMouseGovernor->SetBuffer(2000);

	m_pMouseBehavior->SetMouseCursorStyle(MouseBehavior::mcs_UpDown);
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"MediaMouseHandler::Start m_iTime_Last_Mouse_Up %d",(int) m_pMouseBehavior->m_iTime_Last_Mouse_Up);
#endif

	if( true ) // for now just do it the one way, the absolute seeking was too tough.  m_pMouseBehavior->m_iTime_Last_Mouse_Up )
	{
		m_bTapAndRelease=true;
		m_pMouseBehavior->SetMousePosition(pObj_Grid->m_rPosition.X + pObj_Grid->m_pPopupPoint.X + (pObj_Grid->m_rPosition.Width/2),
			pObj_Grid->m_rPosition.Y + pObj_Grid->m_pPopupPoint.Y + (pObj_Grid->m_rPosition.Height/2));
		m_pDatagridMouseHandlerHelper->Start(pObj_Grid,10,pObj_Grid->m_rPosition.Y,pObj_Grid->m_rPosition.Bottom());
	}
	else
	{
		m_bTapAndRelease=false;
		if( pObj_Grid->DataGridTable_Get() )
		{
			int Rows = pObj_Grid->DataGridTable_Get()->GetRows();
			int CurrentRow = pObj_Grid->m_iHighlightedRow>0 ? pObj_Grid->m_iHighlightedRow : 0;
			double RowHeight = (double) pObj_Grid->m_rPosition.Height / Rows;
			int Y = int(CurrentRow * RowHeight + pObj_Grid->m_rPosition.Y + pObj_Grid->m_pPopupPoint.Y);
			int X = pObj_Grid->m_rPosition.X + pObj_Grid->m_pPopupPoint.X + (pObj_Grid->m_rPosition.Width/2);
			m_pMouseBehavior->SetMousePosition(X,Y);
		}
	}
	m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted_set(pObj_Grid);
}

void MediaMouseHandler::Stop()
{
	m_pDatagridMouseHandlerHelper->Stop();
	m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted_set(NULL);
}

bool MediaMouseHandler::ButtonDown(int PK_Button)
{
	if( m_pDatagridMouseHandlerHelper->CapturingMouse() )
		return true;

	if( PK_Button==BUTTON_Mouse_1_CONST || PK_Button==BUTTON_Mouse_6_CONST || PK_Button==BUTTON_Mouse_2_CONST )
	{
		DCE::CMD_Change_Playback_Speed CMD_Change_Playback_Speed(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,0,1000,false);
		m_pMouseBehavior->m_pOrbiter->SendCommand(CMD_Change_Playback_Speed);

		if( !m_bTapAndRelease && PK_Button==BUTTON_Mouse_1_CONST )
		{
			m_pMouseBehavior->m_pOrbiter->SelectedObject(m_pObj,smNavigation);
		}
		return false; // this is now invalid
	}
	return false; // Keep processing
}

bool MediaMouseHandler::ButtonUp(int PK_Button)
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

void MediaMouseHandler::Move(int X,int Y,int PK_Direction)
{
	PLUTO_SAFETY_LOCK( cm, m_pMouseBehavior->m_pOrbiter->m_ScreenMutex );  // Always lock this before datagrid to prevent a deadlock
	PLUTO_SAFETY_LOCK( dng, m_pMouseBehavior->m_pOrbiter->m_DatagridMutex );

	if( !m_pObj || m_pObj->m_ObjectType!=DESIGNOBJTYPE_Datagrid_CONST )
		return; // Also shouldn't happen

    if (m_bTapAndRelease && m_pDatagridMouseHandlerHelper->Move(X,Y,PK_Direction))
		return;

	DesignObj_DataGrid *pObj_Grid = (DesignObj_DataGrid *) m_pObj;
	if( !pObj_Grid->DataGridTable_Get() )
		return; // Again shouldn't happen

	//int Rows = pObj_Grid->DataGridTable_Get()->GetRows();
	/*
	if( m_bTapAndRelease==false )  // Discrete positioning
	{
		double RowHeight = (double) pObj_Grid->m_rPosition.Height / Rows;
		int YDiff = Y - pObj_Grid->m_rPosition.Y - pObj_Grid->m_pPopupPoint.Y;
		int Row = int(YDiff / RowHeight);
		if( Row>Rows-1 )
			Row=Rows-1;
		if( Row!=m_iLastNotch )
		{
			m_iLastNotch=Row;
			pObj_Grid->m_iHighlightedRow = 1;
			pObj_Grid->m_GridCurRow = Row;
	LoggerWrapper::GetInstance()->Write(LV_FESTIVAL,"MouseBehavior::MediaTracks  *discrete* highlighted row %d",Row);
			pObj_Grid->Flush();
			NeedToRender render( m_pMouseBehavior->m_pOrbiter, "MOUSE BEHAVIOR SCROLL" );
			m_pMouseBehavior->m_pOrbiter->Renderer()->RenderObjectAsync(pObj_Grid);
			m_pMouseBehavior->m_pOrbiter->SelectedObject(pObj_Grid,smMouseGovernor);  // Do this last since it could result in a screen change and deletion of ourself
		}
	}
	*/
}
