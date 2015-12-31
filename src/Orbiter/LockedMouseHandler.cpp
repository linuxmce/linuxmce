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
#include "LockedMouseHandler.h"
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
#include "DataGrid.h"
#include "ScreenHistory.h"

using namespace DCE;

LockedMouseHandler::LockedMouseHandler(DesignObj_Orbiter *pObj,string sOptions,MouseBehavior *pMouseBehavior)
	: MouseHandler(pObj,sOptions,pMouseBehavior)
{
	m_pObj_Highlighted=NULL;
	m_bFirstTime=true;
	m_bActivatedObject = false;
	if( m_pObj->m_iBaseObjectID==DESIGNOBJ_popMainMenu_CONST )
	{
		DesignObj_Orbiter *pObj_First = m_pMouseBehavior->m_pOrbiter->FindObject(
			StringUtils::itos(atoi(m_pObj->m_ObjectID.c_str())) + ".0.0." + StringUtils::itos(DESIGNOBJ_butCurrentMedia_CONST));
		if( pObj_First )
		{
			m_pMouseBehavior->m_pStartMovement.X=pObj_First->m_rPosition.X + pObj_First->m_pPopupPoint.X + (pObj_First->m_rPosition.Width/2);
			m_pMouseBehavior->m_pStartMovement.Y=pObj_First->m_rPosition.Y + pObj_First->m_pPopupPoint.Y + (pObj_First->m_rPosition.Height/2);
			m_pMouseBehavior->SetMousePosition(m_pMouseBehavior->m_pStartMovement.X,m_pMouseBehavior->m_pStartMovement.Y);
			PLUTO_SAFETY_LOCK( cm, m_pMouseBehavior->m_pOrbiter->m_ScreenMutex );  // Protect the highlighed object
			m_pObj_Highlighted=pObj_First;
			m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted_set(pObj_First);
			m_pMouseBehavior->HighlightObject(m_pObj_Highlighted);
		}
	}
}

void LockedMouseHandler::Start()
{
	if( m_pObj )
		m_pMouseBehavior->ConstrainMouse(m_pObj->m_rPosition + m_pObj->m_pPopupPoint);
	m_PK_Direction_Last=0;
	m_bActivatedObject = false;
	if( m_bFirstTime )
		m_bFirstTime=false;
	else if( m_pObj_Highlighted )
	{
		m_pMouseBehavior->m_pStartMovement.X=m_pObj_Highlighted->m_rPosition.X + m_pObj_Highlighted->m_pPopupPoint.X + (m_pObj_Highlighted->m_rPosition.Width/2);
		m_pMouseBehavior->m_pStartMovement.Y=m_pObj_Highlighted->m_rPosition.Y + m_pObj_Highlighted->m_pPopupPoint.Y + (m_pObj_Highlighted->m_rPosition.Height/2);
		m_pMouseBehavior->SetMousePosition(m_pMouseBehavior->m_pStartMovement.X,m_pMouseBehavior->m_pStartMovement.Y);
	}
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"LockedMouseHandler::Start m_iTime_Last_Mouse_Up %d",(int) m_pMouseBehavior->m_iTime_Last_Mouse_Up);
#endif

	if( m_pMouseBehavior->m_iTime_Last_Mouse_Up )
		m_bTapAndRelease=true;
	else
		m_bTapAndRelease=false;

	if( m_sOptions[0]=='A' )
		m_pMouseBehavior->SetMousePosition(m_pMouseBehavior->m_pOrbiter->m_iImageWidth/2,m_pMouseBehavior->m_pOrbiter->m_iImageHeight/2);
}

void LockedMouseHandler::Stop()
{
	if( m_sOptions[0]=='M' && m_pMouseBehavior->m_pMouseHandler==NULL && m_pObj_Highlighted && m_bTapAndRelease==false )
	{
		ActivatedMainMenuPad();  // The user is doing a press and hold on the main menu and changing directions, activate the menu pad
	}
}

bool LockedMouseHandler::ButtonDown(int PK_Button)
{
	if( PK_Button==BUTTON_Mouse_1_CONST )
	{
		if( m_sOptions[0]=='M' )
		{
			ActivatedMainMenuPad();
			return true; // Don't process any more
		}
		else
		{
			m_bActivatedObject = true;
			if( m_sOptions=="LS" )
				ActivatedSubMenu();
			else
				m_pMouseBehavior->m_pOrbiter->CMD_Simulate_Keypress(StringUtils::ltos(BUTTON_Enter_CONST),0, "");
			return true; // Don't process any more
		}
	}

	return false; // Keep processing
}

