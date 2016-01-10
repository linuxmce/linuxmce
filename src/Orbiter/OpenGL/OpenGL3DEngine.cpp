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
#include "OpenGL3DEngine.h"

#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL_ttf.h>

#include "Mesh/MeshPainter.h"
#include "Mesh/MeshBuilder.h"
#include "Layers/GL2DEffectLayersCompose.h"
#include "Texture/TextureManager.h"
#include "DatagridAnimationManager.h"

#ifdef VIA_OVERLAY
#include "VIA/ViaOverlay.h"
#endif

#include "Simulator.h"

#include "DCE/Logger.h"
using namespace DCE;

//#define SCENE_DEBUG 1
//#define DUMP_SCENE_DEBUG 
//#define DISABLE_HIGHLIGHT 

OpenGL3DEngine::OpenGL3DEngine() : 
	AnimationRemain (false),
	SceneMutex("scene mutex"),
	CurrentLayer(NULL),
	OriginalCurrentLayer(NULL),
	SelectedFrame(NULL),
	m_pHighlightFrame(NULL),
	FrameBuilder(NULL),			 
	FrameDatagrid(NULL),
	ModifyGeometry(0),
	MilisecondsHighLight(0),
	m_bNeedToRefresh(true),
	m_bWorldChanged(true),
	m_bQuit(false),
	Compose(NULL),
	m_bDatagridRendering(false),
	m_bUseMask(false)
{
	if(TTF_Init()==-1) {
		printf("Error on TTF_Init: %s\n", TTF_GetError());
		return;
	}

	MilisecondsHighLight = Simulator::GetInstance()->m_iMilisecondsHighLight;

	m_spDatagridAnimationManager.reset(new DatagridAnimationManager(this));

	pthread_mutexattr_t m_SceneAttr;
	pthread_mutexattr_init(&m_SceneAttr);
	pthread_mutexattr_settype(&m_SceneAttr, PTHREAD_MUTEX_RECURSIVE_NP);
	SceneMutex.Init(&m_SceneAttr);
	pthread_mutexattr_destroy(&m_SceneAttr);

	ForceReleaseTextures = false;

	TextureManager::Instance()->Setup(this);
}

OpenGL3DEngine::~OpenGL3DEngine()
{
	Finalize();

	pthread_mutex_destroy(&SceneMutex.mutex);
}

void OpenGL3DEngine::Finalize(void)
{
	TextureManager::Instance()->SafeToReleaseTextures(true);

	UnHighlight();
	UnSelect();

	bool ReleaseVolatileOnly = !TextureManager::Instance()->MemoryManagementEnabled();

	if(CurrentLayer)
		CurrentLayer->CleanUp(ReleaseVolatileOnly);

	delete CurrentLayer;
	CurrentLayer = NULL;

	MeshPainter::Instance()->CleanUp();
	GLEffect2D::LayersCompose::Instance()->CleanUp();
	TextureManager::Instance()->CleanUp();
}

bool OpenGL3DEngine::Paint()
{
	if(m_bQuit)
		return false;

	TextureManager::Instance()->ConvertImagesToTextures();
	TextureManager::Instance()->ReleaseTextures();	

	if(ForceReleaseTextures)
	{
		ForceReleaseTextures = false;
		return false;
	}

	// some geometry changes are expected, means we will not paint the "broken painting tree"
	if(ModifyGeometry)
		return false;

	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, SceneMutex);
	if(NULL == CurrentLayer)
		return false;

	if(Compose->HasEffects())
		AnimationRemain = true;
	
	if(CurrentLayer->Children.size() == 0)
	{
#ifdef DEBUG
	    LoggerWrapper::GetInstance()->Write(LV_STATUS, "NOTHING to paint (current layer: %p)", CurrentLayer);		
#endif
		return false;
	}

	GL.SetClearColor(.0f, .0f, 0.0f);
	GL.ClearScreen(true, false);
		
	GL.EnableZBuffer(false);

	bool bDatagridAnimation = m_spDatagridAnimationManager->Update();

	if(bDatagridAnimation)
		m_bWorldChanged = true;
	else
	{
		if(m_pHighlightFrame)
		{
			Point3D Color;

			Color.X = 1.0f;
			Color.Y = (GetTick() / 2 % 512) / 255.0f * MilisecondsHighLight / 300;
			Color.Y = fabs(Color.Y - 1.0f);
			Color.Z = Color.Y;
			m_pHighlightFrame->SetColor(Color);
		}
	}

	UpdateTopMostObjects();

	Compose->Paint();
	GL.Flip();
	
	if(m_bWorldChanged && !m_bDatagridRendering)
	{
		m_bWorldChanged = false;

#ifdef VIA_OVERLAY
		ViaOverlay::Instance().WorldChanged();
#endif
	}

	if (Compose->HasEffects() == false)
		AnimationRemain = false;

	m_bNeedToRefresh = false;
	return true;
}

