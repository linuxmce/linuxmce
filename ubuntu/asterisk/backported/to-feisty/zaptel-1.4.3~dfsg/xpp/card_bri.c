/*
 * Written by Oron Peled <oron@actcom.co.il>
 * Copyright (C) 2004-2006, Xorcom
 *
 * Parts derived from Cologne demo driver for the chip.
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

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include "xpd.h"
#include "xproto.h"
#include "xpp_zap.h"
#include "card_bri.h"
#include "zap_debug.h"
#include "xpd.h"
#include "xbus-core.h"

static const char rcsid[] = "$Id: card_bri.c 2536 2007-05-17 21:47:40Z tzafrir $";

#ifndef CONFIG_ZAPATA_BRI_DCHANS
#error CONFIG_ZAPATA_BRI_DCHANS is not defined
#endif

DEF_PARM(int, print_dbg, 0, 0644, "Print DBG statements");	/* must be before zap_debug.h */
DEF_PARM(uint, poll_interval, 500, 0644, "Poll channel state interval in milliseconds (0 - disable)");

enum xhfc_states {
	ST_RESET		= 0,	/* G/F0	*/
	/* TE */
	ST_TE_SENSING		= 2,	/* F2	*/
	ST_TE_DEACTIVATED	= 3,	/* F3	*/
	ST_TE_SIGWAIT		= 4,	/* F4	*/
	ST_TE_IDENT		= 5,	/* F5	*/
	ST_TE_SYNCED		= 6,	/* F6	*/
	ST_TE_ACTIVATED		= 7,	/* F7	*/
	ST_TE_LOST_FRAMING	= 8,	/* F8	*/
	/* NT */
	ST_NT_DEACTIVATED	= 1,	/* G1	*/
	ST_NT_ACTIVATING	= 2,	/* G2	*/
	ST_NT_ACTIVATED		= 3,	/* G3	*/
	ST_NT_DEACTIVTING	= 4,	/* G4	*/
};

static const char *xhfc_state_name(xpd_type_t xpd_type, enum xhfc_states state)
{
	const char	*p;

#define	_E(x)	[ST_ ## x] = #x
	static const char *te_names[] = {
		_E(RESET),
		_E(TE_SENSING),
		_E(TE_DEACTIVATED),
		_E(TE_SIGWAIT),
		_E(TE_IDENT),
		_E(TE_SYNCED),
		_E(TE_ACTIVATED),
		_E(TE_LOST_FRAMING),
	};
	static const char *nt_names[] = {
		_E(RESET),
		_E(NT_DEACTIVATED),
		_E(NT_ACTIVATING),
		_E(NT_ACTIVATED),
		_E(NT_DEACTIVTING),
	};
#undef	_E
	if(xpd_type == XPD_TYPE_BRI_TE) {
		if ((state < ST_RESET) || (state > ST_TE_LOST_FRAMING))
			p = "TE ???";
		else
			p = te_names[state];
	} else {
		if ((state < ST_RESET) || (state > ST_NT_DEACTIVTING))
			p = "NT ???";
		else
			p = nt_names[state];
	}
	return p;
}

/* xhfc Layer1 physical commands */
#define HFC_L1_ACTIVATE_TE		0x01
#define HFC_L1_FORCE_DEACTIVATE_TE	0x02
#define HFC_L1_ACTIVATE_NT		0x03
#define HFC_L1_DEACTIVATE_NT		0x04

#define HFC_L1_ACTIVATING	1
#define HFC_L1_ACTIVATED	2
#define	TIMER_T1_MAX		2500
#define	HFC_TIMER_T3		8000	/* 8s activation timer T3 */
#define	HFC_TIMER_T4		500	/* 500ms deactivation timer T4 */
#define	HFC_TIMER_OFF		-1	/* timer disabled */

#define	A_SU_WR_STA		0x30	/* ST/Up state machine register		*/
#define		V_SU_LD_STA	0x10
#define 	V_SU_ACT	0x60	/* start activation/deactivation	*/
#define 	STA_DEACTIVATE	0x40	/* start deactivation in A_SU_WR_STA */
#define 	STA_ACTIVATE	0x60	/* start activation   in A_SU_WR_STA */
#define 	V_SU_SET_G2_G3	0x80

#define	A_SU_RD_STA		0x30
typedef union {
	struct {
		byte	v_su_sta:4;
		byte	v_su_fr_sync:1;
		byte	v_su_t2_exp:1;
		byte	v_su_info0:1;
		byte	v_g2_g3:1;
	} bits;
	byte	reg;
} su_rd_sta_t;

#define	REG30_LOST	3	/* in polls */
#define	DCHAN_LOST	15000	/* in ticks */

#define	BRI_DCHAN_SIGCAP	(			  \
					ZT_SIG_EM	| \
					ZT_SIG_CLEAR	| \
					ZT_SIG_FXSLS	| \
					ZT_SIG_FXSGS	| \
					ZT_SIG_FXSKS	| \
					ZT_SIG_FXOLS	| \
					ZT_SIG_FXOGS	| \
					ZT_SIG_FXOKS	| \
					ZT_SIG_CAS	| \
					ZT_SIG_SF	  \
				)
#define	BRI_BCHAN_SIGCAP	ZT_SIG_CLEAR


/*---------------- BRI Protocol Commands ----------------------------------*/

static int write_state_register(xpd_t *xpd, byte value);
static bool bri_packet_is_valid(xpacket_t *pack);
static void bri_packet_dump(const char *msg, xpacket_t *pack);
static int proc_bri_info_read(char *page, char **start, off_t off, int count, int *eof, void *data);
static int proc_xpd_register_read(char *page, char **start, off_t off, int count, int *eof, void *data);
static int proc_xpd_register_write(struct file *file, const char __user *buffer, unsigned long count, void *data);

#define	PROC_REGISTER_FNAME	"slics"
#define	PROC_BRI_INFO_FNAME	"bri_info"

#define	VALID_CHIPSEL(x)	((x) == 0)

enum led_state {
	BRI_LED_OFF		= 0x0,
	BRI_LED_ON		= 0x1,
	/*
	 * We blink by software from driver, so that
	 * if the driver malfunction that blink would stop.
	 */
	// BRI_LED_BLINK_SLOW	= 0x2,	/* 1/2 a second blink cycle */
	// BRI_LED_BLINK_FAST	= 0x3	/* 1/4 a second blink cycle */
};

enum bri_led_names {
	GREEN_LED	= 0,
	RED_LED		= 1
};

#define	NUM_LEDS	2
#define	LED_TICKS	100


struct bri_leds {
	byte	state:2;
	byte	led_sel:1;	/* 0 - GREEN, 1 - RED */
	byte	reserved:5;
};

#ifndef MAX_DFRAME_LEN_L1
#define MAX_DFRAME_LEN_L1 300
#endif

#define	DCHAN_BUFSIZE	MAX_DFRAME_LEN_L1

struct BRI_priv_data {
	struct proc_dir_entry		*regfile;
	struct proc_dir_entry		*bri_info;
	su_rd_sta_t			state_register;
	bool				initialized;
	int				t1; /* timer 1 for NT deactivation */
	int				t3; /* timer 3 for activation */
	int				t4; /* timer 4 for deactivation */
	ulong				l1_flags;
	bool				reg30_good;
	uint				reg30_ticks;
	bool				layer1_up;

	/*
	 * D-Chan: buffers + extra state info.
	 */
	int				dchan_r_idx;
	byte				dchan_rbuf[DCHAN_BUFSIZE];
	byte				dchan_tbuf[DCHAN_BUFSIZE];
	bool				txframe_begin;

	reg_cmd_t			requested_reply;
	reg_cmd_t			last_reply;
	uint				tick_counter;
	uint				poll_counter;
	uint				dchan_tx_counter;
	uint				dchan_rx_counter;
	uint				dchan_rx_drops;
	bool				dchan_alive;
	uint				dchan_alive_ticks;
	uint				dchan_notx_ticks;
	uint				dchan_norx_ticks;
	enum led_state			ledstate[NUM_LEDS];
};

