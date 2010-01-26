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

#include "udpreplier.h"

UDPReplier::UDPReplier()
{
  serverName = NULL;
}

UDPReplier::~UDPReplier()
{
  shutdown();
}

int UDPReplier::shutdown()
{
  if (threadIsActive()) threadCancel();

  if (serverName) delete[] serverName;
  serverName = NULL;
  return 1;
}

int UDPReplier::run(char* tserverName)
{
  if (threadIsActive()) return 1;

  serverName = new char[strlen(tserverName)+1];
  strcpy(serverName, tserverName);

  if (!ds.init(3024))
  {
    shutdown();
    return 0;
  }

  if (!threadStart())
  {
    shutdown();
    return 0;
  }

  Log::getInstance()->log("UDPReplier", Log::DEBUG, "UDP replier started");
  return 1;
}

void UDPReplier::threadMethod()
{
  int retval;
  while(1)
  {
    retval = ds.waitforMessage(0);
    if (retval == 1) continue;

    if (!strcmp(ds.getData(), "VOMP"))
    {
      Log::getInstance()->log("UDPReplier", Log::DEBUG, "UDP request from %s", ds.getFromIPA());
      ds.send(ds.getFromIPA(), 3024, serverName, strlen(serverName));
    }
  }
}
