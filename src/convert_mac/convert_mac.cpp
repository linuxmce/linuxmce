/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//#include <unistd.h>
#include <math.h>

#include <iostream>
//#include <strstream>
#include <fstream>
#include <cstdlib>
#include <string>
using namespace std;

#include "BD/PhoneDevice.h"
#include "DCE/Logger.h"

using namespace DCE; 

int main(int argc, char *argv[])
{
	if (argc<2)
	{
		cout << "convert_mac [Mac Address as X:X:X:X:X:X]|[Mac as large integer]" << endl;
		exit(0);
	}
	
	string Mac = argv[1];
	if( Mac.find(':')!=string::npos )
	{
		PhoneDevice pd("",Mac,0);
		cout << pd.m_iMacAddress << endl;
	}
	else
	{
		PhoneDevice pd("",atof(Mac.c_str()),0);
		cout << pd.m_sMacAddress << endl;
	}
}