/*virtual*/ int OpenGL3DEngine::GetTick()
{
	return SDL_GetTicks();
}

void OpenGL3DEngine::NewScreen(string ScreenName)
{
	RefreshScreen();

	UnHighlight();
	UnSelect();
	m_spDatagridAnimationManager->StopAnimations();
	m_spDatagridAnimationManager->StopPendingAnimations();

	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, SceneMutex);
	m_bWorldChanged = true;

	if(NULL != CurrentLayer)
	{
		bool bReleaseVolatileOnly = !TextureManager::Instance()->MemoryManagementEnabled();
		CurrentLayer->CleanUp(bReleaseVolatileOnly);
		delete CurrentLayer;
	}
	StartTick = GetTick();

	CurrentLayer = new MeshFrame(ScreenName);
	OriginalCurrentLayer = CurrentLayer;

#ifdef SCENE_DEBUG
	MeshTransform transform;
	transform.ApplyRotateY(5.0f);
	CurrentLayer->ApplyTransform(transform);
#endif
	
	if(NULL != Compose)
		Compose->UpdateLayers(CurrentLayer, /*OldLayer*/CurrentLayer);
}

void OpenGL3DEngine::Setup()
{
	TextureManager::Instance()->SupportTextureNonPowerOfTwo();
	
	GL.Setup();
	Compose = GLEffect2D::LayersCompose::Instance();
	Compose->Setup(GL.Width, GL.Height);
	CurrentLayer = new MeshFrame("desktop");
	OriginalCurrentLayer = CurrentLayer;
}

MeshFrame* OpenGL3DEngine::AddMeshFrameToDesktop(string ParentObjectID, MeshFrame* Frame)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, SceneMutex);
	m_bWorldChanged = true;

	DumpScene();

#ifdef SCENE_DEBUG
	static float ZThing = 0.0f;
	ZThing += 0.2f;
	MeshTransform transform;
	transform.Translate(0.0f, 0.0f, ZThing);
	Frame->ApplyTransform(transform);
#endif

	if(NULL == CurrentLayer)
	{
		Frame->CleanUp();
		delete Frame;
		return NULL;
	}

	MeshFrame *Parent = NULL;
	
	if(ParentObjectID != "")
		Parent = CurrentLayer->FindChild(ParentObjectID);

	if(NULL != Parent)
	{
		MeshFrame *OldChild = Parent->FindChild(Frame->Name());
		if(NULL != OldChild)
		{
			if(OldChild->GetParent() != Parent)
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Got child, but I'm not its parent!");
				//throw "Got child, but I'm not its parent!";
			}
			else
			{
				return Parent->ReplaceChild(OldChild, Frame);
			}
		}
	}
	else
	{
		Parent = CurrentLayer;
		MeshFrame *DuplicatedFrame = CurrentLayer->FindChild(Frame->Name());

		if(DuplicatedFrame == Frame)
			return Frame;

		if(NULL != DuplicatedFrame)
			return CurrentLayer->ReplaceChild(DuplicatedFrame, Frame);
	}

	Parent->AddChild(Frame);

	DumpScene();
	return Frame;
}

inline void OpenGL3DEngine::DumpScene()
{
#ifdef DUMP_SCENE_DEBUG
	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, SceneMutex);
	
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "DUMPING SCENE: current layer %p, size %d",
		CurrentLayer, CurrentLayer->Children.size());

	CurrentLayer->Print("##");
