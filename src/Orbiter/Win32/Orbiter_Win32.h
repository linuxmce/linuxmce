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
#ifndef __ORBITER_WIN32_H__
#define __ORBITER_WIN32_H__
//-----------------------------------------------------------------------------------------------------
#include "Orbiter.h"
using namespace DCE;
//-----------------------------------------------------------------------------------------------------
namespace DCE
{
	class Orbiter_Win32 : public Orbiter
	{
	public:

		Orbiter_Win32(int DeviceID, int PK_DeviceTemplate, string ServerAddress, string sLocalDirectory,  
			bool bLocalMode, int iImageWidth, int iImageHeight, bool bFullScreen);

		~Orbiter_Win32();	

		bool SelfUpdate();

		bool WaitForRelativesIfOSD();
	};
}
//-----------------------------------------------------------------------------------------------------
#endif __ORBITER_WIN32_H__
