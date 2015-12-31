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
#ifndef GL2D_BEZIER_TRANZIT_TOP_LEFT_H
#define GL2D_BEZIER_TRANZIT_TOP_LEFT_H

#include "gl2deffecttransit.h"
#include "../Widgets/bezierwindow.h"
#include <memory>

namespace GLEffect2D
{

class GL2DBezierEffectTransit_TopLeft : public GL2DEffectTransit
{
	TBezierWindow* Button;	
	
	FloatRect ButtonSize;
	FloatRect FullScreen;
	
public:
	GL2DBezierEffectTransit_TopLeft (EffectFactory * EffectsEngine, int StartAfter,
		int TimeForCompleteEffect);
	virtual ~GL2DBezierEffectTransit_TopLeft();
	
	void Configure(PlutoRectangle* ButtonSourceSize);
	
	virtual void Paint(int Now);
	
};

}

#endif
