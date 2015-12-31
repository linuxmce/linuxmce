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
#pragma once
//-----------------------------------------------------------------------------------------------------
#include "Wingdi.h"

#ifdef WINCE
#define LPDRAWTEXTPARAMS void *
#endif

//-----------------------------------------------------------------------------------------------------
inline LPWSTR ToWide(LPCSTR lpString, int cbString, int* sz)
{
	LPWSTR lpwString;

	// Determine required buffer
	*sz = MultiByteToWideChar(CP_UTF8, 0, lpString, cbString, 0, 0);
	if (*sz == 0) { return 0; }

	// Allocate memory
	lpwString = (LPWSTR)new WCHAR[*sz * sizeof(wchar_t)];
	if (lpwString == 0) { return 0; }

	// Convert utf-8 to wide character
	*sz = MultiByteToWideChar(CP_UTF8, 0, lpString, cbString, lpwString, *sz);
	if (*sz == 0) 
	{
		delete [] lpwString;
		return 0;
	}

	return lpwString;
}
//-----------------------------------------------------------------------------------------------------
inline int DrawTextExUTF8(HDC hdc, LPCSTR lpchText, int cchText, LPRECT lprc, UINT dwDTFormat, LPDRAWTEXTPARAMS lpDTParams)
{
	int sz;
	LPWSTR lpwchText;
	BOOL retVal;

	// Correction for null
	int np = (cchText == -1) ? 1 : 0;

	lpwchText = ToWide(lpchText, cchText, &sz);

#ifdef WINCE
	retVal = DrawText(hdc, lpwchText, sz - np, lprc, dwDTFormat);
#else
	// Do unicode DrawTextExW
	retVal = DrawTextExW(hdc, lpwchText, sz - np, lprc, dwDTFormat, lpDTParams);
#endif

	// Free buffer
	delete [] lpwchText;

	return retVal;
}
