#pragma once

/*
Copyright (C) 2004 Pluto, Inc., a Florida Corporation

www.plutohome.com


Phone: +1 (877) 758-8648


This program is distributed according to the terms of the Pluto Public License, available at:
http://plutohome.com/index.php?section=public_license

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

*/

//
// C++ Implementation: UIWindowBase
//
// Description: The interface for UIWindow's
//
//
// Author: Cristian Miron, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
//-----------------------------------------------------------------------------------------------------
#include "SerializeClass/ShapesColors.h"
//-----------------------------------------------------------------------------------------------------
/*interface*/ class UIWindowBase
{
public:

	virtual void AddFilter(const PlutoRectangle& rect, const unsigned char *pFilter = NULL) = 0;
	virtual void RemoveFilter(const PlutoRectangle& rect) = 0;

	virtual void ClearFilters() = 0;
	virtual void ApplyFilters(HWND hWnd) = 0;

	virtual void DumpMask(int nWidth, int nHeight) = 0;
};
//-----------------------------------------------------------------------------------------------------
