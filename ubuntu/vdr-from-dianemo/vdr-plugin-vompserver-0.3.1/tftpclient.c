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

#include "tftpclient.h"

TftpClient::TftpClient()
{
  log = Log::getInstance();
  state = 0;
  blockNumber = 1;
  lastCom = 0;
}

TftpClient::~TftpClient()
{
  shutdown();
}

int TftpClient::shutdown()
{
  if (threadIsActive()) threadCancel();
  ds.shutdown();

  if (file)
  {
    fclose(file);
    file = NULL;
  }

  log->log("TftpClient", Log::DEBUG, "Shutdown");

  return 1;
}

int TftpClient::run(char* tbaseDir, char* tpeerIP, USHORT tpeerPort, UCHAR* data, int length)
{
  if (threadIsActive()) return 1;
  log->log("TftpClient", Log::DEBUG, "Client handler started");

  baseDir = tbaseDir;

  strncpy(peerIP, tpeerIP, 16);
  peerPort = tpeerPort;

  if (length > 599) return 0;
  bufferLength = length;
  memcpy(buffer, data, length);

  if (!ds.init(0))
  {
    log->log("TftpClient", Log::DEBUG, "DSock init error");
    shutdown();
    return 0;
  }

  if (!threadStart())
  {
    log->log("TftpClient", Log::DEBUG, "Thread start error");
    shutdown();
    return 0;
  }

  return 1;
}

void TftpClient::threadMethod()
{
  threadDetach();

  // process the first message received by the parent listener
  // the first incoming message is placed in buffer by above run method
  if (!processMessage(buffer, bufferLength))
  {
    log->log("TftpClient", Log::INFO, "threadMethod terminating connection");
    return;
  }

  int retval;

  for(int counter = 0; counter < 10; counter++)
  {
//    log->log("TftpClient", Log::DEBUG, "Starting wait");
    // timer system to expire after x seconds
    retval = ds.waitforMessage(1);
//    log->log("TftpClient", Log::DEBUG, "Wait finished");

    if (retval == 0)
    {
      log->log("TftpClient", Log::CRIT, "Wait for packet error");
      return;
    }
    else if (retval == 1)
    {
      // 1s timer expired
      // see if we need to retransmit a data packet
      if (((state == 1) || (state == 2)) && (lastCom < (time(NULL) - 1)))
      {
        log->log("TftpClient", Log::DEBUG, "Retransmitting buffer");
        transmitBuffer();
      }

      continue;
    }
    else
    {
      if (strcmp(ds.getFromIPA(), peerIP))
      {
        log->log("TftpClient", Log::ERR, "Not my client IP");
        continue; // not from my client's IP
      }

      if (ds.getFromPort() != peerPort)
      {
        log->log("TftpClient", Log::ERR, "Not my client port %i %u", ds.getFromPort(), peerPort);
        continue; // not from my client's port
      }

      if (!processMessage((UCHAR*)ds.getData(), ds.getDataLength()))
      {
        log->log("TftpClient", Log::INFO, "processMessage terminating connection");
        return;
      }

      counter = 0; // that was a valid packet, reset the counter
    }
  }
  log->log("TftpClient", Log::DEBUG, "Lost connection, exiting");
}

void TftpClient::threadPostStopCleanup()
{
//  log->log("TftpClient", Log::DEBUG, "Deleting tftpclient");
//  delete this; // careful
}

int TftpClient::processMessage(UCHAR* data, int length)
{
//  log->log("TftpClient", Log::DEBUG, "Got request");
//  dump(data, (USHORT)length);

  if ((UINT)length < sizeof(USHORT)) return 0;
  USHORT opcode = ntohs(*(USHORT*)data);
  data += sizeof(USHORT);
  length -= sizeof(USHORT);

  switch(opcode)
  {
    case 1: // Read request
    {
      if (!processReadRequest(data, length)) return 0;
      break;
    }
    case 2: // Write request
    {
      log->log("TftpClient", Log::ERR, "Client wanted to send us a file!");
      return 0; // quit
    }
    case 3: // Data
    {
      log->log("TftpClient", Log::ERR, "Client sent a data packet!");
      return 0; // quit
    }
    case 4: // Ack
    {
      if (!processAck(data, length)) return 0;
      break;
    }
    case 5: // Error
    {
      break;
    }

    default:
    {
      log->log("TftpClient", Log::ERR, "Client TFTP protocol error");
      return 0;
    }
  }

  return 1;
}

int TftpClient::processReadRequest(UCHAR* data, int length)
{
  if (state != 0) return 0;

  // Safety checking - there should be two nulls in the data/length

  int nullsFound = 0;
  for(int i = 0; i < length; i++)
  {
    if (data[i] == '\0') nullsFound++;
  }

  if (nullsFound != 2) return 0;

  char* filename = (char*)data;
  char* mode = (char*)(data + strlen(filename) + 1);

  log->log("TftpClient", Log::DEBUG, "RRQ received for %s", filename);

  if (strcasecmp(mode, "octet")) return 0;
  if (!openFile(filename)) return 0;
  if (!sendBlock()) return 0;

  lastCom = time(NULL);
  state = 1;

  return 1;
}

int TftpClient::processAck(UCHAR* data, int length)
{
  if ((state != 1) && (state != 2)) return 0;

  if (length != 2) return 0;

  USHORT ackBlock = ntohs(*(USHORT*)data);

  if (ackBlock == (blockNumber - 1))
  {
    // successful incoming packet
    lastCom = time(NULL);

//    log->log("TftpClient", Log::DEBUG, "Ack received for block %i - success", ackBlock);

    if (state == 1) // it wasn't the final block
    {
      sendBlock();
    }
    else
    {
      // state == 2, end of transfer. kill connection
      log->log("TftpClient", Log::INFO, "File transfer finished");
      fclose(file);
      file = NULL;
      return 0;
    }
  }
  else
  {
    log->log("TftpClient", Log::DEBUG, "Ack received for block %i - rejected, retransmitting block\n", ackBlock);
    transmitBuffer();
  }

  return 1;
}

int TftpClient::openFile(char* requestedFile)
{
  char fileName[PATH_MAX];
  strcpy(fileName, requestedFile);

  for(UINT i = 0; i < strlen(fileName); i++)
  {
    if (fileName[i] == '/')
    {
      log->log("TftpClient", Log::ERR, "TFTP filename from client contained a path");
      return 0;
    }
  }

  char fileName2[PATH_MAX];
  snprintf(fileName2, PATH_MAX, "%s%s", baseDir, fileName);

  log->log("TftpClient", Log::INFO, "File: '%s'", fileName2);


  file = fopen(fileName2, "r");
  if (!file) return 0;
  return 1;
}

int TftpClient::sendBlock()
{
  *(USHORT*)&buffer[0] = htons(3);
  *(USHORT*)&buffer[2] = htons(blockNumber++);
  bufferLength = 4 + fread(&buffer[4], 1, 512, file);

  if (bufferLength < 516) // 512 + 4 header
  {
    // end of file
    state = 2;
  }
  else
  {
    state = 1;
  }

  transmitBuffer();

  return 1;
}

void TftpClient::transmitBuffer()
{

  ds.send(peerIP, peerPort, (char*)buffer, bufferLength);
//  dump(buffer, bufferLength);
//  log->log("TftpClient", Log::DEBUG, "Sent block number %i to port %u", blockNumber - 1, peerPort);
}
