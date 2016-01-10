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

#include "xpd.h"
#include "xproto.h"
#include "xpp_zap.h"
#include "xbus-core.h"
#include "zap_debug.h"
#include <linux/module.h>
#include <linux/delay.h>

static const char rcsid[] = "$Id: xproto.c 2477 2007-04-29 22:12:49Z tzafrir $";

extern	int print_dbg;

static const xproto_table_t *xprotocol_tables[XPD_TYPE_NOMODULE];

#if MAX_UNIT*MAX_SUBUNIT > MAX_XPDS
#error MAX_XPDS is too small
#endif

bool valid_xpd_addr(const xpd_addr_t *addr)
{
	return ((addr->subunit & ~BITMASK(SUBUNIT_BITS)) == 0) && ((addr->unit & ~BITMASK(UNIT_BITS)) == 0);
}

int xpd_addr2num(const xpd_addr_t *addr)
{
	BUG_ON(!valid_xpd_addr(addr));
	return addr->unit | (addr->subunit << UNIT_BITS);
}

void xpd_set_addr(xpd_addr_t *addr, int xpd_num)
{
	addr->unit = xpd_num & BITMASK(UNIT_BITS);
	addr->subunit = (xpd_num >> UNIT_BITS) & BITMASK(SUBUNIT_BITS);
}

xpd_t	*xpd_by_addr(const xbus_t *xbus, int unit, int subunit)
{
	xpd_addr_t	addr;
	int		xpd_num;

	addr.unit = unit;
	addr.subunit = subunit;
	xpd_num = xpd_addr2num(&addr);
	return xpd_of(xbus, xpd_num);
}

/*---------------- General Protocol Management ----------------------------*/

const xproto_entry_t *xproto_card_entry(const xproto_table_t *table, byte opcode)
{
	const xproto_entry_t *xe;

	//DBG("\n");
	xe = &table->entries[opcode];
	return (xe->handler != NULL) ? xe : NULL;
}

const xproto_entry_t *xproto_global_entry(byte opcode)
{
	const xproto_entry_t *xe;

	xe = xproto_card_entry(&PROTO_TABLE(GLOBAL), opcode);
	//DBG("opcode=0x%X xe=%p\n", opcode, xe);
	return xe;
}

xproto_handler_t xproto_global_handler(byte opcode)
{
	return xproto_card_handler(&PROTO_TABLE(GLOBAL), opcode);
}

const xproto_table_t *xproto_table(xpd_type_t cardtype)
{
	if(cardtype >= XPD_TYPE_NOMODULE)
		return NULL;
	return xprotocol_tables[cardtype];
}

const xproto_table_t *xproto_get(xpd_type_t cardtype)
{
	const xproto_table_t *xtable;

	if(cardtype >= XPD_TYPE_NOMODULE)
		return NULL;
	xtable = xprotocol_tables[cardtype];
	if(!xtable) {	/* Try to load the relevant module */
		int ret = request_module(XPD_TYPE_PREFIX "%d", cardtype);
		if(ret != 0) {
			NOTICE("%s: Failed to load module for type=%d. exit status=%d.\n",
					__FUNCTION__, cardtype, ret);
			/* Drop through: we may be lucky... */
		}
		xtable = xprotocol_tables[cardtype];
	}
	if(xtable) {
		BUG_ON(!xtable->owner);
		DBG("%s refcount was %d\n", xtable->name, module_refcount(xtable->owner));
		if(!try_module_get(xtable->owner)) {
			ERR("%s: try_module_get for %s failed.\n", __FUNCTION__, xtable->name);
			return NULL;
		}
	}
	return xtable;
}

void xproto_put(const xproto_table_t *xtable)
{
	BUG_ON(!xtable);
	DBG("%s refcount was %d\n", xtable->name, module_refcount(xtable->owner));
	BUG_ON(module_refcount(xtable->owner) <= 0);
	module_put(xtable->owner);
}

xproto_handler_t xproto_card_handler(const xproto_table_t *table, byte opcode)
{
	const xproto_entry_t *xe;

	//DBG("\n");
	xe = xproto_card_entry(table, opcode);
	return xe->handler;
}

