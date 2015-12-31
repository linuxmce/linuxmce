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
#include "KeyboardMouseHandler.h"
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

KeyboardMouseHandler::KeyboardMouseHandler(DesignObj_Orbiter *pObj,string sOptions,MouseBehavior *pMouseBehavior) 
	: MouseHandler(pObj,sOptions,pMouseBehavior)
{ 
	m_spDatagridMouseHandlerHelper.reset(new DatagridMouseHandlerHelper(this));
	m_bLockAxis=true; 
}

void KeyboardMouseHandler::Start()
{
	m_PK_Direction_Last=0;
	m_cDirection=0;
	m_pMouseBehavior->SetMouseCursorStyle(MouseBehavior::mcs_AnyDirection);
	m_pMouseBehavior->m_pMouseIterator->SetIterator(MouseIterator::if_None,0,"",0,NULL); // In case we're scrolling a grid
	m_b1Step = m_sOptions.find('1')!=string::npos;
	if( !m_pObj )
		m_pMouseBehavior->SetMousePosition(m_pMouseBehavior->m_pOrbiter->m_Width/2,
			m_pMouseBehavior->m_pOrbiter->m_Height/2);
	else
	{
		if ( !m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted )
		{
			m_pMouseBehavior->SelectFirstObject();
			if( m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted )
				m_pMouseBehavior->HighlightObject(m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted);
		}
		if( m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted )
		{
			PlutoRectangle rect = m_pMouseBehavior->GetHighlighedObjectCoordinates();
			m_pMouseBehavior->SetMousePosition( rect.X + rect.Width/2,
				rect.Y + rect.Height/2 );
		}

		m_iLastNotch = -999;
	}

	if( m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted && m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST )
		m_spDatagridMouseHandlerHelper->Start( (DesignObj_DataGrid *) m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted,10,m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_rPosition.Y,m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_rPosition.Bottom());
}

void KeyboardMouseHandler::Stop()
{
	if( m_pObj )
	{
		// temp hack -- todo.  temporary redraw the screen since the keyboard/speed indicators may be messed up
NeedToRender render( m_pMouseBehavior->m_pOrbiter, "change k/b" );
m_pMouseBehavior->m_pOrbiter->Renderer()->RenderObjectAsync(m_pObj);
	}
}

bool KeyboardMouseHandler::ButtonDown(int PK_Button)
{
	if( PK_Button==BUTTON_Mouse_1_CONST )
	{
		if( m_pObj )
		{
			m_pMouseBehavior->m_pOrbiter->CMD_Simulate_Keypress(StringUtils::itos(BUTTON_Enter_CONST),0,"");
		}
	}
	return false; // Keep processing
}

bool KeyboardMouseHandler::ButtonUp(int PK_Button)
{
	return false; // Keep processing
}

