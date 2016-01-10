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
//  dib.cpp
//

#include "stdafx.h"
#include "dib.h"
#include <windowsx.h>
#include <afxadv.h>
#include <io.h>
#include <errno.h>

/////////////////////////////////////////////////////////////////////////////
// CDib

IMPLEMENT_DYNAMIC(CDib, CObject)

CDib::CDib()
{
	m_pBMI = NULL;
	m_pBits = NULL;
	m_pPalette = NULL;
}

CDib::~CDib()
{
	Free();
}

void CDib::Free()
{
	// Make sure all member data that might have been allocated is freed.
	if (m_pBits)
	{
		GlobalFreePtr(m_pBits);
		m_pBits = NULL;
	}
	if (m_pBMI)
	{
		GlobalFreePtr(m_pBMI);
		m_pBMI = NULL;
	}
	if (m_pPalette)
	{
		m_pPalette->DeleteObject();
		delete m_pPalette;
		m_pPalette = NULL;
	}
}

BOOL CDib::Paint(HDC hDC, LPRECT lpDCRect, LPRECT lpDIBRect) const
{
	if (!m_pBMI)
		return FALSE;

	HPALETTE hPal = NULL;           // Our DIB's palette
	HPALETTE hOldPal = NULL;        // Previous palette

	// Get the DIB's palette, then select it into DC
	if (m_pPalette != NULL)
	{
		hPal = (HPALETTE) m_pPalette->m_hObject;

		// Select as background since we have
		// already realized in forground if needed
		hOldPal = ::SelectPalette(hDC, hPal, TRUE);
	}

	/* Make sure to use the stretching mode best for color pictures */
	::SetStretchBltMode(hDC, COLORONCOLOR);

	/* Determine whether to call StretchDIBits() or SetDIBitsToDevice() */
	BOOL bSuccess;
	if ((RECTWIDTH(lpDCRect)  == RECTWIDTH(lpDIBRect)) &&
	   (RECTHEIGHT(lpDCRect) == RECTHEIGHT(lpDIBRect)))
		bSuccess = ::SetDIBitsToDevice(hDC,        // hDC
								   lpDCRect->left,             // DestX
								   lpDCRect->top,              // DestY
								   RECTWIDTH(lpDCRect),        // nDestWidth
								   RECTHEIGHT(lpDCRect),       // nDestHeight
								   lpDIBRect->left,            // SrcX
								   (int)Height() -
									  lpDIBRect->top -
									  RECTHEIGHT(lpDIBRect),     // SrcY
								   0,                          // nStartScan
								   (WORD)Height(),             // nNumScans
								   m_pBits,                    // lpBits
								   m_pBMI,                     // lpBitsInfo
								   DIB_RGB_COLORS);            // wUsage
   else
	  bSuccess = ::StretchDIBits(hDC,            // hDC
							   lpDCRect->left,               // DestX
							   lpDCRect->top,                // DestY
							   RECTWIDTH(lpDCRect),          // nDestWidth
							   RECTHEIGHT(lpDCRect),         // nDestHeight
							   lpDIBRect->left,              // SrcX
							   lpDIBRect->top,               // SrcY
							   RECTWIDTH(lpDIBRect),         // wSrcWidth
							   RECTHEIGHT(lpDIBRect),        // wSrcHeight
							   m_pBits,                      // lpBits
							   m_pBMI,                       // lpBitsInfo
							   DIB_RGB_COLORS,               // wUsage
							   SRCCOPY);                     // dwROP

	/* Reselect old palette */
	if (hOldPal != NULL)
	{
		::SelectPalette(hDC, hOldPal, TRUE);
	}

   return bSuccess;
}


