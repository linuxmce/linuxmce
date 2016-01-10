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
#include "TextureManager.h"
#include "../ExtensionManager.h"
#include "../Mesh/MeshFrame.h"

#include <GL/gl.h>

#include "../OpenGLGraphic.h"

#include "../OpenGL3DEngine.h"


#include "DCE/Logger.h"
//using namespace DCE;

/*static*/ TextureManager* TextureManager::Instance_ = NULL;

TextureManager* TextureManager::Instance() 
{
	if(Instance_ == NULL)
		Instance_ = new TextureManager();
	return Instance_;
}

TextureManager::TextureManager(void) 
	: LastTexture(0), 
	TextureEnable_(false),
	SupportTextureNonPowerOfTwo_(-1),
	TextureLock("texture mutex"),
	Engine(NULL),
	ReleaseTextureSuspended(0),
	m_bMemoryManagementEnabled(false),
	m_bSafeToReleaseTextures(false)
{
	m_bCacheEnabled = true;

	pthread_mutexattr_t MutexAttr;
	pthread_mutexattr_init(&MutexAttr);
	pthread_mutexattr_settype(&MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
	TextureLock.Init(&MutexAttr);
	pthread_mutexattr_destroy(&MutexAttr);
}

TextureManager::~TextureManager(void)
{
	EmptyCache();

	ReleaseTextureSuspended = false;

    ReleaseTextures();
	WaitForRelease.clear();
}

void TextureManager::EmptyCache()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, TextureLock);
	while(Graphics.size())
	{
		std::map<std::string, MeshFrame*>::iterator it = Graphics.begin();
		string name = it->first;
		MeshFrame *pFrame = it->second;
		Graphics.erase(it);

		pFrame->CleanUp();
		delete pFrame;
	}
	Graphics.clear();
}

void TextureManager::Setup(OpenGL3DEngine *Engine)
{
	this->Engine = Engine;
}

/*static*/ void TextureManager::SetupTexture(OpenGLTexture Texture)
{
	glEnd();
	
	if(0 != Texture)
	{
		if(!TextureEnable_)
		{
			glEnable(GL_TEXTURE_2D);
			TextureEnable_ = true;
		}
		
		glBindTexture(GL_TEXTURE_2D, Texture);
	}
	else
	{
		if(TextureEnable_)
		{
			glDisable(GL_TEXTURE_2D);
			TextureEnable_ = false;
		}
	}
	
	glBegin(GL_TRIANGLES);
	LastTexture = Texture;
}

void TextureManager::CleanUp()
{
	delete Instance_;
	Instance_ = NULL;
}

void TextureManager::PrepareConvert(OpenGLGraphic* TextureGraphic)
{
	if(!TextureGraphic)
		return;

	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, TextureLock);
	WaitForConvert.push_back(TextureGraphic);
}

void TextureManager::RemoveFromConvertQueue(OpenGLGraphic* TextureGraphic)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, TextureLock);
	WaitForConvert.remove(TextureGraphic);
}

void TextureManager::PrepareRelease(OpenGLTexture TextureGraphic)
{
	if(!TextureGraphic)
		return;

	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, TextureLock);
	WaitForRelease.push_back(TextureGraphic);
}

void TextureManager::ConvertImagesToTextures()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, TextureLock);

#ifdef DEBUG
	if(WaitForConvert.size())
		DCE::LoggerWrapper::GetInstance()->Write(LV_STATUS, "TextureManager::ConvertImagesToTextures size %d", WaitForConvert.size());
#endif

	std::list <OpenGLGraphic*>::iterator Item, End = WaitForConvert.end();
	for(Item = WaitForConvert.begin(); Item != End; ++Item)
	{
		if (*Item)
			(*Item)->Convert();
	}
	WaitForConvert.clear();
}

void TextureManager::ReleaseTextures()
{
	if(ReleaseTextureSuspended > 0)
		return;
#ifdef DEBUG
	if(WaitForRelease.size())
		DCE::LoggerWrapper::GetInstance()->Write(LV_STATUS, "TextureManager::ReleaseTextures size %d", WaitForRelease.size());
#endif

	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, TextureLock);
	std::list <OpenGLTexture>::iterator Item, End = WaitForRelease.end();
	for(Item = WaitForRelease.begin(); Item != End; ++Item)
	{
		if (*Item)
		{
			for(std::list <OpenGLGraphic*>::iterator it = WaitForConvert.begin(), 
				end = WaitForConvert.end();	it != end; ++it)
			{
				//we'll handle the release
				if((*it)->Texture == *Item)
					(*it)->Texture = 0;
			}

#ifdef VIDEO_RAM_USAGE
		VideoRAMUsageObserver::Instance().ObserveReleasingProcess(*Item);
#endif

			glDeleteTextures(1, &(*Item));
		}
	}
	WaitForRelease.clear();
}

bool TextureManager::SupportTextureNonPowerOfTwo()
{
	if(SupportTextureNonPowerOfTwo_ == -1)
	{
		SupportTextureNonPowerOfTwo_ = 
			ExtensionManager::CheckExtension("ARB_texture_rectangle") && 
			ExtensionManager::CheckExtension("ARB_texture_non_power_of_two");
	}

	return SupportTextureNonPowerOfTwo_ > 0;
}


MeshFrame* TextureManager::GetCacheItem(std::string ObjectHash)
{
	std::map<std::string, MeshFrame*>::iterator Item = Graphics.find(ObjectHash);

	if(Item == Graphics.end())
		return NULL;

	
	return Item->second;
}

void TextureManager::AddCacheItem(std::string ObjectHash, MeshFrame* Frame)
{
	if(ExistInCache(ObjectHash))
		return;

	Graphics[ObjectHash] = Frame;
}

void TextureManager::InvalidateItem(std::string ObjectHash)
{
	std::map<std::string, MeshFrame*>::iterator Item = Graphics.find(ObjectHash);

	if(Item != Graphics.end())
		Graphics.erase(Item);
}

bool TextureManager::ExistInCache(std::string ObjectHash)
{
	bool Result = Graphics.find(ObjectHash) != Graphics.end();
	return Result;
}

bool TextureManager::ExistInCache(MeshFrame *pFrame)
{
	for(std::map<std::string, MeshFrame*>::const_iterator it = Graphics.begin(),
		end = Graphics.end(); it != end; ++it)
	{
		if(it->second == pFrame)
			return true;
	}

	return false;
}

void TextureManager::InvalidateItem(MeshFrame *pFrame)
{
	for(std::map<std::string, MeshFrame*>::iterator it = Graphics.begin(),
		end = Graphics.end(); it != end; ++it)
	{
		if(it->second == pFrame)
		{
			Graphics.erase(it);
			return;
		}
	}
}

void TextureManager::AttachToScene(string ParentObjectID, MeshFrame* Frame)
{
	if(NULL != Engine)
	{
		MeshFrame *pFrameFromDesktop = Engine->AddMeshFrameToDesktop(ParentObjectID, Frame);
		if(NULL != pFrameFromDesktop && pFrameFromDesktop != Frame)
			Graphics[pFrameFromDesktop->Name()]	= pFrameFromDesktop;
	}
}

bool TextureManager::CacheEnabled()
{
	return m_bCacheEnabled;
}

void TextureManager::DisableCache()
{
	m_bCacheEnabled = false;
}

void TextureManager::EnableCache()
{
	m_bCacheEnabled = true;
}

