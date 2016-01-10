/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/cdrom.h>

int main(int argc, const char * argv[])
{

inline int usage()
{
	printf("Usage: cdop <device> {lock|unlock}\n");
	return 1;
}

	int dev;
	int state;

	if (argc != 3)
	{
		return usage();
	}

	if (strcmp(argv[2], "lock") == 0)
		state = 1;
	else if (strcmp(argv[2], "unlock") == 0)
		state = 0;
	else
		return usage();
	
	dev = open(argv[1], O_RDONLY);
	if (dev == -1)
	{
		perror("Failed to open cdrom");
		return 2;
	}
	
	if (ioctl(dev, CDROM_LOCKDOOR, state) == -1)
	{
		perror("Failed to perform operation");
		return 3;
	}
	
	close(dev);

	return 0;
}
