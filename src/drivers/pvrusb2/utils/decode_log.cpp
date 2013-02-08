
/*
 *
 *  $Id: decode_log.cpp 2016 2008-05-27 04:40:56Z isely $
 *
 *  Copyright (C) 2005 Mike Isely <isely@pobox.com>
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

  This is a decompiler for USB transactions logged between an
  FX2-driven device and its driver.  The expected input format is a
  distilled version of the output of the usbsnoop program which can be
  found at:

      http://benoit.papillault.free.fr/usbsnoop/

  The output of that program is too verbose to be handled by this
  decompiler, so instead pump that data through the usbreplayprep.pl
  perl script, which should be nearby somewhere.  Send the usbsnoop
  data into the script's stdin, and then send that script's stdout
  into the stdin of this program.  The stdout of this program will be
  the decompiled result.

  To compile:  g++ -o decode_log decode_log.cpp

  Though this is a C++ program, it requires no special support or
  other libraries beyond what normally comes with the compiler in
  order to build properly.

  What this decompiler attempts to do is find related USB transactions
  and attempt to discern a higher level meaning from what is found.
  This process is recursively repeated, resulting in successively
  higher level interpretation, until no more patterns can be found.
  The output of the decompiler is normally just the highest level
  interpretation of the constructs found in the snoop data.  Most of
  the time high level commands are only found from adjacent low-level
  operations, however the decompiler can detect and detangle some
  forms of interleaved transactions (e.g. a video output transaction
  in the middle of an I2C operation).

  There are few command options at all for the decompiler.  However,
  in reality there are some internal tweaks you can make to the source
  code which affects the level of decompiling and the verbosity of the
  output.  I'm just too lazy to turn these into command options - but
  if someone would like to contribute a patch that enables these as
  run-time options, then feel free.  This is really a developer's
  tool, so I haven't been too concerned about making it "user-proof".
  Anyway, the tweaks you can do are:

    1. Search through the source code for occurrences of
       "::subEnableFl".  There will only be just a few.  Each
       occurrence is a boolean that enables recursive output for the
       corresponding high level construct.  What this means is that
       rather than just printing the high level construct, the
       elements which compose that construct will also be printed.
       Said another way, for example if you set
       EncoderCommand::subEnableFl to a non-zero value, then every
       time an encoder command is printed to the output, the pieces
       which compose that encoder command will also be printed (and
       indented to separate it from the top level output).  The
       process can be done for each possible command type.  Normally
       all these flags are left to zero since there isn't any reason
       to look "under" a high level command unless there's some
       question as to its correctness (a question I've asked myself a
       lot)...

    2. Near the bottom of this source file is a table called
       "parseTransactionFuncs".  This defines a series of function
       pointers which each attempt to identify a particular type of
       high level command.  For example, "tryEncoderCommand" will
       attempt to identify high level encoder commands.  If you want
       to eliminate the identification of a particular type of command
       / transaction, just comment out the corresponding row from this
       table.

  A few words should be said about the output format:

    1. There is a sparse column of numbers down the left side of the
       output.  These are the line numbers from within the
       decompiler's input where the particular operation has been
       found to start.

    2. Consecutive identical operations are identified and coalesced
       into a single operation.  This is noted in the column next to
       the line number, with a decimal number that has a leading "x".
       This is meant to indicate a repetition count.  For example:

         670. x36 msp3400  read [11.007e] --> 0ffe

       This indicates that the driver polled the msp3400's status 36
       times in a row (starting at line 670 in the distilled log
       input).

    3. Consecutive commands which are naturally sequenced are also
       identified and coalesced.  For example, 128 sets of 2 byte
       eeprom reads to consecutive addresses will show up in the
       output as a single 256 byte block of data.  Video data
       transactions and firmware I/O are also similarly collapsed
       down.

    4. I tried to make the output format as terse as possible while
       still keeping it readable.  I probably wasn't very successful.


 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define ASZ(x) (sizeof(x)/sizeof((x)[0]))

#define DEFARR(x) x,ASZ(x)

class OutputFormatter;

/*------------------------------------------------------------------------*/

unsigned int firmwareCount = 0;
const char *saveFirmwareName = 0;
const char *saveEepromName = 0;


/*------------------------------------------------------------------------*/

class IndentScope {
    friend class OutputFormatter;
public:
    IndentScope(OutputFormatter &,
		unsigned int preIndent = 0,
		const char *sepStr = 0,
		unsigned int postIndent = 0);
    IndentScope(OutputFormatter &,
		const char *sepStr);
    ~IndentScope(void);
    void setSepStr(const char *);
    void setPreIndent(unsigned int);
    void setPostIndent(unsigned int);
    unsigned int getPreIndent(void) const;
    unsigned int getPostIndent(void) const;
private:
    unsigned int preIndent;
    unsigned int postIndent;
    const char *sepStr;
    void attach(OutputFormatter *);
    IndentScope *pNext;
    IndentScope *pPrev;
    OutputFormatter *pHead;
};

class OutputFormatter {
    friend class IndentScope;
public:
    OutputFormatter(FILE *);
    ~OutputFormatter(void);
    FILE *getFP(void) const;

    void printf(const char *fmt, ...);

    void startLine(void);
    void restartLine(void);
    void endLine(void);

private:
    void writePrefix(void);
    IndentScope *pFirst;
    IndentScope *pLast;
    int activeFl;
    FILE *fp;
};


void IndentScope::attach(OutputFormatter *fp)
{
    if (pHead) {
	if (pNext) {
	    pNext->pPrev = pPrev;
	} else {
	    pHead->pLast = pPrev;
	}
	if (pPrev) {
	    pPrev->pNext = pNext;
	} else {
	    pHead->pFirst = pNext;
	}
	pHead = 0;
    }
    if (fp) {
	pHead = fp;
	if (pHead->pLast) {
	    pHead->pLast->pNext = this;
	} else {
	    pHead->pFirst = this;
	}
	pNext = 0;
	pPrev = pHead->pLast;
	pHead->pLast = this;
    }
}


inline IndentScope::~IndentScope(void)
{
    attach(0);
}


IndentScope::IndentScope(OutputFormatter &fmt,
			 unsigned int _preIndent,
			 const char *_sepStr,
			 unsigned int _postIndent)
    :preIndent(_preIndent),
     postIndent(_postIndent),
     sepStr(_sepStr),
     pNext(0),pPrev(0),pHead(0)
{
    attach(&fmt);
}


IndentScope::IndentScope(OutputFormatter &fmt,
			 const char *_sepStr)
    :preIndent(0),
     postIndent(0),
     sepStr(_sepStr),
     pNext(0),pPrev(0),pHead(0)
{
    attach(&fmt);
}


inline void IndentScope::setSepStr(const char *_sepStr)
{
    sepStr = _sepStr;
}


inline unsigned int IndentScope::getPreIndent(void) const
{
    return preIndent;
}


inline unsigned int IndentScope::getPostIndent(void) const
{
    return postIndent;
}


inline void IndentScope::setPreIndent(unsigned int _preIndent)
{
    preIndent = _preIndent;
}


inline void IndentScope::setPostIndent(unsigned int _postIndent)
{
    postIndent = _postIndent;
}


inline FILE *OutputFormatter::getFP(void) const
{
    return fp;
}


void OutputFormatter::printf(const char *fmt, ...)
{
    va_list ap;
    va_start(ap,fmt);
    vfprintf(fp,fmt,ap);
    va_end(ap);
}


void OutputFormatter::writePrefix(void)
{
    unsigned int cnt;
    IndentScope *pp;
    for (pp = pFirst; pp; pp = pp->pNext) {
	cnt = pp->preIndent;
	while (cnt--) fputc(' ',fp);
	if (pp->sepStr) {
	    fprintf(fp,"%s",pp->sepStr);
	}
	cnt = pp->postIndent;
	while (cnt--) fputc(' ',fp);
    }
}


void OutputFormatter::startLine(void)
{
    if (activeFl) return;
    writePrefix();
    activeFl = !0;
}


void OutputFormatter::restartLine(void)
{
    if (activeFl) endLine();
    startLine();
}


void OutputFormatter::endLine(void)
{
    if (!activeFl) return;
    fputc('\n',fp);
    activeFl = 0;
}


OutputFormatter::OutputFormatter(FILE *_fp)
    :pFirst(0),pLast(0),activeFl(0),fp(_fp)
{
    // Intentionally Empty
}


OutputFormatter::~OutputFormatter(void)
{
    endLine();
    while (pLast) pLast->attach(0);
}


/*------------------------------------------------------------------------*/

class BlockBuf {
public:
    BlockBuf(void);
    BlockBuf(const unsigned char *,unsigned int);
    BlockBuf(const BlockBuf &);
    ~BlockBuf(void);
    void assign(const unsigned char *,unsigned int);
    void assign(const BlockBuf &);
    unsigned char get8(unsigned int offset) const;
    unsigned short get16le(unsigned int offset) const;
    unsigned long get32le(unsigned int offset) const;
    unsigned short get16be(unsigned int offset) const;
    unsigned long get32be(unsigned int offset) const;
    void getData(unsigned char *,unsigned int length,
		 unsigned int offset) const;
    unsigned int getCount(void) const;
    int getAlloc(void) const;
    const unsigned char *getPtr(void) const;
    void superDump(OutputFormatter &fmt,
                   unsigned int itemSize,
                   unsigned int itemCnt,
                   unsigned int itemsPerRow,
                   int beFl = 0,
                   unsigned int byteOffs = 0,
                   unsigned int byteSkipCnt = 0,
                   unsigned int itemBias = 0,
		   const char *pref = 0) const;
    void inlineSuperDump(OutputFormatter &fmt,
			 unsigned int lThreshold,
			 unsigned int itemSize,
			 unsigned int itemCnt,
			 int beFl = 0,
			 unsigned int byteOffs = 0,
			 unsigned int skipByteCnt = 0,
			 const char *pref = 0) const;
    void dump(OutputFormatter &fmt,const char *pref = 0) const;
    void dump(OutputFormatter &fmt,unsigned int cnt,
	      const char *pref = 0) const;
    void inlineDump(OutputFormatter &fmt,unsigned int lThreshold,
		    const char *pref = 0) const;
    void inlineDump(OutputFormatter &fmt,unsigned int lThreshold,
		    unsigned int cnt,
		    const char *pref = 0) const;
    void dump32le(OutputFormatter &fmt,const char *pref = 0) const;
    void dump32le(OutputFormatter &fmt,unsigned int cnt,
		  const char *pref = 0) const;
    void dump32be(OutputFormatter &fmt,const char *pref = 0) const;
    void dump32be(OutputFormatter &fmt,unsigned int cnt,
		  const char *pref = 0) const;
    void dump32(OutputFormatter &fmt,unsigned int cnt,int beFl,
		const char *pref = 0) const;
    void print32(OutputFormatter &fmt,unsigned int offs,int beFl) const;
private:
    const unsigned char *ptr;
    unsigned int cnt;
};


void BlockBuf::inlineSuperDump(OutputFormatter &fmt,
			       unsigned int lThreshold,
			       unsigned int itemSize,
			       unsigned int itemCnt,
			       int beFl,
			       unsigned int byteOffs,
			       unsigned int skipByteCnt,
			       const char *pref) const
{
    const unsigned char *bptr = ptr;
    unsigned int bcnt = cnt;
    unsigned int stride = skipByteCnt;
    unsigned int itemIdx;
    unsigned int bn;
    unsigned int offs;
    if (itemSize < 1) {
        itemSize = 1;
    } else if (itemSize > 8) {
        itemSize = 8;
    }
    stride += itemSize;
    if (bptr && (byteOffs < cnt)) {
        bptr += byteOffs;
        bcnt -= byteOffs;
    } else {
        bptr = 0;
        bcnt = 0;
    }

    if (pref) fmt.printf("%s",pref);
    if (itemCnt && (itemCnt <= lThreshold)) {
	if (itemCnt) {
	    fmt.printf("{data:");
	    for (itemIdx = 0; itemIdx < itemCnt; itemIdx++) {
		fmt.printf(" ");
		for (bn = 0; bn < itemSize; bn++) {
		    offs = (itemIdx * stride);
		    if (beFl) {
			offs += bn;
		    } else {
			offs += itemSize - (bn+1);
		    }
		    if ((offs >= bcnt) || !bptr) {
			fmt.printf("??");
		    } else {
			fmt.printf("%02x",bptr[offs]);
		    }
		}
	    }
	    fmt.printf("}");
	}
    } else {
	fmt.printf("[cnt=%02x]",itemCnt);
    }
}

void BlockBuf::superDump(OutputFormatter &fmt,
                         unsigned int itemSize,
                         unsigned int itemCnt,
                         unsigned int itemsPerRow,
                         int beFl,
                         unsigned int byteOffs,
                         unsigned int skipByteCnt,
                         unsigned int itemBias,
			 const char *pref) const
{
    const unsigned char *bptr = ptr;
    unsigned int bcnt = cnt;
    unsigned int stride = skipByteCnt;
    unsigned int itemIdx;
    unsigned int bn;
    unsigned int offs;
    unsigned int itemCCnt;
    unsigned int width = 2;
    if (itemSize < 1) {
        itemSize = 1;
    } else if (itemSize > 8) {
        itemSize = 8;
    }
    stride += itemSize;
    if (bptr && (byteOffs < cnt)) {
        bptr += byteOffs;
        bcnt -= byteOffs;
    } else {
        bptr = 0;
        bcnt = 0;
    }
    itemIdx = itemCnt + itemBias;
    if (itemIdx > (1 << 8)) {
	width = 4;
	if (itemIdx > (1 << 16)) {
	    width = 6;
	    if (itemIdx > (1 << 24)) {
		width = 8;
	    }
	}
    }

    if (!itemsPerRow) itemsPerRow = 1;

    itemCCnt = 0;
    for (itemIdx = 0; itemIdx < itemCnt; itemIdx++) {
        if (!itemCCnt) {
            fmt.startLine();
	    if (pref) fmt.printf("%s",pref);
            fmt.printf("%0*x:",width,itemIdx+itemBias);
        }
        fmt.printf(" ");
        for (bn = 0; bn < itemSize; bn++) {
            offs = (itemIdx * stride);
            if (beFl) {
                offs += bn;
            } else {
                offs += itemSize - (bn+1);
            }
            if ((offs >= bcnt) || !bptr) {
                fmt.printf("??");
            } else {
                fmt.printf("%02x",bptr[offs]);
            }
        }
        itemCCnt++;
        if (itemCCnt >= itemsPerRow) {
            fmt.endLine();
            itemCCnt = 0;
        }
    }
    fmt.endLine();
}


void BlockBuf::inlineDump(OutputFormatter &fmt,
			  unsigned int lThreshold,
			  const char *pref) const
{
    inlineSuperDump(fmt,lThreshold,1,cnt,0,0,0,pref);
}


void BlockBuf::dump(OutputFormatter &fmt,const char *pref) const
{
    superDump(fmt,1,cnt,16,0,0,0,0,pref);
}


void BlockBuf::dump32le(OutputFormatter &fmt,unsigned int lcnt,
			const char *pref) const
{
    superDump(fmt,4,lcnt,8,0,0,0,0,pref);
}


void BlockBuf::dump32be(OutputFormatter &fmt,unsigned int lcnt,
			const char *pref) const
{
    superDump(fmt,4,lcnt,8,!0,0,0,0,pref);
}


void BlockBuf::dump32le(OutputFormatter &fmt,
			const char *pref) const
{
    dump32le(fmt,(cnt >> 2) | (cnt & 0x03 ? 1 : 0),pref);
}


void BlockBuf::dump32be(OutputFormatter &fmt,
			const char *pref) const
{
    dump32be(fmt,(cnt >> 2) | (cnt & 0x03 ? 1 : 0),pref);
}


void BlockBuf::dump(OutputFormatter &fmt,unsigned int dcnt,
		    const char *pref) const
{
    superDump(fmt,1,dcnt,16,0,0,0,0,pref);
}


void BlockBuf::inlineDump(OutputFormatter &fmt,
			  unsigned int lThreshold,
			  unsigned int dcnt,
			  const char *pref) const
{
    inlineSuperDump(fmt,lThreshold,1,dcnt,0,0,0,pref);
}


void BlockBuf::dump32(OutputFormatter &fmt,unsigned int dcnt,int beFl,
		      const char *pref) const
{
    superDump(fmt,4,dcnt,8,beFl,0,0,0,pref);
}


void BlockBuf::print32(OutputFormatter &fmt,unsigned int offs,int beFl) const
{
    unsigned int bn,offs2;
    for (bn = 0; bn < 4; bn++) {
	offs2 = offs;
	if (beFl) {
	    offs2 += bn;
	} else {
	    offs2 += (3-bn);
	}
	if ((offs2 >= cnt) || !ptr) {
	    fmt.printf("??");
	} else {
	    fmt.printf("%02x",get8(offs2));
	}
    }
}