bool LockedMouseHandler::ButtonUp(int PK_Button)
{
	if( PK_Button==BUTTON_Mouse_7_CONST && !m_bStartedMovement )
		m_bTapAndRelease=true;

	if( PK_Button==BUTTON_Mouse_7_CONST && m_bTapAndRelease==false && m_bStartedMovement )
	{
		PLUTO_SAFETY_LOCK( cm, m_pMouseBehavior->m_pOrbiter->m_ScreenMutex );  // Protect the highlighed object
		if( m_sOptions[0]=='M' )
			ActivatedMainMenuPad();
		else if( m_sOptions=="LS" )
			ActivatedSubMenu();
		else if( m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted && !m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->IsHidden(  )  )
		{
			m_bActivatedObject = true;
			m_pMouseBehavior->m_pOrbiter->SelectedObject( m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted, smNavigation );
		}
		else
			m_pMouseBehavior->Clear(true);   // ** WARNING -- this will be deleted exit immediately
		return false;
	}

	return false; // Keep processing
}

void LockedMouseHandler::Move(int X,int Y,int PK_Direction)
{
	if( m_PK_Direction_Last && PK_Direction!=m_PK_Direction_Last && m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted )
	{
		// The user is reversing direction.  Recenter so we don't switch back to easily, to give that 'pop' feel to the button
		int X = m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_rPosition.X + m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_pPopupPoint.X + m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_rPosition.Width/2;
		int Y = m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_rPosition.Y + m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_pPopupPoint.Y + m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_rPosition.Height/2;
		m_pMouseBehavior->SetMousePosition(X,Y);
		m_PK_Direction_Last=PK_Direction;
		return;
	}
	m_PK_Direction_Last=PK_Direction;
	//	LoggerWrapper::GetInstance()->Write(LV_FESTIVAL,"Move %d,%d last %d,%d start %d,%d locked axis: %d current %d pos: %d",
//X,Y,m_pMouseBehavior->m_pSamples[0].X,m_pMouseBehavior->m_pSamples[0].Y,m_pMouseBehavior->m_pStartMovement.X,m_pMouseBehavior->m_pStartMovement.Y,(int) m_pMouseBehavior->m_cLockedAxes,(int) m_pMouseBehavior->m_cLocked_Axis_Current,(int) m_pMouseBehavior->m_iLockedPosition);

	PLUTO_SAFETY_LOCK( cm, m_pMouseBehavior->m_pOrbiter->m_ScreenMutex );  // Protect the highlighed object
//	if( m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted && 
//		( (m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_rPosition+m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_pPopupPoint).Contains(X,Y)==false || m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_ChildObjects.size()) )
	if( !m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted || 
		(m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_rPosition+m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_pPopupPoint).Contains(X,Y)==false )
	{
		DesignObj_Orbiter *pObj_ToHighlight=m_pMouseBehavior->FindChildObjectAtPosition(m_pMouseBehavior->m_pObj_Locked,X,Y);
		// The user has moved off the highlighted object.  Find the object under here to highlight

		if( pObj_ToHighlight && pObj_ToHighlight!=m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted )
		{
			if( m_sOptions[0]=='A' )
			{
				NeedToRender render( m_pMouseBehavior->m_pOrbiter, "LockedMouseHandler::Move" );
				m_bActivatedObject = true;
				m_pMouseBehavior->m_pOrbiter->SelectedObject( pObj_ToHighlight, smNavigation );
			}
			else
			{
				m_pObj_Highlighted = pObj_ToHighlight;
				m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted_set(m_pObj_Highlighted);
				m_pMouseBehavior->HighlightObject(m_pObj_Highlighted);
			}
		}
	}
}

void LockedMouseHandler::Notch(int PK_Direction,int iRepeat)
{
	return;
	if( m_pObj->m_iBaseObjectID!=DESIGNOBJ_popMainMenu_CONST )
		return; 
NeedToRender render( m_pMouseBehavior->m_pOrbiter, "LockedMouseHandler::Notch" );
	PLUTO_SAFETY_LOCK( cm, m_pMouseBehavior->m_pOrbiter->m_ScreenMutex );  // Protect the highlighed object
	for(int i=0;i<iRepeat;++i)
		m_pMouseBehavior->m_pOrbiter->Renderer()->HighlightNextObject(PK_Direction);
	m_pMouseBehavior->SetMousePosition(m_pMouseBehavior->m_pStartMovement.X,m_pMouseBehavior->m_pStartMovement.Y); // So we don't hit limits too fast
}

