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
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
/*---------------------------------------------------------------------------*\
 *
 * (c) Copyright Microsoft Corp. 1997-98 All Rights Reserved
 *
 *  module: imgdecmp.h
 *  date:
 *  author: jaym
 *
 *  purpose: 
 *
\*---------------------------------------------------------------------------*/
#ifndef __IMGDECMP_H__
#define __IMGDECMP_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "imgrendr.h"

typedef void (CALLBACK *PROGRESSFUNC)(IImageRender *pRender, BOOL bComplete, LPARAM lParam);
typedef DWORD (CALLBACK *GETDATAFUNC)(LPSTR szBuffer, DWORD dwBufferMax, LPARAM lParam);

typedef struct tagDecompressImageInfo {
	DWORD                   dwSize;                                 // Size of this structure
	LPBYTE                  pbBuffer;                               // Pointer to the buffer to use for data
	DWORD                   dwBufferMax;                    // Size of the buffer
	DWORD                   dwBufferCurrent;                // The amount of data which is current in the buffer
	HBITMAP *               phBM;                                   // Pointer to the bitmap returned (can be NULL)
	IImageRender ** ppImageRender;                  // Pointer to an IImageRender object (can be NULL)
	int                             iBitDepth;                              // Bit depth of the output image
	LPARAM                  lParam;                                 // User parameter for callback functions
	HDC                             hdc;                                    // HDC to use for retrieving palettes
	int                             iScale;                                 // Scale factor (1 - 100)
	int                             iMaxWidth;                              // Maximum width of the output image
	int                             iMaxHeight;                             // Maxumum height of the output image
	GETDATAFUNC             pfnGetData;                             // Callback function to get more data
	PROGRESSFUNC    pfnImageProgress;               // Callback function to notify caller of progress decoding the image
	COLORREF                crTransparentOverride;  // If this color is not (UINT)-1, it will override the
											// transparent color in the image with this color. (GIF ONLY)
} DecompressImageInfo;

#define IMGDECOMP_E_NOIMAGE             0x800b0100

COLORREF *
GetHalftonePalette();

COLORREF *
Get332Palette();

HRESULT
DecompressImageIndirect(DecompressImageInfo *pParams);

#ifdef __cplusplus
};
#endif // __cplusplus

#endif // !__IMGDECMP_H__
