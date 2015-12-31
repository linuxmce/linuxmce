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
#ifndef X3DBUTTON_H
#define X3DBUTTON_H

#include <string>

#include "X3DWindow.h"

namespace DCE {
/**
@author Ioan Marius Curelariu
*/
class X3DButton : public X3DWindow
{
protected:
    std::string m_sText;
    Font m_font;
    
public:
    X3DButton();
    ~X3DButton();
    
    void SetFont(Font font);

    /*virtual */bool DrawWindow();
    /*virtual */int CreateWindow(Display *pDisplay, int screen, Window wndParent, int x, int y, int cx, int cy, std::string sText);
};

}	// namespace DCE

#endif
