/*
 * lapd.c
 *
 * This program is free software and may be modified and
 * distributed under the terms of the GNU Public License.
 *
 */

#include <linux/kernel.h>
#include <linux/netdevice.h>

#include "lapd.h"

static int lapd_change_mtu(struct net_device *dev, int mtu)
{
	return -EINVAL;
}

static int lapd_mac_addr(struct net_device *dev, void *addr)
{
	return -EINVAL;
}

void setup_lapd(struct net_device *netdev)
{

	netdev->change_mtu         = lapd_change_mtu;
	netdev->hard_header        = NULL;
	netdev->rebuild_header     = NULL;
	netdev->set_mac_address    = lapd_mac_addr;
	netdev->hard_header_cache  = NULL;
	netdev->header_cache_update= NULL;

	netdev->type               = ARPHRD_LAPD;
	netdev->hard_header_len    = 0;
	netdev->mtu                = 512;
	netdev->addr_len           = 0;
	netdev->tx_queue_len       = 10;

	memset(netdev->broadcast, 0x00, sizeof(netdev->broadcast));

	netdev->flags              = IFF_NOARP;
}


