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
#include "gl2deffecttransit.h"

#include "gl2deffectfactory.h"

namespace GLEffect2D
{

GL2DEffectTransit::GL2DEffectTransit(EffectFactory * EffectsEngine, int StartAfter, 
									 int TimeForCompleteEffect)
	: Effect(EffectsEngine, StartAfter, TimeForCompleteEffect)
{
	//clean up the highlighted items to not have remain on the new screen
/*	Commons3D::Instance().SetHighLightArea(NULL);
	Commons3D::Instance().SetSelectedArea(NULL);
	TBasicWindow* Screen3D = Commons3D::Instance().GetScreen3D();
	Screen3D->SetBackgroundImage(
		Effects->Widgets->OldScreen
	);
	*/
}

GL2DEffectTransit::~GL2DEffectTransit()
{
	//clean up the highlighted items to not have remain on the new screen
/*
  	Commons3D::Instance().SetHighLightArea(NULL);
	Commons3D::Instance().SetSelectedArea(NULL);
	Commons3D::Instance().GetScreen3D()->SetBackgroundImage(Effects->Widgets->NewScreen);
*/
}

void GL2DEffectTransit::Paint()
{
	//clean up the highlighted items to not have remain on the new screen
/*
 	Commons3D::Instance().SetHighLightArea(NULL);
	Commons3D::Instance().SetSelectedArea(NULL);

	TBasicWindow* WinHigh = Commons3D::Instance().GetHighLight();
	int Width = WinHigh->GetWidth();
	int Height = WinHigh->GetHeight();

	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Transit higlight size: %d %d\n", Width, Height);

	if(!Configured) {
		Commons3D::Instance().GetScreen3D()->SetTexture(Effects->Widgets->OldScreen);
	}
*/
	
}

}
