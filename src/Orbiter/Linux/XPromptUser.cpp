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
#include "PlutoUtils/CommonIncludes.h"
#include "XPromptUser.h"
#include "DCE/Logger.h"
#include <X11/Xutil.h>

using namespace DCE;

XPromptUser::XPromptUser(string sPrompt, int iTimeoutSeconds, map<int, string> *pMapPrompts/*=NULL*/)
	: m_nSpaceBetweenLines(4)
	, m_bFreeTextFont(false)
	, m_bFreeBtnFont(false)
	, m_btnPlacement(BTN_HORIZ)
	, m_wndName("UserPrompt")
{
	m_sPrompt = sPrompt;
	m_pMapPrompts = pMapPrompts;
	m_tTimeout = iTimeoutSeconds ? time(NULL) + iTimeoutSeconds : 0;

	m_textFont = m_btnFont = 0;
	m_nTextPosX = 10;
	m_nTextPosY = 40;
	m_nTextHeight = 40;

	m_nUserOption = 0;
}

XPromptUser::~XPromptUser()
{
	if (m_pDisplay && m_textFont && m_bFreeTextFont) {
		XUnloadFont(m_pDisplay, m_textFont);
	}
	if (m_pDisplay && m_btnFont && m_bFreeBtnFont) {
		XUnloadFont(m_pDisplay, m_btnFont);
	}
}

void XPromptUser::SetFonts(Font textFont, Font btnFont)
{
	if (textFont) {
		if (m_textFont && m_bFreeTextFont) {
			XUnloadFont(m_pDisplay, m_textFont);
			m_bFreeTextFont = false;
		}
	}
	if (btnFont) {
		m_textFont = textFont;
		if (m_btnFont && m_bFreeBtnFont) {
			XUnloadFont(m_pDisplay, m_btnFont);
			m_bFreeBtnFont = false;
		}
		m_btnFont = btnFont;
	}
}

int XPromptUser::CreateWindow(Display *pDisplay, int nScreen, Window wndParent, int x, int y, int cx, int cy)
{
	X3DWindow::CreateWindow(pDisplay, nScreen, wndParent, x, y, cx, cy);

	int textWidth, textHeight;
	if (m_textFont == 0) {
		m_textFont = XLoadFont(m_pDisplay, "-*-*-*-R-Normal--*-180-75-75-*-*");
		m_bFreeTextFont = true;
	}
	if (m_btnFont == 0) {
		m_btnFont =XLoadFont(m_pDisplay, "-*-*-*-R-Normal--*-180-75-75-*-*");
		m_bFreeBtnFont = true;
	}

	GetPromptTextSizeHint(m_sPrompt, textWidth, textHeight);
	if (textWidth) {
		int dir, ascent, descent;
		XCharStruct charStruct;
		XQueryTextExtents(m_pDisplay, m_textFont, "X", 1,
				&dir, &ascent, &descent, &charStruct);

		m_nTextPosY = 10 + ascent;
		m_nTextHeight = ascent + descent;
	} else {
		m_nTextPosY = 0;
	}

	if (m_pMapPrompts) {
		switch (m_btnPlacement) {
		case BTN_HORIZ:
			{
				int nBtnX = 10, nBtnY = 10 + m_nTextPosY;

				// compute the desired size for the buttons ...
				int nWidth = 0, nHeight = 0;
				map<int, string>::iterator it = m_pMapPrompts->begin();
				while (it != m_pMapPrompts->end()) {
					GetBtnTextSizeHint(it->second, textWidth, textHeight);
					if (textWidth > nWidth) nWidth = textWidth;
					if (textHeight > nHeight)  nHeight = textHeight;

					it++;
				}

				// it is nice to have some space around the label ...
				nWidth += 20;
				nHeight += 10;

				X3DButton *pButton = NULL;
				it = m_pMapPrompts->begin();
				while (it != m_pMapPrompts->end()) {
					pButton = new X3DButton();
					pButton->SetFont(m_btnFont);
					pButton->CreateWindow(m_pDisplay, m_nScreen, m_wndThis, nBtnX, nBtnY, nWidth, nHeight, it->second);
					m_mapButtons[it->first] = pButton;

					nBtnX += nWidth;
					if (nBtnX + nWidth + 10 > m_nWidth) {
						nBtnX = 10;
						nBtnY += nHeight + 10;
					}

					it++;
				}
			}
			break;
		default:
		case BTN_VERT:
			{
				int nBtnX = 10, nBtnY = 10 + m_nTextPosY;

				// compute the desired size for the buttons ...
				int nWidth = 0, nHeight = 0;
				map<int, string>::iterator it = m_pMapPrompts->begin();
				while (it != m_pMapPrompts->end()) {
					GetBtnTextSizeHint(it->second, textWidth, textHeight);
					if (textWidth > nWidth) nWidth = textWidth;
					if (textHeight > nHeight)  nHeight = textHeight;

					it++;
				}

				// it is nice to have some space around the label ...
				nWidth += 40;
				nHeight += 10;
				
				nBtnX = (m_nWidth - nWidth) / 2;

				X3DButton *pButton = NULL;
				it = m_pMapPrompts->begin();
				while (it != m_pMapPrompts->end()) {
					pButton = new X3DButton();
					pButton->SetFont(m_btnFont);
					pButton->CreateWindow(m_pDisplay, m_nScreen, m_wndThis, nBtnX, nBtnY, nWidth, nHeight, it->second);
					m_mapButtons[it->first] = pButton;

					nBtnY += nHeight + 10;
					it++;
				}
			}
			break;
		}
	}

	XClassHint ClassHint;
	ClassHint.res_name = (char *)m_wndName.c_str();
	ClassHint.res_class = (char *)m_wndName.c_str();
	XSetClassHint(m_pDisplay, m_wndThis, &ClassHint);
    
	return 0;
}

