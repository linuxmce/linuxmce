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
#include "X3DWindow.h"
using namespace DCE;

X3DWindow::X3DWindow()
{
    m_pDisplay = NULL;
    m_nScreen = 0;
    m_nPosX = 0;
    m_nPosY = 0;
    m_nWidth = 0;
    m_nHeight = 0;
    m_wndParent = 0;
    m_wndThis = 0;
    
    m_nBorderWidth = 0;
    m_bVisible = false;
}


X3DWindow::~X3DWindow()
{
    if (m_wndThis) {
        DestroyWindow();
    }
}


int X3DWindow::CreateWindow(Display *pDisplay, int screen, Window wndParent, int x, int y, int cx, int cy)
{
    if (m_wndThis) {
        DestroyWindow();
    }
    
    m_pDisplay = pDisplay;
    m_nScreen = screen;
    m_nPosX = x;
    m_nPosY = y;
    m_nWidth = cx;
    m_nHeight = cy;
    m_wndParent = wndParent;
    
    Visual visual;
    visual.visualid = CopyFromParent;
    
    unsigned long mask = CWEventMask | CWBackPixel;
    XSetWindowAttributes attrs;
    attrs.event_mask = ExposureMask;    // | ButtonPressMask | KeyPressMask | ButtonReleaseMask;
    attrs.background_pixel = 0xF0F0F0;
    
    m_wndThis = XCreateWindow (
                    m_pDisplay,
                    m_wndParent,
                    m_nPosX, m_nPosY,
                    m_nWidth, m_nHeight,
                    m_nBorderWidth,
                    DefaultDepth (m_pDisplay, m_nScreen),
                    InputOutput,
                    &visual,
                    mask, &attrs);
                    
    return 0;
}

int X3DWindow::DestroyWindow()
{
    XDestroyWindow (m_pDisplay, m_wndThis);
    m_wndThis = 0;
    m_bVisible = false;
    return m_wndThis;
}

bool X3DWindow::ShowWindow(bool bShow/* = true*/)
{
    if (!m_wndThis) return false;
    
    if (bShow && !m_bVisible) {
        XMapWindow (m_pDisplay, m_wndThis);
        m_bVisible = true;
    }
    if (!bShow && m_bVisible) {
        XUnmapWindow (m_pDisplay, m_wndThis);
        m_bVisible = false;
    }
    return true;
}

bool X3DWindow::ChangeAttributes(unsigned long mask, XSetWindowAttributes attrs)
{
    if (!m_wndThis) return false;
    
    /*int nRet = */XChangeWindowAttributes (m_pDisplay, m_wndThis, mask, &attrs);
    
    return true;
}

bool X3DWindow::DrawWindow()
{
    unsigned long mask = GCForeground | GCBackground | GCLineWidth;
    XGCValues values;
    values.foreground = 0xFFFFFF;
    values.background = 0xFFFFFF;
    values.line_width = 1;
    
    GC gcWhite = XCreateGC(
                m_pDisplay, 
                m_wndThis,
                mask,
                &values);
                
    values.foreground = 0x000000;
    values.background = 0x000000;
    GC gcBlack = XCreateGC(
                m_pDisplay, 
                m_wndThis,
                mask,
                &values);
                
    XDrawLine(m_pDisplay, m_wndThis, gcWhite, 1, 1, m_nWidth, 1);
    XDrawLine(m_pDisplay, m_wndThis, gcWhite, 1, 1, 1, m_nHeight);

    XDrawLine(m_pDisplay, m_wndThis, gcBlack, 1, m_nHeight-1, m_nWidth, m_nHeight-1);
    XDrawLine(m_pDisplay, m_wndThis, gcBlack, m_nWidth-1, 1, m_nWidth-1, m_nHeight);
                
    XFreeGC (m_pDisplay, gcWhite);
    XFreeGC (m_pDisplay, gcBlack);
                
    return true;
}

bool X3DWindow::HitTest(int x, int y)
{
    if (x <= 0 || y <= 0) return false;
    if (x >= m_nWidth || y >= m_nHeight) return false;
    return true;
}
