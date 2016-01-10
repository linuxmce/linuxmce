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
// C++ Interface: gl2deffecttransit
//
// Description: 
//
//
// Author: ,,, <opengl@edgar>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef GL2DEffectFadesFromTop_H
#define GL2DEffectFadesFromTop_H

#include "gl2deffecttransit.h"
#include "../Widgets/basicwindow.h"

namespace GLEffect2D
{

class GL2DEffectFadesFromTop : public GL2DEffectTransit
{
	TBasicWindow* Destination;

	FloatRect ButtonSize;
	FloatRect FullScreen;
	
public:
	GL2DEffectFadesFromTop (EffectFactory * EffectsEngine, int StartAfter, int TimeForCompleteEffect);
	virtual ~GL2DEffectFadesFromTop();
	
	void Configure(PlutoRectangle* EffectSourceSize);
	
	virtual void Paint(int Now);	
	
};

}

#endif
