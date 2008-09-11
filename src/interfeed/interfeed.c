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

/*
 * This application reads standard input continuously.
 * If a TCP client is connected, it writes what it read from stdin to it.
 * It there's no client connected, it discards the input.
 * It accepts just one client, and when a new client connects it disconnects the currently connected one.
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/select.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>

// set close-on-exec flag
void set_close_on_exec(int sock)
{
	long sock_flags;

	sock_flags = fcntl(sock, F_GETFD);
	sock_flags |= FD_CLOEXEC;
	fcntl(sock, F_SETFD, sock_flags);
}

// ./interfeed <port>
int main(int argc, char * argv[])
{
	if (argc != 2)
	{
		printf("Usage: %s <port>\n", argv[0]);
		return 1;
	}

	int port = atoi(argv[1]);
	if (port < 1024 || port > 65535)
	{
		printf("Error: Use a port between 1024 and 65535\n");
		return 1;
	}

	signal(SIGPIPE, SIG_IGN);

	int s = -1, s2 = -1;
	struct sockaddr_in saddr_server;
	char buffer[1024];
	int bytes;

	saddr_server.sin_family = AF_INET;
	saddr_server.sin_port = htons(port);
	saddr_server.sin_addr.s_addr = INADDR_ANY;

	// Start interactor server on client for commands
	s = socket(PF_INET, SOCK_STREAM, 0);
	if (s == -1)
	{
		perror("socket2");
		return 1;
	}

	set_close_on_exec(s);

	if (bind(s, (struct sockaddr *) &saddr_server, sizeof(saddr_server)) == -1)
	{
		perror("bind");
		return 1;
	}
	
	if (listen(s, 5) == -1)
	{
		perror("listen");
		return 1;
	}
	
	while (1)
	{
		fd_set fdset;
		FD_ZERO(&fdset);
		FD_SET(s, &fdset);
		FD_SET(0, &fdset);

		int ret;
		if ((ret = select(s + 1, &fdset, NULL, NULL, NULL)) < 0)
			break;

		if (FD_ISSET(s, &fdset))
		{
			if (s2 != -1)
				close(s2);
			s2 = accept(s, NULL, NULL);
			if (s2 != -1)
				set_close_on_exec(s2);
		}
		
		if (FD_ISSET(0, &fdset))
		{
			bytes = read(0, buffer, 1024);
			if (bytes <= 0)
				break;
			if (s2 != -1 && write(s2, buffer, bytes) != bytes)
			{
				close(s2);
				s2 = -1;
			}
		}
	}
	
	return 0;
}
