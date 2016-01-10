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
#include <linux/version.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
#  warning "This module is tested only with 2.6 kernels"
#endif

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/proc_fs.h>
#ifdef	PROTOCOL_DEBUG
#include <linux/ctype.h>
#endif
#include <linux/workqueue.h>
#include <linux/device.h>
#include <linux/delay.h>	/* for msleep() to debug */
#include "xpd.h"
#include "xpp_zap.h"
#include "xbus-core.h"
#ifdef	XPP_DEBUGFS
#include "xpp_log.h"
#endif
#include "zap_debug.h"

static const char rcsid[] = "$Id: xbus-core.c 2536 2007-05-17 21:47:40Z tzafrir $";

/* Defines */
#define	POLL_TIMEOUT		(2*MAX_XPDS)	/* in jiffies */
#define	INITIALIZATION_TIMEOUT	(60*HZ)		/* in jiffies */
#define	PROC_XBUSES		"xbuses"
#define	PROC_XBUS_SUMMARY	"summary"
#define	PROC_XBUS_WAITFOR_XPDS	"waitfor_xpds"

#ifdef	PROTOCOL_DEBUG
#define	PROC_XBUS_COMMAND	"command"
static int proc_xbus_command_write(struct file *file, const char __user *buffer, unsigned long count, void *data);
#endif

/* Command line parameters */
extern int print_dbg;
DEF_PARM(uint, poll_timeout, POLL_TIMEOUT, 0644, "Timeout (in jiffies) waiting for units to reply");

/* Forward declarations */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,14)
#define	DEVICE_ATTR_FUNC(name,dev,buf)	\
		ssize_t name(struct device *dev, struct device_attribute *attr, char *buf)
#else
#define	DEVICE_ATTR_FUNC(name,dev,buf)	\
		ssize_t name(struct device *dev, char *buf)
#endif

static DEVICE_ATTR_FUNC(connector_show, dev, buf);
static DEVICE_ATTR_FUNC(status_show, dev, buf);

static void xbus_release(struct device *dev);
static int xbus_read_proc(char *page, char **start, off_t off, int count, int *eof, void *data);
static int xbus_read_waitfor_xpds(char *page, char **start, off_t off, int count, int *eof, void *data);

/* Data structures */
static spinlock_t		xbuses_lock = SPIN_LOCK_UNLOCKED;
static xbus_t			*xbuses_array[MAX_BUSES] = {};
static int			bus_count = 0;
static struct proc_dir_entry	*proc_xbuses = NULL;

static	DEVICE_ATTR(connector, S_IRUGO, connector_show, NULL);
static	DEVICE_ATTR(status, S_IRUGO, status_show, NULL);

/*------------------------- Debugfs Handling -----------------------*/
#ifdef	XPP_DEBUGFS

#define DEBUGFS_BUFSIZ		4096	/* must be power of two, otherwise POS_IN_BUF will have to use '%' instead of '&' */
#define POS_IN_BUF(x)		((x) & (DEBUGFS_BUFSIZ-1))

struct debugfs_data {
	spinlock_t lock;
	xbus_t *xbus;
	char buffer[DEBUGFS_BUFSIZ];
	unsigned long head, tail;	/* reading and writing are performed at position (head % BUF_SIZ) and (tail % BUF_SIZ) */
	wait_queue_head_t queue;
};

static unsigned long add_to_buf(struct debugfs_data *d, unsigned long tail, const void *buf, unsigned long len)
{
	unsigned long count = min(len, (unsigned long)(DEBUGFS_BUFSIZ - POS_IN_BUF(tail)));
	memcpy(d->buffer + POS_IN_BUF(tail), buf, count);		/* fill starting at position tail */
	memcpy(d->buffer, (u_char *)buf + count, len - count);		/* fill leftover */
	return len;
}

int xbus_log(xbus_t *xbus, xpd_t *xpd, int direction, const void *buf, unsigned long len)
{
	unsigned long tail;
	unsigned long flags;
	struct debugfs_data *d;
	struct log_header header;
	int ret = 0;
	
	BUG_ON(!xbus);
	BUG_ON(!xpd);
	BUG_ON(sizeof(struct log_header) + len > DEBUGFS_BUFSIZ);
	d = xbus->debugfs_data;
	if (!d)			/* no consumer process */
		return ret;
	spin_lock_irqsave(&d->lock, flags);
	if (sizeof(struct log_header) + len > DEBUGFS_BUFSIZ - (d->tail - d->head)) {
		ret = -ENOSPC;
		DBG("Dropping debugfs data of len %lu, free space is %lu\n", sizeof(struct log_header) + len,
				DEBUGFS_BUFSIZ - (d->tail - d->head));
		goto out;
	}
	header.len = sizeof(struct log_header) + len;
	header.time = jiffies_to_msecs(jiffies);
	header.xpd_num = xpd->id;
	header.direction = (char)direction;
	tail = d->tail;
	tail += add_to_buf(d, tail, &header, sizeof(header));
	tail += add_to_buf(d, tail, buf, len);
	d->tail = tail;
	wake_up_interruptible(&d->queue);
out:
	spin_unlock_irqrestore(&d->lock, flags);
	return ret;
}

static struct dentry	*debugfs_root = NULL;
static int debugfs_open(struct inode *inode, struct file *file);
static ssize_t debugfs_read(struct file *file, char __user *buf, size_t nbytes, loff_t *ppos);
static int debugfs_release(struct inode *inode, struct file *file);

struct file_operations debugfs_operations = {
	.open		= debugfs_open,
	.read		= debugfs_read,
	.release	= debugfs_release,
};

