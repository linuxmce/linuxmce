
/*

  This is an extremely simple program to grab a whole range of
  registers from a specific chip, via the VIDIOC_INT_G_REGISTER
  command.

*/

/* These are awful hacks because I don't have the patience to figure
   out how to pull in the internal linux headers in the correct
   context. */
typedef unsigned int u32;
typedef unsigned char u8;

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <media/v4l2-common.h>

void dumpByteBuffer(const char *bufPtr,unsigned int tcnt, unsigned long offs)
{
    unsigned int cnt1,cnt2,cnt3;

    printf("          00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
    while (tcnt) {
	cnt1 = offs & 0xf;
	cnt2 = 16 - cnt1;
	if (cnt2 > tcnt) cnt2 = tcnt;
	printf("%08x:",offs - cnt1);
	cnt3 = cnt1 * 3;
	while (cnt3--) fputc(' ',stdout);
	for (cnt3 = 0; cnt3 < cnt2; cnt3++) {
	    printf(" %02x",(unsigned char)bufPtr[cnt3]);
	}
        offs += cnt2;
	tcnt -= cnt2;
	bufPtr += cnt2;
	printf("\n");
    }
}


void fillByteBuffer(int fdc,unsigned long chipId,
		    char *bufPtr,unsigned int cnt,unsigned long offs)
{
    struct v4l2_register req;
    int stat;

    while (cnt) {
	req.i2c_id = chipId;
	req.reg = offs;
        req.val = 0xaa;
	stat = ioctl(fdc,VIDIOC_INT_G_REGISTER,&req);
        if (stat < 0) {
            perror("failed to read register space");
            return;
        }
	*bufPtr = req.val;
	bufPtr++; cnt--; offs++;
    }
}


int main(int argc, char *argv[])
{
    char *fname = argv[1];
    unsigned long chipId = atoi(argv[2]);
    unsigned long startAddr = atoi(argv[3]);
    unsigned long readCnt = atoi(argv[4]);
    char *buffer = malloc(readCnt);
    int fdc;

    fdc = open(fname,O_RDWR);
    if (fdc < 0) {
	perror(fname);
	return 1;
    }

    fillByteBuffer(fdc,chipId,buffer,readCnt,startAddr);
    dumpByteBuffer(buffer,readCnt,startAddr);

    free(buffer);
    close(fdc);
    return 0;
}

