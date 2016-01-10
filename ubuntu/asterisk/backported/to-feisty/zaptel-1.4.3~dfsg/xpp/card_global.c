/*
 * Written by Oron Peled <oron@actcom.co.il>
 * Copyright (C) 2004-2006, Xorcom
 *
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <linux/module.h>
#include "xdefs.h"
#include "xpd.h"
#include "xpp_zap.h"
#include "xproto.h"
#include "zap_debug.h"
#include "xbus-core.h"
#include "parport_debug.h"

static const char rcsid[] = "$Id: card_global.c 2536 2007-05-17 21:47:40Z tzafrir $";

DEF_PARM(charp,initdir, "/usr/share/zaptel", 0644, "The directory of card initialization scripts");

extern	int print_dbg;
static bool pcm_valid(xpd_t *xpd, xpacket_t *pack);

/*---------------- GLOBAL Protocol Commands -------------------------------*/

static bool global_packet_is_valid(xpacket_t *pack);
static void global_packet_dump(const char *msg, xpacket_t *pack);

/*---------------- GLOBAL: HOST COMMANDS ----------------------------------*/

/* 0x04 */ HOSTCMD(GLOBAL, DESC_REQ, int xpd_num)
{
	int		ret = 0;
	xframe_t	*xframe;
	xpacket_t	*pack;

	if(!xbus) {
		DBG("NO XBUS\n");
		return -EINVAL;
	}
	XFRAME_NEW(xframe, pack, xbus, GLOBAL, DESC_REQ, xpd_num);
	DBG("on %s #%d\n", xbus->busname, xpd_num);
	ret = xframe_send(xbus, xframe);
	XBUS_COUNTER(xbus, DESC_REQ)++;
	return ret;
}

/*
 * The XPD parameter is totaly ignored by the driver and firmware as well.
 */
/* 0x19 */ HOSTCMD(GLOBAL, SYNC_SOURCE, enum sync_mode mode, int drift)
{
	xframe_t	*xframe;
	xpacket_t	*pack;

	BUG_ON(!xbus);
	DBG("%s: mode=0x%X drift=%d\n", xbus->busname, mode, drift);
	XFRAME_NEW(xframe, pack, xbus, GLOBAL, SYNC_SOURCE, 0);
	RPACKET_FIELD(pack, GLOBAL, SYNC_SOURCE, sync_mode) = mode;
	RPACKET_FIELD(pack, GLOBAL, SYNC_SOURCE, drift) = drift;
	xframe_send(xbus, xframe);
	return 0;
}

/*---------------- GLOBAL: Astribank Reply Handlers -----------------------*/

HANDLER_DEF(GLOBAL, NULL_REPLY)
{
	DBG("got len=%d\n", pack->datalen);
	return 0;
}

HANDLER_DEF(GLOBAL, DEV_DESC)
{
	byte			rev = RPACKET_FIELD(pack, GLOBAL, DEV_DESC, rev);
	byte			type = RPACKET_FIELD(pack, GLOBAL, DEV_DESC, type);
	xpp_line_t		line_status = RPACKET_FIELD(pack, GLOBAL, DEV_DESC, line_status);
	xpd_addr_t		xpd_addr = RPACKET_FIELD(pack, GLOBAL, DEV_DESC, addr);
	struct card_desc_struct	*card_desc;

	BUG_ON(!xbus);
	if((card_desc = kmalloc(sizeof(struct card_desc_struct), GFP_ATOMIC)) == NULL) {
		ERR("%s: Card description allocation failed.\n", __FUNCTION__);
		return -ENOMEM;
	}
	memset(card_desc, 0, sizeof(struct card_desc_struct));
	card_desc->magic = CARD_DESC_MAGIC;
	INIT_LIST_HEAD(&card_desc->card_list);
	card_desc->xbus = xbus;
	card_desc->type = type;
	card_desc->rev = rev;
	card_desc->xpd_addr = xpd_addr;
	card_desc->line_status = line_status;
	DBG("%s: xpd=%d-%d type=%d rev=%d line_status=0x%04X\n",
			xbus->busname, xpd_addr.unit, xpd_addr.subunit, type, rev, line_status);
	xbus_poller_notify(xbus, card_desc);
	return 0;
}