BlockBuf::BlockBuf(void)
    :ptr(0),cnt(0)
{
    // Intentionally Empty
}


BlockBuf::BlockBuf(const unsigned char *_ptr,unsigned int _cnt)
    :ptr(_ptr),cnt(_cnt)
{
    // Intentionally Empty
}


BlockBuf::BlockBuf(const BlockBuf &src)
    :ptr(src.ptr),cnt(src.cnt)
{
    // Intentionally Empty
}


BlockBuf::~BlockBuf(void)
{
    // Intentionally Empty
}


void BlockBuf::assign(const unsigned char *_ptr,unsigned int _cnt)
{
    ptr = _ptr;
    cnt = _cnt;
}


void BlockBuf::assign(const BlockBuf &src)
{
    ptr = src.ptr;
    cnt = src.cnt;
}


inline unsigned int BlockBuf::getCount(void) const
{
    return cnt;
}


inline int BlockBuf::getAlloc(void) const
{
    return ptr != 0;
}


inline const unsigned char *BlockBuf::getPtr(void) const
{
    return ptr;
}


unsigned char BlockBuf::get8(unsigned int offset) const
{
    if (!ptr) return 0;
    if (offset+1 > cnt) return 0;
    return ptr[offset];
}


unsigned short BlockBuf::get16le(unsigned int offset) const
{
    unsigned short d1,d2;
    if (!ptr) return 0;
    if (offset+2 > cnt) return 0;
    d1 = ptr[offset];
    d2 = ptr[offset+1];
    return d1 | (d2 << 8);
}


unsigned short BlockBuf::get16be(unsigned int offset) const
{
    unsigned short d1,d2;
    if (!ptr) return 0;
    if (offset+2 > cnt) return 0;
    d2 = ptr[offset];
    d1 = ptr[offset+1];
    return d1 | (d2 << 8);
}


unsigned long BlockBuf::get32le(unsigned int offset) const
{
    unsigned long d1,d2,d3,d4;
    if (!ptr) return 0;
    if (offset+4 > cnt) return 0;
    d1 = ptr[offset];
    d2 = ptr[offset+1];
    d3 = ptr[offset+2];
    d4 = ptr[offset+3];
    return d1 | (d2 << 8) | (d3 << 16) | (d4 << 24);
}


unsigned long BlockBuf::get32be(unsigned int offset) const
{
    unsigned long d1,d2,d3,d4;
    if (!ptr) return 0;
    if (offset+4 > cnt) return 0;
    d4 = ptr[offset];
    d3 = ptr[offset+1];
    d2 = ptr[offset+2];
    d1 = ptr[offset+3];
    return d1 | (d2 << 8) | (d3 << 16) | (d4 << 24);
}


void BlockBuf::getData(unsigned char *dptr,unsigned int length,
		       unsigned int offset) const
{
    if (!ptr) return;
    memset(dptr,0,length);
    if (offset >= cnt) return;
    if (offset + length > cnt) length = cnt - offset;
    if (length) memcpy(dptr,ptr + offset,length);
}


/*------------------------------------------------------------------------*/

struct RegisterDef;
struct BitfieldDef;
struct RegisterTable;

struct RegisterDef {
    const char *regName;
    unsigned long addr;
    const BitfieldDef *bitfieldDefs;
    unsigned int bitfieldCount;
    unsigned long rMask;
    unsigned long rPat;
};

struct BitfieldDef {
    const char *fieldName;
    unsigned char offs,len;
    void (*formatter)(OutputFormatter &,unsigned long data,unsigned long dw);
};

struct RegisterTable {
    unsigned char addrWidth;
    unsigned char dataWidth;
    unsigned int regNameWidth;
    const RegisterDef *regDefs;
    unsigned int regCount;
};

void formatBitfieldMask(OutputFormatter &fmt,
			unsigned long data,
			unsigned long dw,
			const char *oneName,
			const char *zeroName)
{
    unsigned int idx;
    for (idx = 0; idx < dw; idx++) {
	fmt.printf("%s%s",
		   (idx ? ":" : ""),
		   ((data >> idx) & 1) ? oneName : zeroName);
    }
}

void formatBitfieldTrueFalse(OutputFormatter &fmt,
			     unsigned long data,
			     unsigned long dw)
{
    formatBitfieldMask(fmt,data,dw,"true","false");
}

void formatBitfieldEnable(OutputFormatter &fmt,
			  unsigned long data,
			  unsigned long dw)
{
    formatBitfieldMask(fmt,data,dw,"enabled","disabled");
}

void formatBitfieldOnOff(OutputFormatter &fmt,
			 unsigned long data,
			 unsigned long dw)
{
    formatBitfieldMask(fmt,data,dw,"on","off");
}

void formatBitfieldEnum(OutputFormatter &fmt,
			unsigned long data,
			unsigned long dw,
			const char **namePtr,
			unsigned int nameCnt)
{
    data &= ((1 << dw) - 1);
    if (data >= nameCnt) {
	fmt.printf("0x%x",data);
    } else {
	fmt.printf("%s (0x%x)",namePtr[data],data);
    }
}

#define CREATE_BITFIELD_ENUM(funcName,values) \
void funcName(OutputFormatter &fmt,\
	      unsigned long data,\
	      unsigned long dw)\
{\
    formatBitfieldEnum(fmt,data,dw,DEFARR(values));\
}


void printBitsAction(OutputFormatter &fmt,const RegisterDef *regdp,
		     unsigned int dw,unsigned long data,int readFl = 0)
{
    unsigned int idx;
    unsigned int fdw;
    unsigned long bdat;
    const BitfieldDef *bfdp;

    if (!regdp) return;

    IndentScope iScope(fmt,4);

    if ((data ^ regdp->rPat) & regdp->rMask) {
	fmt.startLine();
	fmt.printf(" reserved: exp=%0*x got=%0*x",
		   dw,regdp->rPat & regdp->rMask,
		   dw,data & regdp->rMask);
	fmt.endLine();
    }

    if (!regdp->bitfieldCount) return;

    for (idx = 0; idx < regdp->bitfieldCount; idx++) {
	bfdp = regdp->bitfieldDefs+idx;
	bdat = data >> bfdp->offs;
	bdat &= (1 << bfdp->len) - 1;
	fmt.startLine();
	fmt.printf("%s %s ",
		   bfdp->fieldName,
		   (readFl ? "-->" : "<--"));
	fdw = (bfdp->len / 4) + ((bfdp->len % 4) ? 1 : 0);
	if (bfdp->formatter) {
	    bfdp->formatter(fmt,bdat,fdw);
	} else if (bfdp->len == 1) {
	    fmt.printf("%0*x",fdw,bdat);
	} else {
	    fmt.printf("0x%0*x",fdw,bdat);
	}
	fmt.endLine();
    }
}


/*------------------------------------------------------------------------*/

class TransactionList;
class Transaction {
    friend class TransactionList;
public:
    Transaction(void);
    virtual ~Transaction(void);
    virtual void describe(OutputFormatter &fmt) const;
    virtual void subDescribe(OutputFormatter &) const;
    virtual int isSame(const Transaction *) const;
    virtual unsigned int getLineNum(void) const = 0;

    void fullDescribe(OutputFormatter &) const;

    Transaction *tGetNext(void) const;
    Transaction *tGetPrev(void) const;
    TransactionList *tGetHead(void) const;

    void tRemove(void);
    void tInsertAfter(Transaction *);
    void tInsertBefore(Transaction *);
private:
    Transaction *tNext;
    Transaction *tPrev;
    TransactionList *tHead;
};


class TransactionList : public Transaction {
    friend class Transaction;
public:
    TransactionList(void);
    ~TransactionList(void);

    void tAppend(Transaction *);
    void tPrepend(Transaction *);
    Transaction *tGetFirst(void) const;
    Transaction *tGetLast(void) const;
    unsigned int tGetCount(void) const;
    void subDescribe(OutputFormatter &) const;
    void describeWithLineNum(OutputFormatter &) const;
    unsigned int getLineNum(void) const;
private:
    Transaction *tFirst;
    Transaction *tLast;
    unsigned int tCount;
};


inline Transaction *Transaction::tGetNext(void) const
{
    return tNext;
}


inline Transaction *Transaction::tGetPrev(void) const
{
    return tPrev;
}


inline TransactionList *Transaction::tGetHead(void) const
{
    return tHead;
}


void Transaction::tRemove(void)
{
    if (!tHead) return;
    if (tNext) {
	tNext->tPrev = tPrev;
    } else {
	tHead->tLast = tPrev;
    }
    if (tPrev) {
	tPrev->tNext = tNext;
    } else {
	tHead->tFirst = tNext;
    }
    (tHead->tCount)--;
    tHead = 0;
    tNext = 0;
    tPrev = 0;
}


void Transaction::tInsertAfter(Transaction *tp)
{
    if (tp == this) return;
    if (!tp->tHead) return;
    tRemove();
    tPrev = tp;
    tNext = tp->tNext;
    tHead = tp->tHead;
    (tHead->tCount)++;
    if (tp->tNext) {
	tp->tNext->tPrev = this;
    } else {
	tp->tHead->tLast = this;
    }
    tp->tNext = this;
}


void Transaction::tInsertBefore(Transaction *tp)
{
    if (tp == this) return;
    if (!tp->tHead) return;
    tRemove();
    tNext = tp;
    tPrev = tp->tPrev;
    tHead = tp->tHead;
    (tHead->tCount)++;
    if (tp->tPrev) {
	tp->tPrev->tNext = this;
    } else {
	tp->tHead->tFirst = this;
    }
    tp->tPrev = this;
}


int Transaction::isSame(const Transaction *) const
{
    return 0;
}


void Transaction::describe(OutputFormatter &) const
{
    // Intentionally Empty
}


void Transaction::subDescribe(OutputFormatter &) const
{
    // Intentionally Empty
}


void Transaction::fullDescribe(OutputFormatter &fmt) const
{
    describe(fmt);
    subDescribe(fmt);
}


Transaction::Transaction(void)
    :tNext(0),tPrev(0),tHead(0)
{
    // Intentionally Empty
}


Transaction::~Transaction(void)
{
    tRemove();
}


Transaction *TransactionList::tGetFirst(void) const
{
    return tFirst;
}


Transaction *TransactionList::tGetLast(void) const
{
    return tLast;
}


unsigned int TransactionList::tGetCount(void) const
{
    return tCount;
}


TransactionList::TransactionList(void)
    :tFirst(0),tLast(0),tCount(0)
{
    // Intentionally Empty
}


TransactionList::~TransactionList(void)
{
    while (tFirst) delete tFirst;
}


unsigned int TransactionList::getLineNum(void) const
{
    if (!tFirst) return 0;
    return tFirst->getLineNum();
}


void TransactionList::subDescribe(OutputFormatter &fmt) const
{
    IndentScope iScope(fmt," | ");
    Transaction *tp;
    for (tp = tFirst; tp; tp = tp->tNext) {
	tp->fullDescribe(fmt);
    }
}


void TransactionList::tAppend(Transaction *tp)
{
    tp->tRemove();
    tp->tPrev = tLast;
    tp->tNext = 0;
    tp->tHead = this;
    tCount++;
    if (tLast) {
	tLast->tNext = tp;
    } else {
	tFirst = tp;
    }
    tLast = tp;
}


void TransactionList::tPrepend(Transaction *tp)
{
    tp->tRemove();
    tp->tNext = tFirst;
    tp->tPrev = 0;
    tp->tHead = this;
    tCount++;
    if (tFirst) {
	tFirst->tPrev = tp;
    } else {
	tLast = tp;
    }
    tFirst = tp;
}


/*------------------------------------------------------------------------*/

class IndentTransactionScope : public IndentScope {
public:
    IndentTransactionScope(OutputFormatter &,Transaction *);
private:
};


IndentTransactionScope::IndentTransactionScope(OutputFormatter &_fmt,
                                               Transaction *tp)
    :IndentScope(_fmt)
{
    if (tp) {
        _fmt.restartLine();
        _fmt.printf("%6u. ",tp->getLineNum());
	setPreIndent(getPreIndent()+8);
    } else {
	setPreIndent(getPreIndent()+8);
        _fmt.restartLine();
    }
}


/*------------------------------------------------------------------------*/

class TransactionRepeat : public TransactionList {
public:
    static TransactionRepeat *parse(Transaction *);
    void describe(OutputFormatter &fmt) const;
    ~TransactionRepeat(void);
    void subDescribe(OutputFormatter &) const;
    static int subEnableFl;
private:
    TransactionRepeat(void);
};

int TransactionRepeat::subEnableFl = 0;


void TransactionRepeat::subDescribe(OutputFormatter &fmt) const
{
    if (!subEnableFl) return;
    TransactionList::subDescribe(fmt);
}


TransactionRepeat::TransactionRepeat(void)
{
    // Intentionally Empty
}


TransactionRepeat::~TransactionRepeat(void)
{
    // Intentionally Empty
}


void TransactionRepeat::describe(OutputFormatter &fmt) const
{
    Transaction *tp;

    unsigned int spc = 0;
    {
        char buf[15];
        sprintf(buf,"x%d ",tGetCount());
        spc = strlen(buf);
        fmt.printf("%s",buf);
    }

    fmt.startLine();
    fmt.printf("repeat=%d",tGetCount());
    tp = tGetFirst();
    if (tp) tp->describe(fmt);
    fmt.endLine();
}


TransactionRepeat *TransactionRepeat::parse(Transaction *tp1)
{
    TransactionRepeat *bp = 0;
    Transaction *tp2,*tp3;
    tp2 = tp1->tGetNext();
    for (;;) {
	if (!tp2) break;
        if (!tp1->isSame(tp2)) break;
        if (!bp) {
            bp = new TransactionRepeat;
            bp->tInsertBefore(tp1);
            bp->tAppend(tp1);
        }
        tp3 = tp2->tGetNext();
        bp->tAppend(tp2);
        tp1 = tp2;
        tp2 = tp3;
    }
    return bp;
}


/*------------------------------------------------------------------------*/

void TransactionList::describeWithLineNum(OutputFormatter &fmt) const
{
    Transaction *tp1,*tp2;
    TransactionRepeat *tr;
    for (tp1 = tFirst; tp1; tp1 = tp1->tNext) {
        IndentScope iScope(fmt);
        tr = dynamic_cast<TransactionRepeat *>(tp1);
        fmt.startLine();
        if (tr) {
            tp2 = tr->tGetFirst();
            fmt.printf("%6u. x%-3d",tp1->getLineNum(),tr->tGetCount());
        } else {
            tp2 = tp1;
            fmt.printf("%6u.     ",tp1->getLineNum());
        }
	iScope.setPreIndent(iScope.getPreIndent()+12);
	tp2->fullDescribe(fmt);
    }

}


/*------------------------------------------------------------------------*/

class DataBlock : public Transaction {
public:
    DataBlock(unsigned lineNum,int readFl,
	      unsigned int _blockCount,unsigned int _endpoint);
    ~DataBlock(void);

    void putData(const unsigned char *,unsigned int length,
		 unsigned int offset);
    const BlockBuf &getBlockBuf(void) const;

    void describe(OutputFormatter &fmt) const;
    int isSame(const Transaction *) const;

    unsigned int getEndpoint(void) const;
    int getReadFl(void) const;
    unsigned int getLineNum(void) const;
private:
    unsigned char *data;
    int readFl;
    unsigned int blockCount;
    unsigned int endpoint;
    unsigned int lineNum;
    BlockBuf bb;
};


DataBlock::DataBlock(unsigned int _lineNum,
		     int _readFl,
		     unsigned int _blockCount,
		     unsigned int _endpoint)
    :data(0),readFl(_readFl),
     blockCount(_blockCount),endpoint(_endpoint),lineNum(_lineNum)
{
    bb.assign(data,blockCount);
}


DataBlock::~DataBlock(void)
{
    if (data) delete[] data;
}

int DataBlock::isSame(const Transaction *tp) const
{
    const DataBlock *bp = dynamic_cast<const DataBlock *>(tp);
    if (!bp) return 0;
    if (bp->endpoint != endpoint) return 0;
    if (bp->blockCount != blockCount) return 0;
    if ((!readFl) != !(bp->readFl)) return 0;
    if ((data != 0) != (bp->data != 0)) return 0;
    if (blockCount && data) {
        if (memcmp(bp->data,data,blockCount)) return 0;
    }
    return !0;
}


void DataBlock::describe(OutputFormatter &fmt) const
{
    const BlockBuf &bb = getBlockBuf();
    unsigned int byteCount = bb.getCount();
    int readFl = getReadFl();
    fmt.startLine();
    fmt.printf("%c%x %x",
               (readFl ? 'r' : 'w'),
               getEndpoint(),
               byteCount);
    fmt.endLine();
    if (!data) byteCount = 0;
    if (!byteCount) return;
    {
        IndentScope iScope(fmt,4);
        bb.dump(fmt);
    }
}

