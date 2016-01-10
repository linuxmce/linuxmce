//changed original from bitmap.c@brougs78

#ifdef HAVE_C295
#include <stl.h>
#endif
#include "bitmap.h"

using namespace std; //need ???
using namespace Magick;

cOSDImageBitmap::cOSDImageBitmap() {
}


cOSDImageBitmap::~cOSDImageBitmap() {
}

bool cOSDImageBitmap::LoadZoomed(const char *file, int zoomWidth, int zoomHeight, int zoomLeft, int zoomTop) {
    bool status;
    status = LoadImageMagick(imgkZoom, file);
    if (zoomWidth != 0)
	imgkZoom.crop(Geometry(zoomWidth, zoomHeight, zoomLeft, zoomTop));
    height = imgkZoom.rows();
    width = imgkZoom.columns();
    return status;
}

void cOSDImageBitmap::Render(cBitmap &bmp, int wWindow, int hWindow, int colors, bool dither) {
    int w = wWindow;
    int h = hWindow;
    int wNew, hNew;
    wNew = wWindow;
    hNew = hWindow;
    if (!loadingFailed)	{
	Image imgkRender = imgkZoom;
	width = imgkRender.columns();
	height = imgkRender.rows();
	if (height != h || width != w) {
	    switch (SkinElchiSetup.resize) {
	        case 0:
		    imgkRender.sample(Geometry(wNew, hNew, 0, 0) );
		    break;
	        case 1:
	    	    imgkRender.scale(Geometry(wNew, hNew, 0, 0) );
		    break;
	        case 2:
		    imgkRender.zoom(Geometry(wNew, hNew, 0, 0) );
		}
	    width = imgkRender.columns();
	    height = imgkRender.rows();
	    if (colors == 16 && (height != wWindow || width != hWindow))
		colors = 15;
	    }
	QuantizeImageMagick(imgkRender, colors, dither);
	ConvertImgk2Bmp(bmp, imgkRender, colors);
	}
    else {
	bmp.SetSize(w, h);
	bmp.SetBpp((colors <= 16) ? 4 : 8);
	width = w;
	height = h;
	const cFont *font = cFont::GetFont(fontSml);
	int smalllineHeight = font->Height();
	bmp.DrawRectangle(0, 0, width - 1, height - 1, Theme.Color(clrBackground));
	bmp.DrawText(0, h - smalllineHeight + 1, tr("Error"), Theme.Color(clrButtonRedBg), Theme.Color(clrBackground), font, w, smalllineHeight, taCenter);
	}	
}

bool cOSDImageBitmap::LoadImageMagick(Image &imgkLoad, const char *file) {
    try {
    	imgkLoad.read(file);
	if (imgkLoad.fileSize() == 0) {
	    loadingFailed = true;
	    return false;
	    }
	else {
	    height = imgkLoad.baseRows();
	    width = imgkLoad.baseColumns();
	    origWidth = width;
	    origHeight = height;
	    loadingFailed = false;
	    return true;
	    }
	}
	catch(exception &error)
    {
	    loadingFailed = true;
	    return false;
	}
}

void cOSDImageBitmap::QuantizeImageMagick(Image &imgkQuant, int colors, bool dither) {
	imgkQuant.quantizeColors(colors);
	imgkQuant.quantizeDither(dither);
	imgkQuant.quantize();
}

void cOSDImageBitmap::ConvertImgk2Bmp(cBitmap &bmp, Image &imgkConv, int colors) {
	int w = Width();
	int h = Height();
	tColor col;
	bmp.SetSize(w, h);
	bmp.SetBpp((colors <= 16) ? 4 : 8);
	const PixelPacket *pixels = imgkConv.getConstPixels(0, 0, w, h);
	for (int iy = 0; iy < h; iy++) {
	    for (int ix = 0; ix < w; ix++) {
		col = (0xFF << 24) 
		    | ( (pixels->green * 255 / MaxRGB) << 8) 
		    | ( (pixels->red * 255 / MaxRGB) << 16) 
		    | ( (pixels->blue * 255 / MaxRGB) );
		bmp.DrawPixel(ix, iy, col);
		pixels++;
		}
	    }
}

