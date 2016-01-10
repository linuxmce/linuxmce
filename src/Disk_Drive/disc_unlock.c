/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include <stdio.h>
#include <dvdread/dvd_reader.h>

int main(int argc, char * argv[])
{
	if (argc != 2)
	{
		printf("Usage: %s <device>\n", argv[0]);
		return 1;
	}
	
	dvd_reader_t * dvd = NULL;

	dvd = DVDOpen(argv[1]);
    if (! dvd)
	{
		fprintf(stderr, "disc_unlock: Failed to open the DVD\n");
		return -1;
	}
	DVDClose(dvd);
	return 0;
}
