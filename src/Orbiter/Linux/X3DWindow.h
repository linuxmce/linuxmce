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
#ifndef X3DWINDOW_H
#define X3DWINDOW_H

#include <X11/X.h>
#include <X11/Xlib.h>

#include <list>

namespace DCE {
/**
@author Ioan Marius Curelariu
*/
class X3DWindow{
protected:
    Display *m_pDisplay;
    int m_nScreen;
    int m_nPosX, m_nPosY, m_nWidth, m_nHeight;
    int m_nBorderWidth;
    Window m_wndParent, m_wndThis;
    bool m_bVisible;
    
    std::list<Window> m_childs;
    
public:
    X3DWindow();
    virtual ~X3DWindow();
    
    virtual bool ShowWindow(bool bShow = true);
    virtual bool ChangeAttributes(unsigned long mask, XSetWindowAttributes attrs);
    virtual bool DrawWindow();
    
    Window GetWindow() { return m_wndThis; }
    bool HitTest(int x, int y);
    
public:
    virtual int CreateWindow(Display *pDisplay, int screen, Window wndParent, int x, int y, int cx, int cy);
    virtual int DestroyWindow();
};

}	// namespace DCE

#endif
