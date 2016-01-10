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
#ifndef __POCKETFROG_GRAPHIC_H__
#define __POCKETFROG_GRAPHIC_H__
//-------------------------------------------------------------------------------------------------------
#include <string>
using namespace std;
//-------------------------------------------------------------------------------------------------------
#include "../PlutoGraphic.h"
#include <PocketFrog.h>
using namespace Frog;
//-------------------------------------------------------------------------------------------------------
class OrbiterRenderer;
//-------------------------------------------------------------------------------------------------------
class PocketFrogGraphic : public PlutoGraphic
{
public:
	PocketFrogGraphic(string Filename, eGraphicManagement GraphicManagement, OrbiterRenderer *pOrbiterRenderer);
	PocketFrogGraphic(Surface *pSurface);
	PocketFrogGraphic(OrbiterRenderer *pOrbiterRenderer);
	~PocketFrogGraphic();

	void Initialize();
	GraphicType GraphicType_get() { return gtPocketFrogGraphic; }
	bool IsEmpty() { return NULL == m_pSurface; }
	bool LoadGraphic(char *pData, size_t iSize,int iRotation=0);
	void Clear(); 
    PlutoGraphic *GetHighlightedVersion();
	PlutoGraphic *Clone();
	bool GetInMemoryBitmap(char*& pRawBitmapData, size_t& ulSize);

	class Surface *m_pSurface;
};
//-------------------------------------------------------------------------------------------------------
#endif //__POCKETFROG_GRAPHIC_H__

