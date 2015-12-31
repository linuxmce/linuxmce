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
#ifndef OpenGL3DEngine_H_
#define OpenGL3DEngine_H_

#include <memory>
#include <map>

#include "ExtensionManager.h"
#include "Mesh/MeshFrame.h"

#include "PlutoUtils/MultiThreadIncludes.h"
#include "Layers/GL2DEffectLayersCompose.h"

class AnimationScrollDatagrid;
class DatagridAnimationManager;

/**
 *	That class is the core of all OpenGL operations
 */
class OpenGL3DEngine
{
	int StartTick;
	bool AnimationRemain;
	pluto_pthread_mutex_t SceneMutex;

	MeshFrame* CurrentLayer;
	MeshFrame* OriginalCurrentLayer; //alias for CurrentLayer
	MeshFrame* SelectedFrame;
	MeshFrame* m_pHighlightFrame;
	MeshFrame* FrameBuilder, *FrameDatagrid;
	

	/**
	 *	Remove selection object
	 */
	virtual void UnSelect();

	map<string, string> TopMostObjects;
	void UpdateTopMostObjects();

	void ShowAnimationTextures();
	/**
	 *	Outputs the display content tree to log
	 */
	void DumpScene();

	auto_ptr<DatagridAnimationManager> m_spDatagridAnimationManager;

	/**
	 *	Block painting because there is a need of bigger geometry painting
	 */
	int ModifyGeometry;

	//setup
	int MilisecondsHighLight;
	bool m_bNeedToRefresh;
	bool m_bWorldChanged;

	virtual void Finalize(void);
public:
	bool ForceReleaseTextures;
	bool m_bQuit;

	GLEffect2D::LayersCompose* Compose;
	bool m_bDatagridRendering;
	bool m_bUseMask;

	ExtensionManager GL;
	/**
	 *	Default constructor
	 */
	OpenGL3DEngine(void);
	/**
	 *	Default destructor
	 */
	virtual ~OpenGL3DEngine();


	bool NeedUpdateScreen();
	/**
	 *	returns the MeshFrame that keeps the name of the target frame in the scene 	
	 */
	MeshFrame* AddMeshFrameToDesktop(string ParentObjectID, MeshFrame* Frame);
	/**
	 *	removes 
	 */
	void RemoveMeshFrameFromDesktopForID(std::string ObjectID);
	void RemoveMeshFrameFromDesktop(MeshFrame* Frame);
	MeshFrame* GetMeshFrameFromDesktop(string ObjectID);
	
	virtual void NewScreen(string Name);
	virtual bool Paint();

	/**
	 *	Select an area on creen
	 */
	virtual void Select(PlutoRectangle* SelectedArea);
	/**
	 *	Highlight an area on creen
	 */
	virtual void Highlight(PlutoRectangle rectHighlightArea, OpenGLGraphic* HighSurface);
	/**
	 *	Unhighlight previously highlight area
	 */
	virtual void UnHighlight();
	virtual int GetTick();
	void Setup();

	/**
	 *	Store the MeshFrame which keeps the Datagrid	
	 */
	void StartDatagridDrawing(std::string ObjectHash);
	MeshFrame* EndDatagridDrawing(std::string ObjectHash);

	/*
	 *	This will replace the frame in the tree with mesh frame with a dummy frame
	 */
	void InvalidateFrame(std::string ObjectHash);

	/**
	 *	Store the current frame, for instance a popup
	 */
	void StartFrameDrawing(std::string ObjectHash);
	MeshFrame* EndFrameDrawing(std::string sObjectHash);

	/**
	 *	Bypassing displaying screen changes until it ends	
	 */
	void BeginModifyGeometry();
	/**
	 *	Bypassing displaying screen changes until it ends	
	 */
	void EndModifyGeometry();

	/**
	 *	Adds one ID as topmost
	 */
	void AddTopMostObject(string ObjectID);
	/**
	 *	Removes one ObjectID from topmost objects
	 */
	void RemoveTopMostObject(string ObjectID);

	/**
	 *	Replace one object with another
	 */
	void ReplaceMeshInAnimations(MeshFrame *OldFrame, MeshFrame *NewFrame);

	/**
	 *	Gets the animation manager, which makes cube animations to datagrids
	 */
	DatagridAnimationManager *GetDatagridAnimationManager() { return m_spDatagridAnimationManager.get(); }
	/**
	 *	Get the scene mutex to sync with screen operations
	 */
	pluto_pthread_mutex_t& GetSceneMutex() { return SceneMutex; }

	/**
	 *  Notify the engine that it needs to update the screen	
	 */
	void RefreshScreen() { m_bNeedToRefresh = true; }

	/**
	 *  Invalidate all children frames starting with prefix
	 */
	void InvalidateFramesStartingWith(string sPrefix, MeshFrame *pParent = NULL);
};

#endif
