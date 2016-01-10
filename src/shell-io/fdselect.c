#include <sys/select.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	if (argc < 4)
	{
		fprintf(stderr, "Syntax: %s <timeout_sec> <timeout_usec> <fd1> [<fd2> ...]\n", argv[0]);
		return 1;
	}

	fd_set fdset;
	fd_set fdseen;
	FD_ZERO(&fdset);
	FD_ZERO(&fdseen);
	int maxfd = 0;
	int i;
	for (i = 3; i < argc; i++)
	{
		int fd = atoi(argv[i]);
		FD_SET(fd, &fdset);
		if (maxfd < fd)
			maxfd = fd;
	}

	struct timeval timeout;
	timeout.tv_sec = atoi(argv[1]);
	timeout.tv_usec = atoi(argv[2]);

	if (timeout.tv_sec == -1)
		select(maxfd + 1, &fdset, NULL, NULL, NULL);
	else
		select(maxfd + 1, &fdset, NULL, NULL, &timeout);

	for (i = 3; i < argc; i++)
	{
		int fd = atoi(argv[i]);
		if (FD_ISSET(fd, &fdset) && !FD_ISSET(fd, &fdseen))
		{
			printf("%d ", fd);
			FD_SET(fd, &fdseen);
		}
	}
	printf("\n");

	return 0;
}
