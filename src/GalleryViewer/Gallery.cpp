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
#include "Gallery.h"

#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include "ExtensionManager.h"
#include "MeshPainter.h"

Gallery* Gallery::Instance_ = NULL;

Gallery::Gallery():
	Quit(false)
{
}

Gallery::~Gallery(void)
{
	delete Scenario;
}

void Gallery::MainLoop(void)
{
	while(!Quit)
	{
		while (!Quit && (FrontEnd->HasEventPending()))
		{
			FrontEnd->TranslateEvent(Event);
			if(Event.Type)
				EvaluateEvent(Event);
		}
		bool StatusChange = Event.Type != -1;

		if(!Quit)
		{
			if(StatusChange)
			{
				PaintScreen();
				Sleep(5);
			}
			else
			{
				Sleep(10);
			}
		}
	}
}

Gallery* Gallery::Instance(void)
{
	if(NULL == Instance_)
		Instance_ = new Gallery();
	return Instance_;
}

void Gallery::PaintScreen(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	Scenario->Update();

	FrontEnd->Flip();
}

bool Gallery::Setup(int Width, int Height, string FolderName)
{
	FrontEnd = new SDLFrontEnd();
	bool Result = FrontEnd->StartVideoMode(Width, Height, false) != 0;
	MeshPainter::Instance()->Setup(&Extensions);
	Extensions.Resize(Width, Height);
	Scenario = new GalleryScenario(Width, Height, 400, 1600);
	return Result;
}

void Gallery::CleanUp(void)
{
}

void Gallery::EvaluateEvent(WM_Event Event)
{
	if(Event.Type == WMET_ESCAPE_KEY)
		Quit = true;
}

void Gallery::Sleep(int Miliseconds)
{
#ifdef WIN32
	::Sleep(Miliseconds);
#else
	usleep(Miliseconds*1000);
#endif
}
