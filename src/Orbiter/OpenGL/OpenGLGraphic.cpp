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
#include "OpenGLGraphic.h"

#include <SDL_image.h>

#include "GLMathUtils.h"
#include "DCE/Logger.h"
#include "../OrbiterRenderer.h"
#include "../Orbiter.h"
#include "SDL_Helpers/SDL_Defs.h"

#include <iostream>

OpenGLGraphic::OpenGLGraphic() : 
	PlutoGraphic(), 
	m_OpenGlMutex("OpenGLGraphicMutex")
{
	Initialize();
}

OpenGLGraphic::OpenGLGraphic(OrbiterRenderer *pOrbiterRenderer) : 
	PlutoGraphic(pOrbiterRenderer), 
	m_OpenGlMutex("OpenGLGraphicMutex")
{
	Initialize();
}

OpenGLGraphic::OpenGLGraphic(string Filename, eGraphicManagement GraphicManagement, OrbiterRenderer *pOrbiterRenderer) : 
	PlutoGraphic(Filename, GraphicManagement, pOrbiterRenderer), 
	m_OpenGlMutex("OpenGLGraphicMutex")
{
	Initialize();
}
//Warning: That function must run in OpenGL thread
OpenGLGraphic::OpenGLGraphic(int Width, int Height) : 
	PlutoGraphic(), 
	m_OpenGlMutex("OpenGLGraphicMutex")
{
	Initialize();

	int MPWidth = MathUtils::MinPowerOf2(Width);
	int MPHeight = MathUtils::MinPowerOf2(Height);
	MaxU = float(Width) / MPWidth;
	MaxV = float(Height) / MPHeight;
	m_nWidth = Width;
	m_nHeight = Height;

	LocalSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, MPWidth, MPHeight, 
		32, 
		rmask, gmask, bmask, amask);
	Prepare();
}

OpenGLGraphic::OpenGLGraphic(SDL_Surface *pSurface) : PlutoGraphic(), 
	m_OpenGlMutex("OpenGLGraphicMutex"), MaxU(1.0f), MaxV(1.0f)
{
	Initialize();
	LocalSurface = pSurface;

	m_nWidth = pSurface->w;
	m_nHeight = pSurface->h;

	Prepare();
}

OpenGLGraphic::~OpenGLGraphic()
{
	PLUTO_SAFETY_LOCK(oglMutex, m_OpenGlMutex);

	if(NULL != LocalSurface)
	{
		SDL_FreeSurface(LocalSurface);
		LocalSurface = NULL;
	}

	oglMutex.Release();

	TextureManager::Instance()->RemoveFromConvertQueue(this);
	TextureManager::Instance()->PrepareRelease(Texture);	

	delete [] m_pAlphaMask;
	m_pAlphaMask = NULL;

#if defined(DETECT_LEAKS)
	LeaksDetector::Instance().DestroyedGraphicObject();
#endif

//#ifdef DEBUG
//	LoggerWrapper::GetInstance()->Write(LV_STATUS, "OGLGRAPHIC: Destroyed opengl graphic %p", this);
//#endif
}

void OpenGLGraphic::Initialize()
{
	m_pAlphaMask = NULL;

#if defined(DETECT_LEAKS)
	LeaksDetector::Instance().NewGraphicObject();
#endif

	pthread_mutexattr_t m_MutexAttr;
	pthread_mutexattr_init(&m_MutexAttr);
	pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
	m_OpenGlMutex.Init(&m_MutexAttr);
	pthread_mutexattr_destroy(&m_MutexAttr);

	MaxU = 1.0f;
	MaxV = 1.0f;
	LocalSurface = NULL;
	Texture = 0;
	m_nWidth = 0;
	m_nHeight = 0;

//#ifdef DEBUG
//	LoggerWrapper::GetInstance()->Write(LV_STATUS, "OGLGRAPHIC: Created new opengl graphic %p", this);
//#endif
}

bool OpenGLGraphic::SetupFromImage(std::string FileName)
{
	PLUTO_SAFETY_LOCK(oglMutex, m_OpenGlMutex);
	LocalSurface = IMG_Load(FileName.c_str());

	Prepare();
	return true;
}

