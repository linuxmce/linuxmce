/*
 * Configuration program for Zapata Telephony Interface
 *
 * Written by Mark Spencer <markster@digium.com>
 * Based on previous works, designs, and architectures conceived and
 * written by Jim Dixon <jim@lambdatel.com>.
 *
 * Copyright (C) 2001 Jim Dixon / Zapata Telephony.
 * Copyright (C) 2001 Linux Support Services, Inc.
 *
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under thet erms of the GNU General Public License as published by
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
 * Primary Author: Mark Spencer <markster@digium.com>
 *
 */

#include <stdio.h> 
#include <getopt.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <newt.h>
#ifdef STANDALONE_ZAPATA
#include "zaptel.h"
#include "tonezone.h"
#else
#include <zaptel/zaptel.h>
#include <zaptel/tonezone.h>
#endif

static int ctl = -1;
static int span_max_chan_pos;

static ZT_SPANINFO s[ZT_MAX_SPANS];

static char *zt_txlevelnames[] = {
"0 db (CSU)/0-133 feet (DSX-1)",
"133-266 feet (DSX-1)",
"266-399 feet (DSX-1)",
"399-533 feet (DSX-1)",
"533-655 feet (DSX-1)",
"-7.5db (CSU)",
"-15db (CSU)",
"-22.5db (CSU)"
} ;

static char *alarmstr(int span)
{
	static char alarms[80];
	strcpy(alarms, "");
	if (s[span].alarms > 0) {
		if (s[span].alarms & ZT_ALARM_BLUE)
			strcat(alarms,"Blue Alarm/");
		if (s[span].alarms & ZT_ALARM_YELLOW)
			strcat(alarms, "Yellow Alarm/");
		if (s[span].alarms & ZT_ALARM_RED)
			strcat(alarms, "Red Alarm/");
		if (s[span].alarms & ZT_ALARM_LOOPBACK)
			strcat(alarms,"Loopback/");
		if (s[span].alarms & ZT_ALARM_RECOVER)
			strcat(alarms,"Recovering/");
		if (s[span].alarms & ZT_ALARM_NOTOPEN)
			strcat(alarms, "Not Open/");
		if (!strlen(alarms))
			strcat(alarms, "<unknown>/");
		if (strlen(alarms)) {
			/* Strip trailing / */
			alarms[strlen(alarms)-1]='\0';
		}
	} else
		strcpy(alarms, "No alarms.");
	return alarms;
}

static char *getalarms(int span, int err)
{
	int res;
	static char tmp[256];
	char alarms[50];
	s[span].spanno = span;
	res = ioctl(ctl, ZT_SPANSTAT, &s[span]);
	if (res) {
		if (err)
			fprintf(stderr, "Unable to get span info on span %d: %s\n", span, strerror(errno)); 
		return NULL;
	}
	strcpy(alarms, "");
	if (s[span].alarms > 0) {
		if (s[span].alarms & ZT_ALARM_BLUE)
			strcat(alarms,"BLU/");
		if (s[span].alarms & ZT_ALARM_YELLOW)
			strcat(alarms, "YEL/");
		if (s[span].alarms & ZT_ALARM_RED)
			strcat(alarms, "RED/");
		if (s[span].alarms & ZT_ALARM_LOOPBACK)
			strcat(alarms,"LB/");
		if (s[span].alarms & ZT_ALARM_RECOVER)
			strcat(alarms,"REC/");
		if (s[span].alarms & ZT_ALARM_NOTOPEN)
			strcat(alarms, "NOP/");
		if (!strlen(alarms))
			strcat(alarms, "UUU/");
		if (strlen(alarms)) {
			/* Strip trailing / */
			alarms[strlen(alarms)-1]='\0';
		}
	} else {
		if (s[span].numchans)
			strcpy(alarms, "OK");
		else
			strcpy(alarms, "UNCONFIGURED");
	}
		
	snprintf(tmp, sizeof(tmp), "%-15s %s", alarms, s[span].desc);
	return tmp;
}

static void add_cards(newtComponent spans)
{
	int x;
	char *s;
	void *prev=NULL;
	
	if (spans)
		prev = newtListboxGetCurrent(spans);
	newtListboxClear(spans);
	for (x=0;x<ZT_MAX_SPANS;x++) {
		s = getalarms(x, 0);
		if (s && spans) {
			/* Found one! */
			newtListboxAppendEntry(spans, s, (void *)(long)x);
		}
	}
	if (spans)
		newtListboxSetCurrentByKey(spans, prev);
	
}

