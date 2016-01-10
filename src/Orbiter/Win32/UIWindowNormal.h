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
// C++ Implementation: UIWindowNormal
//
// Description: Doesn't apply any filter on Orbiter's window
//
//
// Author: Cristian Miron, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

//-----------------------------------------------------------------------------------------------------
#include "UIWindowBase.h"
//-----------------------------------------------------------------------------------------------------
class UIWindowNormal : public UIWindowBase
{
public:

	void AddFilter(const PlutoRectangle& rect, const unsigned char *pFilter = NULL) {};
	void RemoveFilter(const PlutoRectangle& rect) {};

	void ClearFilters() {};
	void ApplyFilters(HWND hWnd) {};

	void DumpMask(int nWidth, int nHeight) {};
};
//-----------------------------------------------------------------------------------------------------
