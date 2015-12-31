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

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include "xpd.h"
#include "xproto.h"
#include "xpp_zap.h"
#include "card_fxo.h"
#include "zap_debug.h"
#include "xbus-core.h"

static const char rcsid[] = "$Id: card_fxo.c 2591 2007-06-07 22:36:45Z tzafrir $";

DEF_PARM(int, print_dbg, 0, 0644, "Print DBG statements");
DEF_PARM(uint, poll_battery_interval, 500, 0644, "Poll battery interval in milliseconds (0 - disable)");
DEF_PARM(int, ring_debounce, 50, 0644, "Number of ticks to debounce a false RING indication");

/* Signaling is opposite (fxs signalling for fxo card) */
#if 1
#define	FXO_DEFAULT_SIGCAP	(ZT_SIG_FXSKS | ZT_SIG_FXSLS)
#else
#define	FXO_DEFAULT_SIGCAP	(ZT_SIG_SF)
#endif

enum fxo_leds {
	LED_GREEN,
};

#define	NUM_LEDS		1
#define	DELAY_UNTIL_DIALTONE	3000

#define	BAT_THRESHOLD		3
#define	BAT_DEBOUNCE		3	/* compensate for battery voltage fluctuation (in poll_battery_interval's) */

static /* 0x0F */ DECLARE_CMD(FXO, REGISTER_REQUEST, byte chipsel, bool writing, bool do_subreg, byte regnum, byte subreg, byte data_low, byte data_high);
/* Shortcuts */
#define	DAA_WRITE	1
#define	DAA_READ	0
#define	DAA_DIRECT_REQUEST(xbus,xpd,chipsel,writing,reg,dL)	\
	CALL_PROTO(FXO, REGISTER_REQUEST, (xbus), (xpd), (chipsel), (writing), 0, (reg), 0, (dL), 0)

#define	VALID_CHIPSEL(x)	(((chipsel) >= 0 && (chipsel) <= 7) || (chipsel) == ALL_CHANS)

/*---------------- FXO Protocol Commands ----------------------------------*/

static /* 0x0F */ DECLARE_CMD(FXO, XPD_STATE, bool on);
static /* 0x0F */ DECLARE_CMD(FXO, RING, lineno_t chan, bool on);
static /* 0x0F */ DECLARE_CMD(FXO, RELAY_OUT, byte which, bool on);

static bool fxo_packet_is_valid(xpacket_t *pack);
static void fxo_packet_dump(const char *msg, xpacket_t *pack);
static int proc_fxo_info_read(char *page, char **start, off_t off, int count, int *eof, void *data);
static int proc_xpd_register_read(char *page, char **start, off_t off, int count, int *eof, void *data);
static int proc_xpd_register_write(struct file *file, const char __user *buffer, unsigned long count, void *data);
static int handle_register_command(xpd_t *xpd, char *cmdline);

#define	PROC_REGISTER_FNAME	"slics"
#define	PROC_FXO_INFO_FNAME	"fxo_info"

#define	DAA_CURRENT_REGISTER	0x1C
#define	DAA_RING_REGISTER	0x05

#define	POWER_DENIAL_CURRENT	3
#define	POWER_DENIAL_TIME	1000	/* ticks */

struct FXO_priv_data {
	struct proc_dir_entry	*regfile;
	struct proc_dir_entry	*fxo_info;
	uint			poll_counter;
	xpp_line_t		battery;
	ushort			battery_debounce[CHANNELS_PERXPD];
	xpp_line_t		polarity;
	ushort			polarity_counter[CHANNELS_PERXPD];
	uint			offhook_timestamp[CHANNELS_PERXPD];
	ushort			current_counter[CHANNELS_PERXPD];
	xpp_line_t		ledstate[NUM_LEDS];	/* 0 - OFF, 1 - ON */
	xpp_line_t		ledcontrol[NUM_LEDS];	/* 0 - OFF, 1 - ON */
	int			led_counter[NUM_LEDS][CHANNELS_PERXPD];
	atomic_t		ring_debounce[CHANNELS_PERXPD];
};

/*
 * LED counter values:
 *	n>1	: BLINK every n'th tick
 */
#define	LED_COUNTER(priv,pos,color)	((priv)->led_counter[color][pos])
#define	IS_BLINKING(priv,pos,color)	(LED_COUNTER(priv,pos,color) > 0)
#define	MARK_BLINK(priv,pos,color,t)	((priv)->led_counter[color][pos] = (t))
#define	MARK_OFF(priv,pos,color)	do { BIT_CLR((priv)->ledcontrol[color],(pos)); MARK_BLINK((priv),(pos),(color),0); } while(0)
#define	MARK_ON(priv,pos,color)		do { BIT_SET((priv)->ledcontrol[color],(pos)); MARK_BLINK((priv),(pos),(color),0); } while(0)

#define	LED_BLINK_RING			(1000/8)	/* in ticks */

