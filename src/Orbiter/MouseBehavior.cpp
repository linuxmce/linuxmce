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
#include "MouseBehavior.h"
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
#include "pluto_main/Define_Screen.h"

#include "Simulator.h"

#include "MediaBrowserMouseHandler.h"
#include "HorizMenuMouseHandler.h"
#include "SpeedMouseHandler.h"
#include "LightMouseHandler.h"
#include "VolumeMouseHandler.h"
#include "MediaMouseHandler.h"
#include "EPGMouseHandler.h"
#include "LockedMouseHandler.h"
#include "KeyboardMouseHandler.h"
#include "OrbiterRenderer.h"
#include "ObjectRenderer.h"

using namespace DCE;

#if defined(POCKETFROG)
	// todo...
#else
	#include "SDL/OrbiterRenderer_SDL.h"
#endif
//-----------------------------------------------------------------------------------------------------
MouseBehavior::MouseBehavior(Orbiter *pOrbiter)
{
	m_pObj_Previously_Highlighted=NULL;
	m_pMouseHandler=NULL;
	ProcessUtils::ResetMsTime();
	m_pOrbiter=pOrbiter;
	m_pMouseGovernor = new MouseGovernor(this);
	m_pMouseIterator = new MouseIterator(this);
	m_iPK_Button_Mouse_Last=0;
    m_iTime_Last_Mouse_Down=m_iTime_Last_Mouse_Up=0;
	m_bMouseConstrained=false;
	m_bMouseKilled=false;
	m_bMouseVisible=true;
	m_bHasTimeline=false;  // Indicates if the last set media info
	m_tIgnoreSpeedChangesUntil=0;
	Clear();
}

MouseBehavior::~MouseBehavior()
{
	PLUTO_SAFETY_LOCK(mb, m_pOrbiter->m_ScreenMutex);

	if(NULL != m_pMouseHandler)
	{
		delete m_pMouseHandler;
		m_pMouseHandler = NULL;
	}

	mb.Release();

	delete m_pMouseGovernor;
	delete m_pMouseIterator;
}

void MouseBehavior::Clear(bool bGotoMainMenu)
{
	PLUTO_SAFETY_LOCK(mb,m_pOrbiter->m_ScreenMutex);

	/* there's a logic flaw that when popups are put on screen they're added to the following
	vect's, but not removed when the popup is removed, although they are marked as m_bOnScreen=false.
	These were created as vect's because the concept of popups didn't exist in the beginning.
	Perhaps they should be changed to something else that's easier to remove entries from.
	For now, with the new UI, we know that whenever the mouse behavior is reset we will
	have no grids or tab stops on screen.  Reset them so the list doesn't grow indefinately. */
//	m_pOrbiter->m_vectObjs_GridsOnScreen.clear();
//	m_pOrbiter->m_vectObjs_TabStops.clear();

	m_pObj_Locked=NULL;
	ResetSamples();
	ConstrainMouse(PlutoRectangle(0,0,0,0));
	m_pLastPosition.X=m_pLastPosition.Y=-1;
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"MouseBehavior::Clear MouseHandler dest %p goto menu %d",m_pMouseHandler,(int) bGotoMainMenu);
#endif
	delete m_pMouseHandler;
	m_pMouseHandler=NULL;
	m_pMouseGovernor->SetBuffer(0);
	m_pMouseIterator->SetIterator(MouseIterator::if_None,0,"",0,NULL);
	m_dwTime_Last_Notch=0;
	m_dwLastSampleShift=0;
	HighlightObject(NULL);

	if( bGotoMainMenu )
	{
		NeedToRender render( m_pOrbiter, "start speed" );
		if( m_pOrbiter->m_bIsOSD && m_pOrbiter->m_iPK_Screen_RemoteOSD && m_pOrbiter->m_iLocation_Initial==m_pOrbiter->m_pLocationInfo->iLocation)  // If we've changed locations, we're not the OSD anymore
			m_pOrbiter->CMD_Goto_Screen("",m_pOrbiter->m_iPK_Screen_RemoteOSD);
		else if( m_pOrbiter->m_iPK_Screen_Remote )
			m_pOrbiter->CMD_Goto_Screen("",m_pOrbiter->m_iPK_Screen_Remote);
		else
		{
			m_pOrbiter->CMD_Set_Main_Menu("N");
			m_pOrbiter->CMD_Goto_Screen("",SCREEN_Main_CONST);
		}
	}
}

