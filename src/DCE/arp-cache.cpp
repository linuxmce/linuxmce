/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU Lesser General Public License for more details.

*/
#include <string>
using namespace std;

#include <stdio.h>

/* Inspired from net-tools arp.c */
string arpcache_MACfromIP(string IP)
{
	FILE * f;
	char buffer[1024];

	char ip[100];
	int type, flags;
	char hwa[100];
	char mask[100];
	char dev[100];

	if (IP == "")
	{
		// empty request
		return "";
	}
	
	f = fopen("/proc/net/arp", "r");
	if (f == NULL)
	{
		// failed to open file
		return "";
	}

	if (fgets(buffer, sizeof(buffer), f) != NULL) // skip header
	{
		while (fgets(buffer, sizeof(buffer), f) != NULL)
		{
			int num = sscanf(buffer, "%s 0x%x 0x%x %100s %100s %100s\n", ip, &type, &flags, hwa, mask, dev);
			if (num < 4)
				break;

			// check if its what we're looking for
			if (IP == ip && (flags & 0x02) == 0x02) // TODO: replace 0x02 with flag mnemonics and make sure there aren't any other combinations
			{
				fclose(f);
				return hwa;
			}
		}
	}
	
	fclose(f);

	// if we get here, we have nothing to show
	return "";
}