/*---------------- FXO: Static functions ----------------------------------*/

/*
 * LED control is done via DAA register 0x20
 */
static int do_led(xpd_t *xpd, lineno_t chan, byte which, bool on)
{
	int			ret = 0;
	struct FXO_priv_data	*priv;
	xbus_t			*xbus;

	BUG_ON(!xpd);
	xbus = xpd->xbus;
	priv = xpd->priv;
	which = which % NUM_LEDS;
	if(IS_SET(xpd->digital_outputs, chan) || IS_SET(xpd->digital_inputs, chan))
		goto out;
	if(chan == ALL_CHANS) {
		priv->ledstate[which] = (on) ? ~0 : 0;
	} else {
		if(on) {
			BIT_SET(priv->ledstate[which], chan);
		} else {
			BIT_CLR(priv->ledstate[which], chan);
		}
	}
	DBG("%s/%s/%d: LED: which=%d -- %s\n", xbus->busname, xpd->xpdname, chan, which, (on) ? "on" : "off");
	ret = DAA_DIRECT_REQUEST(xbus, xpd, chan, DAA_WRITE, 0x20, on);
out:
	return ret;
}

static void handle_fxo_leds(xpd_t *xpd)
{
	int			i;
	unsigned long		flags;
	const enum fxo_leds	color = LED_GREEN;
	unsigned int		timer_count;
	struct FXO_priv_data	*priv;

	BUG_ON(!xpd);
	spin_lock_irqsave(&xpd->lock, flags);
	priv = xpd->priv;
	timer_count = xpd->timer_count;
	for_each_line(xpd, i) {
		if(IS_SET(xpd->digital_outputs, i) || IS_SET(xpd->digital_inputs, i))
			continue;
		if(xpd->blink_mode || IS_BLINKING(priv,i,color)) {
			int	mod_value = LED_COUNTER(priv, i, color);

			if(!mod_value)
				mod_value = DEFAULT_LED_PERIOD;		/* safety value */
			// led state is toggled
			if((timer_count % mod_value) == 0) {
				DBG("%s/%s/%d: ledstate=%s\n", xpd->xbus->busname, xpd->xpdname, i,
						(IS_SET(priv->ledstate[color], i))?"ON":"OFF");
				if(!IS_SET(priv->ledstate[color], i)) {
					do_led(xpd, i, color, 1);
				} else {
					do_led(xpd, i, color, 0);
				}
			}
		} else if(IS_SET(priv->ledcontrol[color], i) && !IS_SET(priv->ledstate[color], i)) {
			do_led(xpd, i, color, 1);
		} else if(!IS_SET(priv->ledcontrol[color], i) && IS_SET(priv->ledstate[color], i)) {
			do_led(xpd, i, color, 0);
		}
	}
	spin_unlock_irqrestore(&xpd->lock, flags);
}

static void mark_ring(xpd_t *xpd, lineno_t pos, bool on, bool update_zap)
{
	struct FXO_priv_data	*priv;

	priv = xpd->priv;
	BUG_ON(!priv);
	atomic_set(&priv->ring_debounce[pos], 0);	/* Stop debouncing */
	if(on && !xpd->ringing[pos]) {
		DBG("%s/%s/%d: START\n", xpd->xbus->busname, xpd->xpdname, pos);
		xpd->ringing[pos] = 1;
		MARK_BLINK(priv, pos, LED_GREEN, LED_BLINK_RING);
		if(update_zap)
			update_zap_ring(xpd, pos, on);
	} else if(!on && xpd->ringing[pos]) {
		DBG("%s/%s/%d: STOP\n", xpd->xbus->busname, xpd->xpdname, pos);
		xpd->ringing[pos] = 0;
		if(IS_BLINKING(priv, pos, LED_GREEN))
			MARK_BLINK(priv, pos, LED_GREEN, 0);
		if(update_zap)
			update_zap_ring(xpd, pos, on);
	}
}

static int do_sethook(xpd_t *xpd, int pos, bool to_offhook)
{
	unsigned long		flags;
	xbus_t			*xbus;
	struct FXO_priv_data	*priv;
	int			ret = 0;
	byte			value;

	BUG_ON(!xpd);
	BUG_ON(xpd->direction == TO_PHONE);		// We can SETHOOK state only on PSTN
	xbus = xpd->xbus;
	priv = xpd->priv;
	BUG_ON(!priv);
	if(!IS_SET(priv->battery, pos)) {
		DBG("%s/%s/%d: WARNING: called while battery is off\n", xbus->busname, xpd->xpdname, pos);
	}
	spin_lock_irqsave(&xpd->lock, flags);
	mark_ring(xpd, pos, 0, 0);				// No more rings
	value = (to_offhook) ? 0x09 : 0x08;	/* Bit 3 is for CID */
	DBG("%s/%s/%d: SETHOOK: value=0x%02X %s\n", xbus->busname, xpd->xpdname, pos, value, (to_offhook)?"OFFHOOK":"ONHOOK");
	if(to_offhook)
		MARK_ON(priv, pos, LED_GREEN);
	else
		MARK_OFF(priv, pos, LED_GREEN);
	ret = DAA_DIRECT_REQUEST(xbus, xpd, pos, DAA_WRITE, DAA_RING_REGISTER, value);
	if(to_offhook) {
		BIT_SET(xpd->offhook, pos);
		priv->offhook_timestamp[pos] = priv->poll_counter;
	} else {
		BIT_CLR(xpd->offhook, pos);
		BIT_CLR(xpd->cid_on, pos);
	}
	spin_unlock_irqrestore(&xpd->lock, flags);
	return ret;
}