static void sel_callback(newtComponent c, void *cbdata)
{
	int span;
	char info[256];
	char info2[256];
	cbdata = newtListboxGetCurrent(c);
	if (cbdata) {
		span = (long)(cbdata);
		snprintf(info, sizeof (info), "Span %d: %d total channels, %d configured", span, s[span].totalchans, s[span].numchans);
		snprintf(info2, sizeof(info2), "%-59s F1=Details F10=Quit", info);
	} else {
		span = -1;
		strcpy(info, "There are no zaptel spans on this system.");
		snprintf(info2, sizeof(info2), "%-59s            F10=Quit", info);
	}
	newtPopHelpLine();
	newtPushHelpLine(info2);
}

static void show_bits(int span, newtComponent bitbox, newtComponent inuse, newtComponent levels, newtComponent bpvcount,
						newtComponent alarms, newtComponent syncsrc, newtComponent irqmisses)
{
	ZT_PARAMS zp;
	int x;
	int res;
	char c;
	char tabits[80];
	char tbbits[80];
	char tcbits[80];
	char tdbits[80];
	char rabits[80];
	char rbbits[80];
	char rcbits[80];
	char rdbits[80];
	char tmp[1024];
	
	int use = 0;
	
	memset(tabits,0, sizeof(tabits));
	memset(tbbits,0, sizeof(tbbits));
	memset(rabits,0, sizeof(rabits));
	memset(rbbits,0, sizeof(rbbits));
	memset(tcbits,0, sizeof(tcbits));
	memset(tdbits,0, sizeof(tdbits));
	memset(rcbits,0, sizeof(rcbits));
	memset(rdbits,0, sizeof(rdbits));
	memset(tabits,32, span_max_chan_pos);
	memset(tbbits,32, span_max_chan_pos);
	memset(rabits,32, span_max_chan_pos);
	memset(rbbits,32, span_max_chan_pos);
	memset(tcbits,32, span_max_chan_pos);
	memset(tdbits,32, span_max_chan_pos);
	memset(rcbits,32, span_max_chan_pos);
	memset(rdbits,32, span_max_chan_pos);
	
	for (x=0;x<ZT_MAX_CHANNELS;x++) {
		memset(&zp, 0, sizeof(zp));
		zp.channo = x;
		res = ioctl(ctl, ZT_GET_PARAMS, &zp);
		if (!res) {
			if (zp.spanno == span) {
				if (zp.sigtype && (zp.rxbits > -1)) {
					if (zp.rxbits & ZT_ABIT)
						rabits[zp.chanpos - 1] = '1';
					else
						rabits[zp.chanpos - 1] = '0';
					if (zp.rxbits & ZT_BBIT)
						rbbits[zp.chanpos - 1] = '1';
					else
						rbbits[zp.chanpos - 1] = '0';

					if (zp.rxbits & ZT_CBIT)
						rcbits[zp.chanpos - 1] = '1';
					else
						rcbits[zp.chanpos - 1] = '0';
					if (zp.rxbits & ZT_DBIT)
						rdbits[zp.chanpos - 1] = '1';
					else
						rdbits[zp.chanpos - 1] = '0';

					if (zp.txbits & ZT_ABIT)
						tabits[zp.chanpos - 1] = '1';
					else
						tabits[zp.chanpos - 1] = '0';
					if (zp.txbits & ZT_BBIT)
						tbbits[zp.chanpos - 1] = '1';
					else
						tbbits[zp.chanpos - 1] = '0';
					if (zp.txbits & ZT_CBIT)
						tcbits[zp.chanpos - 1] = '1';
					else
						tcbits[zp.chanpos - 1] = '0';
					if (zp.txbits & ZT_DBIT)
						tdbits[zp.chanpos - 1] = '1';
					else
						tdbits[zp.chanpos - 1] = '0';
				} else {
					c = '-';
					if (!zp.sigtype)
						c = ' ';
					tabits[zp.chanpos - 1] = c;
					tbbits[zp.chanpos - 1] = c;
					tcbits[zp.chanpos - 1] = c;
					tdbits[zp.chanpos - 1] = c;
					rabits[zp.chanpos - 1] = c;
					rbbits[zp.chanpos - 1] = c;
					rcbits[zp.chanpos - 1] = c;
					rdbits[zp.chanpos - 1] = c;
				}
				if (zp.rxisoffhook)
					use++;
			}
		}
	}
	snprintf(tmp, sizeof(tmp), "%s\n%s\n%s\n%s\n\n%s\n%s\n%s\n%s", tabits, tbbits,tcbits,tdbits,rabits,rbbits,rcbits,rdbits);
	newtTextboxSetText(bitbox, tmp);	
	sprintf(tmp, "%3d/%3d/%3d", s[span].totalchans, s[span].numchans, use);
	newtTextboxSetText(inuse, tmp);
	sprintf(tmp, "%s/", zt_txlevelnames[s[span].txlevel]);
	strcat(tmp, zt_txlevelnames[s[span].rxlevel]);
	sprintf(tmp, "%3d/%3d", s[span].txlevel, s[span].rxlevel);
	newtTextboxSetText(levels, tmp);
	sprintf(tmp, "%7d", s[span].bpvcount);
	newtTextboxSetText(bpvcount, tmp);
	sprintf(tmp, "%7d", s[span].irqmisses);
	newtTextboxSetText(irqmisses, tmp);
	newtTextboxSetText(alarms, alarmstr(span));	
	if (s[span].syncsrc > 0)
		strcpy(tmp, s[s[span].syncsrc].desc);
	else
		strcpy(tmp, "Internally clocked");
	newtTextboxSetText(syncsrc, tmp);
	
	
}

