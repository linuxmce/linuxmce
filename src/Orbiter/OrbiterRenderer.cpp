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
#include "OrbiterRenderer.h"
#include "Simulator.h"
#include "ScreenHistory.h"
#include "ScreenHandler.h"
#include "MouseBehavior.h"
#include "DataGrid.h"
#include "OrbiterRendererFactory.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "DataGridRenderer.h"
#include "../pluto_main/Define_VertAlignment.h" 
#include "../pluto_main/Define_HorizAlignment.h" 
#include "../pluto_main/Define_Button.h"

#ifdef USE_UIWINDOW
	#include "../Win32/UIWindowManager.h"
#endif

#include "math.h"

#define M_PI   3.14159265358979323846

#include <memory>
//-----------------------------------------------------------------------------------------------------
using namespace std;
using namespace DCE;
//-----------------------------------------------------------------------------------------------------
#include "ProgressBarConfig.h"
//-----------------------------------------------------------------------------------------------------
namespace ProgressBar
{
	PlutoColor TextColor(0xDD, 0xDD, 0xDD);
	PlutoColor TextShadowColor = PlutoColor::Black();
	PlutoColor BorderColor(0xB5, 0xB5, 0xB5);
	PlutoColor FillBackgroundColor(0x66, 0x66, 0x66);
	PlutoColor ProgressStartColor(0x10, 0x76, 0xE4);
	PlutoColor ProgressEndColor(0x3D, 0xB2, 0x14);
};
using namespace ProgressBar;
//-----------------------------------------------------------------------------------------------------
OrbiterRenderer::OrbiterRenderer(Orbiter *pOrbiter) : 
	m_NeedRedrawVarMutex("need redraw variables"), m_pOrbiter(pOrbiter)
{
	m_NeedRedrawVarMutex.Init(NULL);
	ConfigureDefaultTextStyle();
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ OrbiterRenderer::~OrbiterRenderer()
{
	PLUTO_SAFETY_LOCK( nd, m_NeedRedrawVarMutex );
	m_vectTexts_NeedRedraw.clear();
	m_vectObjs_NeedRedraw.clear();

	PLUTO_SAFETY_LOCK(vm, m_pOrbiter->m_VariableMutex);
	for(list<BackgroundImage *>::iterator it = m_listBackgroundImage.begin(), 
		end = m_listBackgroundImage.end(); it != end; ++it)
	{
		BackgroundImage *pBackgroundImage = *it;
		delete pBackgroundImage;
	}
	m_listBackgroundImage.clear();
	vm.Release();

	nd.Release();

	pthread_mutex_destroy(&m_NeedRedrawVarMutex.mutex);
}
//-----------------------------------------------------------------------------------------------------
Orbiter *OrbiterRenderer::OrbiterLogic()
{
	return m_pOrbiter;
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer::ConfigureDefaultTextStyle()
{
	//used to render some text
	m_spTextStyle.reset(new TextStyle());
	m_spTextStyle->m_bBold = true;
	m_spTextStyle->m_bItalic = false;
	m_spTextStyle->m_bUnderline = false;
	m_spTextStyle->m_ForeColor = PlutoColor::Black();
	m_spTextStyle->m_sFont = "Arial";
	m_spTextStyle->m_iPixelHeight = 15;
	m_spTextStyle->m_iPK_VertAlignment = VERTALIGNMENT_Middle_CONST;
	m_spTextStyle->m_iPK_HorizAlignment = HORIZALIGNMENT_Center_CONST;
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer::ClipRectangle(int& x, int& y, int& width, int& height)
{
	PlutoRectangle rect(x, y, width, height);
	ClipRectangle(rect);

	x = rect.X;
	y = rect.Y;
	width = rect.Width;
	height = rect.Height;
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer::ClipRectangle(PlutoRectangle &rect)
{
	if( rect.X >= OrbiterLogic()->m_iImageWidth )
		rect.X = OrbiterLogic()->m_iImageWidth-1;
	if( rect.X <= 0)
		rect.X = 0;
	if( rect.Y <= 0)
		rect.Y = 0;
	if( rect.Y >= OrbiterLogic()->m_iImageHeight )
		rect.Y = OrbiterLogic()->m_iImageHeight-1;

	if(rect.Width <= 0)
		rect.Width = 1;

	if(rect.Height <= 0)
		rect.Height = 1;

	if(rect.X + rect.Width >= OrbiterLogic()->m_iImageWidth && rect.Width > 0)
		rect.Width = OrbiterLogic()->m_iImageWidth - rect.X - 1;

	if(rect.Y + rect.Height >= OrbiterLogic()->m_iImageHeight && rect.Height > 0)
		rect.Height = OrbiterLogic()->m_iImageHeight - rect.Y - 1;
}

//-----------------------------------------------------------------------------------------------------
/*virtual*/ int OrbiterRenderer::PromptUser(string sPrompt,int iTimeoutSeconds,map<int,string> *p_mapPrompts)
{
#ifndef WIN32
	map<int,int> mapResponse;
	cout << sPrompt << endl;
	int iCount=1;

	if( p_mapPrompts )
	{
		for(map<int,string>::iterator it=p_mapPrompts->begin();it!=p_mapPrompts->end();++it)
		{
			mapResponse[iCount]=it->first;
			cout << StringUtils::itos(iCount++) + ". " + it->second << endl;
		}

		cout << endl
			<< "Please choose: ";
	}
	else
		cout << "Press Enter to continue";

	int iResponse=0;
	while(true)
	{
		cin >> iResponse;
		if( !p_mapPrompts )
			return 0;
		if( iResponse>0 && iResponse<=int(mapResponse.size()) )
			return mapResponse[iResponse];
		cout << endl << "Invalid.  Please try again: ";
	}
#endif

	return PROMPT_CANCEL;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ int OrbiterRenderer::PromptFor(string sToken)
{
	Event_Impl event_Impl(DEVICEID_MESSAGESEND, 0, OrbiterLogic()->m_sIPAddress);

	string sResults;
	DCE::CMD_Get_Orbiter_Options_DT CMD_Get_Orbiter_Options_DT(OrbiterLogic()->m_dwPK_Device, DEVICETEMPLATE_Orbiter_Plugin_CONST, BL_SameHouse,
		sToken,&sResults);

	CMD_Get_Orbiter_Options_DT.m_pMessage->m_eExpectedResponse = ER_ReplyMessage;
	Message *pResponse = event_Impl.SendReceiveMessage( CMD_Get_Orbiter_Options_DT.m_pMessage );
	if( !pResponse || pResponse->m_dwID != 0 )
	{
		if(pResponse)
			delete pResponse;

		PromptUser("Sorry.  There is a problem getting the list of " + sToken);
		return 0;
	}
	CMD_Get_Orbiter_Options_DT.ParseResponse( pResponse );
	delete pResponse;

	map<int,string> mapResponse;
	vector<string> Choices;
	StringUtils::Tokenize(sResults,"\n",Choices);
	if( sToken=="Size" )
		mapResponse[0]="Default";

	for(vector<string>::iterator it = Choices.begin(); it != Choices.end(); ++it)
	{
		string sChoise = *it;
		string::size_type pos=0;
		int Choice = atoi(StringUtils::Tokenize(sChoise,"\t",pos).c_str());
		string sDescription = StringUtils::Tokenize(sChoise,"\t",pos);

		if( Choice && sDescription.size() )
			mapResponse[Choice]=sDescription;
	}

	if(Simulator::GetInstance()->PreseededInstallValueDefined(sToken))
	{
		int nValue = Simulator::GetInstance()->GetPreseededInstallValue(sToken);

		if(mapResponse.find(nValue) != mapResponse.end())
            return nValue;
		else if(mapResponse.size())
			return mapResponse.begin()->first;
	}

	return PromptUser("Please select the " + sToken,0,&mapResponse);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ bool OrbiterRenderer::DisplayProgress(string sMessage, const map<string, bool> &mapChildDevices, int nProgress)
{
	return DisplayProgress(sMessage, nProgress);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer::RenderScreen(bool bRenderGraphicsOnly)
{
	if(!OrbiterLogic()->m_pScreenHistory_Current || !OrbiterLogic()->m_pScreenHistory_Current->GetObj())
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Got attempt to render null screen");
		return;
	}

	if(!bRenderGraphicsOnly)
	{
		SETUP_SCREEN_HANDLER_CALLBACK(OrbiterLogic()->m_pScreenHandler, cbOnRenderScreen, RenderScreenCallBackData, 
			(OrbiterLogic()->m_pScreenHistory_Current->PK_Screen(), OrbiterLogic()->m_pScreenHistory_Current->GetObj()))
		OrbiterLogic()->ExecuteScreenHandlerCallback(cbOnRenderScreen);
	}

#ifndef WINCE
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "OrbiterRenderer::RenderScreen Render screen: %s", OrbiterLogic()->m_pScreenHistory_Current->GetObj()->m_ObjectID.c_str(  ) );
#endif

#if ( defined( PROFILING ) )
	clock_t clkStart = clock();
#endif
	PLUTO_SAFETY_LOCK( cm, OrbiterLogic()->m_ScreenMutex );

	if (NULL != OrbiterLogic()->m_pBackgroundImage)
	{
#ifdef USE_UIWINDOW
		if(UIWindowManager::Type() == uwtNormal)
#endif
		{
			string sObjectID = "background-image-test";
			RenderGraphic(OrbiterLogic()->m_pBackgroundImage, PlutoRectangle(0, 0, OrbiterLogic()->m_iImageWidth, OrbiterLogic()->m_iImageHeight), false, 
				PlutoPoint(), 255, "", sObjectID, sObjectID);
		}
	}

	/*
	ab 12/26/06 -- when there's a popup on screen (like the alert), OrbiterRenderer::RefreshScreen calls this function.
	The 2 following lines cause the highlighted object to keep getting reset and becoming unavailable so you can't navigate
	the system using arrows when there are popup messages on the screen.  Moving these to NeedToChangeScreens and instead
	only setting them to NULL if the objects are off screen or hidden
	OrbiterLogic()->m_pObj_Highlighted_Last = NULL;
	OrbiterLogic()->m_pObj_Highlighted_set(NULL);
	*/

	if( OrbiterLogic()->m_pObj_Highlighted_Last && (OrbiterLogic()->m_pObj_Highlighted_Last->m_bOnScreen==false || OrbiterLogic()->m_pObj_Highlighted_Last->m_bHidden==true) )
		OrbiterLogic()->m_pObj_Highlighted_Last = NULL;
	if( OrbiterLogic()->m_pObj_Highlighted && (OrbiterLogic()->m_pObj_Highlighted->m_bOnScreen==false || OrbiterLogic()->m_pObj_Highlighted->m_bHidden==true) )
		OrbiterLogic()->m_pObj_Highlighted_set(NULL);

	if ( OrbiterLogic()->m_pScreenHistory_Current  )
	{
		OrbiterLogic()->m_pScreenHistory_Current->GetObj()->RenderObject(OrbiterLogic()->m_pScreenHistory_Current->GetObj());
	}

	for(list<class PlutoPopup*>::iterator it=OrbiterLogic()->m_listPopups.begin();it!=OrbiterLogic()->m_listPopups.end();++it)
		HandleShowPopup(*it, (*it)->m_Position, 1);

	if( OrbiterLogic()->m_pScreenHistory_Current  )
	{
		for(list<class PlutoPopup*>::iterator it=OrbiterLogic()->m_pScreenHistory_Current->GetObj()->m_listPopups.begin();
			it!=OrbiterLogic()->m_pScreenHistory_Current->GetObj()->m_listPopups.end();++it)
			HandleShowPopup(*it, (*it)->m_Position, 1);
	}

	cm.Release(  );
#if ( defined( PROFILING ) )
	clock_t clkFinished = clock();
	if(  OrbiterLogic()->m_pScreenHistory_Current   )
	{
		LoggerWrapper::GetInstance()->Write( LV_CONTROLLER, "Render screen: %s took %d ms",
			OrbiterLogic()->m_pScreenHistory_Current->m_pObj->m_ObjectID.c_str(  ), clkFinished-clkStart );
	}
#endif

	ScreenRendered();

	OrbiterLogic()->CallMaintenanceInMiliseconds(0, &Orbiter::ServiceAlerts, NULL, pe_ALL, false);	

	if( bRenderGraphicsOnly )
		return;

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Render screen: %s finished", OrbiterLogic()->m_pScreenHistory_Current->GetObj()->m_ObjectID.c_str(  ) );
#endif
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer::ShowProgress(int nPercent)
{
	static int nCount = 0;
	if(++nCount % PROGRESS_UPDATE_STEP)
		return;

	RenderProgressBar(nPercent);
	UpdateScreen();
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer::RenderProgressBar(int nPercent)
{
	int nProgressWidth = OrbiterLogic()->m_iImageWidth < PROGRESS_OPTIMAL_SIZE ? OrbiterLogic()->m_iImageWidth - PROGRESS_MAX : PROGRESS_OPTIMAL_SIZE;
	PlutoRectangle rect(
			(OrbiterLogic()->m_iImageWidth - nProgressWidth) / 2, OrbiterLogic()->m_iImageHeight - PROGRESS_BOTTOM_DISTANCE,
			nProgressWidth, PROGRESS_HEIGHT
			);

#ifndef ORBITER_OPENGL
	// this rectangle hides the progress bar in OpenGL for some reason
	SolidRectangle(0, 0, OrbiterLogic()->m_iImageWidth, OrbiterLogic()->m_iImageHeight, PlutoColor::Black());
#endif
	SolidRectangle(rect.Left(), rect.Top(), rect.Width, rect.Height, BorderColor);
	SolidRectangle(rect.Left() + 1, rect.Top() + 1, rect.Width - 3, rect.Height - 3, FillBackgroundColor);
	SolidRectangle(rect.Left() + 1, rect.Top() + 1, rect.Width * nPercent / PROGRESS_MAX - 3, rect.Height - 3,
			PlutoColor(
				ProgressStartColor.R() + nPercent * (ProgressEndColor.R() - ProgressStartColor.R()) / PROGRESS_MAX,
				ProgressStartColor.G() + nPercent * (ProgressEndColor.G() - ProgressStartColor.G()) / PROGRESS_MAX,
				ProgressStartColor.B() + nPercent * (ProgressEndColor.B() - ProgressStartColor.B()) / PROGRESS_MAX
				)
		      );

	DesignObjText text;
	string sText = StringUtils::ltos(nPercent) + "%";

	BatchedTextRendering(false);

	m_spTextStyle->m_ForeColor = TextShadowColor;
	text.m_rPosition = PlutoRectangle(rect.Left() + 1, rect.Top() + 1, rect.Width, rect.Height);

#ifndef ARMV4I
	RenderText(sText, &text, m_spTextStyle.get());
#endif

	m_spTextStyle->m_ForeColor = TextColor;
	text.m_rPosition = rect;

#ifndef ARMV4I
	RenderText(sText, &text, m_spTextStyle.get());
#endif

	BatchedTextRendering(true);
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer::UpdateScreen()
{
	PlutoRectangle rect(0, 0, OrbiterLogic()->m_iImageWidth, OrbiterLogic()->m_iImageHeight);
	UpdateRect(rect);
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer::OnReload()
{
	OnQuit();
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer::DoHighlightObject()
{
	PLUTO_SAFETY_LOCK( cm, OrbiterLogic()->m_ScreenMutex );  // Protect the highlighed object
	if(sbNoSelection == OrbiterLogic()->m_nSelectionBehaviour || !OrbiterLogic()->m_pObj_Highlighted || !OrbiterLogic()->m_pObj_Highlighted->m_bOnScreen )
		return;

	UnHighlightObject();

	if( !OrbiterLogic()->m_pObj_Highlighted )
		return;

	SETUP_SCREEN_HANDLER_CALLBACK(m_pOrbiter->m_pScreenHandler, cbObjectHighlighted, ObjectInfoBackData, 
		(OrbiterLogic()->m_pObj_Highlighted, 0, 0, OrbiterLogic()->m_pObj_Highlighted->m_iBaseObjectID))
	if(m_pOrbiter->ExecuteScreenHandlerCallback(cbObjectHighlighted))
		return;

	OrbiterLogic()->ExecuteCommandsInList( &OrbiterLogic()->m_pObj_Highlighted->m_Action_HighlightList, 
		OrbiterLogic()->m_pObj_Highlighted, smHighlight, 0, 0 );

	if( OrbiterLogic()->m_pObj_Highlighted->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST )
		OrbiterLogic()->GetDataGridHighlightCellCoordinates((DesignObj_DataGrid *) OrbiterLogic()->m_pObj_Highlighted,OrbiterLogic()->m_rectLastHighlight);
	else
		OrbiterLogic()->m_rectLastHighlight = OrbiterLogic()->m_pObj_Highlighted->GetHighlightRegion();

	OrbiterLogic()->m_rectLastHighlight.X += OrbiterLogic()->m_pObj_Highlighted->m_pPopupPoint.X;
	OrbiterLogic()->m_rectLastHighlight.Y += OrbiterLogic()->m_pObj_Highlighted->m_pPopupPoint.Y;

	OrbiterLogic()->m_rectLastHighlight.Width++;  // GetBackground always seems to be 1 pixel to little
	OrbiterLogic()->m_rectLastHighlight.Height++;

	OrbiterLogic()->m_pGraphicBeforeHighlight = 
		OrbiterLogic()->m_pOrbiterRenderer->GetBackground(OrbiterLogic()->m_rectLastHighlight);

	PlutoGraphic *pPlutoGraphic = OrbiterLogic()->m_pGraphicBeforeHighlight->GetHighlightedVersion();
	if(pPlutoGraphic)
	{
		OrbiterLogic()->m_pOrbiterRenderer->RenderGraphic(pPlutoGraphic, OrbiterLogic()->m_rectLastHighlight);
		RELEASE_GRAPHIC(pPlutoGraphic);
	}

	for(int i = 0; i < 4; i++)
        OrbiterLogic()->m_pOrbiterRenderer->HollowRectangle(
			OrbiterLogic()->m_rectLastHighlight.X + i, OrbiterLogic()->m_rectLastHighlight.Y + i,
			OrbiterLogic()->m_rectLastHighlight.Width - 2 * i - 2, OrbiterLogic()->m_rectLastHighlight.Height - 2 * i - 2,
			i < 2 ? PlutoColor::Red() : PlutoColor::White()
		);

	UpdateRect(OrbiterLogic()->m_rectLastHighlight);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer::UnHighlightObject(bool bDeleteOnly)
{
	PLUTO_SAFETY_LOCK( cm, OrbiterLogic()->m_ScreenMutex );  // Protect the highlighed object

	if( !OrbiterLogic()->m_pGraphicBeforeHighlight )
		return;

	if( !bDeleteOnly && OrbiterLogic()->m_pObj_Highlighted)
	{
		RenderGraphic(OrbiterLogic()->m_pGraphicBeforeHighlight, OrbiterLogic()->m_rectLastHighlight);
		UpdateRect(OrbiterLogic()->m_rectLastHighlight);
	}

	RELEASE_GRAPHIC(OrbiterLogic()->m_pGraphicBeforeHighlight);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer::HighlightFirstObject()
{
	PLUTO_SAFETY_LOCK( cm, OrbiterLogic()->m_ScreenMutex );  // Protect the highlighed object
	OrbiterLogic()->m_pObj_Highlighted_set(FindFirstObjectByDirection('1',true,NULL,NULL));
}
//-----------------------------------------------------------------------------------------------------
DesignObj_Orbiter *OrbiterRenderer::FindFirstObjectByDirection(char cDirection /* u,d,l,r,1 (ul),2 (ur),3(dl),4(dr) */,bool bPreferGrid,DesignObj_Orbiter *pObj_Parent,DesignObj_Orbiter *pObj_RelativeTo)
{
	int Position=-1,PositionGrid=-1;  // The X+Y coordinates, we're looking for the upper left-most item, so we just add
	DesignObj_Orbiter *pObj=NULL,*pObjGrid=NULL;  // We give preference to datagrids, so we track them separately
	for(size_t s=0;s<OrbiterLogic()->m_vectObjs_TabStops.size();++s)
	{
		DesignObj_Orbiter *p = OrbiterLogic()->m_vectObjs_TabStops[s];
		if( p->IsHidden() || !p->m_bOnScreen || (pObj_Parent && !p->ChildOf(pObj_Parent)) )
			continue;
		if( p->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST && (!((DesignObj_DataGrid *)p)->DataGridTable_Get() || !((DesignObj_DataGrid *)p)->DataGridTable_Get()->m_RowCount))
			continue;

		int OldPositionGrid = PositionGrid;
		int ThisPosition;
		switch(cDirection)
		{
		case 'u':
			if( pObj_RelativeTo && p->m_rPosition.Y>pObj_RelativeTo->m_rPosition.Y )
				continue;
			ThisPosition = p->m_rPosition.Y;
			break;
		case 'd':
			if( pObj_RelativeTo && p->m_rPosition.Bottom()>pObj_RelativeTo->m_rPosition.Bottom() )
				continue;
			ThisPosition = OrbiterLogic()->m_Height - p->m_rPosition.Bottom();
			break;
		case 'l':
			if( pObj_RelativeTo && p->m_rPosition.X>pObj_RelativeTo->m_rPosition.X )
				continue;
			ThisPosition = p->m_rPosition.X;
			break;
		case 'r':
			if( pObj_RelativeTo && p->m_rPosition.Right()>pObj_RelativeTo->m_rPosition.Right() )
				continue;
			ThisPosition = OrbiterLogic()->m_Width - p->m_rPosition.Right();
			break;
		case '1':
			ThisPosition = p->m_rPosition.X + p->m_rPosition.Y;
			break;
		case '2':
			ThisPosition = OrbiterLogic()->m_Width - p->m_rPosition.Right() + p->m_rPosition.Y;
			break;
		case '3':
			ThisPosition = p->m_rPosition.X + OrbiterLogic()->m_Height - p->m_rPosition.Bottom();
			break;
		case '4':
			ThisPosition = OrbiterLogic()->m_Width - p->m_rPosition.Right() + OrbiterLogic()->m_Height - p->m_rPosition.Bottom();
			break;
		default:
			return NULL; // shouldn't happen
		}
		if( bPreferGrid && p->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST )
		{
			DesignObj_DataGrid *pDesignObj_DataGrid = (DesignObj_DataGrid *) p;
			if( ThisPosition<PositionGrid || PositionGrid==-1 )
			{
				if(!pDesignObj_DataGrid->DataGridTable_Get() || !pDesignObj_DataGrid->DataGridTable_Get()->m_RowCount)
					PositionGrid = OldPositionGrid;
				else
				{
					PositionGrid=ThisPosition;
					pObjGrid=p;
				}
			}
		}
		else
		{
			if( ThisPosition<Position || Position==-1 )
			{
				Position=ThisPosition;
				pObj=p;
			}
		}
	}

	if( pObjGrid )
	{
		DesignObj_DataGrid *pDesignObj_DataGrid = (DesignObj_DataGrid *) pObjGrid;
		// If nothing is highlighted in this grid, start with the first row/column
		if( pDesignObj_DataGrid->m_iHighlightedColumn==-1 && pDesignObj_DataGrid->m_iHighlightedRow==-1 )
		{
			if(  pDesignObj_DataGrid->m_sExtraInfo.find( 'C' )==string::npos )
				pDesignObj_DataGrid->m_iHighlightedRow=0;
			if(  pDesignObj_DataGrid->m_sExtraInfo.find( 'R' )==string::npos )
				pDesignObj_DataGrid->m_iHighlightedColumn=0;
		}
		return pObjGrid;
	}
	//else
	return pObj;
	//return NULL; // warning: will never be executed
}
//-----------------------------------------------------------------------------------------------------
DesignObj_Orbiter *OrbiterRenderer::FindObjectToHighlight( DesignObj_Orbiter *pObjCurrent, int PK_Direction )
{
	// Primary means an object primarily in that direction, Secondary means primarily in another direction
	// if an object is 5 pixels to the right, and 10 pixels below, it's primary direction is right, secondary is down
	// pObj_Primary will be the closes object primarily in the direction we want, pObj_Secondary will be a
	// backup choice if there are no objects in that primary direction
	DesignObj_Orbiter *pObj_Primary=NULL,*pObj_Secondary=NULL;
	int Distance_Primary=-1,Distance_Secondary=-1;
	for(size_t s=0;s<OrbiterLogic()->m_vectObjs_TabStops.size();++s)
	{
		DesignObj_Orbiter *p = OrbiterLogic()->m_vectObjs_TabStops[s];
		if( p==pObjCurrent || p->IsHidden() || !p->m_bOnScreen )
			continue;

		if( p->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST && (!((DesignObj_DataGrid *)p)->DataGridTable_Get() || !((DesignObj_DataGrid *)p)->DataGridTable_Get()->m_RowCount))
			continue;

		PlutoRectangle rectCurrent=pObjCurrent->m_rPosition+pObjCurrent->m_pPopupPoint;
		PlutoRectangle rect=p->m_rPosition+p->m_pPopupPoint;

		bool bSkip=false;
		list<class PlutoPopup*>::reverse_iterator it;
		for(it=OrbiterLogic()->m_pScreenHistory_Current->GetObj()->m_listPopups.rbegin();it!=OrbiterLogic()->m_pScreenHistory_Current->GetObj()->m_listPopups.rend();++it)
		{
			PlutoPopup *pPopup = *it;
			if( pPopup->m_sName=="popup_alert" )
				continue;  // Don't block selecting of items under the popup alert
			if( (pPopup->m_pObj->m_rPosition+pPopup->m_pObj->m_pPopupPoint).IntersectsWith(rect) && p->TopMostObject()!=pPopup->m_pObj )
			{
				bSkip=true;
				break;
			}
		}

		if( bSkip )
			continue;

		for(it=OrbiterLogic()->m_listPopups.rbegin();it!=OrbiterLogic()->m_listPopups.rend();++it)
		{
			PlutoPopup *pPopup = *it;
			if( pPopup->m_sName=="popup_alert" )
				continue;  // Don't block selecting of items under the popup alert
			if( (pPopup->m_pObj->m_rPosition+pPopup->m_pObj->m_pPopupPoint).IntersectsWith(rect) && p->TopMostObject()!=pPopup->m_pObj )
			{
				bSkip=true;
				break;
			}
		}

		if( bSkip )
			continue;

		// It looks funny if you move down and select an object that may have a center point lower, but the top is higher
		if( (PK_Direction==DIRECTION_Down_CONST && rect.Top()<=rectCurrent.Top()) ||
			(PK_Direction==DIRECTION_Up_CONST && rect.Top()>=rectCurrent.Top()) ||
			(PK_Direction==DIRECTION_Right_CONST && rect.Left()<=rectCurrent.Left()) ||
			(PK_Direction==DIRECTION_Left_CONST && rect.Left()>=rectCurrent.Left()) )
			continue;

		int Direction_Primary = 0,Direction_Secondary = 0,Distance = 0;

		PlutoPoint CandidateObjectPosition = p->m_pMidPoint + p->m_pPopupPoint;
		PlutoPoint CurrentObjectPosition = pObjCurrent->m_pMidPoint + pObjCurrent->m_pPopupPoint;
		//CurrentObjectPosition.RelativePosition(CandidateObjectPosition, Direction_Primary, Direction_Secondary, Distance);
		RelativePosition(pObjCurrent->m_rPosition + pObjCurrent->m_pPopupPoint, 
			p->m_rPosition + p->m_pPopupPoint, Direction_Primary, Direction_Secondary, Distance);

		if( Direction_Primary==PK_Direction )
		{
			int Test_Distance_Primary;
			if( PK_Direction==DIRECTION_Left_CONST )
				Test_Distance_Primary=(pObjCurrent->m_rPosition.X+pObjCurrent->m_pPopupPoint.X)-(p->m_rPosition.Right()+p->m_pPopupPoint.X);
			else if( PK_Direction==DIRECTION_Right_CONST )
				Test_Distance_Primary=(p->m_rPosition.X+p->m_pPopupPoint.X)-(pObjCurrent->m_rPosition.Right()+pObjCurrent->m_pPopupPoint.X);
			else if( PK_Direction==DIRECTION_Up_CONST )
				Test_Distance_Primary=(pObjCurrent->m_rPosition.Y+pObjCurrent->m_pPopupPoint.Y)-(p->m_rPosition.Bottom()+p->m_pPopupPoint.Y);
			else if( PK_Direction==DIRECTION_Down_CONST )
				Test_Distance_Primary=(p->m_rPosition.Y+p->m_pPopupPoint.Y)-(pObjCurrent->m_rPosition.Bottom()+pObjCurrent->m_pPopupPoint.Y);
			
			if(Test_Distance_Primary+Distance<Distance_Primary || Distance_Primary==-1)
			{
				Distance_Primary=Test_Distance_Primary+Distance;
				pObj_Primary=p;
			}
		}
		else if( Direction_Secondary==PK_Direction && (Distance<Distance_Secondary || Distance_Secondary==-1) )
		{
			Distance_Secondary=Distance;
			pObj_Secondary=p;
		}
	}

	if(NULL != pObj_Secondary && NULL != pObj_Primary && Distance_Primary > Distance_Secondary)
	{
		PlutoPoint PrimaryObjectPosition = pObj_Primary->m_pMidPoint + pObj_Primary->m_pPopupPoint;
		PlutoPoint SecondaryObjectPosition = pObj_Secondary->m_pMidPoint + pObj_Secondary->m_pPopupPoint;

		if((PK_Direction == DIRECTION_Right_CONST || PK_Direction == DIRECTION_Left_CONST) && PrimaryObjectPosition.Y == SecondaryObjectPosition.Y) 
			pObj_Primary = pObj_Secondary;
		else if((PK_Direction == DIRECTION_Up_CONST || PK_Direction == DIRECTION_Down_CONST) && PrimaryObjectPosition.X == SecondaryObjectPosition.X)
			pObj_Primary = pObj_Secondary;
	}

	if( pObj_Primary )
		return pObj_Primary;
	if( pObj_Secondary )
		return pObj_Secondary;
	return pObjCurrent;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ bool OrbiterRenderer::HighlightNextObject( int PK_Direction )
{
	PLUTO_SAFETY_LOCK( cm, OrbiterLogic()->m_ScreenMutex );  // Protect the highlighed object
	// Nothing is selected, select the first object
	if(NULL == OrbiterLogic()->m_pObj_Highlighted || (OrbiterLogic()->m_pObj_Highlighted && 
		(!OrbiterLogic()->m_pObj_Highlighted->m_bOnScreen || OrbiterLogic()->m_pObj_Highlighted->IsHidden())))
	{
		OrbiterLogic()->m_pObj_Highlighted_set(NULL);
		if(sbNoSelection != OrbiterLogic()->m_nSelectionBehaviour)
			HighlightFirstObject();

		if(OrbiterLogic()->m_pObj_Highlighted)
			RenderObjectAsync(OrbiterLogic()->m_pObj_Highlighted);

		return OrbiterLogic()->m_pObj_Highlighted!=NULL;
	}

	if( OrbiterLogic()->m_pObj_Highlighted->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST )
	{
		DesignObj_DataGrid *pDesignObj_DataGrid = (DesignObj_DataGrid *) OrbiterLogic()->m_pObj_Highlighted;
		DataGridTable *pDataGridTable = pDesignObj_DataGrid->DataGridTable_Get();
		if( !pDataGridTable )
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"OrbiterRenderer::HighlightNextObject !pDataGridTable");
			return false;
		}
		bool bScrolledOutsideGrid=false;  // Will be true if we scroll past the edge of the grid
		PLUTO_SAFETY_LOCK( dg, OrbiterLogic()->m_DatagridMutex );

		switch( PK_Direction )
		{
		case DIRECTION_Up_CONST:
			if(  pDesignObj_DataGrid->m_sExtraInfo.find( 'C' )!=string::npos )
				bScrolledOutsideGrid=true;
			else if(
				(pDesignObj_DataGrid->m_iHighlightedRow > 0 || pDesignObj_DataGrid->m_GridCurRow > 0) &&
				pDesignObj_DataGrid->m_MaxRow > 1
				)
			{
				pDesignObj_DataGrid->m_iHighlightedRow--;
				if(pDesignObj_DataGrid->m_iHighlightedRow < 0 || (pDesignObj_DataGrid->m_iHighlightedRow == 0 && pDesignObj_DataGrid->HasMoreUp() && pDataGridTable->m_iUpRow!=-1 ))
				{
					// Save which row in the actual table we are pointing to,  so we can point there again after doing the scroll
					int iHighlightedAbsoluteRow = pDesignObj_DataGrid->m_iHighlightedRow + pDesignObj_DataGrid->m_GridCurRow;
					dg.Release();
					OrbiterLogic()->CMD_Scroll_Grid( "", pDesignObj_DataGrid->m_ObjectID, PK_Direction );
					pDesignObj_DataGrid->m_iHighlightedRow=iHighlightedAbsoluteRow - pDesignObj_DataGrid->m_GridCurRow;

					if( pDesignObj_DataGrid->m_iHighlightedRow > pDesignObj_DataGrid->m_MaxRow - 1 )
						pDesignObj_DataGrid->m_iHighlightedRow = pDesignObj_DataGrid->m_MaxRow - 1;
				}
			}
			else
				bScrolledOutsideGrid=true;
			break;
		case DIRECTION_Down_CONST:
			if(  pDesignObj_DataGrid->m_sExtraInfo.find( 'C' )!=string::npos )
				bScrolledOutsideGrid=true;
			// Continue only if we're not already highlighting the last cell
			else if(
				pDesignObj_DataGrid->m_GridCurRow + pDesignObj_DataGrid->m_iHighlightedRow + (pDataGridTable->m_iUpRow >= 0 ? 0 : 1) <
				pDataGridTable->GetRows() &&  
				pDesignObj_DataGrid->m_MaxRow > 1
				)  // Add 1 since the highlight is 0 based and get rows is not, add 2 if the last row is just a 'scroll down'
			{
				pDesignObj_DataGrid->m_iHighlightedRow++;
				// See if we've scrolled past the visible end, in which case we need to page.  Subtract 1 or 2 cells for the scroll up/down cells if any
				if(pDesignObj_DataGrid->m_iHighlightedRow >= pDesignObj_DataGrid->m_MaxRow - (pDataGridTable->m_iDownRow >= 0 ? 1 : 0) /*- (pDesignObj_DataGrid->m_iUpRow >= 0 ? 1 : 0)*/)
				{
					dg.Release();
					OrbiterLogic()->CMD_Scroll_Grid( "", pDesignObj_DataGrid->m_ObjectID, PK_Direction );
					pDesignObj_DataGrid->m_iHighlightedRow = 0;
				}
			}
			else
				bScrolledOutsideGrid=true;
			break;
		case DIRECTION_Left_CONST:
			if(  pDesignObj_DataGrid->m_sExtraInfo.find( 'R' )!=string::npos )
				bScrolledOutsideGrid=true;
			else if(pDesignObj_DataGrid->m_iHighlightedColumn>0 || pDesignObj_DataGrid->m_GridCurCol>0  )
			{
				pDesignObj_DataGrid->m_iHighlightedColumn--;
				if(  pDesignObj_DataGrid->m_iHighlightedColumn<0  )
				{
					dg.Release();
					OrbiterLogic()->CMD_Scroll_Grid( "", pDesignObj_DataGrid->m_ObjectID, PK_Direction );
					pDesignObj_DataGrid->m_iHighlightedColumn=pDesignObj_DataGrid->m_MaxCol;
				}
			}
			else
				bScrolledOutsideGrid=true;
			break;
		case DIRECTION_Right_CONST:
			if(  pDesignObj_DataGrid->m_sExtraInfo.find( 'R' )!=string::npos )
				bScrolledOutsideGrid=true;
			// See if we scrolled past the physical end of all the columns
			else if(  pDesignObj_DataGrid->m_GridCurCol+pDesignObj_DataGrid->m_iHighlightedColumn+1 < pDataGridTable->GetCols(  )  ) // Add 1 since the highlight is 0 based and get cols is not
			{
				pDesignObj_DataGrid->m_iHighlightedColumn++;
				// See if we've scrolled past the visible end, in which case we need to page
				if(  pDesignObj_DataGrid->m_iHighlightedColumn>=pDesignObj_DataGrid->m_MaxCol  )
				{
					dg.Release();
					OrbiterLogic()->CMD_Scroll_Grid( "", pDesignObj_DataGrid->m_ObjectID, PK_Direction );
					pDesignObj_DataGrid->m_iHighlightedColumn=0;
				}
			}
			else
				bScrolledOutsideGrid=true;
			break;
		};

		// If we're capturing arrows, then we don't let the user move outside the grid
		if(  pDesignObj_DataGrid->m_sExtraInfo.find( 'A' )!=string::npos )
			bScrolledOutsideGrid=false;

		// Check that pDesignObj_DataGrid->DataGridTable_Get() wasn't deleted
		if(  pDesignObj_DataGrid->m_sExtraInfo.find( 'H' )!=string::npos )
		{
			// We don't want the user to be able to just highlight cells without selecting, so select this cell
			DataGridCell *pCell = pDataGridTable->GetData(
				pDesignObj_DataGrid->m_iHighlightedColumn!=-1 ? pDesignObj_DataGrid->m_iHighlightedColumn + pDesignObj_DataGrid->m_GridCurCol : pDesignObj_DataGrid->m_GridCurCol,
				pDesignObj_DataGrid->m_iHighlightedRow!=-1 ? pDesignObj_DataGrid->m_iHighlightedRow + pDesignObj_DataGrid->m_GridCurRow : 0);

			if( pCell )
				OrbiterLogic()->CMD_Set_Variable(pDesignObj_DataGrid->m_iPK_Variable, pCell->GetValue());
			dg.Release();
			OrbiterLogic()->SelectedObject(pDesignObj_DataGrid,smNavigation);
		}
		dg.Release();
		RenderObjectAsync(pDesignObj_DataGrid);
		OrbiterLogic()->m_pObj_Highlighted_Last=NULL; // Be sure we always re-highlight this object since the grid changed

		if( !bScrolledOutsideGrid )
		{
			//if we have hidden columns/rows, skip them
			PLUTO_SAFETY_LOCK( dg, OrbiterLogic()->m_DatagridMutex );

			DataGridCell *pCell = NULL;
			while(!pCell)
			{
				int nHColumn = pDesignObj_DataGrid->m_iHighlightedColumn!=-1 ? pDesignObj_DataGrid->m_iHighlightedColumn + pDesignObj_DataGrid->m_GridCurCol : pDesignObj_DataGrid->m_GridCurCol;
				int nHRow = pDesignObj_DataGrid->m_iHighlightedRow!=-1 ? pDesignObj_DataGrid->m_iHighlightedRow + pDesignObj_DataGrid->m_GridCurRow - (pDataGridTable->m_iUpRow >= 0 ? 1 : 0) : 0;

				pCell = pDataGridTable->GetData(nHColumn, nHRow);
				if(pCell)
					break; //all ok, we got it

				bool bGiveUp = false;
				switch(PK_Direction)
				{
				case DIRECTION_Up_CONST:
					if(nHRow == 0)
						bGiveUp = true;
					else
						pDesignObj_DataGrid->m_iHighlightedRow--;
					break;

				case DIRECTION_Down_CONST:

					if(nHRow >= pDataGridTable->m_RowCount)
						bGiveUp = true;
					else
						pDesignObj_DataGrid->m_iHighlightedRow--;
					break;

				case DIRECTION_Left_CONST:
					if(nHColumn == 0)
						bGiveUp = true;
					else
						pDesignObj_DataGrid->m_iHighlightedColumn--;
					break;

				case DIRECTION_Right_CONST:
					if(nHColumn >= pDataGridTable->m_ColumnCount)
						bGiveUp = true;
					else
						pDesignObj_DataGrid->m_iHighlightedColumn++;
					break;
				}

				if(bGiveUp)
					break;
			}

			return true; // We just moved around within the grid
		}
	}

	//DesignObj_Orbiter *pDesignObj_Orbiter_OriginallyHighlight=m_pObj_Highlighted;
	DesignObj_Orbiter *pNextObject = OrbiterLogic()->m_pObj_Highlighted;
	switch( PK_Direction )
	{
		// Find the next ones,  skipping over any hidden ones
	case DIRECTION_Up_CONST:
		while(  pNextObject->m_pDesignObj_Orbiter_Up && pNextObject->m_pDesignObj_Orbiter_Up->IsHidden(  )  )
			pNextObject = OrbiterLogic()->m_pObj_Highlighted->m_pDesignObj_Orbiter_Up;
		pNextObject = pNextObject->m_pDesignObj_Orbiter_Up;
		break;
	case DIRECTION_Down_CONST:
		while(  pNextObject->m_pDesignObj_Orbiter_Down && pNextObject->m_pDesignObj_Orbiter_Down->IsHidden(  )  )
			pNextObject = OrbiterLogic()->m_pObj_Highlighted->m_pDesignObj_Orbiter_Down;
		pNextObject = pNextObject->m_pDesignObj_Orbiter_Down;
		break;
	case DIRECTION_Left_CONST:
		while(  pNextObject->m_pDesignObj_Orbiter_Left && pNextObject->m_pDesignObj_Orbiter_Left->IsHidden(  )  )
			pNextObject = OrbiterLogic()->m_pObj_Highlighted->m_pDesignObj_Orbiter_Left;
		pNextObject = pNextObject->m_pDesignObj_Orbiter_Left;
		break;
	case DIRECTION_Right_CONST:
		while(  pNextObject->m_pDesignObj_Orbiter_Right && pNextObject->m_pDesignObj_Orbiter_Right->IsHidden(  )  )
			pNextObject = OrbiterLogic()->m_pObj_Highlighted->m_pDesignObj_Orbiter_Right;
		pNextObject = pNextObject->m_pDesignObj_Orbiter_Right;
		break;
	}

	DesignObj_Orbiter *pObj_Highlighted_Before = OrbiterLogic()->m_pObj_Highlighted;
	if(!pNextObject || pNextObject == OrbiterLogic()->m_pObj_Highlighted)
	{
		OrbiterLogic()->m_pObj_Highlighted_set(FindObjectToHighlight( OrbiterLogic()->m_pObj_Highlighted, PK_Direction ));
		if( OrbiterLogic()->m_pObj_Highlighted==pObj_Highlighted_Before )
			return false;
	}
	else
		OrbiterLogic()->m_pObj_Highlighted_set(pNextObject);

	if( pObj_Highlighted_Before && OrbiterLogic()->m_pObj_Highlighted!=pObj_Highlighted_Before )
		OrbiterLogic()->ExecuteCommandsInList( &pObj_Highlighted_Before->m_Action_UnhighlightList, pObj_Highlighted_Before, smHighlight, 0, 0 );

	if( OrbiterLogic()->m_pObj_Highlighted->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST )
	{
		DesignObj_DataGrid *pDesignObj_DataGrid = (DesignObj_DataGrid *) OrbiterLogic()->m_pObj_Highlighted;
		// If nothing is highlighted in this grid, start with the first row/column
		if( pDesignObj_DataGrid->m_iHighlightedColumn==-1 && pDesignObj_DataGrid->m_iHighlightedRow==-1 )
		{
			if(  pDesignObj_DataGrid->m_sExtraInfo.find( 'C' )==string::npos )
				pDesignObj_DataGrid->m_iHighlightedRow=0;
			if(  pDesignObj_DataGrid->m_sExtraInfo.find( 'R' )==string::npos )
				pDesignObj_DataGrid->m_iHighlightedColumn=0;
		}
	}

	if(OrbiterLogic()->m_pObj_Highlighted)
		RenderObjectAsync(OrbiterLogic()->m_pObj_Highlighted);

	return true;
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer::RenderShortcut(DesignObj_Orbiter *pObj)
{
	string sCharToRender;
	if(pObj->m_iPK_Button >= BUTTON_1_CONST && pObj->m_iPK_Button <= BUTTON_9_CONST)
		sCharToRender += '0' + pObj->m_iPK_Button - BUTTON_1_CONST + 1;
	else if(pObj->m_iPK_Button == BUTTON_0_CONST)
		sCharToRender += '0';

	if(sCharToRender != "")
	{
		PlutoPoint textPos(pObj->m_pPopupPoint.X + pObj->m_rPosition.X + 5, pObj->m_pPopupPoint.Y + pObj->m_rPosition.Y + 5);

		TextStyle *pTextStyle = OrbiterLogic()->m_mapTextStyle_Find(1);
		PlutoColor OldColor = pTextStyle->m_ForeColor;
		pTextStyle->m_ForeColor.m_Value = 0xFF2020;
		pTextStyle->m_iPixelHeight += 15;

		PlutoRectangle rect(textPos.X, textPos.Y, 30, pTextStyle->m_iPixelHeight + 5);
		DesignObjText text(OrbiterLogic()->m_pScreenHistory_Current->GetObj());
		text.m_rPosition = rect;

		RenderText(sCharToRender,&text, pTextStyle);
		pTextStyle->m_iPixelHeight -= 15;
		pTextStyle->m_ForeColor = OldColor;
	}
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer::HidePopups(DesignObj_Orbiter *pObj)
{
	PLUTO_SAFETY_LOCK( cm, OrbiterLogic()->m_ScreenMutex );
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"hide popups");
#endif
	if( pObj )
	{
		for(list<class PlutoPopup*>::iterator it=pObj->m_listPopups.begin();it!=pObj->m_listPopups.end();)
		{
			ObjectOffScreen((*it)->m_pObj);
			delete *it++;
		}

		if(pObj->m_listPopups.size()) //no popup, no refresh
			OrbiterLogic()->CMD_Refresh("");

		pObj->m_listPopups.clear();
	}
	else
	{
		for(list<class PlutoPopup*>::iterator it=OrbiterLogic()->m_listPopups.begin();it!=OrbiterLogic()->m_listPopups.end();)
		{
			ObjectOffScreen((*it)->m_pObj);
			delete *it++;
		}

		if(OrbiterLogic()->m_listPopups.size()) //no popup, no refresh
			OrbiterLogic()->CMD_Refresh("");

		OrbiterLogic()->m_listPopups.clear();
	}
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer::RedrawObjects()
{
	PLUTO_SAFETY_LOCK(rm, m_NeedRedrawVarMutex);
	if(
		m_vectObjs_NeedRedraw.size() == 0 && 
		m_vectTexts_NeedRedraw.size() == 0 && 
		OrbiterLogic()->m_bRerenderScreen == false
	)
		return; // Nothing to do anyway

	OrbiterLogic()->CallMaintenanceInMiliseconds( 0, (OrbiterCallBack) &Orbiter::RealRedraw, NULL, pe_ALL );
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer::RefreshScreen(void *data)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "OrbiterRenderer::RefreshScreen %d",(int) OrbiterLogic()->m_bRerenderScreen );
#endif

	PLUTO_SAFETY_LOCK( cm, OrbiterLogic()->m_ScreenMutex );
	PLUTO_SAFETY_LOCK( nd, m_NeedRedrawVarMutex );
	// We don't have a good solution for rendering objects under popups--so re-render the whole screen if there are popups and we're going to render something else anyway
#ifndef ORBITER_OPENGL
	if(  OrbiterLogic()->m_bRerenderScreen ||
		((OrbiterLogic()->m_listPopups.size() || 
		(OrbiterLogic()->m_pScreenHistory_Current && OrbiterLogic()->m_pScreenHistory_Current->GetObj()->m_listPopups.size())) && 
		(m_vectObjs_NeedRedraw.size() || m_vectTexts_NeedRedraw.size()) ))
#else
	if( OrbiterLogic()->m_bRerenderScreen )
#endif
	{
		if(OrbiterLogic()->m_pGraphicBeforeHighlight)
			UnHighlightObject(true);
		m_vectObjs_NeedRedraw.clear();
		m_vectTexts_NeedRedraw.clear();
		nd.Release();

		RenderScreen( !OrbiterLogic()->m_bRerenderScreen );  // If !m_bRerenderScreen, then only redraw objects because we're just rendering the whole screen because of the popup
		if(NULL != OrbiterLogic()->m_pObj_Highlighted)
			DoHighlightObject();

		OrbiterLogic()->m_bRerenderScreen = false;
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Exiting Redraw Objects" );
#endif
		return;
	}

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write( LV_CONTROLLER, "I won't render the whole screen. Objects to be rendered: %d, texts to be rendered: %d",	
		m_vectObjs_NeedRedraw.size(), m_vectTexts_NeedRedraw.size()	);
#endif

	if(m_vectObjs_NeedRedraw.size() == 0 && m_vectTexts_NeedRedraw.size() == 0)
		return;

	vector<DesignObj_Orbiter *> vectObjs_NeedRedraw;
	vector<DesignObjText *> vectTexts_NeedRedraw;

	// We want to release the mutex since the redrawing may loop back and try to re-use these global objects
	for(vector<DesignObj_Orbiter *>::iterator ito=m_vectObjs_NeedRedraw.begin();ito!=m_vectObjs_NeedRedraw.end();++ito)
		vectObjs_NeedRedraw.push_back(*ito);
	for(vector<DesignObjText *>::iterator itt=m_vectTexts_NeedRedraw.begin();itt!=m_vectTexts_NeedRedraw.end();++itt)
		vectTexts_NeedRedraw.push_back(*itt);

	m_vectObjs_NeedRedraw.clear();
	m_vectTexts_NeedRedraw.clear();
	nd.Release();

	BeginPaint();

	bool bRehighlight=false;
	if(
		OrbiterLogic()->m_pGraphicBeforeHighlight && NULL != OrbiterLogic()->m_pObj_Highlighted && 
		(OrbiterLogic()->m_pObj_Highlighted != OrbiterLogic()->m_pObj_Highlighted_Last)
	)
	{
		bRehighlight=true;
		UnHighlightObject();
	}

	//render objects
	for(vector<DesignObj_Orbiter *>::iterator it = vectObjs_NeedRedraw.begin(), 
		end = vectObjs_NeedRedraw.end(); it != end; ++it )
	{
		class DesignObj_Orbiter *pObj = *it;
		if(pObj && pObj->m_bOnScreen)
		{
			bool bIntersectedWith = (pObj->m_rPosition+pObj->m_pPopupPoint).IntersectsWith(OrbiterLogic()->m_rectLastHighlight);
			bool bIncludedIn = (pObj->m_rPosition+pObj->m_pPopupPoint).Contains(OrbiterLogic()->m_rectLastHighlight);

			if(!bRehighlight && (bIntersectedWith || bIncludedIn) && OrbiterLogic()->m_rectLastHighlight.Width && OrbiterLogic()->m_rectLastHighlight.Height )
			{
				bRehighlight=true;
				UnHighlightObject();
			}
			pObj->RenderObject(OrbiterLogic()->m_pScreenHistory_Current->GetObj(), pObj->m_pPopupPoint);
			UpdateRect(pObj->m_rPosition, pObj->m_pPopupPoint);
		}
	}

	//render texts
	for(vector<DesignObjText *>::iterator it_text = vectTexts_NeedRedraw.begin(), 
		end_text = vectTexts_NeedRedraw.end(); it_text != end_text; ++it_text )
	{
		DesignObjText *pText = *it_text;
		TextStyle *pTextStyle = pText->m_mapTextStyle_Find( 0 );
		if( pTextStyle )
		{
			string sParentObjectID = "";
			if(NULL != pText->m_pObject)
				sParentObjectID = pText->m_pObject->GenerateObjectHash(pText->m_pObject->m_pPopupPoint, false);

			PlutoColor BackColor = pTextStyle->m_BackColor;
			BackColor.SetAlpha(255);

			SolidRectangle(pText->m_pObject->m_pPopupPoint.X + pText->m_rPosition.Left(), 
				pText->m_pObject->m_pPopupPoint.Y + pText->m_rPosition.Top(), 
				pText->m_rPosition.Width,  pText->m_rPosition.Height,  BackColor, 
				sParentObjectID);

			string TextToDisplay = OrbiterLogic()->SubstituteVariables(OrbiterLogic()->SubstituteVariables(pText->m_sText, pText->m_pObject, 0, 0), pText->m_pObject, 0, 0).c_str();
			RenderText(TextToDisplay,pText, pTextStyle, pText->m_pObject->m_pPopupPoint);
			UpdateRect(pText->m_rPosition, pText->m_pObject->m_pPopupPoint);
		}
		else
		{
			RenderObjectAsync(pText->m_pObject);
		}
	}

	if(NULL != OrbiterLogic()->m_pObj_Highlighted && (bRehighlight || OrbiterLogic()->m_pObj_Highlighted != OrbiterLogic()->m_pObj_Highlighted_Last))
	{
		if(OrbiterLogic()->m_pObj_Highlighted->m_GraphicToDisplay == GRAPHIC_HIGHLIGHTED && OrbiterLogic()->m_pObj_Highlighted->m_vectHighlightedGraphic.size())
		{
			//this is a special kind of object; it has a highlighted version - saving the surface here
			//for un-highlighting will be too late (so we already did it in RenderObject)
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Object already highlighted %s", OrbiterLogic()->m_pObj_Highlighted->m_ObjectID.c_str());
		}
		else
		{
			OrbiterLogic()->m_pObj_Highlighted_Last = OrbiterLogic()->m_pObj_Highlighted;
			DoHighlightObject();
		}
	}
	EndPaint();
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer::RenderObjectAsync(DesignObj_Orbiter *pObj)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "RenderObjectAsync: %s", pObj->m_ObjectID.c_str());
#endif

	PLUTO_SAFETY_LOCK(nd, m_NeedRedrawVarMutex);

	//add it to the list with objects to render only if it's not there yet!
	if(find(m_vectObjs_NeedRedraw.begin(), m_vectObjs_NeedRedraw.end(), pObj) == m_vectObjs_NeedRedraw.end())
		m_vectObjs_NeedRedraw.push_back(pObj);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer::RenderTextAsync(DesignObjText *pObj)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "RenderTextAsync: %d", pObj->m_PK_Text);
#endif

	PLUTO_SAFETY_LOCK(nd, m_NeedRedrawVarMutex);
	m_vectTexts_NeedRedraw.push_back(pObj);
	nd.Release();
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer::ObjectOnScreenWrapper()
{
	PLUTO_SAFETY_LOCK( sm, OrbiterLogic()->m_ScreenMutex );

	// ObjectOnSCreen will reset the hidden flags,  and CheckSpecial may set an object to hidden,  so do them in this order
	VectDesignObj_Orbiter vectDesignObj_Orbiter_OnScreen;

	PLUTO_SAFETY_LOCK( nd, m_NeedRedrawVarMutex );
	m_vectObjs_NeedRedraw.clear(  );
	nd.Release();

	PLUTO_SAFETY_LOCK( vm, OrbiterLogic()->m_VariableMutex );
	OrbiterLogic()->m_vectObjs_Selected.clear(  );
	OrbiterLogic()->m_vectObjs_TabStops.clear(  );
	OrbiterLogic()->m_mapDevice_Selected.clear(  );
	OrbiterLogic()->m_mapFloorplanObject_Selected.clear();
	vm.Release(  );

	//also reset the last selected object from the floorplan.
	OrbiterLogic()->m_pObj_LastSelected = NULL;
	OrbiterLogic()->m_dwPK_DesignObj_Toolbar_Last=0;

	ObjectOnScreen( &vectDesignObj_Orbiter_OnScreen, OrbiterLogic()->m_pScreenHistory_Current->GetObj() );
	for(list<class PlutoPopup*>::iterator it=OrbiterLogic()->m_pScreenHistory_Current->GetObj()->m_listPopups.begin();
		it!=OrbiterLogic()->m_pScreenHistory_Current->GetObj()->m_listPopups.end();++it)
	{
		PlutoPopup *pPlutoPopup = *it;
		ObjectOnScreen( &vectDesignObj_Orbiter_OnScreen, pPlutoPopup->m_pObj );
	}

	switch(OrbiterLogic()->m_nSelectionBehaviour)
	{
	case sbAllowsSelected:
		HighlightFirstObject();
		break;

	case sbSelectOnFirstMove:
	case sbNoSelection:
		OrbiterLogic()->m_pObj_Highlighted_set(NULL);
		break;
	}

	// Do the on load actions for the screen itself,  and objects on it
	OrbiterLogic()->ExecuteCommandsInList( &OrbiterLogic()->m_pScreenHistory_Current->GetObj()->m_Action_LoadList, OrbiterLogic()->m_pScreenHistory_Current->GetObj(), smLoadUnload, 0, 0 );
	OrbiterLogic()->HandleNewObjectsOnScreen( &vectDesignObj_Orbiter_OnScreen );
}
//-----------------------------------------------------------------------------------------------------
// If an object has the don't reset state true,  it won't reset to normal,  and it's children won't reset either
void OrbiterRenderer::ObjectOnScreen( VectDesignObj_Orbiter *pVectDesignObj_Orbiter, DesignObj_Orbiter *pObj, PlutoPoint *ptPopup )
{
	// Do this again since sometimes there will be several grids with the same name within the application and if
	// we're going to do a lookup, such as with seek grid, we want to find the one most recently on screen
	if( pObj->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST )
	{
		DesignObj_DataGrid *pObj_Datagrid = (DesignObj_DataGrid *) pObj;
		OrbiterLogic()->m_mapObjs_AllGrids[pObj_Datagrid->m_sGridID] = pObj_Datagrid;
	}
	if( pObj->m_iRegenInterval )
		OrbiterLogic()->CallMaintenanceInMiliseconds(pObj->m_iRegenInterval,&Orbiter::RedrawObject,pObj,pe_Match_Data);

	pVectDesignObj_Orbiter->push_back( pObj );
	pObj->m_bOnScreen=true;

	// Add it to the list of tab stops whether it's visible or not.  The findfirst/next will skip of hidden objects anyway
	// And this way we don't need to worry about the changing state of objects that are hidden/shown
	if(  pObj->m_bTabStop  )
		OrbiterLogic()->m_vectObjs_TabStops.push_back( pObj );

	if( ptPopup )
		pObj->m_pPopupPoint = *ptPopup;

	if( pObj->m_ObjectType==DESIGNOBJTYPE_Broadcast_Video_CONST )
	{
		pObj->m_pvectCurrentGraphic = NULL;
		for(size_t i = 0; i < pObj->m_vectGraphic.size(); i++)
			RELEASE_GRAPHIC(pObj->m_vectGraphic[i]);
		pObj->m_vectGraphic.clear();
		pObj->m_iCurrentFrame = 0;
		OrbiterLogic()->m_vectObjs_VideoOnScreen.push_back(pObj);
		OrbiterLogic()->m_bAlreadyQueuedVideo=false;  // When we start to paint a video object we need to add the maint timer to render it
	}

	if(  pObj->m_GraphicToDisplay==GRAPHIC_SELECTED  )
		OrbiterLogic()->m_vectObjs_Selected.push_back( pObj );

// Hidden on WIN32/WINCE
#if defined(WIN32) || defined(PADORBITER) 
	if(pObj->m_iBaseObjectID == DESIGNOBJ_butKDEDesktop_CONST || pObj->m_iBaseObjectID == DESIGNOBJ_butKDEDesktop_CONST + 1)
		pObj->m_bHidden = true;

	if(pObj->m_iBaseObjectID == DESIGNOBJ_butStartAVwizard_CONST || pObj->m_iBaseObjectID == DESIGNOBJ_butStartWizard_CONST)
		pObj->m_bHidden = true;
#endif

//NOT hidden on WIN32/WINCE
#if defined(WIN32) || defined(MAEMO_NOKIA770)
	if(pObj->m_iBaseObjectID == DESIGNOBJ_objExitController2_CONST)
		pObj->m_bHidden = false;
#endif

	// Move InitializeGrid down so it gets called after the LoadActions,  which may set variables that it will use

	//  Move this down because there's a hide object in the onload
	//  actions that is getting executed before the object's state is reset
	//  DoLoadUnloadActions( pObj->m_Action_LoadList, pObj, CTRLCOMMAND_UNLOAD );

	DesignObj_DataList::iterator iHao;
	for( iHao=pObj->m_ChildObjects.begin(  ); iHao != pObj->m_ChildObjects.end(  ); ++iHao )
		ObjectOnScreen( pVectDesignObj_Orbiter, ( DesignObj_Orbiter * )( *iHao ), ptPopup );
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer::GraphicOffScreen(vector<class PlutoGraphic*> *pvectGraphic)
{
	size_t size = (*pvectGraphic).size();
	for(size_t i = 0; i < size; i++)
		(*pvectGraphic)[i]->Clear();
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer::ObjectOffScreen( DesignObj_Orbiter *pObj )
{
	PLUTO_SAFETY_LOCK(sm, m_pOrbiter->m_ScreenMutex);

	if( OrbiterLogic()->m_pObj_Highlighted==pObj )
		OrbiterLogic()->m_pObj_Highlighted_set(NULL);  // Otherwise an object on popup removed from the screen may remain highlighted
		
	pObj->m_bOnScreen=false;

	RELEASE_GRAPHIC(pObj->m_pGraphicToUndoSelect);

	if( !pObj->m_bKeepGraphicInCache )
	{
		GraphicOffScreen( &(pObj->m_vectGraphic) );
		GraphicOffScreen( &(pObj->m_vectSelectedGraphic) );
		GraphicOffScreen( &(pObj->m_vectHighlightedGraphic) );

		size_t i;
		for(i = 0; i < pObj->m_vectAltGraphics.size(); ++i)
			GraphicOffScreen(&(pObj->m_vectAltGraphics[i]));
	}

	pObj->m_pvectCurrentGraphic = NULL;
	pObj->m_pvectCurrentPlayingGraphic = NULL;

	OrbiterLogic()->ExecuteCommandsInList( &pObj->m_Action_UnloadList, pObj, smLoadUnload, 0, 0 );

	DesignObj_DataList::iterator iHao;
	for( iHao=pObj->m_ChildObjects.begin(  ); iHao != pObj->m_ChildObjects.end(  ); ++iHao )
	{
		ObjectOffScreen( ( DesignObj_Orbiter * )*iHao );
	}
}

void *ImageLoadThread(void *p)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"ImageLoadThread running");
#endif
	OrbiterRenderer *pRenderer = (OrbiterRenderer *)p;
	bool bContinue = true, bRedraw = false;
	do
	{
		PLUTO_SAFETY_LOCK(vm, pRenderer->m_pOrbiter->m_VariableMutex);
		// Do them in the reverse order so if we're paging through lots of cells we'll always be rendering
		// the visible ones first
		if( pRenderer->m_listBackgroundImage.size()==0 )
		{
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"ImageLoadThread pRenderer->m_listBackgroundImage.size()==0");
#endif
			return NULL;  // Must have had another imageloadthread also running that serviced the last one at the same split second.  No problem.
		}
		BackgroundImage *pBackgroundImage = pRenderer->m_listBackgroundImage.front();
		pRenderer->m_listBackgroundImage.pop_front();
		if( !pBackgroundImage->m_pObj_Grid->m_bOnScreen )  // This may have gone off screen, if so ignore it
		{
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"ImageLoadThread pBackgroundImage %s is off screen",pBackgroundImage->m_sPic.c_str());
#endif
			delete pBackgroundImage;
			continue;
		}
		vm.Release();

		size_t GraphicLength=0;
		char *pGraphicData=pRenderer->m_pOrbiter->ReadFileIntoBuffer(pBackgroundImage->m_sPic, GraphicLength);

		PLUTO_SAFETY_LOCK(M, pRenderer->m_pOrbiter->m_DatagridMutex);
		vm.Relock();

		DataGridTable *pDataGridTable = pBackgroundImage->m_pObj_Grid->DataGridTable_Get(pBackgroundImage->m_DataGridTable_Cache.first,pBackgroundImage->m_DataGridTable_Cache.second);
		if( !pDataGridTable || !pBackgroundImage->m_pObj_Grid->m_bOnScreen || pDataGridTable->m_iRequestID!=pBackgroundImage->m_iRequestID )  // This may have gone off screen while loading the graphic with the mutex unlocked
		{
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"ImageLoadThread pBackgroundImage %s request %d!=%d",
				pBackgroundImage->m_sPic.c_str(),pBackgroundImage->m_iRequestID,pDataGridTable ? pDataGridTable->m_iRequestID : -999);
#endif

			delete pBackgroundImage;
			delete pGraphicData;
			continue;
		}
		pBackgroundImage->m_pCell->m_pGraphicData = pGraphicData;
		pBackgroundImage->m_pCell->m_GraphicLength=(unsigned long) GraphicLength;
		if( pBackgroundImage->m_pObj_Grid->CellIsVisible( pBackgroundImage->m_ColRow.first, pBackgroundImage->m_ColRow.second ) )
		{
			vm.Release();
			M.Release();

			// Again load the graphic without holding the mutex
			PlutoGraphic *pPlutoGraphic = pBackgroundImage->m_pObj_Grid->m_pOrbiter->Renderer()->CreateGraphic();;
			pPlutoGraphic->m_Filename = "datagrid thumb";
			pPlutoGraphic->m_GraphicFormat = pBackgroundImage->m_pCell->m_GraphicFormat;
			pPlutoGraphic->LoadGraphic(pGraphicData, GraphicLength, pBackgroundImage->m_pObj_Grid->m_pOrbiter->m_iRotation);

			M.Relock();
			vm.Relock();

			// Again check since this may have gone off-screen while we were releasing the mutex to load the image
			DataGridTable *pDataGridTable2 = pBackgroundImage->m_pObj_Grid->DataGridTable_Get(pBackgroundImage->m_DataGridTable_Cache.first,pBackgroundImage->m_DataGridTable_Cache.second);
			if( !pDataGridTable2 || pDataGridTable2->m_iRequestID!=pBackgroundImage->m_iRequestID || !pBackgroundImage->m_pObj_Grid->CellIsVisible( pBackgroundImage->m_ColRow.first, pBackgroundImage->m_ColRow.second ) )
			{
#ifdef DEBUG
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"ImageLoadThread pBackgroundImage2 %s request %d!=%d",
					pBackgroundImage->m_sPic.c_str(),pBackgroundImage->m_iRequestID,pDataGridTable2 ? pDataGridTable2->m_iRequestID : -999);
#endif
				delete pBackgroundImage;
				RELEASE_GRAPHIC(pPlutoGraphic);
				continue;
			}

#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"delete1 pBackgroundImage->m_pCell->m_pGraphic %p:%p",pBackgroundImage->m_pCell,pBackgroundImage->m_pCell->m_pGraphic);
#endif
			RELEASE_GRAPHIC(pBackgroundImage->m_pCell->m_pGraphic);

			pBackgroundImage->m_pCell->m_pGraphic = pPlutoGraphic;
			int j=pBackgroundImage->m_ColRow.first;
			int i=pBackgroundImage->m_ColRow.second - pBackgroundImage->m_pObj_Grid->m_GridCurRow; // The current visible row
			int x, y, w, h;
			PlutoPoint point(0,0);
			pBackgroundImage->m_pObj_Grid->GetGridCellDimensions(pBackgroundImage->m_pCell->m_Colspan,  pBackgroundImage->m_pCell->m_Rowspan,  j,  i,  x,  y,  w,  h );

			string sCellObjectID, sCellObjectHash;
			sCellObjectID = sCellObjectHash = DATAGRID_THUMB_NAME " - " + StringUtils::ltos(x) + "-" + StringUtils::ltos(y) + 
				(NULL == pBackgroundImage->m_pCell->m_ImagePath ? "" : string("-pic ") + pBackgroundImage->m_pCell->m_ImagePath);
			pBackgroundImage->m_pObj_Grid->m_pOrbiter->Renderer()->RenderGraphic(pBackgroundImage->m_pCell->m_pGraphic, PlutoRectangle(x,  y,  w,  h), 
				pBackgroundImage->m_pObj_Grid->m_bDisableAspectLock, point, 255, 
				"datagrid " + pBackgroundImage->m_pObj_Grid->GenerateObjectHash(pBackgroundImage->m_pObj_Grid->m_pPopupPoint, false),
				sCellObjectID, 
				sCellObjectHash
			);
		}

		bContinue = (pRenderer->m_listBackgroundImage.size() > 0);
		bRedraw = ((pRenderer->m_listBackgroundImage.size() % 10) == 0);
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_EVENT,"ImageLoadThread %s size: %d (%d) %d",pBackgroundImage->m_sPic.c_str(),(int) pRenderer->m_listBackgroundImage.size(),(int) bRedraw,(int) bContinue);
#endif
		
		delete pBackgroundImage;

	} while(bContinue);
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_EVENT,"ImageLoadThread exiting");
#endif
	return NULL;
}

void OrbiterRenderer::BackgroundImageLoad(const char *Filename, DesignObj_DataGrid *pObj_DataGrid,int RequestID, pair<int,int> DataGridTable_Cache, DataGridCell *pCell, pair<int,int> ColRow, bool bDoFirst )
{
	bool bNeedToStartThread;
	PLUTO_SAFETY_LOCK(M, m_pOrbiter->m_VariableMutex);
	bNeedToStartThread = (m_listBackgroundImage.size() == 0);
	BackgroundImage *pBackgroundImage = new BackgroundImage(Filename,pObj_DataGrid,RequestID,DataGridTable_Cache,pCell,ColRow);
	if( bDoFirst )
		m_listBackgroundImage.push_front( pBackgroundImage );
	else
		m_listBackgroundImage.push_back( pBackgroundImage );
LoggerWrapper::GetInstance()->Write(LV_EVENT,"OrbiterRenderer::BackgroundImageLoad p %p %s size: %d need to start %d",
					  pBackgroundImage,Filename,(int) m_listBackgroundImage.size(),(int) bNeedToStartThread);

	M.Release();
	if (bNeedToStartThread)
	{
		pthread_t pthread_id; 
		int result;
		if( (result=pthread_create( &pthread_id, NULL, ImageLoadThread, (void*) this)) )
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"OrbiterRenderer::BackgroundImageLoad Cannot start image loading thread %d",result);
#ifdef DEBUG
		else
		{
			pthread_detach(pthread_id);
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"OrbiterRenderer::BackgroundImageLoad started thread %d",pthread_id);
		}
#endif
	}
}

/*virtual*/ bool OrbiterRenderer::HandleShowPopup(PlutoPopup* pPopup, PlutoPoint point, int EffectID)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"ShowPopup: %s", pPopup->m_pObj->m_ObjectID.c_str());
#endif
	PLUTO_SAFETY_LOCK(sm, OrbiterLogic()->m_ScreenMutex);

	if(pPopup->m_pObj)
		pPopup->m_pObj->RenderObject(pPopup->m_pObj, point);
	else
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Cannot render the popup %s: object doesn't exist", pPopup->m_sName.c_str()/*, pPopup->m_pObj->m_ObjectID.c_str()*/);

	return false;
}

/*virtual*/ bool OrbiterRenderer::HandleHidePopup(PlutoPopup* Popup)
{
	return false;
}

/*virtual*/ void OrbiterRenderer::UpdateObjectImage(string sPK_DesignObj, string sType, 
char *pData, int iData_Size, string sDisable_Aspect_Lock, bool bRefreshObject/* = true*/)
{
	PLUTO_SAFETY_LOCK( cm, OrbiterLogic()->m_ScreenMutex );

	DesignObj_Orbiter *pObj = OrbiterLogic()->FindObject( sPK_DesignObj );
	if(!pObj)
	{
		DesignObj_Orbiter *pObj_Current = OrbiterLogic()->GetCurrentDesignObj();
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "OrbiterRenderer::UpdateObjectImage Got update object image but cannot find: %s, currently %s", sPK_DesignObj.c_str(), pObj_Current ? pObj_Current->m_ObjectID.c_str() : "*none*");
		return;
	}

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"OrbiterRenderer::UpdateObjectImage with image of size %d",iData_Size);
#endif

	int PriorWidth=0, PriorHeight=0;
	if(  pObj->m_pvectCurrentGraphic  )
		pObj->m_pvectCurrentGraphic = NULL;
	if ( pObj->m_vectGraphic.size() )
	{
		PriorWidth = pObj->m_vectGraphic[pObj->m_iCurrentFrame]->m_nWidth;
		PriorHeight = pObj->m_vectGraphic[pObj->m_iCurrentFrame]->m_nHeight;
	}

	for(size_t i = 0; i < pObj->m_vectGraphic.size(); i++)
		RELEASE_GRAPHIC(pObj->m_vectGraphic[i]);
	pObj->m_vectGraphic.clear();
	pObj->m_iCurrentFrame = 0;

	if(  iData_Size==0  )
	{
		//pObj->m_pGraphic=NULL;*
		return;
	}

	PlutoGraphic *pPlutoGraphic = CreateGraphic();
	pPlutoGraphic->m_Filename = DYNAMIC_OBJ_NAME;

	if(sType == "bmp")
		pPlutoGraphic->m_GraphicFormat = GR_BMP;
	else if(sType == "jpg")
		pPlutoGraphic->m_GraphicFormat = GR_JPG;
	else if(sType == "png")
		pPlutoGraphic->m_GraphicFormat = GR_PNG;
	else
		pPlutoGraphic->m_GraphicFormat = GR_UNKNOWN;

	pPlutoGraphic->LoadGraphic(pData, iData_Size, OrbiterLogic()->m_iRotation);  // These weren't pre-rotated
	pObj->m_vectGraphic.push_back(pPlutoGraphic);
	pObj->m_pvectCurrentGraphic = &(pObj->m_vectGraphic);
	pObj->m_bDisableAspectLock = sDisable_Aspect_Lock == "1" ? true : false;

	if(bRefreshObject)
		RenderObjectAsync(pObj);
}

