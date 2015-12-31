/*
 DesignObj_Orbiter
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */
 
#include "PlutoUtils/CommonIncludes.h"	
#include "DataGrid.h"
#include "RendererMNG.h"
#include "OrbiterRenderer.h"
#include "DesignObj_Orbiter.h"
#include "Orbiter.h"
#include "CallBackData.h"
#include "ScreenHandler.h"
#include "ObjectRenderer.h"
#include "../pluto_main/Define_Variable.h"

#include "Gen_Devices/AllCommandsRequests.h"

#ifdef PRONTO
#include "CCF.h"
#endif
//=======================================================================================================
//Concrete class DesignObj_Orbiter
//-------------------------------------------------------------------------------------------------------
DesignObj_Orbiter::DesignObj_Orbiter(Orbiter *pOrbiter)
{
	m_pDesignObj_Orbiter_Up=m_pDesignObj_Orbiter_Down=m_pDesignObj_Orbiter_Left=m_pDesignObj_Orbiter_Right=
		m_pDesignObj_Orbiter_TiedTo=NULL;
	m_pOrbiter = pOrbiter;

	m_vectGraphic.clear();
	m_vectSelectedGraphic.clear();
	m_vectHighlightedGraphic.clear();
	m_vectAltGraphics.clear();
	m_pvectCurrentGraphic = NULL;
	m_pGraphicToUndoSelect = NULL;
	m_pvectCurrentPlayingGraphic = NULL;
	m_pObj_Screen = NULL;
	m_pDesignObj_DataGrid = NULL;
	m_iGridRow=m_iGridCol=0;
	m_bIsARemoteControl = false;

	m_iCurrentFrame = 0;

	m_pCriteria=NULL;
	m_GraphicToDisplay=GRAPHIC_NORMAL;
	m_GraphicBeforeHighlight=GRAPHIC_NORMAL;
	m_GraphicToPlay=GRAPHIC_NORMAL;
	m_bOneTimeDontReset=false;
	m_pParentObject=NULL;
	m_pWebWindow=NULL;
	m_bHidden=false;
	m_pFloorplanObject=NULL;
#ifdef PRONTO
	m_pCCF=NULL;
#endif
	m_bDisableAspectLock=false;
	m_bOnScreen=false;
	m_bContainsDataGrid=false;
	m_bIsBoundToUser=m_bIsBoundToLocation=false;
};
//-------------------------------------------------------------------------------------------------------
/*virtual*/ bool DesignObj_Orbiter::Serialize(bool bWriting, char *&pcDataBlock, unsigned long &dwAllocatedSize, 
	char *&pcCurrentPosition, void *pExtraSerializationData)
{
	bool bResult = DesignObj_Data::Serialize(bWriting, pcDataBlock, dwAllocatedSize, pcCurrentPosition, pExtraSerializationData);
	m_pObjectRenderer = ObjectRendererFactory::CreateObjectRenderer(this, m_ObjectType);

	return bResult;
}
//-------------------------------------------------------------------------------------------------------
DesignObj_Orbiter::~DesignObj_Orbiter()
{
	for(list<class PlutoPopup*>::iterator it=m_listPopups.begin();it!=m_listPopups.end();++it)
		delete *it;
    m_listPopups.clear();

	m_pGraphicToUndoSelect = NULL;
	m_pvectCurrentGraphic = m_pvectCurrentPlayingGraphic = NULL;

	for(VectorPlutoGraphic::iterator itGraphic = m_vectGraphic.begin(); itGraphic != m_vectGraphic.end(); ++itGraphic)
		RELEASE_GRAPHIC(*itGraphic);
	m_vectGraphic.clear();

	for(VectorPlutoGraphic::iterator itSelectedGraphic = m_vectSelectedGraphic.begin(); itSelectedGraphic != m_vectSelectedGraphic.end(); ++itSelectedGraphic)
		RELEASE_GRAPHIC(*itSelectedGraphic);
	m_vectSelectedGraphic.clear();

	for(VectorPlutoGraphic::iterator itHighlightedGraphic = m_vectHighlightedGraphic.begin(); itHighlightedGraphic != m_vectHighlightedGraphic.end(); ++itHighlightedGraphic)
		RELEASE_GRAPHIC(*itHighlightedGraphic);
	m_vectHighlightedGraphic.clear();

	for(vector<VectorPlutoGraphic>::iterator itVectAltGraphic = m_vectAltGraphics.begin(); itVectAltGraphic != m_vectAltGraphics.end(); ++itVectAltGraphic)
	{
		for(VectorPlutoGraphic::iterator itAltGraphic = itVectAltGraphic->begin(); itAltGraphic != itVectAltGraphic->end(); ++itAltGraphic)
			RELEASE_GRAPHIC(*itAltGraphic);

		itVectAltGraphic->clear();
	}
	m_vectAltGraphics.clear();


	m_bvectLoop_Alt.clear();

	DesignObjZoneList::iterator iZone;
	for(iZone = m_ZoneList.begin(); iZone !=m_ZoneList.end(); ++iZone)
		delete *iZone; 

	DesignObjCommandList::iterator iA;
	for(iA = m_Action_StartupList.begin(); iA != m_Action_StartupList.end(); ++iA)
		delete *iA;
	for(iA = m_Action_TimeoutList.begin(); iA != m_Action_TimeoutList.end(); ++iA)
		delete *iA;
	for(iA = m_Action_LoadList.begin(); iA != m_Action_LoadList.end(); ++iA)
		delete *iA; 
	for(iA = m_Action_UnloadList.begin(); iA != m_Action_UnloadList.end(); ++iA)
		delete *iA; 
	for(iA = m_Action_HighlightList.begin(); iA != m_Action_HighlightList.end(); ++iA)
		delete *iA; 
	for(iA = m_Action_UnhighlightList.begin(); iA != m_Action_UnhighlightList.end(); ++iA)
		delete *iA; 
	VectorDesignObjText::iterator iT;
	for(iT=m_vectDesignObjText.begin(); iT != m_vectDesignObjText.end(); ++iT)
	{
		delete *iT; 
	}

	//because all DesignObj_Orbiter objects are stored on Orbiter::m_map_ObjAll,
	//we don't want to delete here children of the object 
	//their children will be deleted in Orbiter destructor using map_ObjAll
	m_ChildObjects.clear();

#ifdef PRONTO
	delete m_pCCF;
#endif

	delete m_pObjectRenderer;
};
//-------------------------------------------------------------------------------------------------------
bool DesignObj_Orbiter::IsHidden() 
{ 
	if(m_bHidden) 
		return true;  
	
	if(m_pParentObject) 
		return ((DesignObj_Orbiter *) m_pParentObject)->IsHidden(); 

	return false; 
}
//-------------------------------------------------------------------------------------------------------
/*virtual*/ void DesignObj_Orbiter::RenderGraphic(PlutoRectangle rectTotal, bool bDisableAspectRatio, PlutoPoint point)
{
	m_pObjectRenderer->RenderGraphic(rectTotal, bDisableAspectRatio, point);
}
//-------------------------------------------------------------------------------------------------------
string DesignObj_Orbiter::GetParameterValue(int ParameterID,bool bSubstitute)
{
	map<int,string>::iterator ipParm = m_mapObjParms.find(ParameterID);
	if (ipParm==m_mapObjParms.end())
		return "";

	if( !bSubstitute )
		return (*ipParm).second;

	return m_pOrbiter->SubstituteVariables((*ipParm).second,this,0,0);
}
//-------------------------------------------------------------------------------------------------------
PlutoRectangle DesignObj_Orbiter::GetHighlightRegion()
{
	PlutoRectangle r;

	r.X = max(0,m_rPosition.X-4);
	r.Y = max(0,m_rPosition.Y-4);
	r.Right( min(m_rPosition.Right()+4,m_pOrbiter->m_Width-1) );
	r.Bottom( min(m_rPosition.Bottom()+4,m_pOrbiter->m_Height-1) );
	return r;
}
//-------------------------------------------------------------------------------------------------------
string DesignObj_Orbiter::GetVariableAssignment(int PK_Variable)
{
	for(DesignObjZoneList::iterator itZ=m_ZoneList.begin();itZ!=m_ZoneList.end();++itZ)
	{
		DesignObjZone *pDesignObjZone = *itZ;

		for(DesignObjCommandList::iterator it=pDesignObjZone->m_Commands.begin();it!=pDesignObjZone->m_Commands.end();++it)
		{
			DesignObjCommand *pDesignObjCommand = *it;
			if( pDesignObjCommand->m_PK_Command==COMMAND_Set_Variable_CONST )
			{
				int PK_Variable2 = atoi(pDesignObjCommand->m_ParameterList[COMMANDPARAMETER_PK_Variable_CONST].c_str());
				if( PK_Variable == PK_Variable2 )
//				if( (!bDescription && PK_Variable == VARIABLE_Array_ID_CONST) || (bDescription && PK_Variable == VARIABLE_Array_Desc_CONST) )
					return pDesignObjCommand->m_ParameterList[COMMANDPARAMETER_Value_To_Assign_CONST];
			}
		}
	}
	return "";
}
//-------------------------------------------------------------------------------------------------------
/*virtual*/ void DesignObj_Orbiter::RenderObject(DesignObj_Orbiter *pObj_Screen, PlutoPoint point)
{
	m_pObjectRenderer->RenderObject(pObj_Screen, point);
}
//-------------------------------------------------------------------------------------------------------
/*virtual*/ void DesignObj_Orbiter::Flush(bool bFlushGraphics)
{ 
} 
//-------------------------------------------------------------------------------------------------------
/*virtual*/ bool DesignObj_Orbiter::PreRenderActions(DesignObj_Orbiter *pObj_Screen, PlutoPoint point)
{
	return m_pObjectRenderer->PreRenderActions(pObj_Screen, point);
}
//-------------------------------------------------------------------------------------------------------
/*virtual*/ bool DesignObj_Orbiter::PostRenderActions(DesignObj_Orbiter *pObj_Screen, PlutoPoint point)
{
	return m_pObjectRenderer->PostRenderActions(pObj_Screen, point);	
}
//-------------------------------------------------------------------------------------------------------
string DesignObj_Orbiter::GenerateObjectHash(const PlutoPoint& point, bool bObjectState/* = true*/)
{
	if(bObjectState)
		return m_ObjectID + 
			"-" + StringUtils::ltos(m_rPosition.X) +
			":" + StringUtils::ltos(m_rPosition.Y) + 
			"-" + StringUtils::ltos(point.X) + 
			":" + StringUtils::ltos(point.Y) + 
			"G:" + StringUtils::ltos(m_GraphicToDisplay);
	else
		return m_ObjectID;
}