HANDLER_DEF(GLOBAL, PCM_READ)
{
	/* FIXME: work around temporary hardware bug */
	xpp_line_t	lines = RPACKET_FIELD(pack, GLOBAL, PCM_READ, lines);
	const byte	*pcm = RPACKET_FIELD(pack, GLOBAL, PCM_READ, pcm);
	xpd_addr_t	addr = RPACKET_FIELD(pack, GLOBAL, PCM_READ, addr);
	volatile u_char	*readchunk;
	volatile u_char	*r;
	unsigned long	flags;
	int		i;
	xpp_line_t	old_lines = lines;
	int		unit;
	int		subunit;
	struct timeval	now;
	unsigned long	sec_diff;
	unsigned long	usec_diff;

	BUG_ON(!xbus);
	do_gettimeofday(&now);
	sec_diff = now.tv_sec - xbus->last_rx_sync.tv_sec;
	usec_diff = sec_diff * 1000000 + (now.tv_usec - xbus->last_rx_sync.tv_usec);
	if(unlikely(abs(sec_diff) > 2)) {
		DBG("%s: PCM RX timing restart (sec_diff=%ld)\n",
					xbus->busname, sec_diff);
	} else {
		if(abs(usec_diff - 1000) > TICK_TOLERANCE) {
			if(print_dbg && printk_ratelimit())
				DBG("%s: Bad PCM RX timing: usec_diff=%ld.\n",
						xbus->busname, usec_diff);
		}
		if(usec_diff > xbus->max_rx_sync)
			xbus->max_rx_sync = usec_diff;
		if(usec_diff < xbus->min_rx_sync)
			xbus->min_rx_sync = usec_diff;
	}
	xbus->last_rx_sync = now;
	if(!xpd) {
#if 0
		int xpd_num = XPD_NUM(pack->addr);
		NOTICE("%s: received %s for non-existing xpd: %d\n",
				__FUNCTION__, cmd->name, xpd_num);
#endif
		return -EPROTO;
	}
	// DBG("lines=0x%04X\n", lines);

	if(!pcm_valid(xpd, pack)) {
		static int	rate_limit;

		if((rate_limit++ % 5001) == 0)
			ERR("%s/%s: old_lines=0x%04X lines=0x%04X subunit=%d\n",
				xbus->busname, xpd->xpdname, old_lines, lines, xpd->addr.subunit);
		return -EPROTO;
	}
	unit = xpd->addr.unit;
	for(subunit = 0; subunit < MAX_SUBUNIT; subunit++, lines >>= SUBUNIT_PCM_SHIFT) {
		xpd_t		*tmp_xpd;
		
		tmp_xpd = xpd_by_addr(xbus, unit, subunit);
		if(!tmp_xpd || !tmp_xpd->card_present)
			continue;
		if(lines == 0)
			break;	/* Optimize */
		spin_lock_irqsave(&tmp_xpd->lock, flags);
		if (tmp_xpd->timer_count & 1) {
			/* First part */
			r = readchunk = tmp_xpd->readchunk;
		} else {
			r = readchunk = tmp_xpd->readchunk + ZT_CHUNKSIZE * CHANNELS_PERXPD;
		}
		/* Copy PCM and put each channel in its index */
		for_each_line(tmp_xpd, i) {
			if(IS_SET(lines, i)) {
				memcpy((u_char *)r, pcm, ZT_CHUNKSIZE);
				//memset((u_char *)r, 0x5A, ZT_CHUNKSIZE);	// DEBUG
				pcm += ZT_CHUNKSIZE;
			}
			r += ZT_CHUNKSIZE;
		}
		XPD_COUNTER(tmp_xpd, PCM_READ)++;
		XBUS_COUNTER(xbus, PCM_READ)++;
		spin_unlock_irqrestore(&tmp_xpd->lock, flags);
		if (!IS_BRI(tmp_xpd))	/* only BRI has subunits */
			break;
	}
	flip_parport_bit(2);
	/*
	 * Firmware marks the sync packets.
	 * This is out of the loop, so we don't send multiple times
	 * for BRI subunits.
	 */
	if(addr.sync_master)
		got_sync_from(xpd);
	return 0;
}

