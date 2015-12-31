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
#include "HorizMenuMouseHandler.h"
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
#include "pluto_main/Define_Variable.h"
#include "DataGrid.h"
#include "ScreenHistory.h"

using namespace DCE;

HorizMenuMouseHandler::HorizMenuMouseHandler(DesignObj_Orbiter *pObj,string sOptions,MouseBehavior *pMouseBehavior,bool bDeactivateWhenOffPad)
	: MouseHandler(pObj,sOptions,pMouseBehavior)
{
	m_bDeactivateWhenOffPad=bDeactivateWhenOffPad;
	m_pObj_ActiveMenuPad = NULL;
	m_pObj_ActiveSubMenu = NULL;
	DesignObj_Orbiter *pObj_First = NULL;
	if( m_pObj->m_iBaseObjectID==DESIGNOBJ_popMainMenu_CONST )
		pObj_First = m_pMouseBehavior->m_pOrbiter->FindObject(
			StringUtils::itos(atoi(m_pObj->m_ObjectID.c_str())) + ".0.0." + StringUtils::itos(DESIGNOBJ_butCurrentMedia_CONST));
	
	if( pObj_First )
	{
		m_pMouseBehavior->m_pStartMovement.X=pObj_First->m_rPosition.X + pObj_First->m_pPopupPoint.X + (pObj_First->m_rPosition.Width/2);
		m_pMouseBehavior->m_pStartMovement.Y=pObj_First->m_rPosition.Y + pObj_First->m_pPopupPoint.Y + (pObj_First->m_rPosition.Height/2);
		m_pMouseBehavior->SetMousePosition(m_pMouseBehavior->m_pStartMovement.X,m_pMouseBehavior->m_pStartMovement.Y);
		PLUTO_SAFETY_LOCK( cm, m_pMouseBehavior->m_pOrbiter->m_ScreenMutex );  // Protect the highlighed object
		m_pObj_ActiveMenuPad=pObj_First;
		m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted_set(pObj_First);
		if( m_pObj_ActiveMenuPad )
			ShowPopup(m_pObj_ActiveMenuPad);
	}
}

void HorizMenuMouseHandler::Start()
{
}

void HorizMenuMouseHandler::Stop()
{
}

DesignObj_Orbiter *HorizMenuMouseHandler::FindChildObjectWithButton(DesignObj_Orbiter *pParent, int PK_Button)
{
	if (pParent != NULL)
	{
		DesignObj_DataList::iterator iHao;
		for( iHao=pParent->m_ChildObjects.begin(  ); iHao != pParent->m_ChildObjects.end(  ); ++iHao )
		{
			DesignObj_Orbiter *pObj = (DesignObj_Orbiter *) *iHao;
			DesignObj_Orbiter *pFound = FindChildObjectWithButton(pObj, PK_Button);
			if (pFound != NULL)
				return pFound;

			if (pObj->m_iPK_Button == PK_Button)
			{
				return pObj;
			}
		}
	}
	return NULL;
}

bool HorizMenuMouseHandler::ButtonDown(int PK_Button)
{
	switch( PK_Button )
	{
	case BUTTON_Mouse_1_CONST:
		if( m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted )
		{
			m_pMouseBehavior->m_pOrbiter->CMD_Simulate_Keypress(StringUtils::ltos(BUTTON_Enter_CONST), 0,"");
			return true; // Don't process any more
		}
		else
			return false;
	case BUTTON_1_CONST:
	case BUTTON_2_CONST:
	case BUTTON_3_CONST:
	case BUTTON_4_CONST:
	case BUTTON_5_CONST:
	case BUTTON_6_CONST:
	case BUTTON_7_CONST:
	case BUTTON_8_CONST:
	case BUTTON_9_CONST:
	case BUTTON_0_CONST:
		if (m_pObj_ActiveSubMenu != NULL) {
			// Select item from submenu
			DesignObj_Orbiter *pObj = FindChildObjectWithButton(m_pObj_ActiveSubMenu, PK_Button);
			if (pObj != NULL)
			{
				m_pMouseBehavior->HighlightObject(pObj);					
				m_pMouseBehavior->m_pOrbiter->CMD_Simulate_Keypress(StringUtils::ltos(BUTTON_Enter_CONST), 0,"");
			}
			return true;
		}
		return false;
	case BUTTON_Enter_CONST:
		{
			DesignObj_Orbiter *pObj_ToHighlight = m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted;
			if(NULL != pObj_ToHighlight && pObj_ToHighlight != m_pObj_ActiveMenuPad && 
				(GetMainMenuPopup(pObj_ToHighlight) != "" || GetFileBrowserPopup(pObj_ToHighlight) != ""))
			{
				if (m_pObj_ActiveMenuPad != NULL)
					m_pObj_ActiveMenuPad->m_GraphicToDisplay_set("hmm1",GRAPHIC_NORMAL);
				ShowPopup(pObj_ToHighlight);
				m_pObj_ActiveMenuPad = pObj_ToHighlight;
			}
		}
		return false; // Keep processing
	}

	return false;
}

