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
#include "GL2DEffectLayer.h"

#include "../Mesh/MeshPainter.h"

namespace GLEffect2D 
{

Layer::Layer(int Width, int Height)
: RenderFrame(NULL),
RenderGraphic(NULL)
{
	Widgets  = new DrawingWidgetsEngine(Width, Height);
	Effects  = new EffectFactory(this, Widgets);	
}

Layer::~Layer(void)
{
	delete Effects;
	delete Widgets;
}

/*virtual*/ void Layer::Paint()
{
	if (!Effects->HasEffects())
		return;
	Effects->UpdateEffects();
	MeshTransform Transform;
	Effects->Paint(Transform);
}


Effect* Layer::CreateEffect(int IDEffect, int StartAfter, int TimeForComplete)
{
	if(Effects == NULL)
		return NULL;
	return Effects->CreateEffect(IDEffect, StartAfter, TimeForComplete);
}

TBaseWidget* Layer::CreateWidget(int WidgetType, int Top, int Left, int Width, int Height, char* Text)
{
	if (!Widgets)
		return NULL;
	return Widgets->CreateWidget(WidgetType, Top, Left, Width, Height, Text);
}

bool Layer::HasEffects()
{
	return Effects->HasEffects();
}

void Layer::Resize(int Width, int Height)
{
	this->Width = Width;
	this->Height = Height;
}

}
