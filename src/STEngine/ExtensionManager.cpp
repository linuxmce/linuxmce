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

#include "Painter.h"

ExtensionManager::ExtensionManager(void)
: Width(0), Height(0)
{
	
}

ExtensionManager::~ExtensionManager(void)
{
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

	glEnable(GL_TEXTURE_2D);


	glScalef(1.f, -1.f, -1.f);
	//glTranslatef(-Width/2, -Height/2, Height);
	this->Width = Width;
	this->Height = Height;
	DrawArea(0, 0, Width, Height);
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
	Painter::Instance()->Setup(this);
}

/*static*/ bool ExtensionManager::CheckExtension(const char* checkFor)
{
	const GLubyte* extensions = glGetString(GL_EXTENSIONS);
	string sExtensions(reinterpret_cast<const char *>(extensions));
	return sExtensions.find(checkFor) != string::npos;
}

/*virtual*/ void ExtensionManager::DrawArea(int X, int Y, int Width, int Height)
{
	glPopMatrix();
	glPushMatrix();
	glViewport(X, Y, Width, Height);
	glTranslatef(float(-Width/2), float(-Height/2), float(Height/2));
	glScalef(1, 1, 1);
}