bool HorizMenuMouseHandler::ButtonUp(int PK_Button)
{
	if( PK_Button==BUTTON_Mouse_7_CONST && m_pMouseBehavior->m_pOrbiter->m_bExpertMode )
	{
		int Diff = ProcessUtils::GetMsTime() - m_pMouseBehavior->m_iTime_Last_Mouse_Down;
		if( !m_bStartedMovement || Diff<1000 )  // Must have started moving and held the button at least 500 ms
		{
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"HorizMenuMouseHandler::ButtonUp not activating started %d diff %d",(int) m_bStartedMovement,Diff);
#endif
			return false;
		}
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"HorizMenuMouseHandler::ButtonUp activating diff %d",Diff);
#endif
		// Treat this like the user clicked the mouse button
		Orbiter::Event event;
		event.type=Orbiter::Event::REGION_DOWN;
		event.data.region.m_iButton=1;
		event.data.region.m_iX=m_pMouseBehavior->m_pOrbiter->m_iLastX;
		event.data.region.m_iY=m_pMouseBehavior->m_pOrbiter->m_iLastY;
		m_pMouseBehavior->m_pOrbiter->ProcessEvent( event );

		return true;
	}

	switch( PK_Button )
	{
	case BUTTON_Left_Arrow_CONST:
	case BUTTON_Right_Arrow_CONST:
		if( m_pObj_ActiveMenuPad ) 
		{
			m_pObj_ActiveMenuPad->m_GraphicToDisplay_set("hmm2",GRAPHIC_NORMAL);
			// If there's a menu pad, temporarily make it highlighted so the following function can easliy calculate left/right accurately
			m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted_set(m_pObj_ActiveMenuPad);
		}
		
		// Now move
		m_pMouseBehavior->m_pOrbiter->Renderer()->HighlightNextObject( PK_Button==BUTTON_Left_Arrow_CONST ? DIRECTION_Left_CONST : DIRECTION_Right_CONST );
		if( m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted )
		{
			ShowPopup(m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted);
			m_pObj_ActiveMenuPad = m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted;
		}

		return true;
	}
	return false; // Keep going
}

