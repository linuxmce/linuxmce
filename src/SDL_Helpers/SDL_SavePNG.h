/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#ifndef SDL_SAVEPNG_H
#define SDL_SAVEPNG_H

#include <stdio.h>
#include "SDL.h"

#include <string>
using namespace std;

bool IMG_SavePNG(SDL_Surface *pSurface, FILE * FileHandle, bool bPrependSignature = true);
bool IMG_SavePNG(SDL_Surface *pSurface, const string & sFileName);

#endif /*SDL_SAVEPNG_H*/
