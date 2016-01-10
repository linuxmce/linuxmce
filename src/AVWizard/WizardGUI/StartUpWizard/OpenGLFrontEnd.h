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
#ifndef OpenGLFrontEnd_H_
#define OpenGLFrontEnd_H_

#include "GenericBackEnd.h"

class OpenGLFrontEnd : public GenericBackEnd
{
	SDL_Surface* Display;
public:
	OpenGLFrontEnd(void);
	virtual ~OpenGLFrontEnd(void);

	int StartVideoMode(int Width, int Height, bool FullScreen);
	virtual void Flip(int LeftBorder, int TopBorder, int Border);

	virtual int TextOutput(char* Text, int Left, int Top, TColorDesc Color, int Mode);

};

#endif