xproto_table_t	PROTO_TABLE(BRI_NT);
xproto_table_t	PROTO_TABLE(BRI_TE);


DEF_RPACKET_DATA(BRI, SET_LED,	/* Set one of the LED's */
	struct bri_leds		bri_leds;
	);

static /* 0x33 */ DECLARE_CMD(BRI, SET_LED, bool red_led, enum led_state to_led_state);
static /* 0x0F */ DECLARE_CMD(BRI, REGISTER_REQUEST, byte chipsel, bool writing, bool do_subreg, byte regnum, byte subreg, byte data_low, byte data_high);

#define DEBUG_BUF_SIZE (100)
static void dump_hex_buf(xpd_t *xpd, char *msg, byte *buf, size_t len)
{
	char	debug_buf[DEBUG_BUF_SIZE + 1];
	int	i;
	int	n = 0;
	xbus_t	*xbus = xpd->xbus;

	debug_buf[0] = '\0';
	for(i = 0; i < len && n < DEBUG_BUF_SIZE; i++)
		n += snprintf(&debug_buf[n], DEBUG_BUF_SIZE - n, "%02X ", buf[i]);
	DBG("%s/%s: %s[0..%d]: %s%s\n", xbus->busname, xpd->xpdname, msg,
			len-1, debug_buf, (n >= DEBUG_BUF_SIZE)?"...":"");
}

static void dump_dchan_packet(xpd_t *xpd, bool transmit, byte *buf, int len)
{
	struct BRI_priv_data	*priv;
	char	msgbuf[MAX_PROC_WRITE];
	char	ftype = '?';
	char	*direction;
	int	frame_begin;

	priv = xpd->priv;
	BUG_ON(!priv);
	if(transmit) {
		direction = "TX";
		frame_begin = priv->txframe_begin;
	} else {
		direction = "RX";
		frame_begin = 1;
	}
	if(frame_begin) {	/* Packet start */
		if(!IS_SET(buf[0], 7))
			ftype = 'I';	/* Information */
		else if(IS_SET(buf[0], 7) && !IS_SET(buf[0], 6))
			ftype = 'S';	/* Supervisory */
		else if(IS_SET(buf[0], 7) && IS_SET(buf[0], 6))
			ftype = 'U';	/* Unnumbered */
		else
			NOTICE("Unknown frame type 0x%X\n", buf[0]);

		snprintf(msgbuf, MAX_PROC_WRITE, "D-Chan %s = (%c) ", direction, ftype);
	} else {
		snprintf(msgbuf, MAX_PROC_WRITE, "D-Chan %s =     ", direction);
	}
	dump_hex_buf(xpd, msgbuf, buf, len);
}

static void layer1_state(xpd_t *xpd, bool up)
{
	struct BRI_priv_data	*priv;

	BUG_ON(!xpd);
	priv = xpd->priv;
	BUG_ON(!priv);
	if(priv->layer1_up == up)
		return;
	priv->layer1_up = up;
	DBG("%s/%s: STATE CHANGE: Layer1 %s\n", xpd->xbus->busname, xpd->xpdname,
		(up)?"UP":"DOWN");
}

static void dchan_state(xpd_t *xpd, bool up)
{
	struct BRI_priv_data	*priv;

	BUG_ON(!xpd);
	priv = xpd->priv;
	BUG_ON(!priv);
	if(priv->dchan_alive == up)
		return;
	if(up) {
		DBG("%s/%s: STATE CHANGE: D-Channel RUNNING\n",
			xpd->xbus->busname, xpd->xpdname);
		priv->dchan_alive = 1;
	} else {
		DBG("%s/%s: STATE CHANGE: D-Channel STOPPED\n",
			xpd->xbus->busname, xpd->xpdname);
		priv->dchan_rx_counter = priv->dchan_tx_counter = priv->dchan_rx_drops = 0;
		priv->dchan_alive = 0;
		priv->dchan_alive_ticks = 0;
	}
}

static void xpd_activation(xpd_t *xpd, bool on)
{
	struct BRI_priv_data	*priv;
	xbus_t			*xbus;

	BUG_ON(!xpd);
	priv = xpd->priv;
	BUG_ON(!priv);
	xbus = xpd->xbus;
	DBG("%s/%s: %s\n", xbus->busname, xpd->xpdname, (on)?"ON":"OFF");
	switch(xpd->type) {
		case XPD_TYPE_BRI_TE:
			if(on) {
				DBG("%s/%s: HFC_L1_ACTIVATE_TE\n", xbus->busname, xpd->xpdname);
				set_bit(HFC_L1_ACTIVATING, &priv->l1_flags);
				write_state_register(xpd, STA_ACTIVATE);
				priv->t3 = HFC_TIMER_T3;
			} else {
				DBG("%s/%s: HFC_L1_FORCE_DEACTIVATE_TE\n", xbus->busname, xpd->xpdname);
				write_state_register(xpd, STA_DEACTIVATE);
			}
			break;
		case XPD_TYPE_BRI_NT:
			if(on) {
				DBG("%s/%s: HFC_L1_ACTIVATE_NT\n", xbus->busname, xpd->xpdname);
				priv->t1 = TIMER_T1_MAX;
				set_bit(HFC_L1_ACTIVATING, &priv->l1_flags);
				write_state_register(xpd, STA_ACTIVATE | V_SU_SET_G2_G3);
			} else {
				DBG("%s/%s: HFC_L1_DEACTIVATE_NT\n", xbus->busname, xpd->xpdname);
				write_state_register(xpd, STA_DEACTIVATE);
			}
			break;
		default:
			ERR("%s: %s/%s: Bad xpd type %d\n", __FUNCTION__,
					xbus->busname, xpd->xpdname, xpd->type);
			BUG();
	}
}


/*
 * D-Chan receive
 */
static int rx_dchan(xpd_t *xpd, reg_cmd_t *regcmd)
{
	xbus_t			*xbus;
	struct BRI_priv_data	*priv;
	byte			*src;
	byte			*dst;
	byte			*dchan_buf;
	struct zt_chan		*dchan;
	uint			len;
	bool			eoframe;
	int			idx;
	int			ret = 0;

	src = REG_XDATA(regcmd);
	len = regcmd->bytes;
	eoframe = regcmd->eoframe;
	if(len <= 0)
		return 0;
	if(!SPAN_REGISTERED(xpd)) /* Nowhere to copy data */
		return 0;
	BUG_ON(!xpd);
	priv = xpd->priv;
	BUG_ON(!priv);
	xbus = xpd->xbus;
#ifdef XPP_DEBUGFS
	xbus_log(xbus, xpd, 0, regcmd, sizeof(reg_cmd_t));		/* 0 = RX */
#endif
	dchan = &xpd->span.chans[2];
	if(!IS_SET(xpd->offhook, 2)) {	/* D-chan is used? */
		static int rate_limit;

		if((rate_limit++ % 1000) == 0)
			DBG("%s/%s: D-Chan unused\n", xbus->busname, xpd->xpdname);
		dchan->bytes2receive = 0;
		dchan->bytes2transmit = 0;
		goto out;
	}
	dchan_buf = dchan->readchunk;
	idx = priv->dchan_r_idx;
	if(idx + len >= DCHAN_BUFSIZE) {
		ERR("%s/%s: D-Chan RX overflow: %d\n", xbus->busname, xpd->xpdname, idx);
		dump_hex_buf(xpd, "    current packet", src, len);
		dump_hex_buf(xpd, "    dchan_buf", dchan_buf, idx);
		ret = -ENOSPC;
		if(eoframe)
			goto drop;
		goto out;
	}
	dst = dchan_buf + idx;
	idx += len;
	priv->dchan_r_idx = idx;
	memcpy(dst, src, len);
	if(!eoframe)
		goto out;
	if(idx < 4) {
		NOTICE("%s/%s: D-Chan RX short frame (idx=%d)\n", xbus->busname, xpd->xpdname, idx);
		dump_hex_buf(xpd, "D-Chan RX:    current packet", src, len);
		dump_hex_buf(xpd, "D-Chan RX:    chan_buf", dchan_buf, idx);
		ret = -EPROTO;
		goto drop;
	}
	if(dchan_buf[idx-1]) {
		NOTICE("%s/%s: D-Chan RX Bad checksum: [%02X:%02X=%02X] (%d)\n",
				xbus->busname, xpd->xpdname,
				dchan_buf[idx-3], dchan_buf[idx-2], dchan_buf[idx-1], dchan_buf[idx-1]);
		dump_hex_buf(xpd, "D-Chan RX:    current packet", src, len);
		dump_hex_buf(xpd, "D-Chan RX:    chan_buf", dchan_buf, idx);
		ret = -EPROTO;
		goto drop;
	}
	if(print_dbg)
		dump_dchan_packet(xpd, 0, dchan_buf, idx /* - 3 */);	/* Print checksum? */
	/* 
	 * Tell Zaptel that we received idx-1 bytes. They include the data and a 2-byte checksum.
	 * The last byte (that we don't pass on) is 0 if the checksum is correct. If it were wrong,
	 * we would drop the packet in the "if(dchan_buf[idx-1])" above.
	 */
	dchan->bytes2receive = idx - 1;
	dchan->eofrx = 1;
	priv->dchan_rx_counter++;
	priv->dchan_norx_ticks = 0;
drop:
	priv->dchan_r_idx = 0;
out:
	return ret;
}

