#ifndef	XPROTO_H
#define	XPROTO_H
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

#include "xdefs.h"

#ifdef	__KERNEL__
#include <linux/list.h>
#include <linux/proc_fs.h>
#include <zaptel.h>

#define	PROTO_TABLE(n)	n ## _protocol_table

/*
 * The LSB of the type number signifies:
 * 	0 - TO_PSTN
 * 	1 - TO_PHONE
 */
#define	XPD_TYPE_FXS		3	// TO_PHONE
#define	XPD_TYPE_FXO		4	// TO_PSTN
#define	XPD_TYPE_BRI_TE		6	// TO_PSTN
#define	XPD_TYPE_BRI_NT		7	// TO_PHONE
#define	XPD_TYPE_NOMODULE	15

typedef	byte	xpd_type_t;

#define	XPD_TYPE_PREFIX	"xpd-type-"

#define	MODULE_ALIAS_XPD(type)	\
	MODULE_ALIAS(XPD_TYPE_PREFIX __stringify(type))

#define	PCM_CHUNKSIZE	(CHANNELS_PERXPD * 8)	/* samples of 8 bytes */

typedef struct xpd_addr {
	byte	unit:UNIT_BITS;
	byte	subunit:SUBUNIT_BITS;
	byte	reserved:1;
	byte	sync_master:1;
} PACKED xpd_addr_t;

bool valid_xpd_addr(const xpd_addr_t *addr);
int xpd_addr2num(const xpd_addr_t *addr);
void xpd_set_addr(xpd_addr_t *addr, int xpd_num);
xpd_t	*xpd_by_addr(const xbus_t *xbus, int unit, int subunit);

#define	XPD_NUM(x)		xpd_addr2num(&x)

#define	XPROTO_NAME(card,op)	card ## _ ## op
#define	XPROTO_HANDLER(card,op)	XPROTO_NAME(card,op ## _handler)
#define	XPROTO_CALLER(card,op)	XPROTO_NAME(card,op ## _send)

#define	HANDLER_DEF(card,op)	\
	int XPROTO_HANDLER(card,op) (		\
		xbus_t *xbus,			\
		xpd_t *xpd,			\
		const xproto_entry_t *cmd,	\
		xpacket_t *pack)

#define	CALL_PROTO(card,op, ...)	XPROTO_CALLER(card,op)( __VA_ARGS__ )