static const xproto_entry_t *find_xproto_entry(xpd_t *xpd, byte opcode)
{
	const xproto_entry_t *xe;
	
	xe = xproto_global_entry(opcode);
	// DBG("opcode=0x%X xe=%p\n", opcode, xe);
	if(!xe) {
		const xproto_table_t *xtable;
		
		if(!xpd)
			return NULL;
		xtable = xproto_table(xpd->type);
		if(!xtable)
			return NULL;
		xe = xproto_card_entry(xtable, opcode);
		if(!xe)
			return NULL;
	}
	return xe;
}

int packet_process(xbus_t *xbus, xpacket_t *pack)
{
	byte			op;
	const xproto_entry_t	*xe;
	xproto_handler_t	handler;
	xproto_table_t		*table;
	xpd_t			*xpd;
	int			ret = 0;

	BUG_ON(!pack);
	if(!valid_xpd_addr(&pack->addr)) {
		if(printk_ratelimit())
			dump_packet("packet_process -- bad address", pack, print_dbg);
		ret = -EPROTO;
		goto out;
	}
	op = pack->opcode;
	xpd = xpd_by_addr(xbus, pack->addr.unit, pack->addr.subunit);
	/* XPD may be NULL (e.g: during bus polling */
	xe = find_xproto_entry(xpd, op);
	/*-------- Validations -----------*/
	if(!xe) {
		if(printk_ratelimit()) {
			ERR("xpp: %s: %s/%d-%d: bad command op=0x%02X\n",
					__FUNCTION__, xbus->busname,
					pack->addr.unit, pack->addr.subunit, op);
			dump_packet("packet_process -- bad command", pack, print_dbg);
		}
		ret = -EPROTO;
		goto out;
	}
	table = xe->table;
	BUG_ON(!table);
	if(!table->packet_is_valid(pack)) {
		if(printk_ratelimit()) {
			ERR("xpp: %s: wrong size %d for op=0x%02X\n",
					__FUNCTION__, pack->datalen, op);
			dump_packet("packet_process -- wrong size", pack, print_dbg);
		}
		ret = -EPROTO;
		goto out;
	}
	handler = xe->handler;
	BUG_ON(!handler);
	XBUS_COUNTER(xbus, RX_BYTES) += pack->datalen;
	handler(xbus, xpd, xe, pack);
out:
	return ret;
}

int xframe_receive(xbus_t *xbus, xframe_t *xframe)
{
	byte	*p;
	byte	*xpacket_start;
	byte	*xframe_end;
	int	ret = 0;
	static int	rate_limit;

	if(print_dbg == 2 && ((rate_limit++ % 1003) == 0))
		dump_xframe("RCV_PCM", xbus, xframe);
	p = xpacket_start = xframe->packets;
	xframe_end = xpacket_start + XFRAME_LEN(xframe);
	do {
		xpacket_t	*pack = (xpacket_t *)p;
		int		len = pack->datalen;

		p += len;
		if(p > xframe_end) {
			ERR("%s: Invalid packet length %d\n", __FUNCTION__, len);
			ret = -EPROTO;
			goto out;
		}
		ret = packet_process(xbus, pack);
		if(unlikely(ret < 0))
			goto out;
	} while(p < xframe_end);
out:
	xbus->ops->xframe_free(xbus, xframe);
	return ret;
}

#define	VERBOSE_DEBUG		1
#define	ERR_REPORT_LIMIT	20

void dump_packet(const char *msg, xpacket_t *packet, bool print_dbg)
{
	byte	op = packet->opcode;
	byte	*addr = (byte *)&packet->addr;

	if(!print_dbg)
		return;
	DBG("%s: XPD=%1X-%1X (0x%X) OP=0x%02X LEN=%d",
			msg,
			packet->addr.unit,
			packet->addr.subunit,
			*addr,
			op,
			(byte)packet->datalen);
#if VERBOSE_DEBUG
	{
		int i;
		byte	*p = (byte *)packet;

		if (print_dbg)
			printk(" BYTES: ");
		for(i = 0; i < packet->datalen; i++) {
			static int limiter = 0;

			if(i >= sizeof(xpacket_t)) {
				if(limiter < ERR_REPORT_LIMIT) {
					ERR("%s: length overflow i=%d > sizeof(xpacket_t)=%d\n",
						__FUNCTION__, i+1, sizeof(xpacket_t));
				} else if(limiter == ERR_REPORT_LIMIT) {
					ERR("%s: error packet #%d... squelsh reports.\n",
						__FUNCTION__, limiter);
				}
				limiter++;
				break;
			}
			if (print_dbg)
				printk("%02X ", p[i]);
		}
	}
#endif
	printk("\n");
}