static int send_bri_multibyte(xpd_t *xpd, byte *buf, int len, bool eoftx)
{
	xbus_t		*xbus = xpd->xbus;
	xframe_t	*xframe;
	xpacket_t	*pack;
	reg_cmd_t	*reg_cmd;
	int		ret;

	BUG_ON(len < 0);
	/*
	 * Zero length multibyte is legal and has special meaning for the
	 * firmware:
	 *   eoftx==1: Start sending us D-channel packets.
	 *   eoftx==0: Stop sending us D-channel packets.
	 */
	if(len > MULTIBYTE_MAX_LEN) {
		ERR("%s: len=%d is too long. dropping.\n", __FUNCTION__, len);
		return -EINVAL;
	}
	XFRAME_NEW(xframe, pack, xbus, BRI, REGISTER_REQUEST, xpd->id);
	reg_cmd = &RPACKET_FIELD(pack, BRI, REGISTER_REQUEST, reg_cmd);
	reg_cmd->bytes = len;
	reg_cmd->eoframe = eoftx;
	reg_cmd->multibyte = 1;
	if(len > 0) {
		memcpy(REG_XDATA(reg_cmd), (byte *)buf, len);
	} else {
		DBG("Magic Packet (eoftx=%d)\n", eoftx);
	}
#ifdef XPP_DEBUGFS
	xbus_log(xbus, xpd, 1, reg_cmd, sizeof(reg_cmd_t));	/* 1 = TX */
#else
	if(print_dbg)
		dump_xframe("SEND_BRI_MULTI", xbus, xframe);
#endif
	ret = xframe_send(xbus, xframe);
	if(ret < 0)
		NOTICE("%s: %s: failed sending xframe\n", __FUNCTION__, xbus->busname);
	return ret;
}

/*
 * D-Chan transmit
 */
static int tx_dchan(xpd_t *xpd)
{
	struct BRI_priv_data	*priv;
	struct zt_chan		*dchan;
	int			len;
	int			eoframe;
	int			ret;

	priv = xpd->priv;
	BUG_ON(!priv);
	if(xpd->type == XPD_TYPE_BRI_TE) {
		static int	rate_limit;

		if (priv->t3 > HFC_TIMER_OFF) {
			/* timer expired ? */
			if (--priv->t3 == 0) {
				if ((rate_limit % 1003) >= 5)
					DBG("%s/%s: T3 expired\n", xpd->xbus->busname, xpd->xpdname);
				priv->t3 = HFC_TIMER_OFF;
				clear_bit(HFC_L1_ACTIVATING, &priv->l1_flags);
				xpd_activation(xpd, 0);		/* Deactivate TE */
			}
		}
		if (priv->t4 > HFC_TIMER_OFF) {
			/* timer expired ? */
			if (--priv->t4 == 0) {
				if ((rate_limit % 1003) >= 5)
					DBG("%s/%s: T4 expired\n", xpd->xbus->busname, xpd->xpdname);
				priv->t4 = HFC_TIMER_OFF;
			}
		}
		rate_limit++;
	}
	if(!SPAN_REGISTERED(xpd) || !(xpd->span.flags & ZT_FLAG_RUNNING))
		return 0;
	dchan = &xpd->chans[2];
	len = dchan->bytes2transmit;	/* dchan's hdlc package len */
	eoframe = dchan->eoftx;		/* dchan's end of frame */
	dchan->bytes2transmit = 0;
	dchan->eoftx = 0;
	dchan->bytes2receive = 0;
	dchan->eofrx = 0;
	if(len <= 0)
		return 0; /* Nothing to transmit on D channel */
	if(len > MULTIBYTE_MAX_LEN) {
		ERR("%s: len=%d. need to split. Unimplemented.\n", __FUNCTION__, len);
		return -EINVAL;
	}
	if(!test_bit(HFC_L1_ACTIVATED, &priv->l1_flags) && !test_bit(HFC_L1_ACTIVATING, &priv->l1_flags)) {
		DBG("%s/%s: Kick D-Channel transmiter\n", xpd->xbus->busname, xpd->xpdname);
		xpd_activation(xpd, 1);
		return 0;
	}
	if(print_dbg)
		dump_dchan_packet(xpd, 1, priv->dchan_tbuf, len);
	if(eoframe)
		priv->txframe_begin = 1;
	else
		priv->txframe_begin = 0;
	ret = send_bri_multibyte(xpd, priv->dchan_tbuf, len, eoframe);
	if(ret < 0)
		NOTICE("%s/%s: %s: failed sending xframe\n",
				__FUNCTION__, xpd->xbus->busname, xpd->xpdname);
	priv->dchan_tx_counter++;
	priv->dchan_notx_ticks = 0;
	return ret;
}

/*---------------- BRI: Methods -------------------------------------------*/

static xpd_t *BRI_card_new(xbus_t *xbus, int xpd_num, const xproto_table_t *proto_table, byte revision)
{
	xpd_t		*xpd = NULL;
	int		channels = min(3, CHANNELS_PERXPD);

	DBG("\n");

	xpd = xpd_alloc(sizeof(struct BRI_priv_data), xbus, xpd_num, proto_table, channels, revision);
	if(!xpd)
		return NULL;
	xpd->direction = (proto_table == &PROTO_TABLE(BRI_NT)) ? TO_PHONE : TO_PSTN;
	xpd->revision = revision;
	return xpd;
}

static void clean_proc(xbus_t *xbus, xpd_t *xpd)
{
	struct BRI_priv_data	*priv;

	BUG_ON(!xpd);
	priv = xpd->priv;
	DBG("%s/%s\n", xbus->busname, xpd->xpdname);
#ifdef	CONFIG_PROC_FS
	if(priv->regfile) {
		DBG("Removing registers file for %s/%s\n", xbus->busname, xpd->xpdname);
		priv->regfile->data = NULL;
		remove_proc_entry(PROC_REGISTER_FNAME, xpd->proc_xpd_dir);
	}
	if(priv->bri_info) {
		DBG("Removing xpd BRI_INFO file %s/%s\n", xbus->busname, xpd->xpdname);
		remove_proc_entry(PROC_BRI_INFO_FNAME, xpd->proc_xpd_dir);
	}
#endif
}

