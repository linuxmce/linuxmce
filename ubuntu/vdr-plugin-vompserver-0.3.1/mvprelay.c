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

#include "mvprelay.h"

MVPRelay::MVPRelay()
{
}

MVPRelay::~MVPRelay()
{
  shutdown();
}

int MVPRelay::shutdown()
{
  if (threadIsActive()) threadCancel();

  return 1;
}

int MVPRelay::run()
{
  if (threadIsActive()) return 1;

  if (!ds.init(16881))
  {
    Log::getInstance()->log("MVPRelay", Log::CRIT, "Could not open UDP 16881");
    shutdown();
    return 0;
  }

  if (!threadStart())
  {
    shutdown();
    return 0;
  }

  Log::getInstance()->log("MVPRelay", Log::DEBUG, "MVPRelay replier started");
  return 1;
}

void MVPRelay::threadMethod()
{
  int retval;
  while(1)
  {
    retval = ds.waitforMessage(0);
    if (retval == 1) continue;

    Log::getInstance()->log("MVPRelay", Log::DEBUG, "MVPRelay request from %s", ds.getFromIPA());

//    TCP::dump((UCHAR*)ds.getData(), ds.getDataLength());

    UCHAR* in = (UCHAR*)ds.getData();

    // Check incoming packet magic number

    ULONG inMagic = ntohl(*(ULONG*)&in[4]);
    if (inMagic != 0xbabefafe)
    {
      Log::getInstance()->log("MVPRelay", Log::DEBUG, "inMagic not correct");
      continue;
    }

    // Get peer info
    USHORT peerPort = ntohs(*(USHORT*)&in[20]);

    // Get my IP for this connection
    ULONG myIP = ds.getMyIP(*(ULONG*)&in[16]);
    Log::getInstance()->log("MVPRelay", Log::DEBUG, "Sending my IP as %x", ntohl(myIP));

    // Required return parameters:
    // -Seq number
    // -Magic number
    // -Client IP & port
    // -First server IP

    // Construct reply packet

    UCHAR out[52];
    memset(out, 0, 52);

    // Copy sequence number
    memcpy(out, in, 4);

    // Return magic number is 0xfafebabe
    *(ULONG*)&out[4] = htonl(0xfafebabe);

    // Copy client IP and port to reply
    memcpy(&out[16], &in[16], 6);

    // Insert server address
    *(ULONG*)&out[24] = myIP;

    // Send it
    ds.send(ds.getFromIPA(), peerPort, (char*)out, 52);

//    TCP::dump(out, 52);
  }
}
