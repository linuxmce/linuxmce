/*
$Id: owread.c,v 1.13 2008/09/30 23:43:56 alfille Exp $
     OW -- One-Wire filesystem
    version 0.4 7/2/2003

    Function naming scheme:
    OW -- Generic call to interaface
    LI -- LINK commands
    L1 -- 2480B commands
    FS -- filesystem commands
    UT -- utility functions
    COM - serial port functions
    DS2480 -- DS9097 serial connector

    Written 2003 Paul H Alfille
*/

#include "owshell.h"

/* ---------------------------------------------- */
/* Command line parsing and result generation     */
/* ---------------------------------------------- */
int main(int argc, char *argv[])
{
	int c;
	int rc = -1;

	Setup();
	/* process command line arguments */
	while (1) {
		c = getopt_long(argc, argv, OWLIB_OPT, owopts_long, NULL);
		if (c == -1) {
			break;
		}
		owopt(c, optarg);
	}

	DefaultOwserver();
	Server_detect();

	/* non-option arguments */
	while (optind < argc) {
		rc = ServerRead(argv[optind]);
		++optind;
	}
	if ( rc >= 0 ) {
		errno = 0 ;
		Exit(0) ;
	} else {
		errno = -rc ;
		Exit(1) ;
	}
	return 0;					// never reached
}
