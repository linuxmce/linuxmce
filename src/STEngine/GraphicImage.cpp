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

#include <GL/glu.h>
#include "GraphicImage.h"
#include "SDL_rotozoom.h"
#include "SDL_image.h"
#include "MathUtils.h"

#include "TextureManager.h"

#include "DCE/Logger.h"

#include <exiv2/image.hpp>
#include <exiv2/exif.hpp>
#include <exiv2/iptc.hpp>

using namespace DCE;

GraphicImage::GraphicImage(int nMaxSize)
: LocalSurface(NULL), Texture(0)
{
	m_nMaxSize = nMaxSize;
}
GraphicImage::~GraphicImage()
{
	if(LocalSurface)
		SDL_FreeSurface(LocalSurface);
	ReleaseTexture();
}

void GraphicImage::adjustImageZoomAndOrientation(string FileName) {
	// Check orientation if jpeg
	Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(FileName);
	if(image.get() != 0) {
	        image->readMetadata();
		Exiv2::ExifData &exifData = image->exifData();

		if(!exifData.empty()) {
		        Exiv2::ExifData::const_iterator end = exifData.end();
		        for (Exiv2::ExifData::const_iterator i = exifData.begin(); i != end; ++i) {
			        if (i->tag() == 274) {
				        int orientation = i->value().toLong();
					if (orientation > 1) {
					        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Image needs to be rotated: orientation = %d", orientation);
						double rotAngle = 0;
						// TODO: support other rotation/flip combinations
						if (orientation == 6) {
						  rotAngle = -90;
						} else if (orientation = 8) {
						  rotAngle = 90;
						}
						LoggerWrapper::GetInstance()->Write(LV_STATUS, "Image rotation = %f",rotAngle);
						SDL_Surface *pSDL_Surface = LocalSurface;
						
						LocalSurface = rotozoomSurface(pSDL_Surface, rotAngle, 1, 1);
						SDL_FreeSurface(pSDL_Surface);

					}
					break;
			        }
			}
		}
        }
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "After rotation w: %d h: %d",LocalSurface->w,LocalSurface->h);

	if(m_nMaxSize && (LocalSurface->w > m_nMaxSize || LocalSurface->h > m_nMaxSize)) 
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Pictures too big %s, downscaling it", FileName.c_str());

		SDL_Surface *pSDL_Surface = LocalSurface;

		//zoom
		double ZoomX = 1;
		double ZoomY = 1;

		SDL_Surface *rotozoom_picture;

		ZoomX = ZoomY = min(m_nMaxSize / double(pSDL_Surface->w),
			m_nMaxSize / double(pSDL_Surface->h));

		LocalSurface = zoomSurface(pSDL_Surface, ZoomX, ZoomY, SMOOTHING_ON);
		SDL_FreeSurface(pSDL_Surface);
	}

}

bool GraphicImage::Load(string FileName)
{
	LocalSurface = IMG_Load(FileName.c_str()); 
	if( LocalSurface )
	{
	        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Loaded %s",FileName.c_str());
	        adjustImageZoomAndOrientation(FileName);
	}
	else
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Failed to load '%s'", FileName.c_str());

	return NULL != LocalSurface;
}

void GraphicImage::Prepare(int nScreenWidth, int nScreenHeight)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "NPOT textures %ssupported", 
		TextureManager::Instance()->SupportTextureNonPowerOfTwo() ? "" : "NOT ");

	if(TextureManager::Instance()->SupportTextureNonPowerOfTwo())
	{
		MaxU = 1;
		MaxV = 1;
	}
	else
		ScaleImage(nScreenWidth, nScreenHeight);

	Convert();
}

static int textureCount = 0;
void GraphicImage::Convert(void)
{
	if(LocalSurface == NULL)
		return;
	ReleaseTexture();

	/* Typical Texture Generation Using Data From The Bitmap */
	glGenTextures( 1, &Texture);
	glBindTexture(GL_TEXTURE_2D, Texture);

	static const int MAPMAP_THRESHOLD = 2048;
	bool bUseMipmap = ( ( LocalSurface->w > MAPMAP_THRESHOLD ) ||
			    ( LocalSurface->h > MAPMAP_THRESHOLD ) );

	if (bUseMipmap)
	{
		// Generate The Texture 
		gluBuild2DMipmaps(
			GL_TEXTURE_2D, 3, LocalSurface->w, LocalSurface->h,
			(LocalSurface->format->BytesPerPixel == 4) ? GL_RGBA : GL_RGB,
			GL_UNSIGNED_BYTE, LocalSurface->pixels );
		// when minifying, bilinear filter the closest mipmap
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
		// when magnifiying, use linear filter
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	}
	else
	{
		// Generate The Texture 
		glTexImage2D( GL_TEXTURE_2D, 
			0, 4, 
			LocalSurface->w, LocalSurface->h, 
			0, (LocalSurface->format->BytesPerPixel == 4) ? GL_RGBA : GL_RGB,
			GL_UNSIGNED_BYTE, 
			LocalSurface->pixels );

		// always use linear minification/magnification
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ); 
	}

	SDL_FreeSurface(LocalSurface);
	LocalSurface = NULL;

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Allocated texture size %d cnt:%d", Width * Height * 4, ++textureCount);
}

void GraphicImage::ReleaseTexture(void)
{
	if(Texture)
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Releasing texture size %d", Width * Height * 4);

		glGetError(); //flush error queue
		glDeleteTextures(1, &Texture);
		int err = glGetError();
		if(err)
		{
                    if(GL_INVALID_VALUE == err)
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Failed to release texture: Invalid Value");
                    if(GL_INVALID_OPERATION == err)
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Failed to release texture: Invalid Operation");
		}
		else
                {
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Released texture successfully cnt:%d", --textureCount);
			Texture = 0;
                }
	}
}

void GraphicImage::ScaleImage(int nScreenWidth, int nScreenHeight)
{
	if(LocalSurface == NULL)
		return;

	SDL_Surface* Surface = LocalSurface;
	Width = MinPowerOf2(Surface->w);
	Height = MinPowerOf2(Surface->h);

	//TODO: use screen dim. to create a surface with optimal size

	if (Width == Surface->w && Height == Surface->h)
		return;

	// Max values tell us how much of the new surface holds an image
	MaxU = ((float)Surface->w)/Width;
	MaxV = ((float)Surface->h)/Height;

	/* Create a 32-bit surface with the bytes of each pixel in R,G,B,A order,
	as expected by OpenGL for textures */

	LocalSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, Width, Height, 
		Surface->format->BitsPerPixel, 
		Surface->format->Rmask, Surface->format->Gmask, Surface->format->Bmask,
		Surface->format->Amask);

	if(LocalSurface == NULL) {
		fprintf(stderr, "CreateRGBSurface failed: %s", SDL_GetError());
		return;
	}

	Uint32 saved_flags = Surface->flags&(SDL_SRCALPHA|SDL_RLEACCELOK);
	Uint8  saved_alpha = Surface->format->alpha;

	if ((saved_flags & SDL_SRCALPHA) == SDL_SRCALPHA) 
		SDL_SetAlpha(Surface, 0, 0);

	SDL_BlitSurface(Surface, NULL, LocalSurface, NULL);

	if ((saved_flags & SDL_SRCALPHA) == SDL_SRCALPHA) 
		SDL_SetAlpha(LocalSurface, saved_flags, saved_alpha);

	SDL_FreeSurface(Surface);  
}

bool GraphicImage::SupportTextureNonPowerOfTwo()
{
	return TextureManager::Instance()->SupportTextureNonPowerOfTwo();
}