void OrbiterRenderer::HandleRefreshCommand(string sDataGrid_ID)
{
	if( sDataGrid_ID.length()==0 )
		OrbiterLogic()->m_bRerenderScreen = true;

	PLUTO_SAFETY_LOCK( cm, OrbiterLogic()->m_ScreenMutex );
	vector<DesignObj_DataGrid*>::iterator it;
	for(it = OrbiterLogic()->m_vectObjs_GridsOnScreen.begin(); it != OrbiterLogic()->m_vectObjs_GridsOnScreen.end(); ++it)
	{
		DesignObj_DataGrid* pDesignObj = *it;

		if(sDataGrid_ID=="*" || pDesignObj->m_sGridID == sDataGrid_ID)
		{
			PLUTO_SAFETY_LOCK( cm, OrbiterLogic()->m_DatagridMutex );
			OrbiterLogic()->InitializeGrid(pDesignObj);
			pDesignObj->Flush();
			RenderObjectAsync(pDesignObj);
		}
	}

	if( OrbiterLogic()->m_pScreenHistory_Current )
		RenderObjectAsync(OrbiterLogic()->m_pScreenHistory_Current->GetObj());

	NeedToRender render( OrbiterLogic(), "CMD_Refresh" );  // Redraw anything that was changed by this command
}

