/*
 * Written by Oron Peled <oron@actcom.co.il>
 * Copyright (C) 2004, Xorcom
 *
 * Derived from ztdummy
 *
 * Copyright (C) 2002, Hermes Softlab
 * Copyright (C) 2004, Digium, Inc.
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
#include <linux/version.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
#  warning "This module is tested only with 2.6 kernels"
#endif

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/init.h>
#include <linux/delay.h>	/* for udelay */
#include <linux/interrupt.h>
#include <linux/proc_fs.h>
#include <zaptel.h>
#include "xbus-core.h"
#include "xproto.h"
#include "xpp_zap.h"
#include "parport_debug.h"

static const char rcsid[] = "$Id: xpp_zap.c 2536 2007-05-17 21:47:40Z tzafrir $";

#ifdef CONFIG_PROC_FS
struct proc_dir_entry *xpp_proc_toplevel = NULL;
#define	PROC_DIR		"xpp"
#define	PROC_SYNC		"sync"
#define	PROC_XPD_ZTREGISTER	"zt_registration"
#define	PROC_XPD_BLINK		"blink"
#define	PROC_XPD_SUMMARY	"summary"
#endif

#define	MAX_QUEUE_LEN		10000
#define	SAMPLE_TICKS		10000
#define	DELAY_UNTIL_DIALTONE	3000

static struct timer_list	xpp_timer;
static xpd_t			*sync_master = NULL;	// Start with host based sync
static unsigned int		xpp_timer_count = 0;
static unsigned int		xpp_last_jiffies = 0;

#ifdef	ZAPTEL_SYNC_TICK
static unsigned int            zaptel_tick_count = 0;
/*
 * Statistics variables
 */
static struct timeval		ticked_zaptel;
static struct timeval		ticked_xpp;
static long			usec_lag_curr;	/* current: zaptel - xpp */
static long			sigma_lag;	/* sum of lags over SYNC_ADJ_INTERVAL */
static long			average_lag;	/* average of lags over SYNC_ADJ_INTERVAL */
static bool			zaptel_is_ticking;
static bool			have_sync_mastership;
static unsigned int		sync_tick_nomaster;

#define	ZAPTEL_BIG_LAG		2000	/* usec */

#define	SYNC_ADJ_MIN            (-30)	/* minimal firmware drift unit */
#define	SYNC_ADJ_MAX            30	/* maximal firmware drift unit */
#define	SYNC_ADJ_INTERVAL       5000	/* minimum interval between fixes (usec) */
#define	SYNC_ADJ_FACTOR         30	/* sigma usec/drift_unit */

DEF_PARM_BOOL(sync_tick_active, 1, 0644, "Measure via zaptel sync_tick() method");

#endif


DEF_PARM_BOOL(pcm_tasklet, 0, 0644, "Handle PCM in a tasklet (lower interrupt load)");
DEF_PARM(int, disable_pcm, 0, 0644, "Disable all PCM transmissions");
DEF_PARM(int, print_dbg, 0, 0644, "Print DBG statements");
DEF_PARM_BOOL(zap_autoreg, 0, 0644, "Register spans automatically (1) or not (0)");
DEF_PARM_BOOL(prefmaster, 1, 0644, "Do we want to be zaptel preferred sync master");
#ifdef	XPP_EC_CHUNK
DEF_PARM_BOOL(xpp_ec, 0, 0444, "Do we use our own (1) or Zaptel's (0) echo canceller");
#else
static int xpp_ec = 0;
#endif
// DEF_ARRAY(int, pcmtx, 4, 0, "Forced PCM values to transmit");

#include "zap_debug.h"
#ifdef	XPP_EC_CHUNK
#include "supress/ec_xpp.h"
#endif

#ifdef	DEBUG_SYNC_PARPORT
/*
 * Use parallel port to sample our PCM sync and diagnose quality and
 * potential problems. A logic analizer or a scope should be connected
 * to the data bits of the parallel port.
 *
 * Array parameter: Choose the two xbuses Id's to sample.
 *                  This can be changed on runtime as well. Example:
 *                    echo "3,5" > /sys/module/xpp/parameters/parport_xbuses
 */
static int parport_xbuses[2] = { 0, 1 };
unsigned int parport_xbuses_num_values;
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,9)
module_param_array(parport_xbuses, int, &parport_xbuses_num_values, 0577);
#else
module_param_array(parport_xbuses, int, parport_xbuses_num_values, 0577);
#endif
MODULE_PARM_DESC(parport_xbuses, "Id's of xbuses to sample (1-2)");

/*
 * Flip a single bit in the parallel port:
 *   - The bit number is either bitnum0 or bitnum1
 *   - Bit is selected by xbus number from parport_xbuses[]
 */
void xbus_flip_bit(xbus_t *xbus, unsigned int bitnum0, unsigned int bitnum1)
{
	int		num = xbus->num;

	if(num == parport_xbuses[0])
		flip_parport_bit(bitnum0);
	if(num == parport_xbuses[1])
		flip_parport_bit(bitnum1);
}
EXPORT_SYMBOL(xbus_flip_bit);
#endif

static void xpp_tick(unsigned long param);
static int zaptel_register_xpd(xpd_t *xpd);
static int zaptel_unregister_xpd(xpd_t *xpd);
static void xpp_transmitprep(xbus_t *xbus, int unit, xpp_line_t lines, xpacket_t *pack);
static void xpp_receiveprep(xpd_t *xpd);
static int xpd_read_proc(char *page, char **start, off_t off, int count, int *eof, void *data);
static int proc_xpd_ztregister_read(char *page, char **start, off_t off, int count, int *eof, void *data);
static int proc_xpd_ztregister_write(struct file *file, const char __user *buffer, unsigned long count, void *data);
static int proc_xpd_blink_read(char *page, char **start, off_t off, int count, int *eof, void *data);
static int proc_xpd_blink_write(struct file *file, const char __user *buffer, unsigned long count, void *data);
static void xpd_free(xpd_t *xpd);
static DECLARE_TASKLET(tasklet_tick, xpp_tick, 0L);
static atomic_t		missed_ticks = ATOMIC_INIT(0);	/* In pcm_tasklet mode */

static void external_sync(xbus_t *the_xbus)
{
	int	i;

	DBG("%s\n", (the_xbus) ? the_xbus->busname : "HOST");
	// Shut all down
	for(i = 0; i < MAX_BUSES; i++) {
		xbus_t	*xbus = xbus_of(i);
		if(!xbus)
			continue;
		if (!xbus->hardware_exists)
			continue;
		CALL_PROTO(GLOBAL, SYNC_SOURCE, xbus, NULL, SYNC_MODE_HOST, 0);
	}
	if(the_xbus)
		CALL_PROTO(GLOBAL, SYNC_SOURCE, the_xbus, NULL, SYNC_MODE_AB, 0);
}

/*
 * Change sync_master. May block. Cannot be called from atomic context
 */
static void sync_master_is(xbus_t *xbus)
{
	xpd_t	*xpd = (xbus)? xpd_of(xbus, 0) : NULL;

	if(xbus && !xpd) {
		NOTICE("Cannot set sync master to %s (has no XPD #0).\n",
				xbus->busname);
		xbus = NULL;	/* Fallback to HOST */
	}
	DBG("SYNC MASTER CHANGING: %s => %s\n",
			(sync_master) ? sync_master->xbus->busname : "HOST",
			(xbus) ? xbus->busname : "HOST");
	/* First stop all generators */
	if(sync_master) {
		tasklet_kill(&tasklet_tick);
	} else
		del_timer_sync(&xpp_timer);
	/* Now set a new master */
	sync_master = xpd;
	if(sync_master) {	// XPD
		external_sync(xbus);
	} else {		// HOST
		external_sync(NULL);
		if(!timer_pending(&xpp_timer)) {
			xpp_timer.function = xpp_tick;
			xpp_timer.data = 0;
			xpp_timer.expires = jiffies + 1;	/* Must be 1KHz rate */
			add_timer(&xpp_timer);
		}
	}
}

static inline void send_pcm_frame(xbus_t *xbus, xframe_t *xframe)
{
	static int	rate_limit;
	unsigned long	flags;
	struct timeval	now;
	unsigned long	sec_diff;
	unsigned long	usec_diff;

	spin_lock_irqsave(&xbus->lock, flags);
	do_gettimeofday(&now);
	if(unlikely(disable_pcm || !xbus->hardware_exists))
		goto dropit;
	sec_diff = now.tv_sec - xbus->last_tx_sync.tv_sec;
	usec_diff = sec_diff * 1000000 + (now.tv_usec - xbus->last_tx_sync.tv_usec);
	if(unlikely(abs(sec_diff) > 2)) {
		DBG("%s: PCM TX timing restart (sec_diff=%ld)\n",
					xbus->busname, sec_diff);
		xbus->last_tx_sync = now;
		goto dropit;
	}
	if(abs(usec_diff - 1000) > TICK_TOLERANCE) {
		if(print_dbg && printk_ratelimit())
			DBG("%s: Bad PCM TX timing: usec_diff=%ld.\n",
					xbus->busname, usec_diff);
	}
	if(usec_diff > xbus->max_tx_sync)
		xbus->max_tx_sync = usec_diff;
	if(usec_diff < xbus->min_tx_sync)
		xbus->min_tx_sync = usec_diff;
	spin_unlock_irqrestore(&xbus->lock, flags);
	/* OK, really send it */
	if(print_dbg == 2 && ((rate_limit++ % 1003) == 0))
		dump_xframe("SEND_PCM", xbus, xframe);
	xframe_send(xbus, xframe);
	XBUS_COUNTER(xbus, SEND_PCM)++;
	xbus->last_tx_sync = now;
	return;
dropit:
	spin_unlock_irqrestore(&xbus->lock, flags);
	xbus->ops->xframe_free(xbus, xframe);
}

