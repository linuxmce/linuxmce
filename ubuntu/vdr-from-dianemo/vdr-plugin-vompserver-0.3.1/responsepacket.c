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

#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

#include "responsepacket.h"
#include "log.h"

/* Packet format for an RR channel response:

4 bytes = channel ID = 1 (request/response channel)
4 bytes = request ID (from serialNumber)
4 bytes = length of the rest of the packet
? bytes = rest of packet. depends on packet
*/

ResponsePacket::ResponsePacket()
{
  buffer = NULL;
  bufSize = 0;
  bufUsed = 0;
}

ResponsePacket::~ResponsePacket()
{
  if (buffer) free(buffer);
}

bool ResponsePacket::init(ULONG requestID)
{
  if (buffer) return false;
  
  bufSize = 512;
  buffer = (UCHAR*)malloc(bufSize);
  if (!buffer) return false;
  
  *(ULONG*)&buffer[0] = htonl(1); // RR channel
  *(ULONG*)&buffer[4] = htonl(requestID);
  *(ULONG*)&buffer[userDataLenPos] = 0;
  bufUsed = headerLength;

  return true;
}

void ResponsePacket::finalise()
{
  *(ULONG*)&buffer[userDataLenPos] = htonl(bufUsed - headerLength);
  //Log::getInstance()->log("Client", Log::DEBUG, "RP finalise %lu", bufUsed - headerLength);
}

bool ResponsePacket::copyin(const UCHAR* src, ULONG len)
{
  if (!checkExtend(len)) return false;
  memcpy(buffer + bufUsed, src, len);
  bufUsed += len;
  return true;
}

bool ResponsePacket::addString(const char* string)
{
  ULONG len = strlen(string) + 1;
  if (!checkExtend(len)) return false;
  memcpy(buffer + bufUsed, string, len);
  bufUsed += len;
  return true;
}

bool ResponsePacket::addULONG(ULONG ul)
{
  if (!checkExtend(sizeof(ULONG))) return false;
  *(ULONG*)&buffer[bufUsed] = htonl(ul);
  bufUsed += sizeof(ULONG);
  return true;
}  

bool ResponsePacket::addUCHAR(UCHAR c)
{
  if (!checkExtend(sizeof(UCHAR))) return false;
  buffer[bufUsed] = c;
  bufUsed += sizeof(UCHAR);
  return true;
}  
  
bool ResponsePacket::addLONG(LONG l)
{
  if (!checkExtend(sizeof(LONG))) return false;
  *(LONG*)&buffer[bufUsed] = htonl(l);
  bufUsed += sizeof(LONG);
  return true;
}

bool ResponsePacket::addULLONG(ULLONG ull)
{
  if (!checkExtend(sizeof(ULLONG))) return false;
  *(ULLONG*)&buffer[bufUsed] = htonll(ull);
  bufUsed += sizeof(ULLONG);
  return true;
}

bool ResponsePacket::checkExtend(ULONG by)
{
  if ((bufUsed + by) < bufSize) return true;
  if (512 > by) by = 512;
  UCHAR* newBuf = (UCHAR*)realloc(buffer, bufSize + by);
  if (!newBuf) return false;
  buffer = newBuf;
  bufSize += by;
  return true;
}

ULLONG ResponsePacket::htonll(ULLONG a)
{
  #if BYTE_ORDER == BIG_ENDIAN
    return a;
  #else
    ULLONG b = 0;

    b = ((a << 56) & 0xFF00000000000000ULL)
      | ((a << 40) & 0x00FF000000000000ULL)
      | ((a << 24) & 0x0000FF0000000000ULL)
      | ((a <<  8) & 0x000000FF00000000ULL)
      | ((a >>  8) & 0x00000000FF000000ULL)
      | ((a >> 24) & 0x0000000000FF0000ULL)
      | ((a >> 40) & 0x000000000000FF00ULL)
      | ((a >> 56) & 0x00000000000000FFULL) ;

    return b;
  #endif
}

