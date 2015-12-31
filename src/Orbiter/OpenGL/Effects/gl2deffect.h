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
#ifndef GL2DEffect_H
#define GL2DEffect_H

#include "../GLMathUtils.h"

/**
 * That class is the base class for 2D effects using OpenGL
 */
namespace GLEffect2D
{
class EffectFactory;

class Effect
{
protected:
	EffectFactory* Effects;
	int Start, Length;

public:
	bool Configured;
	bool Reversed;

	Effect(EffectFactory* EffectsEngine, int StartAfter, int TimeForCompleteEffect);
	virtual ~Effect();
	
	/**
	 * Return the stage of the effect, from 0.0 to 1.0
	 * 0 = StartTime
	 * 1.0 = StartTime+Length;
	 */
	float Stage(float Time);
  
	virtual void Paint(int Time) = 0;

	virtual void Configure(PlutoRectangle* EffectSourceSize) = 0;
	
	//virtual void Configure(PlutoRectangle* EffectSourceSize) = 0;
};

}

#endif
