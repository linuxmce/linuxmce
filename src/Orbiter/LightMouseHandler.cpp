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
#include "LightMouseHandler.h"
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
#include "OrbiterRenderer.h"

using namespace DCE;

void LightMouseHandler::Start()
{
	PlutoRectangle rect(m_pObj->m_rPosition.X + m_pObj->m_pPopupPoint.X  + int(m_pObj->m_rPosition.Width*.1),
		m_pObj->m_rPosition.Y + m_pObj->m_pPopupPoint.Y,
		1,m_pObj->m_rPosition.Height);
	m_pMouseBehavior->ConstrainMouse(rect);
	m_pMouseBehavior->SetMouseCursorStyle(MouseBehavior::mcs_UpDown);

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"LightMouseHandler::Start m_iTime_Last_Mouse_Up %d",(int) m_pMouseBehavior->m_iTime_Last_Mouse_Up);
#endif
	m_pMouseBehavior->m_pMouseGovernor->SetBuffer(500);
	if( m_pMouseBehavior->m_iTime_Last_Mouse_Up )
	{
		m_bTapAndRelease=true;
		m_pObj->m_GraphicToDisplay_set("lmh1",GRAPHIC_NORMAL);
		m_iLastNotch=0;
		m_iCancelLevel = 50;
	}
	else
	{
		m_pObj->m_GraphicToDisplay_set("lmh2",1);
		m_bTapAndRelease=false;
		m_iLastNotch=5;
	}

	int Y=m_pObj->m_rPosition.Y+m_pObj->m_pPopupPoint.Y+m_pObj->m_rPosition.Height/2;
	m_iLastGoodPosition=Y;
	m_pMouseBehavior->SetMousePosition(m_pObj->m_rPosition.X+m_pObj->m_pPopupPoint.X+m_pObj->m_rPosition.Width/2,Y);
}

void LightMouseHandler::Stop()
{
NeedToRender render( m_pMouseBehavior->m_pOrbiter, "change light" );
m_pMouseBehavior->m_pOrbiter->Renderer()->RenderObjectAsync(m_pObj);
#ifdef DEBUG
LoggerWrapper::GetInstance()->Write(LV_CORPCLIENT,"m_pObj.disabled = true");
#endif
}

bool LightMouseHandler::ButtonDown(int PK_Button)
{
	if( PK_Button==BUTTON_Mouse_1_CONST || PK_Button==BUTTON_Mouse_8_CONST )
		m_pMouseBehavior->Clear(true); // this will be deleted
	return false; // Keep processing
}

bool LightMouseHandler::ButtonUp(int PK_Button)
{
	if( PK_Button==BUTTON_Mouse_2_CONST )
	{
		if( !m_bTapAndRelease )
		{
			DCE::CMD_Set_Level CMD_Set_Level(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying_Audio,StringUtils::itos(m_iCancelLevel));
			m_pMouseBehavior->m_pOrbiter->SendMessage(CMD_Set_Level.m_pMessage);
		}
		m_pMouseBehavior->Clear(true); // this will be deleted
		return false; // Keep processing
	}
	else if( PK_Button==BUTTON_Mouse_8_CONST && !m_bTapAndRelease )
	{
		m_pMouseBehavior->Clear(true); // this will be deleted
		return false; // Keep processing

	}
	return false; // Keep processing
}