/*
 * As part of the "inode diet" the private data member of struct inode
 * has changed in 2.6.19. However, Fedore Core 6 adopted this change
 * a bit earlier (2.6.18). If you use such a kernel, Change the 
 * following test from 2,6,19 to 2,6,18.
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19)
#define	I_PRIVATE(inode)	((inode)->u.generic_ip)
#else
#define	I_PRIVATE(inode)	((inode)->i_private)
#endif

static int debugfs_open(struct inode *inode, struct file *file)
{
	xbus_t	*xbus = I_PRIVATE(inode);
	struct debugfs_data *d;
	struct log_global_header gheader;

	BUG_ON(!xbus);
	DBG("%s\n", xbus->busname);
	if (xbus->debugfs_data)
		return -EBUSY;
	d = kmalloc(sizeof(struct debugfs_data), GFP_KERNEL);
	if (!d)
		return -ENOMEM;
	try_module_get(THIS_MODULE);
	memset(d, 0, sizeof(struct debugfs_data));
	spin_lock_init(&d->lock);
	d->xbus = xbus;
	d->head = d->tail = 0;
	init_waitqueue_head(&d->queue);
	file->private_data = d;

	gheader.magic = XPP_LOG_MAGIC;
	gheader.version = 1;
	d->tail += add_to_buf(d, d->tail, &gheader, sizeof(gheader));

	xbus->debugfs_data = d;
	return 0;
}

static ssize_t debugfs_read(struct file *file, char __user *buf, size_t nbytes, loff_t *ppos)
{
	struct debugfs_data *d = file->private_data;
	size_t len;

	BUG_ON(!d);
	BUG_ON(!d->xbus);
	DBG("%s\n", d->xbus->busname);
	while (d->head == d->tail) {
		if (wait_event_interruptible(d->queue, d->head != d->tail))
			return -EAGAIN;
	}
	len = min(nbytes, (size_t)(d->tail - d->head));
	if (copy_to_user(buf, d->buffer + POS_IN_BUF(d->head), len))
		return -EFAULT;
	d->head += len;
	/* optimization to avoid future buffer wraparound */
	if (d->head == d->tail) {
		unsigned long flags;
		spin_lock_irqsave(&d->lock, flags);
		if (d->head == d->tail)
			d->head = d->tail = 0;
		spin_unlock_irqrestore(&d->lock, flags);
	}
	return len;
}

static int debugfs_release(struct inode *inode, struct file *file)
{
	struct debugfs_data *d = file->private_data;

	BUG_ON(!d);
	BUG_ON(!d->xbus);
	DBG("%s\n", d->xbus->busname);
	d->xbus->debugfs_data = NULL;
	kfree(d);
	module_put(THIS_MODULE);
	return 0;
}
#endif

/*------------------------- Frame  Handling ------------------------*/
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,20)
static kmem_cache_t		*xframes_cache = NULL;
#else
static struct kmem_cache	*xframes_cache = NULL;
#endif

xframe_t *xbus_xframe_new(xbus_t *xbus, gfp_t flags)
{
	xframe_t	*frm;

	frm = kmem_cache_alloc(xframes_cache, flags);
	if(!frm)
		return NULL;
	memset(frm, 0, sizeof(xframe_t) + XFRAME_DATASIZE);
	atomic_set(&frm->frame_len, 0);
	frm->packets = (byte *)frm + sizeof(xframe_t);
	return frm;
}

void xbus_xframe_free(xbus_t *xbus, xframe_t *p)
{
	kmem_cache_free(xframes_cache, p);
}

/*
 * Return pointer to next packet slot in the frame
 * or NULL if the frame is full.
 *
 * FIXME: we do not use atomic_add_return() because kernel-2.6.8
 *        does not have it. This make this code a little racy,
 *        but we currently call xframe_next_packet() only in the
 *        PCM loop (xbus_tick() etc.)
 */
xpacket_t *xframe_next_packet(xframe_t *frm, int len)
{
	int newlen = atomic_read(&frm->frame_len);

	newlen += len;
//	DBG("len=%d, newlen=%d, frm->frame_len=%d\n", len, newlen, XFRAME_LEN(frm));
	if (newlen > XFRAME_DATASIZE) {
		return NULL;
	}
	atomic_add(len, &frm->frame_len);
	return (xpacket_t *)(frm->packets + newlen - len);
}

static spinlock_t serialize_dump_xframe = SPIN_LOCK_UNLOCKED;

static void do_hexdump(const char msg[], byte *data, uint16_t len)
{
	int	i;

	for(i = 0; i < len; i++)
		DBG("%s: %3d> %02X\n", msg, i, data[i]);
}

void dump_xframe(const char msg[], const xbus_t *xbus, const xframe_t *xframe)
{
	const uint16_t	frame_len = XFRAME_LEN(xframe);
	xpacket_t	*pack;
	uint16_t	pos = 0;
	uint16_t	nextpos;
	int 		num = 1;
	bool		do_print;
	static int	rate_limit;
	unsigned long	flags;
	
	spin_lock_irqsave(&serialize_dump_xframe, flags);
	do {
		if(pos >= XFRAME_DATASIZE) {
			if(printk_ratelimit()) {
				ERR("%s: xframe overflow (%d bytes)\n",
				    msg, frame_len);
				do_hexdump(msg, xframe->packets, frame_len);
			}
			break;
		}
		if(pos > frame_len) {
			if(printk_ratelimit()) {
				ERR("%s: packet overflow pos=%d frame_len=%d\n",
				    msg, pos, frame_len);
				do_hexdump(msg, xframe->packets, frame_len);
			}
			break;
		}
		pack = (xpacket_t *)&xframe->packets[pos];
		if(pack->datalen <= 0) {
			if(printk_ratelimit()) {
				ERR("%s: xframe -- bad datalen=%d pos=%d frame_len=%d\n",
				    msg, pack->datalen, pos, frame_len);
				do_hexdump(msg, xframe->packets, frame_len);
			}
			break;
		}
		nextpos = pos + pack->datalen;
		if(nextpos > frame_len) {
			if(printk_ratelimit()) {
				ERR("%s: packet overflow nextpos=%d frame_len=%d\n",
				    msg, nextpos, frame_len);
				do_hexdump(msg, xframe->packets, frame_len);
			}
			break;
		}
		do_print = 0;
		if(pack->opcode != XPROTO_NAME(GLOBAL,PCM_READ) &&
			pack->opcode != XPROTO_NAME(GLOBAL,PCM_WRITE))
			do_print = 1;
		if((print_dbg & DBG_PCM) && ((rate_limit % 1003) == 0))
			do_print = 1;
		if(do_print) {
			if(num == 1)
				DBG("%s: %s: frame_len=%d.\n",
						msg, xbus->busname, frame_len);
			DBG("  %3d. DATALEN=%d OP=0x%02X XPD-%d-%d (pos=%d)\n",
				num, pack->datalen, pack->opcode,
				pack->addr.unit, pack->addr.subunit, pos);
			dump_packet("     ", pack, print_dbg);
		}
		num++;
		pos = nextpos;
		if(pos >= frame_len)
			break;
	} while(1);
	spin_unlock_irqrestore(&serialize_dump_xframe, flags);
}


