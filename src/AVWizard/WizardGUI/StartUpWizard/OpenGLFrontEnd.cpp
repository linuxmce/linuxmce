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
#include "OpenGLFrontEnd.h"

#include <iostream>
#include "Wizard.h"
#ifdef WIN32
#define LEAN_AND_MEAN
#include "windows.h"
#endif
#include <GL/gl.h>

OpenGLFrontEnd::OpenGLFrontEnd()
: GenericBackEnd(), Display(NULL)
{
}

OpenGLFrontEnd::~OpenGLFrontEnd(void)
{
}


int OpenGLFrontEnd::StartVideoMode(int Width, int Height, bool FullScreen)
{
	int Flags = SDL_OPENGL;
	if(Display)
		SDL_Quit();
	//Test if SDL inits nicely
	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_NOPARACHUTE)== -1)
	{
		std::cout<<"SDL library cannot init properly!"<<std::endl;
		Wizard::GetInstance()->SetExitWithCode(-1);
		return -1;
	}

	if (FullScreen)
		Flags = SDL_FULLSCREEN;

	Display = SDL_SetVideoMode(Width, Height, 0, Flags);
	if(!Display)
	{
		std::cout<<"Cannot init the video mode!"<<std::endl;
		Wizard::GetInstance()->SetExitWithCode(-1);
		return -1;
	}

	SDL_WM_SetCaption("AVWizard", "AVWizard");
	// Hide the mouse cursor
	SDL_ShowCursor(0);

	return 0;
}

void OpenGLFrontEnd::Flip(int LeftBorder, int TopBorder, int Border)
{
	SDL_GL_SwapBuffers();
	glClear(GL_COLOR_BUFFER_BIT);
}

int OpenGLFrontEnd::TextOutput(char* Text, int Left, int Top, TColorDesc Color, int Mode)
{
	return 0;
}
