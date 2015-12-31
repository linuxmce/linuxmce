#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

/*
 * Globals used when receiving a signal to close and open the output file (helps in log rotation)
 */
FILE * f = NULL;
const char * filename = NULL;
int append = 0;

void open_file()
{
	if (filename == NULL)
	{
		fprintf(stderr, "INTERNAL ERROR: filename == NULL");
		exit(1);
	}

	if (append)
		f = fopen(filename, "ab");
	else
		f = fopen(filename, "wb");

	if (!f)
	{
		fprintf(stderr, "ERROR: Failed to open output file: %s\n", strerror(errno));
		exit(1);
	}

	setvbuf(f, NULL, _IONBF, 0);
}

void signal_handler(int signal)
{
	if (f != NULL)
	{
		fclose(f);
		f = NULL;
	}
	open_file();
}

void usage()
{
	fprintf(stderr, "Usage: %s [-a] file\n", "tee-pluto");
}

int main(int argc, char * argv[])
{
	int i;
	int optc;

	while ((optc = getopt(argc, argv, "a")) != -1)
	{
		switch (optc)
		{
			case 'a':
				append = 1;
				break;

			default:
				usage();
				return 1;
		}
    }

	if (argc - optind < 1)
	{
		usage();
		return 1;
	}

	filename = argv[optind];

	setvbuf(stdout, NULL, _IONBF, 0);

	open_file();
	signal(SIGUSR1, signal_handler);

	ssize_t bytes_read;
	char buffer[BUFSIZ];
	while (1)
	{
		bytes_read = read(0, buffer, sizeof(buffer));
#ifdef EINTR
		if (bytes_read < 0 && errno == EINTR)
			continue;
#endif
		if (bytes_read <= 0)
			break;

		if (fwrite(buffer, 1, bytes_read, f) != bytes_read)
		{
			fprintf(stderr, "ERROR: Failed to write to output file: %s\n", strerror(errno));
			return 1;
		}
		if (fwrite(buffer, 1, bytes_read, stdout) != bytes_read)
		{
			fprintf(stderr, "ERROR: Failed to write to stdout: %s\n", strerror(errno));
			return 1;
		}
	}

	fclose(f);
	return 0;
}