/*---------------- FXO: Methods -------------------------------------------*/

static xpd_t *FXO_card_new(xbus_t *xbus, int xpd_num, const xproto_table_t *proto_table, byte revision)
{
	xpd_t		*xpd = NULL;
	int		channels = min(8, CHANNELS_PERXPD);

	xpd = xpd_alloc(sizeof(struct FXO_priv_data), xbus, xpd_num, proto_table, channels, revision);
	if(!xpd)
		return NULL;
	xpd->direction = TO_PSTN;
	xpd->revision = revision;
	return xpd;
}

static void clean_proc(xbus_t *xbus, xpd_t *xpd)
{
	struct FXO_priv_data	*priv;

	BUG_ON(!xpd);
	priv = xpd->priv;
	DBG("%s/%s\n", xbus->busname, xpd->xpdname);
#ifdef	CONFIG_PROC_FS
	if(priv->regfile) {
		DBG("Removing xpd DAA file %s/%s\n", xbus->busname, xpd->xpdname);
		remove_proc_entry(PROC_REGISTER_FNAME, xpd->proc_xpd_dir);
		priv->regfile->data = NULL;
	}
	if(priv->fxo_info) {
		DBG("Removing xpd FXO_INFO file %s/%s\n", xbus->busname, xpd->xpdname);
		remove_proc_entry(PROC_FXO_INFO_FNAME, xpd->proc_xpd_dir);
		priv->fxo_info = NULL;
	}
#endif
}

static int FXO_card_init(xbus_t *xbus, xpd_t *xpd)
{
	struct FXO_priv_data	*priv;
	int			ret = 0;
	int			i;

	BUG_ON(!xpd);
	priv = xpd->priv;
#ifdef	CONFIG_PROC_FS
	DBG("Creating FXO_INFO file for %s/%s\n", xbus->busname, xpd->xpdname);
	priv->fxo_info = create_proc_read_entry(PROC_FXO_INFO_FNAME, 0444, xpd->proc_xpd_dir, proc_fxo_info_read, xpd);
	if(!priv->fxo_info) {
		ERR("Failed to create proc '%s' for %s/%s\n", PROC_FXO_INFO_FNAME, xbus->busname, xpd->xpdname);
		ret = -ENOENT;
		goto err;
	}
	priv->fxo_info->owner = THIS_MODULE;
	DBG("Creating DAAs file for %s/%s\n", xbus->busname, xpd->xpdname);
	priv->regfile = create_proc_entry(PROC_REGISTER_FNAME, 0644, xpd->proc_xpd_dir);
	if(!priv->regfile) {
		ERR("Failed to create proc file for DAAs of %s/%s\n", xbus->busname, xpd->xpdname);
		ret = -ENOENT;
		goto err;
	}
	priv->regfile->owner = THIS_MODULE;
	priv->regfile->write_proc = proc_xpd_register_write;
	priv->regfile->read_proc = proc_xpd_register_read;
	priv->regfile->data = xpd;
#endif
	ret = run_initialize_registers(xpd);
	if(ret < 0)
		goto err;
	// Hanghup all lines
	for_each_line(xpd, i) {
		do_sethook(xpd, i, 0);
	}
	DBG("done: %s/%s\n", xbus->busname, xpd->xpdname);
	for_each_line(xpd, i) {
		do_led(xpd, i, LED_GREEN, 0);
	}
	for_each_line(xpd, i) {
		do_led(xpd, i, LED_GREEN, 1);
		msleep(50);
	}
	for_each_line(xpd, i) {
		do_led(xpd, i, LED_GREEN, 0);
		msleep(50);
	}
	return 0;
err:
	clean_proc(xbus, xpd);
	ERR("%s/%s: Failed initializing registers (%d)\n", xbus->busname, xpd->xpdname, ret);
	return ret;
}

static int FXO_card_remove(xbus_t *xbus, xpd_t *xpd)
{
	struct FXO_priv_data	*priv;

	BUG_ON(!xpd);
	priv = xpd->priv;
	DBG("%s/%s\n", xbus->busname, xpd->xpdname);
	clean_proc(xbus, xpd);
	return 0;
}

