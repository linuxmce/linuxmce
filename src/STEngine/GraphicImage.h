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
#ifndef GraphicImage_H_
#define GraphicImage_H_

#include <SDL.h>
#ifdef WIN32
#define LEAN_AND_MEAN
#include <windows.h>
#endif
#include <GL/gl.h>
#include <string>

using namespace std;

class GraphicImage
{
	SDL_Surface *LocalSurface;
    
	bool CheckExtension(const char* checkFor);
	bool SupportTextureNonPowerOfTwo();
	void Convert(void);
	void adjustImageZoomAndOrientation(string FileName);

public:
	GraphicImage(int nMaxSize);
	~GraphicImage(void);
	bool Load(string FileName);
	void Prepare(int nScreenWidth, int nScreenHeight);

public:
	GLuint Texture;
	float MaxU;
	float MaxV;
	int Width;
	int Height;
	int m_nMaxSize;
public:
	void ReleaseTexture(void);
	void ScaleImage(int nScreenWidth, int nScreenHeight);
};

#endif
