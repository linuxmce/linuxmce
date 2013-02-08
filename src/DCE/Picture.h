/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU Lesser General Public License for more details.

*/
// Picture.h: interface for the CPicture class.
//
//////////////////////////////////////////////////////////////////////

/** @file Picture.h
 Header file for the CPicture class.
 @todo notcommented
 */

#if !defined(AFX_PICTURE_H__6098A4C3_D6D5_4711_BC7B_1595F459B480__INCLUDED_)
#define AFX_PICTURE_H__6098A4C3_D6D5_4711_BC7B_1595F459B480__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <atlbase.h>


    /**
    @class CPicture
    This class ???
     */
class CPicture
{
public:
	CPicture();
	virtual ~CPicture();

	bool			Load(CString sResourceType, CString sResource);
	bool			Load(CString sFileName);
	bool			Draw(CDC* pDC);
	bool			Draw(CDC* pDC, CPoint Pos);
	bool			Draw(CDC* pDC, CPoint Pos, CSize Size);
	bool			Draw(CDC* pDC, double nSizeRatio);
	bool			Draw(CDC* pDC, CPoint Pos, double nSizeRatio);
	CSize			GetSize(CDC* pDC);
	long			GetOriginalWidth() {
						long hmWidth;  m_pPicture->get_Width(&hmWidth);  return hmWidth;
	}
	long			GetOriginalHeight() {
						long hmHeight;  m_pPicture->get_Height(&hmHeight);  return hmHeight;
	}
private:
	static	bool	GetResource(LPSTR lpName, LPSTR lpType, void* pResource, int& nBufSize);
	void			UnLoad();
	bool			LoadFromBuffer(BYTE* pBuff, int nSize);
	bool			Draw(CDC* pDC, int x, int y, int cx, int cy);
	IPicture*		m_pPicture;
	enum
	{
		HIMETRIC_INCH	= 2540
	};
};

#endif // !defined(AFX_PICTURE_H__6098A4C3_D6D5_4711_BC7B_1595F459B480__INCLUDED_)
