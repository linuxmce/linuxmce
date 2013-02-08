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
#ifndef ZoomSimple_H_
#define ZoomSimple_H_

#include "ZoomBase.h"

class ZoomSimple : public ZoomBase
{
	FloatRect* ZoomStart;
	FloatRect* ZoomEnd;
	int DeltaX1, DeltaY1, DeltaX2, DeltaY2;
	float ZoomAmount1, ZoomAmount2;

	FloatRect* getRandomZoomRect();

public:
	ZoomSimple(Frame* PictureObject, int ScreenWidth, int ScreenHeight, int Width, int Height, int StartTime, int ZoomTime);
	~ZoomSimple(void);

	virtual bool Update(int Time);
};

#endif 