void OpenGLGraphic::Prepare()
{    
	PLUTO_SAFETY_LOCK(oglMutex, m_OpenGlMutex);
	if(LocalSurface == NULL)
		return;

#if defined(VIA_OVERLAY) || defined(USE_UIWINDOW)
	GenerateAlphaMask();
#endif

	if(TextureManager::Instance()->SupportTextureNonPowerOfTwo())
		return;

	SDL_Surface* Surface = LocalSurface;
	int nLocalWidth = GLMathUtils::MinPowerOf2(Surface->w);
	int nLocalHeight = GLMathUtils::MinPowerOf2(Surface->h);

	if (nLocalWidth == Surface->w && nLocalHeight == Surface->h)
		return;
	
	MaxU = ((float)Surface->w)/nLocalWidth;
	MaxV = ((float)Surface->h)/nLocalHeight;
	
	LocalSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, nLocalWidth, nLocalHeight, 
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

void OpenGLGraphic::Convert()
{
	PLUTO_SAFETY_LOCK(oglMutex, m_OpenGlMutex);

	if( NULL == LocalSurface || NULL == LocalSurface->format)
		return;

	OpenGLTexture NewTexture = 0;

	/* Typical Texture Generation Using Data From The Bitmap */
	glGenTextures( 1, &NewTexture);
	glBindTexture(GL_TEXTURE_2D, NewTexture);

#ifdef VIDEO_RAM_USAGE
	VideoRAMUsageObserver::Instance().ObserveConvertingProcess(
		NewTexture, LocalSurface->w, LocalSurface->h, 
		LocalSurface->format->BytesPerPixel, m_Filename);
#endif

	if(LocalSurface->format->BytesPerPixel == 4)
	{
		// Generate The Texture 
		glTexImage2D( GL_TEXTURE_2D, 
		0, 4, 
		LocalSurface->w, LocalSurface->h, 
		0, GL_RGBA,
		GL_UNSIGNED_BYTE, 
		LocalSurface->pixels );
	}
	else if(LocalSurface->format->BytesPerPixel == 3)
	{
		// Generate The Texture 
		glTexImage2D( GL_TEXTURE_2D, 
			0, 3, 
			LocalSurface->w, LocalSurface->h, 
			0, GL_RGB,
			GL_UNSIGNED_BYTE, 
			LocalSurface->pixels );
	}
	else 
	{
		SDL_PixelFormat RGBAFormat;
		RGBAFormat.palette = 0; RGBAFormat.colorkey = 0; RGBAFormat.alpha = 255;
		RGBAFormat.BitsPerPixel = 24; RGBAFormat.BytesPerPixel = 3;
		RGBAFormat.Rloss = 0; RGBAFormat.Gloss = 0; RGBAFormat.Bloss = 0; RGBAFormat.Aloss = 0; 
		RGBAFormat.Rshift = 0; RGBAFormat.Gshift = 8; RGBAFormat.Bshift = 16; RGBAFormat.Ashift = 24; 
		RGBAFormat.Rmask = 0; RGBAFormat.Gmask = 1 << 8; RGBAFormat.Bmask = 1 << 16; RGBAFormat.Amask = 1 << 24; 

		SDL_Surface *LocalSurfaceConverted = SDL_ConvertSurface(LocalSurface, &RGBAFormat, SDL_SWSURFACE);
		SDL_SetAlpha(LocalSurfaceConverted, 0, 0);

		// Generate The Texture 
		glTexImage2D( GL_TEXTURE_2D, 
			0, LocalSurfaceConverted->format->BytesPerPixel, 
			LocalSurfaceConverted->w, LocalSurface->h, 
			0, GL_RGB,
			GL_UNSIGNED_BYTE, 
			LocalSurfaceConverted->pixels );   

		SDL_FreeSurface(LocalSurface);
		LocalSurface = LocalSurfaceConverted;
	}

#ifdef DEBUG
	DCE::LoggerWrapper::GetInstance()->Write(LV_STATUS, "Freeing surface %p" , LocalSurface);
#endif

	if(m_GraphicManagement != GR_KEEPCOMPRESSED)
	{
		//we'll also keep the SDL surface to reuse; the texture will be release after using it
		SDL_FreeSurface(LocalSurface);
		LocalSurface = NULL;
	}

	/* Linear Filtering */
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	OpenGLTexture OldTexture = Texture;
	Texture = NewTexture;

	if(OldTexture)
	{
#ifdef VIDEO_RAM_USAGE
		VideoRAMUsageObserver::Instance().ObserveReleasingProcess(OldTexture, m_Filename);
#endif

		//release it right away!
		glDeleteTextures(1, &OldTexture);
	}
}

PlutoGraphic* OpenGLGraphic::Clone()
{
	return NULL;
}

GraphicType OpenGLGraphic::GraphicType_get()
{
	return gtOpenGLGraphic;
}

bool OpenGLGraphic::IsEmpty()
{
	return LocalSurface == NULL && Texture == 0;
}

//-----------------------------------------------------------------------------------------------------
Uint32 OpenGLGraphic::getpixel(SDL_Surface *pSDL_Surface,int x, int y)
{
    // all pixels outside the pSDL_Surface are black
    if (x < 0 || x >= pSDL_Surface->w || y < 0 || y >= pSDL_Surface->h)
        return SDL_MapRGB(pSDL_Surface->format, 0, 0, 0);

    int bpp = pSDL_Surface->format->BytesPerPixel;
    Uint8 * pixel = (Uint8 *) pSDL_Surface->pixels + y * pSDL_Surface->pitch + x * bpp;

    switch(bpp)
    {
    case 1:
        return * pixel;

    case 2:
        return * (Uint16 *) pixel;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return pixel[0] << 16 | pixel[1] << 8 | pixel[2];
        else
            return pixel[0] | pixel[1] << 8 | pixel[2] << 16;

    case 4:
        return * (Uint32 *) pixel;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}
//-----------------------------------------------------------------------------------------------------
void OpenGLGraphic::putpixel(SDL_Surface *pSDL_Surface,int x, int y, Uint32 pixel_color)
{
    // don't try to put a pixel outside the pSDL_Surface
    if (x < 0 || x >= pSDL_Surface->w || y < 0 || y >= pSDL_Surface->h)
        return;

    int bpp = pSDL_Surface->format->BytesPerPixel;
    Uint8 * pixel = (Uint8 *) pSDL_Surface->pixels + y * pSDL_Surface->pitch + x * bpp;

    switch(bpp)
    {
    case 1:
        * pixel = pixel_color;
        break;

    case 2:
        * (Uint16 *) pixel = pixel_color;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
        {
            pixel[0] = (pixel_color >> 16) & 0xff;
            pixel[1] = (pixel_color >> 8) & 0xff;
            pixel[2] = pixel_color & 0xff;
        }
        else
        {
            pixel[0] = pixel_color & 0xff;
            pixel[1] = (pixel_color >> 8) & 0xff;
            pixel[2] = (pixel_color >> 16) & 0xff;
        }
        break;

    case 4:
        * (Uint32 *) pixel = pixel_color | 0xFF000000; //opaque
        break;
    }
}

bool OpenGLGraphic::LoadGraphic(char *pData, size_t iSize,int iRotation)
{
	PLUTO_SAFETY_LOCK(oglMutex, m_OpenGlMutex);
	if(LocalSurface != NULL)
	{
		SDL_FreeSurface(LocalSurface);
		LocalSurface = NULL;
	}

	if(m_GraphicFormat == GR_OCG)
	{
		string sErrorMessage = 
			"Cannot load OCG files in Orbiter. "
			"Please uncheck 'Use OCG' device data for this device (" + 
			StringUtils::ltos(m_pOrbiterRenderer->OrbiterLogic()->m_dwPK_Device) + ").";

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, sErrorMessage.c_str());
		m_pOrbiterRenderer->PromptUser(sErrorMessage.c_str(), 100);
		exit(1);
		return false;
	}
	else
	{
		SDL_RWops * rw = SDL_RWFromMem(pData, int(iSize));
		LocalSurface = IMG_Load_RW(rw, 1); // rw is freed here
	}

	if(!LocalSurface)
	{
		return false;
	}

	m_nWidth = LocalSurface->w;
	m_nHeight = LocalSurface->h;
	Prepare();

	return LocalSurface != NULL;
}

void OpenGLGraphic::Clear()
{
	if(Texture)
	{
		TextureManager::Instance()->PrepareRelease(Texture);
		Texture = 0;
	}

	PLUTO_SAFETY_LOCK(oglMutex, m_OpenGlMutex);
	if(LocalSurface != NULL)
	{
		if(m_GraphicManagement != GR_KEEPCOMPRESSED)
		{
			SDL_FreeSurface(LocalSurface);
			LocalSurface = NULL;
		}
	}
}

void OpenGLGraphic::Cleanup()
{
	if(Texture)
	{
		TextureManager::Instance()->PrepareRelease(Texture);
		Texture = 0;
	}

	PLUTO_SAFETY_LOCK(oglMutex, m_OpenGlMutex);
	if(LocalSurface != NULL)
	{
			SDL_FreeSurface(LocalSurface);
			LocalSurface = NULL;
	}
}

bool OpenGLGraphic::GetInMemoryBitmap(char*& pRawBitmapData, size_t& ulSize)
{
	PLUTO_SAFETY_LOCK(oglMutex, m_OpenGlMutex);

	pRawBitmapData = NULL;
	ulSize = 0;
	return false;
}

void OpenGLGraphic::ReplaceColorInRectangle(PlutoRectangle Area, PlutoColor ColorToReplace, PlutoColor ReplacementColor)
{
	PLUTO_SAFETY_LOCK(oglMutex, m_OpenGlMutex);

	if(NULL == LocalSurface)
		return;

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "ReplaceColor: %u %u %u : %u %u %u",
		ColorToReplace.R(), ColorToReplace.G(), ColorToReplace.B(),
		ReplacementColor.R(), ReplacementColor.G(), ReplacementColor.B());
#endif

	int width = Area.Width;
	int height = Area.Height;

	Uint32 Pixel = 0;
	SDL_PixelFormat * PF = LocalSurface->format;
	Uint32 PlutoPixelSrc = (ColorToReplace.R() << PF->Rshift) | (ColorToReplace.G() << PF->Gshift) | (ColorToReplace.B() << PF->Bshift) | (ColorToReplace.A() << PF->Ashift);
	Uint32 PlutoPixelDest = ReplacementColor.R() << PF->Rshift | ReplacementColor.G() << PF->Gshift | ReplacementColor.B() << PF->Bshift;
	unsigned char *Source = (unsigned char *) &PlutoPixelSrc;

	Source[3] = 0;
	for (int j = Area.Y; j < Area.Y + height; j++)
	{
		for (int i = Area.X; i < Area.X + width; i++)
		{
			Pixel = getpixel(LocalSurface,i, j);
			unsigned char *pPixel = (unsigned char *) &Pixel;
			const int max_diff = 3;
			if ( abs(Source[0]-pPixel[0])<max_diff && abs(Source[1]-pPixel[1])<max_diff && abs(Source[2]-pPixel[2])<max_diff /*&& abs(Source[3]-pPixel[3])<max_diff*/ )
			{
				pPixel[3]= 255;
				putpixel(LocalSurface,i, j, PlutoPixelDest);
			}

		}
	}
}