// If options contains a 's', this will be selected by default
void MouseBehavior::Set_Mouse_Behavior(string sOptions,bool bExclusive,string sDirection,string sDesignObj)
{
	PLUTO_SAFETY_LOCK(mb,m_pOrbiter->m_ScreenMutex);
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_FESTIVAL,"MouseBehavior::Set_Mouse_Behavior -%s- %d -%s- -%s-",
					  sOptions.c_str(),(int) bExclusive,sDirection.c_str(),sDesignObj.c_str());
#endif
	if( sOptions.size()==0 )
	{
		Clear();
		return; 
	}

	DesignObj_Orbiter *pObj=NULL;
	if( sDesignObj.size() )
		pObj = m_pOrbiter->FindObject(sDesignObj);

	if( !pObj && sOptions[0] !='K' )  // Only the keyboard handler does not require an object
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MouseBehavior::Set_Mouse_Behavior options %s, obj %s invalid",sOptions.c_str(),sDesignObj.c_str());
		return;
	}
	
	if( m_pMouseHandler )
		delete m_pMouseHandler;
	m_pMouseHandler=NULL;

	switch(sOptions[0])
	{
	case 'A': // Auto select, means select whatever we highlight
	case 'L':
	case 'M':
		m_pMouseHandler=new LockedMouseHandler(pObj,sOptions,this);
		break;
	case 'H':
		m_pMouseHandler=new HorizMenuMouseHandler(pObj,sOptions,this);
		break;
	case 'B':
		m_pMouseHandler=new MediaBrowserMouseHandler(pObj,sOptions,this);
		break;
	case 'S':
		m_pMouseHandler=new SpeedMouseHandler(pObj,sOptions,this);
		break;
	case 'G':
		m_pMouseHandler=new LightMouseHandler(pObj,sOptions,this);
		break;
	case 'V':
		m_pMouseHandler=new VolumeMouseHandler(pObj,sOptions,this);
		break;
	case 'T':
		m_pMouseHandler=new MediaMouseHandler(pObj,sOptions,this);
		break;
	case 'E':
		m_pMouseHandler=new EPGMouseHandler(pObj,sOptions,this);
		break;
	case 'K':
		m_pMouseHandler=new KeyboardMouseHandler(pObj,sOptions,this);
		break;
	}
	m_pObj_Locked=pObj;
	if( m_pMouseHandler )
	{
		SetMouseCursorStyle(MouseBehavior::mcs_AnyDirection); //set the default to any direction and let mouse handler to change it
		m_pMouseHandler->Start();
	}
}

void MouseBehavior::Move(int X,int Y)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_FESTIVAL,"MouseBehavior::Move %d,%d",X,Y);
#endif
	PLUTO_SAFETY_LOCK(mb,m_pOrbiter->m_ScreenMutex);
	if( m_pMouseHandler )
		m_pMouseHandler->Move(X,Y,0);
}

void MouseBehavior::RelativeMove(int DeltaX,int DeltaY)
{
	PLUTO_SAFETY_LOCK(mb,m_pOrbiter->m_ScreenMutex);
	if( m_pMouseHandler )
		m_pMouseHandler->RelativeMove(DeltaX, DeltaY);
}