BOOL CDib::CreatePalette()
{
	if (!m_pBMI)
		return FALSE;

   //get the number of colors in the DIB
   WORD wNumColors = NumColors();

   if (wNumColors != 0)
   {
		// allocate memory block for logical palette
		HANDLE hLogPal = ::GlobalAlloc(GHND, sizeof(LOGPALETTE) + sizeof(PALETTEENTRY)*wNumColors);

		// if not enough memory, clean up and return NULL
		if (hLogPal == 0)
			return FALSE;

		LPLOGPALETTE lpPal = (LPLOGPALETTE)::GlobalLock((HGLOBAL)hLogPal);

		// set version and number of palette entries
		lpPal->palVersion = PALVERSION;
		lpPal->palNumEntries = (WORD)wNumColors;

		for (int i = 0; i < (int)wNumColors; i++)
		{
			lpPal->palPalEntry[i].peRed = m_pBMI->bmiColors[i].rgbRed;
			lpPal->palPalEntry[i].peGreen = m_pBMI->bmiColors[i].rgbGreen;
			lpPal->palPalEntry[i].peBlue = m_pBMI->bmiColors[i].rgbBlue;
			lpPal->palPalEntry[i].peFlags = 0;
		}

		/* create the palette and get handle to it */
		if (m_pPalette)
		{
			m_pPalette->DeleteObject();
			delete m_pPalette;
		}
		m_pPalette = new CPalette;
		BOOL bResult = m_pPalette->CreatePalette(lpPal);
		::GlobalUnlock((HGLOBAL) hLogPal);
		::GlobalFree((HGLOBAL) hLogPal);
		return bResult;
	}

	return TRUE;
}


DWORD CDib::Width() const
{
	if (!m_pBMI)
		return 0;

	/* return the DIB width */
	return m_pBMI->bmiHeader.biWidth;
}



DWORD CDib::Height() const
{
	if (!m_pBMI)
		return 0;
	
	/* return the DIB height */
	return m_pBMI->bmiHeader.biHeight;
}



WORD CDib::PaletteSize() const
{
	if (!m_pBMI)
		return 0;

	return NumColors() * sizeof(RGBQUAD);
}

WORD CDib::NumColors() const
{
	if (!m_pBMI)
		return 0;

	WORD wBitCount;  // DIB bit count

	/*  The number of colors in the color table can be less than 
	 *  the number of bits per pixel allows for (i.e. lpbi->biClrUsed
	 *  can be set to some value).
	 *  If this is the case, return the appropriate value.
	 */

	DWORD dwClrUsed;

	dwClrUsed = m_pBMI->bmiHeader.biClrUsed;
	if (dwClrUsed != 0)
		return (WORD)dwClrUsed;

	/*  Calculate the number of colors in the color table based on
	 *  the number of bits per pixel for the DIB.
	 */
	wBitCount = m_pBMI->bmiHeader.biBitCount;

	/* return number of colors based on bits per pixel */
	switch (wBitCount)
	{
		case 1:
			return 2;

		case 4:
			return 16;

		case 8:
			return 256;

		default:
			return 0;
	}
}


DWORD CDib::Save(CFile& file) const
{
	BITMAPFILEHEADER bmfHdr; // Header for Bitmap file
	DWORD dwDIBSize;

	if (m_pBMI == NULL)
		return 0;

	// Fill in the fields of the file header

	// Fill in file type (first 2 bytes must be "BM" for a bitmap)
	bmfHdr.bfType = DIB_HEADER_MARKER;  // "BM"

	// First, find size of header plus size of color table.  Since the
	// first DWORD in both BITMAPINFOHEADER and BITMAPCOREHEADER conains
	// the size of the structure, let's use this.
	dwDIBSize = *(LPDWORD)&m_pBMI->bmiHeader + PaletteSize();  // Partial Calculation

	// Now calculate the size of the image
	if ((m_pBMI->bmiHeader.biCompression == BI_RLE8) || (m_pBMI->bmiHeader.biCompression == BI_RLE4))
	{
		// It's an RLE bitmap, we can't calculate size, so trust the
		// biSizeImage field
		dwDIBSize += m_pBMI->bmiHeader.biSizeImage;
	}
	else
	{
		DWORD dwBmBitsSize;  // Size of Bitmap Bits only

		// It's not RLE, so size is Width (DWORD aligned) * Height
		dwBmBitsSize = WIDTHBYTES((m_pBMI->bmiHeader.biWidth)*((DWORD)m_pBMI->bmiHeader.biBitCount)) * m_pBMI->bmiHeader.biHeight;
		dwDIBSize += dwBmBitsSize;

		// Now, since we have calculated the correct size, why don't we
		// fill in the biSizeImage field (this will fix any .BMP files which
		// have this field incorrect).
		m_pBMI->bmiHeader.biSizeImage = dwBmBitsSize;
	}

	// Calculate the file size by adding the DIB size to sizeof(BITMAPFILEHEADER)
	bmfHdr.bfSize = dwDIBSize + sizeof(BITMAPFILEHEADER);
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;

	/*
	 * Now, calculate the offset the actual bitmap bits will be in
	 * the file -- It's the Bitmap file header plus the DIB header,
	 * plus the size of the color table.
	 */
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + m_pBMI->bmiHeader.biSize + PaletteSize();

	// Write the file header
	file.Write((LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER));
	DWORD dwBytesSaved = sizeof(BITMAPFILEHEADER); 

	// Write the DIB header
	UINT nCount = sizeof(BITMAPINFO) + (NumColors()-1)*sizeof(RGBQUAD);
	dwBytesSaved += nCount; 
	file.Write(m_pBMI, nCount);
	
	// Write the DIB bits
	DWORD dwBytes = m_pBMI->bmiHeader.biBitCount * Width();
  // Calculate the number of bytes per line
	if (dwBytes%32 == 0)
		dwBytes /= 8;
	else
		dwBytes = dwBytes/8 + (32-dwBytes%32)/8 + (((32-dwBytes%32)%8 > 0) ? 1 : 0); 
	nCount = dwBytes * Height();
	dwBytesSaved += nCount; 
	file.Write(m_pBits, nCount);

	return dwBytesSaved;
}