/*
 * Calculate PCM line_count and lines from a xbus+unit number:
 *   - Takes into account BRI subunits
 *   - Returns an XPD pointer if we should transmit, NULL otherwise
 *   - Calls zt_transmit() for all registered subunits so we empty
 *     the relevant zaptel buffers (otherwise asterisk fails to write)
 *     and we have stable buffers to send.
 */
static xpd_t *unit_pcm_calc(const xbus_t *xbus, int unit, size_t *pcm_len, xpp_line_t *plines)
{
	int		line_count = 0;
	xpp_line_t	lines;
	xpd_t		*xpd = NULL;
	xpd_t		*tmp_xpd;
	unsigned long	flags;
	int		i;
	int		subunit;

	lines = 0;
	line_count = 0;
	/*
	 * BRI has a single PCM highway for all subunits, so
	 * we agregate the next subunits into the same
	 * transmition. We shift 4 bits (B1, B2, D, E)
	 */
	for(subunit = 0; subunit < MAX_SUBUNIT; subunit++) {
		xpp_line_t	tmp_lines;

		tmp_xpd = xpd_by_addr(xbus, unit, subunit);
		if(!tmp_xpd || !tmp_xpd->card_present)
			continue;
		if(!xpd)
			xpd = tmp_xpd;	/* First xpd for our unit */
		if(SPAN_REGISTERED(tmp_xpd)) {
			/*
			 * calls to zt_transmit should be out of spinlocks, as it may call back
			 * our hook setting methods.
			 */
			zt_transmit(&tmp_xpd->span);
		}
		spin_lock_irqsave(&tmp_xpd->lock, flags);
		tmp_lines = (tmp_xpd->offhook | tmp_xpd->cid_on) & ~tmp_xpd->digital_signalling;	/* without D-Channel */
		lines |= (tmp_lines << (SUBUNIT_PCM_SHIFT * subunit));
		for_each_line(tmp_xpd, i)
			if(IS_SET(tmp_lines, i))
				line_count++;
		spin_unlock_irqrestore(&tmp_xpd->lock, flags);
		if(!IS_BRI(tmp_xpd))
			break;	/* no subunits */
	}
	/*
	 * FIXME: Workaround a bug in sync code of the Astribank.
	 *        Send dummy PCM for sync.
	 */
	if(unit == 0 && lines == 0) {
		lines = BIT(0);
		line_count = 1;
	}
	if(!line_count)
		return NULL;	/* Nothing to transmit for this unit */
	*pcm_len = RPACKET_HEADERSIZE + sizeof(xpp_line_t)  +  line_count * ZT_CHUNKSIZE;
	*plines = lines;
	return xpd;
}

static void xbus_tick(xbus_t *xbus)
{
	int		unit;
	int		i;
	xpp_line_t	lines;
	xpd_t		*xpd;
	xframe_t	*xframe = NULL;
	xpacket_t	*pack = NULL;
	size_t		pcm_len;

	for(unit = 0; unit < MAX_UNIT; unit++) {
		if((xpd = unit_pcm_calc(xbus, unit, &pcm_len, &lines)) == NULL)
			continue;
		if(lines && SPAN_REGISTERED(xpd)) {
			do {
				// pack = NULL;		/* FORCE single packet frames */
				if(xframe && !pack) {	/* FULL frame */
					send_pcm_frame(xbus, xframe);
					xframe = NULL;
				}
				if(!xframe) {		/* Alloc frame */
					xframe = xbus->ops->xframe_new(xbus, GFP_ATOMIC);
					if (!xframe) {
						ERR("%s: failed to allocate new xframe\n", __FUNCTION__);
						return;
					}
				}
				pack = xframe_next_packet(xframe, pcm_len);
			} while(!pack);
			XPACKET_INIT(pack, GLOBAL, PCM_WRITE);
			xpd_set_addr(&pack->addr, xpd->id);
			pack->datalen = pcm_len;
			RPACKET_FIELD(pack, GLOBAL, PCM_WRITE, lines) = lines;
			xpp_transmitprep(xbus, unit, lines, pack);
		}
	}
	if(xframe)	/* clean any leftovers */
		send_pcm_frame(xbus, xframe);
	for(i = 0; i < MAX_XPDS; i++) {
		xpd = xpd_of(xbus, i);
		if(!xpd || !xpd->card_present)
			continue;
		if(SPAN_REGISTERED(xpd))
			xpp_receiveprep(xpd);
		xpd->timer_count++;
		/*
		 * Must be called *after* tx/rx so
		 * D-Chan counters may be cleared
		 */
		CALL_XMETHOD(card_tick, xbus, xpd);
	}
}

void xpp_tick(unsigned long param)
{
	xbus_t		*xbus;
	int		i;

#ifdef	ZAPTEL_SYNC_TICK
	flip_parport_bit(3);
	do_gettimeofday(&ticked_xpp);
#endif
	if(!sync_master)			/* Called from timer */
		mod_timer(&xpp_timer, jiffies + 1);	/* Must be 1KHz rate */
	else
		atomic_dec(&missed_ticks);
	/* Statistics */
	if((xpp_timer_count % SAMPLE_TICKS) == 0) {
		xpp_last_jiffies = jiffies;
	}
	xpp_timer_count++;
	for(i = 0; i < MAX_BUSES; i++) {
		xbus = xbus_of(i);
		if(!xbus)
			continue;
		if (!xbus->hardware_exists)
			continue;
		if(!down_read_trylock(&xbus->in_use)) {
			DBG("Dropped packet. %s is in_use\n", xbus->busname);
			continue;
		}
		xbus_tick(xbus);
		if((xpp_timer_count % (60*1000)) == 0) {
			xbus->min_tx_sync = INT_MAX;
			xbus->max_tx_sync = 0;
			xbus->min_rx_sync = INT_MAX;
			xbus->max_rx_sync = 0;
		}
		up_read(&xbus->in_use);
	}
}

void got_sync_from(xpd_t *xpd)
{
	xbus_t		*xbus;

	xbus = xpd->xbus;
	if(xpd != sync_master) {
		static int rate_limit;

		if((rate_limit++ % 1003) == 0)
			DBG("%s/%s: is not SYNC master. Go away! (%d)\n",
					xbus->busname, xpd->xpdname, rate_limit);
		return;
	}
	atomic_inc(&missed_ticks);
	if(!pcm_tasklet) {
		xpp_tick(0L);
		return;
	}
	tasklet_schedule(&tasklet_tick);
}

#ifdef	ZAPTEL_SYNC_TICK
static void send_drift(int drift)
{
	struct timeval          now;
	const char              *msg;
	xbus_t			*xbus;

	BUG_ON(!sync_master);
	xbus = sync_master->xbus;
	BUG_ON(drift < SYNC_ADJ_MIN || drift > SYNC_ADJ_MAX);
	do_gettimeofday(&now);
	if(drift == 0)
		msg = "stop";
	else if(drift > 0)
		msg = "up";
	else
		msg = "down";
	DBG("DRIFT adjust %s (%d) (last update %ld seconds ago)\n",
		msg, drift, now.tv_sec - xbus->pll_updated_at);
	CALL_PROTO(GLOBAL, SYNC_SOURCE, xbus, NULL, SYNC_MODE_PLL, drift);
	xbus->pll_updated_at = now.tv_sec;
}

