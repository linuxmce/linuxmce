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

#include "GL2DEffectTransitionNoEffect.h"
#include "gl2deffectfactory.h"

namespace GLEffect2D
{

GL2DEffectTransitionNoEffect::GL2DEffectTransitionNoEffect(EffectFactory * EffectsEngine, int StartAfter,
														   int TimeForCompleteEffect)
	: GL2DEffectTransit(EffectsEngine, StartAfter, TimeForCompleteEffect)
{
	Configured = false;
}


GL2DEffectTransitionNoEffect::~GL2DEffectTransitionNoEffect()
{

}

/**
 * Resets the time to be valid
 */
void GL2DEffectTransitionNoEffect::Paint(int Now)
{
	GL2DEffectTransit::Paint();
	if(!Configured) {
		this->Length = -1;
		Configured = true;
	}
	Effects->UpdateEffects();
}

/**
 *  No code... the efect really doesn't do nothing 
 */
void GL2DEffectTransitionNoEffect::Configure(PlutoRectangle* EffectSourceSize)
{
}

}