void KeyboardMouseHandler::Move(int X,int Y,int PK_Direction)
{
	if( !m_pObj && !m_spDatagridMouseHandlerHelper->m_dwPK_Direction_ScrollGrid && m_PK_Direction_Last && PK_Direction!=m_PK_Direction_Last && m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted )
	{
LoggerWrapper::GetInstance()->Write(LV_FESTIVAL,"xxKeyboardMouseHandler::Move changed direction");
		// The user is reversing direction.  Recenter so we don't switch back to easily, to give that 'pop' feel to the button
		PlutoRectangle rect = m_pMouseBehavior->GetHighlighedObjectCoordinates();
		m_pMouseBehavior->SetMousePosition( rect.X + rect.Width/2,
			rect.Y + rect.Height/2 );
		m_PK_Direction_Last=PK_Direction;
		return;
	}
	m_PK_Direction_Last=PK_Direction;

	if( !m_pObj )
	{
		if( m_b1Step )
			MoveExternalApp1Step(X,Y);
		else
			MoveExternalApp(X,Y);
		return;
	}

	if( !m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted )
	{
		m_pMouseBehavior->SelectFirstObject();
		return; // Shouldn't have happened
	}

	if( m_spDatagridMouseHandlerHelper->m_dwPK_Direction_ScrollGrid )
	{
		m_spDatagridMouseHandlerHelper->ScrollGrid(PK_Direction,X,Y);
		return;
	}

	PK_Direction = m_pMouseBehavior->GetDirectionAwayFromHighlight(X,Y);
	if( PK_Direction )
	{
LoggerWrapper::GetInstance()->Write(LV_FESTIVAL,"xxKeyboardMouseHandler::Move away from highlight %d at %d,%d",PK_Direction,X,Y);
		DesignObj_Orbiter *pObj_Before = m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted;
/*
		// Let's see if we've been highlighting a datagrid and moved past the top or bottom
		// If so, we're going to engage special behavior where we start the iterator to do scroll
		if( pObj_Before->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST )
		{
			if( m_spDatagridMouseHandlerHelper->StayInGrid(PK_Direction,X,Y) )
			{
				TempHack_DrawAlphaSquare();
				return;
			}
		}
*/		
		if( !m_pMouseBehavior->m_pOrbiter->Renderer()->HighlightNextObject(PK_Direction) )
		{
/*
			// We're off in no-mans land, and not moving toward any selectable object, get back on the highlighted object
			PlutoRectangle rect = m_pMouseBehavior->GetHighlighedObjectCoordinates();
			m_pMouseBehavior->SetMousePosition( rect.X + rect.Width/2,
				rect.Y + rect.Height/2 );
				*/
			return;
		}

		if( m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted && 
			pObj_Before!=m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted )
		{
LoggerWrapper::GetInstance()->Write(LV_FESTIVAL,"KeyboardMouseHandler::Move was %s now %s after direction %d",
					  pObj_Before->m_ObjectID.c_str(),m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_ObjectID.c_str(),PK_Direction);
			m_pMouseBehavior->HighlightObject(m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted);

//			m_pMouseBehavior->PositionMouseAtObjectEdge(PK_Direction);
			PlutoRectangle rect = m_pMouseBehavior->GetHighlighedObjectCoordinates();

			if( m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST )
				m_spDatagridMouseHandlerHelper->Start( (DesignObj_DataGrid *) m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted,10,m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_rPosition.Y,m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_rPosition.Bottom());
		}
	}
}

void KeyboardMouseHandler::MoveExternalApp1Step(int X,int Y)
{
	/*
	int NotchSize = bHorizontal ? m_pMouseBehavior->m_pOrbiter->m_Width / m_pMouseBehavior->m_MouseSensitivity.NumNotchesForExternalApp : m_pMouseBehavior->m_pOrbiter->m_Height / m_pMouseBehavior->m_MouseSensitivity.NumNotchesForExternalApp;
	int Diff = bHorizontal ? X-m_pMouseBehavior->m_pOrbiter->m_Width/2 : Y-m_pMouseBehavior->m_pOrbiter->m_Height/2;
	if( abs(Diff)>NotchSize )
	{
		m_pMouseBehavior->SetMousePosition(m_pMouseBehavior->m_pOrbiter->m_Width/2,
			m_pMouseBehavior->m_pOrbiter->m_Height/2);
		int PK_Button;
		if( bHorizontal )
			PK_Button = Diff < 0 ? BUTTON_Left_Arrow_CONST : BUTTON_Right_Arrow_CONST;
		else
			PK_Button = Diff < 0 ? BUTTON_Up_Arrow_CONST : BUTTON_Down_Arrow_CONST;

		DCE::CMD_Simulate_Keypress CMD_Simulate_Keypress(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,
			StringUtils::itos(PK_Button),"");
		m_pMouseBehavior->m_pOrbiter->SendCommand(CMD_Simulate_Keypress);
	}
	*/
}

void KeyboardMouseHandler::MoveExternalApp(int X,int Y)
{
	/*
	bool bHorizontal = m_pMouseBehavior->m_cLocked_Axis_Current==AXIS_LOCK_X;
	int NotchSize = (double) bHorizontal ? m_pMouseBehavior->m_pOrbiter->m_Width / 20 : m_pMouseBehavior->m_pOrbiter->m_Height / 20;
	int Notch;
	if( bHorizontal )
		Notch = X/NotchSize - 10;
	else
		Notch = Y/NotchSize - 10;
LoggerWrapper::GetInstance()->Write(LV_FESTIVAL,"Notch %d  horizontal %d  %d,%d",Notch,(int) bHorizontal,X,Y);

	if( Notch<-10 )
		Notch = -10;
	if( Notch>10 )
		Notch = 10;

	if( Notch!=m_iLastNotch )
	{
		if( Notch==0 )
			m_pMouseBehavior->m_pMouseIterator->SetIterator(MouseIterator::if_None,0,0,NULL);
		else
		{
			int Frequency;
			bool bPage=false;
			if( abs(Notch)>5 )
			{
				bPage=true;
				if( Notch>0 )
					Frequency = (6-(Notch-5))*400;
				else
					Frequency = (6+(Notch+5))*400;
			}
			else
				Frequency = (6-abs(Notch))*400;

LoggerWrapper::GetInstance()->Write(LV_FESTIVAL,"Frequency %d",Frequency);
			if( bHorizontal )
			{
				if( bPage )
					m_cDirection = Notch > 0 ? 'R' : 'L';
				else
					m_cDirection = Notch > 0 ? 'r' : 'l';
			}
			else
			{
				if( bPage )
					m_cDirection = Notch > 0 ? 'D' : 'U';
				else
					m_cDirection = Notch > 0 ? 'd' : 'u';
			}
			m_pMouseBehavior->m_pMouseIterator->SetIterator(MouseIterator::if_Keyboard,0,Frequency,this);
		}
		m_iLastNotch=Notch;
	}
	*/
}

