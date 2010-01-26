/*
    Copyright 2006 Chris Tallon

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

#include "bootpd.h"

//void dump(unsigned char* data, USHORT size);
//unsigned char dcc(UCHAR c);

Bootpd::Bootpd()
{
  log = Log::getInstance();
}

Bootpd::~Bootpd()
{
  shutdown();
}

int Bootpd::shutdown()
{
  if (threadIsActive()) threadCancel();
  ds.shutdown();

  return 1;
}

int Bootpd::run(const char* tconfigDir)
{
  if (threadIsActive()) return 1;
  log->log("BOOTPD", Log::DEBUG, "Starting bootpd");

  configDir = tconfigDir;

  if (!ds.init(16867))
  {
    log->log("BOOTPD", Log::DEBUG, "DSock init error");
    shutdown();
    return 0;
  }

  if (!threadStart())
  {
    log->log("BOOTPD", Log::DEBUG, "Thread start error");
    shutdown();
    return 0;
  }

  log->log("BOOTPD", Log::DEBUG, "Bootp replier started");
  return 1;
}

void Bootpd::threadMethod()
{
  int retval;
  while(1)
  {
    log->log("BOOTPD", Log::DEBUG, "Starting wait");
    retval = ds.waitforMessage(0);
    log->log("BOOTPD", Log::DEBUG, "Wait finished");

    if (retval == 0)
    {
      log->log("BOOTPD", Log::CRIT, "Wait for packet error");
      return;
    }
    else if (retval == 1)
    {
      continue;
    }
    else
    {
      processRequest((UCHAR*)ds.getData(), ds.getDataLength());
    }
  }
}

void Bootpd::processRequest(UCHAR* data, int length)
{
  log->log("BOOTPD", Log::DEBUG, "Got request");
//  dump(data, (USHORT)length);

  if (data[0] != 1) return;  // Check it's a request

  // Open a config file for the given MAC

#ifndef VOMPSTANDALONE
  const char* useConfigDir = configDir;
#else
  const char* useConfigDir = ".";
#endif
  if (!useConfigDir)
  {
    log->log("BOOTPD", Log::ERR, "No config dir!");
    return;
  }

  Config config;
  char configFileName[PATH_MAX];
  snprintf(configFileName, PATH_MAX, "%s/vomp-%02X-%02X-%02X-%02X-%02X-%02X.conf", useConfigDir, data[28], data[29], data[30], data[31], data[32], data[33]);
  if (config.init(configFileName))
  {
    log->log("BOOTPD", Log::DEBUG, "Opened config file: %s", configFileName);
  }
  else
  {
    log->log("BOOTPD", Log::ERR, "Could not open/create config file: %s", configFileName);
    return;
  }

  // Get an IP for the MVP (make a local copy so future returns don't all have to free string)
  char newClientIP[100];
  newClientIP[0] = '\0';
  bool configHasIP = false;
  char* cfnewClientIP = config.getValueString("Boot", "IP");
  if (cfnewClientIP)
  {
    strncpy(newClientIP, cfnewClientIP, 99);
    delete[] cfnewClientIP;
    configHasIP = true;

    log->log("BOOTPD", Log::DEBUG, "Found IP %s for MVP", newClientIP);
  }
  else
  {
    log->log("BOOTPD", Log::WARN, "No IP found for MVP. Hopefully it has one already...");
  }

  // See if we should enforce the IP from the config file
  int failure;
  long enforceConfigIP = config.getValueLong("Boot", "Override IP", &failure);
  if (newClientIP[0] && enforceConfigIP)
  {
    log->log("BOOTPD", Log::DEBUG, "Will enforce IP %s on MVP even if it already has another", newClientIP);
  }
  else
  {
    log->log("BOOTPD", Log::DEBUG, "Will not change MVP IP if it already has one");
  }

  // See if the MVP already has an IP
  bool clientAlreadyHasIP = (data[12] || data[13] || data[14] || data[15]);

  /*
  Subset of Bootp protocol for MVP

  Incoming fields:

  Opcode                    0              Check for value 1            Set to value 2
  Hardware type             1
  Hardware address length   2
  Hop count                 3
  Transaction ID            4-7
  Num seconds               8-9
  Flags                     10-11
  Client IP                 12-15
  Your IP                   16-19                                       Set to MVP IP
  Server IP                 20-23                                       Set to server IP
  Gateway IP                24-27
  Client HW address         28-43  (16)    Use to lookup IP for MVP
  Server Host Name          44-107 (64)
  Boot filename             108-235 (128)                               Fill with filename for TFTP
  Vendor info               236-299 (64)

  IP Possibilities

  mvpAlreadyHasIP    configHasIP    enforceConfigIP    result
1 Y        0                0                0             discard
2 Y        0                0                1             discard
3 Y        0                1                0             set config ip
4 Y        0                1                1             set config ip
5 Y        1                0                0             set mvp ip
6 Y        1                0                1             set enforce false, set mvp ip
7 Y        1                1                0             set mvp ip
8 Y        1                1                1             set config ip
  */

  if (!clientAlreadyHasIP && !configHasIP) // cases 1 & 2
  {
    log->log("BOOTPD", Log::DEBUG, "No IP found to give to MVP");
    return;
  }

  if (!configHasIP) enforceConfigIP = 0;             // case 6

  // Ok, we will send a reply

  in_addr_t finalMVPIP;

  if ((!clientAlreadyHasIP) || (configHasIP && enforceConfigIP))
  {
    // Cases 3 & 4,              case 8
    log->log("BOOTPD", Log::DEBUG, "Giving MVP IP from config");
    // Set config ip
    *((in_addr_t*)&data[16]) = inet_addr(newClientIP);
    finalMVPIP = *((in_addr_t*)&data[16]);
  }
  else
  {
    // Cases 5 & 7
    // copy existing ciaddr to yiaddr?
    log->log("BOOTPD", Log::DEBUG, "Leave YI=0 as MVP already has good IP");
    finalMVPIP = *((in_addr_t*)&data[12]);
  }

  // Set Server IP in packet
  if (!getmyip(finalMVPIP, (in_addr_t*)&data[20]))
  {
    log->log("BOOTPD", Log::ERR, "Get my IP failed");
  }

  // Set filename
  char* tftpFileName = config.getValueString("Boot", "TFTP file name");
  if (tftpFileName)
  {
    strncpy((char*)&data[108], tftpFileName, 127);
    delete[] tftpFileName;
  }
  else
  {
    strncpy((char*)&data[108], "vomp-dongle", 127);
    config.setValueString("Boot", "TFTP file name", "vomp-dongle");
  }

  // set to reply
  data[0] = 2;

