/*
 *	fbmngplay - fb console MNG player.
 *	(c) 2001-2002 by Stefan Reinauer, <stepan@suse.de>
 *  
 *	This program is based on mngplay, part of libmng, written and (C) by
 *	
 *	Ralph Giles <giles@ashlu.bc.ca>
 *
 *	This program my be redistributed under the terms of the
 *	GNU General Public Licence, version 2, or at your preference,
 *	any later version.
 */

#include <stdio.h>
#include "fbmngplay.h"
#include "messages.h"

void usage(char *name)
{
	fprintf(stderr,
		"\nusage: %s [ -x <val> ] [ -y <val> ] [ -a <val> ] [-b] [-v]"
		" [-s] [file.mng [file.mng [...]]]\n", name);
	fprintf(stderr,
		"\n	-x: x coordinate\n" "	-y: y coordinate\n"
		"	-a, --alpha: default alpha channel 1..100\n"
		"	-v, --verbose: verbose mode\n"
		"	-b, --buffered: buffered mode\n"
		"	-s, --signal: wait for SIGUSR1 between animations\n"
		"	-p, --position: dynamically select position\n"
		"	-V, --version: show version and exit\n"
		"	-?, -h, --help: print this help.\n"
		"	-S --start-console: only output animation on console it was started on.\n"
		"       -c: start output animation on specified console.\n\n");
}

void version(void)
{
	fprintf(stderr,
		"fbmngplay v%s, Copyright (C) 2001-2002 Stefan Reinauer <stepan@suse.de>\n\n"
		"fbmngplay comes with ABSOLUTELY NO WARRANTY;\n"
		"This is free software, and you are welcome to redistribute it\n"
		"under certain conditions; Check the GPL for details.\n",
		FBMNGPLAY_VERSION);
}