void KeyboardMouseHandler::DoIteration()
{
LoggerWrapper::GetInstance()->Write(LV_FESTIVAL,"Direction %c",m_cDirection);
	if( m_pObj )
		IterateObject();
	else
		IterateExternalApp();
}

void KeyboardMouseHandler::IterateObject()
{
	switch( m_cDirection )
	{
		case 'U':
		{
			m_pMouseBehavior->m_pOrbiter->CMD_Simulate_Keypress(
				StringUtils::itos(BUTTON_Scroll_Up_CONST),0,"");
		}
		break;

		case 'D':
		{
			m_pMouseBehavior->m_pOrbiter->CMD_Simulate_Keypress(
				StringUtils::itos(BUTTON_Scroll_Down_CONST),0,"");
		}
		break;

		case 'u':
		{
			m_pMouseBehavior->m_pOrbiter->CMD_Simulate_Keypress(
				StringUtils::itos(BUTTON_Up_Arrow_CONST),0,"");
		}
		break;

		case 'd':
		{
			m_pMouseBehavior->m_pOrbiter->CMD_Simulate_Keypress(
				StringUtils::itos(BUTTON_Down_Arrow_CONST),0,"");
		}
		break;

		case 'L':
		{
			m_pMouseBehavior->m_pOrbiter->CMD_Simulate_Keypress(
				StringUtils::itos(BUTTON_Scroll_Left_CONST),0,"");
		}
		break;

		case 'R':
		{
			m_pMouseBehavior->m_pOrbiter->CMD_Simulate_Keypress(
				StringUtils::itos(BUTTON_Scroll_Right_CONST),0,"");
		}
		break;

		case 'l':
		{
			m_pMouseBehavior->m_pOrbiter->CMD_Simulate_Keypress(
				StringUtils::itos(BUTTON_Left_Arrow_CONST),0,"");
		}
		break;

		case 'r':
		{
			m_pMouseBehavior->m_pOrbiter->CMD_Simulate_Keypress(
				StringUtils::itos(BUTTON_Right_Arrow_CONST),0,"");
		}
		break;
	}
}

void KeyboardMouseHandler::IterateExternalApp()
{
	switch( m_cDirection )
	{
		case 'U':
		{
			DCE::CMD_Simulate_Keypress CMD_Simulate_Keypress(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,
				StringUtils::itos(BUTTON_Scroll_Up_CONST),0,"");
			m_pMouseBehavior->m_pOrbiter->SendCommand(CMD_Simulate_Keypress);
		}
		break;

		case 'D':
		{
			DCE::CMD_Simulate_Keypress CMD_Simulate_Keypress(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,
				StringUtils::itos(BUTTON_Scroll_Down_CONST),0,"");
			m_pMouseBehavior->m_pOrbiter->SendCommand(CMD_Simulate_Keypress);
		}
		break;

		case 'u':
		{
			DCE::CMD_Simulate_Keypress CMD_Simulate_Keypress(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,
				StringUtils::itos(BUTTON_Up_Arrow_CONST),0,"");
			m_pMouseBehavior->m_pOrbiter->SendCommand(CMD_Simulate_Keypress);
		}
		break;

		case 'd':
		{
			DCE::CMD_Simulate_Keypress CMD_Simulate_Keypress(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,
				StringUtils::itos(BUTTON_Down_Arrow_CONST),0,"");
			m_pMouseBehavior->m_pOrbiter->SendCommand(CMD_Simulate_Keypress);
		}
		break;

		case 'L':
		{
			DCE::CMD_Simulate_Keypress CMD_Simulate_Keypress(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,
				StringUtils::itos(BUTTON_Scroll_Left_CONST),0,"");
			m_pMouseBehavior->m_pOrbiter->SendCommand(CMD_Simulate_Keypress);
		}
		break;

		case 'R':
		{
			DCE::CMD_Simulate_Keypress CMD_Simulate_Keypress(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,
				StringUtils::itos(BUTTON_Scroll_Right_CONST),0,"");
			m_pMouseBehavior->m_pOrbiter->SendCommand(CMD_Simulate_Keypress);
		}
		break;

		case 'l':
		{
			DCE::CMD_Simulate_Keypress CMD_Simulate_Keypress(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,
				StringUtils::itos(BUTTON_Left_Arrow_CONST),0,"");
			m_pMouseBehavior->m_pOrbiter->SendCommand(CMD_Simulate_Keypress);
		}
		break;

		case 'r':
		{
			DCE::CMD_Simulate_Keypress CMD_Simulate_Keypress(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,
				StringUtils::itos(BUTTON_Right_Arrow_CONST),0,"");
			m_pMouseBehavior->m_pOrbiter->SendCommand(CMD_Simulate_Keypress);
		}
		break;
	}
}


