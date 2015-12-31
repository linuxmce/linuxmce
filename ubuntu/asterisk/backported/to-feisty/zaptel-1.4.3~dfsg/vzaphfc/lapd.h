/*
 * lapd.c - net_device LAPD link layer support functionss
 *
 * Copyright (C) 2004 Daniele Orlandi
 *
 * Daniele "Vihai" Orlandi <daniele@orlandi.com> 
 *
 * This program is free software and may be modified and
 * distributed under the terms of the GNU Public License.
 *
 */

#ifndef _HFC_LAPD_H
#define _HFC_LAPD_H

#ifndef ARPHRD_LAPD
#define ARPHRD_LAPD 1000	/* LAPD pseudo type */
#endif

#ifndef ETH_P_LAPD
#define ETH_P_LAPD 0x0030	/* LAPD pseudo type */
#endif

void setup_lapd(struct net_device *netdev);

#endif