DesignObj_Orbiter *MouseBehavior::FindChildObjectAtPosition(DesignObj_Orbiter *pObj_Parent,int X,int Y,DesignObj_Orbiter **pObj_Furthest, int PK_Direction)
{
	if( pObj_Parent==NULL )
		pObj_Parent = m_pOrbiter->m_pScreenHistory_Current->GetObj();
	DesignObj_DataList::iterator iHao;
	for( iHao=pObj_Parent->m_ChildObjects.begin(  ); iHao != pObj_Parent->m_ChildObjects.end(  ); ++iHao )
	{
		DesignObj_Orbiter *pObj = (DesignObj_Orbiter *) *iHao;
		DesignObj_Orbiter *pObj_Child = FindChildObjectAtPosition(pObj,X,Y,pObj_Furthest, PK_Direction);
		if( pObj_Child )
			return pObj_Child;
	}
	if( pObj_Parent->m_bTabStop &&  !pObj_Parent->IsHidden())
	{
		if( Y==-1 )
		{
			if( (pObj_Parent->m_rPosition+pObj_Parent->m_pPopupPoint).X<=X && (pObj_Parent->m_rPosition+pObj_Parent->m_pPopupPoint).Right()>=X )
				return pObj_Parent;
		}
		else if( (pObj_Parent->m_rPosition+pObj_Parent->m_pPopupPoint).Contains(X,Y) )
			return pObj_Parent;
		else if( pObj_Furthest )
		{
			if( !*pObj_Furthest 
				|| ( PK_Direction==DIRECTION_Up_CONST && pObj_Parent->m_rPosition.Y<(*pObj_Furthest)->m_rPosition.Y )
				)
					*pObj_Furthest = pObj_Parent;
		}
	}
	return NULL;
}

void MouseBehavior::SetMediaInfo(string sTime,string sTotal,string sSpeed,string sTitle,string sSection,bool bHasTimeline)
{
	m_bHasTimeline = bHasTimeline;
	LoggerWrapper::GetInstance()->Write(LV_STATUS," MouseBehavior::SetMediaInfo %s %s timeline %d ptr %p type %d",
		sTotal.c_str(),sTime.c_str(),(int) m_bHasTimeline, m_pMouseHandler,
		m_pMouseHandler ? (int) m_pMouseHandler->TypeOfMouseHandler() : -1 );
	if( m_pMouseHandler && m_pMouseHandler->TypeOfMouseHandler()==MouseHandler::mh_Speed )
	{
		SpeedMouseHandler *pSpeedMouseHandler = (SpeedMouseHandler *) m_pMouseHandler;
		int Stop = pSpeedMouseHandler->ParseTime(sTotal);
		int Current = pSpeedMouseHandler->ParseTime(sTime);
		LoggerWrapper::GetInstance()->Write(LV_STATUS," MouseBehavior::SetMediaInfo 2 %s %s %d %d timeline %d ptr %p type %d",
			sTotal.c_str(),sTime.c_str(),Stop,Current,(int) m_bHasTimeline, m_pMouseHandler,
		m_pMouseHandler ? (int) m_pMouseHandler->TypeOfMouseHandler() : -1 );
		if( Stop==pSpeedMouseHandler->m_CurrentMedia_Stop && Current==pSpeedMouseHandler->m_CurrentMedia_Pos )
			return;
		pSpeedMouseHandler->m_CurrentMedia_Stop = Stop;
		pSpeedMouseHandler->m_CurrentMedia_Pos = Current;
NeedToRender render( m_pOrbiter, "start speed" );
m_pOrbiter->Renderer()->RenderObjectAsync(pSpeedMouseHandler->m_pObj);

	}
}

void MouseBehavior::MediaStopped()
{
	if(m_pMouseHandler && m_pMouseHandler->TypeOfMouseHandler() == MouseHandler::mh_HorizMenu)
	{
		HorizMenuMouseHandler *pHorizMenuMouseHandler = dynamic_cast<HorizMenuMouseHandler *>(m_pMouseHandler);
		if(NULL != pHorizMenuMouseHandler)
		{
			if(pHorizMenuMouseHandler->ActivePopupID() == m_pOrbiter->m_iPK_DesignObj_Remote_Popup)
				m_pOrbiter->CMD_Remove_Popup("", "submenu");
		}
	}
}

