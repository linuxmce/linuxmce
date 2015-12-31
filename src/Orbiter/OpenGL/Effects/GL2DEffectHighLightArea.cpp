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
#include "GL2DEffectHighLightArea.h"

#include "gl2deffectfactory.h"

namespace GLEffect2D
{

GL2DEffectHighLightArea::GL2DEffectHighLightArea(EffectFactory * EffectsEngine, int StartAfter,
												 int TimeForCompleteEffect)
: GL2DEffectHighLight(EffectsEngine, StartAfter, TimeForCompleteEffect)
{
	Configured = false;
	
	Button = dynamic_cast<TBasicWindow*> (Effects->Widgets->CreateWidget(
		BASICWINDOW,
		0, 0, 0, 0,
		"None"
		));
		
}

GL2DEffectHighLightArea::~GL2DEffectHighLightArea(void)
{
	Effects->Widgets->DeleteWidget(Button);
	Button = NULL;
}


void GL2DEffectHighLightArea::Configure(PlutoRectangle* EffectSourceSize)
{
	if (!EffectSourceSize)
		return;

	this->EffectSourceSize.Left = (float)EffectSourceSize->X;
	this->EffectSourceSize.Top = (float)EffectSourceSize->Y;
	this->EffectSourceSize.Width = (float)EffectSourceSize->Width;
	this->EffectSourceSize.Height = (float)EffectSourceSize->Height;
}

void GL2DEffectHighLightArea::Paint(int Time)
{
	Configured = true;
	
	if(!Configured)
	{
		Start = Effects->MilisecondTimmer();
		Configured = true;
		Button->SetRectCoordinates(EffectSourceSize);
		Button->SetVisible(true);
	}
	float Step = Stage(float(Time));
	if (Step <0.5)
	{
		Button->SetBackgroundColor(1.0f, 1.0f, 0.0f, Step);
	}
	else
	{
		Button->SetBackgroundColor(1.0f, 1.0f, 0.0f, 0.5f - (Step-0.5f)*2.f*0.3f);
	}
	//Commons3D::Instance().GetSelected()->SetVisible(true);
}

}
