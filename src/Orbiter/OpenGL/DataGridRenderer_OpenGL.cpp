/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
/*
Orbiter

Copyright (C) 2006 Pluto, Inc., a Florida Corporation

www.plutohome.com

Phone: +1 (877) 758-8648

This program is distributed according to the terms of the Pluto Public License, available at:
http://plutohome.com/index.php?section=public_license

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

@author: "Ciprian Mustiata" ciprian.m at plutohome dot com, "Cristian Miron" chris.m at plutohome dot com 

*/
#include "DataGridRenderer_OpenGL.h"

#include "DCE/DataGrid.h"
#include "OpenGL3DEngine.h"
#include "OrbiterRenderer_OpenGL.h"
#include "DatagridAnimationManager.h"

#include "SDL.h"

#ifdef VIA_OVERLAY
	#include "VIA/ViaOverlay.h"
#endif

#ifdef USE_UIWINDOW
	#include "../Win32/UIWindowManager.h"
#endif

DataGridRenderer_OpenGL::DataGridRenderer_OpenGL(DesignObj_Orbiter *pOwner)
: DataGridRenderer(pOwner), m_pRenderFrame(NULL)
{
	Orbiter* pOrbiter = pOwner->m_pOrbiter;
	OrbiterRenderer_OpenGL* pRendererGL = dynamic_cast<OrbiterRenderer_OpenGL*>(pOrbiter->Renderer());
	if(NULL == pRendererGL)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Cannot get a reference to the OrbiterRenderer!... quiting");
		Engine = NULL;
		return;
	}
	this->Engine = pRendererGL->Engine;
	m_AnimationSpeed = 100;

}

DataGridRenderer_OpenGL::~DataGridRenderer_OpenGL(void)
{

}

void DataGridRenderer_OpenGL::Reset() 
{ 
	PLUTO_SAFETY_LOCK(cm, m_pObj_Owner->m_pOrbiter->m_ScreenMutex);
	StartAnimation = 0; 
	m_pRenderFrame = NULL;

	LoggerWrapper::GetInstance()->Write(LV_WARNING, "Datagrid %s reseted!", m_pObj_Owner->m_ObjectID.c_str());
}

/*virtual*/ void DataGridRenderer_OpenGL::RenderObject(DesignObj_Orbiter *pObj_Screen, PlutoPoint point/* = PlutoPoint(0, 0)*/)
{
	PLUTO_SAFETY_LOCK(cm, m_pObj_Owner->m_pOrbiter->m_ScreenMutex);

	if(!PreRenderActions(pObj_Screen, point))
		return;

#ifdef VIA_OVERLAY
	ViaOverlay::Instance().FillRectangleInAlphaMask(m_pObj_Owner->m_rPosition.X, m_pObj_Owner->m_rPosition.Y, 
		m_pObj_Owner->m_rPosition.Width, m_pObj_Owner->m_rPosition.Height, 255 - m_pObj_Owner->Renderer()->GetAlphaLevel());	
#endif

#ifdef USE_UIWINDOW
	UIWindowManager::UIWindow()->AddFilter(m_pObj_Owner->m_rPosition);
#endif

	//save the datagrid's mesh frame tree before rendering
	MeshFrame *BeforeDataGrid = m_pRenderFrame;

	string DatagridFrameID = "datagrid " + m_pObj_Owner->GenerateObjectHash(point, false);
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "DataGridRenderer_OpenGL::RenderObject %s, 'before datagrid': %p", DatagridFrameID.c_str(), m_pRenderFrame);

	//render datagrid in a frame
	if(0 != StartAnimation)
		Engine->EndModifyGeometry();

	TextureManager::Instance()->DisableCache();
	Engine->StartDatagridDrawing(DatagridFrameID);
	DataGridRenderer::RenderObject(pObj_Screen, point);
    m_pRenderFrame = Engine->EndDatagridDrawing(DatagridFrameID);
	TextureManager::Instance()->EnableCache();

	if(0 != StartAnimation)
		Engine->BeginModifyGeometry();

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "DataGridRenderer_OpenGL::RenderObject %s MIDDLE", DatagridFrameID.c_str());

	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, Engine->GetSceneMutex());
	
	if(0 != StartAnimation && BeforeDataGrid != NULL)
	{
		//suspend texture releases
		TextureManager::Instance()->SuspendTextureRelease();

		//create a clone
		MeshFrame *BeforeDataGridClone = BeforeDataGrid->Clone();

		//disconnect from parent
		BeforeDataGridClone->ResetParent();

		//replace old datagrid with the clone in the animation
		Engine->ReplaceMeshInAnimations(BeforeDataGrid, BeforeDataGridClone);

		//old datagrid already deleted when it was replaced with its clone
		BeforeDataGrid = NULL;

		//prepare animation
		vector<string> Dependencies;
		BuildDependencies(Dependencies);
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "DataGridRenderer_OpenGL::RenderObject StartAnimation %s B(%p) - A(%p)",
			DatagridFrameID.c_str(), BeforeDataGridClone, m_pRenderFrame);

		Engine->GetDatagridAnimationManager()->PrepareForAnimation(
			DatagridFrameID, BeforeDataGridClone, m_pRenderFrame, 
			m_AnimationSpeed, iPK_Direction, Dependencies);
		StartAnimation = 0;
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "DataGridRenderer_OpenGL::RenderObject normal render... stopping all animations! Current datagrid frame %p/%s",
			m_pRenderFrame, m_pRenderFrame->Name().c_str());

		Engine->GetDatagridAnimationManager()->StopAnimations();
		Engine->GetDatagridAnimationManager()->StopPendingAnimations();

		Engine->AddMeshFrameToDesktop("", m_pRenderFrame);
	}

    sm.Release();

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "abc DataGridRenderer_OpenGL::RenderObject ENDED %s", DatagridFrameID.c_str());
}

void DataGridRenderer_OpenGL::m_AnimationSpeed_set(int AnimationSpeed)
{
	m_AnimationSpeed=AnimationSpeed;
	for( vector<DesignObj_Orbiter *>::iterator it=m_pObj_Owner_DataGrid->m_vectObj_TiedToUs.begin();it!=m_pObj_Owner_DataGrid->m_vectObj_TiedToUs.end();++it )
	{
		DesignObj_Orbiter *pObj = *it;
		if( pObj->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST )
		{
			DataGridRenderer_OpenGL *pDataGridRenderer_OpenGL = (DataGridRenderer_OpenGL *) pObj->Renderer();
			pDataGridRenderer_OpenGL->m_AnimationSpeed_set(AnimationSpeed);
		}
	}
}

void DataGridRenderer_OpenGL::BuildDependencies(vector<string> & Dependencies)
{
	Dependencies.clear();
	for( vector<DesignObj_Orbiter *>::iterator it=m_pObj_Owner_DataGrid->m_vectObj_TiedToUs.begin();it!=m_pObj_Owner_DataGrid->m_vectObj_TiedToUs.end();++it )
	{
		DesignObj_Orbiter *pObj = *it;
		Dependencies.push_back("datagrid " + pObj->GenerateObjectHash(pObj->m_pPopupPoint, false));
	}

	if(NULL != m_pObj_Owner_DataGrid->m_pDesignObj_Orbiter_TiedTo)
	{
		//also the parent
		Dependencies.push_back("datagrid " + 
			m_pObj_Owner_DataGrid->m_pDesignObj_Orbiter_TiedTo->GenerateObjectHash(
				m_pObj_Owner_DataGrid->m_pDesignObj_Orbiter_TiedTo->m_pPopupPoint, false)
		);
	}	
}

