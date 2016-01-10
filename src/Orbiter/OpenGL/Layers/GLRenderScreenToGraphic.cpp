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
#include "GLRenderScreenToGraphic.h"
#include "../Texture/TextureManager.h"

#include "../GLMathUtils.h"

#include "DCE/DCEConfig.h"
#include "DCE/Logger.h"

namespace GLEffect2D 
{

GLRenderScreenToGraphic::GLRenderScreenToGraphic(int Width, int Height)
: RenderGraphic(NULL)
{
	this->Width = Width;
	this->Height = Height;

#ifdef VIA_OVERLAY
	DCEConfig config;
	string sValue = config.m_mapParameters_Find("DisableDummyTextures");

	if(sValue != "1")
		CreateRenderTexture();
#endif
}

GLRenderScreenToGraphic::~GLRenderScreenToGraphic(void)
{
	RELEASE_GRAPHIC(RenderGraphic);
}

OpenGLGraphic* GLRenderScreenToGraphic::GetRenderGraphic()
{
	return RenderGraphic;
}

void GLRenderScreenToGraphic::RenderFrameToGraphic()
{
	if(NULL != RenderGraphic)
	{
		glBindTexture(GL_TEXTURE_2D, RenderGraphic->Texture);
		glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, 
			MathUtils::MinPowerOf2(Width),
			MathUtils::MinPowerOf2(Height),
			0); 
	}
}

void GLRenderScreenToGraphic::CreateRenderTexture()
{
	if(RenderGraphic)
		return;

	int nLocalWidth = 1024;
	int nLocalHeight = 1024;
	
	DCE::LoggerWrapper::GetInstance()->Write(LV_WARNING, "Allocating %d video memory for a dummy texture!", nLocalWidth * nLocalHeight * 4);

	RenderGraphic = new OpenGLGraphic(nLocalWidth, nLocalHeight);
	RenderGraphic->m_Filename = "screen";
	TextureManager::Instance()->PrepareConvert(RenderGraphic);
}

}