bool MouseBehavior::ButtonDown(int PK_Button)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_FESTIVAL,"MouseBehavior::ButtonDown %d resetting m_iTime_Last_Mouse_Up",(int) PK_Button);
#endif
	PLUTO_SAFETY_LOCK(mb,m_pOrbiter->m_ScreenMutex);
	m_iPK_Button_Mouse_Last=PK_Button;
	m_iTime_Last_Mouse_Down=ProcessUtils::GetMsTime();
	m_iTime_Last_Mouse_Up=0;
	if(!m_pOrbiter->m_pScreenHistory_Current)
		return false;
	int PK_Screen_OnScreen = m_pOrbiter->m_pScreenHistory_Current->PK_Screen();

	// Special case for the media control
	if( m_iPK_Button_Mouse_Last==BUTTON_Mouse_7_CONST && (PK_Screen_OnScreen!=SCREEN_Main_CONST || (m_pOrbiter->m_pScreenHistory_Current && m_pOrbiter->m_pScreenHistory_Current->GetObj() == m_pOrbiter->m_pDesignObj_Orbiter_ScreenSaveMenu) ) )
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_FESTIVAL,"MouseBehavior::ButtonDown showing main menu");
#endif
		if( m_pMouseHandler && m_pMouseHandler->TypeOfMouseHandler()==MouseHandler::mh_Speed )
			m_pMouseHandler->ButtonDown(PK_Button);

		NeedToRender render( m_pOrbiter, "mousebehavior" );  // Redraw anything that was changed by this command
		m_pOrbiter->CMD_Set_Main_Menu("N");
		m_pOrbiter->CMD_Goto_Screen("",SCREEN_Main_CONST);
		ShowMouse(true);
		SetMouseCursorStyle(MouseBehavior::mcs_AnyDirection);
		return true;
	}
	else if( m_iPK_Button_Mouse_Last==BUTTON_Mouse_3_CONST && !m_bMouseKilled )
	{
		PlutoRectangle r = m_rMouseConstrained;
		PlutoPoint p;
		GetMousePosition(&p);
		PlutoRectangle rp = PlutoRectangle(p.X,p.Y,1,1);
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Constraining mouse to %d,%d",p.X,p.Y);
		ConstrainMouse(rp);
		m_rMouseConstrained = r;  // Don't really update this.  We're going to use this as the 'last known valid' contrain so we can restore normal behavior
		m_bMouseKilled = true;
	}
	else if( m_iPK_Button_Mouse_Last==BUTTON_Mouse_6_CONST && PK_Screen_OnScreen!=SCREEN_mnuPlaybackControl_CONST 
		&& PK_Screen_OnScreen!=m_pOrbiter->m_iPK_Screen_OSD_Speed && PK_Screen_OnScreen!=m_pOrbiter->m_iPK_Screen_OSD_Track )
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_FESTIVAL,"MouseBehavior::ButtonDown showing media menu");
#endif
		m_pOrbiter->m_bShowingSpeedBar=false;  // We're going to explicitly display the speed bar now
		NeedToRender render( m_pOrbiter, "mousebehavior" );  // Redraw anything that was changed by this command
		if( m_pOrbiter->m_iPK_Screen_OSD_Speed && m_pOrbiter->m_iPK_Screen_OSD_Track )
			m_pOrbiter->CMD_Goto_Screen("", SCREEN_mnuPlaybackControl_CONST);
		else if( m_pOrbiter->m_iPK_Screen_OSD_Speed )
			m_pOrbiter->CMD_Goto_Screen("", m_pOrbiter->m_iPK_Screen_OSD_Speed);
		else if( m_pOrbiter->m_iPK_Screen_OSD_Track )
			m_pOrbiter->CMD_Goto_Screen("", m_pOrbiter->m_iPK_Screen_OSD_Track);
		ShowMouse(true);
		SetMouseCursorStyle(MouseBehavior::mcs_AnyDirection);
		return true;
	}
	else if( m_iPK_Button_Mouse_Last==BUTTON_Mouse_8_CONST && PK_Screen_OnScreen!=SCREEN_mnuAmbiance_CONST 
		&& PK_Screen_OnScreen!=SCREEN_mnuVolume_CONST && PK_Screen_OnScreen!=SCREEN_mnuLights_CONST )
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_FESTIVAL,"MouseBehavior::ButtonDown showing ambiance menu");
#endif

		if( m_pMouseHandler && m_pMouseHandler->TypeOfMouseHandler()==MouseHandler::mh_Speed )
			m_pMouseHandler->ButtonDown(PK_Button);

		NeedToRender render( m_pOrbiter, "mousebehavior" );  // Redraw anything that was changed by this command
		m_pOrbiter->CMD_Goto_Screen("", SCREEN_mnuAmbiance_CONST);
		ShowMouse(true);
		SetMouseCursorStyle(MouseBehavior::mcs_AnyDirection);
		return true;
	}
	else if( (m_iPK_Button_Mouse_Last==BUTTON_Mouse_6_CONST && (PK_Screen_OnScreen==SCREEN_mnuPlaybackControl_CONST || PK_Screen_OnScreen==m_pOrbiter->m_iPK_Screen_OSD_Speed || PK_Screen_OnScreen==m_pOrbiter->m_iPK_Screen_OSD_Track)) ||
		(m_iPK_Button_Mouse_Last==BUTTON_Mouse_8_CONST && (PK_Screen_OnScreen==SCREEN_mnuAmbiance_CONST || PK_Screen_OnScreen==SCREEN_mnuVolume_CONST || PK_Screen_OnScreen==SCREEN_mnuLights_CONST)) )
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_FESTIVAL,"MouseBehavior::ButtonDown removing menu");
#endif
		if(m_pMouseHandler && m_pMouseHandler->TypeOfMouseHandler()==MouseHandler::mh_Speed && m_iPK_Button_Mouse_Last==BUTTON_Mouse_6_CONST)
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "MouseBehavior::ButtonDown : Ignore all speed changes for few seconds so the player has a chance to settle down");
			m_tIgnoreSpeedChangesUntil = time(NULL) + 3;  // Ignore all speed changes for few seconds so the player has a chance to settle down
		}	

		Clear(true);
		return true;
	}
	else if( m_iPK_Button_Mouse_Last==BUTTON_Mouse_2_CONST || (m_iPK_Button_Mouse_Last==BUTTON_escape_CONST && PK_Screen_OnScreen==SCREEN_Main_CONST && m_pOrbiter->m_pScreenHistory_Current && m_pOrbiter->m_pScreenHistory_Current->GetObj() != m_pOrbiter->m_pDesignObj_Orbiter_ScreenSaveMenu) )
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_FESTIVAL,"MouseBehavior::ButtonDown right click %d %d==%d | %d",m_pOrbiter->m_iPK_MediaType,PK_Screen_OnScreen,m_pOrbiter->m_iPK_Screen_Remote,m_pOrbiter->m_iPK_Screen_RemoteOSD);
#endif
		if( PK_Screen_OnScreen && PK_Screen_OnScreen==m_pOrbiter->m_PK_Screen_ActiveApp_OSD )
		{
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_FESTIVAL,"MouseBehavior::ButtonDown right click ignoring so on screen app can get it");
#endif
			return false;
		}
		else if( !m_pOrbiter->m_iPK_MediaType && PK_Screen_OnScreen==SCREEN_Main_CONST )
		{
			// If the user hits the close button from the main menu, go to the screen saver, unless there's an active computing application
			if( m_pOrbiter->m_PK_Screen_ActiveApp_OSD )
			{
				NeedToRender render( m_pOrbiter, "m_PK_Screen_ActiveApp_OSD" );
				Clear(false);
				m_pOrbiter->CMD_Goto_Screen("",m_pOrbiter->m_PK_Screen_ActiveApp_OSD);
			}
			else
				m_pOrbiter->StartScreenSaver(true);  
		}