/*------------------------- Bus Management -------------------------*/
xbus_t *xbus_of(int xbus_num)
{
	if(xbus_num < 0 || xbus_num >= MAX_BUSES)
		return NULL;
	return xbuses_array[xbus_num];
}

xpd_t	*xpd_of(const xbus_t *xbus, int xpd_num)
{
	if(!VALID_XPD_NUM(xpd_num))
		return NULL;
	return xbus->xpds[xpd_num];
}

int xbus_register_xpd(xbus_t *xbus, xpd_t *xpd)
{
	unsigned int	xpd_num = xpd->id;
	unsigned long	flags;
	int		ret = 0;

	spin_lock_irqsave(&xbus->lock, flags);
	if(!VALID_XPD_NUM(xpd_num)) {
		ERR("%s: Bad xpd_num = %d\n", xbus->busname, xpd_num);
		ret = -EINVAL;
		goto out;
	}
	if(xbus->xpds[xpd_num] != NULL) {
		xpd_t	*other = xbus->xpds[xpd_num];

		ERR("%s: xpd_num=%d is occupied by %p (%s)\n",
				xbus->busname, xpd_num, other, other->xpdname);
		ret = -EINVAL;
		goto out;
	}
	xbus->xpds[xpd_num] = xpd;
	xbus->num_xpds++;
out:
	spin_unlock_irqrestore(&xbus->lock, flags);
	return ret;
}

int xbus_unregister_xpd(xbus_t *xbus, xpd_t *xpd)
{
	unsigned int	xpd_num = xpd->id;
	unsigned long	flags;
	int		ret = 0;

	spin_lock_irqsave(&xbus->lock, flags);
	if(!VALID_XPD_NUM(xpd_num)) {
		ERR("%s: Bad xpd_num = %d\n", xbus->busname, xpd_num);
		ret = -EINVAL;
		goto out;
	}
	if(xbus->xpds[xpd_num] != xpd) {
		xpd_t	*other = xbus->xpds[xpd_num];

		ERR("%s: xpd_num=%d is occupied by %p (%s)\n",
				xbus->busname, xpd_num, other, other->xpdname);
		ret = -EINVAL;
		goto out;
	}
	xbus->xpds[xpd_num] = NULL;
	xbus->num_xpds--;
	xpd->xbus = NULL;
out:
	spin_unlock_irqrestore(&xbus->lock, flags);
	return ret;
}

/*
 * This must be called from synchronous (non-interrupt) context
 * it returns only when all XPD's on the bus are detected and
 * initialized.
 */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
