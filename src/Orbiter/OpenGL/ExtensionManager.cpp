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
#include "ExtensionManager.h"

#ifdef WIN32
	#define LEAN_AND_MEAN
	#include <Windows.h>
#else
	#include <GL/glx.h>
#endif

#include <SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#ifdef VIA_OVERLAY
#include "VIA/ViaOverlay.h"
#endif

#include "Mesh/MeshPainter.h"

#include "../../DCE/Logger.h"
using namespace DCE;


#include "SDL_syswm.h"

#ifndef USE_SDL_GL_PATCH
	#if !defined(WIN32) && !defined(VIA_OVERLAY)
		#include "utilities/linux/CompositeHelper.h"
	#endif
#endif

ExtensionManager::ExtensionManager(void)
: Width(0), Height(0)
{
	
}

ExtensionManager::~ExtensionManager(void)
{
	SDL_FreeSurface(Screen);
	Screen = NULL;
}

/*virtual*/ void ExtensionManager::SetClearColor(float Red, float Green, float Blue)
{
	this->CleanRed = Red;
	this->CleanGreen = Green;
	this->CleanBlue = Blue;
}

/*virtual*/ void ExtensionManager::ClearScreen(bool CleanColor, bool CleanZBuffer)
{
	if(!CleanColor && !CleanZBuffer)
		return;
	
	glClearColor(CleanRed, CleanGreen, CleanBlue, 0.0f);
	unsigned int uFlags = 0;
	if(CleanColor)
		uFlags |= GL_COLOR_BUFFER_BIT;
	if(CleanZBuffer)
		uFlags |= GL_DEPTH_BUFFER_BIT;
		
	glClear(uFlags);
	
}

/*virtual*/ void ExtensionManager::EnableZBuffer(bool Enable)
{
	if(Enable)
		glEnable(GL_DEPTH_TEST);
	else	
		glDisable(GL_DEPTH_TEST);
}

/*virtual*/ void ExtensionManager::Flip()
{
	SDL_GL_SwapBuffers();
}

void ExtensionManager::Resize(int Width, int Height)
{
	if(Height == 0)
		Height = 1;
	// Set the viewport for the OpenGL window
	glViewport(0, 0, Width, Height);
	// Change Matrix Mode to Projection
	glMatrixMode(GL_PROJECTION);
	// Do the perspective calculations. Last value = max clipping depth
	gluPerspective(90.0, (float)Width/Height, 0.001, 5000);
	// Return to the modelview matrix
	glMatrixMode(GL_MODELVIEW);
	// Reset View
	glLoadIdentity();      
	// Enables Smooth Color Shading
	glShadeModel(GL_SMOOTH);
	//Realy Nice perspective calculations
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glClearDepth(1.0);                       // Depth Buffer Setup
    //glEnable(GL_DEPTH_TEST);                 // Enable Depth Buffer
    glDepthFunc(GL_LESS);		           // The Type Of Depth Test To Do

	EnableBlended(true);

	glScalef(1.f, -1.f, -1.f);
	//glTranslatef(-Width/2, -Height/2, Height);
	this->Width = Width;
	this->Height = Height;

#ifdef VIA_OVERLAY
		ViaOverlay::Instance().WindowResized();
#endif
}

/*virtual*/ bool ExtensionManager::InitVideoMode(int Width, int Height, int Bpp, bool FullScreen, 
		bool UseComposite)
{
#ifndef USE_SDL_GL_PATCH
#if !defined(WIN32) && !defined(VIA_OVERLAY)	
	if(UseComposite)
	{
		Display *dpy = CompositeHelper::GetInstance().GetDisplay();

		if(NULL == dpy)
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed to open display!");
			return false;
		}

		XSynchronize(dpy, 1);
		XVisualInfo *visinfo = CompositeHelper::GetInstance().GetVisualForComposite();

		if(NULL != visinfo && CompositeHelper::GetInstance().RegisterReplacementWindowForSDL(visinfo, Width, Height))
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Replacement SDL window created!");
		
		XSynchronize(dpy, 1);
	}
#endif
#endif //USE_SDL_GL_PATCH

	if(SDL_INIT_VIDEO & SDL_WasInit(SDL_INIT_VIDEO))
		SDL_Quit();

	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE)<0)
		return false;

#ifndef USE_SDL_GL_PATCH
#if !defined(WIN32) && !defined(VIA_OVERLAY)
	CompositeHelper::GetInstance().UnregisterReplacementWindowForSDL();
#endif
#endif
		
	SDL_WM_SetCaption("OrbiterGL", "OrbiterGL");

	/* Sets up OpenGL double buffering */
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

#ifdef USE_SDL_GL_PATCH
#if !defined(WIN32) && !defined(VIA_OVERLAY) 
	if(UseComposite)
	{
		SDL_GL_SetAttribute(SDL_GL_RENDER_TYPE,   GLX_RGBA_BIT);
		SDL_GL_SetAttribute(SDL_GL_DRAWABLE_TYPE, GLX_WINDOW_BIT);

		SDL_GL_SetAttribute(SDL_GL_RED_SIZE,      8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,    8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,     8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,    8);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,  0);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,    0);
	}
