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
#ifndef NEWMD_INTERACTOR_COMMON_H
#define NEWMD_INTERACTOR_COMMON_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/reboot.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define INTERACTOR_PORT 7238

// set close-on-exec flag
void set_close_on_exec(int sock)
{
	long sock_flags;

	sock_flags = fcntl(sock, F_GETFD);
	sock_flags |= FD_CLOEXEC;
	fcntl(sock, F_SETFD, sock_flags);
}

char * chomp(char * buffer)
{
	if (buffer == NULL)
		return NULL;

	if (buffer[strlen(buffer) - 1] == '\n')
		buffer[strlen(buffer) - 1] = 0;
	return buffer;
}
#endif
