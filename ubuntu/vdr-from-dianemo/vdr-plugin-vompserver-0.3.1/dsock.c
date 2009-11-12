/*
    Copyright 2004-2005 Chris Tallon

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

#include "dsock.h"

DatagramSocket::DatagramSocket()
{
  addrlen = sizeof(struct sockaddr);
  log = Log::getInstance();
  initted = 0;
}

DatagramSocket::~DatagramSocket()
{
  shutdown();
}

bool DatagramSocket::init(USHORT port)
{
  myPort = port;
  if ((socketnum = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
  {
    log->log("UDP", Log::CRIT, "Socket error");
    return false;
  }

  myAddr.sin_family = AF_INET;         // host byte order
  myAddr.sin_port = htons(myPort);     // short, network byte order
  myAddr.sin_addr.s_addr = INADDR_ANY; // auto-fill with my IP
  memset(&(myAddr.sin_zero), 0, 8);    // zero the rest of the struct
  if (bind(socketnum, (struct sockaddr *)&myAddr, addrlen) == -1)
  {
    log->log("UDP", Log::CRIT, "Bind error %u", myPort);
    close(socketnum);
    return false;
  }

  int allowed = 1;
  setsockopt(socketnum, SOL_SOCKET, SO_BROADCAST, &allowed, sizeof(allowed));

  FD_ZERO(&readfds);
  FD_SET(socketnum, &readfds);
  tv.tv_sec = 0;
  tv.tv_usec = 0;

  initted = 1;
  return true;
}

void DatagramSocket::shutdown()
{
  if (initted) close(socketnum);
  initted = 0;
}

unsigned char DatagramSocket::waitforMessage(unsigned char how)
{
  if (!initted) return 0;

  /* how = 0 - block
     how = 1 - start new wait
     how = 2 - continue wait
  */

  struct timeval* passToSelect = NULL;


  if (how == 0)
  {
    passToSelect = NULL;
  }
  else if (how == 1)
  {
    tv.tv_sec = 1;
    tv.tv_usec = 100000;
    passToSelect = &tv;
  }
  else if (how == 2)
  {
    if ((tv.tv_sec == 0) && (tv.tv_usec == 0))  // protection in case timer = 0
    {
      tv.tv_sec = 1;
      tv.tv_usec = 100000;
    }
    passToSelect = &tv;
  }
  FD_ZERO(&readfds);
  FD_SET(socketnum, &readfds);

  if (select(socketnum + 1, &readfds, NULL, NULL, passToSelect) <= 0) return 1;

  if ((mlength = recvfrom(socketnum, buf, MAXBUFLEN, 0, (struct sockaddr *)&theirAddr, &addrlen)) == -1)
  {
    log->log("UDP", Log::DEBUG, "recvfrom error");
    return 0;
  }
  else
  {
    memset(&buf[mlength], 0, MAXBUFLEN - mlength);
    strcpy(fromIPA, inet_ntoa(theirAddr.sin_addr));
    fromPort = ntohs(theirAddr.sin_port);
    //log->log("UDP", Log::DEBUG, "%s:%i received length %i", fromIPA, fromPort, mlength);
    return 2;
  }

  /* Return 0, failure
     Return 1, nothing happened, timer expired
     Return 2, packet arrived (timer not expired)
  */
}

void DatagramSocket::send(const char *ipa, USHORT port, char *message, int length)
{
  int sentLength = 0;

  //log->log("UDP", Log::DEBUG, "Send port %u", port);

  theirAddr.sin_family = AF_INET;      // host byte order
  theirAddr.sin_port = htons(port);    // short, network byte order
  struct in_addr tad;                  // temp struct tad needed to pass to theirAddr.sin_addr
  tad.s_addr = inet_addr(ipa);
  theirAddr.sin_addr = tad;            // address
  memset(&(theirAddr.sin_zero), 0, 8); // zero the rest of the struct

  sentLength = sendto(socketnum, message, length, 0, (struct sockaddr *)&theirAddr, addrlen);
  if (sentLength == length)
  {
    //log->log("UDP", Log::DEBUG, "%s:%u sent length %i", ipa, port, length);
    return;
  }

  log->log("UDP", Log::DEBUG, "%s:%u send failed %i", ipa, port, length);

  sentLength = sendto(socketnum, message, length, 0, (struct sockaddr *)&theirAddr, addrlen);
  if (sentLength == length)
  {
    log->log("UDP", Log::DEBUG, "%s:%u sent length %i 2nd try", ipa, port, length);
    return;
  }

  log->log("UDP", Log::DEBUG, "%s:%u send failed %i 2nd try", ipa, port, length);
}

ULONG DatagramSocket::getMyIP(ULONG targetIP)
{
  // More friendly interface to below, takes and returns IP in network order

  struct in_addr stargetIP;
  stargetIP.s_addr = targetIP;
  struct in_addr ret = myIPforIP(stargetIP);
  return ret.s_addr;
}

struct in_addr DatagramSocket::myIPforIP(struct in_addr targetIP)
{
  // This function takes a target IP on the network and returns the local IP
  // that would be used to communicate with it.
  // This function is static.

  struct in_addr fail;
  fail.s_addr = 0;

  int zSocket;
  if ((zSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
  {
    Log::getInstance()->log("UDP", Log::CRIT, "Socket error");
    return fail;
  }

  struct sockaddr_in zTarget;
  zTarget.sin_family = AF_INET;
  zTarget.sin_port = htons(3024); // arbitrary
  zTarget.sin_addr.s_addr = targetIP.s_addr;
  memset(&(zTarget.sin_zero), 0, 8);

  if (connect(zSocket, (struct sockaddr *)&zTarget, sizeof(struct sockaddr)) == -1)
  {
    Log::getInstance()->log("UDP", Log::CRIT, "Connect error");
    close(zSocket);
    return fail;
  }

  struct sockaddr_in zSource;
  socklen_t zSourceLen = sizeof(struct sockaddr_in);
  memset(&zSource, 0, zSourceLen);

  if (getsockname(zSocket, (struct sockaddr*)&zSource, &zSourceLen) == -1)
  {
    Log::getInstance()->log("UDP", Log::CRIT, "Getsockname error");
    close(zSocket);
    return fail;
  }

  close(zSocket);
  return zSource.sin_addr;
}
