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
// StaticPicture.cpp : implementation file
//

#include "stdafx.h"
#include "PlutoVIP.h"
#include "StaticPicture.h"
#include "DIB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStaticPicture

CStaticPicture::CStaticPicture()
{
	
}

CStaticPicture::~CStaticPicture()
{
	if (m_Picture.m_hObject)
		m_Picture.Detach();
}


BEGIN_MESSAGE_MAP(CStaticPicture, CStatic)
	//{{AFX_MSG_MAP(CStaticPicture)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStaticPicture message handlers

void CStaticPicture::SetPicture(CString szPictureFile)
{
	CRect rect;
	GetClientRect(&rect);
	
	ModifyStyle(SS_BLACKFRAME, SS_BITMAP | SS_CENTERIMAGE);

	HBITMAP     hBitmap = NULL;
	HPALETTE    hPal = NULL;
	HDC			hMemDC = NULL;	
	HGDIOBJ		hOldObj = NULL;
	
	CDib		dib;
	int			nWidth, nHeight;

	// no images

	if (szPictureFile.IsEmpty())
	{
		//load a default bitmap for customer
		// attach the thumbnail bitmap handle to an CBitmap object
		m_Picture.LoadBitmap(IDB_DUMMY_BITMAP);
		
		SetBitmap((HBITMAP) m_Picture.m_hObject);
		Invalidate();
	}
	
	CFile ImgFile;
		
	szPictureFile.Replace(_T(".jpg"), _T(".bmp"));
	if (ImgFile.Open(szPictureFile, CFile::modeRead))
	{
		dib.Read(ImgFile);
		ImgFile.Close();
	}
	else
	{
		if (m_Picture.m_hObject)
			m_Picture.Detach();

		m_Picture.LoadBitmap(IDB_DUMMY_BITMAP);
		
		SetBitmap((HBITMAP) m_Picture.m_hObject);
		Invalidate();
		return;
	}

	nWidth = dib.m_pBMI->bmiHeader.biWidth;
	nHeight = dib.m_pBMI->bmiHeader.biHeight;
	dib.m_pBMI->bmiHeader.biWidth = rect.Width();
	dib.m_pBMI->bmiHeader.biHeight = rect.Height();
		
	// create thumbnail bitmap section
	hBitmap = ::CreateDIBSection(NULL, 
								 dib.m_pBMI, 
							     DIB_RGB_COLORS, 
							     NULL, 
							     NULL, 
							     0); 

	// restore dib header
	dib.m_pBMI->bmiHeader.biWidth = nWidth;
	dib.m_pBMI->bmiHeader.biHeight = nHeight;

	// select thumbnail bitmap into screen dc
	hMemDC = ::CreateCompatibleDC(NULL);	
	hOldObj = ::SelectObject(hMemDC, hBitmap);
	  
	// grayscale image, need palette
	if(dib.m_pPalette != NULL)
	{
		hPal = ::SelectPalette(hMemDC, (HPALETTE)dib.m_pPalette->GetSafeHandle(), FALSE);
		::RealizePalette(hMemDC);
	}
	  
	// set stretch mode
	::SetStretchBltMode(hMemDC, COLORONCOLOR);

	// populate the thumbnail bitmap bits
	::StretchDIBits(hMemDC, 0, 0, 
					rect.Width(), rect.Height(), 
					0, 0, 
					dib.m_pBMI->bmiHeader.biWidth, 
					dib.m_pBMI->bmiHeader.biHeight, 
					dib.m_pBits, 
					dib.m_pBMI, 
					DIB_RGB_COLORS, 
					SRCCOPY);

	// restore DC object
	::SelectObject(hMemDC, hOldObj);
	  
	// restore DC palette
	if(dib.m_pPalette != NULL)
		::SelectPalette(hMemDC, (HPALETTE)hPal, FALSE);
	  
	// clean up
	::DeleteObject(hMemDC);
		
	// detatach any prev. attached picture
	m_Picture.Detach();

	// attach the thumbnail bitmap handle to an CBitmap object
	m_Picture.Attach(hBitmap);

	SetBitmap(m_Picture);

	Invalidate();
}
