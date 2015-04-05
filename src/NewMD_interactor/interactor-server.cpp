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

#include <stdlib.h>
#include <unistd.h>

#include "GetCommandOutput.h"

#include <string>
using namespace std;

#define sDT_Generic_PC_as_MD "28"

int main()
{
	int s, s2;
	struct sockaddr_in saddr;
	int do_quit = 0;
	char buffer[1024], cmd[1024];
	char remoteIP[1024], remoteMAC[1024];
	char remoteDeviceData[1024];
	char remoteDeviceTemplate[1024];
	char remotePNPIdentifier[1024];
	char sDefaultPNPIdentifier[1024];
	int bytes, tmp;

	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(INTERACTOR_PORT);
	saddr.sin_addr.s_addr = INADDR_ANY;

	s = socket(PF_INET, SOCK_STREAM, 0);
	if (s == -1)
	{
		perror("socket");
		return 1;
	}

	tmp = 1;
	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &tmp, sizeof(tmp)) == -1)
	{
		perror("setsockopt");
		return 1;
	}
	
	set_close_on_exec(s);

	if (bind(s, (struct sockaddr *) &saddr, sizeof(saddr)) == -1)
	{
		perror("bind");
		return 1;
	}
	
	if (listen(s, 5) == -1)
	{
		perror("listen");
		return 1;
	}
	
	while (! do_quit)
	{
		s2 = accept(s, NULL, NULL);
		if (s2 == -1)
		{
			perror("accept");
			return 1;
		}

		memset(buffer, 0, 1024);
		while (! do_quit && (bytes = read(s2, buffer, 1023)) > 0)
		{
			printf("Received: %s", buffer); // buffer contains EOL
			buffer[bytes] = 0;
			memset(cmd, 0, 1024);
			sscanf(buffer, "%s", cmd);
			if (strcmp(cmd, "quit") == 0)
				do_quit = 1;
			else if (strcmp(cmd, "newmd") == 0)
			{
				memset(remoteIP, 0, 1024);
				memset(remoteMAC, 0, 1024);
				memset(remoteDeviceData, 0, 1024);
				memset(remoteDeviceTemplate, 0, 1024);

				sscanf(buffer, "%*s %s %s %s %s", remoteIP, remoteMAC, remoteDeviceData, remoteDeviceTemplate);
				printf("Split: IP=%s, MAC=%s, DD=%s, DT=%s\n", remoteIP, remoteMAC, remoteDeviceData, remoteDeviceTemplate);

				char * MDexists_args[] = {"/usr/pluto/bin/NewMD_Exists.sh", remoteMAC, NULL };
				string MD_DevID;
				GetCommandOutput(MDexists_args[0], MDexists_args, &MD_DevID);
				if (MD_DevID.length() != 0)
				{
					printf("Existing MD: %s\n", MD_DevID.c_str());
					// chomp output
					while (*MD_DevID.rbegin() == '\n')
						MD_DevID.resize(MD_DevID.length() - 1);
					char * args[] = { "/usr/pluto/bin/New_PnP_MD.sh", remoteIP, remoteMAC, (char *) MD_DevID.c_str(), NULL };
					GetCommandOutput(args[0], args, NULL);
				}
				else
				{
					printf("New MD\n");
					char * args[] = { "/usr/pluto/bin/MessageSend", "localhost", "0", "-1001", "2", "65",
						"28", remoteIP, "5", remoteMAC, "52", "3", "53", "5",
						"49", strlen(remoteDeviceTemplate) == 0 ? (char *) sDT_Generic_PC_as_MD : remoteDeviceTemplate,
						"55", remoteDeviceData,
						NULL };
					GetCommandOutput(args[0], args, NULL);
				}
			}
			else if (strcmp(cmd, "select") == 0)
			{
				memset(remoteIP, 0, 1024);
				memset(remoteMAC, 0, 1024);
				memset(remoteDeviceData, 0, 1024);
				memset(remotePNPIdentifier, 0, 1024);

				sscanf(buffer, "%*s %s %s %s %s", remoteIP, remoteMAC, remoteDeviceData, remotePNPIdentifier);
				printf("Split: IP=%s, MAC=%s, DD=%s, PNP_ID=%s\n", remoteIP, remoteMAC, remoteDeviceData, remotePNPIdentifier);

				char * MDexists_args[] = {"/usr/pluto/bin/NewMD_Exists.sh", remoteMAC, NULL };
				string MD_DevID;
				GetCommandOutput(MDexists_args[0], MDexists_args, &MD_DevID);
				if (MD_DevID.length() != 0)
				{
					printf("Existing MD: %s\n", MD_DevID.c_str());
					// chomp output
					while (*MD_DevID.rbegin() == '\n')
						MD_DevID.resize(MD_DevID.length() - 1);
					char * args[] = { "/usr/pluto/bin/New_PnP_MD.sh", remoteIP, remoteMAC, (char *) MD_DevID.c_str(), NULL };
					GetCommandOutput(args[0], args, NULL);
				}
				else
				{
					printf("New Interactor Device Selection\n");
					char * args[] = { "/usr/pluto/bin/MessageSend", "localhost", "0", "-1001", "2", "65",
						"28", remoteIP, "5", remoteMAC, "52", "3", "53", "6",
						"51", strlen(remotePNPIdentifier) == 0 ? sDefaultPNPIdentifier : remotePNPIdentifier,
						"55", remoteDeviceData,
						NULL };
					GetCommandOutput(args[0], args, NULL);
				}
			}
			else if (strcmp(cmd, "rooms") == 0)
			{
				string sOutput;
				char * args[] = { "/usr/pluto/bin/NewMD_interactor_helper.sh", "--rooms", NULL };
				GetCommandOutput(args[0], args, &sOutput);
				write(s2, sOutput.c_str(), sOutput.size());
			}
		}
		
		close(s2);
	}
	
	return 0;
}
