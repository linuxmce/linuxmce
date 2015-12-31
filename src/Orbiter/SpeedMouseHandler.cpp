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
#include "SpeedMouseHandler.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/ProcessUtils.h"
#include "PlutoUtils/Other.h"
#include "MouseGovernor.h"
#include "MouseIterator.h"
#include "ScreenHistory.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "pluto_main/Define_Button.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_Text.h"
#include "OrbiterRenderer.h"

using namespace DCE;

const int SpeedMouseHandler::m_iSpeeds[] = {0,250,500,1000,2000,3000,4000,6000,8000,10000,15000,20000,30000,50000,100000,200000,400000};

SpeedMouseHandler::SpeedMouseHandler(DesignObj_Orbiter *pObj,string sOptions,MouseBehavior *pMouseBehavior) 
	: MouseHandler(pObj,sOptions,pMouseBehavior)
{
	m_CurrentMedia_Start = m_CurrentMedia_Stop = m_CurrentMedia_Pos = 0;
	m_iLastGoodPosition = 0;

	m_bTapAndRelease=false; // We'll set it to true later
	string sResponse;
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"SpeedMouseHandler::SpeedMouseHandler setting playback speed to 0 device %d",m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying);
#endif

	DCE::CMD_Bind_to_Media_Remote CMD_Bind_to_Media_Remote(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_MediaPlugIn,0,"","1","", StringUtils::itos( m_pMouseBehavior->m_pOrbiter->m_pLocationInfo->PK_EntertainArea ),0,0);  // 0 for the screen means don't send us to the real remote if we're on the wrong one
	m_pMouseBehavior->m_pOrbiter->SendCommand(CMD_Bind_to_Media_Remote);

	m_iLastGoodPosition=-1;
	m_iCancelLevel = 0;
	m_iLastNotch=0;
	m_pMouseBehavior->m_iTime_Last_Mouse_Down=ProcessUtils::GetMsTime();  // The above may have taken too much time already

	if( m_pMouseBehavior->m_pOrbiter->m_bShowingSpeedBar || !m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying )
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"SpeedMouseHandler::SpeedMouseHandler skipping because the speed wasn't changed by the mouse or nothing is playing %d/%d",
			(int) m_pMouseBehavior->m_pOrbiter->m_bShowingSpeedBar,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying);
#endif
		return;
	}
	DCE::CMD_Change_Playback_Speed CMD_Change_Playback_Speed(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,0,0,true);
	m_pMouseBehavior->m_pOrbiter->SendCommand(CMD_Change_Playback_Speed);
}

SpeedMouseHandler::~SpeedMouseHandler() 
{
	if( m_pMouseBehavior->m_pOrbiter->m_bShowingSpeedBar )
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"SpeedMouseHandler::]`SpeedMouseHandler skipping because the speed wasn't changed by the mouse %d/%d",
			(int) m_pMouseBehavior->m_pOrbiter->m_bShowingSpeedBar,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying);
#endif
		return;
	}
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"SpeedMouseHandler::~SpeedMouseHandler setting playback speed to normal");
#endif

	m_pMouseBehavior->m_pMouseGovernor->Purge();

	DCE::CMD_Change_Playback_Speed CMD_Change_Playback_Speed(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,0,1000,false);
	m_pMouseBehavior->m_pOrbiter->SendCommand(CMD_Change_Playback_Speed);
	DCE::CMD_Bind_to_Media_Remote CMD_Bind_to_Media_Remote(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_MediaPlugIn,0,"","0","", StringUtils::itos( m_pMouseBehavior->m_pOrbiter->m_pLocationInfo->PK_EntertainArea ),0,0);
	m_pMouseBehavior->m_pOrbiter->SendCommand(CMD_Bind_to_Media_Remote);
}

void SpeedMouseHandler::Start()
{
	if( m_pMouseBehavior->m_pOrbiter->m_bShowingSpeedBar || !m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying )
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"SpeedMouseHandler::Start skipping because the speed wasn't changed by the mouse %d/%d",
			(int) m_pMouseBehavior->m_pOrbiter->m_bShowingSpeedBar,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying);