static int FXO_card_zaptel_preregistration(xpd_t *xpd, bool on)
{
	xbus_t			*xbus;
	struct FXO_priv_data	*priv;
	int			i;

	BUG_ON(!xpd);
	xbus = xpd->xbus;
	BUG_ON(!xbus);
	priv = xpd->priv;
	BUG_ON(!priv);
	DBG("%s/%s (%d)\n", xbus->busname, xpd->xpdname, on);
	snprintf(xpd->span.desc, MAX_SPANDESC, "Xorcom XPD #%d/%d: FXO", xbus->num, xpd->id);
	for_each_line(xpd, i) {
		struct zt_chan	*cur_chan = &xpd->chans[i];

		DBG("setting FXO channel %d\n", i);
		snprintf(cur_chan->name, MAX_CHANNAME, "XPP_FXO/%d/%d/%d", xbus->num, xpd->id, i);
		cur_chan->chanpos = i + 1;
		cur_chan->pvt = xpd;
		cur_chan->sigcap = FXO_DEFAULT_SIGCAP;
	}
	for_each_line(xpd, i) {
		MARK_ON(priv, i, LED_GREEN);
		msleep(4);
	}
	return 0;
}

static int FXO_card_zaptel_postregistration(xpd_t *xpd, bool on)
{
	xbus_t			*xbus;
	struct FXO_priv_data	*priv;
	int			i;

	BUG_ON(!xpd);
	xbus = xpd->xbus;
	BUG_ON(!xbus);
	priv = xpd->priv;
	BUG_ON(!priv);
	DBG("%s/%s (%d)\n", xbus->busname, xpd->xpdname, on);
	for_each_line(xpd, i) {
		MARK_OFF(priv, i, LED_GREEN);
		msleep(2);
		// MARK_OFF(priv, i, LED_RED);
		msleep(2);
	}
	return 0;
}

int FXO_card_hooksig(xbus_t *xbus, xpd_t *xpd, int pos, zt_txsig_t txsig)
{
	struct FXO_priv_data	*priv;

	priv = xpd->priv;
	BUG_ON(!priv);
	DBG("%s/%s/%d: %s\n", xbus->busname, xpd->xpdname, pos, txsig2str(txsig));
	BUG_ON(xpd->direction != TO_PSTN);
	/* XXX Enable hooksig for FXO XXX */
	switch(txsig) {
		case ZT_TXSIG_START:
			break;
		case ZT_TXSIG_OFFHOOK:
			do_sethook(xpd, pos, 1);
			break;
		case ZT_TXSIG_ONHOOK:
			do_sethook(xpd, pos, 0);
			break;
		default:
			NOTICE("Can't set tx state to %s (%d)\n", txsig2str(txsig), txsig);
			return -EINVAL;
	}
	return 0;
}

static int FXO_card_open(xpd_t *xpd, lineno_t chan)
{
	struct FXO_priv_data	*priv;

	BUG_ON(!xpd);
	priv = xpd->priv;
	/*
	 * We pretend to have battery. If this is really the case
	 * than next calls to update_battery_status() won't change it.
	 * If we don't have battery, than on the next calls to
	 * update_battery_status() a battery_debounce[] cycle would start.
	 * Than, if no-battery is persistent, asterisk would be notified.
	 */
	BIT_SET(priv->battery, chan);
	return 0;
}

static void poll_battery(xbus_t *xbus, xpd_t *xpd)
{
	int	i;

	for_each_line(xpd, i) {
		DAA_DIRECT_REQUEST(xbus, xpd, i, DAA_READ, DAA_VBAT_REGISTER, 0);
	}
}

static void poll_current(xbus_t *xbus, xpd_t *xpd)
{
	int	i;

	for_each_line(xpd, i) {
		if (IS_SET(xpd->offhook, i))
			DAA_DIRECT_REQUEST(xbus, xpd, i, DAA_READ, DAA_CURRENT_REGISTER, 0);
	}
}

static void handle_fxo_ring(xpd_t *xpd)
{
	struct FXO_priv_data	*priv;
	int			i;

	priv = xpd->priv;
	for_each_line(xpd, i) {
		if(atomic_read(&priv->ring_debounce[i]) > 0) {
			/* Maybe start ring */
			if(atomic_dec_and_test(&priv->ring_debounce[i]))
				mark_ring(xpd, i, 1, 1);
		} else if (atomic_read(&priv->ring_debounce[i]) < 0) {
			/* Maybe stop ring */
			if(atomic_inc_and_test(&priv->ring_debounce[i]))
				mark_ring(xpd, i, 0, 1);
		}
	}
}

static int FXO_card_tick(xbus_t *xbus, xpd_t *xpd)
{
	struct FXO_priv_data	*priv;

	BUG_ON(!xpd);
	priv = xpd->priv;
	BUG_ON(!priv);
	if(poll_battery_interval != 0 && (priv->poll_counter % poll_battery_interval) == 0) {
		poll_battery(xbus, xpd);
		poll_current(xbus, xpd);
	}
	handle_fxo_leds(xpd);
	handle_fxo_ring(xpd);
	priv->poll_counter++;
	return 0;
}

