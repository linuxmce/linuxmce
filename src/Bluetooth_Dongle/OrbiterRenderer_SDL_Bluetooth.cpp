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
#include "OrbiterRenderer_SDL_Bluetooth.h"
#include "OrbiterBluetooth.h"

#include "../Orbiter/SDL/JpegWrapper.h"
#include "../Orbiter/Orbiter.h"
#include "../Orbiter/ScreenHistory.h"
using namespace DCE;

#include "../pluto_main/Define_Screen.h"

#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_video.h>
#include <SDL_rwops.h>

#include "sge.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "png.h"
//-----------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------
void SaveImageToFile(struct SDL_Surface *pScreenImage, string FileName);
//-----------------------------------------------------------------------------------------------------
OrbiterRenderer_SDL_Bluetooth::OrbiterRenderer_SDL_Bluetooth(Orbiter *pOrbiter) : OrbiterRenderer_SDL(pOrbiter)
{
	m_listLocalRenderedScreens.push_back( SCREEN_Main_CONST );
	m_listLocalRenderedScreens.push_back( SCREEN_Lights_CONST );
	m_listLocalRenderedScreens.push_back( SCREEN_Media_CONST );
	m_listLocalRenderedScreens.push_back( SCREEN_Climate_CONST );
	m_listLocalRenderedScreens.push_back( SCREEN_Security_CONST );
	m_listLocalRenderedScreens.push_back( SCREEN_Telephony_CONST );	
	m_listLocalRenderedScreens.push_back( SCREEN_CurrentLocation_CONST );		
}
//-----------------------------------------------------------------------------------------------------
OrbiterRenderer_SDL_Bluetooth::~OrbiterRenderer_SDL_Bluetooth()
{
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_SDL_Bluetooth::DisplayImageOnScreen(SDL_Surface *pScreenImage)
{	
	OrbiterBluetooth *pOrbiterBluetooth = dynamic_cast<OrbiterBluetooth *>(OrbiterLogic());

	if(NULL != pOrbiterBluetooth)
	{
		int nPK_Screen = 0;
		if(NULL != pOrbiterBluetooth->m_pScreenHistory_Current)
			nPK_Screen = pOrbiterBluetooth->m_pScreenHistory_Current->PK_Screen();

		if(
			(
				pOrbiterBluetooth->GetPhoneConfig().GetMenuMode()==PhoneConfig::mmMenu &&  
				std::find(m_listLocalRenderedScreens.begin(), m_listLocalRenderedScreens.end(), nPK_Screen) ==
					m_listLocalRenderedScreens.end()
			)
			||
			pOrbiterBluetooth->GetPhoneConfig().GetMenuMode()==PhoneConfig::mmImage 
		) 
		{
			const string csTempFileName = "TmpScreen.png";
			//generate the jpeg or png image with current screen
			if(pOrbiterBluetooth->ImageQuality() == 100) //we'll use pngs for best quality
				SaveImageToFile(pScreenImage, csTempFileName);
			else
				SDL_SaveJPG(pScreenImage, csTempFileName.c_str(), pOrbiterBluetooth->ImageQuality());

			pOrbiterBluetooth->ImageGenerated(csTempFileName);
		}
		else 
		{
			//not sending image...

			//Sending ShowMenu command			
			if ( SCREEN_Main_CONST==nPK_Screen && 
				 pOrbiterBluetooth->GetPhoneConfig().GetMenuMode()==PhoneConfig::mmMenu ) {
				 pOrbiterBluetooth->ShowMenu( );
			}
		}

	}
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_SDL_Bluetooth::BeginPaint()
{
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_SDL_Bluetooth::EndPaint()
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