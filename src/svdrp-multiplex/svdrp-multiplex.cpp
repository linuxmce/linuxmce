#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/select.h>

#include <string>
#include <vector>
#include <map>
#include <algorithm>
using namespace std;

int create_server_socket()
{
	int sock_srv;
	struct sockaddr_in saddr;

	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(2001);
	saddr.sin_addr.s_addr = INADDR_ANY;

	sock_srv = socket(PF_INET, SOCK_STREAM, 0);
	if (sock_srv == -1)
	{
		perror("srv socket");
		return -1;
	}

	int tmp = 1;
	if (setsockopt(sock_srv, SOL_SOCKET, SO_REUSEADDR, &tmp, sizeof(tmp)) == -1)
	{
		perror("srv setsockopt");
		return -1;
	}
	
	if (bind(sock_srv, (struct sockaddr *) &saddr, sizeof(saddr)) == -1)
	{
		perror("srv bind");
		return -1;
	}
	
	if (listen(sock_srv, 5) == -1)
	{
		perror("srv listen");
		return -1;
	}

	return sock_srv;
}

FILE *connect_svdrp(char *hello_message, size_t hello_message_size)
{
	struct sockaddr_in saddr;
	
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(1999);
	saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int sock_svdrp = socket(PF_INET, SOCK_STREAM, 0);
	if (sock_svdrp == -1)
	{
		perror("svdrp socket");
		return NULL;
	}

	int tmp = 1;
	if (setsockopt(sock_svdrp, SOL_SOCKET, SO_REUSEADDR, &tmp, sizeof(tmp)) == -1)
	{
		perror("svdrp setsockopt");
		return NULL;
	}
	
	while (connect(sock_svdrp, (struct sockaddr *) &saddr, sizeof(saddr)) == -1)
	{
		sleep(1);
	}

	// read the initial hello message
	memset(hello_message, 0, hello_message_size);

	FILE *stdiofile_sock_svdrp = fdopen(sock_svdrp, "r+");
	fgets(hello_message, hello_message_size, stdiofile_sock_svdrp);

	return stdiofile_sock_svdrp;
}

bool bridge_svdrp(FILE *sock_client, FILE *sock_svdrp)
{
	char buffer[1024];

	memset(buffer, 0, 1024);
	if (fgets(buffer, 1024, sock_client) == false)
		return false;

	if (buffer[strlen(buffer) - 1] != '\n')
		return false;

	if (strncasecmp(buffer, "QUIT", 4) == 0)
	{
		const char *quit_message = "221 dcerouter SVDRP multiplexer closing connection\r\n";
		fputs(quit_message, sock_client);
		fflush(sock_client);
		return false;
	}

	printf("Received: %s", buffer); // buffer contains EOL
	
	fputs(buffer, sock_svdrp);
	fflush(sock_svdrp);
	while (1)
	{
		fgets(buffer, 1024, sock_svdrp);
		fputs(buffer, sock_client);
		fflush(sock_client);
		if (buffer[3] != '-')
			break;
	}

	return true;
}

void svdrp_keep_alive(FILE *sock_svdrp)
{
	const char *keep_alive_message = "CHAN\n";
	fputs(keep_alive_message, sock_svdrp);
	fflush(sock_svdrp);

	char buffer[1024];
	fgets(buffer, 1024, sock_svdrp);
}

int main()
{
	signal(SIGPIPE, SIG_IGN);

	vector<FILE *> sock_client;
	size_t i;
	const size_t hello_message_size = 1024;
	char hello_message[hello_message_size];

	int sock_srv = create_server_socket();
	if (sock_srv == -1)
		return 1;
	FILE *sock_svdrp = connect_svdrp(hello_message, hello_message_size);
	if (sock_svdrp == NULL)
		return 1;

	while (true)
	{
		fd_set fdset;
		FD_ZERO(&fdset);
		FD_SET(sock_srv, &fdset);

		int fd_svdrp = fileno(sock_svdrp);
		FD_SET(fd_svdrp, &fdset);

		int sock_max = max(sock_srv, fd_svdrp);
		for (i = 0; i < sock_client.size(); i++)
		{
			int fd_sock_client = fileno(sock_client[i]);
			FD_SET(fd_sock_client, &fdset);
			if (sock_max < fd_sock_client)
				sock_max = fd_sock_client;
		}

		struct timeval timeout;
		timeout.tv_sec = 60;
		timeout.tv_usec = 0;

		int fdcount = select(sock_max + 1, &fdset, NULL, NULL, &timeout);

		if (fdcount < 0)
			return 1;
		else if (fdcount == 0)
		{
			svdrp_keep_alive(sock_svdrp);
			continue;
		}

		if (FD_ISSET(fd_svdrp, &fdset))
		{
			fclose(sock_svdrp);
			sock_svdrp = connect_svdrp(hello_message, hello_message_size);
		}

		if (FD_ISSET(sock_srv, &fdset))
		{
			int fd_sock_newclient = accept(sock_srv, NULL, NULL);
			if (fd_sock_newclient == -1)
			{
				perror("accept");
				return 1;
			}
			FILE *sock_newclient = fdopen(fd_sock_newclient, "r+");
			sock_client.push_back(sock_newclient);
			fputs(hello_message, sock_newclient);
			fflush(sock_newclient);
		}

		vector<FILE *> sock_client_valid;
		for (i = 0; i < sock_client.size(); i++)
		{
			int fd_sock_client = fileno(sock_client[i]);
			if (FD_ISSET(fd_sock_client, &fdset))
			{
				if (!bridge_svdrp(sock_client[i], sock_svdrp))
				{
					fclose(sock_client[i]);
					continue;
				}
			}
			sock_client_valid.push_back(sock_client[i]);
		}
		sock_client = sock_client_valid;
		sock_client_valid.clear();
	}
	
	return 0;
}