#ifdef WIN32
		/*  This is confusing.  Use the power button to stop media, not the right mouse button.  But for windows testing it's easiest since we don't have a power button */
		else if( m_pOrbiter->m_iPK_MediaType && (PK_Screen_OnScreen==m_pOrbiter->m_iPK_Screen_Remote || PK_Screen_OnScreen==m_pOrbiter->m_iPK_Screen_RemoteOSD) )
		{
			if( m_pMouseHandler && m_pMouseHandler->ButtonDown(PK_Button) )  // The mouse handler may want to do something with it
				return true;
			DCE::CMD_MH_Stop_Media CMD_MH_Stop_Media(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_MediaPlugIn,0,0,0,"",false);
			m_pOrbiter->SendCommand(CMD_MH_Stop_Media);
			m_pOrbiter->m_iPK_Screen_RemoteOSD=m_pOrbiter->m_iPK_Screen_Remote=0;  // So the Clear below does't try to go to the remote as the main menu
			Clear(true);
		}
#endif
		else
		{
			if( m_pMouseHandler && m_pMouseHandler->ButtonDown(PK_Button) )  // The mouse handler may want to do something with it
				return true;
			else
				Clear(true);
		}
		return true;
	}

	if( m_pMouseHandler )
		return m_pMouseHandler->ButtonDown(PK_Button);

	return false;
}

