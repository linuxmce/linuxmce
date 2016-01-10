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
//
// C++ Implementation: gl2deffectfactory
//
// Description: 
//
//
// Author: ciplogic <ciprian.m@plutohome.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "gl2deffectfactory.h"

/**
 * Implemented effects	
 */
#include "GL2DEffectBlending.h"

//basic transit effect
#include "GL2DEffectTransitionNoEffect.h"

//bezier based effects
#include "gl2deffectbeziertranzit.h"
#include "gl2deffectbeziertranzit_topleft.h"
#include "gl2deffectbeziertranzit_prism.h"
#include "gl2deffectbeziertranzit_flow_slideleft.h"
#include "gl2deffectbeziertranzit_flow_slideleft_reverse.h"

//strict 2D planes effects
#include "gl2deffectslidefromleft.h"
#include "gl2deffectfadesfromtop.h"
#include "gl2deffectfadesfromunderneath.h"
#include "GL2DEffectNoEffect.h"
#include "GL2DEffectHighLightArea.h"
#include "gl2deffectselectedarea.h"
#include "GL2DEffectWipeIn.h"

#include "../../../pluto_main/Define_Effect.h"

#include <iostream>
#include <SDL.h>

#include "../Layers/GL2DEffectLayer.h"

namespace GLEffect2D
{

EffectFactory::EffectFactory (Layer* ParentLayer, DrawingWidgetsEngine* WidgetsEngine)
	: Widgets(WidgetsEngine)
{
	this->ParentLayer = ParentLayer;
	StartTime = SDL_GetTicks();
}

int EffectFactory::MilisecondTimmer()
{ 
	int m_tInterval = SDL_GetTicks() - StartTime;

	return m_tInterval;
}

Effect* EffectFactory::CreateEffect(int IDEffect, int StartAfter, int TimeForComplete)
{
	if(!Effects.empty())
	{
		vector<Effect*>::iterator Effect = Effects.begin();
		if(NULL != dynamic_cast<GL2DEffectTransit*>(*Effect))
		{
			return NULL;
		}
	}

	Effect* Effect = NULL;
	switch (IDEffect)
	{
		case GL2D_EFFECT_TRANSIT_NO_EFFECT:
			Effect = new GL2DEffectTransitionNoEffect(this, StartAfter, TimeForComplete);
			break;
	
		case GL2D_EFFECT_TRANSIT:
			Effect = new GL2DEffectBlending(this, StartAfter, TimeForComplete);
			break;

		case GL2D_EFFECT_WIPE_IN:
			Effect = new GL2DEffectWipeIn(this, StartAfter, TimeForComplete);
			break;
		
		case GL2D_EFFECT_BEZIER_TRANSIT:
			Effect = new GL2DBezierEffectTransit (this, StartAfter, TimeForComplete);
			break;
		case GL2D_EFFECT_SLIDE_FROM_LEFT:
			Effect = new GL2DEffectSlideFromLeft (this, StartAfter, TimeForComplete);
			break;
		case GL2D_EFFECT_FADES_FROM_TOP:
			Effect = new GL2DEffectFadesFromTop(this, StartAfter, TimeForComplete);
			break;
		case GL2D_EFFECT_FADES_FROM_UNDERNEATH:
			Effect = new GL2DEffectFadesFromUnderneath (this, StartAfter, TimeForComplete);
			break;
		case GL2D_EFFECT_NOEFFECT:
			Effect = new GL2DEffectNoEffect(this, StartAfter, TimeForComplete);
			break;
		case GL2D_EFFECT_HIGHLIGHT_AREA:
			Effect = new GL2DEffectHighLightArea(this, StartAfter, TimeForComplete);
			break;
		case GL2D_EFFECT_SELECT_AREA:
			Effect = new GL2DEffectSelectedArea(this, StartAfter, TimeForComplete);
			break;
		case GL2D_EFFECT_BEZIER_TRANSIT_PRISM:
			Effect = new GL2DBezierEffectTransit_Prism(this, StartAfter, TimeForComplete);
			break;
		case GL2D_EFFECT_BEZIER_TRANSIT_TOPLEFT:
			Effect = new GL2DBezierEffectTransit_TopLeft(this, StartAfter, TimeForComplete);
			break;
		case GL2D_EFFECT_BEZIER_TRANZIT_FLOW_SLIDELEFT:
			Effect = new GL2DBezierEffectTransit_Flow_SlideLeft(this, StartAfter, TimeForComplete);
			break;
		case GL2D_EFFECT_BEZIER_TRANZIT_FLOW_SLIDELEFT_REVERSE:
			Effect = new GL2DBezierEffectTransit_Flow_SlideLeft_Reverse(this, StartAfter, TimeForComplete);
			break;
	}

	if(IDEffect >= GL2D_UNIQUE_EFFECT)
		ClearEffects();

	if (Effect != NULL)
		Effects.push_back(Effect);
	return Effect;
}

void EffectFactory::UpdateEffects()
{
	int CurrentTime = MilisecondTimmer();

	if(Effects.empty())
		return;
	
	vector<Effect*>::iterator Effect;
	
	for(Effect = Effects.begin(); Effect < Effects.end(); )
	{
		float EffectStage;
		if((*Effect)->Configured)
		{
			EffectStage = (*Effect)->Stage((float)CurrentTime);
		}
		
		if(((*Effect)->Configured) &&(EffectStage>1.0f) )
		{
			delete *Effect;
			Effect = Effects.erase(Effect);
		}
		else
			++Effect;
	}
	
	if (HasEffects())
	{
		vector<GLEffect2D::Effect*>::iterator Item;	
		for(Item = Effects.begin(); HasEffects() && (Item != Effects.end()); ++Item)
		{
			(*Item)->Paint(CurrentTime);
		}
	}
}

void EffectFactory::ClearEffects()
{
	if (Effects.size() == 0)
		return;

	vector<Effect*>::iterator Item;
 
	for(Item = Effects.begin(); Item < Effects.end(); ++Item)
		delete *Item;

	Effects.clear();
}

void EffectFactory::Paint(MeshTransform Transform)
{
	Widgets->Paint();
}

/*static*/ int EffectFactory::GetEffectCode(int DBEffectCode)
{
	switch(DBEffectCode) {
		//transit
		case EFFECT_No_transit_effect_CONST:
			return GL2D_EFFECT_TRANSIT_NO_EFFECT;
		case EFFECT_Basic_transit_effect_CONST:
			return GL2D_EFFECT_TRANSIT;
		case EFFECT_Bezier_effect_CONST:
			return GL2D_EFFECT_BEZIER_TRANSIT;
		case EFFECT_Slide_from_left_CONST:
			return GL2D_EFFECT_SLIDE_FROM_LEFT;
		case EFFECT_Slide_from_top_CONST:
			return GL2D_EFFECT_TRANSIT_NO_EFFECT; //IMPLEMENT ME!
		case EFFECT_Fades_from_top_CONST:
			//return GL2D_EFFECT_FADES_FROM_TOP;
			return GL2D_EFFECT_WIPE_IN;
		case EFFECT_Fades_from_underneath_CONST:
			return GL2D_EFFECT_FADES_FROM_UNDERNEATH;
		case EFFECT_Bezier_transit_prism_CONST:
			return GL2D_EFFECT_BEZIER_TRANSIT_PRISM;
		case EFFECT_Bezier_transit_top_left_CONST:
			return GL2D_EFFECT_BEZIER_TRANSIT_TOPLEFT;
		case EFFECT_Bezier_transit_flow_slide_left_CONST:
			return GL2D_EFFECT_BEZIER_TRANZIT_FLOW_SLIDELEFT;
		case EFFECT_No_select_or_hightlight_effect_CONST:
			return GL2D_EFFECT_BEZIER_TRANZIT_FLOW_SLIDELEFT_REVERSE;

		//select and highlight
		case EFFECT_Basic_highlight_effect_CONST:
			return GL2D_EFFECT_HIGHLIGHT_AREA;
		case EFFECT_Basic_select_effect_CONST:
			return GL2D_EFFECT_SELECT_AREA;
 	}
	return 0;
}

void EffectFactory::Resize(int Width, int Height)
{
	
}

} // namespace GLEffect2D

