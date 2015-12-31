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
#include "common.h"

// ./interactor <Gateway> <IP> <MAC>
int main(int argc, char * argv[])
{
	int s, s2;
	struct sockaddr_in saddr_client, saddr_server;
	int do_reboot = 0;
	char buffer[1024], cmd[1024];
	int bytes, tmp;
	const char * Gateway, * myIP, * myMAC;

	saddr_server.sin_family = AF_INET;
	saddr_server.sin_port = htons(INTERACTOR_PORT);
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
	
	while (! do_reboot)
	{
		s2 = accept(s, NULL, NULL);
		if (s2 == -1)
		{
			perror("accept");
			return 1;
		}
		set_close_on_exec(s2);

		while (! do_reboot && (bytes = read(s2, buffer, 1023)) > 0)
		{
			buffer[bytes] = 0;
			memset(cmd, 0, 1024);
			sscanf(buffer, "%s", cmd);
			if (strcmp(cmd, "reboot") == 0)
				do_reboot = 1;
			else if (strcmp(cmd, "msg") == 0)
				printf("%s", buffer+4);
			else if (strcmp(cmd, "shell") == 0)
			{
				printf("%s\n", "Running a shell");
				system("/bin/sh");
				printf("\n%s\n", "Shell exited");
			}
		}
		
		close(s2);
	}
	reboot(RB_AUTOBOOT);
	
	return 0;
}