bool MouseBehavior::ButtonUp(int PK_Button)
{
    PLUTO_SAFETY_LOCK(mb,m_pOrbiter->m_ScreenMutex);
    m_iPK_Button_Mouse_Last=PK_Button;
    m_iTime_Last_Mouse_Up=ProcessUtils::GetMsTime();
#ifdef DEBUG
    LoggerWrapper::GetInstance()->Write(LV_FESTIVAL,"MouseBehavior::ButtonUp %d %p m_iTime_Last_Mouse_Up=%d",(int) PK_Button,m_pMouseHandler,(int) m_iTime_Last_Mouse_Up);
#endif

	if( m_iPK_Button_Mouse_Last==BUTTON_Mouse_3_CONST && m_bMouseKilled )
	{
		ConstrainMouse(m_rMouseConstrained);
		m_bMouseKilled = false;
	}

    if( m_pMouseHandler )
		return m_pMouseHandler->ButtonUp(PK_Button);
    return false;
}

void MouseBehavior::HighlightObject(DesignObj_Orbiter *pObj)
{
	if(NULL != pObj && pObj->IsHidden())
		return;
		
	if(m_pObj_Previously_Highlighted)
	{
		if( pObj==m_pObj_Previously_Highlighted )
			return;  // Nothing to do
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "mmm MouseBehavior::HighlightObject : UNHIGHLIGHT %s %d",
			m_pObj_Previously_Highlighted->m_ObjectID.c_str(), m_pObj_Previously_Highlighted->m_GraphicBeforeHighlight);
#endif		
		m_pObj_Previously_Highlighted->m_GraphicToDisplay_set("mbho",m_pObj_Previously_Highlighted->m_GraphicBeforeHighlight);
		m_pOrbiter->ExecuteCommandsInList( &m_pObj_Previously_Highlighted->m_Action_UnhighlightList, m_pObj_Previously_Highlighted, smHighlight, 0, 0 );
		m_pOrbiter->Renderer()->RenderObjectAsync(m_pObj_Previously_Highlighted);
	}

	m_pOrbiter->Renderer()->UnHighlightObject();

	m_pObj_Previously_Highlighted = pObj;
	m_pOrbiter->m_pObj_Highlighted_set(pObj);
	if( !pObj )
		return;

	m_pObj_Previously_Highlighted->m_GraphicBeforeHighlight = m_pObj_Previously_Highlighted->m_GraphicToDisplay;

	pObj->m_GraphicToDisplay_set("mbho2",GRAPHIC_HIGHLIGHTED,true);
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "mmm MouseBehavior::HighlightObject : HIGHLIGHT %s",
		pObj->m_ObjectID.c_str());
#endif
	m_pOrbiter->Renderer()->DoHighlightObject();
	NeedToRender render( m_pOrbiter, "start speed" );
	m_pOrbiter->Renderer()->RenderObjectAsync(m_pOrbiter->m_pObj_Highlighted);
}

