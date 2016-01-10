#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dvdnav/dvdnav.h>
#include <dvdread/dvd_reader.h>


int main(int argc, char **argv) {
    if (argc<3) {
	printf("Usage: dvdcssHelper (-d|-f) dvdpath\nwhere dvdpath is the path to DVD device or image\n"
		"-d - dump all keys for disk\n-f - just print DVDCSS folder for disk\n");
	return 1;
    }

    char *szDVDPath = NULL;
    int iMode = 0;	/* 1 - print folder, 2 - dumpkeys */

    if ( strcmp("-f", argv[1])==0 ) {
	iMode = 1;
    }
    else
	if ( strcmp("-d", argv[1])==0 ) {
	    iMode = 2;
	}
	else {
	    printf("Error: Unknown switch %s\n", argv[1]);
	    return 1;
	}

    szDVDPath = strdup(argv[2]);

    if (iMode == 1)
    {
      setenv("DVDCSS_VERBOSE", "2", 1);

      dvd_reader_t *pDVD = DVDOpen(szDVDPath);
      if (!pDVD) {
        printf("Error: failed to open DVD\n");
        return 1;
      }

      DVDClose(pDVD);
    }

    if (iMode == 2)
    {
      setenv("DVDREAD_VERBOSE", "2", 1);

      dvdnav_t *pHandle;
      dvdnav_status_t status = dvdnav_open(&pHandle, szDVDPath);
      if ( status == DVDNAV_STATUS_OK ) {
        dvdnav_close(pHandle);
      }
      else {
        printf("Error: failed to open DVD\n");
        return 1;
      }
    }

    return 0;
}
