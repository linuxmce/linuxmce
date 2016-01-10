
/*
 *
 *  $Id: decode_eeprom.cpp 950 2006-03-04 19:52:00Z isely $
 *
 *  Copyright (C) 2006 Mike Isely <isely@pobox.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/*

  Mike Isely <isely@pobox.com>

  12-June-2005

  This is a decoder for an FX2-readable eeprom image.  This follows
  the eeprom booting specification from the EZ-USB tech ref for the
  FX2 microcontroller to parse out the contents of the eeprom,
  printing what is found and emitting a file containing the contents
  in a form that can be fed to a disassembler.

 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

struct FileBuffer;
struct SegmentDesc;
struct FileInfo;

struct FileBuffer {
    unsigned char *bufPtr;
    unsigned int bufSize;
    FileBuffer(void);
    ~FileBuffer(void);
    void clean(void);
    int readFile(const char *,FILE *errfp);
};

struct SegmentDesc {
    FileInfo &fileRef;
    SegmentDesc *segNext;
    SegmentDesc *segPrev;
    unsigned int offs;
    unsigned int loadAddr;
    unsigned int cnt;
    int lastFl;
    int okFl;

    SegmentDesc(FileInfo &,unsigned int,FILE *errfp);
    ~SegmentDesc(void);
};

struct FileInfo {
    FileBuffer fileBuffer;
    SegmentDesc *segFirst;
    SegmentDesc *segLast;

    int okFl;
    unsigned short vendId;
    unsigned short prodId;
    unsigned short devId;
    unsigned char cfgByte;
    unsigned int segCnt;

    FileInfo(void);
    ~FileInfo(void);
    void parse(const char *,FILE *errfp);
    int writeSRecFile(const char *fname);
    void clean(void);

    void describe(FILE *);
};


void writeSRecData(FILE *fp,
		   unsigned int offs,
		   unsigned int blockCnt,
		   unsigned char *bufPtr)
{
    unsigned short cnt,idx;
    unsigned char chksum;
    while (blockCnt) {
	cnt = blockCnt <= 32 ? blockCnt : 32;
	chksum = 0;
	fprintf(fp,"S1%02X%04X",
		cnt + 2 + 1,offs);
	chksum += cnt + 2 + 1;
	chksum += offs & 0xffu;
	chksum += (offs >> 8) & 0xffu;
	for (idx = 0; idx < cnt; idx++) {
	    fprintf(fp,"%02X",bufPtr[idx]);
	    chksum += bufPtr[idx];
	}
	chksum = ~chksum;
	fprintf(fp,"%02X\n",chksum);
	bufPtr += cnt;
	blockCnt -= cnt;
	offs += cnt;
    }
}


FileBuffer::FileBuffer(void)
    :bufPtr(0),bufSize(0)
{
    // Intentionally Empty
}


FileBuffer::~FileBuffer(void)
{
    clean();
}


int FileBuffer::readFile(const char *fname,FILE *errfp)
{
    unsigned char buf[65536+1];
    FILE *fp;
    clean();

    fp = fopen(fname,"rb");
    if (!fp) {
	if (errfp) fprintf(errfp,"Failed to open %s for reading\n",fname);
	return 0;
    }

    bufSize = fread(buf,1,sizeof(buf),fp);
    fclose(fp);
    if (bufSize >= sizeof(buf)) {
	if (errfp) fprintf(errfp,"File %s is too big\n",fname);
	return 0; // Too big
    }

    bufPtr = new unsigned char[bufSize];
    memcpy(bufPtr,buf,bufSize);
    return !0;
}


void FileBuffer::clean(void)
{
    if (bufPtr) {
	delete[] bufPtr;
	bufPtr = 0;
    }
    bufSize = 0;
}


FileInfo::FileInfo(void)
    :segFirst(0),segLast(0),
     okFl(0),vendId(0),prodId(0),devId(0),cfgByte(0),
     segCnt(0)
{
    // Intentionally Empty
}


FileInfo::~FileInfo(void)
{
    clean();
}


void FileInfo::clean(void)
{
    while (segFirst) delete segFirst;
    fileBuffer.clean();
}


void FileInfo::parse(const char *fname,FILE *errfp)
{
    unsigned int offs;
    SegmentDesc *seg;
    clean();
    if (!fileBuffer.readFile(fname,errfp)) return;
    unsigned char *buf = fileBuffer.bufPtr;
    if ((buf[0] != 0xc0) && (buf[0] != 0xc2)) {
	if (errfp) fprintf(errfp,"eeprom data not c0 or c2 type\n");
	return;
    }
    vendId = buf[1] | (buf[2] << 8);
    prodId = buf[3] | (buf[4] << 8);
    devId = buf[5] | (buf[6] << 8);
    cfgByte = buf[7];
    if (buf[0] != 0xc2) return;
    offs = 8;
    for (;;) {
	seg = new SegmentDesc(*this,offs,errfp);
	if (!seg->okFl) {
	    if (errfp) fprintf(errfp,"Bad segment encountered, giving up\n");
	    return;
	}
	if (seg->lastFl) break;
	offs += seg->cnt + 4;
    }
    okFl = !0;
}


void FileInfo::describe(FILE *fp)
{
    SegmentDesc *seg;
    if (!okFl) {
	fprintf(fp,"Invalid / improper eeprom data\n");
	return;
    }
    fprintf(fp,"Vendor:Product:Device = %04x:%04x:%04x\n",
	    vendId,prodId,devId);
    fprintf(fp,"cfg Byte: %02x file size=%u\n",cfgByte,fileBuffer.bufSize);
    fprintf(fp,"Data blocks (count=%u):\n",segCnt);
    for (seg = segFirst; seg; seg = seg->segNext) {
	if (!seg->okFl) {
	    fprintf(fp,"  <error>\n");
	    continue;
	}
	fprintf(fp,"  %04x --> %04x (%04x)",
		seg->loadAddr,seg->loadAddr+seg->cnt-1,seg->cnt);
	if (seg->lastFl) fprintf(fp," <last>");
	fprintf(fp,"\n");
    }
}


int FileInfo::writeSRecFile(const char *fname)
{
    SegmentDesc *seg;
    FILE *fp;
    fp = fopen(fname,"w");
    if (!fp) return 0;
    for (seg = segFirst; seg; seg = seg->segNext) {
	if (seg->lastFl) continue;
	writeSRecData(fp,
		      seg->loadAddr,
		      seg->cnt,
		      fileBuffer.bufPtr + seg->offs);
    }
    fclose(fp);
    return !0;
}


SegmentDesc::SegmentDesc(FileInfo &fref,
			 unsigned int _offs,
			 FILE *errfp)
    :fileRef(fref),segNext(0),segPrev(0),
     offs(_offs),loadAddr(0),cnt(0),
     lastFl(0),okFl(0)
{
    fileRef.segCnt++;
    segPrev = fileRef.segLast;
    segNext = 0;
    if (fileRef.segLast) {
	fileRef.segLast->segNext = this;
    } else {
	fileRef.segFirst = this;
    }
    fileRef.segLast = this;
    if (offs >= fileRef.fileBuffer.bufSize) {
	if (errfp) fprintf(errfp,
			   "Segment offset of %u too large (threshold %u)\n",
			   offs,fileRef.fileBuffer.bufSize);
	return;
    }
    unsigned char *buf = fileRef.fileBuffer.bufPtr + offs;
    cnt = buf[1] | (buf[0] << 8);
    lastFl = (cnt & 0x8000);
    if (cnt & 0x6c00) {
	if (errfp) fprintf(errfp,
			   "Bogus bits set in segment count at offset %u\n",
			   offs);
	return;
    }
    cnt &= ~0x8000;
    loadAddr = buf[3] | (buf[2] << 8);
    if (offs + cnt + 4 > fileRef.fileBuffer.bufSize) {
	if (errfp) fprintf(errfp,"Segment size too large"
			   " (offs=%u cnt=%u threshold=%u)\n",
			   offs,cnt,fileRef.fileBuffer.bufSize);
	return;
    }
    offs += 4;
    okFl = !0;
}


SegmentDesc::~SegmentDesc(void)
{
    if (segNext) {
	segNext->segPrev = segPrev;
    } else {
	fileRef.segLast = segPrev;
    }
    if (segPrev) {
	segPrev->segNext = segNext;
    } else {
	fileRef.segFirst = segNext;
    }
    fileRef.segCnt--;
}


int main(int argc,char *argv[])
{
    FileInfo fi;
    fi.parse(argv[1],stderr);
    fi.describe(stdout);
    if (argc > 2) {
	if (!fi.writeSRecFile(argv[2])) {
	    fprintf(stderr,"Failed to write s-records to %s\n",
		    argv[2]);
	}
    }
}
