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
#include <sys/time.h>
#include <sys/select.h>
#include <unistd.h>

int main()
{
	const struct timeval tv_val = { .tv_sec = 10, .tv_usec = 0 };
	struct timeval tv = tv_val;
	fd_set fd;
	char buf[1024];

	FD_ZERO(&fd);
	FD_SET(0, &fd);
	tv.tv_sec = 30; // 1st timeout waits for SSH/TCP negociation too
	while (select(1, &fd, 0, 0, &tv) > 0)
	{
		if (! FD_ISSET(0, &fd))
			break;
		if (read(0, buf, 1024) <= 0)
			break;
		FD_ZERO(&fd);
		FD_SET(0, &fd);
		tv = tv_val;
	}

	return 0;
}
