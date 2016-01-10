/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

/**
 * @file LinuxSerialUSB.h
 Header file for TranslateSerialUSB.
 */
#ifndef LinuxSerialUSB_h
#define LinuxSerialUSB_h

#include "FileUtils.h"
#include "DCE/Logger.h"

using namespace DCE;
string TranslateSerialUSB(string sInput,string sIPAddress="");

#endif