#define	DECLARE_CMD(card,op, ...)	\
	int CALL_PROTO(card, op, xbus_t *xbus, xpd_t *xpd, ## __VA_ARGS__ )

#define	HOSTCMD(card, op, ...)					\
			DECLARE_CMD(card, op, ## __VA_ARGS__ );	\
			EXPORT_SYMBOL(XPROTO_CALLER(card, op));	\
			DECLARE_CMD(card, op, ## __VA_ARGS__ )

#define	RPACKET_NAME(card,op)	XPROTO_NAME(RPACKET_ ## card, op)
#define	RPACKET_TYPE(card,op)	struct RPACKET_NAME(card, op)

#define	DEF_RPACKET_DATA(card,op, ...)	\
	RPACKET_TYPE(card,op) {	\
		uint16_t	datalen;\
		byte		opcode;	\
		xpd_addr_t	addr;	\
		__VA_ARGS__		\
	} PACKED
#define	RPACKET_HEADERSIZE		(sizeof(uint16_t) + sizeof(xpd_addr_t) + 1)
#define	RPACKET_CAST(p,card,op)		((RPACKET_TYPE(card,op) *)p)
#define	RPACKET_FIELD(p,card,op,field)	(RPACKET_CAST(p,card,op)->field)
#define	RPACKET_SIZE(card,op)		sizeof(RPACKET_TYPE(card,op))

#define	XENTRY(prototab,module,op)			\
	[ XPROTO_NAME(module,op) ] = {			\
		.handler = XPROTO_HANDLER(module,op),	\
		.datalen = RPACKET_SIZE(module,op),	\
		.name = #op,				\
		.table = &PROTO_TABLE(prototab)		\
	}



#define	XPACKET_INIT(p, card, op)					\
		do {							\
			p->opcode = XPROTO_NAME(card,op);	\
			p->datalen = RPACKET_SIZE(card,op);		\
		} while(0)

#define	XFRAME_NEW(frm, p, xbus, card, op, to)			\
	do {							\
		int		len = RPACKET_SIZE(card,op);	\
								\
		frm = xbus->ops->xframe_new(xbus, GFP_ATOMIC);	\
		if(!frm)					\
			return -ENOMEM;				\
		p = xframe_next_packet(frm, len);		\
		if(!p)						\
			return -ENOMEM;				\
		XPACKET_INIT(p, card, op);			\
		xpd_set_addr(&p->addr, to);			\
	} while(0)

#endif

/*--------------------------- register handling --------------------------------*/
/*
 * After the opcode, there are always:
 * 	* A size (in bytes) of the rest. Only 6 bits are counted:
 * 		- The MSB signifies a multibyte write (to BRI fifo)
 * 		- The MSB-1 signifies end of frame to multibyte writes in BRI.
 * A normal register command (not multibyte) than contains:
 * 	* A channel selector byte:
 * 		- ALL_CHANS (currently 31) is a broadcast request to set a
 * 		  register for all channels.
 * 		- Smaller numbers (0-30) represent the addressed channel number.
 * 		- The MSB signifies:
 * 			1 - register [R]ead request
 * 			0 - register [W]rite request
 *	* Register number
 * 	* Subregister number -- 0 when there is no subregister
 * 	* Data low
 * 	* Data high -- 0 for single byte registers (direct registers)
 * A multibyte register command than contains a sequence of bytes.
 */

#define	MULTIBYTE_MAX_LEN	5	/* FPGA firmware limitation */

typedef struct reg_cmd {
	byte		bytes:6;
	byte		eoframe:1;		/* For BRI -- end of frame	*/
	byte		multibyte:1;		/* For BRI -- fifo data		*/
	union {
		struct {
			byte		chipsel:CHAN_BITS;	/* chip select */
			byte		reserved:1;
			byte		do_subreg:1;
			byte		read_request:1;
			byte		regnum;
			byte		subreg;
			byte		data_low;
			byte		data_high;
		} PACKED r;
		/* For Write-Multibyte commands in BRI */
		struct {
			byte	xdata[MULTIBYTE_MAX_LEN];
		} PACKED d;
	} PACKED alt;
} PACKED reg_cmd_t;

/* Shortcut access macros */
#define	REG_FIELD(regptr,member)	((regptr)->alt.r.member)
#define	REG_XDATA(regptr)		((regptr)->alt.d.xdata)

#ifdef __KERNEL__
/*--------------------------- protocol tables ----------------------------------*/

typedef struct xproto_entry	xproto_entry_t;
typedef struct xproto_table	xproto_table_t;

typedef int (*xproto_handler_t)(
		xbus_t *xbus,
		xpd_t *xpd,
		const xproto_entry_t *cmd,
		xpacket_t *pack);

const xproto_table_t *xproto_get(xpd_type_t cardtype);
void xproto_put(const xproto_table_t *xtable);
const xproto_entry_t *xproto_card_entry(const xproto_table_t *table, byte opcode);
xproto_handler_t xproto_card_handler(const xproto_table_t *table, byte opcode);

const xproto_entry_t *xproto_global_entry(byte opcode);
xproto_handler_t xproto_global_handler(byte opcode);

#define	CALL_XMETHOD(name, xbus, xpd, ...)				\
			(xpd)->xops->name(xbus, xpd, ## __VA_ARGS__ )

struct xops {
	 xpd_t *(*card_new)(xbus_t *xbus, int xpd_num, const xproto_table_t *proto_table, byte revision);
	int (*card_init)(xbus_t *xbus, xpd_t *xpd);
	int (*card_remove)(xbus_t *xbus, xpd_t *xpd);
	int (*card_tick)(xbus_t *xbus, xpd_t *xpd);
	int (*card_zaptel_preregistration)(xpd_t *xpd, bool on);
	int (*card_zaptel_postregistration)(xpd_t *xpd, bool on);
	int (*card_hooksig)(xbus_t *xbus, xpd_t *xpd, int pos, zt_txsig_t txsig);
	int (*card_ioctl)(xpd_t *xpd, int pos, unsigned int cmd, unsigned long arg);
	int (*card_open)(xpd_t *xpd, lineno_t pos);
	int (*card_close)(xpd_t *xpd, lineno_t pos);
	int (*span_startup)(xpd_t *xpd);
	int (*span_shutdown)(xpd_t *xpd);
	int (*chan_onhooktransfer)(xbus_t *xbus, xpd_t *xpd, lineno_t chan, int millies);

	int (*XPD_STATE)(xbus_t *xbus, xpd_t *xpd, bool on);
	int (*RING)(xbus_t *xbus, xpd_t *xpd, lineno_t chan, bool on);
	int (*RELAY_OUT)(xbus_t *xbus, xpd_t *xpd, byte which, bool on);
};

struct xproto_entry {
	xproto_handler_t	handler;
	int			datalen;
	const char		*name;
	xproto_table_t		*table;
};

struct xproto_table {
	struct module	*owner;
	xproto_entry_t	entries[255];	/* Indexed by opcode */
	xops_t		xops;
	xpd_type_t	type;
	const char	*name;
	bool (*packet_is_valid)(xpacket_t *pack);
	void (*packet_dump)(const char *msg, xpacket_t *pack);
};

#include "card_global.h"
#include "card_fxs.h"
#include "card_fxo.h"

enum opcodes {
	XPROTO_NAME(GLOBAL, NULL_REPLY)		= 0xFE,
	XPROTO_NAME(GLOBAL, DESC_REQ)		= 0x04,
	XPROTO_NAME(GLOBAL, DEV_DESC)		= 0x05,
	XPROTO_NAME(GLOBAL, REGISTER_REQUEST)	= 0x0F,
/**/
	XPROTO_NAME(GLOBAL, PCM_WRITE)		= 0x11,
	XPROTO_NAME(GLOBAL, PCM_READ)		= 0x12,
/**/
	XPROTO_NAME(GLOBAL, SYNC_SOURCE)	= 0x19,
	XPROTO_NAME(GLOBAL, SYNC_REPLY)		= 0x1A,
/**/
	XPROTO_NAME(GLOBAL, ERROR_CODE)		= 0x22,
};


#define	MEMBER(card,op)	RPACKET_TYPE(card,op)	RPACKET_NAME(card,op)

struct xpacket {
	uint16_t	datalen;
	byte		opcode;
	xpd_addr_t	addr;
	union {
		MEMBER(GLOBAL, NULL_REPLY);
		MEMBER(GLOBAL, DESC_REQ);
		MEMBER(GLOBAL, DEV_DESC);
		MEMBER(GLOBAL, PCM_WRITE);
		MEMBER(GLOBAL, PCM_READ);
		MEMBER(GLOBAL, SYNC_REPLY);
		MEMBER(GLOBAL, ERROR_CODE);

		MEMBER(FXS, SIG_CHANGED);
		MEMBER(FXS, REGISTER_REPLY);

		MEMBER(FXO, SIG_CHANGED);
		MEMBER(FXO, DAA_REPLY);

		byte	data[0];
	};
	/* Last byte is chksum */
} PACKED;

void dump_packet(const char *msg, xpacket_t *packet, bool print_dbg);
void dump_reg_cmd(const char msg[], reg_cmd_t *regcmd);
int xframe_receive(xbus_t *xbus, xframe_t *xframe);
int xproto_register(const xproto_table_t *proto_table);
void xproto_unregister(const xproto_table_t *proto_table);
const xproto_entry_t *xproto_global_entry(byte opcode);
const char *xproto_name(xpd_type_t xpd_type);

#endif	/* __KERNEL__ */

#endif	/* XPROTO_H */
