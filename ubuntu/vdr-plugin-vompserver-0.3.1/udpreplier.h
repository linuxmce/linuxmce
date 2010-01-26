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

#ifndef UDPREPLIER_H
#define UDPREPLIER_H

#include <stdio.h>
#include <signal.h>

#include "log.h"
#include "dsock.h"
#include "thread.h"

class UDPReplier : public Thread
{
  public:
    UDPReplier();
    virtual ~UDPReplier();

    int run(char* tserverName);
    int shutdown();

  private:
    void threadMethod();

    DatagramSocket ds;
    char* serverName;
};

#endif
