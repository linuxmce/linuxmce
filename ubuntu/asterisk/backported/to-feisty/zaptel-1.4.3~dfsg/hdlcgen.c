#define FAST_HDLC_NEED_TABLES
#include "fasthdlc.h"
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#define RANDOM "/dev/urandom"			/* Not genuinely random */
/* #define RANDOM "/dev/random" */		/* Quite genuinely random */

int myread(int fd, char *buf, int len)
{
	int sofar;
	int res;
	sofar = 0;
	while(sofar < len) {
		res = read(fd, buf + sofar, len - sofar);
		if (res < 0)
			return res;
		sofar += res;
	}
	return sofar;
}

int main(int argc, char *argv[])
{
	unsigned char buf[1024];
	unsigned char outbuf[2048];
	int res;
	int randin;
	int randout;
	int hdlcout;
	int cnt;
	int hdlccnt;
	int x;
	int flags;
	struct fasthdlc_state transmitter;
	
	fasthdlc_precalc();
	
	fasthdlc_init(&transmitter);
	
	randin = open(RANDOM, O_RDONLY);
	if (randin < 0) {
		fprintf(stderr, "Unable to open %s: %s\n", RANDOM, strerror(errno));
		exit(1);
	}
	randout = open("random.raw", O_WRONLY|O_TRUNC|O_CREAT, 0666);
	if (randout < 0) {
		fprintf(stderr, "Unable to open random.raw: %s\n", strerror(errno));
		exit(1);
	}
	hdlcout = open("random.hdlc", O_WRONLY|O_TRUNC|O_CREAT, 0666);
	if (hdlcout < 0) {
		fprintf(stderr, "Unable to open random.hdlc: %s\n", strerror(errno));
		exit(1);
	}
	for (;;) {
		cnt = (rand() % 256) + 4;	/* Read a pseudo-random amount of stuff */
		res = myread(randin, buf, cnt);
		if (res != cnt) {
			fprintf(stderr, "Tried to read %d bytes, but read %d instead\n", cnt, res);
			exit(1);
		}
		res = write(randout, buf, cnt);
		if (res != cnt) {
			fprintf(stderr, "Tried to write %d bytes, but wrote %d instead\n", cnt, res);
			exit(1);
		}
		/* HDLC encode */
		hdlccnt = 0;
		/* Start with a flag */
		fasthdlc_tx_frame(&transmitter);
		if (transmitter.bits >= 8)
			outbuf[hdlccnt++] = fasthdlc_tx_run(&transmitter);
		for (x=0;x<cnt;x++) {
			res = fasthdlc_tx_load(&transmitter, buf[x]);
			if (res < 0) {
				fprintf(stderr, "Unable to load byte :(\n");
				exit(1);
			}
			while(transmitter.bits >= 8)  {
				outbuf[hdlccnt++] = fasthdlc_tx_run(&transmitter);
			}
		}
		flags = (rand() % 4);
		for (x=0;x<flags;x++) {
			if (transmitter.bits < 8)
				fasthdlc_tx_frame(&transmitter);
			else
				fprintf(stderr, "Huh?  Don't need a frame?\n");
			outbuf[hdlccnt++] = fasthdlc_tx_run(&transmitter);
		}
		if (argc > 1)
			printf("Encoded %d byte message with %d bytes of HDLC and %d extra flags\n", cnt, hdlccnt, flags);
		res = write(hdlcout, outbuf, hdlccnt);
		if (res != hdlccnt) {
			fprintf(stderr, "Tried to write %d HDLC bytes, but wrote %d instead\n", cnt, res);
			exit(1);
		}
		
	}
}
