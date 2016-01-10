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
#include "gl2deffectbeziertranzit.h"
#include "gl2deffectfactory.h"

#include "../Layers/GL2DEffectLayersCompose.h"

namespace GLEffect2D 
{

GL2DBezierEffectTransit::GL2DBezierEffectTransit (EffectFactory * EffectsEngine, int StartAfter, 
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


GL2DBezierEffectTransit::~GL2DBezierEffectTransit() {
	Effects->Widgets->DeleteWidget(Button);	
	Button = NULL;
}

void GL2DBezierEffectTransit::Configure(PlutoRectangle* EffectSourceSize)
{
	
	ButtonSize.Left = (float)EffectSourceSize->X;
	ButtonSize.Top = (float)EffectSourceSize->Y;
	ButtonSize.Width = (float)EffectSourceSize->Width;
	ButtonSize.Height = (float)EffectSourceSize->Height;

	Configured = false;
	
}

void GL2DBezierEffectTransit::Paint(int Now)
{
	GL2DEffectTransit::Paint();
	if(!Configured) 
	{
		//Set up the textures for triangles
		/*
		OpenGLGraphic* ScreenRenderGraphic =
			GLEffect2D::LayersCompose::Instance()->NewScreen->GetRenderGraphic();
			*/
		OpenGLGraphic* ScreenRenderGraphic = Effects->ParentLayer->RenderGraphic;

		Button->SetBackgroundImage(ScreenRenderGraphic);
		float MaxCoordU = 1.0f;
		float MaxCoordV = 1.0f;
		
		Button->SetTextureWraping(0, 0, MaxCoordU, MaxCoordV);
		Button->BezierDefinition.Divisions = 15;

		Start = Effects->MilisecondTimmer();	

		Configured = true;
	}
	
	float Step = Stage((float)Now);
	Button->SetAlpha(Step);

	Step = 1-Step;


	FloatRect Animation;

	bool FistStage = true;
	if (Step <= 0.5)
	{
		Step*= 2;
		Animation = ButtonSize.Interpolate(FullScreen, 1-Step);
	}
	else
	{
		FistStage = false;
		Step = (Step - 0.5f)*2.0f;

		Animation = ButtonSize;
	}
	
	Button->SetRectCoordinates(Animation);

	int i, j;
	float Average = Animation.Width * 2/3;

	if(!FistStage)
		Step = 1.0f-Step;

	// bit-to-bit copy 
	BEZIER_PATCH BezierDefinition = Button->BezierDefinition;

	// 0 = Start, 1 = Control Start, 2 = Control end, 3 = End 
	POINT_3D LeftProfile [4], RightProfile[4];

	for(i = 0; i<4; i++)
	{
		LeftProfile[i] = Button->BezierDefinition.anchors[0][i];
		RightProfile[i] = Button->BezierDefinition.anchors[3][i];
	}

	LeftProfile[0].x += MathUtils::InterpolateValues(0, Average, Step);
	LeftProfile[1].x = LeftProfile[0].x;

	RightProfile[0].x -= MathUtils::InterpolateValues(0, Average/2, Step);
	RightProfile[0].y = LeftProfile[0].y;
	RightProfile[1].x = RightProfile[0].x;
	RightProfile[1].y = LeftProfile[1].y;

	POINT_3D Left1 = MathUtils::Bernstein(0.33f, LeftProfile);
	POINT_3D Left2 = MathUtils::Bernstein(0.66f, LeftProfile);

	LeftProfile[1].x = Left1.x;
	LeftProfile[2].x = Left2.x;


	POINT_3D Right1 = MathUtils::Bernstein(0.66f, RightProfile);
	POINT_3D Right2 = MathUtils::Bernstein(0.33f, RightProfile);

	RightProfile[1] = Right1;
	RightProfile[2] = Right2;
	for(i = 0; i<4; i++)
	{
		Button->BezierDefinition.anchors[0][i] = LeftProfile[i];
		Button->BezierDefinition.anchors[3][i] = RightProfile[i];
		for(j=1; j<=2; j++)
		{
			Button->BezierDefinition.anchors[j][i].x = 
				MathUtils::InterpolateValues(
				float(LeftProfile[i].x),
				float(RightProfile[i].x),
				j/3.0f
				);
		}
	}
	for (i = 1; i< 3; i++)
			for (j = 1; j< 3; j++)
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

