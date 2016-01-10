/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#ifndef XineOsdObject_h
#define XineOsdObject_h

#include <xine.h>

class XineStream;

class XineOsdObject
{
	friend class XineStream;

private:
	xine_osd_t *m_OsdObject;
	XineStream *m_OwningStream;
	
	int m_X, m_Y, m_Width, m_Height;
	bool m_Valid;
	
	operator xine_osd_t*()
	{
		return m_OsdObject;
	}

public:
	XineOsdObject();
	
	bool Initialize(XineStream *pStream, int x, int y, int width, int height);
	bool IsValid();
	
	enum PaletteType {paletteRGB, paletteYUV};
	bool SetPalette(uint32_t *colorsComponents, uint8_t *transparencyLevel, int colorsCount, PaletteType paletteType);
	
	void DrawRect(int x, int y, int width, int height, int color, bool filled);
};

#endif
