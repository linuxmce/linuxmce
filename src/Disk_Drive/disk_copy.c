/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#define _LARGEFILE64_SOURCE

#ifdef DEBUG
#define Dprintf printf
#else
#define Dprintf(...)
#endif

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <stdio.h>

#define BLOCK_SIZE (1 * 1024 * 1024)
#define STR_SIZE 4095

//TODO: replace snprintf + write combination with printf override
// printf does a fstat() first and a write() later, depending if it's a terminal (instantly) or other kind of file (buffering)

ssize_t sector_by_sector_read(int fd, void *buf, size_t count) // prototype compatible with read(2)
{
#define SECT_SIZE 512
	char sector[SECT_SIZE];
	size_t read_bytes = 0;

	while (read_bytes < count)
	{
		off64_t pos_before_read = lseek64(fd, 0, SEEK_CUR);
		Dprintf("pos_before_read: %d\n", pos_before_read);
		int bytes = read(fd, sector, SECT_SIZE);
		Dprintf("bytes: %d\n", bytes);
		if (bytes == 0)
			break;
		else if (bytes < 0)
		{
			if (errno == EIO)
			{
				Dprintf("err: IO\n");
				memset(sector, 0, SECT_SIZE);
				lseek64(fd, pos_before_read + SECT_SIZE, SEEK_SET);
				bytes = SECT_SIZE;
			}
			else
			{
				Dprintf("err: non-IO\n");
				return -1;
			}
		}

		if (read_bytes + bytes > count)
			bytes = count - read_bytes;
		memcpy(buf, sector, bytes);
		buf += bytes;
		read_bytes += bytes;
		Dprintf("read_bytes: %d, count: %d\n", read_bytes, count);
	}

	return read_bytes;
}

char buffer[BLOCK_SIZE];
char str[STR_SIZE + 1];
int main(int argc, const char * argv[])
{
	int f, g;
	off64_t size, pos = 0, bytes, bytes2;
	float progress = 0, last_progress = 0;
	int error;

	if (argc != 3)
	{
		printf("Usage: disk_copy <src> <dest>\n");
		return 1;
	}
	
	f = open(argv[1], O_RDONLY | O_LARGEFILE);
	if (f == -1)
	{
		printf("Failed to open '%s': %s\n", argv[1], strerror(errno));
		return 1;
	}

	g = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC | O_LARGEFILE);
	if (g == -1)
	{
		printf("Failed to open '%s': %s\n", argv[2], strerror(errno));
		return 1;
	}

	size = lseek64(f, 0, SEEK_END);
	lseek64(f, 0, SEEK_SET);

	printf("Disc size: %Lu\n", size);
	fflush(stdout);
	
	int reported_errors = 0;
	while (1)
	{
		off64_t pos_before_read = lseek64(f, 0, SEEK_CUR);
		bytes = read(f, buffer, BLOCK_SIZE);
		if (bytes == 0)
			break;
		else if (bytes < 0)
		{
			if (errno == EIO)
			{
				if (reported_errors == 0)
				{
					printf("Error: I/O error while reading\n");
					fflush(stdout);
					reported_errors = 1;
				}
				lseek64(f, pos_before_read, SEEK_SET);
				bytes = sector_by_sector_read(f, buffer, BLOCK_SIZE);
			}
			else
				break;
		}

		bytes2 = write(g, buffer, bytes);
		if (bytes2 < bytes)
		{
			printf("Error writing to '%s': %s\n", argv[2], strerror(errno));
			return 1;
		}
		
		pos = lseek64(f, 0, SEEK_CUR);
		progress = pos * 1.0 / size * 100;
		if (last_progress != progress)
		{
			printf("Position: %Lu; Progress: %.2f%%\n", pos, progress);
			fflush(stdout);
			last_progress = progress;
		}
	}
	error = errno;

	close(g);
	close(f);

	if (bytes < 0)
	{
		printf("Error reading '%s': %s\n", argv[1], strerror(error));
		return 1;
	}
	
	printf("Done\n");
	return 0;
}
