/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "inotify_class.h"

#include <unistd.h>

#include <iostream>
using namespace std;

int main(int argc, char * argv[])
{
	inotify obj;

	int wd1 = obj.watch(argv[1], IN_ALL_EVENTS);
	int wd2 = obj.watch("/home", IN_ALL_EVENTS);

	cout << "wd1:" << wd1 <<"; wd2:" << wd2 << endl;
	while (1)
	{
		cpp_inotify_event event = obj.get_event();

		cout << "Event: " << event.tostring() << endl;
	}
}