/* FIXME: based on data from from wctdm.h */
#include <wctdm.h>
/*
 * The first register is the ACIM, the other are coefficient registers.
 * We define the array size explicitly to track possible inconsistencies
 * if the struct is modified.
 */
static const char echotune_regs[sizeof(struct wctdm_echo_coefs)] = {30, 45, 46, 47, 48, 49, 50, 51, 52};

static int FXO_card_ioctl(xpd_t *xpd, int pos, unsigned int cmd, unsigned long arg)
{
	int 			i,ret;
	unsigned char		echotune_data[ARRAY_SIZE(echotune_regs)];

	BUG_ON(!xpd);
	switch (cmd) {
		case WCTDM_SET_ECHOTUNE:
			DBG("-- Setting echo registers: \n");
			/* first off: check if this span is fxs. If not: -EINVALID */
			if (copy_from_user(&echotune_data, (void __user *)arg, sizeof(echotune_data)))
				return -EFAULT;

			for (i = 0; i < ARRAY_SIZE(echotune_regs); i++) {
				DBG("Reg=0x%02X, data=0x%02X\n", echotune_regs[i], echotune_data[i]);
				ret = DAA_DIRECT_REQUEST(xpd->xbus, xpd, pos, DAA_WRITE, echotune_regs[i], echotune_data[i]);
				if (ret < 0) {
					NOTICE("%s/%s/%d: Couldn't write %0x02X to register %0x02X\n",
							xpd->xbus->busname, xpd->xpdname, pos, echotune_data[i], echotune_regs[i]);
					return ret;
				}
				msleep(1);
			}

			DBG("-- Set echo registers successfully\n");
			break;
		default:
			DBG("%s/%s/%d: Unknown command 0x%X.\n", xpd->xbus->busname, xpd->xpdname, pos, cmd);
			return -ENOTTY;
	}
	return 0;
}

/*---------------- FXO: HOST COMMANDS -------------------------------------*/