static int BRI_card_init(xbus_t *xbus, xpd_t *xpd)
{
	struct BRI_priv_data	*priv;
	int			ret = 0;

	DBG("\n");

	BUG_ON(!xpd);
	priv = xpd->priv;
#ifdef	CONFIG_PROC_FS
	DBG("Creating BRI_INFO file for %s/%s\n", xbus->busname, xpd->xpdname);
	priv->bri_info = create_proc_read_entry(PROC_BRI_INFO_FNAME, 0444, xpd->proc_xpd_dir, proc_bri_info_read, xpd);
	if(!priv->bri_info) {
		ERR("Failed to create proc '%s' for %s/%s\n", PROC_BRI_INFO_FNAME, xbus->busname, xpd->xpdname);
		ret = -ENOENT;
		goto err;
	}
	priv->bri_info->owner = THIS_MODULE;
	DBG("Creating registers file for %s/%s\n", xbus->busname, xpd->xpdname);
	priv->regfile = create_proc_entry(PROC_REGISTER_FNAME, 0644, xpd->proc_xpd_dir);
	if(!priv->regfile) {
		ERR("Failed to create proc file for registers of %s/%s\n", xbus->busname, xpd->xpdname);
		goto err;
	}
	priv->regfile->owner = THIS_MODULE;
	priv->regfile->write_proc = proc_xpd_register_write;
	priv->regfile->read_proc = proc_xpd_register_read;
	priv->regfile->data = xpd;
#endif
	priv->t1 = HFC_TIMER_OFF;
	ret = run_initialize_registers(xpd);
	if(ret < 0)
		goto err;
	/*
	 * FPGA firmware limitation:
	 *     Force HOST sync *before* sending PCM
	 */
	CALL_PROTO(GLOBAL, SYNC_SOURCE, xbus, NULL, SYNC_MODE_HOST, 0);
	DBG("done: %s/%s\n", xbus->busname, xpd->xpdname);
	priv->initialized = 1;
	return 0;
err:
	clean_proc(xbus, xpd);
	ERR("%s/%s: Failed initializing registers (%d)\n", xbus->busname, xpd->xpdname, ret);
	return ret;
}

static int BRI_card_remove(xbus_t *xbus, xpd_t *xpd)
{
	struct BRI_priv_data	*priv;

	BUG_ON(!xpd);
	priv = xpd->priv;
	DBG("%s/%s\n", xbus->busname, xpd->xpdname);
	clean_proc(xbus, xpd);
	return 0;
}

static int BRI_card_zaptel_preregistration(xpd_t *xpd, bool on)
{
	xbus_t			*xbus;
	struct BRI_priv_data	*priv;
	int			i;
	
	BUG_ON(!xpd);
	xbus = xpd->xbus;
	priv = xpd->priv;
	BUG_ON(!xbus);
	DBG("%s/%s: %s\n", xbus->busname, xpd->xpdname, (on)?"on":"off");
	if(!on) {
		/* Nothing to do yet */
		return 0;
	}
	snprintf(xpd->span.desc, MAX_SPANDESC, "Xorcom XPD #%d/%d: %s",
			xbus->num, xpd->id,
			xpd->xproto->name
			);
	xpd->span.linecompat = ZT_CONFIG_AMI | ZT_CONFIG_CCS;
	xpd->span.deflaw = ZT_LAW_ALAW;
	BIT_SET(xpd->digital_signalling, 2);	/* D-Channel */
	for_each_line(xpd, i) {
		struct zt_chan	*cur_chan = &xpd->chans[i];

		DBG("setting BRI channel %d\n", i);
		snprintf(cur_chan->name, MAX_CHANNAME, "XPP_%s/%d/%d/%d",
				xpd->xproto->name, xbus->num, xpd->id, i);
		cur_chan->chanpos = i + 1;
		cur_chan->pvt = xpd;
		if(i == 2) {	/* D-CHAN */
			cur_chan->sigcap = BRI_DCHAN_SIGCAP;
			cur_chan->flags |= ZT_FLAG_BRIDCHAN;
			cur_chan->flags &= ~ZT_FLAG_HDLC;

			/* Setup big buffers for D-Channel rx/tx */
			cur_chan->readchunk = priv->dchan_rbuf;
			cur_chan->writechunk = priv->dchan_tbuf;
			priv->dchan_r_idx = 0;
			priv->txframe_begin = 1;

			cur_chan->maxbytes2transmit = MULTIBYTE_MAX_LEN;
			cur_chan->bytes2transmit = 0;
			cur_chan->bytes2receive = 0;
		} else
			cur_chan->sigcap = BRI_BCHAN_SIGCAP;
	}
	xpd->offhook = BIT(0) | BIT(1);	/* 2*bchan */
	return 0;
}

static int BRI_card_zaptel_postregistration(xpd_t *xpd, bool on)
{
	xbus_t			*xbus;
	struct BRI_priv_data	*priv;
	
	BUG_ON(!xpd);
	xbus = xpd->xbus;
	priv = xpd->priv;
	BUG_ON(!xbus);
	DBG("%s/%s: %s\n", xbus->busname, xpd->xpdname, (on)?"on":"off");
	return(0);
}

int BRI_card_hooksig(xbus_t *xbus, xpd_t *xpd, int pos, zt_txsig_t txsig)
{
	DBG("%s/%s/%d: %s\n", xbus->busname, xpd->xpdname, pos, txsig2str(txsig));
	return 0;
}

/*
 * LED managment is done by the driver now:
 *   - Turn constant ON RED/GREEN led to indicate NT/TE port
 *   - Very fast "Double Blink" to indicate Layer1 alive (without D-Channel)
 *   - Constant blink (1/2 sec cycle) to indicate D-Channel alive.
 */
static void handle_leds(xbus_t *xbus, xpd_t *xpd)
{
	struct BRI_priv_data	*priv;
	unsigned int		timer_count;
	int			which_led;
	int			other_led;
	enum led_state		ledstate;
	int			mod;

	BUG_ON(!xpd);
	if(xpd->type == XPD_TYPE_BRI_TE) {
		which_led = GREEN_LED;
		other_led = RED_LED;
	} else {
		which_led = RED_LED;
		other_led = GREEN_LED;
	}
	priv = xpd->priv;
	BUG_ON(!priv);
	ledstate = priv->ledstate[which_led];
	timer_count = xpd->timer_count;
	if((timer_count % DEFAULT_LED_PERIOD) == 0) {
		if(xpd->blink_mode) {
			// led state is toggled
				if(ledstate == BRI_LED_OFF) {
					CALL_PROTO(BRI, SET_LED, xbus, xpd, which_led, BRI_LED_ON);
					CALL_PROTO(BRI, SET_LED, xbus, xpd, other_led, BRI_LED_ON);
				} else {
					CALL_PROTO(BRI, SET_LED, xbus, xpd, which_led, BRI_LED_OFF);
					CALL_PROTO(BRI, SET_LED, xbus, xpd, other_led, BRI_LED_OFF);
				}
			return;
		} else if(priv->ledstate[other_led] == BRI_LED_ON)	/* cleanup after blink */
			CALL_PROTO(BRI, SET_LED, xbus, xpd, other_led, BRI_LED_OFF);
	}
	if(priv->dchan_alive) {
		mod = timer_count % 1000;
		switch(mod) {
			case 0:
				if(ledstate == BRI_LED_OFF)
					CALL_PROTO(BRI, SET_LED, xbus, xpd, which_led, BRI_LED_ON);
				break;
			case 500:
				if(ledstate == BRI_LED_ON)
					CALL_PROTO(BRI, SET_LED, xbus, xpd, which_led, BRI_LED_OFF);
				break;
		}
	} else if(priv->layer1_up) {
		mod = timer_count % 1000;
		switch(mod) {
			case 0:
			case 100:
				if(ledstate == BRI_LED_OFF)
					CALL_PROTO(BRI, SET_LED, xbus, xpd, which_led, BRI_LED_ON);
				break;
			case 50:
			case 150:
				if(ledstate == BRI_LED_ON)
					CALL_PROTO(BRI, SET_LED, xbus, xpd, which_led, BRI_LED_OFF);
				break;
		}
	} else {
		if(priv->ledstate[which_led] != BRI_LED_ON)
			CALL_PROTO(BRI, SET_LED, xbus, xpd, which_led, BRI_LED_ON);
	}
}