void DataBlock::putData(const unsigned char *ptr,unsigned int length,
			unsigned int offset)
{
    if (!data) {
	data = new unsigned char[blockCount];
	memset(data,0,blockCount);
	bb.assign(data,blockCount);
    }

    if (offset >= blockCount) return;
    if (offset + length > blockCount) length = blockCount - offset;
    if (length) memcpy(data + offset,ptr,length);
}


const BlockBuf &DataBlock::getBlockBuf(void) const
{
    return bb;
}


inline unsigned int DataBlock::getEndpoint(void) const
{
    return endpoint;
}


inline int DataBlock::getReadFl(void) const
{
    return readFl;
}


inline unsigned int DataBlock::getLineNum(void) const
{
    return lineNum;
}


/*------------------------------------------------------------------------*/

struct DataReaderState {
    FILE *fp;
    unsigned int lineNum;
    TransactionList &list;
    DataReaderState(FILE *,TransactionList &);
    DataBlock *dbCur;
    int parse1(void);
    int parseAll(void);
};


static unsigned long parseHex(const char *buf,unsigned int &gotCnt)
{
    int ch;
    unsigned long val = 0;
    const char *b2 = buf;
    const char *b3;
    while (*b2 && (*b2 == ' ')) b2++;
    b3 = b2;
    while ((ch = *b2) != 0) {
	if (!ch) break;
	if ((ch >= '0') && (ch <= '9')) {
	    val <<= 4;
	    val |= (ch - '0');
	} else if ((ch >= 'a') && (ch <= 'f')) {
	    val <<= 4;
	    val |= (ch - 'a' + 10);
	} else {
	    break;
	}
	b2++;
    }
    if (b2 == b3) {
	gotCnt = 0;
    } else {
	gotCnt = b2 - buf;
    }
    return val;
}


void indicateError(const char *buf,unsigned int pos)
{
    unsigned int cnt = pos;
    fprintf(stderr,"%s\n",buf);
    while (cnt > 1) {
	fputc(' ',stderr);
	cnt--;
    }
    if (cnt == 1) fputc('-',stderr);
    fprintf(stderr,"^- (col %d)\n",pos);
}


int DataReaderState::parse1(void)
{
    char buf[512];
    char *bp;
    unsigned int cnt;

    // Fetch prefix line
    lineNum++;
    if (fgets(buf,sizeof(buf)-50,fp) != buf) return 8;
//    fprintf(stderr,"Line %u\n",lineNum);
    bp = buf;
    while (*bp) {
	if (*bp == '\n') {
	    *bp = 0;
	    break;
	}
	bp++;
    }

    bp = buf;
    if (*bp == ' ') {
	// It is data
	unsigned char bytes[64];
	unsigned int offs;
	unsigned int byteCnt;
	offs = parseHex(bp,cnt);
	if (!cnt) {
	    fprintf(stderr,"Failed to read offset at line %u\n",lineNum);
	    indicateError(buf,bp-buf);
	    return 1;
	}
	bp += cnt;
	if (*bp != ':') {
	    fprintf(stderr,"Missing colon at line %u\n",lineNum);
	    indicateError(buf,bp-buf);
	    return 2;
	}
	bp++;
	byteCnt = 0;
	while (*bp && (*bp != '\n')) {
	    if (byteCnt >= sizeof(bytes)) {
		fprintf(stderr,"Too much data on line %u\n",lineNum);
		indicateError(buf,bp-buf);
		return 3;
	    }
	    bytes[byteCnt] = parseHex(bp,cnt);
	    if (!cnt) {
		fprintf(stderr,"Non-hex data on line %u\n",lineNum);
		indicateError(buf,bp-buf);
		return 4;
	    }
	    bp += cnt;
	    byteCnt++;
	}

	if (byteCnt && dbCur) {
	    dbCur->putData(bytes,byteCnt,offs);
	}
    } else if ((*bp == 'r') || (*bp == 'w')) {
	// It is a command
	int readFl;
	unsigned int blockCount,endpoint;

	readFl = (*bp == 'r');
	bp++;
	endpoint = parseHex(bp,cnt);
	if (!cnt) {
	    fprintf(stderr,"Failed to read endpoint id at line %u\n",lineNum);
	    indicateError(buf,bp-buf);
	    return 5;
	}
	bp += cnt;
	blockCount = parseHex(bp,cnt);
	if (!cnt) {
	    fprintf(stderr,"Failed to read byte count at line %u\n",lineNum);
	    indicateError(buf,bp-buf);
	    return 6;
	}

	dbCur = new DataBlock(lineNum,readFl,blockCount,endpoint);
	list.tAppend(dbCur);
    } else {
	// It isn't a command
	fprintf(stderr,"Failed to read prefix at line %u\n",lineNum);
	indicateError(buf,bp-buf);
	return 7;
    }
    return 0;
}


int DataReaderState::parseAll(void)
{
    int status;
    while ((status = parse1()) == 0);
    if (status == 8) status = 0;
    return status;
}


DataReaderState::DataReaderState(FILE *_fp,TransactionList &_list)
    :fp(_fp),lineNum(0),list(_list),dbCur(0)
{
    // Intentionally Empty
}


int createBlocks(FILE *fp,TransactionList &list)
{
    DataReaderState st(fp,list);
    return st.parseAll();
}


/*------------------------------------------------------------------------*/

class USBCommand : public TransactionList {
public:
    static USBCommand *parse(Transaction *);
    ~USBCommand(void);
    void describe(OutputFormatter &fmt) const;
    void subDescribe(OutputFormatter &) const;
    int isSame(const Transaction *) const;
    unsigned int getReadCnt(void) const;
    unsigned int getWriteCnt(void) const;
    const BlockBuf *getWriteBlock(void) const;
    const BlockBuf *getReadBlock(void) const;
    static int subEnableFl;
private:
    USBCommand(void);
    DataBlock *sendBlock;
    DataBlock *recvBlock;
};

int USBCommand::subEnableFl = 0;

void USBCommand::subDescribe(OutputFormatter &fmt) const
{
    if (!subEnableFl) return;
    TransactionList::subDescribe(fmt);
}


USBCommand::USBCommand(void)
    :sendBlock(0),recvBlock(0)
{
    // Intentionally Empty
}


USBCommand::~USBCommand(void)
{
    // Intentionally Empty
}


int USBCommand::isSame(const Transaction *tp) const
{
    const USBCommand *cmd = dynamic_cast<const USBCommand *>(tp);
    if (!cmd) return 0;
    if (!sendBlock->isSame(cmd->sendBlock)) return 0;
    if (recvBlock) {
        if (!cmd->recvBlock) return 0;
        if (!recvBlock->isSame(cmd->recvBlock)) return 0;
    } else {
        if (cmd->recvBlock) return 0;
    }
    return !0;
}


void USBCommand::describe(OutputFormatter &fmt) const
{
    fmt.startLine();
    fmt.printf("transaction");
    if (sendBlock) {
	const BlockBuf &bb = sendBlock->getBlockBuf();
	bb.inlineDump(fmt,4," send ");
    }
    if (recvBlock) {
	const BlockBuf &bb = recvBlock->getBlockBuf();
	bb.inlineDump(fmt,4," recv ");
    }
    fmt.endLine();
    if (sendBlock) {
	const BlockBuf &bb = sendBlock->getBlockBuf();
	if (bb.getCount() > 4) {
	    IndentScope iscope(fmt,4);
	    bb.dump(fmt,"send ");
	}
    }
    if (recvBlock) {
	const BlockBuf &bb = recvBlock->getBlockBuf();
	if (bb.getPtr() && bb.getCount() > 4) {
	    IndentScope iscope(fmt,4);
	    bb.dump(fmt,"recv ");
	}
    }
}


unsigned int USBCommand::getReadCnt(void) const
{
    if (recvBlock) {
	const BlockBuf &bb = recvBlock->getBlockBuf();
	return bb.getCount();
    } else {
	return 0;
    }
}


unsigned int USBCommand::getWriteCnt(void) const
{
    if (sendBlock) {
	const BlockBuf &bb = sendBlock->getBlockBuf();
	return bb.getCount();
    } else {
	return 0;
    }
}


const BlockBuf *USBCommand::getWriteBlock(void) const
{
    return (sendBlock != 0 ? &sendBlock->getBlockBuf() : 0);
}


const BlockBuf *USBCommand::getReadBlock(void) const
{
    return (recvBlock != 0 ? &recvBlock->getBlockBuf() : 0);
}


USBCommand *USBCommand::parse(Transaction *tp)
{
    USBCommand *cmdp;
    Transaction *tp3;
    DataBlock *wbp = dynamic_cast<DataBlock *>(tp);
    DataBlock *rbp = 0;
    if (!wbp) return 0;
    if (wbp->getEndpoint() != 1) return 0;
    if (wbp->getReadFl()) return 0;
    for (tp3 = tp->tGetNext(); tp3; tp3 = tp3->tGetNext()) {
	rbp = dynamic_cast<DataBlock *>(tp3);
	if (!rbp) continue;
	if (rbp->getEndpoint() == 1) {
	    rbp = 0;
	    break;
	}
	if (rbp->getEndpoint() == 129) {
	    if (!rbp->getReadFl()) rbp = 0;
	    break;
	}
	rbp = 0;
    }
    cmdp = new USBCommand;
    cmdp->sendBlock = wbp;
    cmdp->recvBlock = rbp;
    cmdp->tInsertBefore(tp);
    cmdp->tAppend(wbp);
    if (rbp) cmdp->tAppend(rbp);
    return cmdp;
}


/*------------------------------------------------------------------------*/

struct TransDesc {
    unsigned char id;
    const char *name;
};

TransDesc transTypes[] = {
    0x01,"EncMem Write",
    0x02,"EncMem Read",
    0x04,"EncReg Write",
    0x05,"EncReg Read",
    0x06,"EncMem Select",
    0x08,"I2C Write",
    0x09,"I2C Read",
    0x0a,"Mystery 3 item write",
    0x0b,"Check hi-speed mode",
    0x0c,"I2C Batch",
    0x22,"I2C 1 byte transaction",
    0x24,"I2C 2 byte write",
    0x28,"EncMem 16 word Block Read",
    0x36,"Start Video Capture Endpoint",
    0x37,"Stop Video Capture Endpoint",
    0x38,"Unknown cmd 38",
    0x39,"Unknown cmd 39",
    0x40,"Unknown cmd 40",
    0x41,"Unknown cmd 41",
    0x42,"Unknown cmd 42",
    0x43,"Start Endpoint (vbi?)",
    0x44,"Stop Endpoint (vbi?)",
    0x52,"Stop Endpoint #2 (firmware) & cmd encoder",
    0xa0,"OnAir DTV Streaming On",
    0xa1,"OnAir DTV Streaming Off",
    0xa2,"OnAir DTV Power On",
    0xa3,"OnAir DTV Power Off",
    0xd0,"Unknown cmd d0",
    0xd1,"Unknown cmd d1",
    0xd2,"Unknown cmd d2",
    0xdc,"Power Off",
    0xdd,"Encoder reset",
    0xde,"Power On",
    0xe8,"Stop Endpoint #2 (firmware)",
    0xe9,"Clear Endpoint #2 config",
    0xea,"Mystery 3 item manipulation",
    0xeb,"Get eeprom addr",
    0xec,"Get IR code",
    0xf0,"HCW Demod Resetin",
    0xf1,"HCW DTV Streaming On",
    0xf2,"HCW DTV Streaming Off",
};

class USBNotedCommand : public TransactionList {
public:
    static USBNotedCommand *parse(Transaction *);
    ~USBNotedCommand(void);
    void describe(OutputFormatter &fmt) const;
    int isSame(const Transaction *) const;
    void subDescribe(OutputFormatter &) const;
    static int subEnableFl;
private:
    unsigned char cmdId;
    USBNotedCommand(void);
    USBCommand *cmd;
    TransDesc *td;
};

int USBNotedCommand::subEnableFl = 0;


void USBNotedCommand::subDescribe(OutputFormatter &fmt) const
{
    if (!subEnableFl) return;
    TransactionList::subDescribe(fmt);
}


USBNotedCommand::USBNotedCommand(void)
    :cmd(0),td(0)
{
    // Intentionally Empty
}


USBNotedCommand::~USBNotedCommand(void)
{
    // Intentionally Empty
}


int USBNotedCommand::isSame(const Transaction *tp) const
{
    const USBNotedCommand *c2 = dynamic_cast<const USBNotedCommand *>(tp);
    if (!c2) return 0;
    return cmd->isSame(c2->cmd);
}


void USBNotedCommand::describe(OutputFormatter &fmt) const
{
    fmt.startLine();
    fmt.printf("Command code %02x",cmdId);
    if (td) {
	fmt.printf(" (%s)",td->name);
    }
    const BlockBuf *bwb = cmd->getWriteBlock();
    const BlockBuf *brb = cmd->getReadBlock();
    if (bwb) {
	bwb->inlineDump(fmt,4," send ");
    }
    if (brb) {
	brb->inlineDump(fmt,4," recv ");
    }
    fmt.endLine();
    if (bwb && (bwb->getCount() > 4)) {
	IndentScope iscope(fmt,4);
	bwb->dump(fmt,"send ");
    }
    if (brb && brb->getPtr() && (brb->getCount() > 4)) {
	IndentScope iscope(fmt,4);
	brb->dump(fmt,"recv ");
    }
}


USBNotedCommand *USBNotedCommand::parse(Transaction *tp)
{
    unsigned int idx;
    TransDesc *td;
    unsigned char cmdId;
    USBNotedCommand *cmdp;
    USBCommand *cmdu = dynamic_cast<USBCommand *>(tp);
    if (!cmdu) return 0;
    if (cmdu->getWriteCnt() < 1) return 0;
    cmdId = cmdu->getWriteBlock()->get8(0);
    td = 0;
    for (idx = 0; idx < ASZ(transTypes); idx++) {
	if (transTypes[idx].id == cmdId) {
	    td = transTypes + idx;
	    break;
	}
    }
    if (!td) return 0;
    cmdp = new USBNotedCommand;
    cmdp->cmd = cmdu;
    cmdp->cmdId = cmdId;
    cmdp->td = td;
    cmdp->tInsertBefore(tp);
    cmdp->tAppend(cmdu);
    return cmdp;
}


/*------------------------------------------------------------------------*/

class EncTransaction : public TransactionList {
public:
    static EncTransaction *parse(Transaction *);
    ~EncTransaction(void);
    void describe(OutputFormatter &fmt) const;
    unsigned char getType(void) const;
    unsigned int getOffs(void) const;
    unsigned long getWrite32(unsigned int offs) const;
    unsigned long getRead32(unsigned int offs) const;
    unsigned int getReadCnt(void) const;
    int getReadOk(void) const;
    unsigned int getWriteCnt(void) const;
    void subDescribe(OutputFormatter &) const;
    static int subEnableFl;
private:
    EncTransaction(void);
    USBCommand *cmd;
    unsigned char readCnt,writeCnt;
    unsigned char tp;
    unsigned char offs;
    int okFl;
};

int EncTransaction::subEnableFl = 0;


void EncTransaction::subDescribe(OutputFormatter &fmt) const
{
    if (!subEnableFl) return;
    TransactionList::subDescribe(fmt);
}


inline unsigned char EncTransaction::getType(void) const
{
    return tp;
}


inline unsigned int EncTransaction::getOffs(void) const
{
    return offs;
}


inline unsigned int EncTransaction::getReadCnt(void) const
{
    return readCnt;
}


inline unsigned int EncTransaction::getWriteCnt(void) const
{
    return writeCnt;
}


unsigned long EncTransaction::getWrite32(unsigned int offset) const
{
    const BlockBuf *bwb = cmd->getWriteBlock();
    if (!bwb) return 0;
    return bwb->get32le(offset*7 + 1);
}


unsigned long EncTransaction::getRead32(unsigned int offset) const
{
    const BlockBuf *brb = cmd->getReadBlock();
    if (!brb) return 0;
    return brb->get32le(offset*4);
}


int EncTransaction::getReadOk(void) const
{
    const BlockBuf *brb = cmd->getReadBlock();
    return (brb && brb->getPtr());
}


EncTransaction::EncTransaction(void)
    :cmd(0),readCnt(0),writeCnt(0),tp(0),offs(0),okFl(0)
{
    // Intentionally Empty
}


EncTransaction::~EncTransaction(void)
{
    // Intentionally Empty
}