int zaptel_sync_tick(struct zt_span *span, int is_master)
{
	xpd_t		*xpd = span->pvt;
	int		offset;

	if(!sync_tick_active)
		goto noop;
	/*
	 * Detect if any of our spans is zaptel sync master
	 */
	if(is_master) {
		sync_tick_nomaster = 0;		/* Yes */
		have_sync_mastership = 1;
	}
	if(sync_tick_nomaster < MAX_BUSES * MAX_XPDS) {
		sync_tick_nomaster++;	/* Maybe */
		goto noop;
	}
	/* Now we know for sure someone else is zaptel sync master */
	have_sync_mastership = 0;
	BUG_ON(!xpd);
	/*
	 * Calculate only if:
	 *   - HOST sync (for information only)
	 *   - Called for the sync_master (the function is called
	 *     for each span, so we don't want to do it multiple times).
	 */
	if(sync_master && sync_master != xpd)
		return 0;
	zaptel_tick_count++;
	zaptel_is_ticking = 1;
	flip_parport_bit(1);
	do_gettimeofday(&ticked_zaptel);
	usec_lag_curr =	
		(ticked_zaptel.tv_sec - ticked_xpp.tv_sec)*1000*1000 +
		(ticked_zaptel.tv_usec - ticked_xpp.tv_usec);
	if(unlikely(usec_lag_curr > ZAPTEL_BIG_LAG)) {
		static int	rate_limit;

		if((rate_limit++ % 1003) == 0)
			DBG("Big lag of Xpp ticks relative to zaptel ticks: %ld\n", usec_lag_curr);
	}
	sigma_lag += usec_lag_curr;
	if((zaptel_tick_count % SYNC_ADJ_INTERVAL) == 0) {
		average_lag = sigma_lag / SYNC_ADJ_INTERVAL;
		sigma_lag = 0;
		if(sync_master == xpd) {
			xbus_t	*xbus = sync_master->xbus;

			/* Calculate required fix */
			offset = (average_lag - 500) / SYNC_ADJ_FACTOR;
			if(offset < SYNC_ADJ_MIN)
				offset = SYNC_ADJ_MIN;
			if(offset > SYNC_ADJ_MAX)
				offset = SYNC_ADJ_MAX;
			if(xbus->sync_adjustment != offset)	/* Only fix if needed */
				send_drift(offset);
		}
	}
	return 0;
noop:
	zaptel_is_ticking = 0;
	sigma_lag = 0;
	return 0;	/* No auto sync from zaptel */
}
#endif

#if HZ != 1000
#warning "HZ != 1000. PCM would be good only with Astribank sync"
#endif

static void xpd_free(xpd_t *xpd)
{
	xbus_t	*xbus = NULL;

	if(!xpd)
		return;
	xbus = xpd->xbus;
	if(!xbus)
		return;
	DBG("%s/%s\n", xbus->busname, xpd->xpdname);
	xbus_unregister_xpd(xbus, xpd);
#ifdef CONFIG_PROC_FS
	if(xpd->proc_xpd_dir) {
		if(xpd->proc_xpd_summary) {
			DBG("Removing proc '%s' for %s/%s\n", PROC_XPD_SUMMARY, xbus->busname, xpd->xpdname);
			remove_proc_entry(PROC_XPD_SUMMARY, xpd->proc_xpd_dir);
			xpd->proc_xpd_summary = NULL;
		}
		if(xpd->proc_xpd_ztregister) {
			DBG("Removing proc '%s' for %s/%s\n", PROC_XPD_ZTREGISTER, xbus->busname, xpd->xpdname);
			remove_proc_entry(PROC_XPD_ZTREGISTER, xpd->proc_xpd_dir);
			xpd->proc_xpd_ztregister = NULL;
		}
		if(xpd->proc_xpd_blink) {
			DBG("Removing proc '%s' for %s/%s\n", PROC_XPD_BLINK, xbus->busname, xpd->xpdname);
			remove_proc_entry(PROC_XPD_BLINK, xpd->proc_xpd_dir);
			xpd->proc_xpd_blink = NULL;
		}
		DBG("Removing proc directory for %s/%s\n", xbus->busname, xpd->xpdname);
		remove_proc_entry(xpd->xpdname, xbus->proc_xbus_dir);
		xpd->proc_xpd_dir = NULL;
	}
#endif
	if(xpd->readchunk)
		kfree((void *)xpd->readchunk);
	xpd->readchunk = NULL;
	if(xpd->xproto)
		xproto_put(xpd->xproto);
	xpd->xproto = NULL;
	kfree(xpd);
}


/*------------------------- XPD Management -------------------------*/

#define	REV(x,y)	(10 * (x) + (y))
static byte good_revs[] = {
	REV(2,6),
};
#undef	REV

static bool good_rev(byte rev)
{
	int	i;

	for(i = 0; i < ARRAY_SIZE(good_revs); i++) {
		if(good_revs[i] == rev)
			return 1;
	}
	return 0;
}

/*
 * Synchronous part of XPD detection.
 * Called from xbus_poll()
 */
void card_detected(struct card_desc_struct *card_desc)
{
	xbus_t			*xbus;
	xpd_t			*xpd = NULL;
	int			xpd_num;
	byte			type;
	byte			rev;
	const xops_t		*xops;
	const xproto_table_t	*proto_table;


	BUG_ON(!card_desc);
	BUG_ON(card_desc->magic != CARD_DESC_MAGIC);
	xbus = card_desc->xbus;
	xpd_num = XPD_NUM(card_desc->xpd_addr);
	type = card_desc->type;
	rev = card_desc->rev;
	BUG_ON(!xbus);
	if(!good_rev(rev)) {
		NOTICE("%s: New XPD #%d (%d-%d) type=%d has bad firmware revision %d.%d\n", xbus->busname,
			xpd_num, card_desc->xpd_addr.unit, card_desc->xpd_addr.subunit,
			type, rev / 10, rev % 10);
		goto err;
	}
	INFO("%s: New XPD #%d (%d-%d) type=%d Revision %d.%d\n", xbus->busname,
			xpd_num, card_desc->xpd_addr.unit, card_desc->xpd_addr.subunit,
			type, rev / 10, rev % 10);
	xpd = xpd_of(xbus, xpd_num);
	if(xpd) {
		if(type == XPD_TYPE_NOMODULE) {
			NOTICE("%s: xpd #%d: removed\n", __FUNCTION__, xpd_num);
			BUG();
			goto out;
		}
		NOTICE("%s: xpd #%d: already exists\n", __FUNCTION__, xpd_num);
		goto out;
	}
	if(type == XPD_TYPE_NOMODULE) {
		DBG("No module at address=%d\n", xpd_num);
		goto out;
	}
	proto_table = xproto_get(type);
	if(!proto_table) {
		NOTICE("%s: xpd #%d: missing protocol table for type=%d. Ignored.\n", __FUNCTION__, xpd_num, type);
		goto out;
	}
	xops = &proto_table->xops;
	BUG_ON(!xops);
	xpd = xops->card_new(xbus, xpd_num, proto_table, rev);
	if(!xpd) {
		NOTICE("card_new(%s,%d,%d,%d) failed. Ignored.\n", xbus->busname, xpd_num, proto_table->type, rev);
		goto err;
	}
	xpd->addr = card_desc->xpd_addr;
	xpd->offhook = card_desc->line_status;

	/* For USB-1 disable some channels */
	if(xbus->max_packet_size < RPACKET_SIZE(GLOBAL, PCM_WRITE)) {
		xpp_line_t	no_pcm;

		no_pcm = 0x7F | xpd->digital_outputs | xpd->digital_inputs;
		xpd->no_pcm = no_pcm;
		NOTICE("%s: max packet size = %d, disabling some PCM channels. no_pcm=0x%04X\n",
				xbus->busname, xbus->max_packet_size, xpd->no_pcm);
	}
#ifdef	CONFIG_PROC_FS
	DBG("Creating xpd proc directory for %s/%s\n", xbus->busname, xpd->xpdname);
	xpd->proc_xpd_dir = proc_mkdir(xpd->xpdname, xbus->proc_xbus_dir);
	if(!xpd->proc_xpd_dir) {
		ERR("Failed to create proc directory for %s/%s\n", xbus->busname, xpd->xpdname);
		goto err;
	}
	xpd->proc_xpd_summary = create_proc_read_entry(PROC_XPD_SUMMARY, 0444, xpd->proc_xpd_dir,
			xpd_read_proc, xpd);
	if(!xpd->proc_xpd_summary) {
		ERR("Failed to create proc '%s' for %s/%s\n", PROC_XPD_SUMMARY, xbus->busname, xpd->xpdname);
		goto err;
	}
	xpd->proc_xpd_summary->owner = THIS_MODULE;
	xpd->proc_xpd_ztregister = create_proc_entry(PROC_XPD_ZTREGISTER, 0644, xpd->proc_xpd_dir);
	if (!xpd->proc_xpd_ztregister) {
		ERR("Failed to create proc '%s' for %s/%s\n", PROC_XPD_ZTREGISTER, xbus->busname, xpd->xpdname);
		goto err;
	}
	xpd->proc_xpd_ztregister->owner = THIS_MODULE;
	xpd->proc_xpd_ztregister->data = xpd;
	xpd->proc_xpd_ztregister->read_proc = proc_xpd_ztregister_read;
	xpd->proc_xpd_ztregister->write_proc = proc_xpd_ztregister_write;
	xpd->proc_xpd_blink = create_proc_entry(PROC_XPD_BLINK, 0644, xpd->proc_xpd_dir);
	if (!xpd->proc_xpd_blink) {
		ERR("Failed to create proc '%s' for %s/%s\n", PROC_XPD_BLINK, xbus->busname, xpd->xpdname);
		goto err;
	}
	xpd->proc_xpd_blink->owner = THIS_MODULE;
	xpd->proc_xpd_blink->data = xpd;
	xpd->proc_xpd_blink->read_proc = proc_xpd_blink_read;
	xpd->proc_xpd_blink->write_proc = proc_xpd_blink_write;
#endif
	xbus_register_xpd(xbus, xpd);
	if(CALL_XMETHOD(card_init, xbus, xpd) < 0)
		goto err;
	CALL_XMETHOD(XPD_STATE, xbus, xpd, 0);				/* Turn off all channels */
	CALL_PROTO(GLOBAL, SYNC_SOURCE, xbus, NULL, SYNC_MODE_HOST, 0);	/* Not a SYNC master (yet) */
	xpd->card_present = 1;
	if(xbus->last_tx_sync.tv_sec == 0 && xbus->last_tx_sync.tv_usec == 0) {
		do_gettimeofday(&xbus->last_tx_sync);			/* start timing ticks */
		xbus->last_rx_sync = xbus->last_tx_sync;
	}
	CALL_XMETHOD(XPD_STATE, xbus, xpd, 1);				/* Turn on all channels */

	if(zap_autoreg)
		zaptel_register_xpd(xpd);
out:
	memset(card_desc, 0, sizeof(struct card_desc_struct));
	kfree(card_desc);
	return;
err:
	xpd_free(xpd);
	goto out;
}


