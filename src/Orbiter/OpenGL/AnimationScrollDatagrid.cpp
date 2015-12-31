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
Orbiter

Copyright (C) 2006 Pluto, Inc., a Florida Corporation

www.plutohome.com

Phone: +1 (877) 758-8648

This program is distributed according to the terms of the Pluto Public License, available at:
http://plutohome.com/index.php?section=public_license

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

@author: "Ciprian Mustiata" ciprian.m at plutohome dot com, "Cristian Miron" chris.m at plutohome dot com 

*/
#include "AnimationScrollDatagrid.h"

#include "Mesh/MeshFrame.h"
#include "OpenGL3DEngine.h"

#include "DCE/Logger.h"

AnimationScrollDatagrid::AnimationScrollDatagrid(string ObjectID, OpenGL3DEngine* Engine, MeshFrame *BeforeGrid, 
	MeshFrame *AfterGrid, int MilisecondTime, int Direction, vector <string> pDependencies)
{
	Finished = false;
	this->Engine = Engine;
	this->BeforeGrid = BeforeGrid;
	this->AfterGrid = AfterGrid;
	this->MilisecondTime = MilisecondTime;
	this->Direction = Direction;
	this->ObjectID = ObjectID;

	DCE::LoggerWrapper::GetInstance()->Write(LV_STATUS, "abc AnimationScrollDatagrid before %p/%s and after %p/%s",
		BeforeGrid, BeforeGrid->Name().c_str(), AfterGrid, AfterGrid->Name().c_str());

	CopyDependencies(pDependencies);
}

AnimationScrollDatagrid::~AnimationScrollDatagrid(void)
{
	AfterGrid = NULL;
	BeforeGrid = NULL;

	TextureManager::Instance()->ResumeTextureRelease();
}

void AnimationScrollDatagrid::StartAnimation()
{
	StartTime = SDL_GetTicks();
}

bool AnimationScrollDatagrid::Update(bool ModifyGeometry)
{
	if(Finished)
		return true;

	bool Result = false;
	int CurrentTime = Engine->GetTick();
	float Progress = float(CurrentTime - StartTime) / MilisecondTime;
	if(CurrentTime - StartTime > MilisecondTime)
	{
		Result = true;
		Progress = 1.0f;
	}

	if(!ModifyGeometry)
		return false;

	int Height = Engine->Compose->Height;

	if(Direction != 2)
	{
		MeshTransform Transform;
		Transform.Translate(-0, float(-Height/2), float(-Height/2));
		Transform.ApplyRotateX(float((-Progress)*90));
		Transform.ApplyTranslate(0, float(Height/2), float(Height/2));
		BeforeGrid->SetTransform(Transform);

		MeshTransform Transform2;
		Transform2.Translate(0, float(-Height/2), float(-Height/2));
		Transform2.ApplyRotateX(float((1-Progress)*90));
		Transform2.ApplyTranslate(0, float(Height/2), float(Height/2));
		AfterGrid->SetTransform(Transform2);
	}
	else
	{
		MeshTransform Transform;
		Transform.Translate(-0, float(-Height/2), float(-Height/2));
		Transform.ApplyRotateX(float((Progress)*90));
		Transform.ApplyTranslate(0, float(Height/2), float(Height/2));
		BeforeGrid->SetTransform(Transform);

		MeshTransform Transform2;
		Transform2.Translate(0, float(-Height/2), float(-Height/2));
		Transform2.ApplyRotateX(float((-1+Progress)*90));
		Transform2.ApplyTranslate(0, float(Height/2), float(Height/2));
		AfterGrid->SetTransform(Transform2);
	}

	if(Result)
	{
		return true; 
	}
	Engine->UnHighlight();

	return Result;
}

void AnimationScrollDatagrid::CopyDependencies(vector <string> pDependencies)
{
	this->Dependencies.clear();
	for(vector<string>::iterator it = pDependencies.begin(), End = pDependencies.end();
		it != End; ++it)
		Dependencies.push_back(*it);

}

bool AnimationScrollDatagrid::UpdateStartTime(int StartTime)
{
	if(this->StartTime >= StartTime)
		return false;
	this->StartTime = StartTime;
	return true;
}

int AnimationScrollDatagrid::GetStartTime()
{
	return StartTime;
}

void AnimationScrollDatagrid::StopAnimation()
{
	Finished = true;
	MeshTransform Transform2;
	AfterGrid->SetTransform(Transform2);
	Engine->RemoveMeshFrameFromDesktop(BeforeGrid);
}


