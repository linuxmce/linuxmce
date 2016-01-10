/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#ifndef __SDL_RENDER_TEXT_H__
#define __SDL_RENDER_TEXT_H__
//---------------------------------------------------------------------------------------------------
#include "RendererImage.h"
#include "../Orbiter/DesignObj_Orbiter.h"
#include "../Orbiter/TextStyle.h"
#include "../SerializeClass/ShapesColors.h"
//---------------------------------------------------------------------------------------------------
class SDL_RenderText
{
private:
	std::string m_sFontPath;

public:

	SDL_RenderText(std::string sFontPath) : m_sFontPath(sFontPath) {}

	PlutoSize RealRenderText(RendererImage *pRenderImage, DesignObjText *pDesignObjText, 
		TextStyle *pTextStyle, PlutoPoint pos);

	pair<int, int> GetWordWidth(string Word, TextStyle *pTextStyle, 
		RendererImage *& RI, bool NewSurface = true);
};
//---------------------------------------------------------------------------------------------------
#endif //__SDL_RENDER_TEXT_H__