/* 0x0F */ HOSTCMD(FXO, REGISTER_REQUEST, byte chipsel, bool writing, bool do_subreg, byte regnum, byte subreg, byte data_low, byte data_high)
{
	int		ret = 0;
	xframe_t	*xframe;
	xpacket_t	*pack;
	reg_cmd_t	*reg_cmd;

	if(!xbus) {
		DBG("NO XBUS\n");
		return -EINVAL;
	}
	XFRAME_NEW(xframe, pack, xbus, GLOBAL, REGISTER_REQUEST, xpd->id);
#if 0
	DBG("%s/%s/%d: %c%c R%02X S%02X %02X %02X\n",
			xbus->busname, xpd->xpdname, chipsel,
			(writing)?'W':'R',
			(do_subreg)?'S':'D',
			regnum, subreg, data_low, data_high);
#endif
	reg_cmd = &RPACKET_FIELD(pack, GLOBAL, REGISTER_REQUEST, reg_cmd);
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

static /* 0x0F */ HOSTCMD(FXO, XPD_STATE, bool on)
{
	int			ret = 0;
	struct FXO_priv_data	*priv;

	BUG_ON(!xbus);
	BUG_ON(!xpd);
	priv = xpd->priv;
	BUG_ON(!priv);
	DBG("%s/%s: %s\n", xbus->busname, xpd->xpdname, (on) ? "on" : "off");
	return ret;
}

static /* 0x0F */ HOSTCMD(FXO, RING, lineno_t chan, bool on)
{
	BUG_ON(!xbus);
	BUG_ON(!xpd);
	DBG("%s/%s/%d: %s\n", xbus->busname, xpd->xpdname, chan, (on) ? "on" : "off");
	return DAA_DIRECT_REQUEST(xbus, xpd, chan, DAA_WRITE, 0x40, (on)?0x04:0x01);
}

static /* 0x0F */ HOSTCMD(FXO, RELAY_OUT, byte which, bool on)
{
	return -ENOSYS;
}

/*---------------- FXO: Astribank Reply Handlers --------------------------*/

HANDLER_DEF(FXO, SIG_CHANGED)
{
	xpp_line_t	sig_status = RPACKET_FIELD(pack, FXO, SIG_CHANGED, sig_status);
	xpp_line_t	sig_toggles = RPACKET_FIELD(pack, FXO, SIG_CHANGED, sig_toggles);
	unsigned long	flags;
	int		i;
	struct FXO_priv_data	*priv;

	if(!xpd) {
		NOTICE("%s: received %s for non-existing xpd: %d\n",
				__FUNCTION__, cmd->name, XPD_NUM(pack->addr));
		return -EPROTO;
	}
	priv = xpd->priv;
	BUG_ON(!priv);
	DBG("%s/%s: (PSTN) sig_toggles=0x%04X sig_status=0x%04X\n", xpd->xbus->busname, xpd->xpdname, sig_toggles, sig_status);
	spin_lock_irqsave(&xpd->lock, flags);
	for_each_line(xpd, i) {
		int	debounce;

		if(IS_SET(sig_toggles, i)) {
			if(!IS_SET(priv->battery, i)) {
				DBG("%s/%s/%d: SIG_CHANGED while battery is off.\n",
					xbus->busname, xpd->xpdname, i);
				// FIXME: allow dialing without battery polling...
				// continue;
			}
			/* First report false ring alarms */
			debounce = atomic_read(&priv->ring_debounce[i]);
			if(debounce)
				NOTICE("%s/%s/%d: debounced %d ticks\n", xbus->busname, xpd->xpdname, i, debounce);
			/*
			 * Now set a new ring alarm.
			 * It will be checked in handle_fxo_ring()
			 */
			debounce = (IS_SET(sig_status, i)) ? ring_debounce : -ring_debounce;
			atomic_set(&priv->ring_debounce[i], debounce);
		}
	}
	spin_unlock_irqrestore(&xpd->lock, flags);
	return 0;
}

static void update_battery_status(xpd_t *xpd, byte data_low, lineno_t chipsel)
{
	struct FXO_priv_data	*priv;
	byte			bat = abs((signed char)data_low);
	byte			pol = IS_SET(data_low, 7);

	priv = xpd->priv;
	BUG_ON(!priv);
	if(bat < BAT_THRESHOLD) {
		/*
		 * Check for battery voltage fluctuations
		 */
		if(IS_SET(priv->battery, chipsel) && priv->battery_debounce[chipsel]++ > BAT_DEBOUNCE) {
			DBG("%s/%s/%d: BATTERY OFF voltage=%d\n", xpd->xbus->busname, xpd->xpdname, chipsel, bat);
			BIT_CLR(priv->battery, chipsel);
			if(SPAN_REGISTERED(xpd))
				zt_qevent_lock(&xpd->chans[chipsel], ZT_EVENT_ALARM);

		}
	} else {
		priv->battery_debounce[chipsel] = 0;
		if(!IS_SET(priv->battery, chipsel)) {
			DBG("%s/%s/%d: BATTERY ON voltage=%d\n", xpd->xbus->busname, xpd->xpdname, chipsel, bat);
			BIT_SET(priv->battery, chipsel);
			if(SPAN_REGISTERED(xpd))
				zt_qevent_lock(&xpd->chans[chipsel], ZT_EVENT_NOALARM);
		}
	}
	/*
	 * Handle reverse polarity
	 */
	if (IS_SET(xpd->offhook, chipsel)) {				/* Learn the current polarity */
		if (priv->poll_counter - priv->offhook_timestamp[chipsel] < 3) {
			priv->polarity_counter[chipsel] = 0;
			if (pol)
				BIT_SET(priv->polarity, chipsel);
			else
				BIT_CLR(priv->polarity, chipsel);
		}
		else if (IS_SET(priv->polarity, chipsel) != pol) {	/* Polarity has reversed */
			priv->polarity_counter[chipsel]++;
			if (priv->polarity_counter[chipsel] >= 2) {
				if (pol)
					BIT_SET(priv->polarity, chipsel);
				else
					BIT_CLR(priv->polarity, chipsel);
				priv->polarity_counter[chipsel] = 0;
				/* Inform Zaptel */
				DBG("%s/%s/%d: Send ZT_EVENT_POLARITY\n", xpd->xbus->busname, xpd->xpdname, chipsel);
				zt_qevent_lock(&xpd->chans[chipsel], ZT_EVENT_POLARITY);
#if 0
				/*
				 * These two lines hangup the channel (by sending a message to
				 * the firmware), and inform Zaptel that the line has been hung-up.
				 * They are not needed if Asterisk does the hangup after receiving
				 * a notification from Zaptel (which is sent by the above zt_qevent_lock().
				 * Asterisk does that if it has "hanguponpolarityswitch=1" in zapata.conf.
				 */
				do_sethook(xpd, chipsel, 0);
				update_line_status(xpd, chipsel, 0);
#endif
			}
		}
	}
}

static void update_power_denial(xpd_t *xpd, byte data_low, lineno_t chipsel)
{
	struct FXO_priv_data	*priv;

	priv = xpd->priv;
	BUG_ON(!priv);
	if (IS_SET(xpd->offhook, chipsel) && data_low < POWER_DENIAL_CURRENT) {
		/* Current dropped */
		priv->current_counter[chipsel]++;
		if (priv->current_counter[chipsel] * poll_battery_interval >= POWER_DENIAL_TIME) {
			DBG("%s/%s/%d: Power Denial Hangup\n", xpd->xbus->busname, xpd->xpdname, chipsel);
			priv->current_counter[chipsel] = 0;
			do_sethook(xpd, chipsel, 0);
			update_line_status(xpd, chipsel, 0);
		}
	} else
		priv->current_counter[chipsel] = 0;
}

HANDLER_DEF(FXO, DAA_REPLY)
{
	reg_cmd_t		*info = &RPACKET_FIELD(pack, FXO, DAA_REPLY, regcmd);
	struct FXO_priv_data	*priv;
	lineno_t		chipsel;

	if(!xpd) {
		NOTICE("%s: received %s for non-existing xpd: %d\n",
				__FUNCTION__, cmd->name, XPD_NUM(pack->addr));
		return -EPROTO;
	}
	priv = xpd->priv;
	BUG_ON(!priv);
	chipsel = REG_FIELD(info, chipsel);
	switch(REG_FIELD(info, regnum)) {
		case DAA_VBAT_REGISTER:
			update_battery_status(xpd, REG_FIELD(info, data_low), chipsel);
			break;
		case DAA_CURRENT_REGISTER:
			update_power_denial(xpd, REG_FIELD(info, data_low), chipsel);
			break;
	}
#if 0
	DBG("DAA_REPLY: xpd #%d %s reg_num=0x%X, dataL=0x%X dataH=0x%X\n",
			xpd->id, (info->size == 3)?"I":"D",
			info->reg_num, info->data_low, info->data_high);
#endif

	/* Update /proc info only if reply relate to the last slic read request */
	if(
			REG_FIELD(&xpd->requested_reply, regnum) == REG_FIELD(info, regnum) &&
			REG_FIELD(&xpd->requested_reply, do_subreg) == REG_FIELD(info, do_subreg) &&
			REG_FIELD(&xpd->requested_reply, subreg) == REG_FIELD(info, subreg)) {
		xpd->last_reply = *info;
	}
	return 0;
}


xproto_table_t PROTO_TABLE(FXO) = {
	.owner = THIS_MODULE,
	.entries = {
		/*	Prototable	Card	Opcode		*/
		XENTRY(	FXO,		FXO,	SIG_CHANGED	),
		XENTRY(	FXO,		FXO,	DAA_REPLY	),
	},
	.name = "FXO",
	.type = XPD_TYPE_FXO,
	.xops = {
		.card_new	= FXO_card_new,
		.card_init	= FXO_card_init,
		.card_remove	= FXO_card_remove,
		.card_zaptel_preregistration	= FXO_card_zaptel_preregistration,
		.card_zaptel_postregistration	= FXO_card_zaptel_postregistration,
		.card_hooksig	= FXO_card_hooksig,
		.card_tick	= FXO_card_tick,
		.card_ioctl	= FXO_card_ioctl,
		.card_open	= FXO_card_open,

		.RING		= XPROTO_CALLER(FXO, RING),
		.RELAY_OUT	= XPROTO_CALLER(FXO, RELAY_OUT),
		.XPD_STATE	= XPROTO_CALLER(FXO, XPD_STATE),
	},
	.packet_is_valid = fxo_packet_is_valid,
	.packet_dump = fxo_packet_dump,
};

static bool fxo_packet_is_valid(xpacket_t *pack)
{
	const xproto_entry_t	*xe;

	//DBG("\n");
	xe = xproto_card_entry(&PROTO_TABLE(FXO), pack->opcode);
	return xe != NULL;
}

static void fxo_packet_dump(const char *msg, xpacket_t *pack)
{
	DBG("%s\n", msg);
}

/*------------------------- DAA Handling --------------------------*/

static int proc_fxo_info_read(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	int			len = 0;
	unsigned long		flags;
	xpd_t			*xpd = data;
	struct FXO_priv_data	*priv;
	int			i;

	if(!xpd)
		return -ENODEV;
	spin_lock_irqsave(&xpd->lock, flags);
	priv = xpd->priv;
	BUG_ON(!priv);
	len += sprintf(page + len, "\t%-17s: ", "Channel");
	for_each_line(xpd, i) {
		if(!IS_SET(xpd->digital_outputs, i) && !IS_SET(xpd->digital_inputs, i))
			len += sprintf(page + len, "%2d ", i % 10);
	}
	len += sprintf(page + len, "\n\t%-17s: ", "ledstate");
	for_each_line(xpd, i) {
		if(!IS_SET(xpd->digital_outputs, i) && !IS_SET(xpd->digital_inputs, i))
			len += sprintf(page + len, "%2d ", IS_SET(priv->ledstate[LED_GREEN], i));
	}
	len += sprintf(page + len, "\n\t%-17s: ", "blinking");
	for_each_line(xpd, i) {
		if(!IS_SET(xpd->digital_outputs, i) && !IS_SET(xpd->digital_inputs, i))
			len += sprintf(page + len, "%2d ", IS_BLINKING(priv,i,LED_GREEN));
	}
	len += sprintf(page + len, "\n\t%-17s: ", "battery");
	for_each_line(xpd, i) {
		len += sprintf(page + len, "%2d ", IS_SET(priv->battery, i));
	}
	len += sprintf(page + len, "\n\t%-17s: ", "polarity");
	for_each_line(xpd, i) {
		len += sprintf(page + len, "%2d ", IS_SET(priv->polarity, i));
	}
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
	unsigned		data_low = 0;
	char			op;		/* [W]rite, [R]ead */
	char			reg_type;	/* [D]irect */
	int			reg_num;
	int			elements;
	bool			writing;
	char			*p;
	reg_cmd_t		regcmd;
	xbus_t			*xbus;
	int			ret;

	if((p = strchr(cmdline, '#')) != NULL)	/* Truncate comments */
		*p = '\0';
	if((p = strchr(cmdline, ';')) != NULL)	/* Truncate comments */
		*p = '\0';
	for(p = cmdline; *p && (*p == ' ' || *p == '\t'); p++) /* Trim leading whitespace */
		;
	if(*p == '\0')
		return 0;

	elements = sscanf(cmdline, "%d %c%c %x %x",
			&chipsel,
			&op, &reg_type, &reg_num,
			&data_low);
	// DBG("'%s': %d %c%c %02X %02X %02X\n", cmdline, chipsel, op, reg_type, reg_num, data_low);
	if(elements < 4) {	// At least: chipsel, op, reg_type, reg_num
		ERR("Not enough arguments: (%d args) '%s'\n", elements, cmdline);
		return -EINVAL;
	}
	if(!VALID_CHIPSEL(chipsel)) {
		ERR("Bad chipsel number: %d\n", chipsel);
		return -EINVAL;
	}
	REG_FIELD(&regcmd, chipsel) = chipsel;
	REG_FIELD(&regcmd, do_subreg) = 0;
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
	switch(reg_type) {
		case 'D':
			REG_FIELD(&regcmd, regnum) = reg_num;
			REG_FIELD(&regcmd, subreg) = 0;
			break;
		default:
			ERR("Unkown register type '%c'\n", reg_type);
			return -EINVAL;
	}
	if(
			(op == 'W' && reg_type == 'D' && elements != 5) ||
			(op == 'R' && reg_type == 'D' && elements != 4)
	  ) {
		ERR("%s: '%s' (%d elements): %d %c%c %02X %02X\n", __FUNCTION__,
				cmdline, elements,
				chipsel, op, reg_type, reg_num, data_low);
		return -EINVAL;
	}
	regcmd.bytes = sizeof(regcmd) - 1;
	REG_FIELD(&regcmd, data_low) = data_low;
	REG_FIELD(&regcmd, data_high) = 0;
	REG_FIELD(&regcmd, read_request) = writing;
	BUG_ON(!xpd);
	xbus = xpd->xbus;
	if(!down_read_trylock(&xbus->in_use)) {
		DBG("Dropped packet. %s is in_use\n", xbus->busname);
		return -EBUSY;
	}
	xpd->requested_reply = regcmd;
	if(print_dbg)
		dump_reg_cmd("FXO", &regcmd);
	ret = DAA_DIRECT_REQUEST(xpd->xbus, xpd, REG_FIELD(&regcmd, chipsel), writing, REG_FIELD(&regcmd, regnum), REG_FIELD(&regcmd, data_low));
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
	byte			regnum;

	if(!xpd)
		return -ENODEV;
	spin_lock_irqsave(&xpd->lock, flags);
	info = &xpd->last_reply;
	regnum = REG_FIELD(info, regnum);
	len += sprintf(page + len, "# Writing bad data into this file may damage your hardware!\n");
	len += sprintf(page + len, "# Consult firmware docs first\n");
	len += sprintf(page + len, "#\n");
	len += sprintf(page + len, "#CH\tD/I\tReg.\tDL\n");
	len += sprintf(page + len, "%2d\tRD\t%02X\t%02X\n",
			REG_FIELD(info, chipsel),
			regnum, REG_FIELD(info, data_low));
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

int __init card_fxo_startup(void)
{
	if(ring_debounce <= 0) {
		ERR("%s: ring_debounce=%d. Must be positive number of ticks\n",
				THIS_MODULE->name, ring_debounce);
		return -EINVAL;
	}
	INFO("%s revision %s\n", THIS_MODULE->name, XPP_VERSION);
	xproto_register(&PROTO_TABLE(FXO));
	return 0;
}

void __exit card_fxo_cleanup(void)
{
	xproto_unregister(&PROTO_TABLE(FXO));
}

MODULE_DESCRIPTION("XPP FXO Card Driver");
MODULE_AUTHOR("Oron Peled <oron@actcom.co.il>");
MODULE_LICENSE("GPL");
MODULE_VERSION(XPP_VERSION);
MODULE_ALIAS_XPD(XPD_TYPE_FXO);

module_init(card_fxo_startup);
module_exit(card_fxo_cleanup);
