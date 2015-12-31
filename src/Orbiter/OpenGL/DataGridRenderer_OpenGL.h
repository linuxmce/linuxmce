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

#ifndef DataGridRenderer_OpenGL_H_
#define DataGridRenderer_OpenGL_H_


#include "../DataGridRenderer.h"
#include "PlutoUtils/MultiThreadIncludes.h"

class OpenGL3DEngine;
class DesignObj_Orbiter;
class MeshFrame;

class DataGridRenderer_OpenGL : public DataGridRenderer
{
	OpenGL3DEngine* Engine;
	MeshFrame* m_pRenderFrame;
	int m_AnimationSpeed;

	void BuildDependencies(vector<string> & Dependencies);

public:
	DataGridRenderer_OpenGL(DesignObj_Orbiter *pOwner);
	virtual ~DataGridRenderer_OpenGL(void);

	virtual void RenderObject(DesignObj_Orbiter *pObj_Screen, PlutoPoint point = PlutoPoint(0, 0));
	void m_AnimationSpeed_set(int AnimationSpeed);
	void Reset();
};

#endif