#endif
		return;
	}

	m_pMouseBehavior->SetMouseCursorStyle(MouseBehavior::mcs_LeftRight);

	PlutoRectangle rect(m_pObj->m_rPosition.X + m_pObj->m_pPopupPoint.X,
		m_pObj->m_rPosition.Y + m_pObj->m_pPopupPoint.Y + int(m_pObj->m_rPosition.Height*.2),
		m_pObj->m_rPosition.Width,1);
	m_pMouseBehavior->ConstrainMouse(rect);

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"SpeedMouseHandler::Start timeline %d m_iTime_Last_Mouse_Up); %d",(int) m_pMouseBehavior->m_bHasTimeline, (int) m_pMouseBehavior->m_iTime_Last_Mouse_Up);
#endif
	m_pMouseBehavior->m_pMouseGovernor->SetBuffer(500);
	if( !m_pMouseBehavior->m_bHasTimeline || m_pMouseBehavior->m_iTime_Last_Mouse_Up )
		m_bTapAndRelease=true;
	else
		m_bTapAndRelease=false;  // Can only do this if we have the timeline, and the user is holding down

	m_CurrentMedia_Stop = ParseTime(m_pMouseBehavior->m_pOrbiter->m_sTotalTime);
	m_CurrentMedia_Pos = ParseTime(m_pMouseBehavior->m_pOrbiter->m_sTime);
	m_iCancelLevel = m_CurrentMedia_Pos;
	if( m_pMouseBehavior->m_bHasTimeline && m_bTapAndRelease==false )
	{
		m_iLastNotch=m_CurrentMedia_Pos;
		float Percentage = m_CurrentMedia_Pos==0 ? 0 : ((float) m_CurrentMedia_Pos)/(m_CurrentMedia_Stop-m_CurrentMedia_Start);
		int X = int(m_pObj->m_rPosition.Width * Percentage) + m_pObj->m_rPosition.X + m_pObj->m_pPopupPoint.X;
		int Y = m_pObj->m_rPosition.Y + m_pObj->m_pPopupPoint.Y + (m_pObj->m_rPosition.Height/2);
		m_pMouseBehavior->SetMousePosition(X,Y);
		m_iLastGoodPosition=X;
		m_pObj->m_GraphicToDisplay_set("smh1",1);
	}
	else
	{
		int X=m_pObj->m_rPosition.X+m_pObj->m_pPopupPoint.X+m_pObj->m_rPosition.Width/2;
		m_pMouseBehavior->SetMousePosition(X,m_pObj->m_rPosition.Y+m_pObj->m_pPopupPoint.Y+m_pObj->m_rPosition.Height/2);
		m_iLastGoodPosition=X;
	}
}

void SpeedMouseHandler::Stop()
{
}

bool SpeedMouseHandler::ButtonDown(int PK_Button)
{
	if( m_pMouseBehavior->m_pOrbiter->m_bShowingSpeedBar )
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"SpeedMouseHandler::ButtonDown skipping because the speed wasn't changed by the mouse %d/%d",
			(int) m_pMouseBehavior->m_pOrbiter->m_bShowingSpeedBar,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying);
