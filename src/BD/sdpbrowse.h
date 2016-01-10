/*
    SDP lookup routine for the PlutoMO rfcomm port 

    Copyright (C) 2008 Harald Klein <hari@vt100.at>

    This program is distributed according to the terms of the Pluto Public License, available at:
    http://plutohome.com/index.php?section=public_license

    This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
*/

#include <bluetooth/bluetooth.h>

int sdp_lookup_pluto_rfcomm_channel(bdaddr_t target);