void LockedMouseHandler::ActivatedMainMenuPad()
{
	if( !m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted )
		return;

	string sSubMenu;
	switch( m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_iBaseObjectID )
	{
	case DESIGNOBJ_butCurrentMedia_CONST:
		sSubMenu = StringUtils::itos(m_pMouseBehavior->m_pOrbiter->m_iPK_DesignObj_Remote_Popup);
		break;
	case 4954:  // climate
		sSubMenu = "4957.<%=L:0%>.0";
		break;
	case 4955:  // telecom
		sSubMenu = "4958.<%=L:0%>.0";//"#DESIGNOBJ_icoModeBabySitter_CONST.<%=L:0%>";// "4957.<%=L:0%>";  //
		break;
	case 4956: // security
		sSubMenu = "4959.<%=L:0%>.0";
		break;
	case DESIGNOBJ_butLightsPopup_CONST: // lights
		sSubMenu = "4889.<%=L:0%>.0";
		break;
	case DESIGNOBJ_butMediaPopup_CONST:
		sSubMenu = "4870.<%=L:0%>.0";
		break;
	case DESIGNOBJ_butCurrentLocation_CONST:
		sSubMenu = "4894";
		break;
	case DESIGNOBJ_butFloorplans_CONST:
		sSubMenu = "4960";
		break;
	case DESIGNOBJ_butSettings_CONST:
		sSubMenu = "4961";
		break;
	}

	DesignObj_Orbiter *pObj = m_pMouseBehavior->m_pOrbiter->FindObject(sSubMenu);
	if( !pObj )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"LockedMouseHandler::ActivatedMainMenuPad cannot find %s",sSubMenu.c_str());
		return;
	}

	// We want to put it above us, left justified
	PlutoPoint pt = m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_rPosition.Location();
	pt.Y -= pObj->m_rPosition.Height;
	if( pt.X + pObj->m_rPosition.Width > m_pMouseBehavior->m_pOrbiter->m_Width )
		pt.X = m_pMouseBehavior->m_pOrbiter->m_Width-pObj->m_rPosition.Width;

	m_pMouseBehavior->m_pOrbiter->CMD_Show_Popup(pObj->m_ObjectID,pt.X,pt.Y,"","submenu",false,false);
	m_pMouseBehavior->SelectFirstObject('3',pObj);
	m_pMouseBehavior->Set_Mouse_Behavior("LS",true,"Y",pObj->m_ObjectID);
}

bool LockedMouseHandler::SlowDrift(int &X,int &Y)
{
	return false;
}

bool LockedMouseHandler::MovedOutside(int PK_Direction)
{
	return false;
}

void LockedMouseHandler::ActivatedSubMenu()
{
	NeedToRender render( m_pMouseBehavior->m_pOrbiter, "LockedMouseHandler::ActivatedSubMenu" );
	// This is weird.  If we do a clear, we become deleted and invalid.  If we change screens, and that causes
	// another set mouse behavior, we may also be deleted and become invalid.  So set the horizontal handler
	// to NULL so we won't be deleted, but then delete ourselves when we're done
	m_pMouseBehavior->m_pMouseHandler=NULL;

	DesignObj_Orbiter *pObj_Screen_Before = m_pMouseBehavior->m_pOrbiter->m_pScreenHistory_Current->GetObj();
	if( m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted && !m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->IsHidden(  )  )
	{
		m_bActivatedObject = true;
		m_pMouseBehavior->m_pOrbiter->SelectedObject( m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted, smNavigation );
	}
	if( pObj_Screen_Before==m_pMouseBehavior->m_pOrbiter->m_pScreenHistory_Current->GetObj() && render.m_pScreenHistory_get()==NULL )  // See if the selection commands didn't already result in a change screen
	{
		m_pMouseBehavior->m_pOrbiter->CMD_Goto_Screen("",SCREEN_Main_CONST);
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"LockedMouseHandler::ActivatedSubMenu setting to main because it stayed at %s",
			pObj_Screen_Before->m_ObjectID.c_str());
	}
	else
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"LockedMouseHandler::ActivatedSubMenu already changed from %s to %s",
			pObj_Screen_Before->m_ObjectID.c_str(),m_pMouseBehavior->m_pOrbiter->m_pScreenHistory_Current->GetObj()->m_ObjectID.c_str());
	delete this;  // See above notes
}