static void do_loop(int span, int looped)
{
	newtComponent form;
	newtComponent label;
	char s1[256];
	struct zt_maintinfo m;
	int res;
	struct newtExitStruct es;

	newtOpenWindow(20,12,40,4, s[span].desc);

	form = newtForm(NULL, NULL, 0);
	m.spanno = span;
	if (looped) {
		snprintf(s1, sizeof(s1), "Looping UP span %d...\n", span);
		m.command = ZT_MAINT_LOOPUP;
	} else {
		snprintf(s1, sizeof(s1), "Looping DOWN span %d...\n", span);
		m.command = ZT_MAINT_LOOPDOWN;
	}

	label = newtLabel(3,1,s1);
	newtFormAddComponent(form, label);
	newtPushHelpLine("Please wait...");

	newtFormSetTimer(form, 200);
	newtFormRun(form, &es);
	res = ioctl(ctl, ZT_MAINT, &m);
	newtFormDestroy(form);
	newtPopWindow();
	newtPopHelpLine();
}

static newtComponent spans;
static void show_span(int span)
{
	newtComponent form;
	newtComponent back;
	newtComponent loop;
	newtComponent label;
	newtComponent bitbox;
	newtComponent inuse;
	newtComponent levels;
	newtComponent bpvcount;
	newtComponent alarms;
	newtComponent syncsrc;
	newtComponent irqmisses;
	
	char s1[] = "         1111111111222222222233";
	char s2[] = "1234567890123456789012345678901";
	int x;
	int looped = 0;
	struct newtExitStruct es;

	void *ss;
	char info2[256];

	if (span < 0) {
		/* Display info on a span */
		ss = newtListboxGetCurrent(spans);
		if (ss) {
			span = (long)(ss);
		}
	}

	snprintf(info2, sizeof(info2), "%-59s            F10=Back", s[span].desc);
	newtOpenWindow(10,2,60,20, s[span].desc);
	newtPushHelpLine(info2);

	back = newtButton(48,8,"Back");
	loop = newtButton(48,14,"Loop");
	form = newtForm(NULL, NULL, 0);

	newtFormAddComponents(form, back, loop, NULL);

	span_max_chan_pos = s[span].totalchans;
	for (x=0;x<ZT_MAX_CHANNELS;x++) {
		ZT_PARAMS zp;
		int res;
		memset(&zp, 0, sizeof(zp));
		zp.channo = x;
		res = ioctl(ctl, ZT_GET_PARAMS, &zp);
		if (!res && zp.spanno == span && zp.chanpos > span_max_chan_pos )
			span_max_chan_pos = zp.chanpos;
	}

	if (span_max_chan_pos > 32)
		span_max_chan_pos = 32;

	s1[span_max_chan_pos] = '\0';
	s2[span_max_chan_pos] = '\0';

	bitbox = newtTextbox(8,10,span_max_chan_pos,9,0);
	newtFormAddComponent(form, bitbox);

	label = newtLabel(8,8,s1);
	newtFormAddComponent(form, label);

	label = newtLabel(8,9,s2);
	newtFormAddComponent(form, label);

	newtFormAddHotKey(form, NEWT_KEY_F10);
	newtFormSetTimer(form, 200);

	label = newtLabel(4,10,"TxA");
	newtFormAddComponent(form, label);

	label = newtLabel(4,11,"TxB");
	newtFormAddComponent(form, label);

	label = newtLabel(4,12,"TxC");
	newtFormAddComponent(form, label);

	label = newtLabel(4,13,"TxD");
	newtFormAddComponent(form, label);

	label = newtLabel(4,15,"RxA");
	newtFormAddComponent(form, label);

	label = newtLabel(4,16,"RxB");
	newtFormAddComponent(form, label);

	label = newtLabel(4,17,"RxC");
	newtFormAddComponent(form, label);

	label = newtLabel(4,18,"RxD");
	newtFormAddComponent(form, label);
	
	
	label = newtLabel(4,7,"Total/Conf/Act: ");
	newtFormAddComponent(form, label);
	
	inuse = newtTextbox(24,7,12,1,0);
	newtFormAddComponent(form, inuse);

	label = newtLabel(4,6,"Tx/Rx Levels: ");
	newtFormAddComponent(form, label);

	levels = newtTextbox(24,6,30,1,0);
	newtFormAddComponent(form, levels);
	
	label = newtLabel(4,5,"Bipolar Viol: ");
	newtFormAddComponent(form, label);

	bpvcount = newtTextbox(24,5,30,1,0);
	newtFormAddComponent(form, bpvcount);
	
	label = newtLabel(4,4,"IRQ Misses: ");
	newtFormAddComponent(form, label);

	irqmisses = newtTextbox(24,4,30,1,0);
	newtFormAddComponent(form, irqmisses);

	label = newtLabel(4,3,"Sync Source: ");
	newtFormAddComponent(form, label);

	syncsrc = newtTextbox(24,3,30,1,0);
	newtFormAddComponent(form, syncsrc);

	label = newtLabel(4,2,"Current Alarms: ");
	newtFormAddComponent(form, label);

	alarms = newtTextbox(24,2,30,1,0);
	newtFormAddComponent(form, alarms);
	
	for(;;) {
		/* Wait for user to select something */
		do {
			add_cards(NULL);
			show_bits(span, bitbox, inuse, levels, bpvcount, alarms, syncsrc, irqmisses);
			newtFormRun(form, &es);
		} while(es.reason == NEWT_EXIT_TIMER);
		switch(es.reason) {
		case NEWT_EXIT_COMPONENT:
			if (es.u.co == loop) {
				looped = !looped;
				do_loop(span, looped);
				newtFormSetTimer(form, 200);
			}
			if (es.u.co == back) {
				goto out;
			}
			break;
		case NEWT_EXIT_HOTKEY:
			switch(es.u.key) {
#if 0
			case NEWT_KEY_F1:
				show_span(-1);
				break;
#endif				
			case NEWT_KEY_F10:
				goto out;
			}
			break;
		default:
			break;
		}
	}	

out:	
	newtFormDestroy(form);
	newtPopWindow();
	newtPopHelpLine();
	span_max_chan_pos = 0;
}