void EncTransaction::describe(OutputFormatter &fmt) const
{
    fmt.startLine();
    fmt.printf("EncIO");
    if (!okFl) {
        fmt.printf(" unexpected format ");
        cmd->describe(fmt);
        return;
    }
    const BlockBuf *bwb = cmd->getWriteBlock();
    const BlockBuf *brb = cmd->getReadBlock();
    unsigned int idx;
    unsigned short junk;
    fmt.printf(" type %02x offs %02x",getType(),getOffs());
    if (writeCnt) {
	bwb->inlineSuperDump(fmt,0,4,writeCnt,0,1,3," send ");
    }
    if (readCnt) {
	brb->inlineSuperDump(fmt,0,4,readCnt,0,0,0," recv ");
    }
    fmt.endLine();
    if (writeCnt) {
        IndentScope iScope(fmt,4);
        bwb->superDump(fmt,4,writeCnt,8,0,1,3,offs,"send ");
        for (idx = 0; idx < writeCnt; idx++) {
            junk = bwb->get16le(1 + idx*7 + 4);
            if (junk) {
                fmt.startLine();
                fmt.printf("junk %04x at word %02x",junk,idx);
                fmt.endLine();
            }
        }
    }
    if (readCnt && brb && brb->getPtr()) {
        IndentScope iScope(fmt,4);
        brb->superDump(fmt,4,readCnt,8,0,0,0,offs,"recv ");
    }
}


EncTransaction *EncTransaction::parse(Transaction *tbp)
{
    USBCommand *cmd = dynamic_cast<USBCommand *>(tbp);
    if (!cmd) return 0;
    EncTransaction *tp;
    unsigned char cmdCode;
    unsigned int bWriteCnt,bReadCnt;
    unsigned int idx;
    const BlockBuf &bb = *cmd->getWriteBlock();

    if (bb.getCount() < 1) return 0;

    cmdCode = bb.get8(0);
    switch (cmdCode) {
    case 0x01:
    case 0x02:
    case 0x28:
	break;
    default:
	return 0;
    }

    bReadCnt = cmd->getReadCnt();
    bWriteCnt = cmd->getWriteBlock()->getCount();

    tp = new EncTransaction;
    tp->cmd = cmd;
    tp->readCnt = bReadCnt / 4;
    tp->writeCnt = (bWriteCnt-1) / 7;
    tp->tp = cmdCode;
    tp->offs = 0;
    tp->tInsertBefore(tbp);
    tp->tAppend(cmd);

    if (bWriteCnt != (tp->writeCnt * 7) + 1) goto done;
    if (bReadCnt != (tp->readCnt * 4)) goto done;

    cmdCode = bb.get8(7);
    if (cmdCode < 0x44) goto done;
    if (cmdCode > 0x54) goto done;
    tp->offs = cmdCode - 0x44;

    // Look for truncated data block...
    for (idx = 0; idx < tp->writeCnt; idx++) {
	cmdCode = bb.get8(1+(idx*7)+6);
	if (cmdCode == (idx+tp->offs + 0x44)) continue;
	tp->writeCnt = idx;
	break;
    }

    tp->okFl = !0;
 done:
    return tp;
}


/*------------------------------------------------------------------------*/

TransDesc encoderCmdTypes[] = {
    0x00,"DEC_PING_FW",
    0x01,"DEC_START_PLAYBACK",
    0x02,"DEC_STOP_PLAYBACK",
    0x03,"DEC_SET_PLAYBACK_SPEED",
    0x05,"DEC_STEP_VIDEO",
    0x08,"DEC_SET_DMA_BLOCKSIZE",
    0x09,"DEC_GET_XFER_INFO",
    0x0a,"DEC_GET_DMA_STATUS",
    0x0b,"DEC_SCHED_DMA_FROM_HOST",
    0x0d,"DEC_PAUSE_PLAYBACK",
    0x0f,"DEC_HALT_FW",

    0x10,"DEC_SET_STANDARD",
    0x11,"DEC_GET_VERSION",
    0x14,"DEC_SET_STREAM_INPUT",
    0x15,"DEC_SET_TIMING_INFO",
    0x16,"DEC_SET_AUDIO_MODE",
    0x17,"DEC_SET_EVENT_NOTIFICATION",
    0x18,"DEC_SET_DISPLAY_BUFFERS",
    0x19,"DEC_EXTRACT_VBI",
    0x1a,"DEC_SET_DECODER_SOURCE",
    0x1b,"DEC_SET_AUDIO_OUTPUT",
    0x1c,"DEC_SET_AV_DELAY",
    0x1e,"DEC_SET_PREBUFFERING",

    0x41,"OSD_GET_FRAMEBUFFER",
    0x42,"OSD_GET_PIXEL_FORMAT",
    0x43,"OSD_SET_PIXEL_FORMAT",
    0x44,"OSD_GET_STATE",
    0x45,"OSD_SET_STATE",
    0x46,"OSD_GET_OSD_COORDS",
    0x47,"OSD_SET_OSD_COORDS",
    0x48,"OSD_GET_SCREEN_COORDS",
    0x49,"OSD_SET_SCREEN_COORDS",
    0x4a,"OSD_GET_GLOBAL_ALPHA",
    0x4b,"OSD_SET_GLOBAL_ALPHA",
    0x4c,"OSD_SET_BLEND_COORDS",
    0x4f,"OSD_GET_FLICKER_STATE",

    0x50,"OSD_SET_FLICKER_STATE",
    0x52,"OSD_BLT_COPY",
    0x53,"OSD_BLT_FILL",
    0x54,"OSD_BLT_TEXT",
    0x56,"OSD_SET_FRAMEBUFFER_WINDOW",

    0x60,"OSD_SET_CHROMA_KEY",
    0x61,"OSD_GET_ALPHA_CONTENT_INDEX",
    0x62,"OSD_SET_ALPHA_CONTENT_INDEX",

    0x80,"ENC_PING_FW",
    0x81,"ENC_START_CAPTURE",
    0x82,"ENC_STOP_CAPTURE",
    0x89,"ENC_SET_AUDIO_ID",
    0x8b,"ENC_SET_VIDEO_ID",
    0x8d,"ENC_SET_PCR_ID",
    0x8f,"ENC_SET_FRAME_RATE",

    0x91,"ENC_SET_FRAME_SIZE",
    0x95,"ENC_SET_BIT_RATE",
    0x97,"ENC_SET_GOP_PROPERTIES",
    0x99,"ENC_SET_ASPECT_RATIO",
    0x9b,"ENC_SET_DNR_FILTER_MODE",
    0x9d,"ENC_SET_DNR_FITER_PROPS",
    0x9f,"ENC_SET_CORING_LEVELS",

    0xa1,"ENC_SET_SPATIAL_FILTER_TYPE",

    0xb1,"ENC_SET_3_2_PULLDOWN",
    0xb7,"ENC_SET_VBI_LINE",
    0xb9,"ENC_SET_STREAM_TYPE",
    0xbb,"ENC_SET_OUTPUT_PORT",
    0xbd,"ENC_SET_AUDIO_PROPERTIES",

    0xc3,"ENC_HALT_FW",
    0xc4,"ENC_GET_VERSION",
    0xc5,"ENC_SET_GOP_CLOSURE",
    0xc6,"ENC_SET_SEQ_END",
    0xc7,"ENC_SET_PGM_INDDEX_INFO",
    0xc8,"ENC_SET_VBI_CONFIG",
    0xc9,"ENC_SET_DMA_BLOCK_SIZE",
    0xca,"ENC_GET_PREV_DMA_INFO_MB_10",
    0xcb,"ENC_GET_PREV_DMA_INFO_MB_9",
    0xcc,"ENC_SCHED_DMA_TO_HOST",
    0xcd,"ENC_INITIALIZE_INPUT",

    0xd0,"ENC_SET_FRAME_DROP_RATE",
    0xd2,"ENC_PAUSE_ENCODER",
    0xd3,"ENC_REFRESH_INPUT",
    0xd4,"ENC_SET_COPYRIGHT",
    0xd5,"ENC_SET_EVENT_NOTIFICATION",
    0xd6,"ENC_SET_NUM_VSYNC_LINES",
    0xd7,"ENC_SET_PLACEHOLDER",
    0xd9,"ENC_MUTE_VIDEO",
    0xda,"ENC_MUTE_AUDIO",
    0xdb,"ENC_UNKNOWN",
    0xdc,"ENC_MISC",
};

class EncoderCommand : public TransactionList {
public:
    static EncoderCommand *parse(Transaction *);
    ~EncoderCommand(void);
    void describe(OutputFormatter &fmt) const;
    unsigned char getCmdCode(void) const;
    unsigned char getWierdChar(void) const;
    unsigned long get32(unsigned int offs) const;
    void subDescribe(OutputFormatter &) const;
    static int subEnableFl;
private:
    EncoderCommand(void);
    unsigned char cmdCode;
    unsigned char wierdChar;
    unsigned int seqCnt;
    unsigned int checkCnt;
    int readOk;
    int checkOk;
    unsigned long args[32];
    unsigned long echo[32];
};

int EncoderCommand::subEnableFl = 0;


void EncoderCommand::subDescribe(OutputFormatter &fmt) const
{
    if (!subEnableFl) return;
    TransactionList::subDescribe(fmt);
}


inline unsigned char EncoderCommand::getCmdCode(void) const
{
    return cmdCode;
}


inline unsigned char EncoderCommand::getWierdChar(void) const
{
    return wierdChar;
}


unsigned long EncoderCommand::get32(unsigned int offset) const
{
    if (offset >= ASZ(args)) return 0;
    return args[offset];
}


EncoderCommand::EncoderCommand(void)
    :cmdCode(0),wierdChar(0),seqCnt(0),checkCnt(0),readOk(0),checkOk(0)
{
    memset(args,0,sizeof(args));
    memset(echo,0,sizeof(echo));
}


EncoderCommand::~EncoderCommand(void)
{
    // Intentionally Empty
}


void EncoderCommand::describe(OutputFormatter &fmt) const
{
    char buf[65];
    unsigned int ccnt;
    unsigned int idx;
    unsigned int acnt;
    const char *cmdStr;
    fmt.startLine();
    fmt.printf("Encoder");
    fmt.printf(" cmdCode=%02x",cmdCode);
    cmdStr = 0;
    for (idx = 0; idx < ASZ(encoderCmdTypes); idx++) {
	if (encoderCmdTypes[idx].id == cmdCode) cmdStr = encoderCmdTypes[idx].name;
    }
    if (!cmdStr) cmdStr = "UNKNOWN";
    fmt.printf(" (%s)",cmdStr);
    if (wierdChar != 0x06) {
        if (wierdChar) {
            fmt.printf(" wierdChar=%02x",wierdChar);
        } else {
            fmt.printf(" noWierdChar");
        }
    }
    if (checkCnt != 1) {
        // Number of times we fetched status?
        fmt.printf(" checkCnt=%d",checkCnt);
    }
    if (seqCnt != 4) {
        // Got a different number of extra transfers than expected
        fmt.printf(" seqCnt=%d",seqCnt);
    }
    if (readOk) {
	if (!checkOk) {
	    fmt.printf(" checkNotOk");
	}
    } else {
	fmt.printf(" noReadDataAvail");
    }
    acnt = 0;
    for (idx = 4; idx < ASZ(args); idx++) {
        if (!args[idx]) continue;
        acnt++;
    }
    if (acnt <= 3) {
        for (idx = 4; idx < ASZ(args); idx++) {
            if (!args[idx]) continue;
            fmt.printf(" arg%d: 0x%x",idx+1-4,args[idx]);
        }
    }
    fmt.endLine();
    if (acnt > 3) {
        IndentScope iScope(fmt,4);
        ccnt = 0;
        for (idx = 4; idx < ASZ(args); idx++) {
            if (!args[idx]) continue;
            if (ccnt >= (sizeof(buf)-20)) {
                fmt.startLine();
                fmt.printf("%.*s",ccnt,buf);
                fmt.endLine();
                ccnt = 0;
            }
            sprintf(buf+ccnt,"%sarg%d: 0x%x",
                    (ccnt ? " " : ""),idx+1-4,args[idx]);
            ccnt += strlen(buf+ccnt);
        }
        if (ccnt) {
            fmt.startLine();
            fmt.printf("%.*s",ccnt,buf);
            fmt.endLine();
        }
    }
    if (readOk) for (idx = 0; idx < ASZ(args); idx++) {
	if (args[idx] != echo[idx]) {
	    fmt.startLine();
	    fmt.printf(
		"Word offs %02x: sent %08x but got back %08x",
		idx,args[idx],echo[idx]);
	    fmt.endLine();
	}
    }
}


EncoderCommand *EncoderCommand::parse(Transaction *tp)
{
    EncTransaction *ts[3];
    EncTransaction *etp;
    Transaction *tp2;
    unsigned int tcnt = 0;
    unsigned int idx;
    unsigned long val;
    int readOkFl = !0;

    // Look for enough consecutive EncTransaction operations
    while (tp && (tcnt < ASZ(ts))) {
        ts[tcnt] = dynamic_cast<EncTransaction *>(tp);
        if (!ts[tcnt]) break;
        tcnt++;
        tp = tp->tGetNext();
    }
    if (tcnt < ASZ(ts)) return 0;

    // Make sure that the first 3 transactions look correct
    if (ts[0]->getType() != 0x01) return 0;
    if (ts[0]->getReadCnt() != 0) return 0;
    if (ts[0]->getWriteCnt() != 8) return 0;
    if (ts[0]->getOffs() != 0) return 0;
    if (ts[0]->getWrite32(0) != 0) return 0;
    if ((ts[0]->getWrite32(1) & 0xffffff00) != 0) return 0;
    if (ts[0]->getWrite32(2) != 0) return 0;
    if ((ts[0]->getWrite32(3) & 0xff00ffff) != 0) return 0;

    if (ts[1]->getType() != 0x01) return 0;
    if (ts[1]->getReadCnt() != 0) return 0;
    if (ts[1]->getWriteCnt() != 8) return 0;
    if (ts[1]->getOffs() != 8) return 0;

    if (ts[2]->getType() != 0x01) return 0;
    if (ts[2]->getReadCnt() != 0) return 0;
    if (ts[2]->getWriteCnt() != 4) return 0;
    if (ts[2]->getOffs() != 16) return 0;

    // OK, we've seen enough to recognize an encoder command
    EncoderCommand *cmd = new EncoderCommand;

    for (idx = 0; idx < 8; idx++) {
        cmd->args[idx] = ts[0]->getWrite32(idx);
    }
    for (idx = 0; idx < 8; idx++) {
        cmd->args[idx+8] = ts[1]->getWrite32(idx);
    }
    for (idx = 0; idx < 4; idx++) {
        cmd->args[idx+16] = ts[2]->getWrite32(idx);
    }
    cmd->cmdCode = cmd->args[1];
    cmd->wierdChar = cmd->args[3] >> 16;

    tp2 = ts[2]->tGetNext();
    cmd->tInsertBefore(ts[0]);
    for (idx = 0; idx < ASZ(ts); idx++) {
        cmd->tAppend(ts[idx]);
    }

    // Now look at the rest of the sequence

    tp = tp2;
    tp2 = tp->tGetNext();
    etp = dynamic_cast<EncTransaction *>(tp);
    if (!etp) goto done;
    if (etp->getType() != 0x01) goto done;
    if (etp->getReadCnt() != 0) goto done;
    if (etp->getWriteCnt() != 1) goto done;
    if (etp->getWrite32(0) != 3) goto done;
    if (etp->getOffs() != 0) goto done;
    (cmd->seqCnt)++;
    cmd->tAppend(tp);

    val = 0;
    for (;;) {
        tp = tp2;
        tp2 = tp->tGetNext();
        etp = dynamic_cast<EncTransaction *>(tp);
        if (!etp) goto done;
        if (etp->getType() != 0x02) break;
        if (etp->getReadCnt() != 16) goto done;
	if (etp->getReadOk()) {
	    val = etp->getRead32(0);
	    if ((val != 3) && (val != 7)) goto done;
	} else {
	    readOkFl = 0;
	}
        if (etp->getWriteCnt() != 1) goto done;
        if (etp->getWrite32(0) != 0) goto done;
        if (etp->getOffs() != 0) goto done;
        (cmd->checkCnt)++;
        cmd->tAppend(tp);
    }
    cmd->checkOk = (val == 7);
    cmd->args[0] = val;

    if (etp->getType() != 0x28) goto done;
    if (etp->getReadCnt() != 16) goto done;
    if (!etp->getReadOk()) readOkFl = 0;
    if (etp->getWriteCnt() != 1) goto done;
    if (etp->getWrite32(0) != 0) goto done;
    if (etp->getOffs() != 0) goto done;
    (cmd->seqCnt)++;
    cmd->tAppend(tp);
    if (readOkFl) for (idx = 0; idx < etp->getReadCnt(); idx++) {
	if ((idx + etp->getOffs()) > ASZ(cmd->echo)) {
	    break;
	}
	cmd->echo[idx + etp->getOffs()] = etp->getRead32(idx);
    }

    tp = tp2;
    tp2 = tp->tGetNext();
    etp = dynamic_cast<EncTransaction *>(tp);
    if (!etp) goto done;
    if (etp->getType() != 0x28) goto done;
    if (etp->getReadCnt() != 16) goto done;
    if (!etp->getReadOk()) readOkFl = 0;
    if (etp->getWriteCnt() != 1) goto done;
    if (etp->getWrite32(0) != 0) goto done;
    if (etp->getOffs() != 16) goto done;
    (cmd->seqCnt)++;
    cmd->tAppend(tp);
    if (readOkFl) for (idx = 0; idx < etp->getReadCnt(); idx++) {
	if ((idx + etp->getOffs()) > ASZ(cmd->echo)) {
	    break;
	}
	cmd->echo[idx + etp->getOffs()] = etp->getRead32(idx);
    }

    tp = tp2;
    tp2 = tp->tGetNext();
    etp = dynamic_cast<EncTransaction *>(tp);
    if (!etp) goto done;
    if (etp->getType() != 0x01) goto done;
    if (etp->getReadCnt() != 0) goto done;
    if (etp->getWriteCnt() != 1) goto done;
    if (etp->getWrite32(0) != 0) goto done;
    if (etp->getOffs() != 0) goto done;
    (cmd->seqCnt)++;
    cmd->tAppend(tp);

 done:
    cmd->readOk = readOkFl;
    return cmd;
}


