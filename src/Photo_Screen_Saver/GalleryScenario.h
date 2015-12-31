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
#ifndef GalleryScenario_H_
#define GalleryScenario_H_

#include "GaleryStateMachine.h"
#include "FileBrowser.h"
#include "FadeFactory.h"
#include "ZoomFactory.h"
#include "ZoomBase.h"
#include "AnimatedPicture.h"


class GalleryScenario
{
	AnimatedPicture* AfterPicture;
	AnimatedPicture* BeforePicture;

	GaleryStateMachine* StateMachine;
	int StartTime;
	int StartFrame;
	int FaddingTime;
	int ZoomTime;
	int nLastTimeUpdated;

	int NewImageStartTime;
	bool m_bUseAnimation;
	int m_nMaxSize;

	FileBrowser* Browser;
	FadeBase* Fades;
	int Width;
	int Height;

	ZoomBase* ZoomEffect;
public:
	GalleryScenario(int Width, int Height, int FaddingTime, int ZoomTime, 
		string SearchImageFolder, bool bUseAnimation, int nMaxSize);
	~GalleryScenario();

	bool Update(bool bForceRefresh);
	void RefreshFileList(string sFileList);

	void Reset();
};

#endif
