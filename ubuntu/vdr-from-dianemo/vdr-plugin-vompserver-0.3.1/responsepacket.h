/*
    Copyright 2007 Chris Tallon

    This file is part of VOMP.

    VOMP is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    VOMP is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VOMP; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef RESPONSEPACKET_H
#define RESPONSEPACKET_H

#include "defines.h"

class ResponsePacket
{
  public:
    ResponsePacket();
    ~ResponsePacket();
    
    bool init(ULONG requestID);
    void finalise();
    bool copyin(const UCHAR* src, ULONG len);
    bool addString(const char* string);
    bool addULONG(ULONG ul);
    bool addLONG(LONG l);
    bool addUCHAR(UCHAR c);
    bool addULLONG(ULLONG ull);

    UCHAR* getPtr() { return buffer; }
    ULONG getLen() { return bufUsed; }
    
  private:

    UCHAR* buffer;
    ULONG bufSize;
    ULONG bufUsed;

    bool checkExtend(ULONG by);
    ULLONG htonll(ULLONG a);
    
    const static ULONG headerLength = 12;
    const static ULONG userDataLenPos = 8;
};

#endif