void OrbiterRenderer::RelativePosition(const PlutoRectangle& A, 
	const PlutoRectangle& B,
	int &Direction_Primary, int &Direction_Secondary, int &Distance)
{
	PlutoPoint A_center(A.X + A.Width / 2, A.Y + A.Height / 2);
	PlutoPoint B_center(B.X + B.Width / 2, B.Y + B.Height / 2);
	A_center.RelativePosition(B_center, Direction_Primary, Direction_Secondary, Distance);

	//... and now the adjustments:

	bool bAdjustments = false;
	PlutoRectangle rectB(B);

	if(Direction_Secondary == DIRECTION_Up_CONST)
	{
		PlutoRectangle rectUp(A.X + 1, 0, A.Width - 2, A.Y);
		if(rectB.IntersectsWith(rectUp))
		{
			bAdjustments = true;
			Direction_Primary = DIRECTION_Up_CONST;
		}
	}
	else if(Direction_Secondary == DIRECTION_Down_CONST)
	{
		PlutoRectangle rectDown(A.X + 1, A.Y + A.Height, A.Width - 2, OrbiterLogic()->m_iImageHeight);
		if(rectB.IntersectsWith(rectDown))
		{
			bAdjustments = true;
			Direction_Primary = DIRECTION_Down_CONST;
		}
	}
	else if(Direction_Secondary == DIRECTION_Right_CONST)
	{
		PlutoRectangle rectRight(A.X + A.Width, A.Y + 1, OrbiterLogic()->m_iImageWidth, A.Height - 2);
		if(rectB.IntersectsWith(rectRight))
		{
			bAdjustments = true;
			Direction_Primary = DIRECTION_Right_CONST;
		}
	}
	else if(Direction_Secondary == DIRECTION_Left_CONST)
	{
		PlutoRectangle rectLeft(0, A.Y + 1, A.X, A.Height - 2);
		if(rectB.IntersectsWith(rectLeft))
		{
			bAdjustments = true;
			Direction_Primary = DIRECTION_Left_CONST;
		}
	}
}