void KeyboardMouseHandler::TempHack_DrawAlphaSquare()
{
	DesignObj_Orbiter *pObj = m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted;
	if( !pObj || pObj->m_ObjectType!=DESIGNOBJTYPE_Datagrid_CONST || !m_spDatagridMouseHandlerHelper->m_pObj_MediaBrowser_Alpha )
		return;
	
	DesignObj_DataGrid *pObj_Grid = (DesignObj_DataGrid *) pObj;
	if( pObj_Grid->m_iHighlightedRow==-1 || !pObj_Grid->DataGridTable_Get())
		return;

	DataGridCell *pCell=pObj_Grid->DataGridTable_Get()->GetData( 1,  pObj_Grid->m_GridCurRow + pObj_Grid->m_iHighlightedRow );
	if( !pCell )
		return;

	char cLetter = pCell->m_Text ? pCell->m_Text[0] : 'A';
	if( cLetter>'Z' )
		cLetter -= 32; 

	if( cLetter<65 )
		cLetter = 0;
	else
		cLetter = cLetter - 64; // 0=anything non-alpha, 1=a, etc.

	float NotchWidth = ((float) m_spDatagridMouseHandlerHelper->m_pObj_MediaBrowser_Alpha->m_rPosition.Height)/27; // Allow for 0,A-Z
	int NotchStart = int(cLetter * NotchWidth) + m_spDatagridMouseHandlerHelper->m_pObj_MediaBrowser_Alpha->m_rPosition.Y + m_spDatagridMouseHandlerHelper->m_pObj_MediaBrowser_Alpha->m_pPopupPoint.Y;
	m_pMouseBehavior->m_pOrbiter->Renderer()->HollowRectangle(
		m_spDatagridMouseHandlerHelper->m_pObj_MediaBrowser_Alpha->m_rPosition.X + m_spDatagridMouseHandlerHelper->m_pObj_MediaBrowser_Alpha->m_pPopupPoint.X, NotchStart,
		m_spDatagridMouseHandlerHelper->m_pObj_MediaBrowser_Alpha->m_rPosition.Width, int(NotchWidth),
		PlutoColor::White());
	m_pMouseBehavior->m_pOrbiter->Renderer()->UpdateRect(m_spDatagridMouseHandlerHelper->m_pObj_MediaBrowser_Alpha->m_rPosition + m_spDatagridMouseHandlerHelper->m_pObj_MediaBrowser_Alpha->m_pPopupPoint);

}

bool KeyboardMouseHandler::SlowDrift(int &X,int &Y)
{
	if( m_spDatagridMouseHandlerHelper->m_dwPK_Direction_ScrollGrid )
		return false;
LoggerWrapper::GetInstance()->Write(LV_FESTIVAL,"xxKeyboardMouseHandler::SlowDrift");
	if( m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted )
	{
		PlutoRectangle rect = m_pMouseBehavior->GetHighlighedObjectCoordinates();
		m_pMouseBehavior->SetMousePosition( rect.X + rect.Width/2,
			rect.Y + rect.Height/2 );
		return true;
	}
	return false;
}

