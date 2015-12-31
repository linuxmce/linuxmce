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
#include "TextureManager.h"
#include "ExtensionManager.h"
#include "Frame.h"

#include <GL/gl.h>

#include "GraphicImage.h"

/*static*/ TextureManager* TextureManager::Instance_ = NULL;

TextureManager* TextureManager::Instance() 
{
	if(Instance_ == NULL)
		Instance_ = new TextureManager();
	return Instance_;
}

void TextureManager::Destroy()
{
	delete Instance_;
	Instance_ = NULL;
}

TextureManager::TextureManager(void) 
	: LastTexture(0), 
	m_bSupportTextureNonPowerOfTwo(false)
{
//	glEnable(GL_TEXTURE_2D);
}

TextureManager::~TextureManager(void)
{
}

/*static*/ void TextureManager::SetupTexture(OpenGLTexture Texture)
{
	//if(Texture == LastTexture)
	//	return;

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
			//glDisable(GL_TEXTURE_2D);
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

void TextureManager::SetupNPOTSupport(bool bSupportTextureNonPowerOfTwo)
{
	m_bSupportTextureNonPowerOfTwo = bSupportTextureNonPowerOfTwo;
}

bool TextureManager::SupportTextureNonPowerOfTwo()
{
	return m_bSupportTextureNonPowerOfTwo;
}
