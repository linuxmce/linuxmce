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

#ifndef __DATAGRID_ANIMATION_MANAGER_H__
#define __DATAGRID_ANIMATION_MANAGER_H__

#include <string>
#include <vector>
using namespace std;

class MeshFrame;
class OpenGL3DEngine;
class AnimationScrollDatagrid;


/*
 *	class that manages all AnimationScrollDatagrid if they complete the dependencies
 *	One animation is not started if the TiedTo objects are not satisfied
 */
class DatagridAnimationManager
{
	//private data
	vector<AnimationScrollDatagrid *> m_vectCurrentAnimations;
	vector<AnimationScrollDatagrid *> m_vectPendingAnimations; 
	OpenGL3DEngine *m_pEngine;

	//internal events
	void OnStartAnimation();
	void OnPrepareForAnimation();
	void OnStopAnimation();

	//helper methods
	void Cleanup();
	bool AnimationsPrepared();
	

public:

	/**
	 *	Default constructor, it gets the Engine that needs to be managed
	 */
	DatagridAnimationManager(OpenGL3DEngine *Engine) : m_pEngine(Engine) {}
	/**
	 *	Default destructor, it gets the Engine that needs to be managed
	 */
	~DatagridAnimationManager() { /* todo : cleanup */}

	/**
	 *	Send in pending animation list one object. If it has complete the dependencies, the animation 
	 *	will start
	 */
	void PrepareForAnimation(string ObjectID, MeshFrame *BeforeGrid, MeshFrame *AfterGrid,
		int MilisecondTime, int Direction, vector<string> Dependencies);

	/**
	 *	Stop all animation for one reason (for instance it changes the screen)
	 */
	void StopAnimations();
	void StopPendingAnimations();

	/**
	 *	Replaces one frame if exist in animations with an updated one 
	 */
	void ReplaceMeshInAnimations(MeshFrame *OldFrame, MeshFrame *NewFrame);

	/**
	 *	Updates on screen the animations
	 *	@return it returns true if the animations are end
	 */
	bool Update();

	bool HasAnimations();
};

#endif //__DATAGRID_ANIMATION_MANAGER_H__