void LightMouseHandler::Move(int X,int Y,int PK_Direction)
{
	m_iLastGoodPosition=Y;
	//int NotchWidth = m_pObj->m_rPosition.Height/11; // Allow for 5 repeat levels in each direction
	int Notch = (Y - m_pObj->m_rPosition.Y - m_pObj->m_pPopupPoint.Y) * 11 / 
		(m_pObj->m_rPosition.Height + (m_pObj->m_rPosition.Height ? 1 : 0));
	if( Notch<0 )
		Notch=0;
	if( Notch>10 )
		Notch=10;

	if( m_bTapAndRelease )
		Notch = 5-Notch;  // Higher number at top
	else
		Notch = 10-Notch;  // Higher number at top

	if( Notch!=m_iLastNotch )
	{
		NeedToRender render( m_pMouseBehavior->m_pOrbiter, "LightMouseHandler" );
		m_pMouseBehavior->m_pOrbiter->Renderer()->RenderObjectAsync(m_pObj);
		if( m_bTapAndRelease==false )
		{
			DCE::CMD_Set_Level CMD_Set_Level(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_LightingPlugIn,StringUtils::itos(Notch*10));
			m_pMouseBehavior->m_pMouseGovernor->SendMessage(CMD_Set_Level.m_pMessage);
			m_iLastNotch = Notch;
		}
		else
		{
			if( Notch==0 )
				m_pMouseBehavior->m_pMouseIterator->SetIterator(MouseIterator::if_None,0,"",0,NULL);
			else
				m_pMouseBehavior->m_pMouseIterator->SetIterator(MouseIterator::if_Light,Notch,"",500,this);
			m_iLastNotch = Notch;
		}
	}
}

bool LightMouseHandler::SlowDrift(int &X,int &Y)
{ 
	X = m_pObj->m_rPosition.X + m_pObj->m_pPopupPoint.X + (m_pObj->m_rPosition.Width/2);
	Y = m_iLastGoodPosition;
	m_pMouseBehavior->SetMousePosition(X,Y);
	return true;
}

void LightMouseHandler::CustomRender()
{
	int CurrentLevel = m_bTapAndRelease ? m_iCancelLevel : m_iLastNotch*10;
	if( CurrentLevel > 100 )
		CurrentLevel=100;
	if( CurrentLevel < 0 )
		CurrentLevel = 0;
	CurrentLevel = m_pObj->m_rPosition.Height * CurrentLevel / 100;
	int X = m_pObj->m_rPosition.X + m_pObj->m_rPosition.Width -  int(m_pObj->m_rPosition.Width * .1);

    m_pMouseBehavior->m_pOrbiter->Renderer()->SolidRectangle(
		X,m_pObj->m_rPosition.Bottom()-CurrentLevel,
		int(m_pObj->m_rPosition.Width * .1), CurrentLevel,
		PlutoColor::Green().SetAlpha(128), "", "BaseLightRect");

	if( m_bTapAndRelease )
	{
		int NotchWidth = m_pObj->m_rPosition.Height/11; // Allow for 5 repeat levels in each direction
		int NotchStart = (5-m_iLastNotch) * NotchWidth;
		NotchStart = m_pObj->m_rPosition.Y + m_pObj->m_pPopupPoint.Y + NotchStart;
		m_pMouseBehavior->m_pOrbiter->Renderer()->SolidRectangle(
			m_pObj->m_rPosition.X + m_pObj->m_pPopupPoint.X + int(m_pObj->m_rPosition.Width*.5), NotchStart,
			int(m_pObj->m_rPosition.Width*.1), NotchWidth,
			PlutoColor::Blue(), "", "TapReleaseLightRect");
	}

	return;
}

void LightMouseHandler::DoIteration(int Parm)
{
	NeedToRender render( m_pMouseBehavior->m_pOrbiter, "LightMouseHandler it" );
	m_pMouseBehavior->m_pOrbiter->Renderer()->RenderObjectAsync(m_pObj);
	m_iCancelLevel += (Parm*10);
	if( m_iCancelLevel > 100 )
		m_iCancelLevel=100;
	if( m_iCancelLevel < 0 )
		m_iCancelLevel = 0;
	DCE::CMD_Set_Level CMD_Set_Level(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_LightingPlugIn,(Parm>0 ? "+" : "") + StringUtils::itos(Parm*10));
	m_pMouseBehavior->m_pMouseGovernor->SendMessage(CMD_Set_Level.m_pMessage);
}
