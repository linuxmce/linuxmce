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
#ifndef PhoneDetection_Simulate_h
#define PhoneDetection_Simulate_h

#include "PhoneDetectionEngine.h"

class PhoneDetection_Simulate : public PhoneDetectionEngine
{
public:

protected:
	// This function needs to be overridden 
	virtual bool ScanningLoop();
};
#endif

