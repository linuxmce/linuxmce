/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include <xine.h>

#include "XineStream.h"
#include "XineOsdObject.h"

XineOsdObject::XineOsdObject()
{
	m_OsdObject = NULL;
	m_OwningStream = NULL;
	m_X = m_Y = m_Width = m_Height = -1;
	m_Valid = false;
}


bool XineOsdObject::Initialize(XineStream *pStream, int x, int y, int width, int height)
{
	if (pStream == NULL)
		return false;
	
	m_OwningStream = pStream;
	m_OsdObject = xine_osd_new(*pStream, x, y, width, height);
	
	if (m_OsdObject == NULL)
		return false;
		
	m_X = x;
	m_Y = y;
	m_Width = width;
	m_Height = height;
	
	m_Valid = true;
	
	return true;
}


bool XineOsdObject::IsValid()
{
	return m_Valid;
}


bool XineOsdObject::SetPalette(uint32_t *colorsComponents, uint8_t *transparencyLevel, int colorsCount, PaletteType paletteType)
{
	if (!m_Valid)
		return false;
		
	uint32_t paletteColors[256];
	uint8_t paletteTransparency[256];
	
	for (int i=0; i<colorsCount; i++)
	{
		paletteColors[i] = colorsComponents[i];
		paletteTransparency[i] = transparencyLevel[i];
	}
	
	for (int i=colorsCount; i<256; i++)
	{
		paletteColors[i] = colorsComponents[i];
		paletteTransparency[i] = transparencyLevel[i];
	}
	
	//adjusting palette
	switch (paletteType)
	{
		// converting RGB palette to YUV, changing order of components: YUV => YVU
		case paletteRGB:
			for (int i=0; i<colorsCount; i++)
			{
				// fetching RGB components
				uint8_t r = (paletteColors[i]>>16)&0xFF;
				uint8_t g = (paletteColors[i]>>8)&0xFF;
				uint8_t b = (paletteColors[i])&0xFF;

				// RGB -> YUV transformation
				
				uint8_t y = (uint8_t) (float) (r*0.299 + g*0.587 + b * 0.114);
				uint8_t u = (uint8_t) (float) (r*-0.169 + g*-0.332 + b * 0.500 + 128);
				uint8_t v = (uint8_t) (float) (r*0.500 + g*-0.419 + b *-0.0813 + 128);
				
				uint32_t color = 0x0;
				color |= y;
				color <<= 8;
				color |= v;
				color <<= 8;
				color |= u;

				paletteColors[i] = color;
			}
			break;
			
		// changing order of components: YUV => YVU
		case paletteYUV:
			for (int i=0; i<colorsCount; i++)
			{
				// fetching YUV components
				uint8_t u = (paletteColors[i]>>8)&0xFF;
				uint8_t v = (paletteColors[i])&0xFF;
				
				paletteColors[i] = (paletteColors[i]) &0xFFFF0000 | v | u;
			}
			break;
			
		default:
			break;
	}
	
	xine_osd_set_palette(m_OsdObject, paletteColors, paletteTransparency);
		
	return true;
}


void XineOsdObject::DrawRect(int x, int y, int width, int height, int color, bool filled)
{
	if (!m_Valid)
		return;
	
	xine_osd_draw_rect(m_OsdObject, x, y, width, height, color, filled);
}

















