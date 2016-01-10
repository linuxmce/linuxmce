/*
     Copyright (C) 2010 LinuxMCE

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "Picture.h"
#include "DCE/Logger.h"

#include "SDL_rotozoom.h"

#include <exiv2/image.hpp>
#include <exiv2/exif.hpp>
#include <exiv2/iptc.hpp>

using namespace DCE;
using namespace std;

Picture::Picture(string file)
{
        picture = NULL;
        zoomPicture = NULL;
	m_sFile = file;
}

Picture::~Picture()
{
        if (picture != NULL) {
	        SDL_FreeSurface(picture);
	}
        if (zoomPicture != NULL) {
	        SDL_FreeSurface(zoomPicture);
	}
}

bool Picture::Load()
{
        if (picture) {
	        return true;
	}
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Picture.Load() loading image file : %s ", m_sFile.c_str());
        picture = IMG_Load(m_sFile.c_str());
	if (!picture) {
	        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Picture.Load() Unable to load image, error %s", SDL_GetError());
		return false;
	} else {
	        ExifAdjust();
		m_iCenterX = picture->h / 2;
		m_iCenterY = picture->w / 2;
		return true;
	}
}

void Picture::ExifAdjust()
{
	// Check orientation if exiv format
	Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(m_sFile);
	if(image.get() != 0) {
	        image->readMetadata();
		Exiv2::ExifData &exifData = image->exifData();

		if(!exifData.empty()) {
		        Exiv2::ExifData::const_iterator end = exifData.end();
		        for (Exiv2::ExifData::const_iterator i = exifData.begin(); i != end; ++i) {
			        if (i->tag() == 274) {
				        int orientation = i->value().toLong();
					if (orientation > 1) {
					        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Picture::ExifAdjust(): Image needs to be rotated: orientation = %d", orientation);
						double rotAngle = 0;
						// TODO: support other rotation/flip combinations
						if (orientation == 6) {
						  rotAngle = -90;
						} else if (orientation = 8) {
						  rotAngle = 90;
						}
						LoggerWrapper::GetInstance()->Write(LV_STATUS, "Picture::ExifAdjust(): Image rotation = %f",rotAngle);
						
						SDL_Surface *tmpSurface = rotozoomSurface(picture, rotAngle, 1, 1);
						SDL_FreeSurface(picture);
						picture = tmpSurface;

					}
					break;
			        }
			}
		}
        }
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Picture::ExifAdjust(): After rotation w: %d h: %d",picture->w,picture->h);

}
	
int Picture::GetWidth()
{
        return picture->w;
}

int Picture::GetHeight()
{
        return picture->h;
}

SDL_Surface* Picture::GetSurface()
{
        return zoomPicture != NULL ? zoomPicture : picture;
}

void Picture::ZoomToFit(int sw, int sh) {
        float scale = 1.0f;
	if (picture->w > sw) {
	        scale = (float)sw / picture->w;
	}
	if (picture->h * scale > sh) {
	        scale = (float)sh / picture->h;
	}
	Zoom(scale);
	SetViewCenter(zoomPicture->w/2, zoomPicture->h/2);
}

void Picture::Zoom(float scale) {
        m_fViewZoom = scale;
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Picture::Zoom(): zoom = %f", scale);
        zoomPicture = zoomSurface(picture, scale, scale, SMOOTHING_ON);
}

void Picture::SetViewCenter(int x, int y) {
        m_iCenterX = x;
	m_iCenterY = y;
}

void Picture::GetViewRect(SDL_Rect *rect) {
        rect->x = m_iCenterX - rect->w / 2;
	rect->y = m_iCenterY - rect->h / 2;
}

