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

#ifndef AnimationScrollDatagrid_H_
#define AnimationScrollDatagrid_H_

#include "../../SerializeClass/ShapesColors.h"

class MeshFrame;
class OpenGL3DEngine;

/*
 *	class that manages over one datagrid the "Cube animation" and keeps it's status of animation
 */
class AnimationScrollDatagrid
{
	int MilisecondTime;
	int Direction;
	int StartTime;
	OpenGL3DEngine* Engine;
	bool Finished;


	void CopyDependencies(vector <string> Dependencies);
public:
	vector <string> Dependencies;
	string ObjectID;
	MeshFrame *BeforeGrid, *AfterGrid;

	/**
	 *	Default constructor
	 */
	AnimationScrollDatagrid(string ObjectID, OpenGL3DEngine* Engine, 
		MeshFrame *BeforeGrid, MeshFrame *AfterGrid,
		int MilisecondTime, int Direction, vector <string> Dependencies);
	virtual ~AnimationScrollDatagrid(void);

	/**
	 *	Function that updates the start time of the animation, that is used for sync animation with another
	 *	elements, like another AnimationScrollDatagrid
	 */
	bool UpdateStartTime(int StartTime);

	/**
	 *	Function that gives the start time of the animation, should be used for sync another animations with 
	 *	current animation
	 */
	int GetStartTime();

	//bool DatagridDependenciesSatisfied(vector<AnimationScrollDatagrid*> &AnimationDatagrids);

	/**
	 *	Start datagrid animation, until is used StopAnimation
	 */
	void StartAnimation();

	/**
	 *	Start datagrid animation, until is used StopAnimation
	 */
	void StopAnimation();
	/**
	 *	returns true if the animation is ended
	 *	@param ModifyGeometry, if is false, the animation updates in statuses, but doesn't update
	 *	as geometry changes
	 */
	bool Update(bool ModifyGeometry);

};

#endif
