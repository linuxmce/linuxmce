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
#include "ZoomSimple.h"

#include "MathUtils.h"

#include "DCE/Logger.h"

ZoomSimple::ZoomSimple(Frame* PictureObject, int ScreenWidth, int ScreenHeight, int Width, int Height, int StartTime, int ZoomTime)
: ZoomBase(PictureObject, ScreenWidth, ScreenHeight, Width, Height, StartTime, ZoomTime)
{
        ZoomStart = getRandomZoomRect();
	ZoomEnd = getRandomZoomRect();
}

FloatRect* ZoomSimple::getRandomZoomRect() {
	float PictureRatio = (float) this->Width/this->Height;

	ZoomAmount1 = RandomInInterval(500, 1000)/1000.0f;
	// Delta values are random values between 0 and half the image size
	// This will make more of the image visible during the zoom than using the screen size
	DeltaX1 = RandomInInterval(0, int(this->Width/ZoomAmount1 - Width))/2;
	DeltaY1 = RandomInInterval(0, int(this->Height/ZoomAmount1 - Height))/2;

	// Must scale image so it covers screen width and height, keeping aspect ratio
	int w = this->ScreenWidth;
	int h = w / PictureRatio;
	float scale = (float)this->Width / this->ScreenWidth;
	if (h < this->ScreenHeight) {
	        // Image not high enough, use height as base
	        h = this->ScreenHeight;
		w = h * PictureRatio;
		scale = (float)this->Height / this->ScreenHeight;
	}
	// Adjust Delta values to new image scaling
	DeltaX1 = DeltaX1 / scale;
	DeltaY1 = DeltaY1 / scale;

	// */ZoomAmount will adjust image to cover zoom delta also
	return new FloatRect(float(-DeltaY1), float(-DeltaX1), (float)w/ZoomAmount1, (float)h/ZoomAmount1);
}

ZoomSimple::~ZoomSimple(void)
{
	delete ZoomStart;
	delete ZoomEnd;
}

bool ZoomSimple::Update(int Time)
{
	bool Result = false;
	int ProgressTime = Time - StartTime;
	float Progress = (float) ProgressTime / TimeToZoom;
	if (Progress > 1.0f)
	{
		Result = true;
		Progress = 1.0f;
	}

	FloatRect ZoomValue = ZoomStart->Interpolate(*ZoomEnd, Progress);
/*
	if(ZoomValue.Left>0)
		ZoomValue.Left = 0;
	if(ZoomValue.Top>0)
		ZoomValue.Top = 0;
	if(ZoomValue.Left + ZoomValue.Width < ScreenWidth)
		ZoomValue.Left = ScreenWidth - ZoomValue.Width;
	if(ZoomValue.Top + ZoomValue.Height < ScreenHeight)
		ZoomValue.Top = ScreenHeight - ZoomValue.Height;
*/

	Transform pTransform;
	pTransform.ApplyScale(ZoomValue.Width, ZoomValue.Height, 1);
	pTransform.ApplyTranslate(ZoomValue.Left, ZoomValue.Top, 0);
	//Transform.ApplyScale(ScreenWidth* (ScreenWidth/ZoomValue.Width), ScreenHeight* (ScreenHeight/ZoomValue.Height), 1);
	PictureObject->SetTransform(pTransform);

	return Result;
}
