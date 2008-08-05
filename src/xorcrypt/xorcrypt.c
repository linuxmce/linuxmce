#include <stdio.h>
#include <dirent.h>

int xorFILE(char *filename, char *key)
{

	FILE *infile  = fopen( filename, "rb" );
	if ( infile != NULL ) {
		size_t chrCOUNT, x, y = 0;
		unsigned char buffer[ BUFSIZ ];
		do {
			chrCOUNT = fread(buffer, sizeof *buffer, sizeof buffer, infile);
			for ( x = 0; x < chrCOUNT; ++x )
			{
				buffer[x] ^= key[y++];
				if ( key[y] == '\0' )
					{
						y = 0;
					}
			}
			//write(stdout, buffer, chrCOUNT);
			fwrite(buffer, sizeof *buffer, chrCOUNT, stdout);
		} while ( chrCOUNT == sizeof buffer );
	fclose(infile);
}
return 0;
}

int main(int argc, char *const *argv)
{
	char fname[256];
	struct dirent *dstruct;
	DIR *dir;

	if (argc == 3)
	{
		xorFILE(argv[1], argv[2]);
	} else {
		printf("Usage: %s <filename> <key>\n", argv[0]);
	}
return 0;
}