const unsigned char *OpenGLGraphic::GetAlphaMask() const
{
	return m_pAlphaMask;
}

void OpenGLGraphic::GenerateAlphaMask() //TODO
{
	if(NULL != LocalSurface && NULL != LocalSurface->pixels && NULL == m_pAlphaMask)
	{
		m_pAlphaMask = new unsigned char[LocalSurface->w * LocalSurface->h];

		for (int j = 0; j < LocalSurface->h; j++)
		{
			for (int i = 0; i < LocalSurface->w; i++)
			{
				Uint32 Pixel = getpixel(LocalSurface, i, j);
				m_pAlphaMask[i + j * LocalSurface->w] = 0xFF - ((unsigned char *) &Pixel)[3];
			}
		}
	}
}

void OpenGLGraphic::ResetAlphaMask(int nObjectWidth, int nObjectHeight)
{
	delete [] m_pAlphaMask;
	m_pAlphaMask = NULL; 

	if(NULL != LocalSurface && NULL != LocalSurface->pixels && NULL == m_pAlphaMask)
	{
		m_pAlphaMask = new unsigned char[nObjectWidth * nObjectHeight];
		memset(m_pAlphaMask, 0x00, nObjectWidth * nObjectHeight);
	}
}
//-----------------------------------------------------------------------------------------------------
#ifdef DETECT_LEAKS

