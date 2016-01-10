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
#ifndef __INCOMING_CALL_NOTIFIER_H__
#define __INCOMING_CALL_NOTIFIER_H__
//---------------------------------------------------------------------------------------------------------
#include <Tapi.h>
//---------------------------------------------------------------------------------------------------------
class IncomingCallNotifier
{
public:
	IncomingCallNotifier();
	~IncomingCallNotifier();

private:

	HLINE		m_hLine;
	HLINEAPP	m_hLineApp;
	DWORD		m_dwNumDevs;

	//helper functions
	DWORD InitializeTAPI ();
	DWORD SelectLine(const HLINEAPP hLineApp, const DWORD dwNumberDevices, const DWORD dwAPIVersionLow, 
		const DWORD dwAPIVersionHigh, const TCHAR* const psTSPLineName);
};
//---------------------------------------------------------------------------------------------------------
#endif