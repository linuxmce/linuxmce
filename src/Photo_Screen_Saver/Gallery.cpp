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
#include "Photo_Screen_Saver.h"

#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include "ExtensionManager.h"
#include "Painter.h"

#include "DCE/Logger.h"

#ifdef WIN32
	#include <windows.h>
#else
	#include <fcntl.h>
	#include <sys/ipc.h>
	#include <sys/sem.h>
	#include <unistd.h>
	#include <sys/wait.h>
#endif

using namespace DCE;
unsigned long ProcessUtils::g_SecondsReset=0;

void ProcessUtils::ResetMsTime()
{
	timespec ts;
	gettimeofday( &ts, NULL );
	g_SecondsReset = ts.tv_sec;
}

unsigned long ProcessUtils::GetMicroTime()
{
	timespec ts;
	gettimeofday( &ts, NULL );
	return (ts.tv_sec-g_SecondsReset)*1000000 + (ts.tv_nsec/1000);
}



Gallery Gallery::m_Instance;

Gallery::Gallery(): m_bQuit(false), FrontEnd(NULL), Scenario(NULL), 
	m_FrontEndMutex("front end mutex"), m_bSuspended(false), m_bTextureReseted(false)
{
	pthread_cond_init(&m_FrontEndCond, NULL);
	m_FrontEndMutex.Init(NULL, &m_FrontEndCond);
}

Gallery::~Gallery(void)
{
	delete Scenario;
	Painter::Destroy();

	pthread_mutex_destroy(&m_FrontEndMutex.mutex);
}

void Gallery::MainLoop(Photo_Screen_Saver *pPhoto_Screen_Saver)
{
	while(!pPhoto_Screen_Saver->m_bQuit_get() && m_bQuit==false )
	{
		while (!pPhoto_Screen_Saver->m_bQuit_get() && (FrontEnd->HasEventPending()) && m_bQuit==false )
		{
			FrontEnd->TranslateEvent(Event);
			if(Event.Type)
				EvaluateEvent(Event);
		}
		bool StatusChange = Event.Type != -1;

		if(!pPhoto_Screen_Saver->m_bQuit_get() && m_bQuit==false )
		{
			PLUTO_SAFETY_LOCK_ERRORSONLY(vm, m_FrontEndMutex);
			if(StatusChange && !m_bSuspended)
			{
				bool bForceFlip = FrontEnd->GetLastEvent().type == SDL_VIDEOEXPOSE;
				
				if(bForceFlip)
					FrontEnd->GetLastEvent().type = 0;

				PaintScreen(bForceFlip);
				vm.Release();
				_Sleep(35);
			}
			else if(m_bSuspended)
			{
				if(NULL != Scenario && !m_bTextureReseted)
				{
					Scenario->Reset();
					m_bTextureReseted = true;

					pthread_cond_broadcast(&m_FrontEndCond);
				}

				vm.Release();
				_Sleep(80);
			}
			else
			{
				vm.Release();
				_Sleep(80);
			}
		}
	}
}

Gallery& Gallery::Instance(void)
{
	return m_Instance;
}

void Gallery::Pause()
{
	PLUTO_SAFETY_LOCK(vm, m_FrontEndMutex);
	m_bTextureReseted = false;
	m_bSuspended = true;

	//wait for textures to be released, but no more then 5 seconds
	if(ETIMEDOUT == vm.TimedCondWait(5, 0))
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "PSS's timed out releasing it's textures!");
	else
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "PSS released its textures!");
}

void Gallery::Resume()
{
	PLUTO_SAFETY_LOCK(vm, m_FrontEndMutex);
	m_bSuspended = false;
}

void Gallery::RefreshFileList(string sFileList)
{
	Scenario->RefreshFileList(sFileList);
}

void Gallery::PaintScreen(bool bForceRefresh)
{
	Utils.ResetMsTime();
	glClear(GL_COLOR_BUFFER_BIT);

	if(Scenario->Update(bForceRefresh))
		FrontEnd->Flip();
}

bool Gallery::Setup(int Width, int Height, int FaddingTime, int ZoomTime, string FolderName, bool bUseAnimation, int nMaxSize)
{
	Event.Type = 0;
	FrontEnd = new SDLFrontEnd();

	bool bFullScreen = false;
	bool Result = FrontEnd->StartVideoMode(Width, Height, bFullScreen) != 0;
	Painter::Instance()->Setup(&Extensions);
	Extensions.Resize(Width, Height);
	Scenario = new GalleryScenario(Width, Height, FaddingTime, ZoomTime, FolderName, bUseAnimation, nMaxSize);
	return Result;
}

void Gallery::CleanUp()
{
	PLUTO_SAFETY_LOCK(vm, m_FrontEndMutex);

	delete FrontEnd;
	FrontEnd = NULL;
	Event.Type = 0;
}

void Gallery::EvaluateEvent(WM_Event Event)
{
	if(Event.Type != 0)
		m_bQuit = true;  // Shouldn't be used anymore
}

void Gallery::_Sleep(int Miliseconds)
{
#ifdef WIN32
	::Sleep(Miliseconds);
#else
	usleep(Miliseconds*1000);
#endif
}

void Gallery::m_bQuit_set(bool bQuit)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Gallery::m_bQuit_set now %d",(int) bQuit);
	m_bQuit=bQuit;
}
