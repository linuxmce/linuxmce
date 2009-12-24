
/*

  $Id: fix_comma_bullshit.c 1904 2008-03-08 05:27:18Z isely $

  Copyright (C) 2008 Mike Isely <isely@pobox.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


  The SOLE purpose of this program is to scan and fix all occurrences
  within a C or C++ source file of the use of a comma without trailing
  whitespace.  Even though the accepted, documented, kernel
  CodingStyle doesn't require this, there is apparently an unwritten
  rule requiring whitespace after every comma.  Recent introduction of
  a style-checking script in the kernel source tree enforces this
  unwritten rule.  Hand-editing thousands of lines of code to fix this
  would be a nightmare, so an attempt at automation is being done with
  this program.  Feed it a file and it will do an in-place edit,
  fixing all cases of a comma without a trailing space, I hope.  I am
  doing this with a C program, because stateful parsing of lines with
  quoted strings and multiline commenting is too much of a pain
  otherwise in a language like Perl (where I usually write text
  processing hacks)...

  This whole topic is utter bullshit, but I will attempt to comply.
  Stupid stuff like this just sucks the air out of any enjoyment from
  helping the open source community.

*/


#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>

int scanStream(FILE *fin,FILE *fout)
{
    char buf1[400];
    char buf2[2*sizeof(buf1)];
    unsigned int b1cnt;
    unsigned int b2cnt;
    unsigned int pos;
    unsigned int lineNum = 0;
    int ch;
    int mode = 0;

    while (fgets(buf1,sizeof(buf1),fin) != NULL) {
	lineNum++;
	b1cnt = 0; while (buf1[b1cnt]) b1cnt++;
	if (!b1cnt) {
	    fprintf(stderr,"Bad line %u\n",lineNum);
	    return 1;
	}
	if (buf1[b1cnt-1] != '\n') {
	    fprintf(stderr,"Truncated line %u\n",lineNum);
	    return 1;
	}
	b1cnt--;
	b2cnt = 0;

	for (pos = 0; pos < b1cnt; ) {
	    switch (mode) {
	    case 0:
		if (pos+1 >= b1cnt) {
		    buf2[b2cnt++] = buf1[pos++];
		    break;
		}
		if ((buf1[pos] == ',') &&
		    !isspace(buf1[pos+1])) {
		    buf2[b2cnt++] = buf1[pos++];
		    buf2[b2cnt++] = ' ';
		    break;
		}
		if (buf1[pos] == '"') {
		    buf2[b2cnt++] = buf1[pos++];
		    mode = 2;
		    break;
		}
		if (buf1[pos] == '/') {
		    if (buf1[pos+1] == '*') {
			buf2[b2cnt++] = buf1[pos++];
			buf2[b2cnt++] = buf1[pos++];
			mode = 1;
			break;
		    }
		    if (buf1[pos+1] == '/') {
			while (pos < b1cnt) {
			    buf2[b2cnt++] = buf1[pos++];
			}
			break;
		    }
		}
		buf2[b2cnt++] = buf1[pos++];
		break;
	    case 1: /* Inside a comment */
		if (pos+1 >= b1cnt) {
		    buf2[b2cnt++] = buf1[pos++];
		    break;
		}
		if ((buf1[pos] == '*') &&
		    (buf1[pos+1] == '/')) {
		    buf2[b2cnt++] = buf1[pos++];
		    buf2[b2cnt++] = buf1[pos++];
		    mode = 0;
		    break;
		}
		buf2[b2cnt++] = buf1[pos++];
		break;
	    case 2: /* Inside a quoted string */
		if ((buf1[pos] == '\\') && (pos + 1 < b1cnt)) {
		    buf2[b2cnt++] = buf1[pos++];
		    buf2[b2cnt++] = buf1[pos++];
		    break;
		}
		if (buf1[pos] == '"') {
		    buf2[b2cnt++] = buf1[pos++];
		    mode = 0;
		    break;
		}
		buf2[b2cnt++] = buf1[pos++];
		break;
	    }
	}
	buf2[b2cnt++] = '\n';
	fwrite(buf2,b2cnt,1,fout);
	if (mode == 2) {
	    fprintf(stderr,"Run-on quoted string starting at line %u\n",
		    lineNum);
	    return 1;
	}
    }
    return 0;
}


int processFile(const char *fname)
{
    FILE *fin,*fout;
    char tname[256];
    unsigned int len = strlen(fname);
    int stat;
    if (len > (sizeof(tname)-1-4)) {
	fprintf(stderr,"File name is too long\n");
	return;
    }
    strcpy(tname,fname);
    strcat(tname,".tmp");
    fin = fopen(fname,"r");
    if (!fin) {
	perror(fname);
	return 1;
    }
    fout = fopen(tname,"w");
    if (!fout) {
	perror(tname);
	fclose(fin);
	return 1;
    }
    stat = scanStream(fin,fout);
    fclose(fin);
    fclose(fout);
    unlink(fname);
    link(tname,fname);
    unlink(tname);
    if (stat) {
	fprintf(stderr,"Trouble processing file %s\n",fname);
    }
    return stat;
}


int main(int argc,const char *argv[])
{
    unsigned int idx;
    for (idx = 1; idx < argc; idx++) {
	processFile(argv[idx]);
    }
}