DesignObj_Orbiter *DesignObj_Orbiter::FindChildObjectWithBaseID(int PK_DesignObj)
{
	DesignObj_DataList::iterator iHao;
	for( iHao=m_ChildObjects.begin(  ); iHao != m_ChildObjects.end(  ); ++iHao )
	{
		DesignObj_Orbiter *pObj = (DesignObj_Orbiter *) *iHao;
		if( pObj->m_iBaseObjectID==PK_DesignObj )
			return pObj;
	}

	// Recurse
	for( iHao=m_ChildObjects.begin(  ); iHao != m_ChildObjects.end(  ); ++iHao )
	{
		DesignObj_Orbiter *pObj = (DesignObj_Orbiter *) *iHao;
		DesignObj_Orbiter *pObj_Child = pObj->FindChildObjectWithBaseID(PK_DesignObj);
		if( pObj_Child )
			return pObj_Child;
	}
	return NULL;
}


void DesignObj_Orbiter::m_GraphicToDisplay_set(string sCalling,int GraphicToDisplay,bool bRecurseChildren,bool bSetUnhighlightedState)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"m_GraphicToDisplay_set %s %s=%d",sCalling.c_str(),m_ObjectID.c_str(),GraphicToDisplay);
#endif

	m_GraphicToDisplay=GraphicToDisplay;
	if( bSetUnhighlightedState )
		m_GraphicBeforeHighlight=GraphicToDisplay;
	if( bRecurseChildren )
		for(DesignObj_DataList::iterator it=m_ChildObjects.begin();it!=m_ChildObjects.end();++it)
			((DesignObj_Orbiter *)(*it))->m_GraphicToDisplay_set("recurse",GraphicToDisplay,true,bSetUnhighlightedState);
}
