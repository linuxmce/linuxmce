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
#ifndef ZoomBase_H_
#define ZoomBase_H_

#include "Frame.h"

#include "MathUtils.h"

class ZoomBase
{
protected:
	int StartTime;
	int TimeToZoom;
	Frame* PictureObject;
	int ScreenWidth, ScreenHeight, Width, Height;
public:
	ZoomBase(Frame* PictureObject, int ScreenWidth, int ScreenHeight, int Width, int Height, int StartTime, int ZoomTime);
	virtual ~ZoomBase(void);

	virtual bool Update(int Time) = 0;


};

#endif
