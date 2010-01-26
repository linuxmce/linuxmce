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

#ifndef MVPRELAY_H
#define MVPRELAY_H

#include <stdio.h>
#include <signal.h>

#include "log.h"
#include "dsock.h"
#include "thread.h"

#include "tcp.h" // temp

class MVPRelay : public Thread
{
  public:
    MVPRelay();
    virtual ~MVPRelay();

    int run();
    int shutdown();

  private:
    void threadMethod();

    DatagramSocket ds;
};

#endif

/*

Protocol information (from the original mvprelay.c)

0  4 bytes: sequence
4  4 bytes: magic number = 0xBABEFAFE
8  6 bytes: client MAC address
14 2 bytes: reserved = 0
16 4 bytes: client IP address
20 2 bytes: client port number
22 2 bytes: reserved = 0
24 4 bytes: GUI IP address
28 2 bytes: GUI port number
30 2 bytes: reserved = 0
32 4 bytes: Con IP address       ???
36 2 bytes: Con port number      ???
38 6 bytes: reserved = 0         ???
44 4 bytes: Server IP address    ???
48 2 bytes: Server port number   ???
50 2 bytes: reserved = 0         ???

Total: 52 bytes
*/