HANDLER_DEF(GLOBAL, SYNC_REPLY)
{
	byte	mode = RPACKET_FIELD(pack, GLOBAL, SYNC_REPLY, sync_mode);
	byte	drift = RPACKET_FIELD(pack, GLOBAL, SYNC_REPLY, drift);

	BUG_ON(!xbus);
	if(!xpd) {
		int xpd_num = XPD_NUM(pack->addr);
		NOTICE("%s: received %s for non-existing xpd: addr=0x%02X\n", __FUNCTION__, cmd->name, xpd_num);
		return -EPROTO;
	}
	DBG("%s/%s: mode=0x%X drift=%d\n", xpd->xbus->busname, xpd->xpdname,
			mode, drift);
	dump_packet("SYNC_REPLY", pack, 1);
	xbus->sync_adjustment = (signed char)drift;
	return 0;
}

HANDLER_DEF(GLOBAL, ERROR_CODE)
{
	byte		errorcode = RPACKET_FIELD(pack, GLOBAL, ERROR_CODE, errorcode);
	reg_cmd_t	*bad_cmd;
	char		tmp_xpdname[XPD_NAMELEN];
	static long	rate_limit;

	BUG_ON(!xbus);
	if((rate_limit++ % 5003) > 200)
		return 0;
	if(!xpd) {
		int xpd_num = XPD_NUM(pack->addr);
		snprintf(tmp_xpdname, XPD_NAMELEN, "#%d", xpd_num);
	} else {
		snprintf(tmp_xpdname, XPD_NAMELEN, "%s", xpd->xpdname);
	}
	NOTICE("%s/%s: FIRMWARE: %s CODE = 0x%X (rate_limit=%ld)\n",
			xbus->busname, tmp_xpdname, cmd->name, errorcode, rate_limit);
	switch(errorcode) {
		case 1:
			bad_cmd = &RPACKET_FIELD(pack, GLOBAL, ERROR_CODE, info.bad_spi_cmd);
			dump_packet("FIRMWARE: BAD_SPI_CMD", pack, 1);
			break;
		case 0xAB:
			dump_packet("FIRMWARE: BAD_PACKET_LEN", pack, 1);
			break;
		default:
			NOTICE("%s/%s: FIRMWARE: %s UNKNOWN CODE = 0x%X\n", xbus->busname, tmp_xpdname, cmd->name, errorcode);
			dump_packet("PACKET", pack, 1);
	}
	/*
	 * FIXME: Should implement an error recovery plan
	 */
	return 0;
}


xproto_table_t PROTO_TABLE(GLOBAL) = {
	.entries = {
		/*	Prototable	Card	Opcode		*/
		XENTRY(	GLOBAL,		GLOBAL, NULL_REPLY	),
		XENTRY(	GLOBAL,		GLOBAL, DEV_DESC	),
		XENTRY(	GLOBAL,		GLOBAL,	PCM_READ	),
		XENTRY(	GLOBAL,		GLOBAL,	SYNC_REPLY	),
		XENTRY(	GLOBAL,		GLOBAL, ERROR_CODE	),
	},
	.name = "GLOBAL",
	.packet_is_valid = global_packet_is_valid,
	.packet_dump = global_packet_dump,
};

static bool global_packet_is_valid(xpacket_t *pack)
{
	const xproto_entry_t	*xe;

	//DBG("\n");
	xe = xproto_global_entry(pack->opcode);
	return xe != NULL;
}

static void global_packet_dump(const char *msg, xpacket_t *pack)
{
	DBG("%s\n", msg);
}

