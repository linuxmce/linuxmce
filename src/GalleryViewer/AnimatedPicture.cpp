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
#include "AnimatedPicture.h"

#include "ZoomFactory.h"
#include "MeshBuilder.h"
AnimatedPicture::AnimatedPicture(int ScreenWidth, int ScreenHeight)
{
	this->Picture = new GraphicImage();
}

AnimatedPicture::~AnimatedPicture(void)
{
}

bool AnimatedPicture::LoadFromFile(string FileName)
{
	bool Result = Picture->Load(FileName);
	PictureFrame = BuildRectangle(0.0f, 0.0f, 1.0f, 1.0f, Picture);
	return Result;
}

MeshFrame* AnimatedPicture::GetMeshFrame()
{
	return PictureFrame;
}

void AnimatedPicture::SetZoomKind(int StartTime, int ZoomTime)
{
	Zoom = ZoomFactory::Instance()->CreateZoomType(0, PictureFrame, 
		Picture->Width, Picture->Height, 
		StartTime, ZoomTime);
}

bool AnimatedPicture::Update(int Time)
{
	bool Result = Zoom->Update(Time);

	return Result;
}

void AnimatedPicture::Paint()
{
	PictureFrame->Paint();
}