/* Poll the register ST/Up-State-machine Register, to see if the cable
 * if a cable is connected to the port.
 */
static int BRI_card_tick(xbus_t *xbus, xpd_t *xpd)
{
	struct BRI_priv_data	*priv;

	BUG_ON(!xpd);
	priv = xpd->priv;
	BUG_ON(!priv);
	if(!priv->initialized)
		return 0;
	if(poll_interval != 0 && (priv->tick_counter % poll_interval) == 0) {
		// DBG("%d\n", priv->tick_counter);
		priv->poll_counter++;
		CALL_PROTO(BRI, REGISTER_REQUEST, xbus, xpd, 0, 0, 0, A_SU_RD_STA, 0, 0, 0);
	}
	/* Detect D-Channel disconnect heuristic */
	priv->dchan_notx_ticks++;
	priv->dchan_norx_ticks++;
	priv->dchan_alive_ticks++;
	if(priv->dchan_alive && (priv->dchan_notx_ticks > DCHAN_LOST || priv->dchan_norx_ticks > DCHAN_LOST)) {
		/*
		 * No tx_dchan() or rx_dchan() for many ticks
		 * This D-Channel is probabelly dead.
		 */
		dchan_state(xpd, 0);
	} else if(priv->dchan_rx_counter > 1 &&  priv->dchan_tx_counter > 1) {
		if(!priv->dchan_alive)
			dchan_state(xpd, 1);
	}
	/* Detect Layer1 disconnect */
	if(priv->reg30_good && priv->reg30_ticks > poll_interval * REG30_LOST) {
		/* No reply for 1/2 a second */
		ERR("%s/%s: Lost state tracking for %d ticks\n",
				xbus->busname, xpd->xpdname, priv->reg30_ticks);
		priv->reg30_good = 0;
		layer1_state(xpd, 0);
		dchan_state(xpd, 0);
	}
	handle_leds(xbus, xpd);
	tx_dchan(xpd);
#if	0
	/*
		Currently not used -- do we want to shut down NT if it wasn't
		responding? For how long?
	*/

	/* Detect T1 timer expiry on NT */
	if(xpd->type == XPD_TYPE_BRI_NT) {
		if (priv->t1 > HFC_TIMER_OFF) {
			if (--priv->t1 == 0) {
				DBG("%s/%s: T1 Expired. Kick NT\n", xbus->busname, xpd->xpdname);
				priv->t1 = HFC_TIMER_OFF;
				clear_bit(HFC_L1_ACTIVATING, &priv->l1_flags);
				write_state_register(xpd, STA_DEACTIVATE);
			}
		}
	}
#endif
	priv->tick_counter++;
	priv->reg30_ticks++;
	return 0;
}

static int BRI_card_close(xpd_t *xpd, lineno_t pos)
{
	struct zt_chan	*chan = &xpd->span.chans[pos];

	/* Clear D-Channel pending data */
	chan->bytes2receive = 0;
	chan->eofrx = 0;
	chan->bytes2transmit = 0;
	chan->eoftx = 0;
	return 0;
}

static int BRI_span_startup(xpd_t *xpd)
{
	struct BRI_priv_data	*priv;
	struct zt_chan		*dchan;

	BUG_ON(!xpd);
	priv = xpd->priv;
	BUG_ON(!priv);
	DBG("%s/%s: STARTUP\n", xpd->xbus->busname, xpd->xpdname);
	write_state_register(xpd, 0);	/* Enable L1 state machine */
	xpd_activation(xpd, 1);
	if(SPAN_REGISTERED(xpd)) {
		dchan = &xpd->span.chans[2];
		xpd->span.flags |= ZT_FLAG_RUNNING;
		/*
		 * Zaptel (wrongly) assume that D-Channel need HDLC decoding
		 * and during zaptel registration override our flags.
		 *
		 * Don't Get Mad, Get Even:  Now we override zaptel :-)
		 */
		dchan->flags |= ZT_FLAG_BRIDCHAN;
		dchan->flags &= ~ZT_FLAG_HDLC;
	}
	return 0;
}

static int BRI_span_shutdown(xpd_t *xpd)
{
	struct BRI_priv_data	*priv;

	BUG_ON(!xpd);
	priv = xpd->priv;
	BUG_ON(!priv);
	DBG("%s/%s: SHUTDOWN\n", xpd->xbus->busname, xpd->xpdname);
	if(xpd->type == XPD_TYPE_BRI_NT)
		xpd_activation(xpd, 0);
	return 0;
}

/*---------------- BRI: HOST COMMANDS -------------------------------------*/

/* 0x0F */ HOSTCMD(BRI, REGISTER_REQUEST, byte chipsel, bool writing, bool do_subreg, byte regnum, byte subreg, byte data_low, byte data_high)
{
	int		ret = 0;
	xframe_t	*xframe;
	xpacket_t	*pack;
	reg_cmd_t	*reg_cmd;

	if(!xbus) {
		DBG("NO XBUS\n");
		return -EINVAL;
	}
	XFRAME_NEW(xframe, pack, xbus, BRI, REGISTER_REQUEST, xpd->id);
#if 0
	DBG("%s/%s/%d: %c%c R%02X S%02X %02X %02X\n",
			xbus->busname, xpd->xpdname, chipsel,
			(writing)?'W':'R',
			(do_subreg)?'S':'D',
			regnum, subreg, data_low, data_high);
#endif
	reg_cmd = &RPACKET_FIELD(pack, BRI, REGISTER_REQUEST, reg_cmd);
	reg_cmd->bytes = sizeof(*reg_cmd) - 1;	// do not count the 'bytes' field
	REG_FIELD(reg_cmd, chipsel) = chipsel;
	REG_FIELD(reg_cmd, read_request) = (writing) ? 0 : 1;
	REG_FIELD(reg_cmd, do_subreg) = do_subreg;
	REG_FIELD(reg_cmd, regnum) = regnum;
	REG_FIELD(reg_cmd, subreg) = subreg;
	REG_FIELD(reg_cmd, data_low) = data_low;
	REG_FIELD(reg_cmd, data_high) = data_high;
	ret = xframe_send(xbus, xframe);
	return ret;
}

/* 0x0F */ HOSTCMD(BRI, XPD_STATE, bool on)
{
	BUG_ON(!xpd);
	DBG("%s/%s: %s\n", xbus->busname, xpd->xpdname, (on)?"ON":"OFF");
	xpd_activation(xpd, on);
	return 0;
}

/* 0x0F */ HOSTCMD(BRI, RING, lineno_t chan, bool on)
{
	ERR("%s: Unsupported\n", __FUNCTION__);
	return -ENOSYS;
}

/* 0x0F */ HOSTCMD(BRI, RELAY_OUT, byte which, bool on)
{
	ERR("%s: Unsupported\n", __FUNCTION__);
	return -ENOSYS;
}

/* 0x33 */ HOSTCMD(BRI, SET_LED, bool red_led, enum led_state to_led_state)
{
	int			ret = 0;
	xframe_t	*xframe;
	xpacket_t		*pack;
	struct bri_leds		*bri_leds;
	struct BRI_priv_data	*priv;
	int			which_led = (red_led) ? RED_LED : GREEN_LED;

	BUG_ON(!xbus);
	priv = xpd->priv;
	BUG_ON(!priv);
	XFRAME_NEW(xframe, pack, xbus, BRI, SET_LED, xpd->id);
	bri_leds = &RPACKET_FIELD(pack, BRI, SET_LED, bri_leds);
	bri_leds->state = to_led_state;
	bri_leds->led_sel = which_led;
	pack->datalen = RPACKET_SIZE(BRI, SET_LED);
	ret = xframe_send(xbus, xframe);
	priv->ledstate[which_led] = to_led_state;
	return ret;
}