void HorizMenuMouseHandler::Move(int X,int Y,int PK_Direction)
{
	PLUTO_SAFETY_LOCK( cm, m_pMouseBehavior->m_pOrbiter->m_ScreenMutex );  // Protect the highlighed object

	// Find the active menu pad and store in m_pObj_ActiveMenuPad
	if(	!m_pObj_ActiveMenuPad || X<m_pObj_ActiveMenuPad->m_rPosition.X ||
		X>m_pObj_ActiveMenuPad->m_rPosition.Right() ||
		(m_bDeactivateWhenOffPad && m_pObj_ActiveSubMenu && Y<m_pObj_ActiveSubMenu->m_rPosition.Y + m_pObj_ActiveSubMenu->m_pPopupPoint.Y) ||
		Y>m_pObj_ActiveMenuPad->m_rPosition.Y )
	{
		DesignObj_Orbiter *pObj_ToHighlight=m_pMouseBehavior->FindChildObjectAtPosition(m_pMouseBehavior->m_pObj_Locked,X,
			m_bDeactivateWhenOffPad || (m_pObj_ActiveMenuPad && Y>m_pObj_ActiveMenuPad->m_rPosition.Y) ? Y : -1);  // If m_bDeactivateWhenOffPad is true, we only activate a menu when we're on top of the pad

		// The user has moved off the highlighted object.  Find the object under here to highlight
		if( pObj_ToHighlight && pObj_ToHighlight!=m_pObj_ActiveMenuPad )
		{
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"HorizMenuMouseHandler::Move 1 m_bStartedMovement=true");
#endif
			if( m_pObj_ActiveMenuPad )
				m_pObj_ActiveMenuPad->m_GraphicToDisplay_set("hmm3",GRAPHIC_NORMAL);
			ShowPopup(pObj_ToHighlight);
			m_pObj_ActiveMenuPad=pObj_ToHighlight;
		}
		else if( !pObj_ToHighlight && m_bDeactivateWhenOffPad )
		{
			if( m_pObj_ActiveMenuPad )
			{
				m_pObj_ActiveMenuPad->m_GraphicToDisplay_set("hmm4",GRAPHIC_NORMAL);
				m_pMouseBehavior->m_pOrbiter->Renderer()->RenderObjectAsync(m_pObj_ActiveMenuPad);
			}
			m_pObj_ActiveMenuPad = NULL;
			m_pMouseBehavior->m_pOrbiter->CMD_Remove_Popup("","submenu");
			return;
		}
	}

	// Find the active menu option on this menu pad
	if( !m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted || (m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_rPosition + m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_pPopupPoint).Contains(X,Y)==false )
	{
		if( m_pObj_ActiveSubMenu && Y<m_pObj_ActiveMenuPad->m_rPosition.Y )  // Only do this if the cursor is above the menu pad
		{
//if( m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted )
//{
//PlutoRectangle r = m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_rPosition + m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_pPopupPoint;
//int k=2;
//}
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"HorizMenuMouseHandler::Move 2 m_bStartedMovement=true");
#endif
			m_bStartedMovement=true;
			DesignObj_Orbiter *pObj_ToHighlight_TopMost=NULL;
			DesignObj_Orbiter *pObj_ToHighlight=m_pMouseBehavior->FindChildObjectAtPosition(m_pObj_ActiveSubMenu,X,Y,&pObj_ToHighlight_TopMost,DIRECTION_Up_CONST);
			if( pObj_ToHighlight || pObj_ToHighlight_TopMost)
				m_pMouseBehavior->HighlightObject(pObj_ToHighlight ? pObj_ToHighlight : pObj_ToHighlight_TopMost);
//if( m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted )
//{
//PlutoRectangle r = m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_rPosition + m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_pPopupPoint;
//int k=2;
//}
		}
		else
			m_pMouseBehavior->HighlightObject(NULL); // m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted
	}
}

void HorizMenuMouseHandler::ShowPopup(DesignObj_Orbiter *pObj_MenuPad)
{
	NeedToRender render( m_pMouseBehavior->m_pOrbiter, "HorizMenuMouseHandler::ShowPopup" );

	pObj_MenuPad->m_GraphicToDisplay_set("hmm5",GRAPHIC_SELECTED);
	m_pMouseBehavior->m_pOrbiter->Renderer()->RenderObjectAsync(pObj_MenuPad);
	if( m_pObj_ActiveMenuPad && m_pObj_ActiveMenuPad!=pObj_MenuPad )
	{
		m_pObj_ActiveMenuPad->m_GraphicToDisplay_set("hmm6",GRAPHIC_NORMAL);
		m_pMouseBehavior->m_pOrbiter->Renderer()->RenderObjectAsync(m_pObj_ActiveMenuPad);
	}
	string sSubMenu;

	if( m_pObj->m_iBaseObjectID==DESIGNOBJ_popMainMenu_CONST )
		sSubMenu = GetMainMenuPopup(pObj_MenuPad);
	else if( m_pObj->m_iBaseObjectID==DESIGNOBJ_popFileList_CONST )
		sSubMenu = GetFileBrowserPopup(pObj_MenuPad);

	m_pObj_ActiveSubMenu = m_pMouseBehavior->m_pOrbiter->FindObject(sSubMenu);
	if( !m_pObj_ActiveSubMenu )
	{
		m_pMouseBehavior->m_pOrbiter->CMD_Remove_Popup("","submenu");
		return;
	}

	// We want to put it above us, left justified
	PlutoPoint pt = pObj_MenuPad->m_rPosition.Location();
	pt.Y -= m_pObj_ActiveSubMenu->m_rPosition.Height;
	if( pt.X + m_pObj_ActiveSubMenu->m_rPosition.Width > m_pMouseBehavior->m_pOrbiter->m_Width )
		pt.X = m_pMouseBehavior->m_pOrbiter->m_Width-m_pObj_ActiveSubMenu->m_rPosition.Width;

	m_pMouseBehavior->m_pOrbiter->CMD_Show_Popup(m_pObj_ActiveSubMenu->m_ObjectID,pt.X,pt.Y,"",
		"submenu",false,false);
}