LeaksDetector LeaksDetector::m_Instance;

LeaksDetector::~LeaksDetector()
{
	FileLogger *pLeaksDetectorLogger = new FileLogger("/var/log/pluto/orbiter_detected_leaks.log");

	pLeaksDetectorLogger->Write(LV_STATUS, "Dumping leaks:");

	pLeaksDetectorLogger->Write(LV_STATUS, "Mesh frames: %d", m_listMeshFrames.size());
	for(list<string>::iterator it = m_listMeshFrames.begin(); it != m_listMeshFrames.end(); ++it)
		pLeaksDetectorLogger->Write(LV_STATUS, "\t- mesh frame: %s", (*it).c_str());

	pLeaksDetectorLogger->Write(LV_STATUS, "Clones: %d", m_listClones.size());
	for(list<string>::iterator it = m_listClones.begin(); it != m_listClones.end(); ++it)
		pLeaksDetectorLogger->Write(LV_STATUS, "\t- clones: %s", (*it).c_str());

	pLeaksDetectorLogger->Write(LV_STATUS, "Graphic objects: %d", m_nGraphicObjectsCounter);

	delete pLeaksDetectorLogger;
	pLeaksDetectorLogger = NULL;
}
void LeaksDetector::NewMeshFrame(string sName)
{
	m_listMeshFrames.push_back(sName);
}
void LeaksDetector::NewClone(string sName)
{
	m_listClones.push_back(sName);
}
void LeaksDetector::NewGraphicObject()
{
	++m_nGraphicObjectsCounter;
}
void LeaksDetector::DestroyedMeshFrame(string sName)
{
	list<string>::iterator it = find(m_listMeshFrames.begin(), m_listMeshFrames.end(), sName);
	if(it != m_listMeshFrames.end())
		m_listMeshFrames.erase(it);
}
void LeaksDetector::DestroyedClone(string sName)
{
	list<string>::iterator it = find(m_listClones.begin(), m_listClones.end(), sName);
	if(it != m_listClones.end())
		m_listClones.erase(it);
}
void LeaksDetector::DestroyedGraphicObject()
{
	--m_nGraphicObjectsCounter;
}

