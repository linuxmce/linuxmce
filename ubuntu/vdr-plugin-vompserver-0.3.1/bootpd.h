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

#ifndef BOOTPD_H
#define BOOTPD_H

#include <stdio.h>
#include <signal.h>
#include <ctype.h>
#ifndef VOMPSTANDALONE
#include <vdr/plugin.h>
#endif

#include "defines.h"
#include "log.h"
#include "dsock.h"
#include "thread.h"
#include "config.h"

class Bootpd : public Thread
{
  public:
    Bootpd();
    virtual ~Bootpd();

    int run(const char* tconfigDir);
    int shutdown();

  private:
    void threadMethod();
    void processRequest(UCHAR* data, int length);
    int getmyip(in_addr_t destination, in_addr_t* result);

    DatagramSocket ds;
    Log* log;
    const char* configDir;
};

#endif