void OrbiterRenderer::DrawArrow(PlutoPoint p1, PlutoPoint p2, PlutoSize sizeArrow, 
								PlutoColor color, string ParentObjectID, string ObjectID) 
{
	DrawLine(p1.X, p1.Y, p2.X, p2.Y, color);

	PlutoPoint arrow_p1;
	PlutoPoint arrow_p2;
	CalcArrowValues(p1, p2, sizeArrow, arrow_p1, arrow_p2);

	DrawLine(p2.X, p2.Y, arrow_p1.X, arrow_p1.Y, color);
	DrawLine(p2.X, p2.Y, arrow_p2.X, arrow_p2.Y, color);
}

void OrbiterRenderer::CalcArrowValues(PlutoPoint p1, PlutoPoint p2, PlutoSize sizeArrow, PlutoPoint& arrow_p1, PlutoPoint& arrow_p2) 
{
	int al = sizeArrow.Width;
	int aw = sizeArrow.Height;
    int haw = aw / 2;

	if (p1.X == p2.X) 
	{ 
		if (p2.Y < p1.Y) 
		{
			arrow_p1.X = p2.X - haw;
			arrow_p1.Y = p2.Y + al;
			arrow_p2.X = p2.X + haw;
			arrow_p2.Y = p2.Y + al;
		}
		else 
		{
			arrow_p1.X = p2.X - haw;
			arrow_p1.Y = p2.Y - al;
			arrow_p2.X = p2.X + haw;
			arrow_p2.Y = p2.Y - al;
		}

		return;
	}	

	if (p1.Y == p2.Y) 
	{
		if (p2.X > p1.X) 
		{
			arrow_p1.X = p2.X - al;
			arrow_p1.Y = p2.Y - haw;
			arrow_p2.X = p2.X - al;
			arrow_p2.Y = p2.Y + haw;
		}
		else 
		{
			arrow_p1.X = p2.X + al;
			arrow_p1.Y = p2.Y - haw;
			arrow_p2.X = p2.X + al;
			arrow_p2.Y = p2.Y + haw;
		}

		return;
	}

	CalcArrowValuesQuad(p1, p2, sizeArrow, arrow_p1, arrow_p2);
}

