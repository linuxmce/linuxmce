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
#ifndef XProgressWnd_H
#define XProgressWnd_H

#include "X3DWindow.h"
#include "X3DButton.h"

#include "PlutoUtils/MultiThreadIncludes.h"

#include <string>
#include <pthread.h>

namespace DCE {
/**
@author Ioan Marius Curelariu
*/
class XProgressWnd : public X3DWindow
{
protected:
    X3DButton *m_pButton;
    int m_nBarX, m_nBarY, m_nTextX, m_nTextY;
    int m_nBarWidth, m_nBarHeight, m_nTextWidth;

    std::string m_sText;
    int m_nProgress;

    bool m_bCanceled;
    bool m_bDone;
    bool m_bDestroy;


public:
    pthread_t m_thisThread;

    std::string m_wndName;

public:
    XProgressWnd();
    ~XProgressWnd();

    /* virtual */bool DrawWindow();
    /* virtual */bool ShowWindow(bool bShow = true);
    /* virtual */int CreateWindow(Display *pDisplay, int screen, Window wndParent, int x, int y, int cx, int cy);
    bool UpdateProgress(std::string sText, int nProgress);
    bool IsCancelled(){ return m_bCanceled; };
    bool Destroy(){ return m_bDestroy; }

    virtual bool EventLoop();
    virtual pthread_t Run();
    virtual void Terminate();

    static Bool CheckIfEvent(Display *pDisplay, XEvent *pEvent, XPointer arg);

	Display * GetDisplay();

private:
	list<string> SplitTextInLines(string sText);
	bool GetTextSizeHint(string sText, Font font, int &width, int &height);
	list<string> WrapTextLines(list<string> lText, Font font, int maxWidth, int &RequiredHeight);
};

}	// namespace DCE

#endif