string HorizMenuMouseHandler::GetFileBrowserPopup(DesignObj_Orbiter *pObj_MenuPad)
{
	switch( pObj_MenuPad->m_iBaseObjectID )
	{
	case DESIGNOBJ_butFBSF_Show_MediaType_CONST: // what
		return TOSTRING(DESIGNOBJ_popFBSF_MediaType_CONST);
	case DESIGNOBJ_butFBSF_Show_MediaPrivate_CONST:  // private
		return TOSTRING(DESIGNOBJ_popFBSF_PrivateMedia_CONST);
	case DESIGNOBJ_butFBSF_Show_MediaRating_CONST:  // rating
		return TOSTRING(DESIGNOBJ_popFBSF_RatingsByUser_CONST);
	case DESIGNOBJ_butFBSF_Show_Sort_CONST: // sort
		return TOSTRING(DESIGNOBJ_popFBSF_Sort_CONST) ".<%=" TOSTRING(VARIABLE_PK_MediaType_CONST) "%>.0";
	case DESIGNOBJ_butFBSF_Show_MediaGenre_CONST: // gemre
		return TOSTRING(DESIGNOBJ_popFBSF_Genres_CONST) ".<%=" TOSTRING(VARIABLE_PK_MediaType_CONST) "%>.0";
	case DESIGNOBJ_butFBSF_Show_MediaSubType_CONST: // sub type
		return TOSTRING(DESIGNOBJ_popFBSF_PK_MediaSubType_CONST) ".<%=" TOSTRING(VARIABLE_PK_MediaType_CONST) "%>.0";
	case DESIGNOBJ_butFBSF_Show_MediaFormat_CONST: // format
		return TOSTRING(DESIGNOBJ_popFBSF_PK_FileFormat_CONST) ".<%=" TOSTRING(VARIABLE_PK_MediaType_CONST) "%>.0";
	case DESIGNOBJ_butFBSF_Show_MediaSource_CONST: // source
		return TOSTRING(DESIGNOBJ_popFBSF_MediaSource_CONST) ".<%=" TOSTRING(VARIABLE_PK_MediaType_CONST) "%>.0";
	case DESIGNOBJ_butFBSF_Show_More_CONST: // More
		return TOSTRING(DESIGNOBJ_popFBSF_More_CONST) ".<%=" TOSTRING(VARIABLE_PK_MediaType_CONST) "%>.0";
	case DESIGNOBJ_butFBSF_Show_Media_Go_CONST: // go
		return TOSTRING(DESIGNOBJ_popFBSF_Go_CONST);
	}

	return "";
}

string HorizMenuMouseHandler::GetMainMenuPopup(DesignObj_Orbiter *pObj_MenuPad)
{
	switch( pObj_MenuPad->m_iBaseObjectID )
	{
	case DESIGNOBJ_butCurrentMedia_CONST:
		return StringUtils::itos(m_pMouseBehavior->m_pOrbiter->m_iPK_DesignObj_Remote_Popup);
	case DESIGNOBJ_butClimate2_CONST:  // climate
		return "4957.<%=L:0%>.0";
	case DESIGNOBJ_butTelecom2_CONST:  // telecom
		return "4958.<%=L:0%>.0";//"#DESIGNOBJ_icoModeBabySitter_CONST.<%=L:0%>";// "4957.<%=L:0%>";  //
	case DESIGNOBJ_butSecurity2_CONST: // security
		return "4959.<%=L:0%>.0";
	case DESIGNOBJ_butLightsPopup_CONST: // lights
		return "4889.<%=L:0%>.0";
	case DESIGNOBJ_butMediaPopup_CONST:
		return "4870.<%=L:0%>.0";
	case DESIGNOBJ_butCurrentLocation_CONST:
		return "4894";
	case DESIGNOBJ_butFloorplans_CONST:
		return TOSTRING(DESIGNOBJ_popFloorplan_CONST);
	case DESIGNOBJ_butSettings_CONST:
		return TOSTRING(DESIGNOBJ_popAdvanced_CONST);
	}

	return "";
}

int HorizMenuMouseHandler::ActivePopupID()
{
	return NULL != m_pObj_ActiveSubMenu ? m_pObj_ActiveSubMenu->m_iBaseObjectID : 0;
}

