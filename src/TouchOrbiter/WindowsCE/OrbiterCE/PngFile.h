//------------------------------------------
//  PNGFILE.H -- Header File for pngfile.c
//------------------------------------------

#include <windows.h>
#include "png.h"

HBITMAP PngLoadImageFromRAM (png_byte *PngAddress, unsigned long int length);
