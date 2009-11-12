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

#include "tftpd.h"

Tftpd::Tftpd()
{
  log = Log::getInstance();
  baseDir = NULL;
}

Tftpd::~Tftpd()
{
  shutdown();
}

int Tftpd::shutdown()
{
  if (threadIsActive()) threadCancel();
  ds.shutdown();

  if (baseDir) delete[] baseDir;
  baseDir = NULL;

  return 1;
}

int Tftpd::run(char* tbaseDir)
{
  if (threadIsActive()) return 1;
  log->log("Tftpd", Log::DEBUG, "Starting TFTPd");

  if (!ds.init(16869))
  {
    log->log("Tftpd", Log::DEBUG, "DSock init error");
    shutdown();
    return 0;
  }

  baseDir = new char[strlen(tbaseDir) + 1];
  strcpy(baseDir, tbaseDir);

  if (!threadStart())
  {
    log->log("Tftpd", Log::DEBUG, "Thread start error");
    shutdown();
    return 0;
  }

  log->log("Tftpd", Log::DEBUG, "TFTP server started with base path '%s'", baseDir);
  return 1;
}

void Tftpd::threadMethod()
{
  int retval;
  while(1)
  {
    log->log("Tftpd", Log::DEBUG, "Starting wait");
    retval = ds.waitforMessage(0);
    log->log("Tftpd", Log::DEBUG, "Wait finished");

    if (retval == 0)
    {
      log->log("Tftpd", Log::CRIT, "Wait for packet error");
      return;
    }
    else if (retval == 1)
    {
      continue;
    }
    else
    {
      TftpClient* t = new TftpClient();
      t->run(baseDir, ds.getFromIPA(), ds.getFromPort(), (UCHAR*)ds.getData(), ds.getDataLength());
    }
  }
}