#endif
		return false; // Keep processing
	}
	if( PK_Button==BUTTON_Mouse_1_CONST || PK_Button==BUTTON_Mouse_6_CONST || PK_Button==BUTTON_Mouse_7_CONST || PK_Button==BUTTON_Mouse_2_CONST || PK_Button==BUTTON_Mouse_8_CONST )
	{
		m_pMouseBehavior->m_tIgnoreSpeedChangesUntil = time(NULL) + 3;  // Ignore all speed changes for 2 seconds so the player has a chance to settle down
		m_pMouseBehavior->m_pMouseGovernor->Purge();
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"SpeedMouseHandler::ButtonDown setting playback speed to 0 ignore till %d",(int) m_pMouseBehavior->m_tIgnoreSpeedChangesUntil);
#endif
		DCE::CMD_Change_Playback_Speed CMD_Change_Playback_Speed(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,0,1000,false);
		m_pMouseBehavior->m_pOrbiter->SendCommand(CMD_Change_Playback_Speed);
		if( PK_Button==BUTTON_Mouse_2_CONST && m_pMouseBehavior->m_bHasTimeline )
		{
			DCE::CMD_Set_Media_Position CMD_Set_Media_Position(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,0," POS:" + StringUtils::itos(m_iCancelLevel*1000));
			m_pMouseBehavior->m_pOrbiter->QueueMessageToRouter(CMD_Set_Media_Position.m_pMessage); // Queue it so it gets there after we're done
		}
		m_pMouseBehavior->Clear(true); // this will be deleted
		return false; // Keep processing
	}
	return false; // Keep processing
}

bool SpeedMouseHandler::ButtonUp(int PK_Button)
{
	if( m_pMouseBehavior->m_pOrbiter->m_bShowingSpeedBar )
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"SpeedMouseHandler::ButtonUp skipping because the speed wasn't changed by the mouse %d/%d",
			(int) m_pMouseBehavior->m_pOrbiter->m_bShowingSpeedBar,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying);
#endif
		return false; // Keep processing
	}
	if( PK_Button==BUTTON_Mouse_6_CONST )  // The user was in press and hold mode, or tapped again after the menu appeared on screen
	{
		// There is a logic flaw that when there's only a speed control and no vertical, it goes straight here and immediately stops
		m_pMouseBehavior->m_pMouseGovernor->Purge();
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"SpeedMouseHandler::ButtonUp setting playback speed to 0");
#endif
		DCE::CMD_Change_Playback_Speed CMD_Change_Playback_Speed(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,0,1000,false);
		m_pMouseBehavior->m_pOrbiter->SendCommand(CMD_Change_Playback_Speed);
		m_pMouseBehavior->Clear(true);
		return false; // this is now invalid
	}
	return false; // Keep processing
}

void SpeedMouseHandler::Move(int X,int Y,int PK_Direction)
{
	if( m_pMouseBehavior->m_pOrbiter->m_bShowingSpeedBar )
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"SpeedMouseHandler::Move skipping because the speed wasn't changed by the mouse %d/%d",
			(int) m_pMouseBehavior->m_pOrbiter->m_bShowingSpeedBar,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying);
#endif
		return;
	}
	m_iLastGoodPosition = X;
	int XStart = m_pObj->m_rPosition.X+m_pObj->m_pPopupPoint.X+m_pObj->m_rPosition.Width/2;
	if( m_bTapAndRelease || !m_pMouseBehavior->m_bHasTimeline )  // holding button down, change speed, or this stream doesn't support absolute positioning anyway
	{
		int NotchWidth = m_pObj->m_rPosition.Width/2/MAX_SPEEDS;
		int Notch = abs(X-XStart) / NotchWidth;
		if( Notch>MAX_SPEEDS-1 )
			Notch = MAX_SPEEDS-1;
		int Speed = m_iSpeeds[Notch];
		if( X<XStart )
		{
			Notch *= -1;
			Speed *= -1;
		}
		if( Notch!=m_iLastNotch )
		{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"SpeedMouseHandler::Move setting playback speed to %d",Speed);
#endif
			DCE::CMD_Change_Playback_Speed CMD_Change_Playback_Speed(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,0,Speed,true);
			m_pMouseBehavior->m_pMouseGovernor->SendMessage(CMD_Change_Playback_Speed.m_pMessage);
			m_iLastNotch=Notch;
NeedToRender render( m_pMouseBehavior->m_pOrbiter, "start speed" );
m_pMouseBehavior->m_pOrbiter->Renderer()->RenderObjectAsync(m_pObj);// Redraw even if the object was already in this state,  because maybe we're hiding this and something that
		}
	}
	else  // holding down, go to position
	{
		int Offset = X - m_pObj->m_rPosition.X-m_pObj->m_pPopupPoint.X;
		double Perc = (double) Offset / m_pObj->m_rPosition.Width;
		if( Perc<0 )
			Perc=0;
		if( Perc>1 )
			Perc=1;
		int Notch = int((m_CurrentMedia_Stop-m_CurrentMedia_Start) * Perc) + m_CurrentMedia_Start;
		if( m_iLastNotch!=Notch )
		{
			m_iLastNotch=Notch;
			DCE::CMD_Set_Media_Position CMD_Set_Media_Position(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,0," POS:" + StringUtils::itos(m_iLastNotch*1000));
			m_pMouseBehavior->m_pMouseGovernor->SendMessage(CMD_Set_Media_Position.m_pMessage);
NeedToRender render( m_pMouseBehavior->m_pOrbiter, "start speed" );
m_pMouseBehavior->m_pOrbiter->Renderer()->RenderObjectAsync(m_pObj);// Redraw even if the object was already in this state,  because maybe we're hiding this and something that
		}
	}
}

