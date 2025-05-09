/*
    EIBD eib bus access and management daemon
    Copyright (C) 2005-2008 Martin Koegler <mkoegler@auto.tuwien.ac.at>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "lowlatency.h"

#ifdef HAVE_LINUX_LOWLATENCY

#include <sys/ioctl.h>

void
set_low_latency (int fd, low_latency_save * save)
{
  struct serial_struct snew;
  ioctl (fd, TIOCGSERIAL, save);
  ioctl (fd, TIOCGSERIAL, &snew);
  snew.flags |= ASYNC_LOW_LATENCY;
  ioctl (fd, TIOCSSERIAL, &snew);
}

void
restore_low_latency (int fd, low_latency_save * save)
{
  ioctl (fd, TIOCSSERIAL, save);
}

#else

void
set_low_latency (int fd, low_latency_save * save)
{
}

void
restore_low_latency (int fd, low_latency_save * save)
{
}

#endif
