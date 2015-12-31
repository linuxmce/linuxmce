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
#ifndef __GRAPHIC_BUILDER_H__
#define __GRAPHIC_BUILDER_H__

#include "PlutoGraphic.h"
#include "DesignObj_Orbiter.h"
//-------------------------------------------------------------------------------------------------------
void CreateVectorGraphic(VectorPlutoGraphic& vectPlutoGraphic, GraphicType Type, 
						 string Filename, eGraphicManagement GraphicManagement, class OrbiterRenderer *pOrbiterRenderer);

void CreatePlutoGraphic(PlutoGraphic& PlutoGraphic, GraphicType Type, 
						 string Filename, eGraphicManagement GraphicManagement, class OrbiterRenderer *pOrbiterRenderer);

//-------------------------------------------------------------------------------------------------------
#endif //__GRAPHIC_BUILDER_H__