//  dump(data, (USHORT)length);

  ds.send("255.255.255.255", 16868, (char*)data, length);
}


int Bootpd::getmyip(in_addr_t destination, in_addr_t* result)
{
  int sockfd, r1, r2;
  struct sockaddr_in my_addr;
  struct sockaddr_in dest_addr;
  socklen_t my_addr_len = sizeof(struct sockaddr_in);

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd == -1) return 0;

  dest_addr.sin_family = AF_INET;
  dest_addr.sin_port = htons(1);
  dest_addr.sin_addr.s_addr = destination;
  memset(&(dest_addr.sin_zero), 0, 8);

  r1 = connect(sockfd, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr));
  if (r1 == -1)
  {
    close(sockfd);
    return 0;
  }

  memset(&my_addr, 0, sizeof(struct sockaddr_in));
  r2 = getsockname(sockfd, (struct sockaddr *)&my_addr, &my_addr_len);

  close(sockfd);

  if (r2 == -1) return 0;

  *result = my_addr.sin_addr.s_addr;
  return 1;
}

/*
void dump(unsigned char* data, USHORT size)
{
  printf("Size = %u\n", size);

  USHORT c = 0;
  while(c < size)
  {
    if ((size - c) > 15)
    {
      printf(" %02X %02X %02X %02X  %02X %02X %02X %02X  %02X %02X %02X %02X  %02X %02X %02X %02X  %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n",
        data[c], data[c+1], data[c+2], data[c+3], data[c+4], data[c+5], data[c+6], data[c+7],
        data[c+8], data[c+9], data[c+10], data[c+11], data[c+12], data[c+13], data[c+14], data[c+15],
        dcc(data[c]), dcc(data[c+1]), dcc(data[c+2]), dcc(data[c+3]), dcc(data[c+4]), dcc(data[c+5]), dcc(data[c+6]), dcc(data[c+7]),
        dcc(data[c+8]), dcc(data[c+9]), dcc(data[c+10]), dcc(data[c+11]), dcc(data[c+12]), dcc(data[c+13]), dcc(data[c+14]), dcc(data[c+15]));
      c += 16;
    }
    else
    {
      switch (size - c)
      {
        case 15:
          printf(" %02X %02X %02X %02X  %02X %02X %02X %02X  %02X %02X %02X %02X  %02X %02X %02X     %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n",
            data[c], data[c+1], data[c+2], data[c+3], data[c+4], data[c+5], data[c+6], data[c+7],
            data[c+8], data[c+9], data[c+10], data[c+11], data[c+12], data[c+13], data[c+14],
            dcc(data[c]), dcc(data[c+1]), dcc(data[c+2]), dcc(data[c+3]), dcc(data[c+4]), dcc(data[c+5]), dcc(data[c+6]), dcc(data[c+7]),
            dcc(data[c+8]), dcc(data[c+9]), dcc(data[c+10]), dcc(data[c+11]), dcc(data[c+12]), dcc(data[c+13]), dcc(data[c+14]));
          c += 15;
          break;
        case 14:
          printf(" %02X %02X %02X %02X  %02X %02X %02X %02X  %02X %02X %02X %02X  %02X %02X        %c%c%c%c%c%c%c%c%c%c%c%c%c%c\n",
            data[c], data[c+1], data[c+2], data[c+3], data[c+4], data[c+5], data[c+6], data[c+7],
            data[c+8], data[c+9], data[c+10], data[c+11], data[c+12], data[c+13],
            dcc(data[c]), dcc(data[c+1]), dcc(data[c+2]), dcc(data[c+3]), dcc(data[c+4]), dcc(data[c+5]), dcc(data[c+6]), dcc(data[c+7]),
            dcc(data[c+8]), dcc(data[c+9]), dcc(data[c+10]), dcc(data[c+11]), dcc(data[c+12]), dcc(data[c+13]));
          c += 14;
          break;
        case 13:
          printf(" %02X %02X %02X %02X  %02X %02X %02X %02X  %02X %02X %02X %02X  %02X           %c%c%c%c%c%c%c%c%c%c%c%c%c\n",
            data[c], data[c+1], data[c+2], data[c+3], data[c+4], data[c+5], data[c+6], data[c+7],
            data[c+8], data[c+9], data[c+10], data[c+11], data[c+12],
            dcc(data[c]), dcc(data[c+1]), dcc(data[c+2]), dcc(data[c+3]), dcc(data[c+4]), dcc(data[c+5]), dcc(data[c+6]), dcc(data[c+7]),
            dcc(data[c+8]), dcc(data[c+9]), dcc(data[c+10]), dcc(data[c+11]), dcc(data[c+12]));
          c += 13;
          break;
        case 12:
          printf(" %02X %02X %02X %02X  %02X %02X %02X %02X  %02X %02X %02X %02X               %c%c%c%c%c%c%c%c%c%c%c%c\n",
            data[c], data[c+1], data[c+2], data[c+3], data[c+4], data[c+5], data[c+6], data[c+7],
            data[c+8], data[c+9], data[c+10], data[c+11],
            dcc(data[c]), dcc(data[c+1]), dcc(data[c+2]), dcc(data[c+3]), dcc(data[c+4]), dcc(data[c+5]), dcc(data[c+6]), dcc(data[c+7]),
            dcc(data[c+8]), dcc(data[c+9]), dcc(data[c+10]), dcc(data[c+11]));
          c += 12;
          break;
        case 11:
          printf(" %02X %02X %02X %02X  %02X %02X %02X %02X  %02X %02X %02X                  %c%c%c%c%c%c%c%c%c%c%c\n",
            data[c], data[c+1], data[c+2], data[c+3], data[c+4], data[c+5], data[c+6], data[c+7],
            data[c+8], data[c+9], data[c+10],
            dcc(data[c]), dcc(data[c+1]), dcc(data[c+2]), dcc(data[c+3]), dcc(data[c+4]), dcc(data[c+5]), dcc(data[c+6]), dcc(data[c+7]),
            dcc(data[c+8]), dcc(data[c+9]), dcc(data[c+10]));
          c += 11;
          break;
        case 10:
          printf(" %02X %02X %02X %02X  %02X %02X %02X %02X  %02X %02X                     %c%c%c%c%c%c%c%c%c%c\n",
            data[c], data[c+1], data[c+2], data[c+3], data[c+4], data[c+5], data[c+6], data[c+7],
            data[c+8], data[c+9],
            dcc(data[c]), dcc(data[c+1]), dcc(data[c+2]), dcc(data[c+3]), dcc(data[c+4]), dcc(data[c+5]), dcc(data[c+6]), dcc(data[c+7]),
            dcc(data[c+8]), dcc(data[c+9]));
          c += 10;
          break;
        case 9:
          printf(" %02X %02X %02X %02X  %02X %02X %02X %02X  %02X                        %c%c%c%c%c%c%c%c%c\n",
            data[c], data[c+1], data[c+2], data[c+3], data[c+4], data[c+5], data[c+6], data[c+7],
            data[c+8],
            dcc(data[c]), dcc(data[c+1]), dcc(data[c+2]), dcc(data[c+3]), dcc(data[c+4]), dcc(data[c+5]), dcc(data[c+6]), dcc(data[c+7]),
            dcc(data[c+8]));
          c += 9;
          break;
        case 8:
          printf(" %02X %02X %02X %02X  %02X %02X %02X %02X                            %c%c%c%c%c%c%c%c\n",
            data[c], data[c+1], data[c+2], data[c+3], data[c+4], data[c+5], data[c+6], data[c+7],
            dcc(data[c]), dcc(data[c+1]), dcc(data[c+2]), dcc(data[c+3]), dcc(data[c+4]), dcc(data[c+5]), dcc(data[c+6]), dcc(data[c+7]));
          c += 8;
          break;
        case 7:
          printf(" %02X %02X %02X %02X  %02X %02X %02X                               %c%c%c%c%c%c%c\n",
            data[c], data[c+1], data[c+2], data[c+3], data[c+4], data[c+5], data[c+6],
            dcc(data[c]), dcc(data[c+1]), dcc(data[c+2]), dcc(data[c+3]), dcc(data[c+4]), dcc(data[c+5]), dcc(data[c+6]));
          c += 7;
          break;
        case 6:
          printf(" %02X %02X %02X %02X  %02X %02X                                  %c%c%c%c%c%c\n",
            data[c], data[c+1], data[c+2], data[c+3], data[c+4], data[c+5],
            dcc(data[c]), dcc(data[c+1]), dcc(data[c+2]), dcc(data[c+3]), dcc(data[c+4]), dcc(data[c+5]));
          c += 6;
          break;
        case 5:
          printf(" %02X %02X %02X %02X  %02X                                     %c%c%c%c%c\n",
            data[c], data[c+1], data[c+2], data[c+3], data[c+4],
            dcc(data[c]), dcc(data[c+1]), dcc(data[c+2]), dcc(data[c+3]), dcc(data[c+4]));
          c += 5;
          break;
        case 4:
          printf(" %02X %02X %02X %02X                                         %c%c%c%c\n",
            data[c], data[c+1], data[c+2], data[c+3],
            dcc(data[c]), dcc(data[c+1]), dcc(data[c+2]), dcc(data[c+3]));
          c += 4;
          break;
        case 3:
          printf(" %02X %02X %02X                                            %c%c%c\n",
            data[c], data[c+1], data[c+2],
            dcc(data[c]), dcc(data[c+1]), dcc(data[c+2]));
          c += 3;
          break;
        case 2:
          printf(" %02X %02X                                               %c%c\n",
            data[c], data[c+1],
            dcc(data[c]), dcc(data[c+1]));
          c += 2;
          break;
        case 1:
          printf(" %02X                                                  %c\n",
            data[c],
            dcc(data[c]));
          c += 1;
          break;
      }
    }
  }
}

unsigned char dcc(UCHAR c)
{
  if (isspace(c)) return ' ';
  if (isprint(c)) return c;
  return '.';
}
*/
