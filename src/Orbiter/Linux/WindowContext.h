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
#ifndef __WINDOW_CONTEXT_H__
#define __WINDOW_CONTEXT_H__

#include "SerializeClass/ShapesColors.h"
#include "utilities/linux/window_manager/WMController/WindowLayer.h"
#include <map>
//-----------------------------------------------------------------------------------------------------
class WindowContext
{
public:

	WindowContext(string sWindowName, bool bExclusiveMode);
	~WindowContext();

	WindowLayer Layer();
	void Layer(WindowLayer layer);

	bool IsMaximized();
	void Maximize(bool bValue);

	bool IsFullScreen();
	void FullScreen(bool bValue);

	bool IsVisible();
	void Visible(bool bValue);

	bool IsActivated();
	void Activate(bool bValue);

	PlutoRectangle Position();
	void Position(PlutoRectangle rectangle);

	bool IsErrorFlag();
	void ErrorFlag(bool bValue);

	string ToString();

	WindowContext& operator=(const WindowContext& context);
	friend bool operator==(const WindowContext& context1, const WindowContext& context2);
	friend bool operator!=(const WindowContext& context1, const WindowContext& context2);

private:

	WindowContext() {}

	string m_sWindowName;
	WindowLayer m_WindowLayer;
	bool m_bMaximized;
	bool m_bFullScreen;
	bool m_bVisible;
	bool m_bActivated;
	bool m_bErrorFlag;
	PlutoRectangle m_rectPosition;

	bool m_bExclusiveMode;

	bool IsOurBackgroundApp();
};
//-----------------------------------------------------------------------------------------------------
#endif //__WINDOW_CONTEXT_H__
