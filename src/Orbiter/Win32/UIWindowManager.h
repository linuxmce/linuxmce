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
// C++ Implementation: UIWindowManager
//
// Description: Manages UIWindow classes; Builder class
//
//
// Author: Cristian Miron, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

//-----------------------------------------------------------------------------------------------------
#include "UIWindowNormal.h"
#include "UIWindowMasked.h"
#include "UIWindowAlphaBlended.h"
//-----------------------------------------------------------------------------------------------------
enum UIWindowType
{
	uwtNormal,
	uwtMasked,
	uwtAlphaBlended
};
//-----------------------------------------------------------------------------------------------------
class UIWindowManager
{
	static UIWindowBase *m_pUIWindowBase;
	static UIWindowType m_UIWindowType;

public:

	static UIWindowBase *CreateUIWindow(UIWindowType type);
	static UIWindowBase *UIWindow();
	static UIWindowType Type();
};
//-----------------------------------------------------------------------------------------------------
