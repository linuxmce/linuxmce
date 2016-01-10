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
#include <stdio.h>
#include <sys/time.h>
#include <sys/select.h>
#include <unistd.h>
#include <signal.h>

int main()
{
	const struct timeval tv_val = { .tv_sec = 5, .tv_usec = 500000 };
	struct timeval tv = tv_val;
	fd_set fd;
	char buf[1024];
	int selret, timeouts = 0;

	FD_ZERO(&fd);
	FD_SET(0, &fd);
	
	printf("This is the LinuxMCE Remote Assistance shell\n");
	printf("It displays a dot every ~5.5 seconds and keeps the connection alive\n");
	while(timeouts < 3)
	{
		printf(".");
		fflush(stdout);

		selret = select(1, &fd, 0, 0, &tv);
		if (selret < 0)
		{
			break;
		}
		else if (selret > 0)
		{
			if (read(0, buf, 1024) <= 0)
			{
				break;
			}
			timeouts = 0;
			select(0, 0, 0, 0, &tv);
		}
		else 
		{
			timeouts ++;
		}
		FD_ZERO(&fd);
		FD_SET(0, &fd);
		tv = tv_val;
	}

	printf("\nThe other end failed to transmit the keep-alive character. Exiting\n");
	kill(getppid(), SIGKILL);
	return 0;
}
