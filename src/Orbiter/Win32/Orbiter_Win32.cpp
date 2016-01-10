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
#include "Orbiter_Win32.h"
#include "SelfUpdate.h"
#include "MouseBehavior_Win32.h"
//-----------------------------------------------------------------------------------------------------
Orbiter_Win32::Orbiter_Win32(int DeviceID, int PK_DeviceTemplate, string ServerAddress, string sLocalDirectory,  
		bool bLocalMode, int iImageWidth, int iImageHeight, bool bFullScreen)
		: Orbiter(DeviceID, PK_DeviceTemplate, ServerAddress, sLocalDirectory,  
			bLocalMode, iImageWidth, iImageHeight, NULL, bFullScreen)
{
#ifdef ENABLE_MOUSE_BEHAVIOR
	m_pMouseBehavior = new MouseBehavior_Win32(this);
#endif	
}
//-----------------------------------------------------------------------------------------------------
Orbiter_Win32::~Orbiter_Win32()
{

}
//-----------------------------------------------------------------------------------------------------
bool Orbiter_Win32::SelfUpdate()
{
	OrbiterSelfUpdate orbiterSelfUpdate(this);
	return orbiterSelfUpdate.Run();
}
//-----------------------------------------------------------------------------------------------------
bool Orbiter_Win32::WaitForRelativesIfOSD()
{
	//nothing like this under win32 yet
	return true;
}
//-----------------------------------------------------------------------------------------------------