#endif
#endif

	Uint32 uVideoModeFlags = SDL_OPENGL;

	#ifndef WIN32
		FullScreen = false;
	#endif

	#if !defined(WIN32) || defined(WINCE)
		if(FullScreen)
			uVideoModeFlags |= SDL_FULLSCREEN;
	#endif

	#ifdef VIA_OVERLAY
          LoggerWrapper::GetInstance()->Write(LV_TV, "VIA : set env VIA_3D_OVERLAY");
		
	  unsigned long dwRet = setenv("VIA_3D_OVERLAY","no",1);
	  if(dwRet !=0)
	  {
		  LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "VIA : couldn't set env VIA_3D_OVERLAY");
	  }
	#endif

	Screen = SDL_SetVideoMode(Width, Height, Bpp, uVideoModeFlags);

	if(NULL != Screen)
	{

#if !defined(WIN32)

		LoggerWrapper::GetInstance()->Write(LV_TV, "Ready to remove window decorations for Orbiter");
		
		SDL_SysWMinfo info;
        SDL_VERSION(&info.version);
        if ( SDL_GetWMInfo(&info) )
        {
                SDL_bool set;
                Atom WM_HINTS;

                /* We haven't modified the window manager hints yet */
                set = SDL_FALSE;

                Window WMwindow = info.info.x11.wmwindow;

				LoggerWrapper::GetInstance()->Write(LV_TV, "Attempt to remove _MOTIF_WM_HINTS");

                /* First try to set MWM hints */
                WM_HINTS = XInternAtom(info.info.x11.display, "_MOTIF_WM_HINTS", True);
                if ( WM_HINTS != None ) 
				{
                        /* Hints used by Motif compliant window managers */
                        struct {
                                unsigned long flags;
                                unsigned long functions;
                                unsigned long decorations;
                                long input_mode;
                                unsigned long status;
                        } MWMHints = { (1L << 1), 0, 0, 0, 0 };

                        XChangeProperty(info.info.x11.display, WMwindow,
                                        WM_HINTS, WM_HINTS, 32,
                                        PropModeReplace,
                                        (unsigned char *)&MWMHints,
                                        sizeof(MWMHints)/sizeof(long));
                        set = SDL_TRUE;

						LoggerWrapper::GetInstance()->Write(LV_TV, "Removed _MOTIF_WM_HINTS");
                }

				LoggerWrapper::GetInstance()->Write(LV_TV, "Attempt to remove KWM_WIN_DECORATION");

                /* Now try to set KWM hints */
                WM_HINTS = XInternAtom(info.info.x11.display, "KWM_WIN_DECORATION", True);
                if ( WM_HINTS != None ) 
				{
                        long KWMHints = 0;

                        XChangeProperty(info.info.x11.display, WMwindow,
                                        WM_HINTS, WM_HINTS, 32,
                                        PropModeReplace,
                                        (unsigned char *)&KWMHints,
                                        sizeof(KWMHints)/sizeof(long));
                        set = SDL_TRUE;

						LoggerWrapper::GetInstance()->Write(LV_TV, "Removed KWM_WIN_DECORATION");
                }

				LoggerWrapper::GetInstance()->Write(LV_TV, "Attempt to remove _WIN_HINTS");

                /* Now try to set GNOME hints */
                WM_HINTS = XInternAtom(info.info.x11.display, "_WIN_HINTS", True);
                if ( WM_HINTS != None ) 
				{
                        long GNOMEHints = 0;

                        XChangeProperty(info.info.x11.display, WMwindow,
                                        WM_HINTS, WM_HINTS, 32,
                                        PropModeReplace,
                                        (unsigned char *)&GNOMEHints,
                                        sizeof(GNOMEHints)/sizeof(long));
                        set = SDL_TRUE;

						LoggerWrapper::GetInstance()->Write(LV_TV, "Removed _WIN_HINTS");
                }


		 XMoveWindow(info.info.x11.display, WMwindow, 0, 0);
        }
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Failed to remove window decorations for Orbiter");
		}
#endif

#ifdef VIA_OVERLAY
		//give time for orbiter's window to process its messages
		Sleep(1000);

		//create via overlay connection / enable alpha overlay
		ViaOverlay::Instance().WindowCreated(Width, Height);
#endif

		Resize(Width, Height);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_CULL_FACE);
	}

	return Screen != NULL;
}

/*virtual*/ void ExtensionManager::EnableBlended(bool Enable)
{
	if(Enable)
	{
		glEnable(GL_BLEND);
		//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else
	{
#ifndef WIN32
		glDisable(GL_BLEND);
#endif
	}
}

/*virtual*/ void ExtensionManager::Setup()
{
	MeshPainter::Instance()->Setup(this);
}

/*static*/ bool ExtensionManager::CheckExtension(const char* checkFor)
{
	const GLubyte* extensions = glGetString(GL_EXTENSIONS);
	if(NULL != extensions)
	{
		string sExtensions(reinterpret_cast<const char *>(extensions));
		return sExtensions.find(checkFor) != string::npos;
	}

	return false;
}

/*virtual*/ void ExtensionManager::DrawArea(int X, int Y, int Width, int Height)
{
	glViewport(X, Y, Width, Height);
	glTranslatef(float(-Width/2), float(Height/2), float(Height/2));
	glScalef(1, -1, 1);
}
