/*
    Copyright 2004-2005 Chris Tallon, Andreas Vogel

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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
   This file contaisn the IDs for mediaproviders
   Each provider should have it's ID defined here.
   They should follow the name pattern:
   MPROVIDERID_<name>
   This is the ID a provider has ro use to register at it's local MediaProviderRegister
   As the mediaprovider architecture is hierarchical there are distributors on each level, that
   forward requests to their children.
   Currently it looks like follows:

      MediaPlayer                                     client
        - LocalMediaFile                              client
        - VDR (distributor)                           client
            |- <comm> - MediaPlayer                   server
                           - ServerFileMediaProvider  server

   As a distributor must forward multiple requests, it must register with a range of Ids.
   This range includes it's own Id + a range for all its children.
   This Range should be defined here as
   MPROVIDERRANGE_<name>
   If no range is defined here, the range is 1 - so no distribution.

*/

//we reserve the range 1...999 for client side providers
static const ULONG MPROVIDERID_LOCALMEDIAFILE=1;
static const ULONG MPROVIDERID_VDR=1000;
static const ULONG MPROVIDERRANGE_VDR=10000; //so it has the IDs 1000..10999
                                             //all providers on the server side
                                             //must fit into this range

static const ULONG MPROVIDERID_SERVERMEDIAFILE=1001;

#ifndef MEDIAPROVIDERIDS_H
#define MEDIAPROVIDERIDS_H

#include "defines.h"

#endif
