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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <arpa/inet.h>
#include "eibnetip.h"

/** aborts program with a printf like message */
void
die (const char *msg, ...)
{
  va_list ap;
  va_start (ap, msg);
  vprintf (msg, ap);
  printf ("\n");
  va_end (ap);

  exit (1);
}

void
HexDump (const uchar * data, int Len)
{
  for (int i = 0; i < Len; i++)
    printf (" %02X", data[i]);
  printf ("\n");
}

int
main (int ac, char *ag[])
{
  int tracelevel;
  int sport;
  int dport;
  char *a, *b, *c;
  if (ac != 2 && ac != 3)
    die ("Usage: %s ip[:port] [tracelevel]", ag[0]);
  struct sockaddr_in saddr;
  struct sockaddr_in caddr;
  EIBNetIPSocket *sock;
  pth_event_t timeout = pth_event (PTH_EVENT_TIME, pth_timeout (10, 0));

  pth_init ();
  tracelevel = 0;
  if (ac == 3)
    tracelevel = atoi (ag[2]);
  a = strdup (ag[1]);
  if (!a)
    die ("out of memory");
  for (b = a; *b; b++)
    if (*b == ':')
      break;
  sport = 3672;
  if (*b == ':')
    {
      *b = 0;
      for (c = b + 1; *c; c++)
	if (*c == ':')
	  break;
      if (*c == ':')
	{
	  *c = 0;
	  sport = atoi (c + 1);
	}
      dport = atoi (b + 1);
    }
  else
    dport = 3671;

  Trace t;
  t.SetTraceLevel (tracelevel);
  if (!strcmp (a, "-"))
    a = (char *) "224.0.23.12";

  printf ("Asking %s at port %d from port %d\n", a, dport, sport);

  if (!GetHostIP (&caddr, a))
    die ("Host not found");
  caddr.sin_port = htons (dport);
  if (!GetSourceAddress (&caddr, &saddr))
    die ("No route found");
  saddr.sin_port = htons (sport);
  sock = new EIBNetIPSocket (saddr, 0, &t);
  sock->sendaddr = caddr;
  sock->recvaddr = caddr;
  sock->recvall = 1;
  if (!sock->init ())
    die ("IP initialisation failed");

  EIBnet_SearchRequest req;
  EIBnet_SearchResponse resp;
  EIBNetIPPacket *p1;
  req.caddr = saddr;
  sock->Send (req.ToPacket ());
  do
    {
      p1 = sock->Get (timeout);
      if (p1)
	{
	  if (parseEIBnet_SearchResponse (*p1, resp))
	    {
	      printf ("Invalid Search response\n");
	      continue;
	    }
	  printf ("Answer from %s at port %d\n",
		  inet_ntoa (resp.caddr.sin_addr),
		  ntohs (resp.caddr.sin_port));
	  printf ("Medium: %d\nState: %d\nAddr: %s\nInstallID: %d\nSerial:",
		  resp.KNXmedium, resp.devicestatus,
		  FormatEIBAddr (resp.individual_addr) (), resp.installid);
	  HexDump (resp.serial, sizeof (resp.serial));
	  printf ("Multicast-Addr: %s\nMAC:", inet_ntoa (resp.multicastaddr));
	  HexDump (resp.MAC, sizeof (resp.MAC));
	  printf ("Name: %s\n", resp.name);
	  for (int i = 0; i < resp.services (); i++)
	    printf ("Service %d Version %d\n", resp.services[i].family,
		    resp.services[i].version);
	}
    }
  while (p1);
  delete sock;
  return 0;
}
