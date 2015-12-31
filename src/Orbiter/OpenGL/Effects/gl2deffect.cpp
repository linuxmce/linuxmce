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
#include "gl2deffect.h"

namespace GLEffect2D 
{

Effect::Effect(
			   EffectFactory * EffectsEngine, 
			   int StartAfter, int TimeForCompleteEffect)
	: Effects(EffectsEngine),
	  Start(StartAfter),
	  Length(TimeForCompleteEffect),
	  Configured(false),
	  Reversed(false)
{
}

Effect::~Effect() {
}

float Effect::Stage(float Time) {
	if (Length<= 0) 
		return 1.1f;
	if(Time < Start)
		return -0.0f;
	if(Time > Start + Length)
		return 1.05f;
	float Result = ((float)Time-(float)Start) / (float)Length;
	if(Reversed)
		Result = 1.0f - Result;
	return Result;
} 


}