static int write_state_register(xpd_t *xpd, byte value)
{
	int	ret;

	DBG("%s/%s: value = 0x%02X\n", xpd->xbus->busname, xpd->xpdname, value);
	ret = CALL_PROTO(BRI, REGISTER_REQUEST, xpd->xbus, xpd,
			0,		/* chipsel	*/
			1,		/* writing	*/
			0,		/* do_subreg	*/
			A_SU_WR_STA,	/* regnum	*/
			0,		/* subreg	*/
			value,		/* data_low	*/
			0		/* data_high	*/
			);
	return ret;
}

/*---------------- BRI: Astribank Reply Handlers --------------------------*/
static void su_new_state(xpd_t *xpd, byte reg_x30)
{
	xbus_t			*xbus;
	struct BRI_priv_data	*priv;
	su_rd_sta_t		new_state;

	BUG_ON(!xpd);
	priv = xpd->priv;
	BUG_ON(!priv);
	xbus = xpd->xbus;
	if(!priv->initialized) {
		ERR("%s/%s: %s called on uninitialized AB\n",
			xbus->busname, xpd->xpdname, __FUNCTION__);
		return;
	}
	new_state.reg = reg_x30;
	priv->reg30_ticks = 0;
	priv->reg30_good = 1;
	if((xpd->type == XPD_TYPE_BRI_TE && new_state.bits.v_su_sta == ST_TE_ACTIVATED) ||
		(xpd->type == XPD_TYPE_BRI_NT && new_state.bits.v_su_sta == ST_NT_ACTIVATED)) {
		if(!priv->layer1_up) {
			layer1_state(xpd, 1);
			update_xpd_status(xpd, ZT_ALARM_NONE);
		}
	} else {
		/*
		 * Layer 1 disconnected
		 */
		if(priv->layer1_up) {
			layer1_state(xpd, 0);
			dchan_state(xpd, 0);
		}
		/*
		 * Do NOT notify Zaptel about the disconnection.
		 * If we do, Asterisk stops transmitting on the D-channel and
		 * we can't reactivate layer-1.
		 * Without the notification, Asterisk thinks that we are active
		 * (although the PSTN stopped layer-1) and on call setup, sends
		 * us D-channel data, which triggers the layer-1 activation.
		 */
#if 0
		update_xpd_status(xpd, ZT_ALARM_RED);
#endif
	}
	if (priv->state_register.bits.v_su_sta == new_state.bits.v_su_sta)
		return;	/* same same */
	DBG("%02X ---> %02X\n", priv->state_register.reg, reg_x30);
	DBG("%s/%s: %s%i\n", xbus->busname, xpd->xpdname,
				(xpd->type == XPD_TYPE_BRI_NT)?"G":"F",
				new_state.bits.v_su_sta);

	if(xpd->type == XPD_TYPE_BRI_TE) {
		/* disable T3 ? */
		if ((new_state.bits.v_su_sta <= ST_TE_DEACTIVATED) || (new_state.bits.v_su_sta >= ST_TE_ACTIVATED)) {
			DBG("%s/%s: Disable T3 ?\n", xbus->busname, xpd->xpdname);
			priv->t3 = HFC_TIMER_OFF;
		}
		switch (new_state.bits.v_su_sta) {
			case ST_TE_DEACTIVATED:		/* F3 */
				DBG("%s/%s: State ST_TE_DEACTIVATED (F3)\n", xbus->busname, xpd->xpdname);
				if (test_and_clear_bit(HFC_L1_ACTIVATED, &priv->l1_flags))
					priv->t4 = HFC_TIMER_T4;
				break;
			case ST_TE_SIGWAIT:		/* F4	*/
				DBG("%s/%s: State ST_TE_SIGWAIT (F4)\n", xbus->busname, xpd->xpdname);
				break;
			case ST_TE_IDENT:		/* F5	*/
				DBG("%s/%s: State ST_TE_IDENT (F5)\n", xbus->busname, xpd->xpdname);
				break;
			case ST_TE_SYNCED:		/* F6	*/
				DBG("%s/%s: State ST_TE_SYNCED (F6)\n", xbus->busname, xpd->xpdname);
				break;
			case ST_TE_ACTIVATED:		/* F7 */
				DBG("%s/%s: State ST_TE_ACTIVATED (F7)\n", xbus->busname, xpd->xpdname);
				if (priv->t4 > HFC_TIMER_OFF)
					priv->t4 = HFC_TIMER_OFF;
				clear_bit(HFC_L1_ACTIVATING, &priv->l1_flags);
				set_bit(HFC_L1_ACTIVATED, &priv->l1_flags);
				update_xpd_status(xpd, ZT_ALARM_NONE);
				break;

			case ST_TE_LOST_FRAMING:	/* F8 */
				DBG("%s/%s: State ST_TE_LOST_FRAMING (F8)\n", xbus->busname, xpd->xpdname);
				priv->t4 = HFC_TIMER_OFF;
				break;
			default:
				NOTICE("%s/%s: Bad TE state: %d\n", xbus->busname, xpd->xpdname, new_state.bits.v_su_sta);
				break;
		}

	} else if(xpd->type == XPD_TYPE_BRI_NT) {
		switch (new_state.bits.v_su_sta) {
			case ST_NT_DEACTIVATED:		/* G1 */
				DBG("%s/%s: State ST_NT_DEACTIVATED (G1)\n", xbus->busname, xpd->xpdname);
				clear_bit(HFC_L1_ACTIVATED, &priv->l1_flags);
				priv->t1 = HFC_TIMER_OFF;
				break;
			case ST_NT_ACTIVATING:		/* G2 */
				DBG("%s/%s: State ST_NT_ACTIVATING (G2)\n", xbus->busname, xpd->xpdname);
				xpd_activation(xpd, 1);
				break;
			case ST_NT_ACTIVATED:		/* G3 */
				DBG("%s/%s: State ST_NT_ACTIVATED (G3)\n", xbus->busname, xpd->xpdname);
				clear_bit(HFC_L1_ACTIVATING, &priv->l1_flags);
				set_bit(HFC_L1_ACTIVATED, &priv->l1_flags);
				priv->t1 = HFC_TIMER_OFF;
				break;
			case ST_NT_DEACTIVTING:		/* G4 */
				DBG("%s/%s: State ST_NT_DEACTIVTING (G4)\n", xbus->busname, xpd->xpdname);
				priv->t1 = HFC_TIMER_OFF;
				break;
			default:
				NOTICE("%s/%s: Bad NT state: %d\n", xbus->busname, xpd->xpdname, new_state.bits.v_su_sta);
				break;
		}
	} else
		ERR("%s: %s/%s: Unknown xpd type %d\n", __FUNCTION__,
				xpd->xbus->busname, xpd->xpdname, xpd->type);
	priv->state_register.reg = new_state.reg;
}