/*------------------------------------------------------------------------*/

class I2CCommand : public TransactionList {
public:
    static I2CCommand *parse(Transaction *);
    ~I2CCommand(void);
    int isSame(const Transaction *) const;
    void describe(OutputFormatter &fmt) const;
    unsigned char getAddr(void) const;
    const BlockBuf &getWriteBlockBuf(void) const;
    const BlockBuf &getReadBlockBuf(void) const;
    unsigned char getReadCnt(void) const;
    unsigned char getWriteCnt(void) const;
    unsigned char getWriteBlocks(void) const;
    void describeRetCode(OutputFormatter &fmt) const;
    void subDescribe(OutputFormatter &) const;
    static int subEnableFl;
private:
    I2CCommand(void);
    USBCommand *cmd;
    unsigned char retCode;
    unsigned char i2cAddr;
    int readXtra;
    unsigned int readCnt;
    unsigned int writeCnt;
    unsigned int writeBlocks;
    int mode;
    BlockBuf writeInfo;
    BlockBuf readInfo;
};

int I2CCommand::subEnableFl = 0;


void I2CCommand::subDescribe(OutputFormatter &fmt) const
{
    if (!subEnableFl) return;
    TransactionList::subDescribe(fmt);
}


inline unsigned char I2CCommand::getAddr(void) const
{
    return i2cAddr;
}


const BlockBuf &I2CCommand::getWriteBlockBuf(void) const
{
    return writeInfo;
}


const BlockBuf &I2CCommand::getReadBlockBuf(void) const
{
    return readInfo;
}


inline unsigned char I2CCommand::getReadCnt(void) const
{
    return readCnt;
}


inline unsigned char I2CCommand::getWriteCnt(void) const
{
    return writeCnt;
}


inline unsigned char I2CCommand::getWriteBlocks(void) const
{
    return writeBlocks;
}


I2CCommand::I2CCommand(void)
    :cmd(0),retCode(0),i2cAddr(0),readXtra(0),
     readCnt(0),writeCnt(0),writeBlocks(0),mode(0)
{
    // Intentionally Empty
}


I2CCommand::~I2CCommand(void)
{
    // Intentionally Empty
}


int I2CCommand::isSame(const Transaction *tp) const
{
    const I2CCommand *cmd = dynamic_cast<const I2CCommand *>(tp);
    if (!cmd) return 0;
    if (i2cAddr != cmd->i2cAddr) return 0;
    if ((!mode) != !(cmd->mode)) return 0;
    if (retCode != cmd->retCode) return 0;
    if (writeCnt != cmd->writeCnt) return 0;
    if (writeBlocks != cmd->writeBlocks) return 0;
    if (readCnt != cmd->readCnt) return 0;
    if (readXtra != cmd->readXtra) return 0;
    if (writeInfo.getCount() < writeCnt) return 0;
    if (cmd->writeInfo.getCount() < cmd->writeCnt) return 0;
    if (readInfo.getCount() < readCnt) return 0;
    if (cmd->readInfo.getCount() < cmd->readCnt) return 0;
    if (writeCnt) {
        if (memcmp(writeInfo.getPtr(),cmd->writeInfo.getPtr(),writeCnt)) {
            return 0;
        }
    }
    if ((readInfo.getPtr() != 0) != (cmd->readInfo.getPtr() != 0)) return 0;
    if (readCnt && readInfo.getPtr()) {
        if (memcmp(readInfo.getPtr(),cmd->readInfo.getPtr(),readCnt)) {
            return 0;
        }
    }
    return !0;
}


void I2CCommand::describeRetCode(OutputFormatter &fmt) const
{
    if (readInfo.getPtr()) {
	if (retCode != 0x08) fmt.printf(" retCode=%02x",retCode);
    }
}


void I2CCommand::describe(OutputFormatter &fmt) const
{
    unsigned int idx,cnt,mc;
    fmt.startLine();
    fmt.printf("I2C %s",mode ? (mode == 2 ? "batch" : "read") : "write");
    fmt.printf(" addr=%02x",i2cAddr);
    if (writeBlocks == 1) {
	writeInfo.inlineDump(fmt,6,writeCnt," send ");
	readInfo.inlineDump(fmt,4,readCnt," recv ");
    }
    if (readXtra) fmt.printf(" recvXtra [%02x]",readXtra);
    describeRetCode(fmt);
    fmt.endLine();
    if (writeBlocks != 1) {
        IndentScope iScope(fmt,4);
	if (writeCnt) for (idx = 0; idx < writeBlocks; idx++) {
	    writeInfo.superDump(fmt,1,writeCnt,16,0,idx * writeCnt,
				0,0,"send ");
	}
	if (readInfo.getPtr()) readInfo.dump(fmt,readCnt,"recv ");
    } else {
	if (writeCnt > 6) {
	    IndentScope iScope(fmt,4);
	    writeInfo.dump(fmt,writeCnt,"send ");
	}
	if (readInfo.getPtr() && (readCnt > 4)) {
	    IndentScope iScope(fmt,4);
	    readInfo.dump(fmt,readCnt,"recv ");
	}
    }
}


I2CCommand *I2CCommand::parse(Transaction *tp)
{
    USBCommand *cmd = dynamic_cast<USBCommand *>(tp);
    if (!cmd) return 0;
    I2CCommand *i2cCmd;
    unsigned int readCnt = cmd->getReadCnt();
    unsigned int writeCnt = cmd->getWriteCnt();
    unsigned int writeBlocks = 1;
    unsigned char i2cAddr;
    unsigned char i2cWLen = 0;
    unsigned char i2cRLen = 0;
    unsigned char cmdCode;
    unsigned char retCode;
    unsigned char offs;
    unsigned char cnt;
    int mode;

    if (readCnt < 1) return 0;
    if (writeCnt < 1) return 0;

    const BlockBuf *bwb = cmd->getWriteBlock();
    const BlockBuf *brb = cmd->getReadBlock();

    cmdCode = bwb->get8(0);
    retCode = brb ? brb->get8(0) : 0;

    switch (cmdCode) {
    case 0x08: // I2C write
	offs = 3;
	if (bwb->getCount() < offs) return 0;
	i2cAddr = bwb->get8(1);
	i2cWLen = bwb->get8(2);
	mode = 0;
	break;
    case 0x09: // I2C read
	offs = 4;
	if (bwb->getCount() < offs) return 0;
	i2cWLen = bwb->get8(1);
	i2cRLen = bwb->get8(2);
	i2cAddr = bwb->get8(3);
	mode = 1;
	break;
    case 0x0c: // Batch I2C write
	offs = 4;
	if (bwb->getCount() < offs) return 0;
	i2cAddr = bwb->get8(1);
	writeBlocks = bwb->get8(2);
	i2cWLen = bwb->get8(3);
	mode = 2;
	break;
    default:
	return 0;
    }

    i2cCmd = new I2CCommand;
    i2cCmd->cmd = cmd;
    i2cCmd->retCode = retCode;
    i2cCmd->readCnt = i2cRLen;
    i2cCmd->readXtra = readCnt - (i2cRLen+1);
    i2cCmd->writeCnt = i2cWLen;
    i2cCmd->writeBlocks = writeBlocks;
    i2cCmd->i2cAddr = i2cAddr;
    i2cCmd->mode = mode;
    cnt = bwb->getCount() - offs;
    if (cnt > (i2cWLen*writeBlocks)) cnt = (i2cWLen*writeBlocks);
    i2cCmd->writeInfo.assign(bwb->getPtr()+offs,cnt);
    cnt = brb ? brb->getCount() - 1 : 0;
    if (cnt > i2cRLen) cnt = i2cRLen;
    if (brb->getPtr()) i2cCmd->readInfo.assign(brb->getPtr()+1,cnt);
    i2cCmd->tInsertBefore(tp);
    i2cCmd->tAppend(tp);
    return i2cCmd;
}


/*------------------------------------------------------------------------*/

class RegisterCommand : public TransactionList {
public:
    static RegisterCommand *parse(Transaction *);
    ~RegisterCommand(void);
    int isSame(const Transaction *tp) const;
    void describe(OutputFormatter &fmt) const;
    unsigned short getAddr(void) const;
    unsigned long getData(void) const;
    int getReadFl(void) const;
    void subDescribe(OutputFormatter &) const;
    static int subEnableFl;
private:
    RegisterCommand(void);
    USBCommand *cmd;
    int okFl;
    int readMode;
    int readValid;
    unsigned long rData;
    unsigned short regAddr;
    unsigned char b5;
};

int RegisterCommand::subEnableFl = 0;


void RegisterCommand::subDescribe(OutputFormatter &fmt) const
{
    if (!subEnableFl) return;
    TransactionList::subDescribe(fmt);
}


int RegisterCommand::getReadFl(void) const
{
    return readMode;
}


inline unsigned short RegisterCommand::getAddr(void) const
{
    return regAddr;
}


inline unsigned long RegisterCommand::getData(void) const
{
    return rData;
}


struct RegisterName {
    unsigned short id;
    const char *name;
};


RegisterName registerNames[] = {
    0x9008,"gpio_in",
    0x900c,"gpio_out",
    0x9020,"gpio_dir",
    0x0048,"int_mask",
    0xa064,"apu_cmd",
    0x9054,"hw_blocks",
    0x9058,"vpu_ctrl",
    0x07f8,"sdram_refresh",
    0x07fc,"sdram_precharge",
    0x0700,"i2c_cllk",
};


RegisterCommand::RegisterCommand(void)
    :cmd(0),okFl(0),readMode(0),readValid(0),rData(0),regAddr(0)
{
    // Intentionally Empty
}


RegisterCommand::~RegisterCommand(void)
{
    // Intentionally Empty
}


int RegisterCommand::isSame(const Transaction *tp) const
{
    const RegisterCommand *t2p;
    t2p = dynamic_cast<const RegisterCommand *>(tp);
    if (!t2p) return 0;
    return cmd->isSame(t2p->cmd);
}


void RegisterCommand::describe(OutputFormatter &fmt) const
{
    unsigned int idx,cnt,mc;
    RegisterName *rn;
    fmt.startLine();
    fmt.printf("Encoder Register");
    if (!okFl) {
        fmt.printf(" unexpected format ");
        cmd->describe(fmt);
        return;
    }
    rn = 0;
    for (idx = 0; idx < ASZ(registerNames); idx++) {
	if (registerNames[idx].id == regAddr) {
	    rn = registerNames + idx;
	}
    }
    if (rn) {
	fmt.printf(" %5s [%04x (%s)]",readMode ? "read" : "write",regAddr,
		   rn->name);
    } else {
	fmt.printf(" %5s [%04x]",readMode ? "read" : "write",regAddr);
    }
    if ((!readMode) || (readValid)) {
        fmt.printf(" %s %08x",
		   (readMode ? "-->" : "<--"),
		   rData);
    }
    if (b5) {
        fmt.printf(" byte[5]: %02x",b5);
    }
    fmt.endLine();
}


RegisterCommand *RegisterCommand::parse(Transaction *tp)
{
    USBCommand *cmd = dynamic_cast<USBCommand *>(tp);
    if (!cmd) return 0;
    RegisterCommand *regCmd;
    int readValid = 0;
    unsigned short regAddr;
    unsigned long rData;
    unsigned char cmdCode;
    unsigned char b5;
    int okFl;
    unsigned int readCnt = cmd->getReadCnt();
    unsigned int writeCnt = cmd->getWriteCnt();

    if (!writeCnt) return 0;

    const BlockBuf &bwb = *cmd->getWriteBlock();

    okFl = (bwb.getCount() == 8);
    regAddr = bwb.get16be(6);
    cmdCode = bwb.get8(0);
    b5 = bwb.get8(5);
    switch (cmdCode) {
    case 0x05: // Read register
	if (readCnt == 4) {
	    const BlockBuf *brb = cmd->getReadBlock();
	    if (brb && brb->getPtr()) {
		readValid = !0;
		rData = brb->get32le(0);
	    }
	} else {
	    okFl = 0;
	}
	break;
    case 0x04: // Write register
	rData = bwb.get32le(1);
	if (readCnt != 0) {
	    okFl = 0;
	}
	break;
    default:
	return 0;
    }

    regCmd = new RegisterCommand;
    regCmd->cmd = cmd;
    regCmd->regAddr = regAddr;
    regCmd->rData = rData;
    regCmd->readMode = (cmdCode == 0x05);
    regCmd->readValid = readValid;
    regCmd->b5 = b5;
    regCmd->okFl = okFl;
    regCmd->tInsertBefore(tp);
    regCmd->tAppend(tp);
    return regCmd;
}


/*------------------------------------------------------------------------*/

class VideoData : public TransactionList {
public:
    static VideoData *parse(Transaction *);
    ~VideoData(void);
    void describe(OutputFormatter &fmt) const;
    void subDescribe(OutputFormatter &) const;
    static int subEnableFl;
private:
    VideoData(void);
    unsigned int cnt;
};

int VideoData::subEnableFl = 0;


void VideoData::subDescribe(OutputFormatter &fmt) const
{
    if (!subEnableFl) return;
    TransactionList::subDescribe(fmt);
}


VideoData::VideoData(void)
    :cnt(0)
{
    // Intentionally Empty
}


VideoData::~VideoData(void)
{
    // Intentionally Empty
}


void VideoData::describe(OutputFormatter &fmt) const
{
    fmt.startLine();
    fmt.printf("Video data retrieval size: 0x%x",cnt);
    if (tGetCount() > 1) {
        fmt.printf(" (%d transactions)",tGetCount());
    }
    fmt.endLine();
}


static DataBlock *VideoFetchOp(Transaction *tp)
{
    DataBlock *bp = dynamic_cast<DataBlock *>(tp);
    if (!bp) return 0;
    if (bp->getEndpoint() != 0x84) return 0;
    if (!bp->getReadFl()) return 0;
    return bp;
}


VideoData *VideoData::parse(Transaction *tp)
{
    DataBlock *bp = VideoFetchOp(tp);
    Transaction *tp2;
    if (!bp) return 0;
    VideoData *p = new VideoData;
    p->cnt = bp->getBlockBuf().getCount();
    tp2 = tp->tGetNext();
    p->tInsertBefore(tp);
    p->tAppend(tp);
    while (tp2) {
        bp = VideoFetchOp(tp2);
        if (!bp) break;
        p->cnt += bp->getBlockBuf().getCount();
        tp = tp2;
        tp2 = tp2->tGetNext();
        p->tAppend(tp);
    }
    return p;
}


/*------------------------------------------------------------------------*/

class FirmwareUpload : public TransactionList {
public:
    static FirmwareUpload *parse(Transaction *);
    ~FirmwareUpload(void);
    void describe(OutputFormatter &fmt) const;
    void subDescribe(OutputFormatter &) const;
    static int subEnableFl;
private:
    FirmwareUpload(void);
    unsigned int cnt;
    unsigned int num;
};

int FirmwareUpload::subEnableFl = 0;


void FirmwareUpload::subDescribe(OutputFormatter &fmt) const
{
    if (!subEnableFl) return;
    TransactionList::subDescribe(fmt);
}


FirmwareUpload::FirmwareUpload(void)
    :cnt(0),num(firmwareCount++)
{
    // Intentionally Empty
}


FirmwareUpload::~FirmwareUpload(void)
{
    // Intentionally Empty
}


