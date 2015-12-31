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
#include "gl2deffectbeziertranzit_prism.h"
#include "gl2deffectfactory.h"

#include "../Layers/GL2DEffectLayersCompose.h"

namespace GLEffect2D
{

GL2DBezierEffectTransit_Prism::GL2DBezierEffectTransit_Prism (EffectFactory * EffectsEngine, int StartAfter, 
															  int TimeForCompleteEffect)
	: GL2DEffectTransit(EffectsEngine, StartAfter, TimeForCompleteEffect)
{
	FullScreen.Left = 0.0f;
	FullScreen.Top = 0.0f;
	FullScreen.Width = (float)Effects->Widgets->GetWidth();
	FullScreen.Height = (float)Effects->Widgets->GetHeight();

	//create the button which keep the source of the screen (the button part)
	Button = (TBezierWindow *)Effects->Widgets->CreateWidget(BEZIERWINDOW, 
		0, 0, 
		Effects->Widgets->GetWidth(), Effects->Widgets->GetHeight(), 
		"Button");
	Button->SetVisible(true);
}


GL2DBezierEffectTransit_Prism::~GL2DBezierEffectTransit_Prism() {
	Effects->Widgets->DeleteWidget(Button);	

	Button = NULL;
}

void GL2DBezierEffectTransit_Prism::Configure(PlutoRectangle* EffectSourceSize)
{
	ButtonSize.Left = (float)EffectSourceSize->X;
	ButtonSize.Top = (float)EffectSourceSize->Y;
	ButtonSize.Width = (float)EffectSourceSize->Width;
	ButtonSize.Height = (float)EffectSourceSize->Height;

	Configured = false;
}

void GL2DBezierEffectTransit_Prism::Paint(int Now)
{
	GL2DEffectTransit::Paint();
	if(!Configured) 
	{
		//Set up the textures for triangles
		OpenGLGraphic* ScreenRenderGraphic = Effects->ParentLayer->RenderGraphic;
		Button->SetBackgroundImage(ScreenRenderGraphic);
		
		float MaxCoordU = 1;
		float MaxCoordV = 1;
		
		Button->SetTextureWraping(0.0, 0.0, 
			MaxCoordU, MaxCoordV);
			
		Button->BezierDefinition.Divisions = 50;

		Start = Effects->MilisecondTimmer();	
		Configured = true;
	}
	
	float Step = Stage((float)Now), Step2;
	//Step = 1-Step;
	//Step2 = keeped for backup reason of Step variable
	Step2 = Step;
	
	Button->SetAlpha(Step);
	FloatRect Animation;
	
	
	if (Step <= 0.5f)
	{
		Animation = ButtonSize.Interpolate(FullScreen, Step*2);
	}
	else
		Animation = FullScreen;

	if (Step <= 0.5f)
		Step*= 2.0f;
	else
		Step = 1.0f;

	Button->SetRectCoordinates(Animation);
	Step = Step2;
	if( Step < 0.5)
		Step *= 2;
	else
		Step= (1.0f-Step)*2; 

	int i;
	//float Average = Animation.Width * 2/3;

	// bit-to-bit copy 
	BEZIER_PATCH BezierDefinition = Button->BezierDefinition;

	// 0 = Start, 1 = Control Start, 2 = Control end, 3 = End 
	float TopProfile [4], LeftProfile[4];

	LeftProfile[0] = 0.0f;
	LeftProfile[1] = MathUtils::InterpolateValues(
		Animation.Left,
		Animation.Left + Animation.Width,
		0.33f*Step);
	LeftProfile[2] = LeftProfile[1];
	LeftProfile[3] = 0.0f;

	TopProfile[0] = 0.0f;
	TopProfile[1] = MathUtils::InterpolateValues(
		Animation.Top,
		Animation.Top + Animation.Height,
		0.33f*Step);
	TopProfile[2] = TopProfile[1];
	TopProfile[3] = 0.0f;

	for(i = 0; i<4; i++)
	{
		Button->BezierDefinition.anchors[i][0].y += 
			TopProfile[i];
		Button->BezierDefinition.anchors[i][3].y -= 
			TopProfile[i];
		
		Button->BezierDefinition.anchors[0][i].x += 
			LeftProfile[i];
		Button->BezierDefinition.anchors[3][i].x -= 
			LeftProfile[i];
	}

	for (i = 1; i< 3; i++)
		for (int j = 1; j< 3; j++)
		{
			float Delta = 
				fabs(BezierDefinition.anchors[j][i].x -
				Button->BezierDefinition.anchors[j][i].x)+
				fabs(BezierDefinition.anchors[j][i].y -
				Button->BezierDefinition.anchors[j][i].y);
			Button->BezierDefinition.anchors[j][i].z =
				Button->BezierDefinition.anchors[j][i].z +
				Delta;
		}

}

}
