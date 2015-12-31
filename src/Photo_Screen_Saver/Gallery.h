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
#ifndef Gallery_H_
#define Gallery_H_

#include "DCE/Logger.h"

#include "SDLFrontEnd.h"
#include "GalleryScenario.h"
#include "ExtensionManager.h"

#include <string>
using namespace std;

namespace DCE
{
	class Photo_Screen_Saver;
};


class ProcessUtils
{
public:
	static unsigned long g_SecondsReset;
	void ResetMsTime();
	unsigned long GetMicroTime();
};

class Gallery
{
	Gallery();
	static Gallery m_Instance;

	void PaintScreen(bool bForceRefresh);

	ExtensionManager Extensions;
	SDLFrontEnd* FrontEnd;
	bool m_bQuit;
	pluto_pthread_mutex_t m_FrontEndMutex;
	pthread_cond_t m_FrontEndCond;

	bool m_bSuspended;
	bool m_bTextureReseted;

	/**
	* Last pending event definition
	*/
	WM_Event Event; 
	GalleryScenario* Scenario;
	ProcessUtils Utils;

public:
	~Gallery(void);
	void MainLoop(DCE::Photo_Screen_Saver *pPhoto_Screen_Saver);
	static Gallery& Instance();

    bool Setup(int Width, int Height, int FaddingTime, int ZoomTime, string FolderName, bool bUseAnimation, int nMaxSize);
	void CleanUp(void);
	void EvaluateEvent(WM_Event Event);
	void _Sleep(int Miliseconds);
	void m_bQuit_set(bool bQuit);

	void Pause();
	void Resume();
	void RefreshFileList(string sFileList);
};

#endif
