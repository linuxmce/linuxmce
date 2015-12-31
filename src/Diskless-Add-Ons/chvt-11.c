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
/* extremely light program for switching to tty11 */
/* inspired by chvt.c */
#include <fcntl.h>

#define VT_ACTIVATE 0x5606
#define VT_WAITACTIVE 0x5607

int main()
{
	int fd;

	fd = open("./dev/console", O_RDWR);
	ioctl(fd, VT_ACTIVATE, 11);
	ioctl(fd, VT_WAITACTIVE, 11);
	return 0;
}
