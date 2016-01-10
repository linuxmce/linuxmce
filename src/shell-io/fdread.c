#include "fdio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

void UsageExit(char *Program)
{
	fprintf(stderr, "Usage: %s [--raw|--enc|--hex] [--line [--eol eol_character]] [--stream] [--count max_bytes] --fd <file_descriptor>\n", Program);
	exit(1);
}

/* I/O functions */

void ReadLine(unsigned char **Line, int *LineLength)
{
	*Line = malloc(Settings.MaxBytes);
	memset(*Line, 0, Settings.MaxBytes);

	int Pos = 0;
	while (Pos < Settings.MaxBytes)
	{
		unsigned char Chr;
		int Bytes = read(Settings.Fd, &Chr, 1);

		if (Bytes < 0)
		{
			fprintf(stderr, "read error (fd=%d): %s\n", Settings.Fd, strerror(errno));
			exit(2);
		}
		else if (Bytes > 0)
		{
			(*Line)[Pos] = Chr;
			++Pos;

			if (Chr == Settings.EolSeparator)
				break;
		}
		else /* EOF */
		{
			break;
		}
	}
	*LineLength = Pos;
}

void ReadStream(unsigned char **Data, int *DataLength)
{
	*Data = malloc(Settings.MaxBytes);
	memset(*Data, 0, Settings.MaxBytes);

	int Bytes = read(Settings.Fd, *Data, Settings.MaxBytes);
	if (Bytes < 0)
	{
		fprintf(stderr, "read error (fd=%d): %s\n", Settings.Fd, strerror(errno));
		exit(2);
	}
	else
	{
		*DataLength = Bytes;
	}
}

/* main */

int main(int argc, char *argv[])
{
	ParseParms(argc, argv);

	unsigned char *Buffer;
	int BufferLength = 0;
	switch (Settings.BufferType)
	{
		case BufferType_Stream:
			ReadStream(&Buffer, &BufferLength);
			break;
		case BufferType_Line:
			ReadLine(&Buffer, &BufferLength);
			break;
	}

	unsigned char *EncodedBuffer;
	int EncodedBufferLength;

	Encode(Buffer, BufferLength, &EncodedBuffer, &EncodedBufferLength);

	write(1, EncodedBuffer, EncodedBufferLength);

	return 0;
}