PlutoRectangle MouseBehavior::GetHighlighedObjectCoordinates()
{
	if( m_pOrbiter->m_pObj_Highlighted )
	{
		if( m_pOrbiter->m_pObj_Highlighted->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST )
		{
			PlutoRectangle rect;
			m_pOrbiter->GetDataGridHighlightCellCoordinates((DesignObj_DataGrid *) m_pOrbiter->m_pObj_Highlighted,rect);

LoggerWrapper::GetInstance()->Write(LV_FESTIVAL,"MouseBehavior::GetHighlighedObjectCoordinates %d,%d - %d,%d",rect.X,rect.Y,rect.Width,rect.Height);
			return rect + m_pOrbiter->m_pObj_Highlighted->m_pPopupPoint;
		}
		else
			return m_pOrbiter->m_pObj_Highlighted->m_rPosition + m_pOrbiter->m_pObj_Highlighted->m_pPopupPoint;
	}
	return PlutoRectangle(0,0,0,0);
}

void MouseBehavior::SelectFirstObject(char cDirection,DesignObj_Orbiter *pObj_Parent)
{
	// Select the first object to highlight if we didn't already and center over it
	m_pOrbiter->m_pObj_Highlighted_set(m_pOrbiter->Renderer()->FindFirstObjectByDirection(cDirection,true,pObj_Parent,NULL));
	DesignObj_Orbiter *pObj = m_pOrbiter->m_pObj_Highlighted;
	if( pObj )
	{
		PlutoRectangle rect = GetHighlighedObjectCoordinates();
		SetMousePosition( rect.X + rect.Width/2,
			rect.Y + rect.Height/2 );
	}
}

int MouseBehavior::GetDirectionAwayFromHighlight(int X,int Y)
{
	DesignObj_Orbiter *pObj = m_pOrbiter->m_pObj_Highlighted; // Make the code look better

	PlutoRectangle plutoRectangle = GetHighlighedObjectCoordinates();
	if( plutoRectangle.Contains(X,Y) )
		return 0;  // We're still inside the object

	// If the highlighted object is a datagrid and we're totally outside the datagrid, then use the datagrid
	// as a whole for determining which side we're on, and not the individual cell, otherwise we'll always
	// think we're below a datagrid without a lot of cells even if we're to the left or right
	if( pObj->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST && !(pObj->m_rPosition+pObj->m_pPopupPoint).Contains(X,Y) )
		plutoRectangle = pObj->m_rPosition + pObj->m_pPopupPoint;

//	LoggerWrapper::GetInstance()->Write(LV_FESTIVAL,"KeyboardMouseHandler::GetDirectionAwayFromHighlight object %s at %d,%d w:%d h:%d doesn't contain %d,%d",
//		pObj->m_ObjectID.c_str(),plutoRectangle.X,plutoRectangle.Y,
//		plutoRectangle.Width,plutoRectangle.Height,X,Y);
	int FromLeft = plutoRectangle.X - X;
	int FromTop = plutoRectangle.Y - Y;
	int FromRight = X - plutoRectangle.Right();
	int FromBottom = Y - plutoRectangle.Bottom();

	if( FromLeft>0 && FromLeft>=FromTop && FromLeft>=FromBottom )
		return DIRECTION_Left_CONST;
	if( FromTop>0 && FromTop>=FromLeft && FromTop>=FromRight )
		return DIRECTION_Up_CONST;
	if( FromRight>0 && FromRight>=FromTop && FromRight>=FromBottom )
		return DIRECTION_Right_CONST;
	if( FromBottom>0 && FromBottom>=FromLeft && FromBottom>=FromRight )
		return DIRECTION_Down_CONST;

	return 0; // shouldn't happen
}

void MouseBehavior::PositionMouseAtObjectEdge(int PK_Direction)
{
	//DesignObj_Orbiter *pObj = m_pOrbiter->m_pObj_Highlighted; // Make the code look better
	PlutoRectangle rect = GetHighlighedObjectCoordinates();

	switch(PK_Direction)
	{
	case DIRECTION_Left_CONST:
		SetMousePosition( rect.Right(), 
			rect.Y + rect.Height/2 );
		return;
	case DIRECTION_Up_CONST:
		SetMousePosition( rect.X + rect.Width/2, 
			rect.Bottom());
		return;
	case DIRECTION_Right_CONST:
		SetMousePosition( rect.X, 
			rect.Y + rect.Height/2 );
		return;
	case DIRECTION_Down_CONST:
		SetMousePosition( rect.X + rect.Width/2, 
			rect.Y);
		return;
	};
}