static bool pcm_valid(xpd_t *xpd, xpacket_t *pack)
{
	xpp_line_t	lines = RPACKET_FIELD(pack, GLOBAL, PCM_READ, lines);
	int		i;
	int		count = 0;
	uint16_t	good_len;

	BUG_ON(!pack);
	BUG_ON(pack->opcode != XPROTO_NAME(GLOBAL, PCM_READ));
/*
 * Don't use for_each_line(xpd, i) here because for BRI it will ignore the channels of the other
 * xpd's in the same unit.
 */
	for (i = 0; i < CHANNELS_PERXPD; i++)
		if(IS_SET(lines, i))
			count++;
	/* FRAMES: include opcode in calculation */
	good_len = RPACKET_HEADERSIZE + sizeof(xpp_line_t) + count * 8;
	if(pack->datalen != good_len) {
		static int rate_limit = 0;

		XPD_COUNTER(xpd, RECV_ERRORS)++;
		if((rate_limit++ % 1000) <= 10) {
			ERR("%s/%s: BAD PCM REPLY: pack->datalen=%d (should be %d), count=%d\n",
					xpd->xbus->busname, xpd->xpdname,
					pack->datalen, good_len, count);
			dump_packet("BAD PCM REPLY", pack, 1);
		}
		return 0;
	}
	return 1;
}

#define	MAX_ENV_STR	20
#define	MAX_PATH_STR	60

int run_initialize_registers(xpd_t *xpd)
{
	int	ret;
	xbus_t	*xbus;
	char	busstr[MAX_ENV_STR];
	char	xpdstr[MAX_ENV_STR];
	char	unitstr[MAX_ENV_STR];
	char	subunitstr[MAX_ENV_STR];
	char	typestr[MAX_ENV_STR];
	char	revstr[MAX_ENV_STR];
	char	init_card[MAX_PATH_STR];
	char	*argv[] = {
		init_card,
		NULL
	};
	char	*envp[] = {
		busstr,
		xpdstr,
		unitstr,
		subunitstr,
		typestr,
		revstr,
		NULL
	};

	BUG_ON(!xpd);
	xbus = xpd->xbus;
	if(!initdir || !initdir[0]) {
		NOTICE("%s/%s: Missing initdir parameter\n", xbus->busname, xpd->xpdname);
		return -EINVAL;
	}
	snprintf(busstr, MAX_ENV_STR, "XPD_BUS=%s", xbus->busname);
	snprintf(xpdstr, MAX_ENV_STR, "XPD_NAME=%s", xpd->xpdname);
	snprintf(unitstr, MAX_ENV_STR, "XPD_UNIT=%d", xpd->addr.unit);
	snprintf(subunitstr, MAX_ENV_STR, "XPD_SUBUNIT=%d", xpd->addr.subunit);
	snprintf(typestr, MAX_ENV_STR, "XPD_TYPE=%d", xpd->type);
	snprintf(revstr, MAX_ENV_STR, "XPD_REVISION=%d", xpd->revision);
	if(snprintf(init_card, MAX_PATH_STR, "%s/init_card_%d_%d",
				initdir, xpd->type, xpd->revision) > MAX_PATH_STR) {
		NOTICE("%s/%s: Cannot initialize. pathname is longer than %d characters.\n",
				xbus->busname, xpd->xpdname, MAX_PATH_STR);
		return -E2BIG;
	}
	if(!down_read_trylock(&xbus->in_use)) {
		ERR("Skipped register initialization. %s is going down\n", xbus->busname);
		return -ENODEV;
	}
	DBG("%s/%s: running '%s' for type=%d revision=%d\n",
			xbus->busname, xpd->xpdname, init_card, xpd->type, xpd->revision);
	ret = call_usermodehelper(init_card, argv, envp, 1);
	/*
	 * Carefully report results
	 */
	if(ret == 0)
		DBG("%s/%s: '%s' finished OK\n", xbus->busname, xpd->xpdname, init_card);
	else if(ret < 0) {
		ERR("%s/%s: Failed running '%s' (errno %d)\n",
				xbus->busname, xpd->xpdname, init_card, ret);
	} else {
		byte	exitval = ((unsigned)ret >> 8) & 0xFF;
		byte	sigval = ret & 0xFF;

		if(!exitval) {
			ERR("%s/%s: '%s' killed by signal %d\n",
					xbus->busname, xpd->xpdname, init_card, sigval);
		} else {
			ERR("%s/%s: '%s' aborted with exitval %d\n",
					xbus->busname, xpd->xpdname, init_card, exitval);
		}
		ret = -EINVAL;
	}
	up_read(&xbus->in_use);
	return ret;
}

EXPORT_SYMBOL(run_initialize_registers);