#endif
}

/*virtual*/ void OpenGL3DEngine::Select(PlutoRectangle* SelectedArea)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, SceneMutex);

	if(NULL == CurrentLayer || NULL == SelectedArea)
		return;

	MeshBuilder MB;
	MB.Begin(MBMODE_TRIANGLE_STRIP);
	//MB.SetTexture(HighSurface);
	MB.SetAlpha(0.5f);
	MB.SetColor(1.0f, 0.0f, 0.0f);
	MB.SetTexture2D(
		float(SelectedArea->Left()/GL.Width),
		float(SelectedArea->Top()/GL.Height)
		);
	MB.AddVertexFloat(
		float(SelectedArea->Left()), 
		float(SelectedArea->Top()), 
		0);

	MB.SetTexture2D(
		float(SelectedArea->Right()/GL.Width),
		float(SelectedArea->Top()/GL.Height)
		);
	MB.AddVertexFloat(
		float(SelectedArea->Right()),
		float(SelectedArea->Top()),
		0);

	MB.SetTexture2D(
		float(SelectedArea->Left()/GL.Width),
		float(SelectedArea->Bottom()/GL.Height)
		);
	MB.AddVertexFloat(
		float(SelectedArea->Left()),
		float(SelectedArea->Bottom()),
		0);

	MB.SetTexture2D(
		float(SelectedArea->Right()/GL.Width),
		float(SelectedArea->Bottom()/GL.Height)
		);

	MB.AddVertexFloat(
		float(SelectedArea->Right()), 
		float(SelectedArea->Bottom()),
		0);

	UnSelect();
	SelectedFrame = new MeshFrame("select");
	SelectedFrame->SetMeshContainer(MB.End());
	CurrentLayer->AddChild(SelectedFrame);
}

/*virtual*/ void OpenGL3DEngine::Highlight(PlutoRectangle rectHighlightArea, OpenGLGraphic* HighSurface)
{
#ifdef DISABLE_HIGHLIGHT
	return;
#endif

	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, SceneMutex);
	
	if(NULL == CurrentLayer)
		return;

	MeshFrame *pLeftBar = new MeshFrame("highlight-frame-left");
	MeshFrame *pTopBar = new MeshFrame("highlight-frame-top");
	MeshFrame *pRightBar = new MeshFrame("highlight-frame-right");
	MeshFrame *pBottomBar = new MeshFrame("highlight-frame-bottom");
	
	PlutoRectangle rect(rectHighlightArea);
	
	const int nSize = GL.Width / 200;

	PlutoRectangle rectOriginal;

	if(m_bUseMask)
	{
		rectOriginal = rect;
	}
	else
	{
		PlutoRectangle rectSpecific(rect.X - nSize + 1, rect.Y - nSize + 1, 
			rect.Width + 2 * nSize - 2, rect.Height + 2 * nSize - 2);
		rectOriginal = rectSpecific;
	}

	rect = rectOriginal;
	rect.Width = nSize;
	pLeftBar->SetMeshContainer(MeshBuilder::BuildRectangle(rect, NULL));

	rect = rectOriginal;
	rect.Height = nSize;
	pTopBar->SetMeshContainer(MeshBuilder::BuildRectangle(rect, NULL));

	rect = rectOriginal;
	rect.X += rect.Width - nSize;
	rect.Width = nSize;
	pRightBar->SetMeshContainer(MeshBuilder::BuildRectangle(rect, NULL));

	rect = rectOriginal;
	rect.Y += rect.Height - nSize;
	rect.Height = nSize;
	pBottomBar->SetMeshContainer(MeshBuilder::BuildRectangle(rect, NULL));

	UnHighlight();

	m_pHighlightFrame = new MeshFrame("highlight-frame");
	m_pHighlightFrame->AddChild(pLeftBar);
	m_pHighlightFrame->AddChild(pTopBar);
	m_pHighlightFrame->AddChild(pRightBar);
	m_pHighlightFrame->AddChild(pBottomBar);
	m_pHighlightFrame->MarkAsVolatileRecursively();

	Point3D Color;
	Color.X = 1.0f;

	m_pHighlightFrame->SetColor(Color);

	if(NULL != OriginalCurrentLayer)
		OriginalCurrentLayer->AddChild(m_pHighlightFrame);

	AddTopMostObject("highlight");
}

