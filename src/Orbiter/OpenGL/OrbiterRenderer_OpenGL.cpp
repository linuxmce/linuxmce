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
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */

#include "OrbiterRenderer_OpenGL.h"
//-----------------------------------------------------------------------------------------------------
#ifdef WIN32
	#include "MainDialog.h"
	#include "PopupMessage.h"
	#include "ProgressDialog.h"
#endif	
//-----------------------------------------------------------------------------------------------------
#include "Orbiter.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "../ScreenHistory.h"
#include "../../pluto_main/Define_Button.h"
#include "DataGrid.h" 
#include "DataGridRenderer.h"
#include "ScreenHandler.h"
#include "../../Splitter/TextWrapper.h"
#include "../../SDL_Helpers/SDL_Defs.h"
//-----------------------------------------------------------------------------------------------------
#include "GLMathUtils.h"
#include "Widgets/basicwindow.h"
#include "Widgets/DrawingWidgetsEngine.h"
#include "Effects/gl2deffecttransit.h"
#include "Effects/gl2deffectbeziertranzit.h"
#include "Effects/gl2deffectslidefromleft.h"
#include "Effects/gl2deffectfadesfromtop.h"
#include "Effects/gl2deffectfadesfromunderneath.h"
#include "Simulator.h" 
#include "OpenGLGraphic.h"
#include "OpenGL3DEngine.h"
#include "Mesh/MeshFrame.h"
#include "Mesh/MeshBuilder.h"
#include "ObjectRenderer_OpenGL.h"
#include "DataGridRenderer_OpenGL.h"
//-----------------------------------------------------------------------------------------------------
#ifdef VIA_OVERLAY
	#include "VIA/ViaOverlay.h"
#endif

#ifdef USE_UIWINDOW
	#include "../Win32/UIWindowManager.h"
