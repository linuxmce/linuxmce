/*
    EIB Demo program - write device memory (without verify)
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
  int len, addr;
  EIBConnection *con;
  uchar buf[255];
  eibaddr_t dest;
  char *prog = ag[0];

  parseKey (&ac, &ag);
  if (ac < 4)
    die ("usage: %s [-k key] url eibaddr addr [xx xx xx ..]", prog);
  con = EIBSocketURL (ag[1]);
  if (!con)
    die ("Open failed");
  dest = readaddr (ag[2]);
  addr = readHex (ag[3]);
  len = readBlock (buf, sizeof (buf), ac - 4, ag + 4);

  if (EIB_MC_Connect (con, dest) == -1)
    die ("Connect failed");
  auth (con);

  printf ("Write: ");
  printHex (len, buf);
  printf ("\n");
  len = EIB_MC_Write_Plain (con, addr, len, buf);
  if (len == -1)
    die ("Write failed");

  EIBClose (con);
  return 0;
}
