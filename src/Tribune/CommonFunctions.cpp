/*
 CommonFunctions
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

 
/**
 *
 * @file CommonFunctions.cpp
 * @brief source file for the GlobalConfig, SerializeableStrings classes; it uses the Table, Repository and Database classes
 *
 */
 

#include "PlutoUtils/CommonIncludes.h"	
#include "CommonFunctions.h"
#include <iostream>

namespace Tribune
{
	void NewSection()
	{ 
		cout << endl << "==============================================================================" << endl; 
	}
}

using namespace Tribune;

string int2string(int i)
{
	char s[1000];
	sprintf(s, "%i", i);
	return s;
}
