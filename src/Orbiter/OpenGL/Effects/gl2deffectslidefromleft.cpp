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
#include "gl2deffectslidefromleft.h"

#include "gl2deffectfactory.h"
#include "../Layers/GL2DEffectLayersCompose.h"

namespace GLEffect2D
{

GL2DEffectSlideFromLeft::GL2DEffectSlideFromLeft (EffectFactory * EffectsEngine, int StartAfter, int TimeForCompleteEffect)
	: GL2DEffectTransit(EffectsEngine, StartAfter, TimeForCompleteEffect)
{

 	FullScreen.Left = 0;
	FullScreen.Top = 0;
	FullScreen.Width = float(Effects->Widgets->GetWidth());
	FullScreen.Height = float(Effects->Widgets->GetHeight());


	//creating a basic window that merge the effect
	Destination = (TBasicWindow*)Effects->Widgets->CreateWidget(BASICWINDOW, 
		0, 0, 
		Effects->Widgets->GetWidth(), Effects->Widgets->GetHeight(), 
		"Destination");	
	Destination->SetVisible(true);
}


GL2DEffectSlideFromLeft::~GL2DEffectSlideFromLeft() {
	Effects->Widgets->DeleteWidget(Destination);
}

/**
 * It copy the SourceFrame as button texture, DestFrame as destination image
 * @param SourceFrame 
 * @param DestFrame 
 * @param ButtonSourceSize 
 */
void GL2DEffectSlideFromLeft::Configure(PlutoRectangle* EffectSourceSize)
{
	ButtonSize.Left = -FullScreen.Width;
	ButtonSize.Top = 0;
	ButtonSize.Width = FullScreen.Width;
	ButtonSize.Height = FullScreen.Height;
	
	Configured = false;
}

/**
 * Paint the effect based of the current time
 * @param Now Gives the global time
 */
void GL2DEffectSlideFromLeft::Paint(int Time)
{
	GL2DEffectTransit::Paint();

	if(!Configured) {
		//Set up the textures for triangles
		OpenGLGraphic* ScreenRenderGraphic =
			GLEffect2D::LayersCompose::Instance()->NewScreen->GetRenderGraphic();
		Destination->SetBackgroundImage(ScreenRenderGraphic);

		//float MaxCoordU = (FullScreen.Width)/MathUtils::MinPowerOf2((int)FullScreen.Width);
		//float MaxCoordV = (FullScreen.Height)/MathUtils::MinPowerOf2((int)FullScreen.Height);
		
		Destination->SetTextureWraping(0.0, 0.0, 1.0f, 1.0f);

		Start = Effects->MilisecondTimmer();

		Configured = true;
	}
	
	float Step = Stage(float(Time));
	
	FloatRect Animation = ButtonSize.Interpolate(FullScreen, Step);
	
	Destination->SetRectCoordinates(Animation);
}

}
