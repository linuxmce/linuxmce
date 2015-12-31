#ifndef	ZAP_DEBUG_H
#define	ZAP_DEBUG_H
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

/* Debugging Macros */
#define	DBG(fmt, ...)	\
	((void)((print_dbg) && printk(KERN_DEBUG "DBG-%s: %s: " fmt, \
		THIS_MODULE->name, __FUNCTION__, ## __VA_ARGS__)))
#define	INFO(fmt, ...)	printk(KERN_INFO "INFO-%s: " fmt, THIS_MODULE->name, ## __VA_ARGS__)
#define	NOTICE(fmt, ...)	printk(KERN_NOTICE "NOTICE-%s: " fmt, THIS_MODULE->name, ## __VA_ARGS__)
#define	WARN(fmt, ...)	printk(KERN_WARNING "WARN-%s: %s: " fmt, THIS_MODULE->name, __FUNCTION__, ## __VA_ARGS__)
#define	ERR(fmt, ...)	printk(KERN_ERR "ERR-%s: " fmt, THIS_MODULE->name, ## __VA_ARGS__)

/*
 * Bits for print_dbg
 */
#define	DBG_DEFAULT	0x01
#define	DBG_PCM		0x02

void dump_poll(int print_dbg, const char *msg, int poll);

static inline char *rxsig2str(zt_rxsig_t sig)
{
	switch(sig) {
		case ZT_RXSIG_ONHOOK:	return "ONHOOK";
		case ZT_RXSIG_OFFHOOK:	return "OFFHOOK";
		case ZT_RXSIG_START:	return "START";
		case ZT_RXSIG_RING:	return "RING";
		case ZT_RXSIG_INITIAL:	return "INITIAL";
	}
	return "Unknown rxsig";
}

static inline char *txsig2str(zt_txsig_t sig)
{
	switch(sig) {
		case ZT_TXSIG_ONHOOK:	return "TXSIG_ONHOOK";
		case ZT_TXSIG_OFFHOOK:	return "TXSIG_OFFHOOK";
		case ZT_TXSIG_START:	return "TXSIG_START";
		case ZT_TXSIG_KEWL:	return "TXSIG_KEWL";				/* Drop battery if possible */
	}
	return "Unknown txsig";
}

static inline char *event2str(int event)
{
	switch(event) {
		case ZT_EVENT_NONE:		return "NONE";
		case ZT_EVENT_ONHOOK:		return "ONHOOK";
		case ZT_EVENT_RINGOFFHOOK:	return "RINGOFFHOOK";
		case ZT_EVENT_WINKFLASH:	return "WINKFLASH";
		case ZT_EVENT_ALARM:		return "ALARM";
		case ZT_EVENT_NOALARM:		return "NOALARM";
		case ZT_EVENT_ABORT:		return "ABORT";
		case ZT_EVENT_OVERRUN:		return "OVERRUN";
		case ZT_EVENT_BADFCS:		return "BADFCS";
		case ZT_EVENT_DIALCOMPLETE:	return "DIALCOMPLETE";
		case ZT_EVENT_RINGERON:		return "RINGERON";
		case ZT_EVENT_RINGEROFF:	return "RINGEROFF";
		case ZT_EVENT_HOOKCOMPLETE:	return "HOOKCOMPLETE";
		case ZT_EVENT_BITSCHANGED:	return "BITSCHANGED";
		case ZT_EVENT_PULSE_START:	return "PULSE_START";
		case ZT_EVENT_TIMER_EXPIRED:	return "TIMER_EXPIRED";
		case ZT_EVENT_TIMER_PING:	return "TIMER_PING";
		case ZT_EVENT_POLARITY:		return "POLARITY";
	}
	return "Unknown event";
}

static inline char *hookstate2str(int hookstate)
{
	switch(hookstate) {
		case ZT_ONHOOK:		return "ZT_ONHOOK";
		case ZT_START:		return "ZT_START";
		case ZT_OFFHOOK:	return "ZT_OFFHOOK";
		case ZT_WINK:		return "ZT_WINK";
		case ZT_FLASH:		return "ZT_FLASH";
		case ZT_RING:		return "ZT_RING";
		case ZT_RINGOFF:	return "ZT_RINGOFF";
	}
	return "Unknown hookstate";
}

/* From zaptel.c */
static inline char *sig2str(int sig)
{
	switch (sig) {
		case ZT_SIG_FXSLS:	return "FXSLS";
		case ZT_SIG_FXSKS:	return "FXSKS";
		case ZT_SIG_FXSGS:	return "FXSGS";
		case ZT_SIG_FXOLS:	return "FXOLS";
		case ZT_SIG_FXOKS:	return "FXOKS";
		case ZT_SIG_FXOGS:	return "FXOGS";
		case ZT_SIG_EM:		return "E&M";
		case ZT_SIG_EM_E1:	return "E&M-E1";
		case ZT_SIG_CLEAR:	return "Clear";
		case ZT_SIG_HDLCRAW:	return "HDLCRAW";
		case ZT_SIG_HDLCFCS:	return "HDLCFCS";
		case ZT_SIG_HDLCNET:	return "HDLCNET";
		case ZT_SIG_SLAVE:	return "Slave";
		case ZT_SIG_CAS:	return "CAS";
		case ZT_SIG_DACS:	return "DACS";
		case ZT_SIG_DACS_RBS:	return "DACS+RBS";
		case ZT_SIG_SF:		return "SF (ToneOnly)";
		case ZT_SIG_NONE:
					break;
	}
	return "Unconfigured";
}

#endif	/* ZAP_DEBUG_H */