static DataBlock *firmwareFetchOp(Transaction *tp)
{
    DataBlock *bp = dynamic_cast<DataBlock *>(tp);
    if (!bp) return 0;
    if (bp->getEndpoint() != 2) return 0;
    if (bp->getReadFl()) return 0;
    return bp;
}


void FirmwareUpload::describe(OutputFormatter &fmt) const
{
    fmt.startLine();
    fmt.printf("Firmware upload size: 0x%x",cnt);
    if (tGetCount() > 1) {
        fmt.printf(" (%d transactions)",tGetCount());
    }
    fmt.endLine();
    if (saveFirmwareName) {
	int swabfl = 0;
	char *newname;
	FILE *fp;
	DataBlock *bp;
	Transaction *tp;
	// This is an ugly, ugly hack job.
	if (firmwareCount > 1) {
	    newname = new char[strlen(saveFirmwareName)+10+1];
	    sprintf(newname,"%s-%u",saveFirmwareName,num);
	} else {
	    newname = new char[strlen(saveFirmwareName)+1];
	    sprintf(newname,"%s",saveFirmwareName);
	}
	fp = fopen(newname,"wb");
	if (fp) {
	    if (cnt >= 0x40000) {
		// This looks like encoder firmware, in which case we
		// have to byte&word swap it :-(
		swabfl = !0;
	    }
	    for (tp = tGetFirst(); tp; tp = tp->tGetNext()) {
		bp = firmwareFetchOp(tp);
		if (!bp) continue;
		const BlockBuf &bb = bp->getBlockBuf();
		unsigned int offs,idx;
		char buf[4];
		if (swabfl) {
		    for (offs = 0; offs < bb.getCount(); offs += 4) {
			for (idx = 0; idx < 4; idx++) {
			    buf[idx] = bb.get8(offs+idx);
			}
			for (idx = 0; idx < 4; idx++) {
			    fputc(buf[3-idx],fp);
			}
		    }
		} else {
		    for (offs = 0; offs < bb.getCount(); offs++) {
			fputc(bb.get8(offs),fp);
		    }
		}
	    }
	    fclose(fp);
	    fprintf(stderr,"Wrote %u bytes of%s firmware data to %s\n",
		    cnt,
		    (swabfl ? " byte swapped" : ""),
		    newname);
	    delete[] newname;
	} else {
	    fprintf(stderr,"Failed to open %s for writing firmware data\n",
		    saveFirmwareName);
	}
    }
}


FirmwareUpload *FirmwareUpload::parse(Transaction *tp)
{
    DataBlock *bp = firmwareFetchOp(tp);
    Transaction *tp2;
    if (!bp) return 0;
    FirmwareUpload *p = new FirmwareUpload;
    p->cnt = bp->getBlockBuf().getCount();
    tp2 = tp->tGetNext();
    p->tInsertBefore(tp);
    p->tAppend(tp);
    while (tp2) {
        bp = firmwareFetchOp(tp2);
        if (!bp) break;
        p->cnt += bp->getBlockBuf().getCount();
        tp = tp2;
        tp2 = tp2->tGetNext();
        p->tAppend(tp);
    }
    return p;
}


/*------------------------------------------------------------------------*/

class I2CDeviceTransaction : public TransactionList {
public:
    static I2CDeviceTransaction *parse(Transaction *);
    ~I2CDeviceTransaction(void);
    int isSame(const Transaction *tp) const;
    void subDescribe(OutputFormatter &) const;
    static int subEnableFl;
protected:
    I2CDeviceTransaction(I2CCommand *);
    I2CCommand *cmd;
private:
public:
    I2CCommand *getI2CCommand(void) const {return cmd; }
};

int I2CDeviceTransaction::subEnableFl = 0;


void I2CDeviceTransaction::subDescribe(OutputFormatter &fmt) const
{
    if (!subEnableFl) return;
    TransactionList::subDescribe(fmt);
}


I2CDeviceTransaction::I2CDeviceTransaction(I2CCommand *_cmd)
    :cmd(_cmd)
{
    // Intentionally Empty
}


I2CDeviceTransaction::~I2CDeviceTransaction(void)
{
    if (cmd) delete cmd;
}


int I2CDeviceTransaction::isSame(const Transaction *tp) const
{
    const I2CDeviceTransaction *t2p;
    t2p = dynamic_cast<const I2CDeviceTransaction *>(tp);
    if (!t2p) return 0;
    return cmd->isSame(t2p->cmd);
}


/*------------------------------------------------------------------------*/

struct DataFormat {
    unsigned char size;
    unsigned char bigEndian;
    unsigned long getItem(const BlockBuf &,unsigned int idx) const;
};

unsigned long DataFormat::getItem(const BlockBuf &bb,unsigned int idx) const
{
    unsigned int bytes = (size / 8) + ((size % 8) ? 1 : 0);
    switch (bytes) {
    case 1:
	return bb.get8(idx);
    case 2:
	return bigEndian ? bb.get16be(idx) : bb.get16le(idx);
    case 4:
	return bigEndian ? bb.get32be(idx) : bb.get32le(idx);
    default: break;
    }
    return 0;
}


/* Generic base for I2C devices which can be completely described via
   normal register access mappings. */
class I2CDeviceRegistered : public I2CDeviceTransaction {
    friend class I2CDeviceTransaction;
public:
    I2CDeviceRegistered(I2CCommand *);
    void describe(OutputFormatter &fmt) const;
    virtual int isOk(void) const;
    void subDescribe(OutputFormatter &) const;
    static int subEnableFl;
protected:
    DataFormat addrFormat;
    DataFormat dataFormat;
    int incMode; //0=not legal 1=increment 2=static
private:
    virtual const char *getName(void) const = 0;
    virtual const RegisterTable *getRegisterTable(void) const {return 0;}
    virtual void getWriteAddrData(const BlockBuf &,unsigned int idx,
				  unsigned long &addr,
				  unsigned long &data) const;
    virtual void getReadAddrData(const BlockBuf &bwb,const BlockBuf &brb,
				 unsigned long &addr,
				 unsigned long &data) const;
};


int I2CDeviceRegistered::subEnableFl = 0;


void I2CDeviceRegistered::subDescribe(OutputFormatter &fmt) const
{
    if (!subEnableFl) return;
    TransactionList::subDescribe(fmt);
}


void I2CDeviceRegistered::getWriteAddrData(const BlockBuf &bb,unsigned int idx,
					   unsigned long &addr,
					   unsigned long &data) const
{
    unsigned int bytes;
    bytes = (addrFormat.size / 8) + ((addrFormat.size % 8) ? 1 : 0);
    addr = addrFormat.getItem(bb,idx);
    data = dataFormat.getItem(bb,idx+bytes);
}


void I2CDeviceRegistered::getReadAddrData(const BlockBuf &bwb,
					  const BlockBuf &brb,
					  unsigned long &addr,
					  unsigned long &data) const
{
    addr = addrFormat.getItem(bwb,0);
    data = dataFormat.getItem(brb,0);
}


I2CDeviceRegistered::I2CDeviceRegistered(I2CCommand *_cmd)
    :I2CDeviceTransaction(_cmd)
{
    addrFormat.size = 8;
    addrFormat.bigEndian = 0;
    dataFormat.size = 8;
    dataFormat.bigEndian = 0;
    incMode = 0;
}


int I2CDeviceRegistered::isOk(void) const
{
    unsigned int abytes,databytes;
    unsigned int wcnt = cmd->getWriteCnt();
    unsigned int rcnt = cmd->getReadCnt();
    abytes = (addrFormat.size / 8) + ((addrFormat.size % 8) ? 1 : 0);
    databytes = (dataFormat.size / 8) + ((dataFormat.size % 8) ? 1 : 0);

    if (wcnt < abytes) return 0;
    wcnt -= abytes;
    if (rcnt) {
	if (cmd->getWriteBlocks() != 1) return 0;
	if (wcnt) return 0;
	if ((rcnt != 1) && !incMode) return 0;
	if (rcnt % databytes) return 0;
	return !0;
    }
    if (wcnt < databytes) return 0;
    if ((wcnt != 1) && !incMode) return 0;
    if (wcnt % databytes) return 0;
    return !0;
}


const RegisterDef *lookupRegisterDef(const RegisterTable *tabPtr,
				     unsigned long addr)
{
    unsigned int idx;
    if (!tabPtr) return 0;
    for (idx = 0; idx < tabPtr->regCount; idx++) {
	if (tabPtr->regDefs[idx].addr == addr) {
	    return tabPtr->regDefs+idx;
	}
    }
    return 0;
}


void I2CDeviceRegistered::describe(OutputFormatter &fmt) const
{
    unsigned int offs;
    char fmtbuf[50];
    fmt.startLine();
    fmt.printf("%s",getName());
    if (!isOk()) {
        fmt.printf(" unexpected format ");
        cmd->describe(fmt);
        return;
    }
    const BlockBuf &bwb = cmd->getWriteBlockBuf();
    const BlockBuf &brb = cmd->getReadBlockBuf();
    unsigned int writeBlocks = cmd->getWriteBlocks();
    unsigned int writeCnt = cmd->getWriteCnt();
    unsigned int readCnt = cmd->getReadCnt();
    const RegisterTable *rtab = getRegisterTable();
    const RegisterDef *regdp;
    const char *regName = 0;
    unsigned long addr,data;
    unsigned int aw,dw,ab,db;
    aw = (addrFormat.size / 4) + ((addrFormat.size % 4) ? 1 : 0);
    dw = (dataFormat.size / 4) + ((dataFormat.size % 4) ? 1 : 0);
    ab = (addrFormat.size / 8) + ((addrFormat.size % 8) ? 1 : 0);
    db = (dataFormat.size / 8) + ((dataFormat.size % 8) ? 1 : 0);

    if (readCnt && !brb.getPtr()) {
	sprintf(fmtbuf," register %%s%%s%5s [%%0%ux]",
		"read",
		aw,dw);
    } else {
	sprintf(fmtbuf," register %%s%%s%5s [%%0%ux] %s %%0%ux",
		(readCnt == 0 ? "write" : "read"),
		aw,
		(readCnt == 0 ? "<--" : "-->"),
		dw);
    }
    if (readCnt == 0) {
	if ((writeBlocks == 1) && ((ab + db) == writeCnt)) {
	    // Print using really nice compact format
	    getWriteAddrData(bwb,0,addr,data);
	    regdp = lookupRegisterDef(rtab,addr);
	    regName = regdp ? regdp->regName : 0;
	    fmt.printf(fmtbuf,
		       regName ? regName : "",
		       regName ? " " : "",
		       addr,data);
	    cmd->describeRetCode(fmt);
	    fmt.endLine();
	    if (regdp) printBitsAction(fmt,regdp,dw,data,0);
	} else {
	    cmd->describeRetCode(fmt);
	    fmt.endLine();
	    IndentScope iScope(fmt,4);
	    for (unsigned int idx = 0; idx < writeBlocks; idx++) {
		offs = ab + writeCnt;
		offs *= idx;
		addr = addrFormat.getItem(bwb,offs);
		offs += ab;
		for (unsigned int wdx = 0; wdx < writeCnt; wdx += db) {
		    fmt.startLine();
		    data = dataFormat.getItem(bwb,offs);
		    offs += db;
		    regdp = lookupRegisterDef(rtab,addr);
		    regName = regdp ? regdp->regName : 0;
		    fmt.printf(fmtbuf,
			       regName ? regName : "",
			       regName ? " " : "",
			       addr,data);
		    fmt.endLine();
		    if (regdp) printBitsAction(fmt,regdp,dw,data,0);
		    if (incMode == 1) addr++;
		}
	    }
	}
    } else {
	if (db == readCnt) {
	    // Print using really nice compact format
	    getReadAddrData(bwb,brb,addr,data);
	    regdp = lookupRegisterDef(rtab,addr);
	    regName = regdp ? regdp->regName : 0;
	    fmt.printf(fmtbuf,
		       regName ? regName : "",
		       regName ? " " : "",
		       addr,data);
	    cmd->describeRetCode(fmt);
	    fmt.endLine();
	    if (regdp) printBitsAction(fmt,regdp,dw,data,!0);
	} else {
	    cmd->describeRetCode(fmt);
	    fmt.endLine();
	    IndentScope iScope(fmt,4);
	    addr = addrFormat.getItem(bwb,0);
	    offs = 0;
	    for (unsigned int idx = 0; idx < readCnt; idx += db) {
		fmt.startLine();
		data = dataFormat.getItem(brb,offs);
		offs += db;
		regdp = lookupRegisterDef(rtab,addr);
		regName = regdp ? regdp->regName : 0;
		fmt.printf(fmtbuf,
			   regName ? regName : "",
			   regName ? " " : "",
			   addr,data);
		fmt.endLine();
		if (regdp) printBitsAction(fmt,regdp,dw,data,0);
		if (incMode == 1) addr++;
	    }
	}
    }
}

/*------------------------------------------------------------------------*/

class I2CDeviceMSP3400 : public I2CDeviceTransaction {
    friend class I2CDeviceTransaction;
public:
    void describe(OutputFormatter &fmt) const;
private:
    I2CDeviceMSP3400(I2CCommand *);
    int okFl;
    int readMode;
    int readValid;
    unsigned char subAddr;
    unsigned short addr;
    unsigned short rData;
public:
    static I2CDeviceTransaction *alloc(I2CCommand *cp)
	{ return new I2CDeviceMSP3400(cp);}
};


I2CDeviceMSP3400::I2CDeviceMSP3400(I2CCommand *_cmd)
    :I2CDeviceTransaction(_cmd),okFl(0),readMode(0),readValid(0)
{
    unsigned int readCnt,writeCnt;
    const BlockBuf &bwb = cmd->getWriteBlockBuf();
    const BlockBuf &brb = cmd->getReadBlockBuf();
    readCnt = cmd->getReadCnt();
    writeCnt = cmd->getWriteCnt();
    if (cmd->getWriteBlocks() != 1) return;
    if (writeCnt < 3) return;
    subAddr = bwb.get8(0);
    if (subAddr == 0) {
	if ((writeCnt == 3) && (readCnt == 0)) {
	    rData = bwb.get16be(1);
	    okFl = !0;
	} else if ((writeCnt == 1) && (readCnt == 2)) {
	    readMode = !0;
	    okFl = !0;
	    if (brb.getAlloc()) {
		readValid = !0;
		rData = brb.get16be(0);
	    }
	}
    } else {
	addr = bwb.get16be(1);
	if ((writeCnt == 5) && (readCnt == 0)) {
	    rData = bwb.get16be(3);
	    okFl = !0;
	} else if ((writeCnt == 3) && (readCnt == 2)) {
	    readMode = !0;
	    okFl = !0;
	    if (brb.getAlloc()) {
		readValid = !0;
		rData = brb.get16be(0);
	    }
	}
    }
}


void I2CDeviceMSP3400::describe(OutputFormatter &fmt) const
{
    fmt.startLine();
    fmt.printf("msp3400");
    if (!okFl) {
        fmt.printf(" unexpected format ");
        cmd->describe(fmt);
        return;
    }
    fmt.printf(" %5s",readMode ? "read" : "write");
    if (subAddr) {
        fmt.printf(" [%02x.%04x]",subAddr,addr);
    } else {
        fmt.printf(" [%02x.xxxx]",subAddr);
    }
    if (readMode) {
	if (readValid) {
	    fmt.printf(" --> %04x",rData);
	}
    } else {
        fmt.printf(" <-- %04x",rData);
    }
    cmd->describeRetCode(fmt);
    fmt.endLine();
}


/*------------------------------------------------------------------------*/

class I2CDeviceSAA7115 : public I2CDeviceRegistered {
private:
    I2CDeviceSAA7115(I2CCommand *_cmd)
	:I2CDeviceRegistered(_cmd) {
	addrFormat.size = 8;
	dataFormat.size = 8;
    }
    const char *getName(void) const {return "saa7115";}
public:
    static I2CDeviceTransaction *alloc(I2CCommand *cp)
	{ return new I2CDeviceSAA7115(cp);}
};


/*------------------------------------------------------------------------*/

class I2CDeviceCX25840 : public I2CDeviceRegistered {
private:
    I2CDeviceCX25840(I2CCommand *_cmd)
	:I2CDeviceRegistered(_cmd) {
	addrFormat.size = 16;
	addrFormat.bigEndian = !0;
	dataFormat.size = 8;
	incMode = 1;
    }
    const char *getName(void) const {return "cx25840";}
public:
    static I2CDeviceTransaction *alloc(I2CCommand *cp)
	{ return new I2CDeviceCX25840(cp);}
};


/*------------------------------------------------------------------------*/

BitfieldDef bitfieldWM8775_07[] = {
    {"TOD",3,1,0},
};

