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
#include "ZoomFactory.h"

#include "ZoomSimple.h"
#include "ZoomPanUp.h"

ZoomFactory* ZoomFactory::Instance_ = NULL;

ZoomFactory* ZoomFactory::Instance(void)
{
	if(NULL == Instance_)
		Instance_ = new ZoomFactory();
	return Instance_;
}

void ZoomFactory::Destroy()
{
	delete Instance_;
	Instance_ = NULL;
}

ZoomFactory::ZoomFactory(void)
{
}



ZoomFactory::~ZoomFactory(void)
{
}

void ZoomFactory::Setup(int ScreenWidth, int ScreenHeight)
{
	this->ScreenWidth = ScreenWidth;
	this->ScreenHeight = ScreenHeight;
}

ZoomBase* ZoomFactory::CreateZoomType(int ZoomType, Frame* PictureObject, int Width, int Height, 
									  int StartTime, int ZoomTime)
{
	ZoomType = 0;
	switch(ZoomType) {
	case 0:
		return new ZoomSimple(PictureObject, ScreenWidth, ScreenHeight, Width, Height, StartTime, ZoomTime);
		break;
	case 1:
		return new ZoomPanUp(PictureObject, ScreenWidth, ScreenHeight, Width, Height, StartTime, ZoomTime);
		break;
	default:
		return new ZoomSimple(PictureObject, ScreenWidth, ScreenHeight, Width, Height, StartTime, ZoomTime);
	}
	
}
