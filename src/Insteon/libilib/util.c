/*
 * $RCSfile: util.c,v $  $Revision: 1.2 $  $Name:  $
 * $Id: util.c,v 1.2 2006/12/16 00:18:32 bpaauwe Exp $
 * $Author: bpaauwe $
 * $Date: 2006/12/16 00:18:32 $
 * ----------------------------------------------------------------------------
 *
 *  Copyright (c) Bob Paauwe (2006)
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the Free
 *  Software Foundation; either version 2 of the License, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 *  more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * ----------------------------------------------------------------------------
 *
 *  Description:
 *    Utilitiy functions.
 * ----------------------------------------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <memory.h>
#include <ctype.h>
#include <time.h>

int alpha_to_i(char *str);
unsigned char *str_id_to_hex(char *device_id);
unsigned char atoh(char h);
int ahtoi(char *h);
int getline(FILE *fp, char *s, int lim);


static char *commands[] = {
	"RES1",
	"ASSIGN_TO_GROUP",
	"DELETE_FROM_GROUP",
	"READ_CONNECTIONS",
	"READ_DEVICE_DATA",
	"FACTORY_RESET",
	"DEVICE_RESET",
	"TERMINATE_DOWNLOAD",
	"zz",
	"ENTER_MASTER_PRG",
	"ENTER_SLAVE_PRG",
	"zz",
	"zz",
	"zz",
	"zz",
	"zz",
	"PING",
	"ON",
	"FAST_ON",
	"OFF",
	"FAST_OFF",
	"BRIGHT",
	"DIM",
	"START_MANUAL_CHNG",
	"STOP_MANUAL_CHNG",
	"STATUS_REQUEST",
	"STATUS_REPORT",
	"READ_LAST_LEVEL",
	"SET_LAST_LEVEL",
	"READ_PRESET_LEVEL",
	"SET_PRESET_LEVEL",
	"READ_OPER_FLAGS",
	"SET_OPER_FLAGS",
	"DELETE_GRP_X10",
	"LOAD_OFF",
	"LOAD_ON",
	"DO_READ_EE",
	"LEVEL_POKE",
	"RATE_POKE",
	"CURRENT_STATUS",
	"SET_ADDR_MSB",
	"POKE",
	"POKE_EXT",
	"PEEK",
	"PEEK_INTERNAL",
	"POKE_INTERNAL"
	};

#define NUM_CMDS 45

/*
 * alpha to integer
 *
 * Convert a string (hex or dec) to an integer.
 */
int alpha_to_i(char *str)
{
	int base = 10;
	int num = 0;
	char c;

	while (*str) {
		c = tolower(*str);
		if (c == 'x') {
			base = 16;
		}

		/* skip leading zeros */
		if ((num != 0) && (c == '0')) {
			num *= base;
		}

		if (c != '0') {
			/* numbers */
			if ((c >= '1') && (c <= '9')) {
				num *= base;
				num += (c - '0');
			}

			/* hex digits */
			if ((c >= 'a') && (c <= 'f')) {
				num *= base;
				num += ((c - 'a') + 10);
			}
		}
		str++;
	}
	return num;
}

/*
 * str_id_to_hex
 *
 * Convert a device id in text form to binary.
 *
 * Text form looks like "aa.bb.cc"
 */
unsigned char *str_id_to_hex(char *device_id)
{
	static unsigned char id[3];

	memset(id, 0, 3);

	id[0] = atoh(device_id[0]) * 16;
	id[0] += atoh(device_id[1]);

	id[1] = atoh(device_id[3]) * 16;
	id[1] += atoh(device_id[4]);

	id[2] = atoh(device_id[6]) * 16;
	id[2] += atoh(device_id[7]);

	return id;
}


unsigned char atoh(char h)
{
	if ((h >= '0') && (h <= '9')) {
		return (h - '0');
	} else if ((h >= 'a') && (h <= 'f')) {
		return (h - 'a' + 10);
	} else if ((h >= 'A') && (h <= 'F')) {
		return (h - 'A' + 10);
	}

	return 0;
}

int ahtoi(char *h)
{
	int value = 0;

	while (*h++) {
		if (isxdigit(*h)) {
			value = value << 4;
			if ((*h >= '0') && (*h <= '9')) {
				value += (*h - '0');
			} else if ((*h >= 'a') && (*h <= 'f')) {
				value += (*h - 'a' + 10);
			} else if ((*h >= 'A') && (*h <= 'F')) {
				value += (*h - 'A' + 10);
			}
		}
	}

	return value;
}


int getline(FILE *fp, char *s, int lim)
{
	int c, i;
	int ws = 0;

	i = 0;

	while (--lim > 0 && (c = fgetc(fp)) != EOF && c != '\n') {
		if (c != '\r') {
			if (!ws && ((c == ' ') || (c == '\t'))) {
				/* skip leading white space */
			} else {
				s[i++] = c;
				ws = 1;
			}
		}
	}

	s[i] = '\0';
	if ( c == EOF ) {
		return (c);
	} else {
		return(i);
	}
}


/*
 * command_to_num
 *
 * Convert a INSTEON command from text form to the command number.
 */
