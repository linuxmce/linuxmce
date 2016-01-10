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
#ifndef OPENGLGRAPHIC_H_
#define OPENGLGRAPHIC_H_

#include<string>

#include "Texture/TextureManager.h"
#include "../PlutoGraphic.h"
#include "PlutoUtils/MultiThreadIncludes.h"

#include <SDL.h>

class PlutoRectangle;
class PlutoColor;

//#ifdef DEBUG
//	#define DETECT_LEAKS
//	#define VIDEO_RAM_USAGE
//#endif
//-----------------------------------------------------------------------------------------------------
#ifdef DETECT_LEAKS
class LeaksDetector
{
	int m_nGraphicObjectsCounter;

	list<string> m_listMeshFrames;
	list<string> m_listClones;

	LeaksDetector() : m_nGraphicObjectsCounter(0) {}
	~LeaksDetector();
	static LeaksDetector m_Instance;

public:

	static LeaksDetector& Instance() { return m_Instance; }
	void NewMeshFrame(string sName);
	void NewClone(string sName);
	void NewGraphicObject();
	void DestroyedMeshFrame(string sName);
	void DestroyedClone(string sName);
	void DestroyedGraphicObject();
};
#endif
//-----------------------------------------------------------------------------------------------------
#ifdef VIDEO_RAM_USAGE

typedef map<OpenGLTexture, pair<string, int> > TextureInfoMap;

class VideoRAMUsageObserver
{
private:
	TextureInfoMap mapTexturesInfo;
	static VideoRAMUsageObserver m_Instance;

	VideoRAMUsageObserver() {};
	void DumpVideoMemoryInfo();

public:
	
	~VideoRAMUsageObserver();

	static VideoRAMUsageObserver& Instance() { return m_Instance; }
	
	void ObserveConvertingProcess(OpenGLTexture texture, int x, int y, int bpp, string sFileName);
	void ObserveReleasingProcess(OpenGLTexture texture, string sFileName = "dynamic-unknown");
};
#endif
//-----------------------------------------------------------------------------------------------------

class OpenGLGraphic : public PlutoGraphic
{
	/**
	 *	Safeing texture operations
	 */
	pluto_pthread_mutex_t m_OpenGlMutex; 

	Uint32 getpixel(SDL_Surface *pSDL_Surface,int x, int y);
	void putpixel(SDL_Surface *pSDL_Surface,int x, int y, Uint32 pixel_color);
	void GenerateAlphaMask();

	~OpenGLGraphic();

public:
	SDL_Surface* LocalSurface;

	OpenGLTexture Texture;
	float MaxU, MaxV;
	unsigned char *m_pAlphaMask;

	OpenGLGraphic();
	OpenGLGraphic(OrbiterRenderer *pOrbiterRenderer);
	OpenGLGraphic(string Filename, eGraphicManagement GraphicManagement, OrbiterRenderer *pOrbiterRenderer);
	OpenGLGraphic(SDL_Surface *pSurface);
	/**
	 *	Use that only in OpenGL thread
	 */

	OpenGLGraphic(int Width, int Height);

	bool SetupFromImage(std::string FileName);
	void Prepare();
	void Convert();

	void ReplaceColorInRectangle(PlutoRectangle Area, 
		PlutoColor ColorToReplace, 
		PlutoColor ReplacementColor);

	//from PlutoGraphic
	PlutoGraphic* Clone(); 
	void Initialize();
	GraphicType GraphicType_get();
	bool IsEmpty(); 
	bool LoadGraphic(char *pData, size_t iSize,int iRotation=0); 
	void Clear(); 
	void Cleanup();
	PlutoGraphic *GetHighlightedVersion() { return NULL; }
	bool GetInMemoryBitmap(char*& pRawBitmapData, size_t& ulSize);

	const unsigned char *GetAlphaMask() const;
	void ResetAlphaMask(int nObjectWidth, int nObjectHeight);
};

#endif /*OPENGLGRAPHIC_H_*/

