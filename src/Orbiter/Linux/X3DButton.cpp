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
#include "X3DButton.h"
using namespace DCE;

X3DButton::X3DButton()
{
    m_sText = "";
}


X3DButton::~X3DButton()
{
}


bool X3DButton::DrawWindow()
{
    bool bRet = X3DWindow::DrawWindow();
    if (bRet) {
	Font font = 0;
	    
        unsigned long mask = GCForeground | GCBackground | GCLineWidth | GCFont;
        XGCValues values;
        values.foreground = 0x000000;
        values.background = 0x000000;
        values.line_width = 1;
    	if (!m_font) {
	        font = XLoadFont(m_pDisplay, "-*-*-*-R-Normal--*-140-100-100-*-*");
		values.font = font;
	} else {
		values.font = m_font;
		font = m_font;
        }
        
        GC gcText = XCreateGC(m_pDisplay, m_wndThis, mask, &values);
        
        int dir, ascent, descent;
        XCharStruct charStr;
        XQueryTextExtents (m_pDisplay, font, m_sText.c_str(), m_sText.length(), 
                        &dir, &ascent, &descent, &charStr);
                        
        int xPos = (m_nWidth - charStr.width) / 2;
        int yPos = (m_nHeight + ascent - descent) / 2;
        
        XDrawString(m_pDisplay, m_wndThis, gcText, xPos, yPos, m_sText.c_str(), m_sText.length());
        
        if (!m_font) XUnloadFont(m_pDisplay, font);
        XFreeGC(m_pDisplay, gcText);
    }
    
    return bRet;
}

int X3DButton::CreateWindow(Display *pDisplay, int screen, Window wndParent, int x, int y, int cx, int cy, std::string sText)
{
    X3DWindow::CreateWindow(pDisplay, screen, wndParent, x, y, cx, cy);
    
    unsigned long mask = CWEventMask;
    XSetWindowAttributes attrs;
    attrs.event_mask = ExposureMask | ButtonPressMask | KeyPressMask | ButtonReleaseMask;
    ChangeAttributes(mask, attrs);
    
    m_sText = sText;
    
    return 0;
}

void X3DButton::SetFont(Font font)
{
	m_font = font;
}