int command_to_num(char *cmd)
{
	int i;

	for (i = 0; i < NUM_CMDS; i++) {
		if (strncasecmp(cmd, commands[i], strlen(commands[i])) == 0) {
			return i;
		}
	}
	return -1;
}

/*
 * print_commands
 *
 * Print a list of valid commands.
 */
void print_commands()
{
	int i;

	for (i = 0; i < NUM_CMDS; i++) {
		if (strncasecmp("zz", commands[i], strlen(commands[i])) != 0) {
			printf("   %s\n", commands[i]);
		}
	}
}



/*
 * command
 *
 * Convert a command byte to a text name to make it human readable.
 */
char *command(unsigned char cmd)
{
	switch (cmd) {
		case 0x01: return "ASSIGN_TO_GROUP";
		case 0x02: return "DELETE_FROM_GROUP";
		case 0x03: return "READ_CONNECTIONS";
		case 0x04: return "READ_DEVICE_DATA";
		case 0x05: return "FACTORY_RESET";
		case 0x06: return "DEVICE_RESET";
		case 0x07: return "TERMINATE_DOWNLOAD";
		case 0x09: return "ENTER_MASTER_PRG";
		case 0x0a: return "ENTER_SLAVE_PRG";
		case 0x10: return "PING";
		case 0x11: return "ON";
		case 0x12: return "FAST_ON";
		case 0x13: return "OFF";
		case 0x14: return "FAST_OFF";
		case 0x15: return "BRIGHT";
		case 0x16: return "DIM";
		case 0x17: return "START_MANUAL_CHNG";
		case 0x18: return "STOP_MANUAL_CHNG";
		case 0x19: return "STATUS_REQUEST";
		case 0x1a: return "STATUS_REPORT";
		case 0x1b: return "READ_LAST_LEVEL";
		case 0x1c: return "SET_LAST_LEVEL";
		case 0x1d: return "READ_PRESET_LEVEL";
		case 0x1e: return "SET_PRESET_LEVEL";
		case 0x1f: return "READ_OPER_FLAGS";
		case 0x20: return "SET_OPER_FLAGS";
		case 0x21: return "DELETE_GRP_X10";
		case 0x22: return "LOAD_OFF";
		case 0x23: return "LOAD_ON";
		case 0x24: return "DO_READ_EE";
		case 0x25: return "LEVEL_POKE";
		case 0x26: return "RATE_POKE";
		case 0x27: return "CURRENT_STATUS";
		case 0x28: return "SET_ADDR_MSB";
		case 0x29: return "POKE";
		case 0x2a: return "POKE_EXT";
		case 0x2b: return "PEEK";
		case 0x2c: return "PEEK_INTERNAL";
		case 0x2d: return "POKE_INTERNAL";
		case 0x81: return "ASSIGN_TO_CPN_GRP";
		default: return "Unknown";
	}
}


/*
 * flags
 *
 * decode message flags (high nybble) in to human readable messages.
 */
char *flags(unsigned char flag)
{
	static char buf[100];

	memset (buf, 0, 100);

	if (flag & 0x10) {
		strcat(buf, "extended ");
	}
	flag = (flag >> 1);
	if (flag & 0x10) {
		strcat(buf, "ack ");
	}
	flag = (flag >> 1);
	if (flag & 0x10) {
		strcat(buf, "group ");
	}
	flag = (flag >> 1);
	if (flag & 0x10) {
		strcat(buf, "broadcast ");
	}

	return buf;
}


/*
 * device_type_name
 *
 * Decode known device types.
 */
char *device_type_name(unsigned char hi, unsigned char low)
{
	unsigned short type;
	static char unknown[26];

	type = (hi * 256) + low;

	switch (type) {
		case 0x0000: return "PowerLinc V2 pre 1.6     ";
		case 0x0001: return "PowerLinc V2             ";
		case 0x0004: return "ControlLinc V2           ";
		case 0x0005: return "RF RemoteLinc            ";
		case 0x0006: return "ICON Tabletop Controller ";
		case 0x0009: return "SiglalLinc RF            ";
		case 0x0100: return "LampLinc V2 Dimmer       ";
		case 0x0101: return "SwitchLinc V2 Dimmer     ";
		case 0x0102: return "ICON Dimmer Lamp 3-pin   ";
		case 0x0103: return "ICON Dimmer Switch       ";
		case 0x0104: return "SwitchLinc V2 Dimmer 1kw ";
		case 0x0105: return "ICON Switch 1000w        ";
		case 0x0106: return "LampLinc V2 Dimmer 2-pin ";
		case 0x0107: return "ICON Dimmer Lamp         ";
		case 0x0109: return "KeypadLinc V2            ";
		case 0x010a: return "ICON In-wall Controller  ";
		case 0x0114: return "SwitchLinc V2 Companion  ";
		case 0x0115: return "SwitchLinc Dimmer Comp   ";
		case 0x0116: return "ICON Switch Companion hw ";
		case 0x0209: return "ApplianceLinc V2         ";
		case 0x020a: return "SwitchLinc V2 Relay      ";
		case 0x020b: return "ICON Relay Switch        ";
		case 0x020c: return "ICON Appliance Adaptor   ";
		case 0x0400: return "ICON USB Interface       ";
		default:
			sprintf(unknown, "0x%04x Unknown           ", type);
			return unknown;
	}
	return "";
}