bool XPromptUser::ShowWindow(bool bShow/*= true*/)
{
	if (!m_wndThis) return false;

	map<int, X3DButton*>::iterator it = m_mapButtons.begin();
	while (it != m_mapButtons.end()) {
		it->second->ShowWindow(bShow);
		
		it++;
	}
	
	X3DWindow::ShowWindow(bShow);
    if (bShow)
    {
        XClassHint classHint;
        classHint.res_name = "prompt_user_window";
        classHint.res_class = "prompt_user_window";
        XSetClassHint(m_pDisplay, m_wndThis, &classHint);
        XStoreName(m_pDisplay, m_wndThis, "prompt_user_window");
        XSync(m_pDisplay, false);
    }
	return true;
}

bool XPromptUser::DrawWindow()
{
	bool bRet = X3DWindow::DrawWindow();
	if (!bRet) return false;

	unsigned long mask = GCForeground | GCBackground | GCFont | GCLineWidth;
	XGCValues values;
	values.foreground = 0x000000;
	values.background = 0x000000;
	values.line_width = 1;
	values.font = m_textFont;
	
	GC gcBackground = XCreateGC(m_pDisplay, m_wndThis, mask, &values);
	
	values.foreground = 0xFFFFFF;
	values.background = 0xFFFFFF;
	GC gcText = XCreateGC(m_pDisplay, m_wndThis, mask, &values);

	XFillRectangle(m_pDisplay, m_wndThis, gcBackground, 2, 2, m_nWidth-4, m_nHeight-4);	// it shold be enough to clean the text area here, but anyway ...
	list<string> stringList = SplitTextInLines(m_sPrompt);
	list<string>::iterator it = stringList.begin();
	int nTextY = m_nTextPosY;
	while (it != stringList.end()) {
		XDrawString(m_pDisplay, m_wndThis, gcText, m_nTextPosX, nTextY, (*it).c_str(), (*it).length());
		nTextY += m_nTextHeight + m_nSpaceBetweenLines;
		
		it++;
	}

	XFreeGC(m_pDisplay, gcText);
	XFreeGC(m_pDisplay, gcBackground);

	map<int, X3DButton*>::iterator btnit = m_mapButtons.begin();
	while (btnit != m_mapButtons.end()) {
		btnit->second->DrawWindow();
		
		btnit++;
	}

	return true;
}

list<string> XPromptUser::SplitTextInLines(string sText)
{
	list<string> lines;

	int nP1, nP2 = 0;
	nP1 = sText.find('\n', nP2);
	while (nP1 >= 0) {
		lines.push_back(sText.substr(nP2, nP1 - nP2));
		nP2 = nP1 + 1;
		nP1 = sText.find('\n', nP2);
	}
	lines.push_back(sText.substr(nP2, sText.length() - nP2));
	
	return lines;
}

