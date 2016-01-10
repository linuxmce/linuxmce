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
#include <strstream>
#include <fstream>
#include <cstdlib>
#include <string>
using namespace std;

#include "DCE/Logger.h"
#include "SerializeClass/ShapesColors.h"

using namespace DCE; 

int main(int argc, char *argv[])
{
cout << argc << endl;
	if (argc!=2 && argc!=5)
	{
		cout << "convert_color [color as 32-bit Argb]|[r g b a]" << endl;
		exit(0);
	}

	if( argc==2 )
	{
		PlutoColor plutoColor(atoi(argv[1]));
		cout << "Color " << plutoColor.AsARGB() << " = R: " << plutoColor.R() << " G: " << plutoColor.G() << " B: " << plutoColor.B() << " A: " << plutoColor.A() << endl;
	}
	else
	{
		PlutoColor plutoColor(atoi(argv[1]),atoi(argv[2]),atoi(argv[3]),atoi(argv[4]));
		cout << "Color " << plutoColor.AsARGB() << " = R: " << plutoColor.R() << " G: " << plutoColor.G() << " B: " << plutoColor.B() << " A: " << plutoColor.A() << endl;
	}
}