void dump_reg_cmd(const char msg[], reg_cmd_t *regcmd)
{
	char		action;
	char		mode;
	byte		chipsel;
	byte		regnum;
	byte		data_low;
	byte		data_high;

	if(regcmd->bytes != sizeof(*regcmd) - 1) {	/* The size byte is not included */
		NOTICE("%s: Wrong size: regcmd->bytes = %d\n", __FUNCTION__, regcmd->bytes);
		return;
	}
	action = (REG_FIELD(regcmd, chipsel) & 0x80) ? 'R' : 'W';
	chipsel = REG_FIELD(regcmd, chipsel) & ~0x80;
	if(REG_FIELD(regcmd, regnum) == 0x1E) {
		mode = 'I';
		regnum = REG_FIELD(regcmd, subreg);
		data_low = REG_FIELD(regcmd, data_low);
		data_high = REG_FIELD(regcmd, data_high);
	} else {
		mode = 'D';
		regnum = REG_FIELD(regcmd, regnum);
		data_low = REG_FIELD(regcmd, data_low);
		data_high = 0;
	}
	DBG("%d %c%c %02X %02X %02X # m=%d eof=%d\n", chipsel, action, mode, regnum,
			data_low, data_high, regcmd->multibyte, regcmd->eoframe);
}

const char *xproto_name(xpd_type_t xpd_type)
{
	const xproto_table_t	*proto_table;

	BUG_ON(xpd_type >= XPD_TYPE_NOMODULE);
	proto_table = xprotocol_tables[xpd_type];
	if(!proto_table)
		return NULL;
	return proto_table->name;
}

#define	CHECK_XOP(f)	\
		if(!(xops)->f) { \
			ERR("%s: missing xmethod %s [%s (%d)]\n", __FUNCTION__, #f, name, type);	\
			return -EINVAL;	\
		}

int xproto_register(const xproto_table_t *proto_table)
{
	int		type;
	const char	*name;
	const xops_t	*xops;
	
	BUG_ON(!proto_table);
	type = proto_table->type;
	name = proto_table->name;
	if(type >= XPD_TYPE_NOMODULE) {
		NOTICE("%s: Bad xproto type %d\n", __FUNCTION__, type);
		return -EINVAL;
	}
	DBG("%s (%d)\n", name, type);
	if(xprotocol_tables[type])
		NOTICE("%s: overriding registration of %s (%d)\n", __FUNCTION__, name, type);
	xops = &proto_table->xops;
	CHECK_XOP(card_new);
	CHECK_XOP(card_init);
	CHECK_XOP(card_remove);
	CHECK_XOP(card_tick);
	CHECK_XOP(card_zaptel_preregistration);
	CHECK_XOP(card_zaptel_postregistration);
	CHECK_XOP(card_hooksig);
	// CHECK_XOP(card_ioctl);	// optional method -- call after testing
	CHECK_XOP(XPD_STATE);
	CHECK_XOP(RING);
	CHECK_XOP(RELAY_OUT);

	xprotocol_tables[type] = proto_table;
	return 0;
}

void xproto_unregister(const xproto_table_t *proto_table)
{
	int		type;
	const char	*name;
	
	BUG_ON(!proto_table);
	type = proto_table->type;
	name = proto_table->name;
	DBG("%s (%d)\n", name, type);
	if(type >= XPD_TYPE_NOMODULE) {
		NOTICE("%s: Bad xproto type %s (%d)\n", __FUNCTION__, name, type);
		return;
	}
	if(!xprotocol_tables[type])
		NOTICE("%s: xproto type %s (%d) is already unregistered\n", __FUNCTION__, name, type);
	xprotocol_tables[type] = NULL;
}

EXPORT_SYMBOL(dump_packet);
EXPORT_SYMBOL(dump_reg_cmd);
EXPORT_SYMBOL(xframe_receive);
EXPORT_SYMBOL(valid_xpd_addr);
EXPORT_SYMBOL(xpd_addr2num);
EXPORT_SYMBOL(xpd_set_addr);
EXPORT_SYMBOL(xproto_global_entry);
EXPORT_SYMBOL(xproto_card_entry);
EXPORT_SYMBOL(xproto_name);
EXPORT_SYMBOL(xproto_register);
EXPORT_SYMBOL(xproto_unregister);
