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
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#ifndef ExtensionManager_H_
#define ExtensionManager_H_

#include <SDL.h>

class ExtensionManager
{
	SDL_Surface* Screen;
	float CleanRed, CleanGreen, CleanBlue;

public:
	int Width, Height;
	ExtensionManager(void);
	virtual ~ExtensionManager(void);
	
	virtual void Flip();
	virtual void SetClearColor(float Red, float Green, float Blue);
	virtual void ClearScreen(bool CleanColor = true, bool CleanZBuffer = true);
	virtual void EnableZBuffer(bool Enable);
	virtual void EnableBlended(bool Enable);

	virtual void Resize(int Width, int Height);

	virtual bool InitVideoMode(int Width, int Height, int Bpp, bool FullScreen, bool UseComposite);
	virtual void Setup();

	static bool CheckExtension(const char* checkFor);

	virtual void DrawArea(int X, int Y, int Width, int Height);
};


#endif
