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
#ifndef TextureManager_H_
#define TextureManager_H_

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <WINDOWS.H>
#endif

#include <GL/gl.h>

#include <queue>
#include <map>
#include <string>
#include <pthread.h>
#include "PlutoUtils/MultiThreadIncludes.h"

typedef GLuint OpenGLTexture; 

class OpenGLGraphic;
class MeshFrame;
class OpenGL3DEngine;

class TextureManager
{
	OpenGLTexture LastTexture;
	TextureManager();
	static TextureManager* Instance_;
	bool TextureEnable_;
	bool m_bCacheEnabled;

	int SupportTextureNonPowerOfTwo_;

	pluto_pthread_mutex_t TextureLock;
	
	std::list <OpenGLGraphic*> WaitForConvert;
	std::list <OpenGLTexture> WaitForRelease;
	std::map<std::string, MeshFrame*> Graphics;

	bool ExistInCache(std::string ObjectHash);

	OpenGL3DEngine *Engine;
	int ReleaseTextureSuspended;
	bool m_bMemoryManagementEnabled;
	bool m_bSafeToReleaseTextures;

public:
	static TextureManager* Instance();
	virtual ~TextureManager(void);

	void EnableMemoryManagement()   { m_bMemoryManagementEnabled = true; }
	bool MemoryManagementEnabled()  { return m_bMemoryManagementEnabled; }

	void SafeToReleaseTextures(bool bValue)	{ m_bSafeToReleaseTextures = bValue; }
	bool IsSafeToReleaseTextures()			{ return m_bSafeToReleaseTextures;	 }

	void EmptyCache();
	bool ExistInCache(MeshFrame *pFrame);
	void Setup(OpenGL3DEngine *Engine);

	void SetupTexture(OpenGLTexture Texture);
	void CleanUp();
	
	void PrepareConvert(OpenGLGraphic* TextureGraphic);
	void ConvertImagesToTextures();

	void RemoveFromConvertQueue(OpenGLGraphic* TextureGraphic);
 
	void PrepareRelease(OpenGLTexture Texture);
	void ReleaseTextures();
	void SuspendTextureRelease() { ++ReleaseTextureSuspended; }
	void ResumeTextureRelease()  { --ReleaseTextureSuspended; }

	bool SupportTextureNonPowerOfTwo();

	MeshFrame* GetCacheItem(std::string ObjectHash);
	void AddCacheItem(std::string ObjectHash, MeshFrame* Frame);
	void InvalidateItem(std::string ObjectHash);
	void InvalidateItem(MeshFrame *pFrame);

	void AttachToScene(std::string ParentObjectID, MeshFrame* Frame);

	bool CacheEnabled();
	void DisableCache();
	void EnableCache();
};

#endif //TextureManager_H_