HANDLER_DEF(BRI, REGISTER_REPLY)
{
	reg_cmd_t		*info = &RPACKET_FIELD(pack, BRI, REGISTER_REPLY, regcmd);
	unsigned long		flags;
	struct BRI_priv_data	*priv;
	int			ret;

	if(!xpd) {
		NOTICE("%s: received %s for non-existing xpd: %d\n",
				__FUNCTION__, cmd->name, XPD_NUM(pack->addr));
		return -EPROTO;
	}
	spin_lock_irqsave(&xpd->lock, flags);
	priv = xpd->priv;
	BUG_ON(!priv);
#if 0
	if(REG_FIELD(info, do_subreg)) {
		DBG("REGISTER_REPLY: %s/%s: RS %02X %02X %02X\n",
				xbus->busname, xpd->xpdname, REG_FIELD(info, regnum), REG_FIELD(info, subreg), REG_FIELD(info, data_low));
	} else {
		if (REG_FIELD(info, regnum) != A_SU_RD_STA)
			DBG("REGISTER_REPLY: %s/%s: RD %02X %02X\n",
					xbus->busname, xpd->xpdname, REG_FIELD(info, regnum), REG_FIELD(info, data_low));
	}
#endif
	if(info->multibyte) {
#if 0
		DBG("%s/%s: Got Multibyte: %d bytes, eoframe: %d\n",
				xbus->busname, xpd->xpdname,
				info->bytes, info->eoframe);
#endif
		ret = rx_dchan(xpd, info);
		if (ret < 0) {
			priv->dchan_rx_drops++;
			if(atomic_read(&xpd->open_counter) > 0)
				NOTICE("%s/%s: Multibyte Drop: errno=%d\n", xbus->busname, xpd->xpdname, ret);
		} 
		goto end;
	}
	if(REG_FIELD(info, regnum) == A_SU_RD_STA) {
		su_new_state(xpd, REG_FIELD(info, data_low));
	}

	/* Update /proc info only if reply relate to the last slic read request */
	if(
			REG_FIELD(&priv->requested_reply, regnum) == REG_FIELD(info, regnum) &&
			REG_FIELD(&priv->requested_reply, do_subreg) == REG_FIELD(info, do_subreg) &&
			REG_FIELD(&priv->requested_reply, subreg) == REG_FIELD(info, subreg)) {
		priv->last_reply = *info;
	}
	
end:
	spin_unlock_irqrestore(&xpd->lock, flags);
	return 0;
}

xproto_table_t PROTO_TABLE(BRI_NT) = {
	.owner = THIS_MODULE,
	.entries = {
		/*	Table	Card	Opcode		*/
		XENTRY(	BRI_NT,	BRI,	REGISTER_REPLY		),
	},
	.name = "BRI_NT",
	.type = XPD_TYPE_BRI_NT,
	.xops = {
		.card_new	= BRI_card_new,
		.card_init	= BRI_card_init,
		.card_remove	= BRI_card_remove,
		.card_zaptel_preregistration	= BRI_card_zaptel_preregistration,
		.card_zaptel_postregistration	= BRI_card_zaptel_postregistration,
		.card_hooksig	= BRI_card_hooksig,
		.card_tick	= BRI_card_tick,
		.card_close	= BRI_card_close,
		.span_startup	= BRI_span_startup,
		.span_shutdown	= BRI_span_shutdown,

		.RING		= XPROTO_CALLER(BRI, RING),
		.RELAY_OUT	= XPROTO_CALLER(BRI, RELAY_OUT),
		.XPD_STATE	= XPROTO_CALLER(BRI, XPD_STATE),
	},
	.packet_is_valid = bri_packet_is_valid,
	.packet_dump = bri_packet_dump,
};

xproto_table_t PROTO_TABLE(BRI_TE) = {
	.owner = THIS_MODULE,
	.entries = {
		/*	Table	Card	Opcode		*/
		XENTRY(	BRI_TE,	BRI,	REGISTER_REPLY		),
	},
	.name = "BRI_TE",
	.type = XPD_TYPE_BRI_TE,
	.xops = {
		.card_new	= BRI_card_new,
		.card_init	= BRI_card_init,
		.card_remove	= BRI_card_remove,
		.card_zaptel_preregistration	= BRI_card_zaptel_preregistration,
		.card_zaptel_postregistration	= BRI_card_zaptel_postregistration,
		.card_hooksig	= BRI_card_hooksig,
		.card_tick	= BRI_card_tick,
		.span_startup	= BRI_span_startup,
		.span_shutdown	= BRI_span_shutdown,

		.RING		= XPROTO_CALLER(BRI, RING),
		.RELAY_OUT	= XPROTO_CALLER(BRI, RELAY_OUT),
		.XPD_STATE	= XPROTO_CALLER(BRI, XPD_STATE),
	},
	.packet_is_valid = bri_packet_is_valid,
	.packet_dump = bri_packet_dump,
};

static bool bri_packet_is_valid(xpacket_t *pack)
{
	const xproto_entry_t	*xe_nt = NULL;
	const xproto_entry_t	*xe_te = NULL;
	// DBG("\n");
	xe_nt = xproto_card_entry(&PROTO_TABLE(BRI_NT), pack->opcode);
	xe_te = xproto_card_entry(&PROTO_TABLE(BRI_TE), pack->opcode);
	return xe_nt != NULL || xe_te != NULL;
}

static void bri_packet_dump(const char *msg, xpacket_t *pack)
{
	DBG("%s\n", msg);
}
/*------------------------- REGISTER Handling --------------------------*/

static int proc_bri_info_read(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	int			len = 0;
	unsigned long		flags;
	xpd_t			*xpd = data;
	struct BRI_priv_data	*priv;
	int			led;

	DBG("\n");
	if(!xpd)
		return -ENODEV;
	spin_lock_irqsave(&xpd->lock, flags);
	priv = xpd->priv;
	BUG_ON(!priv);
	len += sprintf(page + len, "%05d Layer 1: ", priv->poll_counter);
	if(priv->reg30_good) {
		len += sprintf(page + len, "%-5s ", (priv->layer1_up) ? "UP" : "DOWN");
		len += sprintf(page + len, "%c%d %-15s -- fr_sync=%d t2_exp=%d info0=%d g2_g3=%d\n",
					(xpd->type == XPD_TYPE_BRI_NT)?'G':'F',
					priv->state_register.bits.v_su_sta,
					xhfc_state_name(xpd->type, priv->state_register.bits.v_su_sta),
					priv->state_register.bits.v_su_fr_sync,
					priv->state_register.bits.v_su_t2_exp,
					priv->state_register.bits.v_su_info0,
					priv->state_register.bits.v_g2_g3);
	} else
		len += sprintf(page + len, "Unkown\n");
	if(xpd->type == XPD_TYPE_BRI_NT)
		len += sprintf(page + len, "T1 Timer: %d\n", priv->t1);
	len += sprintf(page + len, "Tick Counter: %d\n", priv->tick_counter);
	len += sprintf(page + len, "Last Poll Reply: %d ticks ago\n", priv->reg30_ticks);
	len += sprintf(page + len, "reg30_good=%d\n", priv->reg30_good);
	len += sprintf(page + len, "D-Channel: TX=[%5d]    RX=[%5d]    BAD=[%5d] (%s)\n\n",
			priv->dchan_tx_counter, priv->dchan_rx_counter, priv->dchan_rx_drops,
			(priv->dchan_alive) ? "alive" : "dead");
	len += sprintf(page + len, "dchan_alive_ticks: %d\n", priv->dchan_alive_ticks);
	len += sprintf(page + len, "dchan_notx_ticks: %d\n", priv->dchan_notx_ticks);
	len += sprintf(page + len, "dchan_norx_ticks: %d\n", priv->dchan_norx_ticks);
	for(led = 0; led < NUM_LEDS; led++) {
		len += sprintf(page + len, "LED #%d\n", led);
		len += sprintf(page + len, "\tledstate  : %d\n", priv->ledstate[led]);
	}
	len += sprintf(page + len, "\nDCHAN:\n");
	len += sprintf(page + len, "\n");
	spin_unlock_irqrestore(&xpd->lock, flags);
	if (len <= off+count)
		*eof = 1;
	*start = page + off;
	len -= off;
	if (len > count)
		len = count;
	if (len < 0)
		len = 0;
	return len;
}

/*
 *
 * Direct/Indirect
 *     |
 *     | Reg#
 *     | |
 *     | |  Data (only in Write)
 *     | |    |
 *     | |  +-+-+
 *     v v  v   v
 * FF WD 06 01 05
 * ^  ^
 * |  |
 * |  Write/Read
 * |
 * Chan#
 *
 */
