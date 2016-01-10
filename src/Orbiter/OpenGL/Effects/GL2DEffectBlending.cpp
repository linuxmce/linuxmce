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
#include "GL2DEffectBlending.h"

#include "gl2deffectfactory.h"

#include "../Layers/GL2DEffectLayersCompose.h"

namespace GLEffect2D
{

GL2DEffectBlending::GL2DEffectBlending (EffectFactory * EffectsEngine, int StartAfter, int TimeForCompleteEffect)
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

	//create the button which keep the source of the screen (the button part)
	Button = (TBasicWindow*) Effects->Widgets->CreateWidget(BASICWINDOW, 
		0, 0, 
		Effects->Widgets->GetWidth(), Effects->Widgets->GetHeight(), 
		"Button");
	Button->SetVisible(true);
}


GL2DEffectBlending::~GL2DEffectBlending() {
	Effects->Widgets->DeleteWidget(Destination);
	Effects->Widgets->DeleteWidget(Button);	
}

/**
 * It copy the SourceFrame as button texture, DestFrame as destination image
 * @param SourceFrame 
 * @param DestFrame 
 * @param ButtonSourceSize 
 */
void GL2DEffectBlending::Configure(PlutoRectangle* EffectSourceSize)
{

	ButtonSize.Left = float(EffectSourceSize->X);
	ButtonSize.Top = float(EffectSourceSize->Y);
	ButtonSize.Width = float(EffectSourceSize->Width);
	ButtonSize.Height = float(EffectSourceSize->Height);

	Configured = false;
}

/**
 * Paint the effect based of the current time
 * @param Now Gives the global time
 */
void GL2DEffectBlending::Paint(int Now)
{
	Configured = true;

	GL2DEffectTransit::Paint();
	if(!Configured) {
		//Set up the textures for triangles
		OpenGLGraphic* ScreenRenderGraphic = Effects->ParentLayer->RenderGraphic;
		Destination->SetBackgroundImage(ScreenRenderGraphic);
		
		float MaxCoordU = 1.0f/MathUtils::MinPowerOf2((int)FullScreen.Width);
		float MaxCoordV = 1.0f/MathUtils::MinPowerOf2((int)FullScreen.Height);
		
		Button->SetTextureWraping(
			ButtonSize.Left*MaxCoordU,
			 ButtonSize.Top*MaxCoordV,
			(ButtonSize.Width)*MaxCoordU,
			(ButtonSize.Height)*MaxCoordV 
			);

		Button->SetTextureWraping(0.0f, FullScreen.Width*0.5f*MaxCoordV,
			FullScreen.Height*0.8f*MaxCoordU, FullScreen.Width*0.9f*MaxCoordV);
		Destination->SetTextureWraping(0.0f, 0.0f, 
			MaxCoordU*FullScreen.Width, MaxCoordV*FullScreen.Height);
		Start = Effects->MilisecondTimmer();

		
		Configured = true;
	}
	
	float Step = Stage(float(Now));
	
	FloatRect Animation = ButtonSize.Interpolate(FullScreen, Step);

	Button->SetRectCoordinates(Animation);
	Button->SetAlpha(1-Step);
	
	Destination->SetRectCoordinates(Animation);
	Destination->SetAlpha(Step);
}

}