bool XPromptUser::GetPromptTextSizeHint(string sText, int &width, int &height)
{
	return GetTextSizeHint(sText, m_textFont, width, height);
}

bool XPromptUser::GetBtnTextSizeHint(string sText, int &width, int &height)
{
	return GetTextSizeHint(sText, m_btnFont, width, height);
}

bool XPromptUser::GetTextSizeHint(string sText, Font font, int &width, int &height)
{
	width = height = 0;
	if (sText.length() == 0) return true;
	if (font == 0) return false;
	if (m_pDisplay == NULL) return false;

	int dir, ascent, descent;
	XCharStruct charStr;
	list<string> textLines = SplitTextInLines(sText);
	list<string>::iterator it = textLines.begin();
	while (it != textLines.end()) {
		XQueryTextExtents(m_pDisplay, font, (*it).c_str(), (*it).length(),
				&dir, &ascent, &descent, &charStr);

		if (width < charStr.width) width = charStr.width;
		height += ascent + descent + m_nSpaceBetweenLines;
		it++;
	}
	
	return true;
}

bool XPromptUser::EventLoop()
{
	XEvent event;
	bool bDone = false;
	while (!bDone) {
		while( true )
		{
			if( XCheckTypedEvent(m_pDisplay, Expose, &event) ||
				XCheckTypedEvent(m_pDisplay, ButtonRelease, &event) )
					break;
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"nO EVENT time %d timeout %d",(int) time(NULL), (int) m_tTimeout);
			if( m_tTimeout && m_tTimeout<time(NULL) )
			{
LoggerWrapper::GetInstance()->Write(LV_STATUS,"Timed out waiting for prompt user");
				return true;
}
			usleep(250000); // Don't hog the cpu
		}

LoggerWrapper::GetInstance()->Write(LV_STATUS,"got event %d (%d)",(int) event.type,(int) ButtonRelease);

		switch (event.type) {
		case Expose:
			if (LoggerWrapper::GetInstance()) LoggerWrapper::GetInstance()->Write(LV_STATUS, "XPromptUser: Expose event");
			DrawWindow();
			break;
		case ButtonRelease:
			map<int, X3DButton*>::iterator it = m_mapButtons.begin();
			while (!bDone && it != m_mapButtons.end()) {
				XButtonEvent *pButtonEvent = (XButtonEvent *)&event;
				if ((*it).second && (*it).second->HitTest(pButtonEvent->x, pButtonEvent->y)) {
					bDone = true;
					m_nUserOption = (*it).first;
				}
				it++;
			}
			break;
		}
	}

	return true;
}

void XPromptUser::UnloadFonts()
{
	if (!m_pDisplay) return;

	if (m_textFont && m_bFreeTextFont) {
		XUnloadFont(m_pDisplay, m_textFont);
		m_bFreeTextFont = false;
	}
	if (m_btnFont && m_bFreeBtnFont) {
		XUnloadFont(m_pDisplay, m_btnFont);
		m_bFreeBtnFont = false;
	}
}

void XPromptUser::Init()
{
	Display *pDisplay = XOpenDisplay(NULL);
	if( pDisplay==NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "XPromptUser::Init(): cannot open display");
		return;
	}
        
	int nScreenNo = DefaultScreen(pDisplay);
	int nDesktopX = DisplayWidth(pDisplay, nScreenNo);
	int nDesktopY = DisplayHeight(pDisplay, nScreenNo);

	CreateWindow(pDisplay, nScreenNo, DefaultRootWindow(pDisplay), 0, 0, nDesktopX, nDesktopY);
	ShowWindow();
	DrawWindow();
}

int XPromptUser::RunModal()
{
	EventLoop();
	
	return m_nUserOption;
}

void XPromptUser::DeInit()
{
	map<int, X3DButton*>::iterator btnit = m_mapButtons.begin();
	while (btnit != m_mapButtons.end()) {
		btnit->second->DestroyWindow();
		
		btnit++;
	}
	UnloadFonts();
	DestroyWindow();
	XSync(m_pDisplay, false);
	XCloseDisplay(m_pDisplay);
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "XPromptUser::DeInit(): pDisplay=%d ->Closed", m_pDisplay);
	m_pDisplay = NULL;
	
}

void XPromptUser::SetButtonPlacement(BtnPlacement placement) 
{
	m_btnPlacement = placement;
}