static void show_spans(void)
{
	newtComponent form;
	newtComponent quit;
	newtComponent label;
	newtComponent sel;

	
	struct newtExitStruct es;
	
	
	quit = newtButton(50,14,"Quit");
	sel = newtButton(10,14,"Select");
	
	spans = newtListbox(5, 2, 10, NEWT_FLAG_SCROLL);
	newtListboxSetWidth(spans, 65);
	
	label = newtLabel(5,1,"Alarms          Span");
	
	newtCenteredWindow(72,18, "Zapata Telephony Interfaces");
	form = newtForm(NULL, NULL, 0);
	
	newtFormSetTimer(form, 200);
	
	newtFormAddComponents(form, spans, sel, quit, label, NULL);

	newtComponentAddCallback(spans, sel_callback, NULL);

	newtFormAddHotKey(form, NEWT_KEY_F1);
	newtFormAddHotKey(form, NEWT_KEY_F10);
	
	for(;;) {
		/* Wait for user to select something */
		do {
			add_cards(spans);
			newtFormRun(form, &es);
		} while(es.reason == NEWT_EXIT_TIMER);

		switch(es.reason) {
		case NEWT_EXIT_COMPONENT:
			if (es.u.co == quit) {
				/* Quit if appropriate */
				newtFormDestroy(form);
				return;
			} else if (es.u.co == sel) {
				show_span(-1);
			}
			break;
		case NEWT_EXIT_HOTKEY:
			switch(es.u.key) {
			case NEWT_KEY_F1:
				show_span(-1);
				break;
			case NEWT_KEY_F10:
				newtFormDestroy(form);
				return;
			}
			break;
		default:
			break;
		}
	}
}

static void cleanup(void)
{
	newtPopWindow();
}

int main(int argc, char *argv[])
{

	ctl = open("/dev/zap/ctl", O_RDWR);
	if (ctl < 0) {
		fprintf(stderr, "Unable to open /dev/zap/ctl: %s\n", strerror(errno));
		exit(1);
	}
	newtInit();
	newtCls();
	
	newtDrawRootText(0,0,"Zaptel Tool (C)2002 Linux Support Services, Inc.");
	newtPushHelpLine("Welcome to the Zaptel Tool!");
	show_spans();
	cleanup();
	newtFinished();
	return 0;
}
