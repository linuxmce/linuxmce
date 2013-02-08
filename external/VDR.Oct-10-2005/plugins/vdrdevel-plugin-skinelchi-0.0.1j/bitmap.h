//changed original from bitmap.h@brougs78

#ifndef _OSDIMAGE_BITMAP_H_
#define _OSDIMAGE_BITMAP_H_

#define X_DISPLAY_MISSING

#include "setup.h"
#include <vdr/osd.h>
#include <vdr/skins.h>
#include <Magick++.h>

using namespace Magick;

class cOSDImageBitmap {
public:
    cOSDImageBitmap();
    ~cOSDImageBitmap();
    bool LoadZoomed(const char *file, int zoomWidth, int zoomHeight, int zoomLeft, int zoomTop);
    void Render(cBitmap &bmp, int wWindow, int hWindow, int colors, bool dither);
    inline int Width() { return width; }
    inline int Height() { return height; }

private:	
    bool LoadImageMagick(Image &imgkLoad, const char *file);
    void QuantizeImageMagick(Image &imgkQuant, int colors, bool dither);
    void ConvertImgk2Bmp(cBitmap &bmp, Image &imgkConv, int colors);
    Image imgkZoom;
    int ZoomWidth, ZoomHeight, ZoomLeft, ZoomTop;
    int origWidth, origHeight;
    bool loadingFailed;
    int width, height;
};

#endif