static const char *bf_WM8775_ADCWL[] = {
    "16 bit data",
    "20 bit data",
    "24 bit data",
    "32 bit data"};
CREATE_BITFIELD_ENUM(fm_WM8775_ADCWL,bf_WM8775_ADCWL)
BitfieldDef bitfieldWM8775_0b[] = {
    {"ADCHPD",8,1,0},
    {"ADCMCLK",6,1,0},
    {"ADCWL",4,2,fm_WM8775_ADCWL},
    {"ADCBCP",3,1,0},
    {"ADCLRP",2,1,0},
    {"ADCFMT",0,2,0},
};

static const char *bf_WM8775_ADCMS[] = {
    "Slave Mode",
    "Master Mode"};
CREATE_BITFIELD_ENUM(fm_WM8775_ADCMS,bf_WM8775_ADCMS)
static const char *bf_WM8775_ADCRATE[] = {
    "256fs",
    "384fs",
    "512fs",
    "768fs"};
CREATE_BITFIELD_ENUM(fm_WM8775_ADCRATE,bf_WM8775_ADCRATE)
static const char *bf_WM8775_ADCOSR[] = {
    "128x",
    "64x"};
CREATE_BITFIELD_ENUM(fm_WM8775_ADCOSR,bf_WM8775_ADCOSR)
BitfieldDef bitfieldWM8775_0c[] = {
    {"ADCMS",8,1,fm_WM8775_ADCMS},
    {"ADCOSR",3,1,fm_WM8775_ADCOSR},
    {"ADCRATE",0,3,fm_WM8775_ADCRATE},
};

static const char *bf_WM8775_PD[] = {
    "normal",
    "power down"};
CREATE_BITFIELD_ENUM(fm_WM8775_PD,bf_WM8775_PD)
BitfieldDef bitfieldWM8775_0d[] = {
    {"AINPD",6,1,fm_WM8775_PD},
    {"ADCPD",1,1,fm_WM8775_PD},
    {"PWDN",0,1,fm_WM8775_PD},
};

BitfieldDef bitfieldWM8775_0e[] = {
    {"ZCLA",8,1,0},
    {"LAG",0,8,0},
};

BitfieldDef bitfieldWM8775_0f[] = {
    {"ZCRA",8,1,0},
    {"RAG",0,8,0},
};

BitfieldDef bitfieldWM8775_10[] = {
    {"LCSEL",7,2,0},
    {"MAXGAIN",4,3,0},
    {"LCT",0,4,0},
};

BitfieldDef bitfieldWM8775_11[] = {
    {"LCEN",8,1,0},
    {"ALCZC",7,1,0},
    {"HLD",0,4,0},
};

BitfieldDef bitfieldWM8775_12[] = {
    {"DCY",4,4,0},
    {"ATK",0,4,0},
};

BitfieldDef bitfieldWM8775_13[] = {
    {"NGTH",2,3,0},
    {"NGAT",0,1,0},
};

BitfieldDef bitfieldWM8775_14[] = {
    {"TRAINWIN",4,3,0},
    {"MAXATTEN",0,4,0},
};

void fm_WM8775_AMX(OutputFormatter &fmt,
		   unsigned long data,
		   unsigned long)
{
    int fl = 0;
    unsigned int idx;
    for (idx = 0; idx < 4; idx++) {
	if (data & (1 << idx)) {
	    if (fl) fmt.printf("+");
	    fmt.printf("AIN%u",idx+1);
	    fl = !0;
	}
    }
    if (!fl) {
	fmt.printf("<none>");
    }
}
BitfieldDef bitfieldWM8775_15[] = {
    {"LRBOTH",8,1,0},
    {"MUTELA",7,1,0},
    {"MUTERA",6,1,0},
    {"AMX",0,4,fm_WM8775_AMX},
};

BitfieldDef bitfieldWM8775_17[] = {
    {"RESET",0,9,0},
};

RegisterDef registerWM8775[] = {
    {0,0x7,DEFARR(bitfieldWM8775_07),0x1f7,0x000},
    {0,0xb,DEFARR(bitfieldWM8775_0b),0x080,0x000},
    {0,0xc,DEFARR(bitfieldWM8775_0c),0x0f0,0x000},
    {0,0xd,DEFARR(bitfieldWM8775_0d),0x1bc,0x008},
    {0,0xe,DEFARR(bitfieldWM8775_0e),0x000,0x000},
    {0,0xf,DEFARR(bitfieldWM8775_0f),0x000,0x000},
    {0,0x10,DEFARR(bitfieldWM8775_10),0x000,0x000},
    {0,0x11,DEFARR(bitfieldWM8775_11),0x070,0x000},
    {0,0x12,DEFARR(bitfieldWM8775_12),0x100,0x000},
    {0,0x13,DEFARR(bitfieldWM8775_13),0x1e2,0x000},
    {0,0x14,DEFARR(bitfieldWM8775_14),0x180,0x080},
    {0,0x15,DEFARR(bitfieldWM8775_15),0x030,0x000},
    {0,0x17,DEFARR(bitfieldWM8775_17),0x000,0x000},
};

RegisterTable tableWM8775 = {
    7,9,0,DEFARR(registerWM8775)
};

class I2CDeviceWM8775 : public I2CDeviceRegistered {
private:
    I2CDeviceWM8775(I2CCommand *_cmd)
	:I2CDeviceRegistered(_cmd) {
	addrFormat.size = 7;
	addrFormat.bigEndian = 0;
	dataFormat.size = 9;
	addrFormat.bigEndian = !0;
    }
    const char *getName(void) const {return "wm8775";}
    const RegisterTable *getRegisterTable(void) const {return &tableWM8775;}
    int isOk(void) const {
	if (cmd->getReadCnt()) return 0;
	if (cmd->getWriteCnt() != 2) return 0;
	return !0;
    }
    void getWriteAddrData(const BlockBuf &bb,unsigned int idx,
			  unsigned long &addr,
			  unsigned long &data) const {
	unsigned short d2 = bb.get16be(idx);
	addr = d2 >> 9;
	data = d2 & 0x1ffu;
    }
public:
    static I2CDeviceTransaction *alloc(I2CCommand *cp)
	{ return new I2CDeviceWM8775(cp);}
};


/*------------------------------------------------------------------------*/

class I2CDeviceTuner : public I2CDeviceTransaction {
    friend class I2CDeviceTransaction;
public:
    void describe(OutputFormatter &fmt) const;
private:
    I2CDeviceTuner(I2CCommand *);
    int okFl;
    unsigned short progWord;
    unsigned short controlWord;
    unsigned short statusWord;
    int readMode;
    int readValid;
public:
    static I2CDeviceTransaction *alloc(I2CCommand *cp)
	{ return new I2CDeviceTuner(cp);}
};


I2CDeviceTuner::I2CDeviceTuner(I2CCommand *_cmd)
    :I2CDeviceTransaction(_cmd),okFl(0),
     readMode(0),readValid(0)
{
    unsigned char writeCnt = cmd->getWriteCnt();
    unsigned char readCnt = cmd->getReadCnt();

    if (cmd->getWriteBlocks() != 1) return;

    if ((readCnt == 2) && (writeCnt == 0)) {
	const BlockBuf &bb = cmd->getReadBlockBuf();
	readMode = !0;
	okFl = !0;
	if (bb.getCount()) {
	    readValid = !0;
	    statusWord = bb.get16be(0);
	}
    } else if ((readCnt == 0) && (writeCnt == 4)) {
	const BlockBuf &bb = cmd->getWriteBlockBuf();
	progWord = bb.get16be(0);
	controlWord = bb.get16be(2);
	okFl = !0;
    }
}


void I2CDeviceTuner::describe(OutputFormatter &fmt) const
{
    fmt.startLine();
    fmt.printf("tuner");
    if (!okFl) {
        fmt.printf(" unexpected format ");
        cmd->describe(fmt);
        return;
    }
    if (readMode) {
        fmt.printf(" read status");
	if (readValid) {
	    fmt.printf("=%04x",statusWord);
	}
    } else {
        fmt.printf(" write prog=%04x control=%04x",
                   progWord,controlWord);
    }
    cmd->describeRetCode(fmt);
    fmt.endLine();
}


/*------------------------------------------------------------------------*/

class I2CDeviceTDA9887 : public I2CDeviceTransaction {
    friend class I2CDeviceTransaction;
public:
    void describe(OutputFormatter &fmt) const;
private:
    I2CDeviceTDA9887(I2CCommand *);
    int okFl;
    unsigned char d_b,d_c,d_e;
    unsigned char f_b,f_c,f_e;
    unsigned char readMode;
    unsigned char readValid;
    unsigned char readData;
public:
    static I2CDeviceTransaction *alloc(I2CCommand *cp)
	{ return new I2CDeviceTDA9887(cp);}
};


I2CDeviceTDA9887::I2CDeviceTDA9887(I2CCommand *_cmd)
    :I2CDeviceTransaction(_cmd),okFl(0),
     f_b(0),f_c(0),f_e(0),
     readMode(0),readValid(0)
{
    unsigned char writeCnt = cmd->getWriteCnt();
    unsigned char readCnt = cmd->getReadCnt();

    if (cmd->getWriteBlocks() != 1) return;

    if ((writeCnt > 1) && (writeCnt <= 4)) {
	const BlockBuf &bb = cmd->getWriteBlockBuf();
	unsigned char idx,offs;
	okFl = !0;
	offs = bb.get8(0);
	idx = 1;
	writeCnt--;
	switch (offs) {
	case 0:
	    d_b = bb.get8(idx++);
	    f_b = !0;
	    writeCnt--;
	    if (!writeCnt) break;
	case 1:
	    d_c = bb.get8(idx++);
	    f_c = !0;
	    writeCnt--;
	    if (!writeCnt) break;
	case 2:
	    d_e = bb.get8(idx++);
	    f_e = !0;
	    writeCnt--;
	    if (!writeCnt) break;
	}
    } else if (readCnt == 1) {
	const BlockBuf &bb = cmd->getReadBlockBuf();
	readMode = !0;
	okFl = !0;
	if (bb.getCount()) {
	    readValid = !0;
	    readData = bb.get8(0);
	}
    }
}


void I2CDeviceTDA9887::describe(OutputFormatter &fmt) const
{
    fmt.startLine();
    fmt.printf("tda9887");
    if (!okFl) {
        fmt.printf(" unexpected format ");
        cmd->describe(fmt);
        return;
    }
    if (readMode) {
        fmt.printf(" read status");
	if (readValid) {
	    fmt.printf("=%02x",readData);
	}
    } else {
        fmt.printf(" write");
	if (f_b) fmt.printf(" b=%02x",d_b);
	if (f_c) fmt.printf(" c=%02x",d_c);
	if (f_e) fmt.printf(" e=%02x",d_e);
    }
    cmd->describeRetCode(fmt);
    fmt.endLine();
}


/*------------------------------------------------------------------------*/

class I2CDeviceEEPROM : public I2CDeviceTransaction {
    friend class I2CDeviceTransaction;
    friend class I2CDeviceBatchEEPROM;
public:
    void describe(OutputFormatter &fmt) const;
    unsigned char getAddr(void) const;
    unsigned char getCnt(void) const;
private:
    I2CDeviceEEPROM(I2CCommand *);
    unsigned short addr;
    unsigned char cnt;
    int okFl;
    int mode16;
public:
    static I2CDeviceTransaction *alloc(I2CCommand *cp)
	{ return new I2CDeviceEEPROM(cp);}
};


I2CDeviceEEPROM::I2CDeviceEEPROM(I2CCommand *_cmd)
    :I2CDeviceTransaction(_cmd),addr(0),cnt(0),okFl(0)
{
    if (cmd->getWriteBlocks() != 1) return;
    switch (cmd->getWriteCnt()) {
    case 1:
	mode16 = 0;
	addr = cmd->getWriteBlockBuf().get8(0);
	break;
    case 2:
	mode16 = !0;
	addr = cmd->getWriteBlockBuf().get16be(0);
	break;
    default: return;
    }
    cnt = cmd->getReadCnt();
    okFl = !0;
}


void I2CDeviceEEPROM::describe(OutputFormatter &fmt) const
{
    const BlockBuf &bb = cmd->getReadBlockBuf();
    fmt.startLine();
    fmt.printf("eeprom fetch [%d bit addressing]",mode16 ? 16 : 8);
    if (!okFl) {
        fmt.printf(" unexpected format ");
        cmd->describe(fmt);
        return;
    }
    if (mode16) {
	fmt.printf(" addr=%04x",addr);
    } else {
	fmt.printf(" addr=%02x",addr);
    }
    bb.inlineDump(fmt,10," ");
    cmd->describeRetCode(fmt);
    fmt.endLine();
    if (bb.getCount() > 10) {
        IndentScope iScope(fmt,4);
        bb.dump(fmt);
    }
}


/*------------------------------------------------------------------------*/

typedef I2CDeviceTransaction *(*I2CCreateFunc)(I2CCommand *);

I2CCreateFunc i2cTable[256];

I2CDeviceTransaction *I2CDeviceTransaction::parse(Transaction *tp)
{
    I2CCommand *cmd = dynamic_cast<I2CCommand *>(tp);
    I2CDeviceTransaction *ntp;
    I2CCreateFunc func;
    if (!cmd) return 0;
    unsigned char i2cAddr = cmd->getAddr();
    func = i2cTable[i2cAddr];
    ntp = func ? (*func)(cmd) : 0;
    if (ntp) {
	ntp->tInsertBefore(tp);
	ntp->tAppend(tp);
    }
    return ntp;
}


/*------------------------------------------------------------------------*/

class I2CDeviceBatchEEPROM : public TransactionList {
    friend class I2CDeviceTransaction;
public:
    static I2CDeviceBatchEEPROM *parse(Transaction *);
    void describe(OutputFormatter &fmt) const;
    ~I2CDeviceBatchEEPROM(void);
    void subDescribe(OutputFormatter &) const;
    static int subEnableFl;
private:
    I2CDeviceBatchEEPROM(void);
    int mode16;
    unsigned short addr;
    unsigned int cnt;
};

int I2CDeviceBatchEEPROM::subEnableFl = 0;


void I2CDeviceBatchEEPROM::subDescribe(OutputFormatter &fmt) const
{
    if (!subEnableFl) return;
    TransactionList::subDescribe(fmt);
}


I2CDeviceBatchEEPROM::I2CDeviceBatchEEPROM(void)
    :addr(0),cnt(0)
{
    // Intentionally Empty
}


I2CDeviceBatchEEPROM::~I2CDeviceBatchEEPROM(void)
{
    // Intentionally Empty
}


void I2CDeviceBatchEEPROM::describe(OutputFormatter &fmt) const
{
    unsigned char writeCnt,readCnt;
    unsigned char buf[0x10000];
    unsigned int bcnt = 0;
    int readOkFl = !0;
    BlockBuf bb;
    I2CDeviceEEPROM *ep1;

    for (ep1 = dynamic_cast<I2CDeviceEEPROM *>(tGetFirst()); ep1;
	 ep1 = dynamic_cast<I2CDeviceEEPROM *>(ep1->tGetNext())) {
	if (bcnt + ep1->cnt > sizeof(buf)) break;
	const BlockBuf &brb = ep1->cmd->getReadBlockBuf();
	if (!brb.getPtr()) {
	    readOkFl = 0;
	    break;
	}
	brb.getData(buf+bcnt,ep1->cnt,0);
	bcnt += ep1->cnt;
    }
    bb.assign(buf,bcnt);
    fmt.startLine();
    fmt.printf("eeprom %d fetch addr=%02x (%d transaction(s))",
               mode16 ? 16 : 8,addr,tGetCount());
    if (readOkFl) bb.inlineDump(fmt,6," ");
    fmt.endLine();
    if (readOkFl && (bb.getCount() > 6)) {
	IndentScope iscope(fmt,4);
	bb.dump(fmt);
    }
    if (saveEepromName) {
	FILE *fp;
	DataBlock *bp;
	Transaction *tp;
	fp = fopen(saveEepromName,"wb");
	if (fp) {
	    fprintf(stderr,"Writing eeprom data to %s\n",saveEepromName);
	    fwrite(buf,bb.getCount(),1,fp);
	    fclose(fp);
	} else {
	    fprintf(stderr,"Failed to open %s for writing firmware data\n",
		    saveFirmwareName);
	}
    }
}


