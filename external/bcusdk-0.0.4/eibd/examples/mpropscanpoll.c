/*
    EIB Demo program - list properties
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
#include "common.h"

int
main (int ac, char *ag[])
{
  uchar buf[10000];
  int len;
  int i;
  EIBConnection *con;
  eibaddr_t dest;
  fd_set read;
  char *prog = ag[0];

  parseKey (&ac, &ag);
  if (ac != 3)
    die ("usage: %s [-k key] url eibaddr", prog);
  con = EIBSocketURL (ag[1]);
  if (!con)
    die ("Open failed");
  dest = readaddr (ag[2]);

  if (EIB_MC_Connect (con, dest) == -1)
    die ("Connect failed");
  auth (con);

  len = EIB_MC_PropertyScan_async (con, sizeof (buf), buf);
  if (len == -1)
    die ("Read failed");

lp:
  FD_ZERO (&read);
  FD_SET (EIB_Poll_FD (con), &read);
  printf ("Waiting\n");
  if (select (EIB_Poll_FD (con) + 1, &read, 0, 0, 0) == -1)
    die ("select failed");
  printf ("Data available\n");
  len = EIB_Poll_Complete (con);
  if (len == -1)
    die ("Read failed");
  if (len == 0)
    goto lp;
  printf ("Completed\n");

  len = EIBComplete (con);

  for (i = 0; i < len; i += 6)
    if (buf[i + 1] == 1 && buf[i + 2] == 4)
      printf ("Obj: %d Property: %d Type: %d Objtype:%d Access:%02X\n",
	      buf[i + 0], buf[i + 1], buf[i + 2],
	      (buf[i + 3] << 8) | buf[i + 4], buf[i + 5]);
    else
      printf ("Obj: %d Property: %d Type: %d Count:%d Access:%02X\n",
	      buf[i + 0], buf[i + 1], buf[i + 2],
	      (buf[i + 3] << 8) | buf[i + 4], buf[i + 5]);

  EIBClose (con);
  return 0;
}