#ifdef CONFIG_PROC_FS

/**
 * Prints a general procfs entry for the bus, under xpp/BUSNAME/summary
 * @page TODO: figure out procfs
 * @start TODO: figure out procfs
 * @off TODO: figure out procfs
 * @count TODO: figure out procfs
 * @eof TODO: figure out procfs
 * @data an xbus_t pointer with the bus data.
 */
static int xpd_read_proc(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	int		len = 0;
	xpd_t		*xpd = data;
	xbus_t		*xbus;
	int		i;

	if(!xpd)
		goto out;

	xbus = xpd->xbus;
	len += sprintf(page + len, "%s (%s ,card %s, span %s) %s\n"
			"timer_count: %d span->mainttimer=%d\n"
			,
			xpd->xpdname, xproto_name(xpd->type),
			(xpd->card_present) ? "present" : "missing",
			(SPAN_REGISTERED(xpd)) ? "registered" : "NOT registered",
			(xpd == sync_master) ? "SYNC MASTER" : "SYNC SLAVE",
			xpd->timer_count, xpd->span.mainttimer
			);
	len += sprintf(page + len, "Address: U=%d S=%d\n\n", xpd->addr.unit, xpd->addr.subunit);
	len += sprintf(page + len, "STATES:");
	len += sprintf(page + len, "\n\t%-17s: ", "output_relays");
	for_each_line(xpd, i) {
		len += sprintf(page + len, "%d ", IS_SET(xpd->digital_outputs, i));
	}
	len += sprintf(page + len, "\n\t%-17s: ", "input_relays");
	for_each_line(xpd, i) {
		len += sprintf(page + len, "%d ", IS_SET(xpd->digital_inputs, i));
	}
	len += sprintf(page + len, "\n\t%-17s: ", "offhook");
	for_each_line(xpd, i) {
		len += sprintf(page + len, "%d ", IS_SET(xpd->offhook, i));
	}
	len += sprintf(page + len, "\n\t%-17s: ", "cid_on");
	for_each_line(xpd, i) {
		len += sprintf(page + len, "%d ", IS_SET(xpd->cid_on, i));
	}
	len += sprintf(page + len, "\n\t%-17s: ", "ringing");
	for_each_line(xpd, i) {
		len += sprintf(page + len, "%d ", xpd->ringing[i]);
	}
	len += sprintf(page + len, "\n\t%-17s: ", "no_pcm");
	for_each_line(xpd, i) {
		len += sprintf(page + len, "%d ", IS_SET(xpd->no_pcm, i));
	}
#if 1
	if(SPAN_REGISTERED(xpd)) {
		len += sprintf(page + len, "\nPCM:\n            |         [readchunk]       |         [writechunk]      | delay");
		for_each_line(xpd, i) {
			struct zt_chan	*chans = xpd->span.chans;
			byte	rchunk[ZT_CHUNKSIZE];
			byte	wchunk[ZT_CHUNKSIZE];
			byte	*rp;
			byte	*wp;
			int j;

			if(IS_SET(xpd->digital_outputs, i))
				continue;
			if(IS_SET(xpd->digital_inputs, i))
				continue;
			if(IS_SET(xpd->digital_signalling, i))
				continue;
#if 0
			rp = chans[i].readchunk;
#else
			rp = (byte *)xpd->readchunk + (ZT_CHUNKSIZE * i);
#endif
			wp = chans[i].writechunk;
			memcpy(rchunk, rp, ZT_CHUNKSIZE);
			memcpy(wchunk, wp, ZT_CHUNKSIZE);
			len += sprintf(page + len, "\n  port %2d>  |  ", i);
			for(j = 0; j < ZT_CHUNKSIZE; j++) {
				len += sprintf(page + len, "%02X ", rchunk[j]);
			}
			len += sprintf(page + len, " |  ");
			for(j = 0; j < ZT_CHUNKSIZE; j++) {
				len += sprintf(page + len, "%02X ", wchunk[j]);
			}
			len += sprintf(page + len, " | ");
		}
	}
#endif
#if 0
	if(SPAN_REGISTERED(xpd)) {
		len += sprintf(page + len, "\nSignalling:\n");
		for_each_line(xpd, i) {
			struct zt_chan *chan = &xpd->span.chans[i];
			len += sprintf(page + len, "\t%2d> sigcap=0x%04X sig=0x%04X\n", i, chan->sigcap, chan->sig);
		}
	}
#endif
	len += sprintf(page + len, "\nCOUNTERS:\n");
	for(i = 0; i < XPD_COUNTER_MAX; i++) {
		len += sprintf(page + len, "\t\t%-20s = %d\n",
				xpd_counters[i].name, xpd->counters[i]);
	}
	len += sprintf(page + len, "<-- len=%d\n", len);
out:
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

#endif

/*
 * xpd_alloc - Allocator for new XPD's
 *
 */
xpd_t *xpd_alloc(size_t privsize, xbus_t *xbus, int xpd_num, const xproto_table_t *proto_table, int channels, byte revision)
{
	xpd_t		*xpd = NULL;
	size_t		alloc_size = sizeof(xpd_t) + privsize;

	DBG("%s: xpd #%d\n", xbus->busname, xpd_num);
	if(!VALID_XPD_NUM(xpd_num)) {
		ERR("%s: illegal xpd id = %d\n", __FUNCTION__, xpd_num);
		goto err;
	}
	if(channels > CHANNELS_PERXPD) {
		ERR("%s: too many channels %d for xpd #%d\n", __FUNCTION__, channels, xpd_num);
		goto err;
	}

	if((xpd = kmalloc(alloc_size, GFP_KERNEL)) == NULL) {
		ERR("%s: Unable to allocate memory for xpd #%d\n", __FUNCTION__, xpd_num);
		goto err;
	}
	memset(xpd, 0, alloc_size);
	xpd->priv = (byte *)xpd + sizeof(xpd_t);

	spin_lock_init(&xpd->lock);
	xpd->xbus = xbus;
	xpd->id = xpd_num;
	xpd->channels = channels;
	xpd->chans = NULL;
	xpd->card_present = 0;
	snprintf(xpd->xpdname, XPD_NAMELEN, "XPD-%02x", xpd_num);
	xpd->offhook = 0x0;	/* ONHOOK */
	xpd->type = proto_table->type;
	xpd->xproto = proto_table;
	xpd->xops = &proto_table->xops;
	xpd->digital_outputs = 0;
	xpd->digital_inputs = 0;

	atomic_set(&xpd->zt_registered, 0);
	atomic_set(&xpd->open_counter, 0);

	xpd->chans = kmalloc(sizeof(struct zt_chan)*xpd->channels, GFP_KERNEL);
	if (xpd->chans == NULL) {
		ERR("%s: Unable to allocate channels\n", __FUNCTION__);
		goto err;
	}
	alloc_size = ZT_MAX_CHUNKSIZE * CHANNELS_PERXPD * 2;	/* Double Buffer */
	if((xpd->readchunk = kmalloc(alloc_size, GFP_KERNEL)) == NULL) {
		ERR("%s: Unable to allocate memory for readchunk\n", __FUNCTION__);
		goto err;
	}
	/* Initialize read buffers to all blank data */
	memset((void *)xpd->readchunk, 0, alloc_size);
	return xpd;
err:
	if(xpd) {
		if(xpd->chans)
			kfree((void *)xpd->chans);
		if(xpd->readchunk)
			kfree((void *)xpd->readchunk);
		kfree(xpd);
	}
	return NULL;
}

/* FIXME: this should be removed once digium patch their zaptel.h
 * I simply wish to avoid changing zaptel.h in the xpp patches.
 */
#ifndef ZT_EVENT_REMOVED
#define ZT_EVENT_REMOVED (20)
#endif

void xpd_disconnect(xpd_t *xpd)
{
	unsigned long	flags;

	BUG_ON(!xpd);

	// TODO: elect a new sync master
	if(sync_master == xpd)
		sync_master_is(NULL);

	spin_lock_irqsave(&xpd->lock, flags);
	DBG("%s/%s (%p)\n", xpd->xbus->busname, xpd->xpdname, xpd->xproto);
	if(!xpd->card_present)	/* Multiple reports */
		goto out;
	xpd->card_present = 0;
	if(SPAN_REGISTERED(xpd)) {
		int i;

		update_xpd_status(xpd, ZT_ALARM_NOTOPEN);
		/* TODO: Should this be done before releasing the spinlock? */
		DBG("Queuing ZT_EVENT_REMOVED on all channels to ask user to release them\n");
		for (i=0; i<xpd->span.channels; i++)
			zt_qevent_lock(&xpd->chans[i],ZT_EVENT_REMOVED);
	}
out:
	spin_unlock_irqrestore(&xpd->lock, flags);
}

void xpd_remove(xpd_t *xpd)
{
	xbus_t	*xbus;

	BUG_ON(!xpd);
	xbus = xpd->xbus;
	INFO("%s: Remove XPD #%d from\n", xbus->busname, xpd->id);

	zaptel_unregister_xpd(xpd);
	CALL_XMETHOD(card_remove, xbus, xpd);
	xpd_free(xpd);
}

void update_xpd_status(xpd_t *xpd, int alarm_flag)
{
	struct zt_span *span = &xpd->span;

	if(!SPAN_REGISTERED(xpd)) {
		// NOTICE("%s: %s is not registered. Skipping.\n", __FUNCTION__, xpd->xpdname);
		return;
	}
	switch (alarm_flag) {
		case ZT_ALARM_NONE:
			xpd->last_response = jiffies;
			break;
		default:
			// Nothing
			break;
	}
	if(span->alarms == alarm_flag)
		return;
	span->alarms = alarm_flag;
	zt_alarm_notify(span);
	DBG("Update XPD alarms: %s -> %02X\n", xpd->span.name, alarm_flag);
}

void update_line_status(xpd_t *xpd, int pos, bool to_offhook)
{
	zt_rxsig_t	rxsig;

	BUG_ON(!xpd);
	if(to_offhook) {
		BIT_SET(xpd->offhook, pos);
		rxsig = ZT_RXSIG_OFFHOOK;
	} else {
		BIT_CLR(xpd->offhook, pos);
		BIT_CLR(xpd->cid_on, pos);
		rxsig = ZT_RXSIG_ONHOOK;
	}
	/*
	 * We should not spinlock before calling zt_hooksig() as
	 * it may call back into our xpp_hooksig() and cause
	 * a nested spinlock scenario
	 */
	DBG("%s/%s/%d: rxsig=%s\n", xpd->xbus->busname, xpd->xpdname, pos,
			(rxsig == ZT_RXSIG_ONHOOK) ? "ONHOOK" : "OFFHOOK");
	if(SPAN_REGISTERED(xpd))
		zt_hooksig(&xpd->chans[pos], rxsig);
}

void update_zap_ring(xpd_t *xpd, int pos, bool on)
{
	zt_rxsig_t	rxsig;

	BUG_ON(!xpd);
	if(on) {
		BIT_CLR(xpd->cid_on, pos);
		rxsig = ZT_RXSIG_RING;
	} else {
		BIT_SET(xpd->cid_on, pos);
		rxsig = ZT_RXSIG_OFFHOOK;
	}
	/*
	 * We should not spinlock before calling zt_hooksig() as
	 * it may call back into our xpp_hooksig() and cause
	 * a nested spinlock scenario
	 */
	if(SPAN_REGISTERED(xpd))
		zt_hooksig(&xpd->chans[pos], rxsig);
}

#ifdef CONFIG_PROC_FS

int proc_sync_read(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	int		len = 0;
	unsigned int	xpp_timer_rate;
	unsigned int	jiffies_now;

	len += sprintf(page + len, "# To modify sync source write into this file:\n");
	len += sprintf(page + len, "#     HOST        - For host based sync\n");
	len += sprintf(page + len, "#     SYNC=nn     - XBUS-nn provide sync\n");
	len += sprintf(page + len, "#     QUERY=nn    - Query XBUS-nn for sync information (DEBUG)\n");
	if(!sync_master)
		len += sprintf(page + len, "HOST\n");
	else {
		len += sprintf(page + len, "SYNC=%02d\n", sync_master->xbus->num);
		len += sprintf(page + len, "PLL DRIFT=%d\n",
				sync_master->xbus->sync_adjustment);
	}
#ifdef	ZAPTEL_SYNC_TICK
	if(sync_tick_active) {
		len += sprintf(page + len, "\nZaptel Reference Sync (%s):\n",
				(have_sync_mastership)?"xpp":"external");
		if(zaptel_is_ticking) {
			if(sync_master) {
				struct timeval	now;
				xbus_t		*xbus = sync_master->xbus;

				do_gettimeofday(&now);
				len += sprintf(page + len, "\t%-19s: %5ld seconds ago\n",
						"PLL updated",
						(xbus->pll_updated_at == 0) ? 0 : now.tv_sec - xbus->pll_updated_at);
			}
			len += sprintf(page + len, "\t%-19s: %5ld (usec)\n",
					"current lag", usec_lag_curr);
			len += sprintf(page + len, "\t%-19s: %5ld (usec)\n",
					"average lag", average_lag);
		} else
			len += sprintf(page + len, "\tNot activated\n");
	}
	len += sprintf(page + len, "zaptel_tick: #%d\n", zaptel_tick_count);
	len += sprintf(page + len, "tick - zaptel_tick = %d\n", xpp_timer_count - zaptel_tick_count);
#endif
	len += sprintf(page + len, "\ntick: #%d\n", xpp_timer_count);
	xpp_timer_rate = 0;
	jiffies_now = jiffies;
	if(jiffies_now - xpp_last_jiffies > 0) {
		unsigned long delta = (jiffies_now - xpp_last_jiffies);
		xpp_timer_rate = (xpp_timer_count % SAMPLE_TICKS) * HZ / delta;
		len += sprintf(page + len, "tick rate: %4d/second (SAMPLE_TICKS=%d)\n", xpp_timer_rate, SAMPLE_TICKS);
	}
	if(pcm_tasklet)
		len += sprintf(page + len, "TASKLETS: missed_ticks: %d\n", atomic_read(&missed_ticks));
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

static int proc_sync_write(struct file *file, const char __user *buffer, unsigned long count, void *data)
{
	char		buf[MAX_PROC_WRITE];
	int		xbus_num;
	int		xpd_num;
	xbus_t		*xbus;

	// DBG("%s: count=%ld\n", __FUNCTION__, count);
	if(count >= MAX_PROC_WRITE)
		return -EINVAL;
	if(copy_from_user(buf, buffer, count))
		return -EFAULT;
	buf[count] = '\0';
	if(strncmp("HOST", buf, 4) == 0) {
		sync_master_is(NULL);
	} else if(sscanf(buf, "SYNC=%d", &xbus_num) == 1) {
		DBG("SYNC=%d\n", xbus_num);
		if((xbus = xbus_of(xbus_num)) == NULL) {
			ERR("No bus %d exists\n", xbus_num);
			return -ENXIO;
		}
		sync_master_is(xbus);
	} else if(sscanf(buf, "QUERY=%d", &xbus_num) == 1) {
		DBG("QUERY=%d\n", xbus_num);
		if((xbus = xbus_of(xbus_num)) == NULL) {
			ERR("No bus %d exists\n", xbus_num);
			return -ENXIO;
		}
		CALL_PROTO(GLOBAL, SYNC_SOURCE, xbus, NULL, SYNC_MODE_QUERY, 0);
	} else if(sscanf(buf, "%d %d", &xbus_num, &xpd_num) == 2) {
		NOTICE("Using deprecated syntax to update /proc/%s/%s\n", 
				PROC_DIR, PROC_SYNC);
		if(xpd_num != 0) {
			ERR("Currently can only set sync for XPD #0\n");
			return -EINVAL;
		}
		if((xbus = xbus_of(xbus_num)) == NULL) {
			ERR("No bus %d exists\n", xbus_num);
			return -ENXIO;
		}
		sync_master_is(xbus);
	} else {
		ERR("%s: cannot parse '%s'\n", __FUNCTION__, buf);
		count = -EINVAL;
	}
	return count;
}

int proc_xpd_ztregister_read(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	int		len = 0;
	unsigned long	flags;
	xpd_t		*xpd = data;

	BUG_ON(!xpd);
	spin_lock_irqsave(&xpd->lock, flags);

	len += sprintf(page + len, "%d\n", SPAN_REGISTERED(xpd));
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

static int proc_xpd_ztregister_write(struct file *file, const char __user *buffer, unsigned long count, void *data)
{
	xpd_t		*xpd = data;
	char		buf[MAX_PROC_WRITE];
	int		zt_reg;
	int		ret;

	BUG_ON(!xpd);
	if(count >= MAX_PROC_WRITE)
		return -EINVAL;
	if(copy_from_user(buf, buffer, count))
		return -EFAULT;
	buf[count] = '\0';
	ret = sscanf(buf, "%d", &zt_reg);
	if(ret != 1)
		return -EINVAL;
	DBG("%s: %s/%s %s\n", __FUNCTION__,
			xpd->xbus->busname, xpd->xpdname, (zt_reg) ? "register" : "unregister");
	if(zt_reg)
		ret = zaptel_register_xpd(xpd);
	else
		ret = zaptel_unregister_xpd(xpd);
	return (ret < 0) ? ret : count;
}

int proc_xpd_blink_read(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	int		len = 0;
	unsigned long	flags;
	xpd_t		*xpd = data;

	BUG_ON(!xpd);
	spin_lock_irqsave(&xpd->lock, flags);

	len += sprintf(page + len, "%d\n", xpd->blink_mode);
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

static int proc_xpd_blink_write(struct file *file, const char __user *buffer, unsigned long count, void *data)
{
	xpd_t		*xpd = data;
	char		buf[MAX_PROC_WRITE];
	int		blink;
	int		ret;

	BUG_ON(!xpd);
	if(count >= MAX_PROC_WRITE)
		return -EINVAL;
	if(copy_from_user(buf, buffer, count))
		return -EFAULT;
	buf[count] = '\0';
	ret = sscanf(buf, "%d", &blink);
	if(ret != 1)
		return -EINVAL;
	DBG("%s: %s/%s %s\n", __FUNCTION__,
			xpd->xbus->busname, xpd->xpdname, (blink) ? "blink" : "unblink");
	xpd->blink_mode = blink;
	return count;
}

#endif

/**
 *
 * Frame is freed:
 * 	- In case of error, by this function.
 * 	- Otherwise, by the underlying sending mechanism
 */
int xframe_send(xbus_t *xbus, xframe_t *xframe)
{
	int		ret = -ENODEV;

	if(!xframe) {
		DBG("null xframe\n");
		return -EINVAL;
	}
	if(!xbus) {
		DBG("null xbus\n");
		ret = -EINVAL;
		goto error;
	}
	if (!xbus->hardware_exists) {
		DBG("xbus %s Dropped a xframe -- NO HARDWARE.", xbus->busname);
		ret = -ENODEV;
		goto error;
	}
	if(down_read_trylock(&xbus->in_use)) {
		ret = xbus->ops->xframe_send(xbus, xframe);
		XBUS_COUNTER(xbus, TX_BYTES) += XFRAME_LEN(xframe);
		up_read(&xbus->in_use);
	} else {
		DBG("Dropped xframe. %s is in_use\n", xbus->busname);
	}
	return ret;

error:	
	xbus->ops->xframe_free(xbus, xframe);
	return ret;
}


#define	XPP_MAX_LEN	512

/*------------------------- Zaptel Interfaces ----------------------*/

#define	PREP_REPORT_RATE	1000

static void xpp_transmitprep(xbus_t *xbus, int unit, xpp_line_t lines, xpacket_t *pack)
{
	byte		*pcm;
	struct zt_chan	*chans;
	unsigned long	flags;
	int		i;
	int		subunit;


	BUG_ON(!xbus);
	BUG_ON(!pack);
	pcm = RPACKET_FIELD(pack, GLOBAL, PCM_WRITE, pcm);
	for(subunit = 0; subunit < MAX_SUBUNIT; subunit++, lines >>= SUBUNIT_PCM_SHIFT) {
		xpd_t		*tmp_xpd;

		tmp_xpd = xpd_by_addr(xbus, unit, subunit);
		if(!tmp_xpd || !tmp_xpd->card_present)
			continue;
		if(lines == 0)
			break;	/* Optimize */
		spin_lock_irqsave(&tmp_xpd->lock, flags);
		chans = tmp_xpd->span.chans;
		for (i = 0; i < tmp_xpd->channels; i++) {
			if(IS_SET(lines, i)) {
				if(SPAN_REGISTERED(tmp_xpd)) {
					memcpy((u_char *)pcm, chans[i].writechunk, ZT_CHUNKSIZE);
					// fill_beep((u_char *)pcm, tmp_xpd->addr.subunit, 2);
					// memset((u_char *)pcm, pcmtx[tmp_xpd->addr.subunit % 4], ZT_CHUNKSIZE);
				} else
					memset((u_char *)pcm, 0x7F, ZT_CHUNKSIZE);
				pcm += ZT_CHUNKSIZE;
			}
		}
		XPD_COUNTER(tmp_xpd, PCM_WRITE)++;
		spin_unlock_irqrestore(&tmp_xpd->lock, flags);
		if(!IS_BRI(tmp_xpd))
			break; /* only BRI has subunits */
	}
}

void fill_beep(u_char *buf, int num, int duration)
{
	bool	alternate = (duration) ? (jiffies/(duration*1000)) & 0x1 : 0;
	int	which;
	u_char	*snd;

	/*
	 * debug tones
	 */
	static u_char beep[] = {
		0x7F, 0xBE, 0xD8, 0xBE, 0x80, 0x41, 0x24, 0x41,	/* Dima */
		0x67, 0x90, 0x89, 0x90, 0xFF, 0x10, 0x09, 0x10,	/* Izzy */
	};
	static u_char beep_alt[] = {
		0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F,	/* silence */
	};
	if(alternate) {
		which = num % ARRAY_SIZE(beep_alt);
		snd = &beep_alt[which];
	} else {
		which = num % ARRAY_SIZE(beep);
		snd = &beep[which];
	}
	memcpy(buf, snd, ZT_CHUNKSIZE);
}

#ifdef	XPP_EC_CHUNK
/*
 * Taken from zaptel.c
 */
static inline void xpp_ec_chunk(struct zt_chan *chan, unsigned char *rxchunk, const unsigned char *txchunk)
{
	int16_t		rxlin;
	int		x;
	unsigned long	flags;

	/* Perform echo cancellation on a chunk if necessary */
	if (!chan->ec)
		return;
	spin_lock_irqsave(&chan->lock, flags);
	for (x=0;x<ZT_CHUNKSIZE;x++) {
		rxlin = ZT_XLAW(rxchunk[x], chan);
		rxlin = xpp_echo_can_update(chan->ec, ZT_XLAW(txchunk[x], chan), rxlin);
		rxchunk[x] = ZT_LIN2X((int)rxlin, chan);
	}
	spin_unlock_irqrestore(&chan->lock, flags);
}
#endif


static void xpp_receiveprep(xpd_t *xpd)
{
	volatile u_char *readchunk;
	int i;
	int	channels = xpd->channels;
	struct zt_chan	*chans = xpd->span.chans;
	unsigned long	flags;

	spin_lock_irqsave(&xpd->lock, flags);
//	if((xpd->timer_count % PREP_REPORT_RATE) == 0)
//		DBG("%d\n", xpd->timer_count);

	if (xpd->timer_count & 1) {
		/* First part */
		readchunk = xpd->readchunk;
	} else {
		readchunk = xpd->readchunk + ZT_CHUNKSIZE * CHANNELS_PERXPD;
	}

	for (i = 0; i < channels; i++, readchunk += ZT_CHUNKSIZE) {
		/*
		 * We don't copy signalling buffers (they may be
		 * longer than ZT_CHUNKSIZE).
		 */
		if(IS_SET(xpd->digital_signalling, i))
			continue;
		if(IS_SET(xpd->offhook, i) || IS_SET(xpd->cid_on, i) || IS_BRI(xpd)) {
			// memset((u_char *)readchunk, 0x5A, ZT_CHUNKSIZE);	// DEBUG
			// fill_beep((u_char *)readchunk, 1, 1);	// DEBUG: BEEP
			memcpy(chans[i].readchunk, (u_char *)readchunk, ZT_CHUNKSIZE);
		} else {
			memset(chans[i].readchunk, 0x7F, ZT_CHUNKSIZE);	// SILENCE
		}
	}
#if WITH_ECHO_SUPPRESSION
	/* FIXME: need to Echo cancel double buffered data */
	for (i = 0;i < xpd->span.channels; i++) {
		if(IS_SET(xpd->digital_signalling, i))	/* Don't echo cancel PRI/BRI D-chans */
			continue;
#ifdef XPP_EC_CHUNK 
		/* even if defined, parameterr xpp_ec can override at run-time */
		if (xpp_ec)
			xpp_ec_chunk(&chans[i], chans[i].readchunk, xpd->ec_chunk2[i]);
		else
#endif
			zt_ec_chunk(&chans[i], chans[i].readchunk, xpd->ec_chunk2[i]);
		memcpy(xpd->ec_chunk2[i], xpd->ec_chunk1[i], ZT_CHUNKSIZE);
		memcpy(xpd->ec_chunk1[i], chans[i].writechunk, ZT_CHUNKSIZE);
	}
#endif
	spin_unlock_irqrestore(&xpd->lock, flags);
	/*
	 * This should be out of spinlocks, as it may call back our hook setting
	 * methods
	 */
	zt_receive(&xpd->span);
}

/*
 * Called only for 'span' keyword in /etc/zaptel.conf
 */
static int xpp_startup(struct zt_span *span)
{
	xpd_t	*xpd = span->pvt;

	BUG_ON(!xpd);
	// Turn on all channels
	CALL_XMETHOD(XPD_STATE, xpd->xbus, xpd, 1);
	if(!xpd->xops->span_startup) {
		NOTICE("%s/%s: %s called\n", xpd->xbus->busname, xpd->xpdname, __FUNCTION__);
		return 0;
	}
	return xpd->xops->span_startup(xpd);
}

/*
 * Called only for 'span' keyword in /etc/zaptel.conf
 */
static int xpp_spanconfig(struct zt_span *span, struct zt_lineconfig *lc)
{
	xpd_t *xpd = span->pvt;

	DBG("%s\n", xpd->xpdname);
	return 0;
}

/*
 * Called only for 'span' keyword in /etc/zaptel.conf
 */
static int xpp_shutdown(struct zt_span *span)
{
	xpd_t	*xpd = span->pvt;

	BUG_ON(!xpd);
	// Turn off all channels
	CALL_XMETHOD(XPD_STATE, xpd->xbus, xpd, 0);
	if(!xpd->xops->span_shutdown) {
		NOTICE("%s/%s: %s called\n", xpd->xbus->busname, xpd->xpdname, __FUNCTION__);
		return 0;
	}
	return xpd->xops->span_shutdown(xpd);
}

/*
 * Called from zaptel with spinlock held on chan. Must not call back
 * zaptel functions.
 */
int xpp_open(struct zt_chan *chan)
{
	xpd_t		*xpd = chan->pvt;
	xbus_t		*xbus = xpd->xbus;
	int		pos = chan->chanpos - 1;
	unsigned long	flags;

	spin_lock_irqsave(&xbus->lock, flags);
	xbus->open_counter++;
	atomic_inc(&xpd->open_counter);
	if(IS_SET(xpd->digital_signalling, pos))	/* D-chan offhook */
		BIT_SET(xpd->offhook, pos);
	DBG("chan=%d (open_counter=%d)\n", pos, xbus->open_counter);
	spin_unlock_irqrestore(&xbus->lock, flags);
	if(xpd->xops->card_open)
		xpd->xops->card_open(xpd, pos);
	return 0;
}

int xpp_close(struct zt_chan *chan)
{
	xpd_t		*xpd = chan->pvt;
	xbus_t		*xbus = xpd->xbus;
	int		pos = chan->chanpos - 1;
	unsigned long	flags;
	bool		should_remove = 0;

	spin_lock_irqsave(&xbus->lock, flags);
	xbus->open_counter--;
	atomic_dec(&xpd->open_counter);
	if (!xbus->hardware_exists && xbus->open_counter == 0)
		should_remove = 1;
	if(IS_SET(xpd->digital_signalling, pos))	/* D-chan onhook */
		BIT_CLR(xpd->offhook, pos);
	spin_unlock_irqrestore(&xbus->lock, flags);
	if(xpd->xops->card_close)
		xpd->xops->card_close(xpd, pos);
	DBG("chan=%d (open_counter=%d, should_remove=%d)\n", pos, xbus->open_counter, should_remove);
	if(should_remove) {
		DBG("Going to remove: %s\n", xbus->busname);
		xbus_remove(xbus);
	}
	return 0;
}

int xpp_ioctl(struct zt_chan *chan, unsigned int cmd, unsigned long arg)
{
	xpd_t	*xpd = chan->pvt;
	int	pos = chan->chanpos - 1;
	int	x;
	int	ret = 0;

	switch (cmd) {
		case ZT_ONHOOKTRANSFER:
			if (get_user(x, (int __user *)arg))
				return -EFAULT;
			DBG("%s/%s/%d: ZT_ONHOOKTRANSFER (%d millis)\n",
					xpd->xbus->busname, xpd->xpdname, pos, x);
			if(xpd->xops->chan_onhooktransfer)
				ret = CALL_XMETHOD(chan_onhooktransfer, xpd->xbus, xpd, pos, x);
			return ret;
		case ZT_TONEDETECT:
			if (get_user(x, (int __user *)arg))
				return -EFAULT;
			DBG("%s/%s/%d: ZT_TONEDETECT (TONEDETECT_ON=%d TONEDETECT_MUTE=%d)\n",
				xpd->xbus->busname, xpd->xpdname, pos,
				(x & ZT_TONEDETECT_ON), (x & ZT_TONEDETECT_MUTE));
			return -ENOTTY;
		default:
			/* Some span-specific commands before we give up: */
			if (xpd->xops->card_ioctl != NULL) {
				ret = xpd->xops->card_ioctl(xpd, pos, cmd, arg);
				if (ret != -ENOTTY)
					return ret;
			}
			DBG("ENOTTY: chan=%d cmd=0x%x\n", pos, cmd);
			DBG("        IOC_TYPE=0x%02X\n", _IOC_TYPE(cmd));
			DBG("        IOC_DIR=0x%02X\n", _IOC_DIR(cmd));
			DBG("        IOC_NR=0x%02X\n", _IOC_NR(cmd));
			DBG("        IOC_SIZE=0x%02X\n", _IOC_SIZE(cmd));
			return -ENOTTY;
	}
	return 0;
}

static int xpp_hooksig(struct zt_chan *chan, zt_txsig_t txsig)
{
	xpd_t	*xpd = chan->pvt;
	xbus_t	*xbus;
	int pos = chan->chanpos - 1;

	BUG_ON(!xpd);
	xbus = xpd->xbus;
	BUG_ON(!xbus);
	DBG("Setting %s to %s (%d)\n", chan->name, txsig2str(txsig), txsig);
	return CALL_XMETHOD(card_hooksig, xbus, xpd, pos, txsig);
}

/* Req: Set the requested chunk size.  This is the unit in which you must
   report results for conferencing, etc */
int xpp_setchunksize(struct zt_span *span, int chunksize);

/* Enable maintenance modes */
int xpp_maint(struct zt_span *span, int cmd)
{
	xpd_t		*xpd = span->pvt;
	int		ret = 0;
#if 0
	char		loopback_data[] = "THE-QUICK-BROWN-FOX-JUMPED-OVER-THE-LAZY-DOG";
#endif

	BUG_ON(!xpd);
	DBG("%s: span->mainttimer=%d\n", __FUNCTION__, span->mainttimer);
	switch(cmd) {
		case ZT_MAINT_NONE:
			printk("XXX Turn off local and remote loops XXX\n");
			break;
		case ZT_MAINT_LOCALLOOP:
			printk("XXX Turn on local loopback XXX\n");
			break;
		case ZT_MAINT_REMOTELOOP:
			printk("XXX Turn on remote loopback XXX\n");
			break;
		case ZT_MAINT_LOOPUP:
			printk("XXX Send loopup code XXX\n");
			// CALL_XMETHOD(LOOPBACK_AX, xpd->xbus, xpd, loopback_data, ARRAY_SIZE(loopback_data));
			break;
		case ZT_MAINT_LOOPDOWN:
			printk("XXX Send loopdown code XXX\n");
			break;
		case ZT_MAINT_LOOPSTOP:
			printk("XXX Stop sending loop codes XXX\n");
			break;
		default:
			ERR("XPP: Unknown maint command: %d\n", cmd);
			ret = -EINVAL;
			break;
	}
	if (span->mainttimer || span->maintstat) 
		update_xpd_status(xpd, ZT_ALARM_LOOPBACK);
	return ret;
}

/*
 * Set signalling type (if appropriate)
 * Called from zaptel with spinlock held on chan. Must not call back
 * zaptel functions.
 */
static int xpp_chanconfig(struct zt_chan *chan, int sigtype)
{
	DBG("channel %d (%s) -> %s\n", chan->channo, chan->name, sig2str(sigtype));
	// FIXME: sanity checks:
	// - should be supported (within the sigcap)
	// - should not replace fxs <->fxo ??? (covered by previous?)
	return 0;
}

#if 0
/* Okay, now we get to the signalling.  You have several options: */

/* Option 1: If you're a T1 like interface, you can just provide a
   rbsbits function and we'll assert robbed bits for you.  Be sure to 
   set the ZT_FLAG_RBS in this case.  */

/* Opt: If the span uses A/B bits, set them here */
int (*rbsbits)(struct zt_chan *chan, int bits);

/* Option 2: If you don't know about sig bits, but do have their
   equivalents (i.e. you can disconnect battery, detect off hook,
   generate ring, etc directly) then you can just specify a
   sethook function, and we'll call you with appropriate hook states
   to set.  Still set the ZT_FLAG_RBS in this case as well */
int (*hooksig)(struct zt_chan *chan, zt_txsig_t hookstate);

/* Option 3: If you can't use sig bits, you can write a function
   which handles the individual hook states  */
int (*sethook)(struct zt_chan *chan, int hookstate);
#endif

static int xpp_echocan(struct zt_chan *chan, int len)
{
#ifdef	XPP_EC_CHUNK
	if(len == 0) {	/* shut down */
		/* zaptel calls this also during channel initialization */
		if(chan->ec) {
			xpp_echo_can_free(chan->ec);
		}
		return 0;
	}
	if(chan->ec) {
		ERR("%s: Trying to override an existing EC (%p)\n", __FUNCTION__, chan->ec);
		return -EINVAL;
	}
	chan->ec = xpp_echo_can_create(len, 0);
	if(!chan->ec) {
		ERR("%s: Failed creating xpp EC (len=%d)\n", __FUNCTION__, len);
		return -EINVAL;
	}
#endif
	return 0;
}

#ifdef	CONFIG_ZAPTEL_WATCHDOG
/*
 * If the watchdog detects no received data, it will call the
 * watchdog routine
 */
static int xpp_watchdog(struct zt_span *span, int cause)
{
	static	int rate_limit = 0;

	if((rate_limit++ % 1000) == 0)
		DBG("\n");
	return 0;
}
#endif

/**
 * Unregister an xpd from zaptel and release related resources
 * @xpd The xpd to be unregistered
 * @returns 0 on success, errno otherwise
 * 
 * Checks that nobody holds an open channel.
 *
 * Called by:
 * 	- User action through /proc
 * 	- During xpd_remove()
 */
static int zaptel_unregister_xpd(xpd_t *xpd)
{
	unsigned long	flags;

	BUG_ON(!xpd);
	spin_lock_irqsave(&xpd->lock, flags);

	if(!SPAN_REGISTERED(xpd)) {
		NOTICE("%s/%s is already unregistered\n", xpd->xbus->busname, xpd->xpdname);
		spin_unlock_irqrestore(&xpd->lock, flags);
		return -EIDRM;
	}
	if(sync_master == xpd)
		sync_master_is(NULL);			// FIXME: it's better to elect a new prince
	update_xpd_status(xpd, ZT_ALARM_NOTOPEN);
	if(atomic_read(&xpd->open_counter)) {
		NOTICE("%s/%s is busy (open_counter=%d). Skipping.\n", xpd->xbus->busname, xpd->xpdname, atomic_read(&xpd->open_counter));
		spin_unlock_irqrestore(&xpd->lock, flags);
		return -EBUSY;
	}
	mdelay(2);	// FIXME: This is to give chance for transmit/receiveprep to finish.
	spin_unlock_irqrestore(&xpd->lock, flags);
	if(xpd->card_present)
		xpd->xops->card_zaptel_preregistration(xpd, 0);
	atomic_dec(&xpd->zt_registered);
	zt_unregister(&xpd->span);
#ifdef	ZAPTEL_SYNC_TICK
	zaptel_is_ticking = 0;
#endif
	if(xpd->card_present)
		xpd->xops->card_zaptel_postregistration(xpd, 0);
	return 0;
}

static int zaptel_register_xpd(xpd_t *xpd)
{
	struct zt_span	*span;
	xbus_t		*xbus;
	int		cn;
	const xops_t	*xops;

	BUG_ON(!xpd);
	xops = xpd->xops;

	if (SPAN_REGISTERED(xpd)) {
		ERR("xpd %s already registered\n", xpd->xpdname);
		return -EEXIST;
	}
	cn = xpd->channels;
	DBG("Initializing span: xpd %d have %d channels.\n", xpd->id, cn);

	memset(xpd->chans, 0, sizeof(struct zt_chan)*cn);
	memset(&xpd->span, 0, sizeof(struct zt_span));

	span = &xpd->span;
	xbus = xpd->xbus;
	snprintf(span->name, MAX_SPANNAME, "%s/%s", xbus->busname, xpd->xpdname);
	span->deflaw = ZT_LAW_MULAW;
	init_waitqueue_head(&span->maintq);
	span->pvt = xpd;
	span->channels = cn;
	span->chans = xpd->chans;

	span->startup = xpp_startup;
	span->shutdown = xpp_shutdown;
	span->spanconfig = xpp_spanconfig;
	span->chanconfig = xpp_chanconfig;
	span->open = xpp_open;
	span->close = xpp_close;
	span->flags = ZT_FLAG_RBS;
	span->hooksig = xpp_hooksig;	/* Only with RBS bits */
	span->ioctl = xpp_ioctl;
	span->maint = xpp_maint;
#ifdef	ZAPTEL_SYNC_TICK
	span->sync_tick = zaptel_sync_tick;
#endif
	if (xpp_ec)
		span->echocan = xpp_echocan;
#ifdef	CONFIG_ZAPTEL_WATCHDOG
	span->watchdog = xpp_watchdog;
#endif

	DBG("Registering span of %s.\n", xpd->xpdname);
	xpd->xops->card_zaptel_preregistration(xpd, 1);
	if(zt_register(&xpd->span, prefmaster)) {
		xbus_t	*xbus = xpd->xbus;
		ERR("%s/%s: Failed to zt_register span\n", xbus->busname, xpd->xpdname);
		return -ENODEV;
	}
	atomic_inc(&xpd->zt_registered);
	xpd->xops->card_zaptel_postregistration(xpd, 1);
	/*
	 * Update zaptel about our state
	 */
#if 0
	/*
	 * FIXME: since asterisk didn't open the channel yet, the report
	 * is discarded anyway. OTOH, we cannot report in xpp_open or
	 * xpp_chanconfig since zaptel call them with a spinlock on the channel
	 * and zt_hooksig tries to acquire the same spinlock, resulting in
	 * double spinlock deadlock (we are lucky that RH/Fedora kernel are
	 * compiled with spinlock debugging).... tough.
	 */
	for_each_line(xpd, cn) {
		struct zt_chan	*chans = xpd->span.chans;

		if(IS_SET(xpd->offhook, cn)) {
			NOTICE("%s/%s/%d: Report OFFHOOK to zaptel\n",
					xbus->busname, xpd->xpdname, cn);
			zt_hooksig(&chans[cn], ZT_RXSIG_OFFHOOK);
		}
	}
#endif
	return 0;
}

/*------------------------- Proc debugging interface ---------------*/

#ifdef CONFIG_PROC_FS

#if 0
static int xpp_zap_write_proc(struct file *file, const char __user *buffer, unsigned long count, void *data)
{
}
#endif

#endif

/*------------------------- Initialization -------------------------*/

static void do_cleanup(void)
{
	if(timer_pending(&xpp_timer))
		del_timer_sync(&xpp_timer);
	tasklet_kill(&tasklet_tick);
#ifdef CONFIG_PROC_FS
	DBG("Removing '%s' from proc\n", PROC_SYNC);
	remove_proc_entry(PROC_SYNC, xpp_proc_toplevel);
	if(xpp_proc_toplevel) {
		DBG("Removing '%s' from proc\n", PROC_DIR);
		remove_proc_entry(PROC_DIR, NULL);
		xpp_proc_toplevel = NULL;
	}
#endif
}

int __init xpp_zap_init(void)
{
	int			ret = 0;
	struct proc_dir_entry	*ent;

	INFO("%s revision %s MAX_XPDS=%d (%d*%d)\n", THIS_MODULE->name, XPP_VERSION,
			MAX_XPDS, MAX_UNIT, MAX_SUBUNIT);
#if	WITH_ECHO_SUPPRESSION
	INFO("FEATURE: %s with ECHO_SUPPRESSION\n", THIS_MODULE->name);
#else
	INFO("FEATURE: %s without ECHO_SUPPRESSION\n", THIS_MODULE->name);
#endif
	if (xpp_ec)
		INFO("FEATURE: %s with XPP_EC_CHUNK\n", THIS_MODULE->name);
	else
		INFO("FEATURE: %s without XPP_EC_CHUNK\n", THIS_MODULE->name);
#ifdef CONFIG_ZAPATA_BRI_DCHANS
	INFO("FEATURE: %s with BRISTUFF support\n", THIS_MODULE->name);
#else
	INFO("FEATURE: %s without BRISTUFF support\n", THIS_MODULE->name);
#endif
#ifdef	ZAPTEL_SYNC_TICK
	INFO("FEATURE: %s with sync_tick() from ZAPTEL\n", THIS_MODULE->name);
#else
	INFO("FEATURE: %s without sync_tick() from ZAPTEL\n", THIS_MODULE->name);
#endif
#ifdef CONFIG_PROC_FS
	xpp_proc_toplevel = proc_mkdir(PROC_DIR, NULL);
	if(!xpp_proc_toplevel) {
		ret = -EIO;
		goto err;
	}

	ent = create_proc_entry(PROC_SYNC, 0644, xpp_proc_toplevel);
	if(!ent) {
		ret = -EFAULT;
		goto err;
	}
	ent->read_proc = proc_sync_read;
	ent->write_proc = proc_sync_write;
	ent->data = NULL;
#endif
	ret = xbus_core_init();
	if(ret) {
		ERR("xbus_core_init failed (%d)\n", ret);
		goto err;
	}

	/* Only timer init. We add it only *after* zt_register */
	init_timer(&xpp_timer);
	sync_master_is(NULL);			/* Internal ticking */
	return 0;
err:
	do_cleanup();
	return ret;
}

void __exit xpp_zap_cleanup(void)
{
	xbus_core_shutdown();
	do_cleanup();
}

EXPORT_SYMBOL(print_dbg);
EXPORT_SYMBOL(card_detected);
EXPORT_SYMBOL(xpd_alloc);
EXPORT_SYMBOL(xpd_disconnect);
EXPORT_SYMBOL(xframe_send);
EXPORT_SYMBOL(update_xpd_status);
EXPORT_SYMBOL(update_zap_ring);
EXPORT_SYMBOL(update_line_status);
EXPORT_SYMBOL(fill_beep);
EXPORT_SYMBOL(xpp_open);
EXPORT_SYMBOL(xpp_close);
EXPORT_SYMBOL(xpp_ioctl);
EXPORT_SYMBOL(xpp_maint);

MODULE_DESCRIPTION("XPP Zaptel Driver");
MODULE_AUTHOR("Oron Peled <oron@actcom.co.il>");
MODULE_LICENSE("GPL");
MODULE_VERSION(XPP_VERSION);

module_init(xpp_zap_init);
module_exit(xpp_zap_cleanup);