bool SpeedMouseHandler::SlowDrift(int &X,int &Y)
{ 
	X = m_iLastGoodPosition;
	Y = m_pObj->m_rPosition.Y + m_pObj->m_pPopupPoint.Y + (m_pObj->m_rPosition.Height/2);
	m_pMouseBehavior->SetMousePosition(X,Y);
	return true;
}

bool SpeedMouseHandler::ParsePosition(string sMediaPosition)
{
	m_CurrentMedia_Start=0;
	string::size_type pos=sMediaPosition.find(" POS:");
	if( pos==string::npos )
		return false;
	m_CurrentMedia_Pos = atoi( sMediaPosition.substr(pos+5).c_str() ) / 1000;
	
	pos=sMediaPosition.find(" TOTAL:");
	if( pos==string::npos )
		return false;
	m_CurrentMedia_Stop = atoi( sMediaPosition.substr(pos+7).c_str() ) / 1000;
	
	return m_CurrentMedia_Stop>0;
}

void SpeedMouseHandler::DrawInfo()
{
	if( m_pObj->m_bHidden )
		return;

	int Style=0; // Option #1, the style
	//char *SpeedOptions = "FULL";  // Option #2, the number of available speeds
	int Speed = -9999;  // Option #3, the current speed, -9999 = don't show any speed
	int SeekPosition = -1;  // Option #5, the current seek to position.  -1 = none

    //LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"SpeedMouseHandler::DrawInfo()");
    if( m_pMouseBehavior->m_bHasTimeline )
	{
		if( m_bTapAndRelease )
			Style = SPEED_STYLE_TIMELINE_SPEED;
		else
			Style = SPEED_STYLE_TIMELINE_SEEK;
	}
	else
		Style = SPEED_STYLE_SPEED_ONLY;

	// We only set the current speed for these
	if( Style==SPEED_STYLE_SPEED_ONLY || Style==SPEED_STYLE_TIMELINE_SPEED )
		Speed = m_iLastNotch;

	if( Style==SPEED_STYLE_TIMELINE_SEEK )
		SeekPosition = m_iLastNotch;

	if( SeekPosition!=-1 )
	{
		float Percent = ((float) SeekPosition) / (m_CurrentMedia_Stop-m_CurrentMedia_Start);
		int Offset = int(m_pObj->m_rPosition.Width * Percent) - 5;
		if( Offset<0 )
			Offset=0;
		m_pMouseBehavior->m_pOrbiter->Renderer()->SolidRectangle(
			m_pObj->m_rPosition.X + Offset, m_pObj->m_rPosition.Y,
			int(m_pObj->m_rPosition.Width * .01), int(m_pObj->m_rPosition.Height * .25),
			PlutoColor::White().SetAlpha(128),
			"",
			"BasePercentSpeed");
	}

	DesignObjText *pText = m_pMouseBehavior->m_pOrbiter->FindText(m_pObj,TEXT_Current_Speed_CONST);
	if( Speed!=-9999 )
	{
		float Percent = ((float) Speed+16)/33;
		int Offset = int(m_pObj->m_rPosition.Width * Percent);
		if( Offset<0 )
			Offset=0;
		int Y = m_pObj->m_rPosition.Y + int(m_pObj->m_rPosition.Height * .3);
		m_pMouseBehavior->m_pOrbiter->Renderer()->SolidRectangle(
			m_pObj->m_rPosition.X + Offset, Y,
			int(m_pObj->m_rPosition.Width * .035), int(m_pObj->m_rPosition.Height * .25),
			PlutoColor::Blue().SetAlpha(128), 
			"",
			"BasePercentSpeed");

		if( pText )
		{
			int SpeedText = m_iSpeeds[abs(Speed)];
			string sSpeed;
			if( SpeedText==250 )
				sSpeed = "1/4";
			else if( SpeedText==500 )
				sSpeed = "1/2";
			else
				sSpeed = StringUtils::itos(SpeedText/1000);
			pText->m_sText=(Speed<0 ? "-" : "") + sSpeed + "x";
			pText->m_rPosition.X = m_pObj->m_rPosition.X + Offset;
		}
	}
	else if( pText )
		pText->m_sText="";  // No speed control

	if( m_pMouseBehavior->m_bHasTimeline )
	{
		DesignObjText *pText = m_pMouseBehavior->m_pOrbiter->FindText(m_pObj,TEXT_Current_Time_CONST);
		if( pText && m_CurrentMedia_Pos>0 )
		{
			float Percent = ((float) m_CurrentMedia_Pos) / (m_CurrentMedia_Stop-m_CurrentMedia_Start);
			int Offset = int(m_pObj->m_rPosition.Width * Percent) - 5;
			if( Offset<0 )
				Offset=0;
			m_pMouseBehavior->m_pOrbiter->Renderer()->SolidRectangle(
				m_pObj->m_rPosition.X, m_pObj->m_rPosition.Y + int(m_pObj->m_rPosition.Height * .6),
				Offset, int(m_pObj->m_rPosition.Height * .12),
				PlutoColor::White().SetAlpha(128),
				"",
				"TimeLine");
			pText->m_rPosition.X = Offset + m_pObj->m_rPosition.X;
			pText->m_sText = FormatTime(m_CurrentMedia_Pos);
		}

		pText = m_pMouseBehavior->m_pOrbiter->FindText(m_pObj,TEXT_Start_Time_CONST);
		if( pText )
			pText->m_sText = FormatTime(m_CurrentMedia_Start);

		pText = m_pMouseBehavior->m_pOrbiter->FindText(m_pObj,TEXT_Stop_Time_CONST);
		if( pText )
			pText->m_sText = FormatTime(m_CurrentMedia_Stop);
	}
}

