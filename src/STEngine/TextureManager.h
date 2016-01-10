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
typedef GLuint OpenGLTexture; 

class GraphicImage;
class MeshFrame;
class OpenGL3DEngine;

class TextureManager
{
	OpenGLTexture LastTexture;
	TextureManager();
	static TextureManager* Instance_;
	bool TextureEnable_;

	bool m_bSupportTextureNonPowerOfTwo;

public:
	static TextureManager* Instance();
	static void Destroy();
	virtual ~TextureManager(void);
	

	void SetupTexture(OpenGLTexture Texture);
	void CleanUp();

	void RemoveFromConvertQueue(GraphicImage* TextureGraphic);
 
	void SetupNPOTSupport(bool bSupportTextureNonPowerOfTwo);
	bool SupportTextureNonPowerOfTwo();
};

#endif //TextureManager_H_