const MouseHandler* MouseBehavior::GetMouseHandler() const
{
	return m_pMouseHandler;
}

void MouseSensitivity::SetSensitivity(int Level)
{
	switch(Level)
	{
	case 2:
		SampleInterval = 350;
		DiscardSamplesOlderThanSec = 2250; // 1500 ms
		Threshhold_1_Minimum=70;  // Minimum number of pixels to move
		Threshhold_1_Ratio=400; // Minimum ratio : 100=baseline, 200=twice as much on the directional axis as the non-directional, 500=5x as much
		Threshhold_2_Minimum=100;  // 20 pixels over 5 samples
		Threshhold_2_Ratio=300;
		Threshhold_2_Samples=4; // How many samples must be taken before the ratio/minimum's are lowered to threshhold 2
		Threshhold_3_Minimum=160;
		Threshhold_3_Ratio=200;
		Threshhold_3_Samples=4;
		IgnoreMovesLessThanThisPerSample=20;
		NumNotchesForExternalApp=12;
		break;
	case 3:
		SampleInterval = 500;
		DiscardSamplesOlderThanSec = 3000; // 1500 ms
		Threshhold_1_Minimum=80;  // Minimum number of pixels to move
		Threshhold_1_Ratio=400; // Minimum ratio : 100=baseline, 200=twice as much on the directional axis as the non-directional, 500=5x as much
		Threshhold_2_Minimum=120;  // 20 pixels over 5 samples
		Threshhold_2_Ratio=300;
		Threshhold_2_Samples=3; // How many samples must be taken before the ratio/minimum's are lowered to threshhold 2
		Threshhold_3_Minimum=200;
		Threshhold_3_Ratio=200;
		Threshhold_3_Samples=4;
		IgnoreMovesLessThanThisPerSample=30;
		NumNotchesForExternalApp=5;
		break;
	case 1:
	default:
		SampleInterval = 200; //250; // 250 ms
		DiscardSamplesOlderThanSec = 1500; // 1500 ms
		Threshhold_1_Minimum=60;  // Minimum number of pixels to move
		Threshhold_1_Ratio=400; // Minimum ratio : 100=baseline, 200=twice as much on the directional axis as the non-directional, 500=5x as much
		Threshhold_2_Minimum=80;  // 20 pixels over 5 samples
		Threshhold_2_Ratio=300;
		Threshhold_2_Samples=5; // How many samples must be taken before the ratio/minimum's are lowered to threshhold 2
		Threshhold_3_Minimum=80;
		Threshhold_3_Ratio=200;
		Threshhold_3_Samples=10;
		IgnoreMovesLessThanThisPerSample=10;//30;
		NumNotchesForExternalApp=20;
		break;
	}
}

MouseHandler::MouseHandler(DesignObj_Orbiter *pObj, string sOptions, MouseBehavior *pMouseBehavior)
{
    m_bStartedMovement=false;
    m_pObj=pObj;
    m_sOptions=sOptions;
    m_pMouseBehavior=pMouseBehavior;
    m_bLockAxis=true;
}

MouseHandler::~MouseHandler()
{
}


void MouseBehavior::CaptureRelativeMovements()
{
#if defined(POCKETFROG)
	// Todo
#elif defined(ORBITER_OPENGL)	
	// Todo
#else
	OrbiterRenderer_SDL *pRenderer = (OrbiterRenderer_SDL *)m_pOrbiter->Renderer();
	
	pRenderer->CaptureRelativeMovements();

#endif
}

void MouseBehavior::ReleaseRelativeMovements()
{
#if defined(POCKETFROG)
	// Todo
#elif defined(ORBITER_OPENGL)	
	// Todo
#else
	OrbiterRenderer_SDL *pRenderer = (OrbiterRenderer_SDL *)m_pOrbiter->Renderer();
	
	pRenderer->ReleaseRelativeMovements();
#endif

}
