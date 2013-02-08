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
#include "ZoomBase.h"

ZoomBase::ZoomBase(Frame* PictureObject, 
				   int ScreenWidth, int ScreenHeight, int Width, int Height, 
				   int StartTime, int ZoomTime)
{
	this->PictureObject = PictureObject;
	this->StartTime = StartTime;
	this->TimeToZoom = ZoomTime;

	this->ScreenWidth = ScreenWidth;
	this->ScreenHeight = ScreenHeight;

	this->Width = Width;
	this->Height = Height;

}

ZoomBase::~ZoomBase(void)
{

}