#endif
//-----------------------------------------------------------------------------------------------------
#include <SDL_ttf.h>
//-----------------------------------------------------------------------------------------------------
#include "../../pluto_main/Define_Effect.h"
//-----------------------------------------------------------------------------------------------------
class SDL_Finalizer
{
public:
	~SDL_Finalizer()
	{
		TTF_Quit();
		SDL_Quit();
	}
};
//-----------------------------------------------------------------------------------------------------
static SDL_Finalizer sdl_finalizer;
//-----------------------------------------------------------------------------------------------------
void *OrbiterRenderer_OpenGLThread(void *p)
{
	OrbiterRenderer_OpenGL * pOrbiterRenderer = (OrbiterRenderer_OpenGL *)p;
	if(NULL == pOrbiterRenderer)
		throw string("OpenGL thread started, but Orbiter is null!");

	int Width = pOrbiterRenderer->OrbiterLogic()->m_iImageWidth;
	int Height = pOrbiterRenderer->OrbiterLogic()->m_iImageHeight;

	pOrbiterRenderer->Engine = new OpenGL3DEngine();
	
	bool bFullScreen = Simulator::GetInstance()->m_bFullScreen;
	if(!pOrbiterRenderer->Engine->GL.InitVideoMode(Width, Height, 32, bFullScreen, pOrbiterRenderer->m_pOrbiter->m_bUseComposite))
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Thread -- GL.InitVideoMode FAILED Width: %d Height: %d Full Screen: %d Composite: %d",
			Width, Height, (int) bFullScreen, (int) pOrbiterRenderer->m_pOrbiter->m_bUseComposite);
		pOrbiterRenderer->OrbiterLogic()->OnReload();
		if( pOrbiterRenderer->m_pOrbiter->m_bUseComposite )
			pOrbiterRenderer->PromptUser("Orbiter failed to setup the transparency. Please check if the transparency manager is running!", 300);
		else
			pOrbiterRenderer->PromptUser("Orbiter failed to initialize OpenGL", 300);
		pthread_cond_broadcast(&(pOrbiterRenderer->Condition));
		return NULL;
	}
	else 
	{
		std::cout << "*** Thread -- GL.InitVideoMode SUCCEEDED" << std::endl;
	}

	pOrbiterRenderer->InitializeAfterSetVideoMode();
	pOrbiterRenderer->SetupWindow();
	pOrbiterRenderer->WindowCreated();

	pOrbiterRenderer->Engine->Setup();

	pOrbiterRenderer->Engine->m_bUseMask = !pOrbiterRenderer->OrbiterLogic()->m_bUseComposite;
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "Using masked UI? %s", pOrbiterRenderer->Engine->m_bUseMask ? "YES" : "NO");

	//wake up every one. the window is created and ready to be used
	pthread_cond_broadcast(&(pOrbiterRenderer->Condition));
	pOrbiterRenderer->Engine->Paint();

	while(pOrbiterRenderer->Engine && !pOrbiterRenderer->Engine->m_bQuit)
	{
#ifdef WIN32	
		SDL_Event SDL_Event_Pending;
		while(SDL_PollEvent(&SDL_Event_Pending) && !pOrbiterRenderer->Engine->m_bQuit)
			SDL_PushEvent(&SDL_Event_Pending);
#endif			

		if(!pOrbiterRenderer->NeedToUpdateScreen())
			Sleep(20);

		if(!pOrbiterRenderer->Engine->NeedUpdateScreen())
			pOrbiterRenderer->ScreenUpdated();

		if(!pOrbiterRenderer->OrbiterLogic()->m_bQuit_get())
			pOrbiterRenderer->Engine->Paint();

		pOrbiterRenderer->OnIdle();
		Sleep(60);
	}

	delete pOrbiterRenderer->Engine;
	pOrbiterRenderer->Engine = NULL;

	return NULL; 
}
//-----------------------------------------------------------------------------------------------------
OrbiterRenderer_OpenGL::OrbiterRenderer_OpenGL(Orbiter *pOrbiter) : 
	OrbiterRenderer(pOrbiter), NeedToUpdateScreen_(false), m_bWindowCreated(false), Popups(NULL), 
	m_pLastHighlightedObject(NULL), Engine(NULL), Mutex("open gl")
{
	std::cout << "*** OrbiterRenderer_OpenGL::OrbiterRenderer_OpenGL()" << std::endl;
	GLThread = 0;
	pthread_cond_init(&Condition, NULL);
	Mutex.Init(NULL, &Condition);
	m_spPendingGLEffects.reset(new PendingGLEffects());

	m_pObj_Floorplan_Current = NULL;
	m_pGraphic_Floorplan_Current = NULL;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ OrbiterRenderer_OpenGL::~OrbiterRenderer_OpenGL()
{
	delete Popups;
	Popups = NULL;

	if(NULL != Engine)
		Engine->m_bQuit=true;

	if(GLThread != 0)
		pthread_join(GLThread, NULL);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::Configure()
{
	if(OrbiterLogic()->m_bMemoryManagementEnabled)
		TextureManager::Instance()->EnableMemoryManagement();

	pthread_cond_init(&Condition, NULL);
	Mutex.Init(NULL, &Condition);

	pthread_create(&GLThread, NULL, OrbiterRenderer_OpenGLThread, (void*)this);	

	PLUTO_SAFETY_LOCK_ERRORSONLY(cm, Mutex);// Keep this locked to protect the map
	cm.CondWait();
	Popups = new PopupCollection(Engine);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::PostInitializeActions()
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterRenderer_OpenGL::PostInitializeActions");
	
	if(!OrbiterLogic()->m_bQuit_get()&& !IsWindowCreated())
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterRenderer_OpenGL::PostInitializeActions waiting the window to be created...");
		
		//wait here until the window will be created
		PLUTO_SAFETY_LOCK_ERRORSONLY(cm, Mutex);// Keep this locked to protect the map
		cm.CondWait();
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterRenderer_OpenGL::PostInitializeActions window created!");
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterRenderer_OpenGL::PostInitializeActions window already created!");
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterRenderer_OpenGL::PostInitializeActions END");
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::GetWindowPosition(PlutoPoint& point)
{

}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::UpdateScreen()
{
	NeedToUpdateScreen_ = true;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::SolidRectangle(int x, int y, int width, int height, PlutoColor color, 
	string ParentObjectID/* = ""*/, string ObjectID /* = ""*/)
{
	PlutoRectangle Position(x, y, width, height);
	MeshContainer* Container = MeshBuilder::BuildRectangle(Position, NULL);

	Point3D Color(color.R() / 255.0f, color.G() / 255.0f, color.B() / 255.0f);
	Container->SetColor(Color);
	Container->SetAlpha(color.A() / 255.0f);

	string RectangleUniqueID; 
	if ("" == ObjectID)
		RectangleUniqueID = "rectangle " + 
			StringUtils::ltos(x) + "," + StringUtils::ltos(y) + "," +
			StringUtils::ltos(width) + "," + StringUtils::ltos(height);
	else
		RectangleUniqueID = ObjectID;

	MeshFrame* Frame = new MeshFrame(RectangleUniqueID, Container);
	Frame->MarkAsVolatile();

	Engine->AddMeshFrameToDesktop(ParentObjectID, Frame);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::HollowRectangle(int X, int Y, int Width, int Height, PlutoColor color, 
	string ParentObjectID /* = ""*/, string ObjectID/* = ""*/)
{
	string RectangleUniqueID; 
	if ("" == ObjectID)
		RectangleUniqueID = "rectangle " + 
			StringUtils::ltos(X) + "," + StringUtils::ltos(Y) + "," +
			StringUtils::ltos(Width) + "," + StringUtils::ltos(Height);
	else
		RectangleUniqueID = ObjectID;

	color = PlutoColor::Red();
	MeshFrame * LeftBar = new MeshFrame("highlight-frame-left");
	MeshFrame * TopBar = new MeshFrame("highlight-frame-top");
	MeshFrame * RightBar = new MeshFrame("highlight-frame-right");
	MeshFrame * BottomBar = new MeshFrame("highlight-frame-bottom");
	
	const int HollowThickness = 4;
	PlutoRectangle Rect(X - HollowThickness, Y - HollowThickness, 
		Width + 2 * HollowThickness, Height + 2 * HollowThickness);
	PlutoRectangle Original (Rect);

	//left
	Rect.Width = HollowThickness;
	LeftBar->SetMeshContainer(MeshBuilder::BuildRectangle(Rect, NULL));

	//top
	Rect = Original;
	Rect.Height = HollowThickness;
	TopBar->SetMeshContainer(MeshBuilder::BuildRectangle(Rect, NULL));
	
	//right
	Rect = Original;
	Rect.X += Rect.Width-HollowThickness;
	Rect.Width = HollowThickness;
	RightBar->SetMeshContainer(MeshBuilder::BuildRectangle(Rect, NULL));

	//bottom
	Rect = Original;
	Rect.Y += Rect.Height-HollowThickness;
	Rect.Height = HollowThickness;
	BottomBar->SetMeshContainer(MeshBuilder::BuildRectangle(Rect, NULL));

	MeshFrame* HighLightPopup = new MeshFrame(RectangleUniqueID);
	HighLightPopup->AddChild(LeftBar);
	HighLightPopup->AddChild(TopBar);
	HighLightPopup->AddChild(RightBar);
	HighLightPopup->AddChild(BottomBar);
	Point3D Red(1.0f, 0.0f, 0.0f);
	HighLightPopup->SetColor(Red);

	HighLightPopup->MarkAsVolatileRecursively();
	Engine->AddMeshFrameToDesktop(ParentObjectID, HighLightPopup);
	
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::UnSelectObject(string ParentObjectID /* = ""*/, string ObjectID/* = ""*/)
{
	Engine->RemoveMeshFrameFromDesktopForID(ObjectID);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::PrepareFloorplan(DesignObj_Orbiter *pObj_Floorplan)
{
	Engine->UnHighlight();
	Engine->InvalidateFramesStartingWith(FLOORPLAN_OBJ_NAME);
	Engine->InvalidateFramesStartingWith(HOLLOW_OBJ_NAME);

	if(NULL != pObj_Floorplan && !pObj_Floorplan->m_vectGraphic.empty())
	{
		PlutoGraphic *pPlutoGraphic = pObj_Floorplan->m_vectGraphic[0];
		OpenGLGraphic *pOpenGLGraphic = dynamic_cast<OpenGLGraphic *>(pPlutoGraphic);

		m_pObj_Floorplan_Current = pObj_Floorplan;
		m_pGraphic_Floorplan_Current = pOpenGLGraphic;

		if(NULL != pOpenGLGraphic)
		{
			ObjectRenderer_OpenGL* pRenderer = dynamic_cast <ObjectRenderer_OpenGL*>(pObj_Floorplan->Renderer());

			if(NULL != pRenderer)
                pRenderer->LoadPicture(pOpenGLGraphic, true);
    	}
	}
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::UpdateFloorplan(DesignObj_Orbiter *pObj_Floorplan) 
{
	if(NULL != m_pObj_Floorplan_Current && NULL != m_pGraphic_Floorplan_Current)
	{
		m_pObj_Floorplan_Current->Renderer()->RenderGraphic(
			m_pObj_Floorplan_Current->m_rPosition,
			m_pObj_Floorplan_Current->m_bDisableAspectLock,
			m_pObj_Floorplan_Current->m_pPopupPoint
		);
	}

	m_pGraphic_Floorplan_Current = NULL;
	m_pObj_Floorplan_Current = NULL;
}	
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::ReplaceColorInRectangle(int x, int y, int width, int height, 
	PlutoColor ColorToReplace, PlutoColor ReplacementColor, DesignObj_Orbiter *pObj/*=NULL*/)
{
	if(NULL == m_pObj_Floorplan_Current || NULL == m_pGraphic_Floorplan_Current)
		return;

	PlutoRectangle Area(
		x - m_pObj_Floorplan_Current->m_rPosition.X - m_pObj_Floorplan_Current->m_pPopupPoint.X, 
		y - m_pObj_Floorplan_Current->m_rPosition.Y - m_pObj_Floorplan_Current->m_pPopupPoint.Y, 
		width, 
		height
	);

	m_pGraphic_Floorplan_Current->ReplaceColorInRectangle(Area, ColorToReplace, ReplacementColor);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::RenderText(string &sTextToDisplay, DesignObjText *Text,
	TextStyle *pTextStyle, PlutoPoint point/* = PlutoPoint(0, 0)*/, string ObjectID/* = ""*/)
{
	PLUTO_SAFETY_LOCK(cm,OrbiterLogic()->m_ScreenMutex);

	PlutoRectangle rectPosition(Text->m_rPosition);
	rectPosition.X += point.X;
	rectPosition.Y += point.Y;

	string sParentObjectID = "";
	if(NULL != Text->m_pObject)
	{
		sParentObjectID = Text->m_pObject->GenerateObjectHash(Text->m_pObject->m_pPopupPoint, false);

		SolidRectangle(rectPosition.X, rectPosition.Y, rectPosition.Width, rectPosition.Height,
			pTextStyle->m_BackColor, sParentObjectID);
	}

	string TextUniqueID = ObjectID;
	if(TextUniqueID == "")
		TextUniqueID = 
			"text " +
			(NULL != Text->m_pObject ? Text->m_pObject->m_ObjectID : string()) + 
			"-" + StringUtils::itos(Text->m_rPosition.X) + 
			"-" + StringUtils::itos(Text->m_rPosition.Y);

#ifdef WIN32
	char pWindowsDirector[MAX_PATH];
	GetWindowsDirectory(pWindowsDirector, MAX_PATH);
	string BasePath = string(pWindowsDirector) + "\\Fonts\\";
#else
	string BasePath="/usr/share/fonts/truetype/msttcorefonts/";
#endif //win32

	SDL_Surface *pSurface = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA, rectPosition.Width, rectPosition.Height, 32, 
		rmask, gmask, bmask, amask);

	if(NULL == pSurface)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Not enough memory!");
		return;
	}

	SDL_Rect rectBar;
	rectBar.x = 0; rectBar.y = 0; rectBar.w = rectPosition.Width; rectBar.h = rectPosition.Height;
	SDL_FillRect(pSurface, &rectBar, SDL_MapRGBA(pSurface->format, 0, 0, 0, 0));

	WrapAndRenderText(pSurface, sTextToDisplay, 0, 0, rectPosition.Width, rectPosition.Height, BasePath,
		pTextStyle,Text->m_iPK_HorizAlignment,Text->m_iPK_VertAlignment, &OrbiterLogic()->m_mapTextStyle);

	OpenGLGraphic *pGraphic = new OpenGLGraphic(pSurface);
	pGraphic->m_Filename = TextUniqueID;
	MeshContainer* Container = MeshBuilder::BuildRectangle(rectPosition, pGraphic);

	MeshFrame* Frame = new MeshFrame(TextUniqueID);
	Frame->MarkAsVolatile();
	Frame->SetMeshContainer(Container);

	TextureManager::Instance()->PrepareConvert(pGraphic);
	Engine->AddMeshFrameToDesktop(sParentObjectID, Frame);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::SaveBackgroundForDeselect(DesignObj_Orbiter *pObj, PlutoPoint point)
{

}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ PlutoGraphic *OrbiterRenderer_OpenGL::GetBackground( PlutoRectangle &rect ) 
{
	//todo
	return NULL;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ PlutoGraphic *OrbiterRenderer_OpenGL::CreateGraphic()
{
	return new OpenGLGraphic(this);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::RenderGraphic(class PlutoGraphic *pPlutoGraphic, 
	PlutoRectangle rectTotal, bool bDisableAspectRatio, PlutoPoint point/* = PlutoPoint(0, 0)*/,
	int nAlphaChannel/* = 255*/, string ParentObjectID/* = ""*/, string ObjectID/* = ""*/, 
	string ObjectHash/* = ""*/)
{
	if(TextureManager::Instance()->CacheEnabled() && ObjectID == "background-image-test")
	{
		MeshFrame* Frame = TextureManager::Instance()->GetCacheItem(ObjectID);
		if(NULL != Frame)
		{
			TextureManager::Instance()->AttachToScene(ParentObjectID, Frame);
			return;
		}
	}

g_PlutoProfiler->Start("ObjectRenderer_OpenGL::RenderGraphic2");
	if(ObjectID == "")
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "RenderGraphic with no object id!");
	}
	OpenGLGraphic* Graphic = dynamic_cast<OpenGLGraphic*> (pPlutoGraphic);
	if(Graphic == NULL)
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Graphic to render is NULL!");
		return;
	}

	PlutoRectangle Position(rectTotal);
	Position.X+= point.X;
	Position.Y+= point.Y;
	float ZoomX = 1.0f;
	float ZoomY = 1.0f;

	//we'll have to keep the aspect
	if(!bDisableAspectRatio) 
	{
		ZoomX = float(Graphic->m_nWidth) / rectTotal.Width;
		ZoomY = float(Graphic->m_nHeight) / rectTotal.Height;
		if (ZoomX > ZoomY)
		{
			ZoomY = ZoomY / ZoomX;
			ZoomX = 1.0f;
		}
		else
		{
			ZoomX = ZoomX / ZoomY;
			ZoomY = 1.0f;
		}
	}

	MeshContainer* Container = MeshBuilder::BuildRectangle(Position, Graphic);
	Container->SetAlpha(nAlphaChannel / 255.0f);
	MeshFrame* Frame = new MeshFrame(ObjectID);
	
	Frame->SetMeshContainer(Container);

	MeshTransform AspectRatioTransform;
	AspectRatioTransform.ApplyTranslate((float)-Position.X, (float)-Position.Y, 0.0f);
	AspectRatioTransform.ApplyScale((float)ZoomX, (float)ZoomY, 1.0f);
	AspectRatioTransform.ApplyTranslate(
		(float)Position.X + Position.Width*(1 - ZoomX)/2.0f, 
		(float)Position.Y + Position.Height*(1 - ZoomY)/2.0f, 
		0.0f);
	Frame->ApplyTransform(AspectRatioTransform);

	TextureManager::Instance()->PrepareConvert(Graphic);

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "AddMeshFrameToDesktop (%d,%d,%d,%d)",
		point.X, point.Y, rectTotal.Width, rectTotal.Height);
#endif

	//make datagrid's thumbs opaque
	if(Frame->Name().find(DATAGRID_THUMB_NAME) == 0)
	{
#ifdef VIA_OVERLAY
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "VIA Rendering a datagrid thumb : %d %d %d %d", 
			point.X + rectTotal.X, point.Y + rectTotal.Y, rectTotal.Width, rectTotal.Height);
		ViaOverlay::Instance().FillRectangleInAlphaMask(point.X + rectTotal.X, point.Y + rectTotal.Y, rectTotal.Width, rectTotal.Height, 0x00, true);
#endif

#ifdef USE_UIWINDOW
		PlutoRectangle rectFilter(point.X + rectTotal.X, point.Y + rectTotal.Y, rectTotal.Width, rectTotal.Height);
		UIWindowManager::UIWindow()->AddFilter(rectFilter);
#endif
	}

	Graphic->AddRef(); //add a reference to this object

	Engine->AddMeshFrameToDesktop(ParentObjectID, Frame);

g_PlutoProfiler->Stop("ObjectRenderer_OpenGL::RenderGraphic2");

	if(TextureManager::Instance()->CacheEnabled())
	{
		if(
			(!OrbiterLogic()->m_bMemoryManagementEnabled || point.X != 0 || point.Y != 0)
			&& 
			Graphic->m_Filename.find(DYNAMIC_OBJ_NAME) != 0
		)
		{
			TextureManager::Instance()->AddCacheItem(ObjectHash, Frame);
		}
		else
			Frame->MarkAsVolatile();
	}
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::BeginPaint()
{
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::EndPaint()
{
	NeedToUpdateScreen_ = true;
	Engine->RefreshScreen();
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::UpdateRect(PlutoRectangle rect, PlutoPoint point/*= PlutoPoint(0,0)*/)
{
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::AdjustWindowSize(int iWidth, int iHeight)
{

}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_OpenGL::WakeupFromCondWait()
{
	pthread_cond_broadcast(&Condition);
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_OpenGL::OnIdle()
{
	//WakeupFromCondWait();
	Sleep(5);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::UnHighlightObject(bool bDeleteOnly/*=false*/)
{
	DesignObj_Orbiter *pObj = OrbiterLogic()->m_pObj_Highlighted;

	if(NULL != pObj)
	{
		DesignObj_Orbiter *pTopParent = (DesignObj_Orbiter *)pObj->TopMostObject();
		if(NULL != pTopParent && pTopParent->m_pPopupPoint.X != 0 || pTopParent->m_pPopupPoint.Y != 0)
		{
			//reset the object
			Popups->ResetObjectInPopup(
				pTopParent->GenerateObjectHash(pTopParent->m_pPopupPoint),
				pObj
			);

			//reset its children
			for(DesignObj_DataList::iterator it=pObj->m_ChildObjects.begin();it!=pObj->m_ChildObjects.end();++it)
				Popups->ResetObjectInPopup(
					pTopParent->GenerateObjectHash(pTopParent->m_pPopupPoint),
					(DesignObj_Orbiter *)(*it)
				);
		}
		else
		{
			pObj->m_GraphicToDisplay_set("ogl1",pObj->m_GraphicBeforeHighlight);
			pObj->m_pvectCurrentGraphic = 
				pObj->m_GraphicToDisplay == GRAPHIC_NORMAL ?
					&(pObj->m_vectGraphic) :
					&(pObj->m_vectSelectedGraphic);

			RenderObjectAsync(pObj);
		}
	}

	OrbiterLogic()->m_pObj_Highlighted_set(NULL);

	if(NULL != Engine)
		Engine->UnHighlight();
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::DoHighlightObject()
{
	if(sbNoSelection == OrbiterLogic()->m_nSelectionBehaviour || !OrbiterLogic()->m_pObj_Highlighted || !OrbiterLogic()->m_pObj_Highlighted->m_bOnScreen )
		return;

	CallBackData *pCallBackData = OrbiterLogic()->m_pScreenHandler_get()->m_mapCallBackData_Find(cbObjectHighlighted);
	if(pCallBackData)
	{
		ObjectInfoBackData *pObjectData = (ObjectInfoBackData *)pCallBackData;
		pObjectData->m_pObj = OrbiterLogic()->m_pObj_Highlighted;
		pObjectData->m_PK_DesignObj_SelectedObject = OrbiterLogic()->m_pObj_Highlighted->m_iBaseObjectID;
	}
	if(OrbiterLogic()->ExecuteScreenHandlerCallback(cbObjectHighlighted))
		return;

	if(NULL != m_pLastHighlightedObject &&	m_pLastHighlightedObject != OrbiterLogic()->m_pObj_Highlighted)
	{
		DesignObj_Orbiter *pCurrentHighlightedObject = OrbiterLogic()->m_pObj_Highlighted;
		OrbiterLogic()->m_pObj_Highlighted_set(m_pLastHighlightedObject);
		UnHighlightObject();
		OrbiterLogic()->m_pObj_Highlighted_set(pCurrentHighlightedObject);
	}

	if(!OrbiterLogic()->m_pObj_Highlighted)
		return;

	m_pLastHighlightedObject = OrbiterLogic()->m_pObj_Highlighted;
	DesignObj_Orbiter *pObj = OrbiterLogic()->m_pObj_Highlighted;
	PlutoRectangle Position(pObj->m_rPosition);

	OrbiterLogic()->ExecuteCommandsInList( &pObj->m_Action_HighlightList, 
		pObj, smHighlight, 0, 0 );

	if( pObj->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST )
		OrbiterLogic()->GetDataGridHighlightCellCoordinates((DesignObj_DataGrid *)pObj, Position);

	Position.X += OrbiterLogic()->m_pObj_Highlighted->m_pPopupPoint.X;
	Position.Y += OrbiterLogic()->m_pObj_Highlighted->m_pPopupPoint.Y;

	Engine->Highlight(Position, NULL);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::SelectObject( DesignObj_Orbiter *pObj, PlutoPoint point )
{
	GLEffect2D::EffectFactory::GetEffectCode(rand()%9);
	Engine->Select(&pObj->m_rPosition);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::EventLoop()
{
	SDL_EventState(SDL_VIDEOEXPOSE, SDL_ENABLE);
	SDL_EventState(SDL_VIDEORESIZE, SDL_ENABLE);

	int SDL_Event_Pending = 0;

	SDL_Event Event;
	while (!OrbiterLogic()->m_bQuit_get()&& !OrbiterLogic()->m_bReload)
	{
		SDL_Event_Pending = SDL_PollEvent(&Event);

		if (SDL_Event_Pending)
		{
			Orbiter::Event orbiterEvent;
			orbiterEvent.type = Orbiter::Event::NOT_PROCESSED;

			if (Event.type == SDL_QUIT)
			{
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "Received sdl event SDL_QUIT");
				break;
			} 
			else if(Event.type == SDL_VIDEOEXPOSE && Event.type == SDL_VIDEORESIZE)
			{
				Engine->RefreshScreen();
			}
		}
		else
		{
			OnIdle();
		}
	}  // while
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::OnQuit()
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"Got an on quit.  Pushing an event into SDL");
	SDL_Event *pEvent = new SDL_Event;
	pEvent->type = SDL_QUIT;
	SDL_PushEvent(pEvent);
	delete pEvent;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::Destroy()
{
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::RenderObjectAsync(DesignObj_Orbiter *pObj)
{
	OrbiterRenderer::RenderObjectAsync(pObj);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::ShowProgress(int nPercent) 
{
	TextureManager::Instance()->SafeToReleaseTextures(true);

	Popups->Reset();
	Engine->NewScreen("Progress");
	TextureManager::Instance()->EmptyCache();

	TextureManager::Instance()->SafeToReleaseTextures(false);

	m_pLastHighlightedObject = NULL;
	Engine->BeginModifyGeometry();

	OrbiterRenderer::ShowProgress(nPercent);

	Engine->EndModifyGeometry();

	UpdateScreen();
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ bool OrbiterRenderer_OpenGL::NeedToUpdateScreen()
{
	return NeedToUpdateScreen_;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::ScreenUpdated()
{
	NeedToUpdateScreen_ = false;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::RedrawObjects()
{
	OrbiterRenderer::RedrawObjects();
	NeedToUpdateScreen_ = true;
}

//----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::ObjectOnScreen(VectDesignObj_Orbiter *pVectDesignObj_Orbiter, 
DesignObj_Orbiter *pObj, PlutoPoint *ptPopup/* = NULL*/)
{
	if(pObj->m_PK_Effect_On_Screen > 0)
		m_spPendingGLEffects->m_nOnScreenTransitionEffectID = pObj->m_PK_Effect_On_Screen;

	OrbiterRenderer::ObjectOnScreen(pVectDesignObj_Orbiter, pObj, ptPopup);
}
//----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::ObjectOffScreen(DesignObj_Orbiter *pObj)
{
	if(pObj->m_PK_Effect_Off_Screen > 0)
		m_spPendingGLEffects->m_nOffScreenTransitionEffectID = pObj->m_PK_Effect_Off_Screen;

	if(pObj->m_pPopupPoint.X != 0 || pObj->m_pPopupPoint.Y != 0)
	{
		//we are on a popup; keep me cached!
		pObj->m_bKeepGraphicInCache = true;
	}

	if(pObj->m_ObjectType == DESIGNOBJTYPE_Datagrid_CONST)
		pObj->Flush();

	OrbiterRenderer::ObjectOffScreen(pObj);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::RenderScreen(bool bRenderGraphicsOnly)
{
	if(OrbiterLogic()->m_bQuit_get())
		return; //we are about to quit

	if(OrbiterLogic()->m_pObj_SelectedLastScreen)
		m_spPendingGLEffects->m_nOnSelectWithChangeEffectID = 
		OrbiterLogic()->m_pObj_SelectedLastScreen->m_FK_Effect_Selected_WithChange;

	int OffScreenTransition = m_spPendingGLEffects->m_nOffScreenTransitionEffectID;
	int OnScreenTransition = m_spPendingGLEffects->m_nOnScreenTransitionEffectID;
	//int OnSelectWithChange = m_spPendingGLEffects->m_nOnSelectWithChangeEffectID;

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"OrbiterRenderer_OpenGL::RenderScreen %d",OrbiterLogic()->m_pScreenHistory_Current ? OrbiterLogic()->m_pScreenHistory_Current->PK_Screen() : 0);
#endif

	PlutoRectangle rectLastSelected(0, 0, 0, 0);

	if(OrbiterLogic()->m_pObj_SelectedLastScreen)
	{
		rectLastSelected = PlutoRectangle(OrbiterLogic()->m_pObj_SelectedLastScreen->m_rPosition);
	}
	else
	{
		rectLastSelected.X = 0;
		rectLastSelected.Y = 0;
		rectLastSelected.Width = 80;
		rectLastSelected.Height = 80;
	}

	string sScreenName = "desktop";
	if(NULL != OrbiterLogic()->m_pScreenHistory_Current)
		sScreenName = "desktop - " + OrbiterLogic()->m_pScreenHistory_Current->GetObj()->GenerateObjectHash(PlutoPoint(0, 0), false);


	TextureManager::Instance()->SafeToReleaseTextures(true);

	Popups->Reset();
	Engine->NewScreen(sScreenName);
	TextureManager::Instance()->EmptyCache();

	TextureManager::Instance()->SafeToReleaseTextures(false);

	m_pLastHighlightedObject = NULL;

#ifdef VIA_OVERLAY
	ViaOverlay::Instance().ResetAlphaMask();
#endif

	Engine->BeginModifyGeometry();

	//make sure the datagrids are reseted (previous "image" with mesh frames for the datagrid - used for animations)
	if(NULL != OrbiterLogic()->m_pScreenHistory_Current)
		ResetDatagrids(OrbiterLogic()->m_pScreenHistory_Current->GetObj());

	//render the screen
	OrbiterRenderer::RenderScreen(bRenderGraphicsOnly);

	Engine->EndModifyGeometry();
	
	NeedToUpdateScreen_ = true;

	GLEffect2D::Effect* Item;
	GLEffect2D::EffectFactory::GetEffectCode(rand()%9);
	
	Item = Engine->Compose->CreateEffect(2, OffScreenTransition, 0, Simulator::GetInstance()->m_iMilisecondsTransition);
	if(Item)
	{
		Item->Configure(&rectLastSelected);
		Item->Reversed = true;
	}
	Item = Engine->Compose->CreateEffect(3, OnScreenTransition, 0, Simulator::GetInstance()->m_iMilisecondsTransition);
	if(Item)
		Item->Configure(&rectLastSelected);
}

/*virtual*/ bool OrbiterRenderer_OpenGL::HandleHidePopup(PlutoPopup* Popup)
{
	PLUTO_SAFETY_LOCK(cm, OrbiterLogic()->m_ScreenMutex );

	if(Popups)
	{
#ifdef VIA_OVERLAY
		ViaOverlay::Instance().HidePopup(Popup->m_Position.X, Popup->m_Position.Y, 
			Popup->m_pObj->m_rPosition.Width, Popup->m_pObj->m_rPosition.Height);
#endif

		//the popup alerts are dynamic; we don't want to reuse them
		bool bMarkAsVolation = Popup->m_sName == "popup_alert";

		Popups->HidePopup(Popup->m_pObj->GenerateObjectHash(Popup->m_Position, false),
			Popup->m_pObj->GenerateObjectHash(Popup->m_Position), bMarkAsVolation);
	}
	Engine->UnHighlight();
	return true;
}

/*virtual*/ bool OrbiterRenderer_OpenGL::HandleShowPopup(PlutoPopup* pPopup, PlutoPoint Position, int EffectID)
{
	PLUTO_SAFETY_LOCK(cm, OrbiterLogic()->m_ScreenMutex);

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"OrbiterRenderer_OpenGL::HandleShowPopup: %s", pPopup->m_pObj->m_ObjectID.c_str());
#endif

#ifdef VIA_OVERLAY
	ViaOverlay::Instance().ShowPopup(pPopup->m_Position.X, pPopup->m_Position.Y, 
		pPopup->m_pObj->m_rPosition.Width, pPopup->m_pObj->m_rPosition.Height);
#endif

	if(Popups)
		Popups->PaintPopup(pPopup->m_pObj->GenerateObjectHash(pPopup->m_Position, false), 
			pPopup->m_pObj->GenerateObjectHash(pPopup->m_Position), pPopup, EffectID);

	return true;
}

/*virtual*/ void OrbiterRenderer_OpenGL::UpdateObjectImage(string sPK_DesignObj, string sType, 
	char *pData, int iData_Size, string sDisable_Aspect_Lock, bool bRefreshObject/* = true*/)
{
	PLUTO_SAFETY_LOCK(vm, m_pOrbiter->m_VariableMutex);
	if(!m_listBackgroundImage.empty())
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "Got update object image, but we are still loading the background images");
		return;	
	}
	vm.Release();

	DesignObj_Orbiter *pObj = OrbiterLogic()->FindObject( sPK_DesignObj );
	if(!pObj)
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "Got update object image but cannot find: %s", sPK_DesignObj.c_str());
		return;
	}

	int PriorWidth=0, PriorHeight=0;
	if(  pObj->m_pvectCurrentGraphic  )
		pObj->m_pvectCurrentGraphic = NULL;
	if ( pObj->m_vectGraphic.size() )
	{
		PriorWidth = pObj->m_vectGraphic[pObj->m_iCurrentFrame]->m_nWidth;
		PriorHeight = pObj->m_vectGraphic[pObj->m_iCurrentFrame]->m_nHeight;
	}

	PlutoGraphic *pPlutoGraphic = NULL;
	if(pObj->m_vectGraphic.size() > 0)
	{
		pPlutoGraphic = pObj->m_vectGraphic[0];
		if(NULL == pPlutoGraphic)
		{
			pPlutoGraphic = CreateGraphic();
			pObj->m_vectGraphic[0] = pPlutoGraphic;
		}
	}
	else
	{
		pPlutoGraphic = CreateGraphic();
		pObj->m_vectGraphic.push_back(pPlutoGraphic);
	}

	pObj->m_iCurrentFrame = 0;

	sType = StringUtils::ToLower(sType);

	if(sType == "bmp")
		pPlutoGraphic->m_GraphicFormat = GR_BMP;
	else if(sType == "jpg")
		pPlutoGraphic->m_GraphicFormat = GR_JPG;
	else if(sType == "png")
		pPlutoGraphic->m_GraphicFormat = GR_PNG;
	else if(sType == "gif")
		pPlutoGraphic->m_GraphicFormat = GR_GIF;
	else
		pPlutoGraphic->m_GraphicFormat = GR_UNKNOWN;

	OpenGLGraphic *pOpenGLGraphic = dynamic_cast<OpenGLGraphic *>(pPlutoGraphic);
	if(NULL != pOpenGLGraphic)
	{
		pOpenGLGraphic->m_Filename = DYNAMIC_OBJ_NAME " - " + pObj->GenerateObjectHash(pObj->m_pPopupPoint);

		TextureManager::Instance()->InvalidateItem(pObj->GenerateObjectHash(pObj->m_pPopupPoint));

		if(NULL != pData && iData_Size != 0)
		{
			pOpenGLGraphic->LoadGraphic(pData, iData_Size, OrbiterLogic()->m_iRotation);  // These weren't pre-rotated
			pObj->m_pvectCurrentGraphic = &(pObj->m_vectGraphic);

			TextureManager::Instance()->PrepareConvert(pOpenGLGraphic);

			if(sDisable_Aspect_Lock.length())
				pObj->m_bDisableAspectLock = ( sDisable_Aspect_Lock=="1" ) ? true : false;
		}
		else
		{
			pOpenGLGraphic->Cleanup();
		}

#ifdef VIA_OVERLAY
			pOpenGLGraphic->ResetAlphaMask(pObj->m_rPosition.Width, pObj->m_rPosition.Height);
#endif

		if(bRefreshObject)
		{
			RenderObjectAsync(pObj);
			RedrawObjects();
		}
	}
}

void OrbiterRenderer_OpenGL::RemoveGraphic(string ObjectID)
{
	Engine->RemoveMeshFrameFromDesktopForID(ObjectID);
}

void OrbiterRenderer_OpenGL::HandleRefreshCommand(string sDataGrid_ID)
{
	PLUTO_SAFETY_LOCK( cm, OrbiterLogic()->m_ScreenMutex );
	
	vector<DesignObj_DataGrid*>::iterator it;
	for(it = OrbiterLogic()->m_vectObjs_GridsOnScreen.begin(); it != OrbiterLogic()->m_vectObjs_GridsOnScreen.end(); ++it)
	{
		DesignObj_DataGrid* pDesignObj = *it;

		if(sDataGrid_ID=="*" || pDesignObj->m_sGridID == sDataGrid_ID)
		{
			PLUTO_SAFETY_LOCK( cm, OrbiterLogic()->m_DatagridMutex );

			string DatagridFrameID = "datagrid " + pDesignObj->GenerateObjectHash(pDesignObj->m_pPopupPoint, false);
			Engine->InvalidateFrame(DatagridFrameID);

			OrbiterLogic()->InitializeGrid(pDesignObj);
			pDesignObj->Flush();
			RenderObjectAsync(pDesignObj);
		}
	}

	if( OrbiterLogic()->m_pScreenHistory_Current )
		RenderObjectAsync(OrbiterLogic()->m_pScreenHistory_Current->GetObj());

	NeedToRender render( OrbiterLogic(), "CMD_Refresh" );  // Redraw anything that was changed by this command
}

void OrbiterRenderer_OpenGL::ProcessingInputEvent()
{
	Engine->RefreshScreen();
}

void OrbiterRenderer_OpenGL::DestroyGraphic(string sObjectID)
{
	//remove it from the list with top most objects too, if needed
	Engine->RemoveTopMostObject(sObjectID);

	//this will detach the frame from the tree and also delete it
	Engine->RemoveMeshFrameFromDesktopForID(sObjectID);
}

void OrbiterRenderer_OpenGL::ObjectRendered(DesignObj_Orbiter *pObj, PlutoPoint point)
{
#ifdef VIA_OVERLAY

	if(point.X != 0 || point.Y != 0)
	{
		//don't handle the objects from popups
		return;
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "VIA ObjectRendered: %s", pObj->m_ObjectID.c_str());

	PlutoGraphic *pPlutoGraphic = NULL;
	pPlutoGraphic = 
		pObj->m_pvectCurrentGraphic != NULL && pObj->m_pvectCurrentGraphic->size() > 0 ?
		(pObj->m_pvectCurrentGraphic->operator [](0)) : 
		pObj->m_vectGraphic.size() > 0 ? 
			pObj->m_vectGraphic[0] :
			NULL;

	if(NULL != pPlutoGraphic)
	{
		OpenGLGraphic* pOpenGLGraphic = dynamic_cast<OpenGLGraphic*> (pPlutoGraphic);
		if(NULL != pOpenGLGraphic)
		{
			ViaOverlay::Instance().ApplyAlphaMask(pObj->m_rPosition.X, pObj->m_rPosition.Y, 
				pObj->m_rPosition.Width, pObj->m_rPosition.Height, pOpenGLGraphic->GetAlphaMask());		
		}
	}

#endif
} 

/*virtual*/ void OrbiterRenderer_OpenGL::GraphicOffScreen(vector<class PlutoGraphic*> *pvectGraphic)
{
	if(OrbiterLogic()->m_bMemoryManagementEnabled)
		OrbiterRenderer::GraphicOffScreen(pvectGraphic);
}

void OrbiterRenderer_OpenGL::DrawArrow(PlutoPoint p1, PlutoPoint p2, PlutoSize sizeArrow, 
	PlutoColor color, string ParentObjectID, string ObjectID)
{
	PlutoPoint arrow_p1;
	PlutoPoint arrow_p2;
	CalcArrowValues(p1, p2, sizeArrow, arrow_p1, arrow_p2);
	MeshContainer* Container = MeshBuilder::BuildArrow(p1, p2, sizeArrow, arrow_p1, arrow_p2);

	Point3D Color(color.R() / 255.0f, color.G() / 255.0f, color.B() / 255.0f);
	Container->SetColor(Color);
	Container->SetAlpha(color.A() / 255.0f);

	string ArrowUniqueID = 
		ObjectID.empty() ? 
		"arrow " + StringUtils::ltos(p1.X) + "," + StringUtils::ltos(p1.Y) + "," + StringUtils::ltos(p2.X) + "," + StringUtils::ltos(p2.Y) : 
		ObjectID;

	MeshFrame* Frame = new MeshFrame(ArrowUniqueID, Container);
	Frame->MarkAsVolatile();

	Engine->AddMeshFrameToDesktop(ParentObjectID, Frame);	
}

void OrbiterRenderer_OpenGL::ResetDatagrids(DesignObj_Orbiter *pObj)
{
	if(NULL != pObj)
	{
		if(pObj->m_ObjectType == DESIGNOBJTYPE_Datagrid_CONST)
		{
			DataGridRenderer_OpenGL *pDatagridRenderer = dynamic_cast<DataGridRenderer_OpenGL *>(pObj->Renderer());
			if(NULL != pDatagridRenderer)
				pDatagridRenderer->Reset();
		}
		else
		{
			for(DesignObj_DataList::iterator it = pObj->m_ChildObjects.begin(); it != pObj->m_ChildObjects.end(); ++it)
				ResetDatagrids(dynamic_cast<DesignObj_Orbiter *>(*it));
		}
	}
}

