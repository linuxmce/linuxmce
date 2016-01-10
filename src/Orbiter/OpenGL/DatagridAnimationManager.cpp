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
#include "DatagridAnimationManager.h"
#include "AnimationScrollDatagrid.h"
#include "OpenGL3DEngine.h"

#include "DCE/Logger.h"
using namespace DCE;

bool DatagridAnimationManager::Update()
{
	if(!m_vectCurrentAnimations.size())
		return false;

	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, m_pEngine->GetSceneMutex());

	bool bAnimationFinished = false;

	for(vector<AnimationScrollDatagrid*>::iterator it = m_vectCurrentAnimations.begin();
		it != m_vectCurrentAnimations.end(); ++it)
	{
		AnimationScrollDatagrid *pAnimation = *it;
		if(pAnimation->Update(true))
		{
			bAnimationFinished = true;
		}
	}

	if(bAnimationFinished)
		OnStopAnimation();

	return true;
}

void DatagridAnimationManager::Cleanup()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, m_pEngine->GetSceneMutex());

	for(vector<AnimationScrollDatagrid*>::iterator it = m_vectCurrentAnimations.begin();
		it != m_vectCurrentAnimations.end(); ++it)
	{
		AnimationScrollDatagrid *pAnimation = *it;

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "DatagridAnimationManager::StopAnimation (anim %p) B(%p) - A(%p), pending %d",
			pAnimation, pAnimation->BeforeGrid, pAnimation->AfterGrid, m_vectCurrentAnimations.size() - 1);

		m_pEngine->RemoveMeshFrameFromDesktop(pAnimation->BeforeGrid);
		delete pAnimation;
	}

	m_vectCurrentAnimations.clear();
}

void DatagridAnimationManager::PrepareForAnimation(string ObjectID, MeshFrame *BeforeGrid, MeshFrame *AfterGrid,
		int MilisecondTime, int Direction, vector<string> Dependencies)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, m_pEngine->GetSceneMutex());

	AnimationScrollDatagrid* pAnimation = new AnimationScrollDatagrid(ObjectID, m_pEngine, BeforeGrid, AfterGrid, MilisecondTime, Direction, Dependencies); 

	for(vector<AnimationScrollDatagrid *>::iterator it = m_vectPendingAnimations.begin();
		it != m_vectPendingAnimations.end(); ++it)
	{
		AnimationScrollDatagrid* pLocalAnimation = *it;
		if(pLocalAnimation->ObjectID == ObjectID)
		{
			delete pLocalAnimation;
			m_vectPendingAnimations.erase(it);
			break;
		}
	}

	m_vectPendingAnimations.push_back(pAnimation);

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "DatagridAnimationManager::PrepareForAnimation %s B(%p) - A(%p), pending %d",
		ObjectID.c_str(), BeforeGrid, AfterGrid, m_vectPendingAnimations.size());

	pAnimation->StartAnimation();

	if(AnimationsPrepared())
	{
		OnStopAnimation();
		OnStartAnimation();
	}
}

bool DatagridAnimationManager::AnimationsPrepared()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, m_pEngine->GetSceneMutex());

	vector<string> vectPreparedObjects;
	map<string, bool> mapDependencies;

	for(vector<AnimationScrollDatagrid*>::iterator it = m_vectPendingAnimations.begin();
		it != m_vectPendingAnimations.end(); ++it)
	{
		AnimationScrollDatagrid *pAnimation = *it;
		vectPreparedObjects.push_back(pAnimation->ObjectID);

		for(vector<string>::iterator Item = pAnimation->Dependencies.begin(), End = pAnimation->Dependencies.end();
			Item != End; ++Item)
		{
			mapDependencies[*Item] = false;
		}
	}

	for(map<string, bool>::iterator itd = mapDependencies.begin(); itd != mapDependencies.end(); ++itd)
	{
		if(find(vectPreparedObjects.begin(), vectPreparedObjects.end(), itd->first) == vectPreparedObjects.end())
			return false;
	}

	return true;
}

void DatagridAnimationManager::OnStartAnimation()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, m_pEngine->GetSceneMutex());

	Cleanup();

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "DatagridAnimationManager::OnStartAnimation");

	int StartTime = SDL_GetTicks();
	for(vector<AnimationScrollDatagrid*>::iterator it = m_vectPendingAnimations.begin();
		it != m_vectPendingAnimations.end(); ++it
	)
	{
		AnimationScrollDatagrid *pAnimation = *it;
		
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "DatagridAnimationManager::OnStartAnimation %s B(%p) - A(%p)",
			pAnimation->ObjectID.c_str(), pAnimation->BeforeGrid, pAnimation->AfterGrid);

		m_pEngine->AddMeshFrameToDesktop("", pAnimation->AfterGrid);
		pAnimation->UpdateStartTime(StartTime);
		m_vectCurrentAnimations.push_back(pAnimation);
	}

	m_vectPendingAnimations.clear();
}

void DatagridAnimationManager::OnPrepareForAnimation()
{

}

void DatagridAnimationManager::OnStopAnimation()
{
	Cleanup();
}

void DatagridAnimationManager::ReplaceMeshInAnimations(MeshFrame *OldFrame, MeshFrame *NewFrame)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, m_pEngine->GetSceneMutex());

	for(vector<AnimationScrollDatagrid*>::iterator it = m_vectCurrentAnimations.begin();
		it != m_vectCurrentAnimations.end(); ++it)
	{
		AnimationScrollDatagrid *pAnimationScrollDatagrid = *it;

		if(pAnimationScrollDatagrid->AfterGrid == OldFrame)
		{
			pAnimationScrollDatagrid->AfterGrid = NewFrame;
		}

		if(pAnimationScrollDatagrid->BeforeGrid == OldFrame)
		{
			m_pEngine->RemoveMeshFrameFromDesktop(pAnimationScrollDatagrid->BeforeGrid);
			delete pAnimationScrollDatagrid;
			m_vectCurrentAnimations.erase(it);
			break;
		}
	}
}

void DatagridAnimationManager::StopAnimations()
{
	Cleanup();
}

void DatagridAnimationManager::StopPendingAnimations()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, m_pEngine->GetSceneMutex());

	for(vector<AnimationScrollDatagrid*>::iterator it = m_vectPendingAnimations.begin();
		it != m_vectPendingAnimations.end(); ++it)
	{
		AnimationScrollDatagrid *pAnimation = *it;

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "DatagridAnimationManager::StopPendingAnimation (anim %p) B(%p) - A(%p), pending %d",
			pAnimation, pAnimation->BeforeGrid, pAnimation->AfterGrid, m_vectPendingAnimations.size() - 1);

		m_pEngine->RemoveMeshFrameFromDesktop(pAnimation->BeforeGrid);
		delete pAnimation;
	}

	m_vectPendingAnimations.clear();
}

bool DatagridAnimationManager::HasAnimations()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, m_pEngine->GetSceneMutex());
	return m_vectCurrentAnimations.size() > 0;
}
