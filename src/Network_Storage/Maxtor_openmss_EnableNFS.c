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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#define ecposix(x) if ((x) == -1) { perror("Error"); return 1; }
#define ecnull(x) if ((x) == NULL) { perror("Error"); return 1; }
#define BUFFSIZE 1024
#define LAN_SUBST "%l"
#define LAN_SUFFIX ".0/255.255.255.0"

const char * commands[] = {
	"mkdir -p /shares/mss-hdd/Pluto",
	"portmap >/dev/null 2>&1 </dev/null",
	"rpc.mountd >/dev/null 2>&1 </dev/null",
	"rpc.nfsd >/dev/null 2>&1 </dev/null",
	/*"exportfs -io rw,no_root_squash,no_all_squash " LAN_SUBST ":/shares/mss-hdd/Pluto\n",*/
	"echo \"/shares/mss-hdd/Pluto   192.168.80.0/255.255.255.0(rw,sync,wdelay,hide,nocrossmnt,secure,no_root_squash,no_all_squash,subtree_check,secure_locks,acl,mapping=identity,anonuid=-2,anongid=-2)\" >/var/lib/nfs/xtab\n",
	"exit",
	NULL
};

int main(int argc, char * argv[])
{
	int sock;
	struct sockaddr_in saddr;
	FILE * fsock;
	char buffer[BUFFSIZE];
	int cmd_idx = 0;
	char * new_cmd, * IP, * LAN;
	int LAN_length;
	char * cptr;

	if (argc != 2)
	{
		printf("Usage: %s <IP>\n", argv[0]);
		return 1;
	}
	IP = strdup(argv[1]);
	printf("IP: %s\n", IP);
	
	ecnull(cptr = rindex(IP, '.'));
	* cptr = 0;
	
	LAN_length = strlen(IP) + strlen(LAN_SUFFIX) + 1;
	LAN = (char *) malloc(LAN_length);
	snprintf(LAN, LAN_length, "%s%s", IP, LAN_SUFFIX);
	printf("LAN: %s\n", LAN);
	
	* cptr = '.';
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(23); // telnet
	ecposix(inet_aton(IP, &saddr.sin_addr));

	ecposix(sock = socket(PF_INET, SOCK_STREAM, 0));
	ecposix(connect(sock, (struct sockaddr *) &saddr, sizeof(saddr)));

	ecnull(fsock = fdopen(sock, "r+"));

	fprintf(fsock, "%s", "PS1=\"prompt>\n\"\n");
	while (1)
	{
		if (fgets(buffer, sizeof(buffer), fsock) == NULL)
		{
			printf("All done\n");
			break;
		}
		
		printf("Received: %s", buffer);

		if (commands[cmd_idx] != NULL) // reason for this: sometimes we receive a "prompt>" after the last command, which resulted in a Segmentation Fault
		{
			if (strstr(buffer, "prompt>") == buffer)
			{
				printf("Got prompt\n");
				if ((strstr(commands[cmd_idx], LAN_SUBST)) != NULL)
				{
					new_cmd = strdup(commands[cmd_idx]);
					cptr = strstr(new_cmd, LAN_SUBST);
					cptr[1] = 's';
					printf("Sending cmd %d: %s\n", cmd_idx, new_cmd);
					printf(new_cmd, LAN);
					fprintf(fsock, new_cmd, LAN); // not really the safest approach, but...
					free(new_cmd);
				}
				else
				{
					printf("Sending cmd %d: %s\n", cmd_idx, commands[cmd_idx]);
					fprintf(fsock, "%s\n", commands[cmd_idx]);
				}
				
				cmd_idx++;
			}
		}
	}

	fclose(fsock); // also closes sock
	free(IP);
	free(LAN);
	return 0;
}
