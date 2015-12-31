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
#ifndef ZoomFactory_H_
#define ZoomFactory_H_

#include "ZoomBase.h"

class ZoomFactory
{
	ZoomFactory(void);
	static ZoomFactory* Instance_;
	int ScreenWidth, ScreenHeight;
public:
	static ZoomFactory* Instance();
	static void Destroy();
	~ZoomFactory(void);

	void Setup(int ScreenWidth, int ScreenHeight);

	ZoomBase* CreateZoomType(int ZoomType, Frame* PictureObject, int Width, int Height, int StartTime, int ZoomTime);
};

#endif
