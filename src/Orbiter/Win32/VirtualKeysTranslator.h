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
#ifndef __VIRTUAL_KEYS_TRANSLATOR_H__
#define __VIRTUAL_KEYS_TRANSLATOR_H__

#include "Windows.h"
//-----------------------------------------------------------------------------------------------------
#define VK_A 0x41
#define VK_C 0x43
#define VK_D 0x44
#define VK_E 0x45
#define VK_T 0x54
#define VK_P 0x50
#define VK_L 0x4C
#define VK_R 0x52
#define VK_Z 0x5A
#define VK_W 0x57
//-----------------------------------------------------------------------------------------------------
bool TranslateVirtualKeys2PlutoKeys(UINT uMsg, WPARAM wParam, LPARAM lParam,
                               int& iPK_Button, bool& bShiftDown, bool& bAltDown, 
                               bool& bControlDown, bool& m_bCapsLock);
//-----------------------------------------------------------------------------------------------------
#endif