DWORD CDib::Read(CFile& file)
{
	// Ensures no memory leaks will occur
	Free();
	
	BITMAPFILEHEADER bmfHeader;

	// Go read the DIB file header and check if it's valid.
	if (file.Read((LPSTR)&bmfHeader, sizeof(bmfHeader)) != sizeof(bmfHeader))
		return 0;
	if (bmfHeader.bfType != DIB_HEADER_MARKER)
		return 0;
	DWORD dwReadBytes = sizeof(bmfHeader);

	// Allocate memory for DIB
	m_pBMI = (LPBITMAPINFO)GlobalAllocPtr(GHND, bmfHeader.bfOffBits-sizeof(BITMAPFILEHEADER) + 256*sizeof(RGBQUAD));
	if (m_pBMI == 0)
		return 0;

	// Read header.
	if (file.Read(m_pBMI, bmfHeader.bfOffBits-sizeof(BITMAPFILEHEADER)) != (UINT)(bmfHeader.bfOffBits-sizeof(BITMAPFILEHEADER)))
	{
		GlobalFreePtr(m_pBMI);
		m_pBMI = NULL;
		return 0;
	}
	dwReadBytes += bmfHeader.bfOffBits-sizeof(BITMAPFILEHEADER);

	DWORD dwLength = file.GetLength();
	// Go read the bits.
	m_pBits = (LPBYTE)GlobalAllocPtr(GHND, dwLength - bmfHeader.bfOffBits);
	if (m_pBits == 0)
	{
		GlobalFreePtr(m_pBMI);
		m_pBMI = NULL;
		return 0;
	}
	
	if (file.Read(m_pBits, dwLength-bmfHeader.bfOffBits) != (dwLength - bmfHeader.bfOffBits))
	{
		GlobalFreePtr(m_pBMI);
		m_pBMI = NULL;
		GlobalFreePtr(m_pBits);
		m_pBits = NULL;
		return 0;
	}
	dwReadBytes += dwLength - bmfHeader.bfOffBits;

	CreatePalette();
	return dwReadBytes;
}

#ifdef _DEBUG
void CDib::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);
}
#endif

HGLOBAL CDib::CopyToHandle() const
{
	CSharedFile file;
	try
	{
		if (Save(file)==0)
			return 0;
	}
	catch (CFileException* e)
	{
		e->Delete();
		return 0;
	}
		
	return file.Detach();
}


DWORD CDib::ReadFromHandle(HGLOBAL hGlobal)
{
	CSharedFile file;
	file.SetHandle(hGlobal, FALSE);
	DWORD dwResult = Read(file);
	file.Detach();
	return dwResult;
}

//////////////////////////////////////////////////////////////////////////
//// Serialization support

void CDib::Serialize(CArchive& ar) 
{
	CFile* pFile = ar.GetFile();
	ASSERT(pFile != NULL);
	if (ar.IsStoring())
	{	// storing code
		Save(*pFile);
	}
	else
	{	// loading code
		Read(*pFile);
	}
}
