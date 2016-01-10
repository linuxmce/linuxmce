#ifndef	XPD_H
#define	XPD_H

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
#include "xproto.h"

#ifdef	__KERNEL__
#include <linux/kernel.h>
#include <linux/device.h>
#include <asm/atomic.h>
#include <asm/semaphore.h>
#include <linux/moduleparam.h>
#ifdef	XPP_DEBUGFS
#ifndef	CONFIG_DEBUG_FS
#warning kernel does not include CONFIG_DEBUG_FS, canceling XPP_DEBUGFS support
#undef	XPP_DEBUGFS
#else
#include <linux/debugfs.h>
#endif
#endif
#endif	/* __KERNEL__ */

#include <zaptel.h>

#ifdef __KERNEL__
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,14)
/* also added in RHEL kernels with the OpenInfiniband backport: */
#if LINUX_VERSION_CODE != KERNEL_VERSION(2,6,9) || !defined(DEFINE_SPINLOCK)
typedef	unsigned gfp_t;		/* Added in 2.6.14 */
#endif
#endif

/*
 * FIXME: Kludge for 2.6.19
 * bool is now defined as a proper boolean type (gcc _Bool)
 * but the command line parsing framework handles it as int.
 */
#define	DEF_PARM_BOOL(name,init,perm,desc)	\
	int name = init;	\
	module_param(name, bool, perm);		\
	MODULE_PARM_DESC(name, desc " [default " #init "]")

#define	DEF_PARM(type,name,init,perm,desc)	\
	type name = init;	\
	module_param(name, type, perm);		\
	MODULE_PARM_DESC(name, desc " [default " #init "]")

#if	LINUX_VERSION_CODE	< KERNEL_VERSION(2,6,10)
/*
 * Old 2.6 kernels had module_param_array() macro that receive the counter
 * by value.
 */
#define	DEF_ARRAY(type,name,count,init,desc)	\
	unsigned int name ## _num_values;	\
	type name[count] = { [0 ... ((count)-1)] = (init) };			\
	module_param_array(name, type, name ## _num_values, 0600);	\
	MODULE_PARM_DESC(name, desc " ( 1-" __MODULE_STRING(count) ")")
#else
#define	DEF_ARRAY(type,name,count,init,desc)	\
	unsigned int name ## _num_values;	\
	type name[count] = {[0 ... ((count)-1)] = init};			\
	module_param_array(name, type, &name ## _num_values, 0600);	\
	MODULE_PARM_DESC(name, desc " ( 1-" __MODULE_STRING(count) ")")
#endif
#endif	// __KERNEL__


typedef	struct xbus_ops		xbus_ops_t;

typedef enum xbus_type {
		FIRMWARE_LOOPBACK = 1,
		FIRMWARE_XPP = 2,
} xbus_type_t;

#ifdef	__KERNEL__


typedef struct packet_queue {
	char			qname[XPD_NAMELEN];
	struct list_head	head;
	unsigned int		count;
	unsigned int		worst_count;
	unsigned int		overflows;
	spinlock_t		lock;
} packet_queue_t;

struct xbus_ops {
	int (*xframe_send)(xbus_t *xbus, xframe_t *xframe);
	xframe_t *(*xframe_new)(xbus_t *xbus, gfp_t flags);
	void (*xframe_free)(xbus_t *xbus, xframe_t *p);
};

/*
 * XBUS statistics counters
 */
enum {
	XBUS_N_DESC_REQ,
	XBUS_N_DEV_DESC_FULL,
	XBUS_N_DEV_DESC_EMPTY,
	XBUS_N_SEND_PCM,
	XBUS_N_PCM_READ,
	XBUS_N_TX_BYTES,
	XBUS_N_RX_BYTES,
};

#define	XBUS_COUNTER(xbus, counter)	((xbus)->counters[XBUS_N_ ## counter])

#define	C_(x)	[ XBUS_N_ ## x ] = { #x }

/* yucky, make an instance so we can size it... */
static struct xbus_counters {
	char	*name;
} xbus_counters[] = {
	C_(DESC_REQ),
	C_(DEV_DESC_FULL),
	C_(DEV_DESC_EMPTY),
	C_(SEND_PCM),
	C_(PCM_READ),
	C_(TX_BYTES),
	C_(RX_BYTES),
};

#undef C_

#define	XBUS_COUNTER_MAX	ARRAY_SIZE(xbus_counters)

#define	CARD_DESC_MAGIC	0xca9dde5c

struct	card_desc_struct {
	struct list_head	card_list;
	u32			magic;
	xbus_t			*xbus;
	byte			rev;		/* Revision number */
	byte			type;		/* LSB: 1 - to_phone, 0 - to_line */
	xpd_addr_t		xpd_addr;
	xpp_line_t		line_status;	/* Initial line status (offhook) */
};

#ifdef XPP_DEBUGFS
/* definition in xbus-core.c */
struct debugfs_data;
#endif

/*
 * Encapsulate all poll related data of a single xbus.
 */
struct xbus_poller {
	/*
	 * Bus scanning
	 */
	xbus_t			*xbus;
	struct workqueue_struct	*wq;
	bool			is_polling;
	atomic_t		count_poll_answers;
	struct list_head	poll_results;
	wait_queue_head_t	wait_for_polls;

	struct work_struct	xpds_init_work;

	atomic_t		count_xpds_to_initialize;
	atomic_t		count_xpds_initialized;
	wait_queue_head_t	wait_for_xpd_initialization;
};

/*
 * An xbus is a transport layer for Xorcom Protocol commands
 */
struct xbus {
	char			busname[XBUS_NAMELEN];	/* only xbus_new set this */
	char			busdesc[XBUS_DESCLEN];	/* lowlevel drivers set this */
	int			num;
	xbus_ops_t		*ops;
	struct xpd		*xpds[MAX_XPDS];
	int			max_packet_size;

	/* Device-Model */
	struct device		the_bus;

	/* Simulator data */
	xbus_type_t		bus_type;

	spinlock_t		lock;

	bool			hardware_exists;	/* Hardware is functional */
	int			open_counter;		/* Number of open channels */
	atomic_t		packet_counter;		/* Allocated packets */
	wait_queue_head_t	packet_cache_empty;

	/* PCM metrics */
	struct timeval		last_tx_sync;
	struct timeval		last_rx_sync;
	unsigned long		max_tx_sync;
	unsigned long		min_tx_sync;
	unsigned long		max_rx_sync;
	unsigned long		min_rx_sync;

	struct xbus_poller	*poller;

	/*
	 * Sync adjustment
	 */
	int			sync_adjustment;
	long			pll_updated_at;

	struct	rw_semaphore	in_use;
	int			num_xpds;
	void			*priv;			/* Pointer to transport level data structures */

#ifdef	XPP_DEBUGFS
	struct dentry		*debugfs_dir;
	struct dentry		*debugfs_file;
	struct debugfs_data	*debugfs_data;
#endif
#ifdef CONFIG_PROC_FS
	struct proc_dir_entry	*proc_xbus_dir;
	struct proc_dir_entry	*proc_xbus_summary;
	struct proc_dir_entry	*proc_xbus_waitfor_xpds;
#ifdef	PROTOCOL_DEBUG
	struct proc_dir_entry	*proc_xbus_command;
#endif
#endif

	/* statistics */
	int		counters[XBUS_COUNTER_MAX];
};
#endif

typedef enum xpd_direction {
	TO_PSTN = 0,
	TO_PHONE = 1,
} xpd_direction_t;

#ifdef	__KERNEL__

/*
 * XPD statistics counters
 */
enum {
	XPD_N_PCM_READ,
	XPD_N_PCM_WRITE,
	XPD_N_RECV_ERRORS,
};

#define	XPD_COUNTER(xpd, counter)	((xpd)->counters[XPD_N_ ## counter])

#define	C_(x)	[ XPD_N_ ## x ] = { #x }

/* yucky, make an instance so we can size it... */
static struct xpd_counters {
	char	*name;
} xpd_counters[] = {
	C_(PCM_READ),
	C_(PCM_WRITE),
	C_(RECV_ERRORS),
};

#undef C_

#define	XPD_COUNTER_MAX	(sizeof(xpd_counters)/sizeof(xpd_counters[0]))

/*
 * An XPD is a single Xorcom Protocol Device
 */
struct xpd {
	char xpdname[XPD_NAMELEN];
	struct zt_span	span;
	struct zt_chan	*chans;
	int channels;
	xpd_type_t	type;
	byte		revision;		/* Card revision */
	xpd_direction_t	direction;		/* TO_PHONE, TO_PSTN */
	xpp_line_t	no_pcm;			/* Temporary: disable PCM (for USB-1) */
	xpp_line_t	offhook;		/* Actual chip state: 0 - ONHOOK, 1 - OFHOOK */
	xpp_line_t	cid_on;
	xpp_line_t	digital_outputs;	/* 0 - no, 1 - yes */
	xpp_line_t	digital_inputs;		/* 0 - no, 1 - yes */
	xpp_line_t	digital_signalling;	/* PRI/BRI signalling channels */

	bool		ringing[CHANNELS_PERXPD];

	xbus_t *xbus;			/* The XBUS we are connected to */

	spinlock_t	lock;
	atomic_t	zt_registered;	/* Am I fully registered with zaptel */
	atomic_t	open_counter;	/* Number of open channels */

	int		flags;
	bool		blink_mode;	/* for visual identification */
#define	DEFAULT_LED_PERIOD	(1000/8)	/* in tick */

#ifdef CONFIG_PROC_FS
	struct proc_dir_entry	*proc_xpd_dir;
	struct proc_dir_entry	*proc_xpd_summary;
	struct proc_dir_entry	*proc_xpd_ztregister;
	struct proc_dir_entry	*proc_xpd_blink;
#endif

	int		counters[XPD_COUNTER_MAX];

	const xproto_table_t	*xproto;	/* Card level protocol table */
	const xops_t	*xops;			/* Card level operations */
	void		*priv;			/* Card level private data */
	bool		card_present;
	reg_cmd_t	requested_reply;
	reg_cmd_t	last_reply;

	unsigned long	last_response;	/* in jiffies */
	unsigned	id;
	xpd_addr_t	addr;
	struct list_head xpd_list;
	unsigned int	timer_count;
	volatile u_char *readchunk;	/* Double-word aligned read memory */
	/* Echo cancelation */
	u_char ec_chunk1[CHANNELS_PERXPD][ZT_CHUNKSIZE];
	u_char ec_chunk2[CHANNELS_PERXPD][ZT_CHUNKSIZE];
};

#define	for_each_line(xpd,i)	for((i) = 0; (i) < (xpd)->channels; (i)++)
#define	IS_BRI(xpd)		((xpd)->type == XPD_TYPE_BRI_NT || (xpd)->type == XPD_TYPE_BRI_TE)
#define	TICK_TOLERANCE		500 /* usec */

#ifdef	DEBUG_SYNC_PARPORT
void xbus_flip_bit(xbus_t *xbus, unsigned int bitnum0, unsigned int bitnum1);
#else
#define	xbus_flip_bit(xbus, bitnum0, bitnum1)
#endif

#endif

#endif	/* XPD_H */