I2CDeviceBatchEEPROM *I2CDeviceBatchEEPROM::parse(Transaction *tp)
{
    int mode16;
    I2CDeviceBatchEEPROM *bp = 0;
    I2CDeviceEEPROM *ep1,*ep2;
    ep1 = dynamic_cast<I2CDeviceEEPROM *>(tp);
    if (!ep1) return 0;
    mode16 = ep1->mode16;
    if (!ep1->okFl) return 0;
    tp = ep1->tGetNext();
    for (;;) {
	if (!tp) break;
	ep2 = dynamic_cast<I2CDeviceEEPROM *>(tp);
	if (!ep2) break;
	if ((!ep2->mode16) != (!mode16)) break;
	if (!ep2->okFl) break;
	if (!bp) {
	    if (ep1->addr + ep1->cnt != ep2->addr) break;
	    bp = new I2CDeviceBatchEEPROM;
	    bp->mode16 = mode16;
	    bp->addr = ep1->addr;
	    bp->cnt = ep1->cnt;
	    bp->tInsertBefore(ep1);
	    bp->tAppend(ep1);
	}
	if (bp->addr + bp->cnt != ep2->addr) break;
	if (bp->cnt + ep2->cnt > 0x10000) break;
	bp->cnt += ep2->cnt;
	tp = ep2->tGetNext();
	bp->tAppend(ep2);
    }
    return bp;
}


/*------------------------------------------------------------------------*/

class I2CDeviceFirmwareCX25840 : public TransactionList {
public:
    static I2CDeviceFirmwareCX25840 *parse(Transaction *);
    ~I2CDeviceFirmwareCX25840(void);
    void describe(OutputFormatter &fmt) const;
    void subDescribe(OutputFormatter &) const;
    static int subEnableFl;
private:
    I2CDeviceFirmwareCX25840(void);
    unsigned int cnt;
    unsigned int num;
};

int I2CDeviceFirmwareCX25840::subEnableFl = 0;


void I2CDeviceFirmwareCX25840::subDescribe(OutputFormatter &fmt) const
{
    if (!subEnableFl) return;
    TransactionList::subDescribe(fmt);
}


I2CDeviceFirmwareCX25840::I2CDeviceFirmwareCX25840(void)
    :cnt(0),num(firmwareCount++)
{
    // Intentionally Empty
}


I2CDeviceFirmwareCX25840::~I2CDeviceFirmwareCX25840(void)
{
    // Intentionally Empty
}


static I2CDeviceCX25840 *firmwareCX25840FetchOp(Transaction *tp)
{
    unsigned int wcnt;
    I2CCommand *cmd;
    I2CDeviceCX25840 *bp = dynamic_cast<I2CDeviceCX25840 *>(tp);
    if (!bp) return 0;
    cmd = bp->getI2CCommand();
    if (!cmd) return 0;
    if (cmd->getReadCnt()) return 0;
    if (cmd->getWriteBlocks() != 1) return 0;
    wcnt = cmd->getWriteCnt();
    if (wcnt <= 2) return 0;
    const BlockBuf &bwb = cmd->getWriteBlockBuf();
    if (bwb.get16be(0) != 0x0802) return 0;
    return bp;
}


void I2CDeviceFirmwareCX25840::describe(OutputFormatter &fmt) const
{
    fmt.startLine();
    fmt.printf("cx25840 firmware upload size: 0x%x",cnt);
    if (tGetCount() > 1) {
        fmt.printf(" (%d transactions)",tGetCount());
    }
    fmt.endLine();
    if (saveFirmwareName) {
	char *newname;
	FILE *fp;
	I2CDeviceCX25840 *bp;
	Transaction *tp;
	// This is an ugly, ugly hack job.
	if (firmwareCount > 1) {
	    newname = new char[strlen(saveFirmwareName)+10+1];
	    sprintf(newname,"%s-%u",saveFirmwareName,num);
	} else {
	    newname = new char[strlen(saveFirmwareName)+1];
	    sprintf(newname,"%s",saveFirmwareName);
	}
	fp = fopen(newname,"wb");
	if (fp) {
	    for (tp = tGetFirst(); tp; tp = tp->tGetNext()) {
		bp = firmwareCX25840FetchOp(tp);
		if (!bp) continue;
		const BlockBuf &bb = bp->getI2CCommand()->getWriteBlockBuf();
		for (unsigned int offs = 2; offs < bb.getCount(); offs++) {
		    fputc(bb.get8(offs),fp);
		}
	    }
	    fclose(fp);
	    fprintf(stderr,"Wrote %u bytes of cx25840 firmware data to %s\n",
		    cnt,
		    newname);
	    delete[] newname;
	} else {
	    fprintf(stderr,"Failed to open %s for writing firmware data\n",
		    saveFirmwareName);
	}
    }
}


I2CDeviceFirmwareCX25840 *I2CDeviceFirmwareCX25840::parse(Transaction *tp)
{
    I2CDeviceCX25840 *bp = firmwareCX25840FetchOp(tp);
    Transaction *tp2;
    if (!bp) return 0;
    I2CDeviceFirmwareCX25840 *p = new I2CDeviceFirmwareCX25840;
    p->cnt = bp->getI2CCommand()->getWriteCnt()-2;
    tp2 = tp->tGetNext();
    p->tInsertBefore(tp);
    p->tAppend(tp);
    while (tp2) {
        bp = firmwareCX25840FetchOp(tp2);
        if (!bp) break;
        p->cnt += bp->getI2CCommand()->getWriteCnt()-2;
        tp = tp2;
        tp2 = tp2->tGetNext();
        p->tAppend(tp);
    }
    return p;
}


/*------------------------------------------------------------------------*/

Transaction *tryUSBCommand(Transaction *tp)
{
    return USBCommand::parse(tp);
}


Transaction *tryUSBNotedCommand(Transaction *tp)
{
    return USBNotedCommand::parse(tp);
}


Transaction *tryEncTransaction(Transaction *tp)
{
    return EncTransaction::parse(tp);
}


Transaction *tryEncoderCommand(Transaction *tp)
{
    return EncoderCommand::parse(tp);
}


Transaction *tryI2CCommand(Transaction *tp)
{
    return I2CCommand::parse(tp);
}


Transaction *tryRegisterCommand(Transaction *tp)
{
    return RegisterCommand::parse(tp);
}


Transaction *tryFirmwareUpload(Transaction *tp)
{
    return FirmwareUpload::parse(tp);
}


Transaction *tryI2CDeviceFirmwareCX25840(Transaction *tp)
{
    return I2CDeviceFirmwareCX25840::parse(tp);
}


Transaction *tryVideoData(Transaction *tp)
{
    return VideoData::parse(tp);
}


Transaction *tryI2CDeviceTransaction(Transaction *tp)
{
    return I2CDeviceTransaction::parse(tp);
}


Transaction *tryI2CDeviceBatchEEPROM(Transaction *tp)
{
    return I2CDeviceBatchEEPROM::parse(tp);
}


Transaction *tryTransactionRepeat(Transaction *tp)
{
    return TransactionRepeat::parse(tp);
}


typedef Transaction *(*ParseTransactionFunc)(Transaction *);

ParseTransactionFunc parseTransactionFuncs[] = {
    tryUSBCommand,
    tryEncTransaction,
    tryEncoderCommand,
    tryI2CCommand,
    tryRegisterCommand,
    tryFirmwareUpload,
    tryI2CDeviceFirmwareCX25840,
    tryI2CDeviceTransaction,
    tryI2CDeviceBatchEEPROM,
    tryVideoData,
    tryUSBNotedCommand,
    tryTransactionRepeat,
};

/*------------------------------------------------------------------------*/

Transaction *decompile1Step(Transaction *tp)
{
    unsigned int idx;
    Transaction *t2p;

    for (idx = 0;
         idx < ASZ(parseTransactionFuncs);
         idx++) {
        t2p = (*parseTransactionFuncs[idx])(tp);
        if (t2p) tp = t2p;
    }
    return tp;
}


unsigned int decompile(TransactionList &tl)
{
    unsigned int passCount = 0;
    Transaction *tp1;
    Transaction *tp2;
    int didSomethingFl;

    do {
        passCount++;
        didSomethingFl = 0;
        tp1 = tl.tGetFirst();
        while (tp1) {
            tp2 = decompile1Step(tp1);
            if (tp2 == tp1) {
                tp1 = tp1->tGetNext();
            } else {
                tp1 = tp2->tGetNext();
                didSomethingFl = !0;
            }
        }
    } while (didSomethingFl);

    return passCount;
}


/*------------------------------------------------------------------------*/

struct I2CAddrDef {
    unsigned char i2cAddr;
    I2CCreateFunc func;
};


struct I2CDeviceDef {
    const char *name;
    I2CAddrDef *def;
    unsigned int cnt;
};

struct I2CNameDef {
    const char *name;
    I2CCreateFunc func;
};


void populateI2CTable(I2CAddrDef *defPtr,unsigned int cnt)
{
    I2CAddrDef *dp;
    unsigned int idx;
    for (idx = 0; idx < cnt; idx++) {
	dp = defPtr + idx;
	i2cTable[dp->i2cAddr] = dp->func;
    }
}


// This table is used to map command line device names to the actual
// underlying create functions.  We also (inefficiently) use this to
// map back in the other direction when printing out the i2c table.
I2CNameDef defI2CNames[] = {
    {"wm8775",&I2CDeviceWM8775::alloc},
    {"eeprom",&I2CDeviceEEPROM::alloc},
    {"msp3400",&I2CDeviceMSP3400::alloc},
    {"tuner",&I2CDeviceTuner::alloc},
    {"tda9887",&I2CDeviceTDA9887::alloc},
    {"saa7115",&I2CDeviceSAA7115::alloc},
    {"cx25840",&I2CDeviceCX25840::alloc},
};


// Expected devices and their addresses when dealing with a 29xxx device.
I2CAddrDef defDeviceI2C29xxx[] = {
    {0x1b, &I2CDeviceWM8775::alloc},
    {0x50, &I2CDeviceEEPROM::alloc},
    {0x40, &I2CDeviceMSP3400::alloc},
    {0x61, &I2CDeviceTuner::alloc},
    {0x43, &I2CDeviceTDA9887::alloc},
    {0x21, &I2CDeviceSAA7115::alloc},
};

// Expected devices and their addresses when dealing with a 24xxx device.
I2CAddrDef defDeviceI2C24xxx[] = {
    {0x1b, &I2CDeviceWM8775::alloc},
    {0x51, &I2CDeviceEEPROM::alloc},
    {0x61, &I2CDeviceTuner::alloc},
    {0x43, &I2CDeviceTDA9887::alloc},
    {0x44, &I2CDeviceCX25840::alloc},
};

// Generic catch-all table; use this if you're trying to figure
// something out for the first time.
I2CAddrDef defDeviceI2CGeneric[] = {
    {0x1b, &I2CDeviceWM8775::alloc},
    {0x21, &I2CDeviceSAA7115::alloc},
    {0x40, &I2CDeviceMSP3400::alloc},
    {0x50, &I2CDeviceEEPROM::alloc},
    {0x51, &I2CDeviceEEPROM::alloc},
    {0x61, &I2CDeviceTuner::alloc},
    {0x43, &I2CDeviceTDA9887::alloc},
    {0x44, &I2CDeviceCX25840::alloc},
};

// Full table of device names and their corresponding I2C mapping.
I2CDeviceDef defI2CDevices[] = {
    {"29xxx",defDeviceI2C29xxx,ASZ(defDeviceI2C29xxx)},
    {"24xxx",defDeviceI2C24xxx,ASZ(defDeviceI2C24xxx)},
    {"Generic",defDeviceI2CGeneric,ASZ(defDeviceI2CGeneric)},
};


I2CDeviceDef *translateDeviceNameToDef(const char *name)
{
    unsigned int idx;
    I2CDeviceDef *dp;
    for (idx = 0; idx < ASZ(defI2CDevices);
	 idx++) {
	dp = defI2CDevices + idx;
	if (!strcmp(name,dp->name)) return dp;
    }
    return 0;
}


I2CCreateFunc translateI2CNameToFunc(const char *name)
{
    unsigned int idx;
    for (idx = 0; idx < ASZ(defI2CNames); idx++) {
	if (!strcmp(name,defI2CNames[idx].name)) {
	    return defI2CNames[idx].func;
	}
    }
    return 0;
}

const char *translateI2CFuncToName(I2CCreateFunc func)
{
    unsigned int idx;
    for (idx = 0; idx < ASZ(defI2CNames); idx++) {
	if (defI2CNames[idx].func == func) {
	    return defI2CNames[idx].name;
	}
    }
    return 0;
}

/*------------------------------------------------------------------------*/

void parseArg(int argc,char *argv[])
{
    int state = 0;
    unsigned char i2cAddr;
    I2CCreateFunc i2cFunc;
    I2CDeviceDef *devDef;
    const char *argp;
    char *str2ptr;
    while (argc--) {
	argp = *argv++;
	switch (state) {
	case 0:
	    if ((argp[0] != '-') || (argp[1] != '-')) {
		fprintf(stderr,"Don't understand argument %s\n",argp);
		exit(1);
	    }
	    argp+= 2;
	    if (!strcmp(argp,"firmware")) {
		state = 1;
		break;
	    }
	    if (!strcmp(argp,"eeprom")) {
		state = 2;
		break;
	    }
	    if (!strcmp(argp,"i2c")) {
		state = 3;
		break;
	    }
	    if (!strcmp(argp,"device")) {
		state = 4;
		break;
	    }
	    fprintf(stderr,"Don't understand option --%s\n",argp);
	    exit(1);
	case 1:
	    saveFirmwareName = argp;
	    state = 0;
	    break;
	case 2:
	    saveEepromName = argp;
	    state = 0;
	    break;
	case 3:
	    i2cAddr = strtoul(argp,&str2ptr,16);
	    if (*str2ptr != ':') {
		fprintf(stderr,"Don't understand argument %s to i2c option\n",
			argp);
		exit(1);
	    }
	    str2ptr++;
	    i2cFunc = 0;
	    if (strcmp(str2ptr,"none")) {
		i2cFunc = translateI2CNameToFunc(str2ptr);
		if (!i2cFunc) {
		    fprintf(stderr,
			    "No such defined i2c device \"%s\""
			    " in i2c option %s\n",
			    str2ptr,argp);
		    fprintf(stderr,
			    "Available i2c devices are:\n");
		    for (unsigned int idx = 0;
			 idx < ASZ(defI2CNames);
			 idx++) {
			fprintf(stderr,"  %s\n",defI2CNames[idx].name);
		    }
		    exit(1);
		}
	    }
	    i2cTable[i2cAddr] = i2cFunc;
	    state = 0;
	    break;
	case 4:
	    devDef = translateDeviceNameToDef(argp);
	    if (!devDef) {
		fprintf(stderr,
			"No such defined device type \"%s\"\n",argp);
		fprintf(stderr,
			"Available defined devices are:\n");
		for (unsigned int idx = 0;
		     idx < ASZ(defI2CDevices);
		     idx++) {
		    fprintf(stderr,"  %s\n",defI2CDevices[idx].name);
		}
		exit(1);
	    }
	    populateI2CTable(devDef->def,devDef->cnt);
	    state = 0;
	    break;
	}
    }
    if (state) {
	fprintf(stderr,"Option argument missing\n");
	exit(1);
    }
}


void fixupI2CTable(void)
{
    unsigned int idx;
    unsigned int cnt = 0;

    for (idx = 0; idx < 256; idx++) {
	if (!i2cTable[idx]) continue;
	cnt++;
    }
    if (!cnt) {
	fprintf(stderr,"Setting up Generic I2C mapping\n");
	populateI2CTable(defDeviceI2CGeneric,ASZ(defDeviceI2CGeneric));
    }
    fprintf(stderr,"Mapping of I2C address to device:\n");
    for (idx = 0; idx < 256; idx++) {
	if (!i2cTable[idx]) continue;
	if (!cnt) {
	}
	cnt++;
	fprintf(stderr,"  %02X: %s\n",
		idx,translateI2CFuncToName(i2cTable[idx]));
    }
}


int main(int argc,char *argv[])
{
    TransactionList tl;
    unsigned int passCount;
    unsigned int rawTransCount;
    unsigned int processedTransCount;
    parseArg(argc-1,argv+1);
    fprintf(stderr,"Mapping I2C devices...");
    fixupI2CTable();
    fprintf(stderr,"Reading input...");
    fflush(stderr);
    if (createBlocks(stdin,tl)) {
        return 1;
    }
    rawTransCount = tl.tGetCount();
    fprintf(stderr," found %d URBs\n",rawTransCount);
    fprintf(stderr,"Decompiling... ");
    fflush(stderr);
    passCount = decompile(tl);
    processedTransCount = tl.tGetCount();
    fprintf(stderr," deduced %d operations in %d pass(es)\n",
	    processedTransCount,passCount);
    fprintf(stderr,"Generating output...");
    fflush(stderr);
    OutputFormatter fmt(stdout);
    tl.describeWithLineNum(fmt);
    fprintf(stderr," done.\n");
#ifdef notdef
    fprintf(stderr,
            "Decompiled %d bus transactions into %d operations"
            " in %d pass(es)\n",rawTransCount,processedTransCount,passCount);
#endif
    return 0;
}