static int handle_register_command(xpd_t *xpd, char *cmdline)
{
	unsigned		chipsel;
	unsigned		data = 0;
	unsigned		xdata1 = 0;
	unsigned		xdata2 = 0;
	char			op;		/* [W]rite, [R]ead */
	char			reg_type;	/* [D]irect, [S]ubregister */
	int			reg_num;
	int			subreg;
	int			elements;
	bool			writing;
	char			*p;
	reg_cmd_t		regcmd;
	xbus_t			*xbus;
	int			ret;
	struct BRI_priv_data	*priv;
	byte			buf[MAX_PROC_WRITE];

	if((p = strchr(cmdline, '#')) != NULL)	/* Truncate comments */
		*p = '\0';
	if((p = strchr(cmdline, ';')) != NULL)	/* Truncate comments */
		*p = '\0';
	for(p = cmdline; *p && (*p == ' ' || *p == '\t'); p++) /* Trim leading whitespace */
		;
	if(*p == '\0')
		return 0;

	memset(buf, 0, MAX_PROC_WRITE);
	elements = sscanf(cmdline, "%d %c%c %x %x %x %x %x",
			&chipsel,
			&op, &reg_type, &reg_num,
			&subreg,
			&data, &xdata1, &xdata2);
	// DBG("'%s': %d %c%c %02X %02X %02X\n", cmdline, chipsel, op, reg_type, reg_num, subreg, data);
	if(elements < 3) {	// At least: chipsel, op, reg_type, reg_num
		ERR("Not enough arguments: (%d args) '%s'\n", elements, cmdline);
		return -EINVAL;
	}
	if(!VALID_CHIPSEL(chipsel)) {
		ERR("Bad chip select number: %d\n", chipsel);
		return -EINVAL;
	}
	REG_FIELD(&regcmd, chipsel) = chipsel;
	switch(op) {
		case 'W':
			writing = 1;
			break;
		case 'R':
			writing = 0;
			break;
		default:
			ERR("Unkown operation type '%c'\n", op);
			return -EINVAL;
	}
	if(
			(op == 'W' && reg_type == 'D' && elements != 5) ||
			(op == 'W' && reg_type == 'S' && elements != 6) ||
			(op == 'R' && reg_type == 'D' && elements != 4) ||
			(op == 'R' && reg_type == 'S' && elements != 4)
	  ) {
		ERR("Bad number of elements: '%s' (%d elements): %d %c%c %02X %02X %02X\n",
				cmdline, elements,
				chipsel, op, reg_type, reg_num, subreg, data);
		return -EINVAL;
	}
	switch(reg_type) {
		case 'S':
			REG_FIELD(&regcmd, do_subreg) = 1;
			REG_FIELD(&regcmd, regnum) = reg_num;
			REG_FIELD(&regcmd, subreg) = subreg;
			REG_FIELD(&regcmd, data_low) = data;
			break;
		case 'D':
			REG_FIELD(&regcmd, do_subreg) = 0;
			REG_FIELD(&regcmd, regnum) = reg_num;
			REG_FIELD(&regcmd, subreg) = 0;
			REG_FIELD(&regcmd, data_low) = subreg;
			break;
		case 'M':	/* Multi without eoftx */
		case 'm':	/* Multi with eoftx */
			if(!writing) {
				ERR("Read multibyte is not implemented\n");
				return -EINVAL;
			}
			elements -= 3;
			REG_XDATA(&regcmd)[0] = reg_num;
			REG_XDATA(&regcmd)[1] = subreg;
			REG_XDATA(&regcmd)[2] = data;
			REG_XDATA(&regcmd)[3] = xdata1;
			REG_XDATA(&regcmd)[4] = xdata2;
			return send_bri_multibyte(xpd, REG_XDATA(&regcmd), elements, (reg_type == 'm'));
		default:
			ERR("Unkown register type '%c'\n", reg_type);
			return -EINVAL;
	}
	regcmd.bytes = sizeof(regcmd) - 1;
	REG_FIELD(&regcmd, read_request) = writing;
	REG_FIELD(&regcmd, data_high) = 0;
	BUG_ON(!xpd);
	xbus = xpd->xbus;
	if(!down_read_trylock(&xbus->in_use)) {
		DBG("Dropped packet. %s is in_use\n", xbus->busname);
		return -EBUSY;
	}
	priv = xpd->priv;
	BUG_ON(!priv);
	priv->requested_reply = regcmd;
	if(print_dbg)
		dump_reg_cmd("BRI", &regcmd);
	ret = CALL_PROTO(BRI, REGISTER_REQUEST, xpd->xbus, xpd,
			REG_FIELD(&regcmd, chipsel),
			writing,
			REG_FIELD(&regcmd, do_subreg),
			REG_FIELD(&regcmd, regnum),
			REG_FIELD(&regcmd, subreg),
			REG_FIELD(&regcmd, data_low),
			REG_FIELD(&regcmd, data_high));
	up_read(&xbus->in_use);
	return ret;
}

static int proc_xpd_register_write(struct file *file, const char __user *buffer, unsigned long count, void *data)
{
	xpd_t		*xpd = data;
	char		buf[MAX_PROC_WRITE];
	char		*p;
	int		i;
	int		ret;

	if(!xpd)
		return -ENODEV;
	for(i = 0; i < count; /* noop */) {
		for(p = buf; p < buf + MAX_PROC_WRITE; p++) {	/* read a line */
			if(i >= count)
				break;
			if(get_user(*p, buffer + i))
				return -EFAULT;
			i++;
			if(*p == '\n' || *p == '\r')	/* whatever */
				break;
		}
		if(p >= buf + MAX_PROC_WRITE)
			return -E2BIG;
		*p = '\0';
		ret = handle_register_command(xpd, buf);
		if(ret < 0)
			return ret;
		msleep(1);
	}
	return count;
}


static int proc_xpd_register_read(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	int			len = 0;
	unsigned long		flags;
	xpd_t			*xpd = data;
	reg_cmd_t		*info;
	struct BRI_priv_data	*priv;

	if(!xpd)
		return -ENODEV;
	priv = xpd->priv;
	BUG_ON(!priv);
	spin_lock_irqsave(&xpd->lock, flags);
	info = &priv->last_reply;
	len += sprintf(page + len, "# Writing bad data into this file may damage your hardware!\n");
	len += sprintf(page + len, "# Consult firmware docs first\n");
	len += sprintf(page + len, "#\n");
	if(REG_FIELD(info, do_subreg)) {
		len += sprintf(page + len, "#CH\tOP\tReg.\tSub\tDL\n");
		len += sprintf(page + len, "%2d\tRS\t%02X\t%02X\t%02X\n",
				REG_FIELD(info, chipsel),
				REG_FIELD(info, regnum), REG_FIELD(info, subreg), REG_FIELD(info, data_low));
	} else {
		len += sprintf(page + len, "#CH\tOP\tReg.\tDL\n");
		len += sprintf(page + len, "%2d\tRD\t%02X\t%02X\n",
				REG_FIELD(info, chipsel),
				REG_FIELD(info, regnum), REG_FIELD(info, data_low));
	}
	spin_unlock_irqrestore(&xpd->lock, flags);
	if (len <= off+count)
		*eof = 1;
	*start = page + off;
	len -= off;
	if (len > count)
		len = count;
	if (len < 0)
		len = 0;
	return len;
}

int __init card_bri_startup(void)
{
	DBG("\n");

	INFO("%s revision %s\n", THIS_MODULE->name, XPP_VERSION);
	xproto_register(&PROTO_TABLE(BRI_NT));
	xproto_register(&PROTO_TABLE(BRI_TE));
	return 0;
}

void __exit card_bri_cleanup(void)
{
	DBG("\n");
	xproto_unregister(&PROTO_TABLE(BRI_NT));
	xproto_unregister(&PROTO_TABLE(BRI_TE));
}

MODULE_DESCRIPTION("XPP BRI Card Driver");
MODULE_AUTHOR("Oron Peled <oron@actcom.co.il>");
MODULE_LICENSE("GPL");
MODULE_VERSION(XPP_VERSION);
MODULE_ALIAS_XPD(XPD_TYPE_BRI_NT);
MODULE_ALIAS_XPD(XPD_TYPE_BRI_TE);

module_init(card_bri_startup);
module_exit(card_bri_cleanup);
