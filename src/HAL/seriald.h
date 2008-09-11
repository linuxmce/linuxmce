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
//
// C++ Interface: seriald
//
// Description:
//
//
// Author: Eugen Constantinescu <eugen.c@plutohome.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SERIALD_H
#define SERIALD_H

#include <vector>
#include <string>

namespace DCE
{
	class HAL;
}

using namespace DCE;

/**
The PnP daemon for serial connection.
 
	@author Eugen Constantinescu <eugen.c@plutohome.com>
*/
class SerialD
{
	public:
		
		SerialD();
	
		~SerialD();
	
		/***/
		static void* startUp(void * HAL);
		
		/***/
		static bool shutDown();
		
	private:
		
		static DCE::HAL * halDevice;
		
		static std::vector<std::string> serialDevices;
		
		static bool running;

		static bool stoprunning;
};

#endif