static void xbus_poll(struct work_struct *work)
{
	struct xbus_poller	*poller = container_of(work, struct xbus_poller, xpds_init_work);
#else
static void xbus_poll(void *data)
{
	struct xbus_poller	*poller = data;
#endif
	int			id;
	int			ret = 0;
	unsigned long		flags;
	struct list_head	*card;
	struct list_head	*next_card;
	struct list_head	removal_list;
	struct list_head	additions_list;
	int			count_removed;
	int			count_added;
	xbus_t			*xbus;

	BUG_ON(!poller);
	xbus = poller->xbus;
	BUG_ON(!xbus);
	if(!down_read_trylock(&xbus->in_use)) {
		ERR("%s is being removed...\n", xbus->busname);
		return;
	}
	msleep(2);	/* roundtrip for older polls */
	spin_lock_irqsave(&xbus->lock, flags);
	DBG("%s\n", xbus->busname);
	poller->is_polling = 1;

	/*
	 * Send out the polls
	 */
	for(id = 0; id < MAX_XPDS; id++) {
		if(!xbus->hardware_exists)
			break;
		// DBG("  Polling slot %d %s\n", id, xbus->busname);
		spin_unlock_irqrestore(&xbus->lock, flags);
		ret = CALL_PROTO(GLOBAL, DESC_REQ, xbus, NULL, id);
		spin_lock_irqsave(&xbus->lock, flags);
		if(ret < 0) {
			ERR("xpp: %s: Failed sending DESC_REQ to XPD #%d\n", __FUNCTION__, id);
			goto out;
		}
		mdelay(1);	/* FIXME: debugging for Dima */
	}
	spin_unlock_irqrestore(&xbus->lock, flags);
	/*
	 * Wait for replies
	 */
	DBG("%s: Polled %d XPD's. Waiting for replies max %d jiffies\n", xbus->busname, MAX_XPDS, poll_timeout);
	ret = wait_event_interruptible_timeout(poller->wait_for_polls, atomic_read(&poller->count_poll_answers) >= MAX_XPDS, poll_timeout);
	if(ret == 0) {
		ERR("%s: Poll timeout. Continuing anyway.\n", xbus->busname);
		/*
		 * Continue processing. Maybe some units did reply.
		 */
	} else if(ret < 0) {
		ERR("%s: Poll interrupted %d\n", xbus->busname, ret);
		goto out;
	} else
		DBG("%s: Poll finished in %d jiffies.\n", xbus->busname, poll_timeout - ret);
	/*
	 * Build removals/additions lists
	 */
	spin_lock_irqsave(&xbus->lock, flags);
	INIT_LIST_HEAD(&removal_list);
	INIT_LIST_HEAD(&additions_list);
	count_removed = 0;
	count_added = 0;
	list_for_each_safe(card, next_card, &poller->poll_results) {
		struct card_desc_struct	*card_desc = list_entry(card, struct card_desc_struct, card_list);
		byte			type = card_desc->type;
		xpd_t			*xpd;

		BUG_ON(card_desc->magic != CARD_DESC_MAGIC);
		xpd = xpd_by_addr(xbus, card_desc->xpd_addr.unit, card_desc->xpd_addr.subunit);

		if(xpd && type == XPD_TYPE_NOMODULE) {		/* card removal */
			list_move_tail(card, &removal_list);
			count_removed++;
		} else if(!xpd && type != XPD_TYPE_NOMODULE) {	/* card detection */
			list_move_tail(card, &additions_list);
			count_added++;
		} else {					/* same same */
			list_del(card);
			kfree(card_desc);
		}
	}
	/*
	 * We set this *after* poll is finished, so wait_for_xpd_initialization can
	 * tell we already know how many units we have.
	 */
	atomic_set(&poller->count_xpds_to_initialize, count_added);
	spin_unlock_irqrestore(&xbus->lock, flags);
	INFO("%s: Poll results: removals=%d additions=%d\n", xbus->busname, count_removed, count_added);
	/*
	 * Process removals first
	 */
	list_for_each_safe(card, next_card, &removal_list) {
		struct card_desc_struct	*card_desc = list_entry(card, struct card_desc_struct, card_list);
		xpd_t			*xpd;

		list_del(card);
		xpd = xpd_by_addr(xbus, card_desc->xpd_addr.unit, card_desc->xpd_addr.subunit);
		if(xpd)
			xpd_disconnect(xpd);
		kfree(card);
	}
	/*
	 * Now process additions
	 */
	list_for_each_safe(card, next_card, &additions_list) {
		struct card_desc_struct	*card_desc = list_entry(card, struct card_desc_struct, card_list);

		list_del(card);
		/* FIXME: card_detected() should have a return value for count_xpds_initialized */
		card_detected(card_desc);
		atomic_inc(&poller->count_xpds_initialized);
	}
	wake_up(&poller->wait_for_xpd_initialization);
out:
	poller->is_polling = 0;
	up_read(&xbus->in_use);
	return;
}

void xbus_poller_notify(xbus_t *xbus, struct card_desc_struct *card_desc)
{
	struct xbus_poller	*poller;
	unsigned long		flags;

	BUG_ON(!xbus);
	poller = xbus->poller;
	BUG_ON(!poller);
	if(!poller->is_polling) {
		NOTICE("%s: %d-%d replied not during poll. Ignore\n",
				xbus->busname,
				card_desc->xpd_addr.unit,
				card_desc->xpd_addr.subunit);
		kfree(card_desc);
		return;
	}
	spin_lock_irqsave(&xbus->lock, flags);
	if(card_desc->type == XPD_TYPE_NOMODULE)
		XBUS_COUNTER(xbus, DEV_DESC_EMPTY)++;
	else
		XBUS_COUNTER(xbus, DEV_DESC_FULL)++;
	atomic_inc(&poller->count_poll_answers);
	list_add_tail(&card_desc->card_list, &poller->poll_results);
	spin_unlock_irqrestore(&xbus->lock, flags);
	/*
	 * wake_up only after exiting our critical section.
	 * We suspect that otherwise a spinlock nesting may occur
	 * and cause a panic (if spinlock debugging is compiled in).
	 */
	wake_up(&poller->wait_for_polls);
	return;
}

static void poller_destroy(struct xbus_poller *poller)
{
	if(!poller)
		return;
	if(poller->xbus) {
		DBG("%s: detach poller\n", poller->xbus->busname);
		poller->xbus->poller = NULL;
	}
	if (poller->wq) {
		DBG("%s: destroy workqueue\n", poller->xbus->busname);
		flush_workqueue(poller->wq);
		destroy_workqueue(poller->wq);
		poller->wq = NULL;
	}
	kfree(poller);
}

/*
 * Allocate a poller for the xbus including the nessessary workqueue.
 * May call blocking operations, but only briefly (as we are called
 * from xbus_new() which is called from khubd.
 */
static struct xbus_poller *poller_new(xbus_t *xbus)
{
	struct xbus_poller	*poller;

	BUG_ON(xbus->busname[0] == '\0');	/* No name? */
	BUG_ON(xbus->poller);			/* Hmmm... overrun pollers? */
	DBG("%s\n", xbus->busname);
	poller = kmalloc(sizeof(*poller), GFP_KERNEL);
	if(!poller)
		goto err;
	memset(poller, 0, sizeof(*poller));
	poller->xbus = xbus;
	xbus->poller = poller;
	/* poll related variables */
	atomic_set(&poller->count_poll_answers, 0);
	atomic_set(&poller->count_xpds_to_initialize, 0);
	atomic_set(&poller->count_xpds_initialized, 0);
	INIT_LIST_HEAD(&poller->poll_results);
	init_waitqueue_head(&poller->wait_for_polls);
	init_waitqueue_head(&poller->wait_for_xpd_initialization);
	poller->wq = create_singlethread_workqueue(xbus->busname);
	if(!poller->wq) {
		ERR("%s: Failed to create poller workqueue.\n", xbus->busname);
		goto err;
	}
	return poller;
err:
	poller_destroy(poller);
	return NULL;
}

/*
 * Sends an xbus_poll() work to the poller workqueue of the given xbus.
 */
static int poller_dispatch(xbus_t *xbus)
{
	struct xbus_poller	*poller = xbus->poller;

	if(!poller) {
		ERR("%s: missing poller\n", xbus->busname);
		return 0;
	}
	/* Initialize the work. (adapt to kernel API changes). */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
	INIT_WORK(&poller->xpds_init_work, xbus_poll);
#else
	INIT_WORK(&poller->xpds_init_work, xbus_poll, poller);
#endif
	/* Now send it */
	if(!queue_work(poller->wq, &poller->xpds_init_work)) {
		ERR("%s: Failed to queue xpd initialization work\n",
				xbus->busname);
		return 0;
	}
	return 1;
}

void xbus_activate(xbus_t *xbus)
{
	xbus_ops_t		*ops;
	struct xbus_poller	*poller;

	BUG_ON(!xbus);
	ops = xbus->ops;
	BUG_ON(!ops);
	poller = xbus->poller;
	BUG_ON(!poller);
	/* Sanity checks */
	BUG_ON(!ops->xframe_send);
	BUG_ON(!ops->xframe_new || !ops->xframe_free);
	xbus->hardware_exists = 1;
	DBG("Activating: %s\n", xbus->busname);
	/* Poll it */
	poller_dispatch(xbus);
}

void xbus_disconnect(xbus_t *xbus)
{
	int	i;

	BUG_ON(!xbus);
	DBG("%s\n", xbus->busname);
	xbus->hardware_exists = 0;
	for(i = 0; i < MAX_XPDS; i++) {
		xpd_t *xpd = xpd_of(xbus, i);
		if(!xpd)
			continue;
		if(xpd->id != i) {
			ERR("%s: BUG: xpd->id=%d != i=%d\n", __FUNCTION__, xpd->id, i);
			continue;
		}
		xpd_disconnect(xpd);
	}
	DBG("%s (deactivated)\n", xbus->busname);
	if(xbus->open_counter == 0) {
		xbus_remove(xbus);
	}
}

static xbus_t *xbus_alloc(void)
{
	unsigned long	flags;
	xbus_t	*xbus;
	int	i;

	xbus = kmalloc(sizeof(xbus_t), GFP_KERNEL);
	if(!xbus) {
		ERR("%s: out of memory\n", __FUNCTION__);
		return NULL;
	}
	memset(xbus, 0, sizeof(xbus_t));
	spin_lock_irqsave(&xbuses_lock, flags);
	for(i = 0; i < MAX_BUSES; i++)
		if(xbuses_array[i] == NULL)
			break;
	if(i >= MAX_BUSES) {
		ERR("%s: No free slot for new bus. i=%d\n", __FUNCTION__, i);
		kfree(xbus);
		return NULL;
	}
	/* Found empty slot */
	xbuses_array[i] = xbus;
	xbus->num = i;
	bus_count++;
	spin_unlock_irqrestore(&xbuses_lock, flags);
	return xbus;
}


static void xbus_free(xbus_t *xbus)
{
	unsigned long	flags;

	if(!xbus)
		return;
	spin_lock_irqsave(&xbuses_lock, flags);
	BUG_ON(!xbus_of(xbus->num));
	BUG_ON(xbus != xbus_of(xbus->num));
	xbuses_array[xbus->num] = NULL;
	bus_count--;
	spin_unlock_irqrestore(&xbuses_lock, flags);
#ifdef	XPP_DEBUGFS
	if(xbus->debugfs_dir) {
		if(xbus->debugfs_file) {
			DBG("Removing debugfs file for %s\n", xbus->busname);
			debugfs_remove(xbus->debugfs_file);
		}
		DBG("Removing debugfs directory for %s\n", xbus->busname);
		debugfs_remove(xbus->debugfs_dir);
	}
#endif
#ifdef CONFIG_PROC_FS
	if(xbus->proc_xbus_dir) {
		if(xbus->proc_xbus_summary) {
			DBG("Removing proc '%s' for %s\n", PROC_XBUS_SUMMARY, xbus->busname);
			remove_proc_entry(PROC_XBUS_SUMMARY, xbus->proc_xbus_dir);
			xbus->proc_xbus_summary = NULL;
		}
		if(xbus->proc_xbus_waitfor_xpds) {
			DBG("Removing proc '%s' for %s\n", PROC_XBUS_WAITFOR_XPDS, xbus->busname);
			remove_proc_entry(PROC_XBUS_WAITFOR_XPDS, xbus->proc_xbus_dir);
			xbus->proc_xbus_waitfor_xpds = NULL;
		}
#ifdef	PROTOCOL_DEBUG
		if(xbus->proc_xbus_command) {
			DBG("Removing proc '%s' for %s\n", PROC_XBUS_COMMAND, xbus->busname);
			remove_proc_entry(PROC_XBUS_COMMAND, xbus->proc_xbus_dir);
			xbus->proc_xbus_command = NULL;
		}
#endif
		DBG("Removing proc directory %s\n", xbus->busname);
		remove_proc_entry(xbus->busname, xpp_proc_toplevel);
		xbus->proc_xbus_dir = NULL;
	}
#endif
	device_remove_file(&xbus->the_bus, &dev_attr_status);
	device_remove_file(&xbus->the_bus, &dev_attr_connector);
	device_unregister(&xbus->the_bus);
	poller_destroy(xbus->poller);
	kfree(xbus);
}

static void xbus_release(struct device *dev)
{
	xbus_t	*xbus;

	BUG_ON(!dev);
	xbus = dev->driver_data;
	DBG("%s\n", xbus->busname);
}

xbus_t *xbus_new(xbus_ops_t *ops)
{
	int			err;
	xbus_t			*xbus = NULL;
	struct xbus_poller	*poller;

	BUG_ON(!ops);
	xbus = xbus_alloc();
	if(!xbus)
		return NULL;
	/* Init data structures */
	spin_lock_init(&xbus->lock);
	snprintf(xbus->busname, XBUS_NAMELEN, "XBUS-%02d", xbus->num);
	INFO("New xbus: %s\n", xbus->busname);
	init_waitqueue_head(&xbus->packet_cache_empty);
	atomic_set(&xbus->packet_counter, 0);
	xbus->min_tx_sync = INT_MAX;
	xbus->min_rx_sync = INT_MAX;

	xbus->num_xpds = 0;
	poller = poller_new(xbus);
	if(!poller) {
		ERR("Failed to allocate poller\n");
		xbus_free(xbus);
		return NULL;
	}

	init_rwsem(&xbus->in_use);
	xbus_reset_counters(xbus);

	/* Device-Model */
	snprintf(xbus->the_bus.bus_id, BUS_ID_SIZE, "xbus-%d", xbus->num);
	xbus->the_bus.driver_data = xbus;
	xbus->the_bus.release = xbus_release;

	err = device_register(&xbus->the_bus);
	if(err) {
		ERR("%s: device_register failed: %d\n", __FUNCTION__, err);
		goto nobus;
	}
	err = device_create_file(&xbus->the_bus, &dev_attr_connector);
	if(err) {
		ERR("%s: device_create_file failed: %d\n", __FUNCTION__, err);
		goto nobus;
	}
	err = device_create_file(&xbus->the_bus, &dev_attr_status);
	if(err) {
		ERR("%s: device_create_file failed: %d\n", __FUNCTION__, err);
		goto nobus;
	}

#ifdef CONFIG_PROC_FS
	DBG("Creating xbus proc directory %s.\n",xbus->busname);
	xbus->proc_xbus_dir = proc_mkdir(xbus->busname, xpp_proc_toplevel);
	if(!xbus->proc_xbus_dir) {
		ERR("Failed to create proc directory for xbus %s\n", xbus->busname);
		err = -EIO;
		goto nobus;
	}
	xbus->proc_xbus_summary = create_proc_read_entry(PROC_XBUS_SUMMARY, 0444, xbus->proc_xbus_dir,
			xbus_read_proc, xbus);
	if (!xbus->proc_xbus_summary) {
		ERR("Failed to create '%s' proc file for xbus %s\n", PROC_XBUS_SUMMARY, xbus->busname);
		err = -EIO;
		goto nobus;
	}
	xbus->proc_xbus_summary->owner = THIS_MODULE;
	xbus->proc_xbus_waitfor_xpds = create_proc_read_entry(PROC_XBUS_WAITFOR_XPDS, 0444, xbus->proc_xbus_dir,
			xbus_read_waitfor_xpds, xbus);
	if (!xbus->proc_xbus_waitfor_xpds) {
		ERR("Failed to create '%s' proc file for xbus %s\n", PROC_XBUS_WAITFOR_XPDS, xbus->busname);
		err = -EIO;
		goto nobus;
	}
	xbus->proc_xbus_waitfor_xpds->owner = THIS_MODULE;
#ifdef	PROTOCOL_DEBUG
	xbus->proc_xbus_command = create_proc_entry(PROC_XBUS_COMMAND, 0200, xbus->proc_xbus_dir);
	if (!xbus->proc_xbus_command) {
		ERR("Failed to create '%s' proc file for xbus %s\n", PROC_XBUS_COMMAND, xbus->busname);
		err = -EIO;
		goto nobus;
	}
	xbus->proc_xbus_command->write_proc = proc_xbus_command_write;
	xbus->proc_xbus_command->data = xbus;
	xbus->proc_xbus_command->owner = THIS_MODULE;
#endif
#endif
#ifdef	XPP_DEBUGFS
	xbus->debugfs_dir = debugfs_create_dir(xbus->busname, debugfs_root);
	if(!xbus->debugfs_dir) {
		ERR("Failed to create debugfs directory for %s\n", xbus->busname);
		goto nobus;
	}
	xbus->debugfs_file = debugfs_create_file("dchannel", S_IFREG|S_IRUGO|S_IWUSR, xbus->debugfs_dir, xbus, &debugfs_operations);
	if(!xbus->debugfs_file) {
		ERR("Failed to create dchannel file for %s\n", xbus->busname);
		goto nobus;
	}
#endif
	/* Sanity checks */
	if(!ops->xframe_send) {
		ERR("%s: missing mandatory handler: xframe_send\n", __FUNCTION__);
		goto nobus;
	}
	if(!ops->xframe_new || !ops->xframe_free) {
		NOTICE("%s: Using default packet allocators\n", __FUNCTION__);
		ops->xframe_new = xbus_xframe_new;
		ops->xframe_free = xbus_xframe_free;
	}

	xbus->ops = ops;
	return xbus;
nobus:
	xbus_free(xbus);
	return NULL;
}

void xbus_remove(xbus_t *xbus)
{
	int	i;
	int	ret;

	BUG_ON(!xbus);
	if(!xbus_of(xbus->num)) {
		DBG("XBUS #%d was already removed. Skip.\n", xbus->num);
		return;
	}
	DBG("%s\n", xbus->busname);

	/* Block until no one use */
	down_write(&xbus->in_use);

	INFO("Removing xbus(%d) %s\n", xbus->num, xbus->busname);
	for(i = 0; i < MAX_XPDS; i++) {
		xpd_t *xpd = xpd_of(xbus, i);

		if(xpd) {
			if(xpd->id != i) {
				ERR("%s: BUG: xpd->id=%d != i=%d\n", __FUNCTION__, xpd->id, i);
				continue;
			}
			DBG("  Removing xpd id=%d\n", xpd->id);
			xpd_remove(xpd);
		}
		xbus->xpds[i] = NULL;
	}
	ret = wait_event_interruptible(xbus->packet_cache_empty, 
			atomic_read(&xbus->packet_counter) == 0);
	if(ret) {
		ERR("waiting for packet_cache_empty interrupted!!!\n");
	}
	xbus_free(xbus);
}

/*------------------------- Proc handling --------------------------*/

void xbus_reset_counters(xbus_t *xbus)
{
	int	i;

	DBG("Reseting counters of %s\n", xbus->busname);
	for(i = 0; i < XBUS_COUNTER_MAX; i++) {
		xbus->counters[i] = 0;
	}
}

#if CONFIG_PROC_FS
static int xbus_read_proc(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	xbus_t			*xbus = data;
	struct xbus_poller	*poller;
	unsigned long		flags;
	int			len = 0;
	int			i;

	if(!xbus)
		goto out;
	spin_lock_irqsave(&xbus->lock, flags);
	poller = xbus->poller;

	len += sprintf(page + len, "%s: CONNECTOR=%s STATUS=%s bus_type=%d\n",
			xbus->busname,
			xbus->busdesc,
			(xbus->hardware_exists) ? "connected" : "missing",
			xbus->bus_type
		      );
	len += sprintf(page + len, "POLLS: %d/%d\n",
			atomic_read(&poller->count_poll_answers), MAX_XPDS);
	len += sprintf(page + len, "XPDS_READY: %d/%d\n",
			atomic_read(&poller->count_xpds_initialized),
			atomic_read(&poller->count_xpds_to_initialize));
	len += sprintf(page + len, "\nmax_packet_size=%d open_counter=%d packet_count=%d\n",
			xbus->max_packet_size,
			xbus->open_counter,
			atomic_read(&xbus->packet_counter)
		      );
	len += sprintf(page + len, "\nPLL DRIFT=%d\n", xbus->sync_adjustment);
	len += sprintf(page + len, "PCM Metrices:\n");
	len += sprintf(page + len, "\tPCM TX: min=%ld  max=%ld\n",
				xbus->min_tx_sync, xbus->max_tx_sync);
	len += sprintf(page + len, "\tPCM RX: min=%ld  max=%ld\n",
				xbus->min_rx_sync, xbus->max_rx_sync);
	len += sprintf(page + len, "COUNTERS:\n");
	for(i = 0; i < XBUS_COUNTER_MAX; i++) {
		len += sprintf(page + len, "\t%-15s = %d\n",
				xbus_counters[i].name, xbus->counters[i]);
	}
	len += sprintf(page + len, "<-- len=%d\n", len);
	spin_unlock_irqrestore(&xbus->lock, flags);
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

static int xbus_read_waitfor_xpds(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	int		len = 0;
	unsigned long	flags;
	xbus_t		*xbus = data;
	struct xbus_poller	*poller;
	int		ret;

	if(!xbus)
		goto out;
	poller = xbus->poller;
	DBG("%s: Waiting for card initialization of %d XPD's max %d seconds\n", xbus->busname, MAX_XPDS, INITIALIZATION_TIMEOUT/HZ);
	/*
	 * xbus_poll sets count_xpds_to_initialize only when polling is finished.
	 * To prevent race conditions we test both:
	 *   - It is none zero -- meaning we already have the poll results.
	 *   - And all units have finished initialization.
	 */
	ret = wait_event_interruptible_timeout(poller->wait_for_xpd_initialization,
			atomic_read(&poller->count_xpds_to_initialize) &&
			atomic_read(&poller->count_xpds_initialized) >= atomic_read(&poller->count_xpds_to_initialize),
			INITIALIZATION_TIMEOUT);
	if(ret == 0) {
		ERR("%s: Card Initialization Timeout\n", xbus->busname);
		return ret;
	} else if(ret < 0) {
		ERR("%s: Card Initialization Interrupted %d\n", xbus->busname, ret);
		return ret;
	}
	DBG("%s: Finished initialization of %d XPD's in %d seconds.\n", xbus->busname, MAX_XPDS, (INITIALIZATION_TIMEOUT - ret)/HZ);
	spin_lock_irqsave(&xbus->lock, flags);
	len += sprintf(page + len, "XPDS_READY: %s: %d/%d\n",
			xbus->busname,
			atomic_read(&poller->count_xpds_initialized),
			atomic_read(&poller->count_xpds_to_initialize));
	spin_unlock_irqrestore(&xbus->lock, flags);
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

#ifdef	PROTOCOL_DEBUG
static int proc_xbus_command_write(struct file *file, const char __user *buffer, unsigned long count, void *data)
{
	char			buf[MAX_PROC_WRITE];
	xbus_t			*xbus = data;
	xpacket_t		*pack;
	char			*p;
	byte			*pack_start;
	byte			*q;
	xframe_t		*xframe;

	if(count >= MAX_PROC_WRITE) {
		ERR("%s: line too long\n", __FUNCTION__);
		return -EFBIG;
	}
	if(copy_from_user(buf, buffer, count))
		return -EINVAL;
	buf[count] = '\0';
	q = pack_start = buf;
	for(p = buf; *p;) {
		int	val;
		char	hexdigit[3];

		while(*p && isspace(*p))	// skip whitespace
			p++;
		if(!(*p))
			break;
		if(!isxdigit(*p)) {
			ERR("%s: %s: bad hex value ASCII='0x%X' at position %d\n",
					__FUNCTION__, xbus->busname, *p, p - buf);
			return -EINVAL;
		}
		hexdigit[0] = *p++;
		hexdigit[1] = '\0';
		hexdigit[2] = '\0';
		if(isxdigit(*p))
			hexdigit[1] = *p++;
		if(sscanf(hexdigit, "%2X", &val) != 1) {
			ERR("%s: %s: bad hex value '%s' at position %d\n",
					__FUNCTION__, xbus->busname, hexdigit, p - buf);
			return -EINVAL;
		}
		*q++ = val;
		// DBG("%s: %s: '%s' val=%d\n", __FUNCTION__, xbus->busname, hexdigit, val);
	}
	xframe = xbus->ops->xframe_new(xbus, GFP_KERNEL);
	if(!xframe)
		return -ENOMEM;
	pack = xframe_next_packet(xframe, q - pack_start);
	if(!pack) {
		xbus->ops->xframe_free(xbus, xframe);
		return -ENOMEM;
	}
	memcpy(pack, pack_start, q - pack_start);		/* FRAMES: checksum? */
	xframe_send(xbus, xframe);
	return count;
}
#endif


static int read_proc_xbuses(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	int len = 0;
	unsigned long flags;
	int i;

	spin_lock_irqsave(&xbuses_lock, flags);
	for(i = 0; i < MAX_BUSES; i++) {
		xbus_t *xbus = xbus_of(i);

		if(xbus) {
			len += sprintf(page + len, "%s: CONNECTOR=%s STATUS=%s bus_type=%d\n",
					xbus->busname,
					xbus->busdesc,
					(xbus->hardware_exists) ? "connected" : "missing",
					xbus->bus_type
				      );
		}
	}
#if 0
	len += sprintf(page + len, "<-- len=%d\n", len);
#endif
	spin_unlock_irqrestore(&xbuses_lock, flags);
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

/*------------------------- Initialization -------------------------*/

static DEVICE_ATTR_FUNC(connector_show, dev, buf)
{
	xbus_t	*xbus;
	int	ret;

	xbus = dev->driver_data;
	ret = snprintf(buf, PAGE_SIZE, "%s\n", xbus->busdesc);
	return ret;
}

static DEVICE_ATTR_FUNC(status_show, dev, buf)
{
	xbus_t	*xbus;
	int	ret;

	xbus = dev->driver_data;
	ret = snprintf(buf, PAGE_SIZE, "%s\n", (xbus->hardware_exists)?"connected":"missing");
	return ret;
}

static int xbus_match(struct device *dev, struct device_driver *driver)
{
	DBG("dev->bus_id = %s, driver->name = %s\n", dev->bus_id, driver->name);
	return strncmp(dev->bus_id, driver->name, strlen(driver->name)) == 0;
}

#if 0
/* Hotplug replaced with uevent in 2.6.16 */
static int xbus_hotplug(struct device *device, char **envp, int envnum, char *buff, int bufsize)
{
	envp[0] = buff;
	if(snprintf(buff, bufsize, "XBUS_VERSION=%s", revision) >= bufsize)
		return -ENOMEM;
	envp[1] = NULL;
	return 0;
}
#endif

struct bus_type	xbus_bus_type = {
	.name = "xbus",
	.match = xbus_match,
/* FIXME: Hotplug replaced with uevent in 2.6.16 */
#if 0
	.hotplug = xbus_hotplug,
#endif
};

static void xbus_core_cleanup(void)
{
#ifdef	XPP_DEBUGFS
	if(debugfs_root) {
		DBG("Removing xpp from debugfs\n");
		debugfs_remove(debugfs_root);
	}
#endif
#ifdef CONFIG_PROC_FS
	if(proc_xbuses) {
		DBG("Removing " PROC_XBUSES " from proc\n");
		remove_proc_entry(PROC_XBUSES, xpp_proc_toplevel);
		proc_xbuses = NULL;
	}
#endif
	if(xframes_cache)
		kmem_cache_destroy(xframes_cache);
}

int __init xbus_core_init(void)
{
	int	ret;

#ifdef PROTOCOL_DEBUG
	INFO("FEATURE: %s with PROTOCOL_DEBUG\n", THIS_MODULE->name);
#endif
#ifdef	XPP_DEBUGFS
	INFO("FEATURE: %s with XPP_DEBUGFS support\n", THIS_MODULE->name);
#endif
	xframes_cache = kmem_cache_create("xpp_frames",
			sizeof(xframe_t) + XFRAME_DATASIZE,
			0, 0,
			NULL, NULL);
	if(!xframes_cache) {
		return -ENOMEM;
	}
#ifdef CONFIG_PROC_FS
	proc_xbuses = create_proc_read_entry(PROC_XBUSES, 0444, xpp_proc_toplevel, read_proc_xbuses, NULL);
	if (!proc_xbuses) {
		ERR("Failed to create proc file %s\n", PROC_XBUSES);
		ret = -EFAULT;
		goto err;
	}
	proc_xbuses->owner = THIS_MODULE;
#endif
#ifdef	XPP_DEBUGFS
	DBG("Creating debugfs xpp root\n");
	debugfs_root = debugfs_create_dir("xpp", NULL);
	if(!debugfs_root) {
		ERR("Failed to create debugfs root\n");
		ret = -EFAULT;
		goto err;
	}
#endif
	ret = bus_register(&xbus_bus_type);
	if(ret) {
		ERR("%s: bus_register failed. Error number %d", __FUNCTION__, ret);
		goto err;
	}
	return 0;
err:
	xbus_core_cleanup();
	return ret;
}


void __exit xbus_core_shutdown(void)
{
	int		i;

	for(i = 0; i < MAX_BUSES; i++) {
		xbus_t	*xbus = xbus_of(i);
		if(xbus)
			xbus_remove(xbus);
	}
	BUG_ON(bus_count);
	bus_unregister(&xbus_bus_type);
	xbus_core_cleanup();
}

EXPORT_SYMBOL(xpd_of);
EXPORT_SYMBOL(xbus_new);
EXPORT_SYMBOL(xbus_remove);
EXPORT_SYMBOL(xbus_activate);
EXPORT_SYMBOL(xbus_disconnect);
EXPORT_SYMBOL(xbus_reset_counters);
EXPORT_SYMBOL(xframe_next_packet);
EXPORT_SYMBOL(dump_xframe);
EXPORT_SYMBOL(xbus_poller_notify);
#ifdef XPP_DEBUGFS
EXPORT_SYMBOL(xbus_log);
#endif