/*virtual*/ void OpenGL3DEngine::UnHighlight()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, SceneMutex);

	if(NULL != m_pHighlightFrame)
	{
		CurrentLayer->RemoveChild(m_pHighlightFrame, true);
		m_pHighlightFrame->CleanUp();
		delete m_pHighlightFrame;
		m_pHighlightFrame = NULL;
	}
}

/*virtual*/ void OpenGL3DEngine::UnSelect()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, SceneMutex);

	if(NULL != SelectedFrame)
	{
		CurrentLayer->RemoveChild(SelectedFrame, true);
		SelectedFrame->CleanUp();
		delete SelectedFrame;
		SelectedFrame = NULL;
	}
}

bool OpenGL3DEngine::NeedUpdateScreen()
{
	return Compose->HasEffects() || m_pHighlightFrame != NULL || m_bNeedToRefresh;
}

MeshFrame* OpenGL3DEngine::GetMeshFrameFromDesktop(string ObjectID)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, SceneMutex);

	if(NULL != OriginalCurrentLayer)
		return OriginalCurrentLayer->FindChild(ObjectID);

	return NULL;
}

void OpenGL3DEngine::RemoveMeshFrameFromDesktop(MeshFrame* Frame)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, SceneMutex);
	if(NULL == OriginalCurrentLayer)
		return;

	m_bWorldChanged = true;
	OriginalCurrentLayer->RemoveChild(Frame, false);
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "RemoveMeshFrameFromDesktop: removed object %p, size is not %d", 
		Frame, OriginalCurrentLayer->Children.size());
#endif
}

void OpenGL3DEngine::StartFrameDrawing(string ObjectHash)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, SceneMutex);
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "OpenGL3DEngine::StartFrameDrawing!");
#endif

	m_bWorldChanged = true;
	if(NULL != FrameBuilder)
		EndFrameDrawing("");

	FrameBuilder = CurrentLayer;
	CurrentLayer = new MeshFrame(ObjectHash);
}

void OpenGL3DEngine::StartDatagridDrawing(string ObjectHash)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, SceneMutex);

	m_bDatagridRendering = true;

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "OpenGL3DEngine::StartFrameDrawing!");
#endif

	m_bWorldChanged = true;
	if(NULL != FrameDatagrid)
		EndDatagridDrawing("");

	FrameDatagrid = CurrentLayer;
	CurrentLayer = new MeshFrame(ObjectHash);
}

MeshFrame* OpenGL3DEngine::EndDatagridDrawing(string ObjectHash)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, SceneMutex);
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "OpenGL3DEngine::EndFrameDrawing!");
#endif

	m_bWorldChanged = true;
	MeshFrame* Result = CurrentLayer;
	CurrentLayer = FrameDatagrid;
	FrameDatagrid = NULL;

	Result->MarkAsVolatileRecursively();

	m_bDatagridRendering = false;
	return Result;
}

void OpenGL3DEngine::InvalidateFrame(string ObjectHash)
{
	MeshFrame *pMeshFrame = CurrentLayer->FindChild(ObjectHash);

	m_bWorldChanged = true;
	if(NULL != pMeshFrame)
	{
		MeshFrame *pNewDummyFrame = new MeshFrame(ObjectHash);
		pNewDummyFrame->MarkAsVolatile();
		CurrentLayer->ReplaceChild(pMeshFrame, pNewDummyFrame);
	}
}

/**
*	Return as result the popup
*/
MeshFrame* OpenGL3DEngine::EndFrameDrawing(string sObjectHash)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, SceneMutex);

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "OpenGL3DEngine::EndFrameDrawing!");
#endif
	m_bWorldChanged = true;
	MeshFrame* Result = CurrentLayer;

	CurrentLayer = FrameBuilder;
	if(!CurrentLayer)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Current layer = NULL, that should not happend!");
	}
	else
	{
		AddMeshFrameToDesktop("", Result);
	}

	FrameBuilder = NULL;
	return Result;
}