void OrbiterRenderer::CalcArrowValuesQuad(PlutoPoint p1, PlutoPoint p2, PlutoSize sizeArrow, PlutoPoint& arrow_p1, PlutoPoint& arrow_p2) 
{ 
	int al = sizeArrow.Width;
	int aw = sizeArrow.Height;
    int haw = aw / 2;

	double arrowAng = atan(haw / static_cast<double>(al));
	double dist = sqrt(static_cast<double>(al * al + aw));
	double lineAng = atan(abs(p1.X - p2.X) / static_cast<double>(abs(p1.Y - p2.Y)));

	if (p1.X > p2.X) 
	{
		if (p1.Y > p2.Y) 
			lineAng = M_PI - lineAng;
	}
	else 
	{
		if (p1.Y > p2.Y) 
			lineAng = M_PI + lineAng;
		else 
			lineAng = 2 * M_PI - lineAng;
	}

	CalcArrowCoords(p2.X, p2.Y, dist, lineAng - arrowAng, arrow_p1);
	CalcArrowCoords(p2.X, p2.Y, dist, lineAng + arrowAng, arrow_p2);
}

void OrbiterRenderer::CalcArrowCoords(int x, int y, double dist, double dirn, PlutoPoint& arrow) 
{
	while(dirn < 0.0)   
		dirn = 2 * M_PI + dirn;
	while(dirn > 2 * M_PI) 
		dirn = dirn - 2 * M_PI;

	if (dirn <= M_PI / 2) 
	{
		arrow.X = x + (int) (sin(dirn) * dist);
		arrow.Y = y - (int) (cos(dirn) * dist);
		return;
	}
	if (dirn <= M_PI) 
	{
		arrow.X = x + (int) ((cos(dirn - M_PI / 2)) * dist);
		arrow.Y = y + (int) ((sin(dirn - M_PI / 2)) * dist);
		return;
	}
	if (dirn <= 3 * M_PI / 2) 
	{
		arrow.X = x - (int) ((sin(dirn - M_PI)) * dist);
		arrow.Y = y + (int) ((cos(dirn - M_PI)) * dist);
	}
	else 
	{
		arrow.X = x - (int) ((cos(dirn - 3 * M_PI / 2)) * dist);
		arrow.Y = y - (int) ((sin(dirn - 3 * M_PI / 2)) * dist);
	}
}      
