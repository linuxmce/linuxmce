#include "fdio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

void UsageExit(char *Program)
{
	fprintf(stderr, "Usage: %s [--raw|--enc|--hex] --fd <file_descriptor> --data <data_string>\n", Program);
	exit(1);
}

/* I/O functions */

void WriteData(unsigned char *Data, int DataLength)
{
	int Bytes = write(Settings.Fd, Data, DataLength);
	if (Bytes < 0)
	{
		fprintf(stderr, "write error (fd=%d): %s\n", Settings.Fd, strerror(errno));
		exit(2);
	}
}

/* main */

int main(int argc, char *argv[])
{
	ParseParms(argc, argv);

	if (Settings.WriteData == NULL)
		UsageExit(argv[0]);

	unsigned char *Buffer;
	int BufferLength;
	Decode((unsigned char *) Settings.WriteData, strlen(Settings.WriteData), &Buffer, &BufferLength);

	WriteData(Buffer, BufferLength);

	return 0;
}