void OpenGL3DEngine::RemoveMeshFrameFromDesktopForID(string ObjectID)
{
	DumpScene();

	if(ObjectID == "")
		return;

	m_bWorldChanged = true;
	MeshFrame *Frame = CurrentLayer->FindChild(ObjectID);
	if(NULL != Frame)
		RemoveMeshFrameFromDesktop(Frame);

	DumpScene();
}

void OpenGL3DEngine::BeginModifyGeometry()
{
	++ModifyGeometry;
}

void OpenGL3DEngine::EndModifyGeometry()
{
	--ModifyGeometry;
}

void OpenGL3DEngine::AddTopMostObject(string ObjectID)
{
	if(ObjectID== "")
		return;

	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, SceneMutex);

	if(TopMostObjects.find(ObjectID) != TopMostObjects.end())
		return;

	TopMostObjects[ObjectID] = ObjectID;
}

void OpenGL3DEngine::RemoveTopMostObject(string ObjectID)
{	
	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, SceneMutex);

	map<string,string>::iterator Item = TopMostObjects.find(ObjectID);
	if(Item != TopMostObjects.end())
		TopMostObjects.erase(Item);
}

void OpenGL3DEngine::UpdateTopMostObjects()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, SceneMutex);

	if(TopMostObjects.size() == 0 || NULL == OriginalCurrentLayer)
		return;

	bool bOldWorldChanged = m_bWorldChanged;

	map<string,string>::iterator Item;
	for(Item = TopMostObjects.begin(); Item != TopMostObjects.end(); )
	{
		MeshFrame*Frame = this->GetMeshFrameFromDesktop(Item->second);
		if(Frame == NULL)
		{
			map<string,string>::iterator SearchItem = TopMostObjects.find(Item->second);
			if(SearchItem != TopMostObjects.end())
				TopMostObjects.erase(Item++);
		}
		else
		{
			OriginalCurrentLayer->RemoveChild(Frame, true);
			OriginalCurrentLayer->AddChild(Frame);
			++Item;
		}
	}

	m_bWorldChanged = bOldWorldChanged;
}

void OpenGL3DEngine::ReplaceMeshInAnimations(MeshFrame *OldFrame, MeshFrame *NewFrame)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, SceneMutex);

	if(NULL == OriginalCurrentLayer)
		return;

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "OpenGL3DEngine::ReplaceMeshInAnimations %p/%s with %p/%s",
		OldFrame, OldFrame->Name().c_str(), NewFrame, NewFrame->Name().c_str());

	m_bWorldChanged = true;
	MeshFrame *pOldCloneFrame = OriginalCurrentLayer->FindChild(NewFrame->Name());
	if(NULL != pOldCloneFrame)
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "OpenGL3DEngine::ReplaceMeshInAnimations also replacing %p/%s",
			pOldCloneFrame, pOldCloneFrame->Name().c_str());
		m_spDatagridAnimationManager->ReplaceMeshInAnimations(pOldCloneFrame, NewFrame);
	}

	AddMeshFrameToDesktop("", NewFrame);
	RemoveMeshFrameFromDesktop(OldFrame);

	m_spDatagridAnimationManager->ReplaceMeshInAnimations(OldFrame, NewFrame);
}

void OpenGL3DEngine::InvalidateFramesStartingWith(string sPrefix, MeshFrame *pParent/*=NULL*/)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, SceneMutex);

	if(NULL == pParent)
		pParent = OriginalCurrentLayer;

	if(NULL != pParent)
	{
		for(vector<MeshFrame*>::iterator it = pParent->Children.begin(); it != pParent->Children.end(); ++it)
		{
			MeshFrame *pMeshFrame = *it;

			if(NULL != pMeshFrame && pMeshFrame->Name().find(sPrefix) == 0)
			{
				MeshFrame *pNewDummyFrame = new MeshFrame(pMeshFrame->Name());
				pNewDummyFrame->MarkAsVolatile();
				pParent->ReplaceChild(pMeshFrame, pNewDummyFrame);
			}
			else
			{
                InvalidateFramesStartingWith(sPrefix, pMeshFrame);								
			}
		}
	}
}
