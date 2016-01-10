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
#include "WindowContext.h"
#include "DCE/Logger.h"
using namespace DCE;
//-----------------------------------------------------------------------------------------------------
char *WindowLayerStr[] = 
{
	"LayerUnknown",
	"LayerBelow",
	"LayerNormal",
	"LayerAbove"
};
//-----------------------------------------------------------------------------------------------------
WindowContext::WindowContext(string sWindowName, bool bExclusiveMode) :
	m_sWindowName(sWindowName),
	m_WindowLayer(LayerNormal),
	m_bMaximized(false),
	m_bFullScreen(false),
	m_bVisible(true),
	m_bActivated(false),
	m_bErrorFlag(false),
	m_rectPosition(-1, -1, -1, -1),
	m_bExclusiveMode(bExclusiveMode)
{
}
//-----------------------------------------------------------------------------------------------------
WindowContext::~WindowContext()
{
}
//-----------------------------------------------------------------------------------------------------
WindowLayer WindowContext::Layer()
{
	return m_WindowLayer;
}
//-----------------------------------------------------------------------------------------------------
void WindowContext::Layer(WindowLayer layer)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "WindowContext::Layer '%s' : %s", m_sWindowName.c_str(), WindowLayerStr[layer]);
#endif

	m_WindowLayer = layer;
}
//-----------------------------------------------------------------------------------------------------
bool WindowContext::IsMaximized()
{
	return m_bMaximized;
}
//-----------------------------------------------------------------------------------------------------
void WindowContext::Maximize(bool bValue)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "WindowContext::Maximize '%s' : %s", m_sWindowName.c_str(), bValue ? "true" : "false");
#endif
	m_bMaximized = bValue;
}
//-----------------------------------------------------------------------------------------------------
bool WindowContext::IsFullScreen()
{
	return m_bFullScreen;
}
//-----------------------------------------------------------------------------------------------------
void WindowContext::FullScreen(bool bValue)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "WindowContext::FullScreen '%s' : %s", m_sWindowName.c_str(), bValue ? "true" : "false");
#endif
	m_bFullScreen = bValue;
}
//-----------------------------------------------------------------------------------------------------
bool WindowContext::IsVisible()
{
	return m_bVisible;
}
//-----------------------------------------------------------------------------------------------------
void WindowContext::Visible(bool bValue)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "WindowContext::Visible '%s' : %s", m_sWindowName.c_str(), bValue ? "true" : "false");
#endif

        if(!bValue)
                m_bActivated = false; // If the window has become invisible, it should be activated the next time Activate(true) is set

	if(IsOurBackgroundApp() && m_bExclusiveMode)
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "WindowContext::Visible '%s' : changing the layer instead of visibility to: %s", 
				m_sWindowName.c_str(), bValue ? "normal" : "below");
#endif

		m_WindowLayer = bValue ? LayerNormal : LayerBelow;
		return;
	}

	m_bVisible = bValue;
}
//-----------------------------------------------------------------------------------------------------
bool WindowContext::IsActivated()
{
	return m_bActivated;
}
//-----------------------------------------------------------------------------------------------------
void WindowContext::Activate(bool bValue)
{	
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "WindowContext::Activate '%s' : %s", m_sWindowName.c_str(), bValue ? "true" : "false");
#endif
	m_bActivated = bValue;
}

//-----------------------------------------------------------------------------------------------------
bool WindowContext::IsErrorFlag()
{
	return m_bErrorFlag;
}
//-----------------------------------------------------------------------------------------------------
void WindowContext::ErrorFlag(bool bValue)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "WindowContext::IsErrorFlag '%s' : %s", m_sWindowName.c_str(), bValue ? "true" : "false");
#endif
	m_bErrorFlag = bValue;
}
//-----------------------------------------------------------------------------------------------------
PlutoRectangle WindowContext::Position()
{
	return m_rectPosition;
}
//-----------------------------------------------------------------------------------------------------
void WindowContext::Position(PlutoRectangle rectangle)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "WindowContext::Position '%s' : x %d, y %d, w %d, h %d", 
		m_sWindowName.c_str(), rectangle.X, rectangle.Y, rectangle.Width, rectangle.Height);
#endif
	m_rectPosition = rectangle;
}
//-----------------------------------------------------------------------------------------------------
string WindowContext::ToString()
{
	char result[512];
	sprintf(result, "layer %s, maximized %s, fullscreen %s, visible %s, "
		"activated %s, position: x %d, y %d, w %d, h %d",
		WindowLayerStr[m_WindowLayer], m_bMaximized ? "true" : "false", m_bFullScreen ? "true" : "false",
		m_bVisible ? "true" : "false", m_bActivated ? "true" : "false",
		m_rectPosition.X, m_rectPosition.Y, m_rectPosition.Width, m_rectPosition.Height
	);

	return result;
}
//-----------------------------------------------------------------------------------------------------
WindowContext& WindowContext::operator=(const WindowContext& context)
{
	m_WindowLayer = context.m_WindowLayer;
	m_bMaximized = context.m_bMaximized;
	m_bFullScreen = context.m_bFullScreen;
	m_bVisible = context.m_bVisible;
	m_bActivated = context.m_bActivated;
	m_rectPosition = context.m_rectPosition;
	m_sWindowName = context.m_sWindowName;

	return *this;
}
//-----------------------------------------------------------------------------------------------------
bool operator==(const WindowContext& context1, const WindowContext& context2)
{
	return 
		context1.m_WindowLayer == context2.m_WindowLayer				&&
		context1.m_bMaximized == context2.m_bMaximized					&&
		context1.m_bFullScreen == context2.m_bFullScreen				&&
		context1.m_bVisible == context2.m_bVisible						&&
		context1.m_bActivated == context2.m_bActivated					&&
		context1.m_rectPosition.X == context2.m_rectPosition.X			&&
		context1.m_rectPosition.Y == context2.m_rectPosition.Y			&&
		context1.m_rectPosition.Width == context2.m_rectPosition.Width	&&
		context1.m_rectPosition.Height == context2.m_rectPosition.Height;
}
//-----------------------------------------------------------------------------------------------------
bool operator!=(const WindowContext& context1, const WindowContext& context2)
{
	return !(context1 == context2);
}
//-----------------------------------------------------------------------------------------------------
bool WindowContext::IsOurBackgroundApp()
{
	return m_sWindowName.find("Screen_Saver") != string::npos;
}
//-----------------------------------------------------------------------------------------------------

