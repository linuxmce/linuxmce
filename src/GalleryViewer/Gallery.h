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

#include "SDLFrontEnd.h"
#include "GalleryScenario.h"
#include "ExtensionManager.h"

#include <string>
using namespace std;

class Gallery
{
	ExtensionManager Extensions;
	static Gallery* Instance_;
	void PaintScreen(void);
	SDLFrontEnd* FrontEnd;
	bool Quit;
	/**
	* Last pending event definition
	*/
	WM_Event Event; 
	GalleryScenario* Scenario;
public:
	Gallery(void);
	~Gallery(void);
	void MainLoop(void);
	static Gallery* Instance(void);
private:
public:
	bool Setup(int Width, int Height, string FolderName);
	void CleanUp(void);
	void EvaluateEvent(WM_Event Event);
	void Sleep(int Miliseconds);
};

#endif