#endif
//-----------------------------------------------------------------------------------------------------
#ifdef VIDEO_RAM_USAGE

VideoRAMUsageObserver VideoRAMUsageObserver::m_Instance; 

void VideoRAMUsageObserver::ObserveConvertingProcess(OpenGLTexture texture, int x, int y, int bpp, string sFileName)
{
	TextureInfoMap::iterator it = mapTexturesInfo.find(texture);

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "VideoRAMUsageObserver: texture %d (%s) allocated with size %d", 
		texture, sFileName.c_str(), x * y * bpp);

	if(it == mapTexturesInfo.end())
	{
		mapTexturesInfo.insert(make_pair(texture, make_pair(sFileName, x * y * bpp)));
	}
	else
	{
		if(sFileName != "dynamic-unknown")
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "VideoRAMUsageObserver: texture %d (%s) allocated before and never released!", 
				texture, sFileName.c_str());

		it->second.second += x * y * bpp;
	}

	DumpVideoMemoryInfo();
}

void VideoRAMUsageObserver::ObserveReleasingProcess(OpenGLTexture texture, string sFileName)
{
	TextureInfoMap::iterator it = mapTexturesInfo.find(texture);

	if(it == mapTexturesInfo.end())
	{
		if(sFileName != "dynamic-unknown")
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "VideoRAMUsageObserver: texture %d (%s) not allocated or already released!", 
				texture, sFileName.c_str());
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "VideoRAMUsageObserver: texture %d (%s) deallocated with size %d", 
			texture, it->second.first.c_str(), it->second.second);

		mapTexturesInfo.erase(it);
	}

	DumpVideoMemoryInfo();
}

void VideoRAMUsageObserver::DumpVideoMemoryInfo()
{
	int nVRAMUsed = 0;

	for(TextureInfoMap::iterator it= mapTexturesInfo.begin(); it != mapTexturesInfo.end(); ++it)
		nVRAMUsed += it->second.second;

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "VideoRAMUsageObserver: Video RAM used by Orbiter: %d bytes (%d textures) !", nVRAMUsed, mapTexturesInfo.size());
}

VideoRAMUsageObserver::~VideoRAMUsageObserver()
{
	FileLogger *pLeaksDetectorLogger = new FileLogger("/var/log/pluto/orbiter_textures_leaks.log");

	pLeaksDetectorLogger->Write(LV_STATUS, "Dumping textures:");

	int nVRAMUsed = 0;
	for(TextureInfoMap::iterator it= mapTexturesInfo.begin(); it != mapTexturesInfo.end(); ++it)
	{
		pLeaksDetectorLogger->Write(LV_STATUS, "\t- texture %d (%s): %d bytes", 
			it->first, it->second.first.c_str(), it->second.second);
		nVRAMUsed += it->second.second;
	}

	pLeaksDetectorLogger->Write(LV_STATUS, "Video RAM used by Orbiter and not released: %d bytes (%d textures) !", nVRAMUsed, mapTexturesInfo.size());

	delete pLeaksDetectorLogger;
	pLeaksDetectorLogger = NULL;
}

#endif
//-----------------------------------------------------------------------------------------------------

