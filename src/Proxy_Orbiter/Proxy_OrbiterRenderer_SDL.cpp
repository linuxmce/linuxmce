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
#include "Proxy_OrbiterRenderer_SDL.h"
#include "Proxy_Orbiter.h"

#include "../Orbiter/SDL/JpegWrapper.h"
#include "../Orbiter/Orbiter.h"
#include "../Orbiter/ScreenHistory.h"
using namespace DCE;

#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_video.h>
#include <SDL_rwops.h>

#include "sge.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "png.h"
#include "zlib.h"
//-----------------------------------------------------------------------------------------------------
void SaveImageToFile(struct SDL_Surface *pScreenImage, string FileName);
//-----------------------------------------------------------------------------------------------------
Proxy_OrbiterRenderer_SDL::Proxy_OrbiterRenderer_SDL(Orbiter *pOrbiter) : OrbiterRenderer_SDL(pOrbiter)
{

}
//-----------------------------------------------------------------------------------------------------
Proxy_OrbiterRenderer_SDL::~Proxy_OrbiterRenderer_SDL()
{
}
//-----------------------------------------------------------------------------------------------------
void Proxy_OrbiterRenderer_SDL::DisplayImageOnScreen(SDL_Surface *pScreenImage)
{
	PLUTO_SAFETY_LOCK(sm,OrbiterLogic()->m_ScreenMutex);

	if(NULL != OrbiterLogic()->m_pScreenHistory_Current)
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Proxy_Orbiter::DisplayImageOnScreen Current screen: %s",  
			OrbiterLogic()->m_pScreenHistory_Current->GetObj()->m_ObjectID.c_str());
	}

	Proxy_Orbiter *pProxy_Orbiter = dynamic_cast<Proxy_Orbiter *>(OrbiterLogic());

	if(pScreenImage->w <= 320 && pScreenImage->h <= 240) //ip phone
		pProxy_Orbiter->ImageQuality(100);

	if(NULL != pProxy_Orbiter)
	{
		string sDevicePng = pProxy_Orbiter->GetDevicePngFileName();
		string sDeviceXml = pProxy_Orbiter->GetDeviceXmlFileName();

		PLUTO_SAFETY_LOCK(rm, pProxy_Orbiter->m_ResourcesMutex);
		//generate the jpeg or png image with current screen
		if(pProxy_Orbiter->ImageQuality() == 100) //we'll use pngs for best quality
			SaveImageToFile(pScreenImage, sDevicePng);
		else
			SDL_SaveJPG(pScreenImage, sDevicePng.c_str(), pProxy_Orbiter->ImageQuality());

		pProxy_Orbiter->SaveXML(sDeviceXml);
		pProxy_Orbiter->SaveBits();
		rm.Release();

		pProxy_Orbiter->ImageGenerated();
	}
}
//-----------------------------------------------------------------------------------------------------
void Proxy_OrbiterRenderer_SDL::BeginPaint()
{
	Proxy_Orbiter *pProxy_Orbiter = dynamic_cast<Proxy_Orbiter *>(OrbiterLogic());
	pProxy_Orbiter->m_dequeCellXMLItems.clear();
}
//-----------------------------------------------------------------------------------------------------
void Proxy_OrbiterRenderer_SDL::EndPaint()
{
	DisplayImageOnScreen(m_pScreenImage);
}
//-----------------------------------------------------------------------------------------------------
void SaveImageToFile(struct SDL_Surface *pScreenImage, string FileName)
{
	SDL_Surface * Drawing = pScreenImage;

	png_bytepp image_rows;
	FILE * File;
	int BitsPerColor;

	File = fopen(FileName.c_str(), "wb");

	image_rows = new png_bytep[Drawing->h];
	for (int n = 0; n < Drawing->h; n++)
		image_rows[n] = (unsigned char *) Drawing->pixels + n * Drawing->pitch;

	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	png_infop png_info = png_create_info_struct(png_ptr);

	png_init_io(png_ptr, File);
	png_set_filter(png_ptr, 0, PNG_FILTER_NONE);
	png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);

	BitsPerColor = Drawing->format->BitsPerPixel / Drawing->format->BytesPerPixel;
	png_set_IHDR(png_ptr, png_info, Drawing->w, Drawing->h, BitsPerColor, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, png_info);
	png_write_image(png_ptr, image_rows);
	png_write_end(png_ptr, png_info);

	delete [] image_rows;

	png_destroy_write_struct(&png_ptr, &png_info);
	fclose(File);
}
//-----------------------------------------------------------------------------------------------------
void Proxy_OrbiterRenderer_SDL::RefreshScreen(void *data)
{
	Proxy_Orbiter *pProxy_Orbiter = dynamic_cast<Proxy_Orbiter *>(OrbiterLogic());
	pProxy_Orbiter->m_dequeCellXMLItems.clear();
	//pProxy_Orbiter->m_bRerenderScreen = true; //force full redraw

	OrbiterRenderer::RefreshScreen(data);
}
//-----------------------------------------------------------------------------------------------------
void Proxy_OrbiterRenderer_SDL::RenderScreen(bool bRenderGraphicsOnly)
{
	Proxy_Orbiter *pProxy_Orbiter = dynamic_cast<Proxy_Orbiter *>(OrbiterLogic());

	if(!bRenderGraphicsOnly)
		pProxy_Orbiter->m_dequeCellXMLItems.clear();

	return OrbiterRenderer_SDL::RenderScreen(bRenderGraphicsOnly);
}
//-----------------------------------------------------------------------------------------------------
bool Proxy_OrbiterRenderer_SDL::DisplayProgress(string, int)
{
	return false;
}
//-----------------------------------------------------------------------------------------------------
bool Proxy_OrbiterRenderer_SDL::DisplayProgress(string, const map<string, bool> &, int)
{
	return false;
}
//-----------------------------------------------------------------------------------------------------
int Proxy_OrbiterRenderer_SDL::PromptUser(string, int, map<int,string> *)
{
	return 0;
}
//-----------------------------------------------------------------------------------------------------
int Proxy_OrbiterRenderer_SDL::PromptFor(string)
{
	return 0;
}
//-----------------------------------------------------------------------------------------------------