string SpeedMouseHandler::FormatTime(int Time)
{
	string sResponse;
	int Hours = Time/3600;
	if( Hours )
	{
		sResponse = StringUtils::itos(Hours) + ":";
		Time -= Hours*3600;
	}

	int Minutes = Time/60;
	if( Hours && Minutes<10 )
		sResponse += "0";
	sResponse += StringUtils::itos(Minutes) + ":";

	Time -= Minutes*60;
	if( Time<10 )
		sResponse += "0";
	sResponse += StringUtils::itos(Time);
	return sResponse;
}

int SpeedMouseHandler::ParseTime(string sTime)
{
	string::size_type pos=0;

	int iNumTokens=0;
	int iToken1 = atoi(StringUtils::Tokenize(sTime,":",pos).c_str());
	if( pos<sTime.size() )
		iNumTokens++;

	int iToken2 = atoi(StringUtils::Tokenize(sTime,":",pos).c_str());
	if( pos<sTime.size() )
		iNumTokens++;

	int iToken3 = atoi(StringUtils::Tokenize(sTime,":",pos).c_str());

	if( iNumTokens==2 )
		return iToken1*3600+iToken2*60+iToken3;
	else if( iNumTokens==1 )
		return iToken1*60+iToken2;
	else
		return iToken1;
}
