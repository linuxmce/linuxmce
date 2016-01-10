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
#include "PFGHelper.h"
#include "../RendererOCG.h"
#include "PlutoUtils/PlutoDefs.h"
#include "PlutoUtils/FileUtils.h"
#include "src/internal/graphicbuffer.h"
#include "Orbiter.h"

//----------------------------------------------------------------------------------------------------------------
Surface* PocketFrog_LoadPFG(DisplayDevice *pDisplayDevice, char *pOCGData, size_t iOCGDataSize)
{
	RendererOCG *pRendererOCG = new RendererOCG();
	pRendererOCG->SetOCGData(pOCGData, (int)iOCGDataSize);
	Surface *pSurface = NULL;

	char *pPixelsData;
	int iPixelsDataSize;
	char *pPixelFormatData;	
	int iPixelFormatDataSize;
	int iWidth;
	int iHeigth;

	if(pRendererOCG->GetSurface(pPixelsData, iPixelsDataSize, pPixelFormatData, iPixelFormatDataSize, iWidth, iHeigth))
	{
		if(iPixelsDataSize)
		{
			pSurface = pDisplayDevice->CreateSurface(iWidth, iHeigth);

			Pixel * pPixels = pSurface->m_buffer->GetPixels();
			memcpy((char *)pPixels, pPixelsData, iPixelsDataSize);
		}
	}

	PLUTO_SAFE_DELETE(pRendererOCG);
	return pSurface; 
}
//----------------------------------------------------------------------------------------------------------------
Surface* PocketFrog_LoadPFG(DisplayDevice *pDisplayDevice, string sFilename)
{
	size_t iSize;
	char *pData = FileUtils::ReadFileIntoBuffer(sFilename, iSize);

	if(!pData)
		return NULL;

	Surface *pSurface = PocketFrog_LoadPFG(pDisplayDevice, pData, iSize);

	PLUTO_SAFE_DELETE_ARRAY(pData);
	return pSurface;
}
//----------------------------------------------------------------------------------------------------------------
bool PocketFrog_SavePFG(Surface *pSurface, string sFilename)
{
	int iPixelsDataSize = pSurface->m_height * pSurface->m_buffer->GetPitch();//pSurface.pitch;
	char *pPixelsData = new char[iPixelsDataSize];
	char *pPixels = (char *)pSurface->m_buffer->GetPixels();
	memcpy(pPixelsData, pPixels, iPixelsDataSize);

	int iPixelFormatDataSize = 4;
	char dummy[] = {0, 0, 0, 0};
	char *pPixelFormatData = new char[iPixelFormatDataSize];
	memcpy(pPixelFormatData, dummy, iPixelFormatDataSize);

	RendererOCG *pRendererOCG = new RendererOCG(pPixelsData, iPixelsDataSize, pPixelFormatData, iPixelFormatDataSize, 
		pSurface->m_width, pSurface->m_height);

	bool bResult = pRendererOCG->SaveOCG(sFilename);
	PLUTO_SAFE_DELETE(pRendererOCG);	

	return bResult;
}
//----------------------------------------------------------------------------------------------------------------