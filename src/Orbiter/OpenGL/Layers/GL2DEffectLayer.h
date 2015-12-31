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
#ifndef GL2DEffectLayer_H_
#define GL2DEffectLayer_H_

#include "../Effects/gl2deffectfactory.h"

#include "../Mesh/MeshFrame.h"
#include "../OpenGLGraphic.h"

namespace GLEffect2D 
{

class Layer
{
	int Width, Height;
	EffectFactory* Effects;
	DrawingWidgetsEngine* Widgets;

	MeshFrame* RenderFrame;

	void CreateRenderTexture();
public:
	OpenGLGraphic* RenderGraphic;

	Layer(int Width, int Height);
	virtual ~Layer(void);

	virtual void Paint();

	Effect* CreateEffect(int IDEffect, int StartAfter, int TimeForComplete);
	TBaseWidget* CreateWidget(int WidgetType, int Top, int Left, int Width, int Height, char* Text);

	bool HasEffects();

	
	void Resize(int Width, int Height);

	/**
	 *	Render-To-Texture a Render MeshFrame	
	 */
	void SetRenderFrame(MeshFrame* RenderFrame);
	MeshFrame* GetRenderFrame();

	OpenGLGraphic* GetRenderGraphic();

	void RenderFrameToGraphic(MeshTransform& Transform);
};

}

#endif
