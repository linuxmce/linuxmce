/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#ifdef WIN32
#include "winsock2.h"
#include <windows.h>
#include <time.h>
#include <sys/timeb.h>
#endif

#include <stdio.h>

#ifdef LINUX
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdint.h>
#include <sys/time.h>

typedef int SOCKET;
typedef int DWORD;
typedef int WSAEVENT;
#define closesocket		close
#endif


#include "remote.h"
#include "errcode.h"
#include "network.h"
#include "dbstruct.h"
#include "lowlevel.h"
#include "fileio.h"
#include "global.h"

#ifdef WIN32
#include "winio.h"
#include "winusbio.h"
BOOL WINAPI ShutdownHandler (DWORD type);
#endif

#define RESEND_WAIT_SER		5
#define RESEND_WAIT_USB		10


byte byteorder;
extern STATUS_BUFFER remote_statusex[MAX_IR_DEVICES];
extern byte new_lcd_flag;
extern byte display_bus;

byte DispConvTable[TABLE_CNT][256];

#define FN_IR		1
#define	FN_SBUS		2
#define FN_SER		4
#define FN_USB		8
#define FN_POWERON	16
#define FN_B_O		32
#define FN_TEMP		64
#define FN_SOFTID	128
#define FN_EEPROM	256
#define FN_TRANSL	512
#define FN_HWCARR	1024
#define FN_DUALRCV	2048


int SetTransceiverModusEx (byte bus,byte mode,word send_mask,byte addr,char *hotcode,int hotlen,byte extended_mode,byte extended_mode2)
{
	MODE_BUFFER md;
	byte res;
	md.sbus_command = HOST_SETMODE;
	md.sbus_address = 0;
	if (addr <= 15)  md.sbus_address |= addr;
	if (addr == 'L') md.sbus_address |= ADRESS_LOCAL;
	if (addr == '*') md.sbus_address |= ADRESS_ALL;

	md.hotcode_len = hotlen;
	memcpy (md.hotcode,hotcode,hotlen);

	md.hotcode[hotlen] = extended_mode;
	md.hotcode[hotlen+1] = extended_mode2;

	md.sbus_len = sizeof (MODE_BUFFER) + md.hotcode_len - (CODE_LENRAW - 2);

	md.mode = mode;
	md.target_mask = send_mask;

	res = WriteTransceiverEx (IRDevices + bus,(IRDATA *)&md);
	if (res) return (res);

	return (0);
}

int TransferFlashdataEx (byte bus,word data[],int adr,int len,byte active,long iradr)
{
	IRDATA md;
	byte res;
	int sl;
	unsigned long cap;

	if (bus > MAX_IR_DEVICES) bus = 0;						// Fix für Kompatibilität mit Clients < V5.0
	cap = remote_statusex[bus].stat[iradr].capabilities;

	if ((cap & FN_EEPROM) == 0) {
		fprintf (stderr,"No IRTrans with EEPROM connected\n\n");
		return (-1);
	}

	if (len > 128) {
		fprintf (stderr,"Maximum Transfer length is 128 Bytes [%d]\n\n",len);
		return (-1);
	}

	md.command = SET_TRANSLATE_DATA;
	md.address = iradr & 0xff;
	md.target_mask = adr;
	md.ir_length = len;
	md.mode = active;
	md.len = (sizeof (IRDATA) - CODE_LEN) + md.ir_length;

	memcpy (md.data,data,len);

	if (time_len != TIME_LEN) ConvertToIRTRANS3 (&md);

	res = WriteTransceiverEx (IRDevices + bus,&md);
	if (res) return (res);

	if ((iradr & 0xff) != IRDevices[bus].my_addr) {
		if (remote_statusex[bus].stat[iradr & 0xff].extended_mode & SBUS_UART) {
			sl = 250;
			if ((remote_statusex[bus].stat[iradr & 0xff].extended_mode2 & SBUS_BAUD_MASK) == 0) sl = 400;
			else if ((remote_statusex[bus].stat[iradr & 0xff].extended_mode2 & SBUS_BAUD_MASK) == 16) sl = 150;
			else if ((remote_statusex[bus].stat[iradr & 0xff].extended_mode2 & SBUS_BAUD_MASK) == 24) sl = 80;
		}
		else sl = 400;
		msSleep (sl);
	}

	return (0);
}

int ReadFlashdataEx (byte bus,int adr,int len)
{
	IRDATA md;
	byte res;


	if ((IRDevices[bus].fw_capabilities  & FN_EEPROM) == 0) {
		fprintf (stderr,"No IRTrans with EEPROM connected\n\n");
		return (-1);
	}


	md.command = 202;
	md.address = 0;
	md.target_mask = adr;
	md.ir_length = len;
	md.len = 20;

	res = WriteTransceiverEx (IRDevices + bus,&md);
	if (res) return (res);

	return (0);
}

int SetTransceiverIDEx (byte bus,byte id)
{
	IRDATA md;
	byte res;


	if ((IRDevices[bus].fw_capabilities & FN_SOFTID) == 0) {
		fprintf (stderr,"No SoftID IRTrans connected. Cannot set ID\n\n");
		return (-1);
	}

	md.command = SET_ID;
	md.address = 0;

	md.len = 8;

	md.target_mask = id;

	res = WriteTransceiverEx (IRDevices + bus,&md);
	if (res) return (res);

	return (0);
}


int ResetTransceiverEx (byte bus)
{
	IRDATA md;
	byte res;


	md.command = SBUS_RESET;
	md.len = 3;

	res = WriteTransceiverEx (IRDevices + bus,&md);
	if (res) return (res);

	return (0);
}


int GetBusInfoEx (STATUS_BUFFER *sb,byte bus)
{
	int i,res;
	IRDATA ir;

	ir.command = HOST_NETWORK_STATUS;
	ir.len = 3;

	if (bus == 100) {
		for (i=0;i<device_cnt;i++) {
			res = WriteTransceiverEx  (IRDevices + i,&ir);
			if (res) return (res);
		}
		for (i=0;i<device_cnt;i++) {
			GetBusInfoDetail (sb + i,(byte)i);
			if (res) return (res);
		}
	}
	else {
		res = WriteTransceiverEx  (IRDevices + bus,&ir);
		if (res) return (res);
		GetBusInfoDetail (sb,bus);
		if (res) return (res);
	}

	return (0);
}


int GetBusInfoDetail (STATUS_BUFFER *sb,byte bus)
{
	int i,res;
	STATUS_BUFFER_1 sb1;
	STATUS_BUFFER_2 sb2;
	STATUS_BUFFER_3 sb3;

	res = ReadIRStringEx (IRDevices+bus,(byte *)sb,sizeof (STATUS_BUFFER),10000);

	if (res == sizeof (STATUS_BUFFER_1)) {
		memcpy (&sb1,sb,sizeof (STATUS_BUFFER_1));
		memset (sb,0,sizeof (STATUS_BUFFER));
		sb->my_adress = sb1.my_adress;

		for (i=0;i <= 15;i++) {
			sb->stat[i].device_mode = sb1.stat[i].device_mode;
			sb->stat[i].send_mask  = sb1.stat[i].send_mask;
			memcpy (sb->stat[i].version,sb1.stat[i].version,10);
			if (i == sb->my_adress) sb->stat[i].capabilities = IRDevices[bus].fw_capabilities;
		}
	}

	if (res == sizeof (STATUS_BUFFER_2)) {
		memcpy (&sb2,sb,sizeof (STATUS_BUFFER_2));
		memset (sb,0,sizeof (STATUS_BUFFER));
		sb->my_adress = sb2.my_adress;

		for (i=0;i <= 15;i++) {
			sb->stat[i].device_mode = sb2.stat[i].device_mode;
			sb->stat[i].extended_mode = sb2.stat[i].extended_mode;
			sb->stat[i].send_mask  = sb2.stat[i].send_mask;
			memcpy (sb->stat[i].version,sb2.stat[i].version,10);
			if (i == sb->my_adress) sb->stat[i].capabilities = IRDevices[bus].fw_capabilities;
		}
	}

	if (res == sizeof (STATUS_BUFFER_3)) {
		memcpy (&sb3,sb,sizeof (STATUS_BUFFER_3));
		memset (sb,0,sizeof (STATUS_BUFFER));
		sb->my_adress = sb3.my_adress;

		for (i=0;i <= 15;i++) {
			sb->stat[i].device_mode = sb3.stat[i].device_mode;
			sb->stat[i].extended_mode = sb3.stat[i].extended_mode;
			sb->stat[i].send_mask  = sb3.stat[i].send_mask;
			memcpy (sb->stat[i].version,sb3.stat[i].version,10);
			sb->stat[i].capabilities = sb3.stat[i].capabilities;
		}
	}

	if (res != sizeof (STATUS_BUFFER) && res != sizeof (STATUS_BUFFER_1) && res != sizeof (STATUS_BUFFER_2) && res != sizeof (STATUS_BUFFER_3)) return (ERR_TIMEOUT);

	IRDevices[bus].my_addr = sb->my_adress;

	SwapStatusbuffer (sb);
	return (0);
}


int ResendIREx (byte bus,IRDATA *ir_data)
{
	int res,i;

	ir_data -> command = HOST_RESEND;
	ir_data -> len = 6;

	if (bus == 255) for (i=0;i<device_cnt;i++) res = WriteTransceiverEx (&IRDevices[i],ir_data);
	else res = (WriteTransceiverEx (&IRDevices[bus],ir_data));
	if (res == ERR_RESEND) {
		ir_data -> command = HOST_SEND;
		if (ir_data -> mode & RAW_DATA)
			ir_data -> len = (sizeof (IRDATA) - (CODE_LEN + (RAW_EXTRA))) + ir_data -> ir_length;
		else
			ir_data -> len = (sizeof (IRDATA) - CODE_LEN) + ir_data -> ir_length;
		if (bus == 255) for (i=0;i<device_cnt;i++) res = WriteTransceiverEx (&IRDevices[i],ir_data);
		else res = (WriteTransceiverEx (&IRDevices[bus],ir_data));
	}

	return (res);
}

int GetDeviceData (long cmd_num,DATABUFFER *dat)
{
	int res;
	IRDATA ird;
	IRDATA send;
	long mac_len,mac_pause,rpt_len;

	res = DBGetIRCode (cmd_num,&ird,0,&mac_len,&mac_pause,&rpt_len);
	if (res) return (res);
	if (mac_len || rpt_len) return (ERR_ISMACRO);

	ird.command = HOST_SEND;
	if (ird.mode & RAW_DATA) ird.len = (sizeof (IRDATA) - (CODE_LEN + (RAW_EXTRA))) + ird.ir_length;
	else {
		ird.len = (sizeof (IRDATA) - CODE_LEN) + ird.ir_length;
		if (ird.mode & RC6_DATA) ird.len++;
	}

	swap_irdata (&ird,&send);

	if (time_len != TIME_LEN) {
		if (ird.mode & RAW_DATA) {
			if (ird.ir_length > OLD_LENRAW) return (ERR_LONGRAW);
		}
		else {
			if (ird.time_cnt > 6) return (ERR_LONGDATA);
			ConvertToIRTRANS3 (&send);
		}
	}
	send.checksumme = get_checksumme (&send);
	dat->statustype = STATUS_DEVICEDATA;
	dat->statuslen = sizeof (DATABUFFER);
	memcpy (dat->data,&send,sizeof (IRDATA));

	return (0);
	
}

int SendIR (long cmd_num,long address)
{
	int i,res,bus = 0;
	IRDATA ird;
	long mac_len,mac_pause,rpt_len;

	res = DBGetIRCode (cmd_num,&ird,0,&mac_len,&mac_pause,&rpt_len);
	if (res) return (res);

	if (address & 0x10000) {
		ird.target_mask = (word)address & 0xffff;
	}
	if (address & 0x60000) ird.address = (address >> 17) & 0x3;
	bus = (address >> 20) & (MAX_IR_DEVICES - 1);
	if (address & 0x40000000) bus = 255;

	res = DoSendIR (&ird,rpt_len,bus);
	
	if (mac_len) msSleep (mac_pause);
	
	for (i=1;i < mac_len;i++) {
		res = DBGetIRCode (cmd_num,&ird,i,&mac_len,&mac_pause,&rpt_len);
		if (res) return (res);

		if (address & 0x10000) {
			ird.target_mask = (word)address & 0xffff;
		}
		if (address & 0x60000) ird.address = (address >> 17) & 0x3;
		res = DoSendIR (&ird,rpt_len,bus);
		
		if (mac_len) msSleep (mac_pause);
	}

	return (res);
}



int DoSendIR (IRDATA *ir_data,long rpt_len,int bus)
{
	byte ocarrier;
	int res,i;
	unsigned long end_time;

	ir_data -> command = HOST_SEND;
	if (ir_data -> mode & RAW_DATA) {
		ir_data -> len = (sizeof (IRDATA) - (CODE_LEN + (RAW_EXTRA))) + ir_data -> ir_length;
		if (mode_flag & CODEDUMP) PrintRawData ((IRRAW *)ir_data);
	}
	else {
		ir_data -> len = (sizeof (IRDATA) - CODE_LEN) + ir_data -> ir_length;
		if (ir_data->mode & RC6_DATA) ir_data->len++;
		if (mode_flag & CODEDUMP) PrintPulseData (ir_data);
	}

	if (!rpt_len) {
		if (bus == 255) for (i=0;i<device_cnt;i++) {
			ocarrier = ir_data->transmit_freq;
			if (!IRDevices[i].io.ext_carrier) ir_data->transmit_freq = Convert2OldCarrier (ocarrier);
			res = WriteTransceiverEx (&IRDevices[i],ir_data);
			ir_data->transmit_freq = ocarrier;
		}
		else {
			ocarrier = ir_data->transmit_freq;
			if (!IRDevices[bus].io.ext_carrier) ir_data->transmit_freq = Convert2OldCarrier (ocarrier);
			res = (WriteTransceiverEx (&IRDevices[bus],ir_data));
			ir_data->transmit_freq = ocarrier;
		}
		return (res);
	}

	end_time = GetMsTime () + rpt_len;
	if (bus == 255) for (i=0;i<device_cnt;i++) {
		ocarrier = ir_data->transmit_freq;
		if (!IRDevices[i].io.ext_carrier) ir_data->transmit_freq = Convert2OldCarrier (ocarrier);
		res = WriteTransceiverEx (&IRDevices[i],ir_data);
		ir_data->transmit_freq = ocarrier;
	}
	else {
		ocarrier = ir_data->transmit_freq;
		if (!IRDevices[bus].io.ext_carrier) ir_data->transmit_freq = Convert2OldCarrier (ocarrier);
		res = (WriteTransceiverEx (&IRDevices[bus],ir_data));
		ir_data->transmit_freq = ocarrier;
	}

	while (GetMsTime () < end_time) {
		ir_data -> command = HOST_RESEND;
		ir_data -> len = 3;

		if (bus == 255) for (i=0;i<device_cnt;i++) res = WriteTransceiverEx (&IRDevices[i],ir_data);
		else res = (WriteTransceiverEx (&IRDevices[bus],ir_data));
		if (res == ERR_RESEND) {
			ir_data -> command = HOST_SEND;
			if (ir_data -> mode & RAW_DATA) ir_data -> len = (sizeof (IRDATA) - (CODE_LEN + (RAW_EXTRA))) + ir_data -> ir_length;
			else {
				ir_data -> len = (sizeof (IRDATA) - CODE_LEN) + ir_data -> ir_length;
				if (ir_data->mode & RC6_DATA) ir_data->len++;
			}
			if (bus == 255) for (i=0;i<device_cnt;i++) {
				ocarrier = ir_data->transmit_freq;
				if (!IRDevices[i].io.ext_carrier) ir_data->transmit_freq = Convert2OldCarrier (ocarrier);
				res = WriteTransceiverEx (&IRDevices[i],ir_data);
				ir_data->transmit_freq = ocarrier;
			}
			else {
				ocarrier = ir_data->transmit_freq;
				if (!IRDevices[bus].io.ext_carrier) ir_data->transmit_freq = Convert2OldCarrier (ocarrier);
				res = (WriteTransceiverEx (&IRDevices[bus],ir_data));
				ir_data->transmit_freq = ocarrier;
			}
		}
	}
	return (res);
}


byte Convert2OldCarrier (byte carrier)
{
	word oc;
	if (carrier == 255 || carrier < 128) return (carrier);
	oc = (carrier & 127) << 2;
	if (oc > 255) oc = 255;
	return ((byte)oc);
}


int SendIRDataEx (IRDATA *ir_data,long address)
{
	byte ocarrier;
	int bus = 0,res,i;
	if (address & 0x10000) {
		ir_data->target_mask = (word)address & 0xffff;
	}
	bus = (address >> 20) & (MAX_IR_DEVICES - 1);
	if (address & 0x40000000) bus = 255;
	if (address & 0x60000) ir_data->address = (address >> 17) & 0x3;

	ir_data -> command = HOST_SEND;
	if (ir_data -> mode & RAW_DATA) {
		ir_data -> len = (sizeof (IRDATA) - (CODE_LEN + (RAW_EXTRA))) + ir_data -> ir_length;
		if (mode_flag & CODEDUMP) PrintRawData ((IRRAW *)ir_data);
	}
	else {
		ir_data -> len = (sizeof (IRDATA) - CODE_LEN) + ir_data -> ir_length;
		if (ir_data->mode & RC6_DATA) ir_data->len++;
		if (mode_flag & CODEDUMP) PrintPulseData (ir_data);
	}
	if (bus == 255) for (i=0;i<device_cnt;i++) {
		ocarrier = ir_data->transmit_freq;
		if (!IRDevices[i].io.ext_carrier) ir_data->transmit_freq = Convert2OldCarrier (ocarrier);
		res = WriteTransceiverEx (&IRDevices[i],ir_data);
		ir_data->transmit_freq = ocarrier;
	}
	else {
		ocarrier = ir_data->transmit_freq;
		if (!IRDevices[bus].io.ext_carrier) ir_data->transmit_freq = Convert2OldCarrier (ocarrier);
		res = (WriteTransceiverEx (&IRDevices[bus],ir_data));
		ir_data->transmit_freq = ocarrier;
	}
	return (res);
}


void LCDTimeCommand (byte mode)
{
	int i;
	IRRAW irw;
#ifdef WIN32
	struct _timeb tb;
#endif
#ifdef LINUX
	struct timeval tb;
	struct timezone tz;
#endif

	memset (&irw,0,sizeof (IRRAW));

	irw.command = HOST_SEND;
	irw.mode = LCD_DATA | mode;
	irw.target_mask = 0xffff;

#ifdef WIN32
	_ftime (&tb);
	if (tb.dstflag) i = 3600;
	else i = 0;
	*((unsigned long *)irw.data) = tb.time - 60 * tb.timezone + i;
	irw.data[4] = tb.millitm / 48;
#endif

#ifdef LINUX
	i = gettimeofday (&tb,&tz);
	if (i) {
		gettimeofday (&tb,NULL);
		*((unsigned long *)irw.data) = tb.tv_sec;
	}
	else *((unsigned long *)irw.data) = tb.tv_sec - 60 * tz.tz_minuteswest;
	irw.data[4] = tb.tv_usec / 48000;
#endif

	swap_long ((int *)irw.data);
	irw.len = (sizeof (IRDATA) - (CODE_LEN + (RAW_EXTRA))) + 5;
	for (i=0;i<device_cnt;i++) WriteTransceiverEx (&IRDevices[i],(IRDATA *)&irw);

}


void LCDBrightness (int val)
{
	byte data[10];
	
	*data = val;
	AdvancedLCD (LCD_DATA | LCD_BRIGHTNESS,data,1);
}

int AdvancedLCD (byte mode,byte data[],int len)
{
	int res,i;
	IRRAW irr;

	if (display_bus!= 255 && !IRDevices[display_bus].io.advanced_lcd) return (0);

	memset (&irr,0,sizeof (IRRAW));

	irr.command = HOST_SEND;
	irr.ir_length = 0;
	irr.mode = mode;

	irr.target_mask = 0xffff;

	memcpy (irr.data,data,len);

	irr.len = sizeof (IRRAW) - CODE_LENRAW + len;
	
	if (display_bus == 255) for (i=0;i<device_cnt;i++) {
		res = WriteTransceiverEx (&IRDevices[i],(IRDATA *)&irr);
	}
	else res = WriteTransceiverEx (&IRDevices[display_bus],(IRDATA *)&irr);

	return (res);
}



int SendLCD (IRRAW *ir_data,long address)
{
	int i,res,bus = 0;

	ir_data -> command = HOST_SEND;
	ir_data -> ir_length = 0;
	if (address & 0x10000) {
		ir_data->target_mask = (word)address & 0xffff;
	}
	bus = display_bus;

	ir_data -> len = sizeof (IRRAW) - CODE_LENRAW + strlen (ir_data -> data) + 1;
	if (bus == 255) for (i=0;i<device_cnt;i++) {
		res = WriteTransceiverEx (&IRDevices[i],(IRDATA *)ir_data);
	}
	else res = WriteTransceiverEx (&IRDevices[bus],(IRDATA *)ir_data);
	return (res);
}


int LearnNextIREx (IRDATA *ir_data,word addr,word timeout,word ir_timeout)
{
	int res;
	byte len,bus;

	IRDATA buffer;
	do {
		ir_data -> address = 0;
		if ((addr & 0xff) <= 15)  ir_data -> address |= addr;
		if ((addr & 0xff) == 'L') ir_data -> address |= ADRESS_LOCAL;
		if ((addr & 0xff) == '*') ir_data -> address |= ADRESS_ALL;

		bus = (addr >> 8) & (MAX_IR_DEVICES - 1);

		ir_data -> command = HOST_LEARNIRQUICK;
		ir_data -> len = sizeof (IRDATA) - CODE_LEN;
		ir_data -> target_mask = ir_timeout;
		res = WriteTransceiverEx (IRDevices + bus,ir_data);
		if (res) return (res);

		res = ReadIRStringEx (IRDevices + bus,&len,1,timeout);
		if (!res) {
			CancelLearnEx (IRDevices + bus);
			return (ERR_TIMEOUT);
		}
	} while (len <= sizeof (IRDATA) - CODE_LEN);

	if (ReadIRStringEx (IRDevices + bus,&buffer.checksumme,len-1,200) != len-1) return (ERR_TIMEOUT);

	if (TIME_LEN != time_len) ConvertToIRTRANS4 ((IRDATA3 *)&buffer);

	swap_irdata (&buffer,NULL);

	memcpy (ir_data -> data,buffer.data,CODE_LEN);
	ir_data -> ir_length = buffer.ir_length;
	ir_data -> address = buffer.address;
	ir_data -> data[ir_data -> ir_length] = 0;

	return (0);
}

void ConvertToIRTRANS4 (IRDATA3 *ird)
{
	IRDATA irnew;

	memset (&irnew,0,sizeof (IRDATA));

	memcpy (&irnew,ird,21);
	memcpy (irnew.pulse_len,ird->pulse_len,12);
	memcpy (&irnew.time_cnt,&ird->time_cnt,CODE_LEN + 3);
	irnew.len = ird->len + 8;
	memcpy (ird,&irnew,sizeof (IRDATA));
}


void CancelLearnEx (DEVICEINFO *dev)
{
	byte res = 1;
	char st[10];
	WriteIRStringEx (dev,(byte *)&res,1);
	res = ReadIRStringEx (dev,st,1,500);
}

int LearnIREx (IRDATA *ir_data,word addr,word timeout,word ir_timeout)
{
	int res;
	byte len,bus;

	do {
		ir_data -> address = 0;
		if ((addr & 0xff) <= 15)  ir_data -> address |= addr;
		if ((addr & 0xff) == 'L') ir_data -> address |= ADRESS_LOCAL;
		if ((addr & 0xff) == '*') ir_data -> address |= ADRESS_ALL;

		bus = (addr >> 8) & (MAX_IR_DEVICES - 1);

		ir_data -> command = HOST_LEARNIR;

		if (strcmp (IRDevices[bus].version+1,"4.04.35") < 0 && ir_timeout > 14) ir_timeout = 14;

		ir_data -> target_mask = ir_timeout;
		ir_data -> len = 6;

		res = WriteTransceiverEx (IRDevices + bus,ir_data);
		if (res) return (res);
		res = ReadIRStringEx (IRDevices + bus,&len,1,timeout);
		if (!res) {
			CancelLearnEx (IRDevices + bus);
			return (ERR_TIMEOUT);
		}
	} while (len <= sizeof (IRDATA) - CODE_LEN);

	if (ReadIRStringEx (IRDevices + bus,&ir_data->checksumme,len-1,200) != len-1) return (ERR_TIMEOUT);

	if (TIME_LEN != time_len) ConvertToIRTRANS4 ((IRDATA3 *)ir_data);

	ir_data -> data[ir_data -> ir_length] = 0;
	swap_irdata (ir_data,NULL);

	return (0);
}


int LearnRepeatIREx (IRDATA *ir_data,word addr,word timeout,word ir_timeout)
{
	int res;
	byte len,bus;

	do {
		ir_data -> address = 0;
		if ((addr & 0xff) <= 15)  ir_data -> address |= addr;
		if ((addr & 0xff) == 'L') ir_data -> address |= ADRESS_LOCAL;
		if ((addr & 0xff) == '*') ir_data -> address |= ADRESS_ALL;

		bus = (addr >> 8) & (MAX_IR_DEVICES - 1);

		ir_data -> command = HOST_LEARNIRREPEAT;
		ir_data -> target_mask = ir_timeout;
		ir_data -> len = 6;

		res = WriteTransceiverEx (IRDevices + bus,(IRDATA *)ir_data);
		if (res) return (res);
		res = ReadIRStringEx (IRDevices + bus,&len,1,timeout);
		if (!res) {
			CancelLearnEx (IRDevices + bus);
			return (ERR_TIMEOUT);
		}
	} while (len <= sizeof (IRDATA) - CODE_LEN);

	if (ReadIRStringEx (IRDevices + bus,&ir_data->checksumme,len-1,200) != len-1) return (ERR_TIMEOUT);

	if (TIME_LEN != time_len) ConvertToIRTRANS4 ((IRDATA3 *)ir_data);

	ir_data -> data[ir_data -> ir_length] = 0;

	swap_irdata (ir_data,NULL);

	return (0);
}


int LearnRawIREx (IRRAW *ir_data,word addr,word timeout,word ir_timeout)
{
	int res;
	byte len,bus;

	do {
		ir_data -> address = 0;
		if ((addr & 0xff) <= 15)  ir_data -> address |= addr;
		if ((addr & 0xff) == 'L') ir_data -> address |= ADRESS_LOCAL;
		if ((addr & 0xff) == '*') ir_data -> address |= ADRESS_ALL;

		bus = (addr >> 8) & (MAX_IR_DEVICES - 1);

		ir_data -> command = HOST_LEARNIRRAW;
		ir_data -> target_mask = ir_timeout;
		ir_data -> len = 6;

		res = WriteTransceiverEx (IRDevices + bus,(IRDATA *)ir_data);
		if (res) return (res);
		res = ReadIRStringEx (IRDevices + bus,&len,1,timeout);
		if (!res) {
			CancelLearnEx (IRDevices + bus);
			return (ERR_TIMEOUT);
		}
	} while (len <= (sizeof (IRRAW) - CODE_LENRAW));

	if (ReadIRStringEx (IRDevices + bus,&ir_data->checksumme,len-1,200) != len-1) return (ERR_TIMEOUT);
	ir_data -> data[ir_data -> ir_length] = 0;

	swap_irdata ((IRDATA *)ir_data,NULL);

	return (0);
}

int LearnRawIRRepeatEx (IRRAW *ir_data,word addr,word timeout,word ir_timeout)
{
	int res;
	byte len,bus;

	do {
		ir_data -> address = 0;
		if ((addr & 0xff) <= 15)  ir_data -> address |= addr;
		if ((addr & 0xff) == 'L') ir_data -> address |= ADRESS_LOCAL;
		if ((addr & 0xff) == '*') ir_data -> address |= ADRESS_ALL;

		bus = (addr >> 8) & (MAX_IR_DEVICES - 1);

		ir_data -> command = HOST_LEARNIRRAWREPEAT;
		ir_data -> target_mask = ir_timeout;
		ir_data -> len = 6;

		res = WriteTransceiverEx (IRDevices + bus,(IRDATA *)ir_data);
		if (res) return (res);
		res = ReadIRStringEx (IRDevices + bus,&len,1,timeout);
		if (!res) {
			CancelLearnEx (IRDevices + bus);
			return (ERR_TIMEOUT);
		}
	} while (len <= (sizeof (IRRAW) - CODE_LENRAW));

	if (ReadIRStringEx (IRDevices + bus,&ir_data->checksumme,len-1,200) != len-1) return (ERR_TIMEOUT);
	ir_data -> data[ir_data -> ir_length] = 0;

	swap_irdata ((IRDATA *)ir_data,NULL);

	return (0);
}



void PrintPulseData (IRDATA *ir_data)
{
	int i,frq;
	char msg[256];

	frq = ir_data->transmit_freq;
	
	if (frq == 255) frq = 1000;
	else if (frq & 128) frq = (frq & 127) * 4;
	for (i=0;i < TIME_LEN;i++) {
		sprintf (msg,"TIME : %d %4d  %4d\n",i,ir_data -> pulse_len[i] * 8,ir_data -> pause_len[i] * 8);
		log_print (msg,LOG_INFO);
	}
	sprintf (msg,"T.REP   :   %4d\n",ir_data->repeat_pause);
	log_print (msg,LOG_INFO);
	sprintf (msg,"REP/MODE:   %4d %d\n",ir_data->ir_repeat,ir_data -> mode);
	log_print (msg,LOG_INFO);
	sprintf (msg,"FREQ    :   %4d\n",frq);
	log_print (msg,LOG_INFO);
	sprintf (msg,"DATA    :   %s\n",ir_data->data);
	log_print (msg,LOG_INFO);
	sprintf (msg,"LEN     :   %4d\n",ir_data->ir_length);
	log_print (msg,LOG_INFO);
	sprintf (msg,"ADR     :   %d\n",ir_data -> address);
	log_print (msg,LOG_INFO);

}


void PrintRawData (IRRAW *ir_data)
{
	int i,frq;
	char msg[256];
	word pulse,pause;

	frq = ir_data->transmit_freq;
	
	if (frq == 255) frq = 455;
	else if (frq & 128) frq = (frq & 127) * 4;
	sprintf (msg,"MODE    :   %d\n",ir_data -> mode & 0xf0);
	log_print (msg,LOG_INFO);
	sprintf (msg,"Repeat  :   %d\n",ir_data -> mode & 0xf);
	log_print (msg,LOG_INFO);
	sprintf (msg,"FREQ    :   %4d\n",frq);
	log_print (msg,LOG_INFO);
	sprintf (msg,"LEN     :   %4d\n",ir_data->ir_length);
	log_print (msg,LOG_INFO);
	sprintf (msg,"ADR     :   %d\n",ir_data -> address);
	log_print (msg,LOG_INFO);

	i = 0;
	
	while (i < ir_data -> ir_length) {
		pulse = ir_data ->data [i++];
		if (!pulse) {
			pulse = ir_data ->data [i++] << 8;
			pulse |= ir_data->data [i++];
		}
		pause = ir_data ->data [i++];
		if (!pause) {
			pause = ir_data ->data[i++] << 8;
			pause |= ir_data->data[i++];
		}
		sprintf (msg,"%5d %5d\n",pulse * 8,pause * 8);
		log_print (msg,LOG_INFO);
	}


}

void PrintCommand (IRDATA *ir_data)
{
	char msg[256];
	
	sprintf (msg,"DATA    :   %s\n",ir_data->data);
	log_print (msg,LOG_INFO);

}



DEVICEINFO IRDevices[MAX_IR_DEVICES];
int device_cnt;

int InitCommunicationEx (char devicesel[])
{
	int i;
	char msg[255];

	byteorder = GetByteorder ();

	i = get_devices (devicesel,0);
	if( i )
		return 1;

	raw_repeat = 1;
	time_len = IRDevices[0].io.time_len;

	for (i=0;i<device_cnt;i++) {
		sprintf (msg,"Name   : %s\n",IRDevices[i].name);
		log_print (msg,LOG_INFO);
		sprintf (msg,"Version: %s\n",IRDevices[i].version);
		log_print (msg,LOG_INFO);
		sprintf (msg,"FW SNo : %d\n",IRDevices[i].fw_serno);
		log_print (msg,LOG_INFO);
		sprintf (msg,"Capab  : %s\n",IRDevices[i].cap_string);
		log_print (msg,LOG_INFO);
		sprintf (msg,"FW Cap : %d\n",IRDevices[i].fw_capabilities);
		log_print (msg,LOG_INFO);
		sprintf (msg,"USB SNo: %s\n",IRDevices[i].usb_serno);
		log_print (msg,LOG_INFO);
		sprintf (msg,"Node   : %s\n\n",IRDevices[i].device_node);
		log_print (msg,LOG_INFO);
	}
		
	for (i=0;i<device_cnt;i++) {
		raw_repeat &= IRDevices[i].io.raw_repeat;
		if (IRDevices[i].io.time_len != time_len) {
			sprintf (msg,"Devices < Version 4.xx.xx and Devices > 4.xx.xx can not be mixed.\nExiting now\n\n");
			log_print (msg,LOG_FATAL);
			return (-1);
		}
	}

	if (new_lcd_flag) display_bus = 255;

	for (i=0;i < device_cnt;i++) if (IRDevices[i].version[0] == 'D' || (IRDevices[i].fw_capabilities & FN_DISPMASK)) {
		display_bus = i;
		break;
	}

#ifdef WIN32
	SetConsoleCtrlHandler (ShutdownHandler,TRUE);
#endif

	return (0);
}

#ifdef WIN32

BOOL WINAPI ShutdownHandler (DWORD type)
{
	mode_flag |= NO_RECONNECT;

	LCDTimeCommand (LCD_DISPLAYTIME);
	LCDBrightness (4);
	exit (0);
}

int GetAvailableDataEx (DEVICEINFO *dev)
{
	if (dev->io.if_type == IF_USB) return GetUSBAvailableEx (dev);
	if (dev->io.if_type == IF_RS232) return GetSerialAvailableEx (dev);

	return (0);
}

#endif

int ReadInstantReceive (DEVICEINFO *dev,byte pnt[],int len)
{
	word to,cnt = 0;

#ifdef WIN32
	if (dev->io.if_type == IF_USB) to = 5;
	else to = 25;
#endif

#ifdef LINUX
	to = 100;
#endif

	cnt = ReadIRStringEx (dev,pnt,len,to);

	if (!cnt) return (0);

	if (dev->io.inst_receive_mode & 1) {
		if (pnt[cnt-1]) {
			cnt += ReadIRStringEx (dev,pnt+cnt,len-cnt,to);
		}
	}
	pnt[cnt] = 0;
	return (cnt);
}


int	get_devices (char sel[],byte testflag)  // Errflag = Continue bei USB Error
{
	int res,i,p,q,autoflag = 0;
	char st[1024],msg[1024];
#ifdef WIN32
	enum FT_STATUS stat;
	char *buf_pnt[17];
	char buf_value[16][64];
	int numDevs;
#else
	char dst[50];
#endif

	strcpy (st,sel);
	st[strlen (st) + 1] = 0;
	q = p = 0;
	device_cnt = 0;
	if (testflag) {
		printf ("Probing IRTrans Devices ..");
		fflush (stdout);
	}
	while (st[p]) {
		if (testflag) {
			printf (".");
			fflush (stdout);
		}
		while (st[p] && sel[p] != ' ' && st[p] != ';') p++;		st[p] = 0;
#ifdef WIN32
		if ((!strncmp (st + q,"usb",3) || !strncmp (st + q,"USB",3)) && !autoflag) {
			res = LoadUSBLibrary ();
			if (res) {
				if (testflag) {
					q = ++p;
					continue;
				}
				return (ERR_OPENUSB);
			}

			autoflag = 1;
			for (i=0;i<16;i++) buf_pnt[i] = buf_value[i];
			buf_pnt[i] = NULL;

			stat = F_ListDevices(buf_pnt,&numDevs,FT_LIST_ALL|FT_OPEN_BY_SERIAL_NUMBER);

			if (FT_SUCCESS(stat)) {
				for (i=0;i<numDevs && device_cnt < MAX_IR_DEVICES;i++) get_detail_deviceinfo (buf_pnt[i],"USB",IF_USB);
			}

			else {
				if (testflag) {
					q = ++p;
					continue;
				}
				return (ERR_OPENUSB);
			}
			if (st[q + 3] == ':') sort_ir_devices (st + q + 4);
		}
		else if (!strncmp (st + q,"com",3) || !strncmp (st + q,"COM",3)) res = get_detail_deviceinfo ("",st+q,IF_RS232);
#endif
#ifdef LINUX
		//LINUX Autofind USB devices
		if ((!strncmp (st + q,"usb",3) || !strncmp (st + q,"USB",3)) && !autoflag) {
			autoflag = 1;
			res = 0;
			for (i=0;i < 16;i++) {
				sprintf (dst,"/dev/usb/ttyUSB%d",i);
				get_detail_deviceinfo ("",dst,IF_USB);

			}
		}
		else if (!strncmp (st + q,"/dev/tty",8)) res = get_detail_deviceinfo ("",st+q,IF_RS232);

		else if ((!strncmp (st + q,"/dev/usb/tty",12) || !strncmp (st + q,"/dev/ttyUSB",11) || 
			      !strncmp (st + q,"/dev/ttyusb",11)) && !autoflag) res = get_detail_deviceinfo ("",st+q,IF_USB);
#endif
		if (res && !testflag) {
			sprintf (msg,"\nCan not open device %s.\nAborting ...\n\n",st+q);
			log_print (msg,LOG_FATAL);
#ifdef IRTRANS_AS_LIB
			return (ERR_OPENUSB);
#else
			exit (-1);
#endif
		}
		q = ++p;
	}

	if (testflag) {
		printf ("\n\n");
		if (!device_cnt) {
			sprintf (st,"No IRTrans Devices found.\nAborting ...\n\n");
			log_print (st,LOG_FATAL);
#ifdef IRTRANS_AS_LIB
			return (ERR_OPENUSB);
#else
			exit (-1);
#endif
		}
		else {
			for (i=0;i<device_cnt;i++) {
				sprintf (st,"Name   : %s\n",IRDevices[i].name);
				log_print (st,LOG_INFO);
				sprintf (st,"Version: %s\n",IRDevices[i].version);
				log_print (st,LOG_INFO);
				sprintf (st,"FW SNo : %d\n",IRDevices[i].fw_serno);
				log_print (st,LOG_INFO);
				sprintf (st,"Capab  : %s\n",IRDevices[i].cap_string);
				log_print (st,LOG_INFO);
				sprintf (st,"FW Cap : %d\n",IRDevices[i].fw_capabilities);
				log_print (st,LOG_INFO);
				sprintf (st,"USB SNo: %s\n",IRDevices[i].usb_serno);
				log_print (st,LOG_INFO);
				sprintf (st,"Node   : %s\n\n",IRDevices[i].device_node);
				log_print (st,LOG_INFO);
			}
		}
	}

	return (0);
}

void sort_ir_devices (char selstring[])
{
	int i,cnt,start,p,q;
	DEVICEINFO di[MAX_IR_DEVICES];

	memcpy (di,IRDevices,sizeof (DEVICEINFO) * MAX_IR_DEVICES);

	i = 0;
	while (di[i].io.if_type == IF_RS232 && i < device_cnt) i++;
	start = cnt = i;
	p = q = 0;
	while (selstring[p]) {
		while (selstring[p] && selstring[p] != ' ' && selstring[p] != ',') p++;
		selstring[p] = 0;
		for (i = start;i < device_cnt;i++) {
			if (!strcmp (selstring+q,di[i].usb_serno) || (atoi (selstring+q) && (word)atoi (selstring+q) == di[i].fw_serno)) {
				memcpy(&IRDevices[cnt++],&di[i],sizeof (DEVICEINFO));
			}
		}
		q = ++p;
	}
	device_cnt = cnt;

}

int get_detail_deviceinfo (char serno[],char devnode[],byte if_type)
{
	int res;
	DEVICEINFO *dev;

#ifdef WIN32
	enum FT_STATS stat;

	dev = IRDevices + device_cnt;
	strcpy (dev->device_node,devnode);
	dev->io.if_type = if_type;
	if (if_type == IF_USB) {
		if (memcmp (serno,"MM",2) && memcmp (serno,"IR",2)) return (ERR_OPEN);

		strcpy (dev->usb_serno,serno);
		stat = F_OpenEx(dev->usb_serno,FT_OPEN_BY_SERIAL_NUMBER,&(dev->io.usbport));

		if (!FT_SUCCESS(stat)) return (ERR_OPEN);
	}
	else {
		dev->usb_serno[0] = 0;
		res = OpenSerialPortEx (devnode,&(dev->io.comport));
		if (res) return (ERR_OPEN);
	}
	dev->io.event = CreateEvent (NULL,TRUE,FALSE,NULL);
#endif

#ifdef LINUX
	dev = IRDevices + device_cnt;
	dev->usb_serno[0] = 0;
	strcpy (dev->device_node,devnode);
	dev->io.if_type = if_type;

	res = OpenSerialPortEx (devnode,&(dev->io.comport));
	if (res) return (ERR_OPEN);
#endif

	res = GetTransceiverVersionEx (dev);
	if (res) {
#ifdef WIN32
		if (dev->io.if_type == IF_USB) F_Close (dev->io.usbport);
		CloseHandle (dev->io.comport);
#else
		close (dev->io.comport);
#endif
		return (ERR_OPEN);
	}
	
	device_cnt++;
	return (0);
}


int GetTransceiverVersionEx (DEVICEINFO *dev)
{
	int res;
	IRDATA ir;
	char data[8];
	
	ir.command = HOST_VERSION;
	ir.len = 3;

	FlushIoEx (dev);

	dev->version[0] = 0;
	res = WriteTransceiverEx (dev,&ir);

	dev->version[8] = 0;
	res = ReadIRStringEx (dev,dev->version,8,500);
	if (res != 8) return (ERR_READVERSION);

	if (strcmp (dev->version+1,MINIMUM_SW_VERSION) < 0) return (ERR_VERSION);
	
	if (strcmp (dev->version+1,"2.24.01") >= 0) {
		res = ReadIRStringEx (dev,data,8,500);
		if (res != 8) return (ERR_READVERSION);

		if (strcmp (dev->version+1,"4.04.01") >= 0) {
			memcpy (&dev->fw_capabilities,data,4);
			swap_long (&dev->fw_capabilities);
			memcpy (&dev->fw_serno,data + 4,4);
			swap_long (&dev->fw_serno);
		}
		else {
			memcpy (&dev->fw_capabilities,data + 1,2);
			swap_long (&dev->fw_capabilities);
			memcpy (&dev->fw_serno,data + 3,4);
			swap_long (&dev->fw_serno);
		}
		if (dev->fw_serno == 2802 || dev->fw_serno == 0x11223344) dev->fw_serno = 0;

	}
	dev->io.raw_repeat = 0;
	dev->io.ext_carrier = 0;
	dev->io.inst_receive_mode = 0;
	dev->io.advanced_lcd = 0;
	if (strcmp (dev->version+1,"4.00.00") >= 0) dev->io.time_len = 8;										// Version 4.0 with TIME_LEN = 8
	else dev->io.time_len = 6;
	if (strcmp (dev->version+1,"4.02.00") >= 0) dev->io.raw_repeat = 1;										// Version 4.2 with RAW_REPEAT feature
	if (strcmp (dev->version+1,"4.03.00") >= 0) dev->io.ext_carrier = 1;									// Version 4.3 mit erweitertem Carrier
	if (strcmp (dev->version+1,"4.04.06") >= 0) dev->io.inst_receive_mode |= 1;								// Version 4.4.6 mit geändertem Instant Rcv
	if (strcmp (dev->version+1,"4.04.07") >= 0 && 
		(dev->version[0] == 'D' || dev->version[0] == 'E')) dev->io.inst_receive_mode |= 2;					// Version 4.4.7 mit geändertem Instant Rcv
	if (strcmp (dev->version+1,"4.04.17") >= 0 &&
		(dev->version[0] == 'D' || dev->version[0] == 'E')) dev->io.advanced_lcd |= 1;						// Version 4.4.17 mit zusätzlichen LCD Funktionen
	if (strcmp (dev->version+1,"4.04.23") >= 0 &&
		(dev->version[0] == 'D' || dev->version[0] == 'E')) dev->io.advanced_lcd |= 2;						// Version 4.4.23 mit neuem LCD Protokoll
	
	if (dev->version[0] == 'F') dev->io.advanced_lcd = 7;													// Uneed V2

	if (dev->fw_capabilities & FN_DEBOUNCE) dev->io.inst_receive_mode |= 2;

	switch (dev->version[0]) {
		case 'X':
			strcpy (dev->name,"IRTrans Translator");
			break;
		case 'U':
			strcpy (dev->name,"IRTrans USB");
			break;
		case 'V':
			strcpy (dev->name,"IRTrans USB 455kHz");
			break;
		case 'S':
			strcpy (dev->name,"IRTrans RS232");
			break;
		case 'T':
			strcpy (dev->name,"IRTrans RS232 455kHz");
			break;

	}
	dev->cap_string[0] = 0;

	if (dev->fw_capabilities & FN_POWERON) strcat (dev->cap_string,"Power On; ");
	if (dev->fw_capabilities & FN_SOFTID) strcat (dev->cap_string,"Soft ID; ");
	if (dev->fw_capabilities & FN_DUALSND) strcat (dev->cap_string,"Dual Transmitter Drivers; ");

	return (0);
}


void FlushIoEx (DEVICEINFO *dev)
{
#ifdef WIN32
	if (dev->io.if_type == IF_USB) FlushUSBEx (dev->io.usbport);
	else FlushComEx (dev->io.comport);
#endif

#ifdef LINUX
	FlushComEx (dev->io.comport);
#endif

}

void Hexdump_File (IRDATA *ird)
{
	int i;
	byte *pnt;

	pnt = (byte *)ird;
	for (i=0;i < ird->len;i++) {
		fprintf (hexfp,"0x%02x ",pnt[i]);
		if (((i+1)%16) == 0) fprintf (hexfp,"\n");
	}
	fprintf (hexfp,"\n\n");
	fflush (hexfp);
	hexflag = 0;
}

void Hexdump_IO (IRDATA *ird)
{
	int i;
	char st[2048],nm[100];
	byte *pnt;

	strcpy (st,"IODUMP: ");
	pnt = (byte *)ird;
	for (i=0;i < ird->len;i++) {
		sprintf (nm,"0x%x  ",pnt[i]);
		strcat (st,nm);
	}
	strcat (st,"\n");

	log_print (st,LOG_DEBUG);
}


int WriteTransceiverEx (DEVICEINFO *dev,IRDATA *src)
{
	byte res = 0,i,pos;
	int count = 0,max;
	IRDATA send;
	byte buffer[1024];

	if (dev->version[0] == 0 && src->command != HOST_VERSION) return (ERR_WRONGBUS);

	swap_irdata (src,&send);

	if (time_len != TIME_LEN && ((src->command == HOST_SEND && !(src->mode & NON_IRMODE)) || src->command == HOST_LEARNIRQUICK)) {
		if (src->mode & RAW_DATA) {
			if (src->ir_length > OLD_LENRAW) return (ERR_LONGRAW);
		}
		else {
			if (src->time_cnt > 6) return (ERR_LONGDATA);
			ConvertToIRTRANS3 (&send);
		}
	}
	if (dev->version[0]) max = 5;
	else max = 2;
	send.checksumme = get_checksumme (&send);
	do {
		if (mode_flag & HEXDUMP) Hexdump_IO (&send);
		if (hexflag) Hexdump_File (&send);

		res = WriteIRStringEx (dev,(byte *)&send,send.len);

		if (res) return (ERR_TIMEOUT);
		res = ReadIRStringEx (dev,buffer,1,500);
		count++;
		if (res != 1 || *buffer != 'O') {
			if (res == 1) {
				res = ReadIRStringEx (dev,buffer+1,250,20);
				if (res == 0) {
					if (*buffer == 'R') return (ERR_RESEND);
				}
				else {
					res++;
					pos = 0;
					while (pos < res) {
						if (dev->io.receive_buffer_cnt == 4) {
							i = 1;
							while (buffer[pos+i] && buffer[pos+i] != 'E' && buffer[pos+i] != 'O' && buffer[pos+i] != 'E' && (pos+i) < res) i++;
						}
						else {
							dev->io.receive_buffer[dev->io.receive_buffer_cnt][0] = buffer[pos];
							i = 1;
							while (buffer[pos+i] && buffer[pos+i] != 'E' && buffer[pos+i] != 'O' && buffer[pos+i] != 'E' && (pos+i) < res) {
								dev->io.receive_buffer[dev->io.receive_buffer_cnt][i] = buffer[pos+i];
								i++;
							}
							dev->io.receive_buffer[dev->io.receive_buffer_cnt][i] = 0;
							dev->io.receive_cnt[dev->io.receive_buffer_cnt] = i;
						}
						pos += i;
						dev->io.receive_buffer_cnt++;
						if (dev->io.inst_receive_mode & 1 && buffer[pos] == 0) pos++;
					}
					if (buffer[res-1] == 'O') return (0);
					res = ReadIRStringEx (dev,buffer,1,500);
					if (*buffer == 'O') return (0);

				}
			}
			FlushIoEx (dev);
			msSleep (200);
			if (count > 2) msSleep (1000);
			res = 0;
		}
	} while (res == 0 && count < max && !(mode_flag & NO_RECONNECT));

	if (count == 5) return (ERR_TIMEOUT);

	return (0);

}

void ConvertToIRTRANS3 (IRDATA *ird)
{
	IRDATA3 irold;

	memcpy (&irold,ird,21);
	memcpy (irold.pulse_len,ird->pulse_len,12);
	memcpy (&irold.time_cnt,&ird->time_cnt,CODE_LEN + 3);
	irold.len = ird->len - 8;
	memcpy (ird,&irold,irold.len);
}


void SwapStatusbuffer (STATUS_BUFFER *sb)
{
	int i;

	if (!byteorder) return;

	for (i=0;i<16;i++) {
		swap_word (&sb->stat[i].send_mask);
		swap_long ((int32_t *)&sb->stat[i].capabilities);
	}
}


void swap_irdata (IRDATA *src,IRDATA *tar)
{	
	int i;
	IRDATA *ir;

	if (tar) {
		memcpy (tar,src,sizeof (IRDATA));
		ir = tar;
	}
	else ir = src;

	if (!byteorder) return;

	swap_word (&ir->target_mask);

	if (!(ir -> mode & (RAW_DATA | LCD_DATA))) {

		for (i=0;i < TIME_LEN;i++) {
			swap_word (ir->pause_len + i);
			swap_word (ir->pulse_len + i);
		}
	}
}


void swap_word (word *pnt)
{
	byte *a,v;

	if (!byteorder) return;

	a = (byte *)pnt;
	v = a[0];
	a[0] = a[1];
	a[1] = v;
}


void swap_long (int32_t *pnt)
{
	byte *a,v;

	if (!byteorder) return;

	a = (byte *)pnt;
	v = a[0];
	a[0] = a[3];
	a[3] = v;

	v = a[1];
	a[1] = a[2];
	a[2] = v;
}


byte get_checksumme (IRDATA *ir)
{
	int i = 2;
	byte cs = 0;
	while (i < ir->len) cs += ((byte *)ir)[i++];
	return (cs);
}


int GetByteorder ()
{
	char arr[2];
	short *pnt;

	pnt = (short *)arr;
	*pnt = 1;

	return (arr[1]);
}


int WriteIRStringEx (DEVICEINFO *dev,byte pnt[],int len)
{
#ifdef WIN32
	if (dev->io.if_type == IF_USB) {
		WriteUSBStringEx (dev,pnt,len);
		return (0);
	}

	return (WriteSerialStringEx (dev,pnt,len));
#endif

#ifdef LINUX
	return WriteSerialStringEx (dev,pnt,len);
#endif
}

int	ReadIRStringEx (DEVICEINFO *dev,byte pnt[],int len,word timeout)
{
#ifdef WIN32
	if (dev->io.if_type == IF_USB) return (ReadUSBStringEx (dev,pnt,len,timeout));
	else return (ReadSerialStringEx (dev,pnt,len,timeout));
#endif

#ifdef LINUX
	return (ReadSerialStringEx (dev,pnt,len,timeout));
#endif
}

void ConvertLCDCharset (byte *pnt)
{
	int i;
	for (i=0;i < 200;i++) pnt[i] = DispConvTable[0][pnt[i]];
}


void InitConversionTables ()
{
	int i;
	int num;

	num = 0;
	memset (DispConvTable[num],' ',256);					// Initialize Table

	for (i=0;i <= 127;i++) DispConvTable[num][i] = i;		// Set lower 7 Bits

	DispConvTable[num][192] = 65;
	DispConvTable[num][193] = 65;
	DispConvTable[num][194] = 65;
	DispConvTable[num][195] = 65;
	DispConvTable[num][197] = 0x81;
	DispConvTable[num][198] = 0x90;
	DispConvTable[num][199] = 0x99;
	DispConvTable[num][200] = 69;
	DispConvTable[num][201] = 69;
	DispConvTable[num][202] = 69;
	DispConvTable[num][203] = 69;
	DispConvTable[num][204] = 73;
	DispConvTable[num][205] = 73;
	DispConvTable[num][206] = 73;
	DispConvTable[num][207] = 73;
	DispConvTable[num][208] = 68;
	DispConvTable[num][209] = 78;
	DispConvTable[num][210] = 79;
	DispConvTable[num][211] = 79;
	DispConvTable[num][212] = 79;
	DispConvTable[num][213] = 79;
	DispConvTable[num][216] = 0x88;
	DispConvTable[num][217] = 85;
	DispConvTable[num][218] = 85;
	DispConvTable[num][219] = 85;
	DispConvTable[num][221] = 89;
	DispConvTable[num][224] = 97;
	DispConvTable[num][225] = 0x83;
	DispConvTable[num][226] = 97;
	DispConvTable[num][227] = 97;
	DispConvTable[num][229] = 0x84;
	DispConvTable[num][230] = 0x91;
	DispConvTable[num][231] = 0x99;
	DispConvTable[num][232] = 101;
	DispConvTable[num][233] = 101;
	DispConvTable[num][234] = 101;
	DispConvTable[num][235] = 101;
	DispConvTable[num][236] = 105;
	DispConvTable[num][237] = 105;
	DispConvTable[num][238] = 105;
	DispConvTable[num][239] = 105;
	DispConvTable[num][241] = 0xee;
	DispConvTable[num][242] = 111;
	DispConvTable[num][243] = 111;
	DispConvTable[num][244] = 111;
	DispConvTable[num][245] = 111;
	DispConvTable[num][248] = 0x88;
	DispConvTable[num][249] = 117;
	DispConvTable[num][250] = 117;
	DispConvTable[num][251] = 117;
	DispConvTable[num][253] = 121;
	DispConvTable[num][255] = 121;


	DispConvTable[num][0xC4] = 0x80;						// Set Conversion (Upper 8 Bits)
	DispConvTable[num][0xC5] = 0x82;
	DispConvTable[num][0xE1] = 0x83;
	DispConvTable[num][0xE5] = 0x84;
	DispConvTable[num][0xD6] = 0x86;
	DispConvTable[num][0xF6] = 0x87;
	DispConvTable[num][0xD8] = 0x88;
	DispConvTable[num][0xF8] = 0x89;
	DispConvTable[num][0xDC] = 0x8A;
	DispConvTable[num][0xFC] = 0x8B;
	DispConvTable[num][0x5C] = 0x8C;
	DispConvTable[num][0xA5] = 0x5C;
	DispConvTable[num][0xA7] = 0x8F;
	DispConvTable[num][0xC6] = 0x90;
	DispConvTable[num][0xE6] = 0x91;
	DispConvTable[num][0xA3] = 0x92;
	DispConvTable[num][0xA6] = 0x98;
	DispConvTable[num][0xC7] = 0x99;
	DispConvTable[num][0xB0] = 0xDF;
	DispConvTable[num][0xE4] = 0xE1;
	DispConvTable[num][0xDF] = 0xE2;
	DispConvTable[num][0xB5] = 0xE4;
	DispConvTable[num][0xA4] = 0xEB;
	DispConvTable[num][0xA2] = 0xEC;
	DispConvTable[num][0xF1] = 0xEE;
	DispConvTable[num][0xF7] = 0xFD;
	DispConvTable[num][0x94] = 0x94;
	DispConvTable[num][0x95] = 0x95;
	DispConvTable[num][0x96] = 0x96;
	DispConvTable[num][0x97] = 0x97;
	DispConvTable[num][0x9b] = 0x9b;
	DispConvTable[num][0x9c] = 0x9c;
	DispConvTable[num][0x9d] = 0x9d;
	DispConvTable[num][0x9e] = 0x9e;
	DispConvTable[num][0x9f] = 0x9f;
}

void SetSpecialChars (byte dat[])
{
	int i;

	dat[0] = 6;
	
	i = 1;
	dat[i] = (i / 9) + 1; // Play
	dat[i+1] = 16;
	dat[i+2] = 24;
	dat[i+3] = 28;
	dat[i+4] = 30;
	dat[i+5] = 28;
	dat[i+6] = 24;
	dat[i+7] = 16;
	dat[i+8] = 0;

	i = 10;
	dat[i] = (i / 9) + 1; // Pause
	dat[i+1] = 27;
	dat[i+2] = 27;
	dat[i+3] = 27;
	dat[i+4] = 27;
	dat[i+5] = 27;
	dat[i+6] = 27;
	dat[i+7] = 27;
	dat[i+8] = 0;

	i = 19;
	dat[i] = (i / 9) + 1; // Stop
	dat[i+1] = 0;
	dat[i+2] = 31;
	dat[i+3] = 31;
	dat[i+4] = 31;
	dat[i+5] = 31;
	dat[i+6] = 31;
	dat[i+7] = 0;
	dat[i+8] = 0;

	i = 28; 
	dat[i] = (i / 9) + 1; // Next
	dat[i+1] = 17;
	dat[i+2] = 25;
	dat[i+3] = 29;
	dat[i+4] = 31;
	dat[i+5] = 29;
	dat[i+6] = 25;
	dat[i+7] = 17;
	dat[i+8] = 0;

	i = 37;
	dat[i] = (i / 9) + 1; // Prev
	dat[i+1] = 17;
	dat[i+2] = 19;
	dat[i+3] = 23;
	dat[i+4] = 31;
	dat[i+5] = 23;
	dat[i+6] = 19;
	dat[i+7] = 17;
	dat[i+8] = 0;

	i = 46;
	dat[i] = (i / 9) + 1; // Rew
	dat[i+1] = 1;
	dat[i+2] = 3;
	dat[i+3] = 7;
	dat[i+4] = 15;
	dat[i+5] = 7;
	dat[i+6] = 3;
	dat[i+7] = 1;
	dat[i+8] = 0;
}


int SetTransceiverModus (byte mode,word send_mask,byte addr,char *hotcode,int hotlen,byte extended_mode,byte extended_mode2,byte usb_mode)
{
	MODE_BUFFER md;
	byte res;
	md.sbus_command = HOST_SETMODE;
	md.sbus_address = 0;
	if (addr <= 15)  md.sbus_address |= addr;
	if (addr == 'L') md.sbus_address |= ADRESS_LOCAL;
	if (addr == '*') md.sbus_address |= ADRESS_ALL;

	md.hotcode_len = hotlen;
	memcpy (md.hotcode,hotcode,hotlen);

	md.hotcode[hotlen] = extended_mode;
	md.hotcode[hotlen+1] = extended_mode2;

	md.sbus_len = sizeof (MODE_BUFFER) + md.hotcode_len - 98;


	md.mode = mode;
	md.target_mask = send_mask;

	res = WriteTransceiver ((IRDATA *)&md,usb_mode);
	if (res) return (res);

	return (0);
}


int GetTransceiverVersion (char version [],unsigned int *cap,unsigned long *serno,byte usbmode)
{
	int res;
	IRDATA ir;
	char data[8];
	
	ir.command = HOST_VERSION;
	ir.len = 3;

//	FlushIoEx (dev);

	*cap = 0;
	*serno = 0;
	version[0] = 0;
	res = WriteTransceiver (&ir,usbmode);

	version[8] = 0;
	res = ReadIRString (version,8,500,usbmode);
	if (res != 8) return (ERR_READVERSION);

	if (strcmp (version+1,"2.24.01") >= 0) {
		res = ReadIRString (data,8,500,usbmode);
		if (res != 8) return (ERR_READVERSION);

		if (strcmp (version+1,"4.04.01") >= 0) {
			memcpy (cap,data,4);
			swap_long (cap);
			memcpy (serno,data + 4,4);
			swap_long (serno);
		}
		else {
			memcpy (cap,data + 1,2);
			swap_long (cap);
			memcpy (serno,data + 3,4);
			swap_long (serno);
		}
		if (*serno == 2802 || *serno == 0x11223344) *serno = 0;

	}
	return (0);
}





int WriteTransceiver (IRDATA *src,byte usb_mode)
{
	byte res = 0;
	char st[100];
	int count = 0;
	IRDATA send;

	swap_irdata (src,&send);

	send.checksumme = get_checksumme (&send);
	do {
		if (usb_mode) FlushUSB ();
		else FlushCom ();
		if (mode_flag & HEXDUMP) Hexdump_IO (&send);
		if (hexflag) Hexdump_File (&send);
		WriteIRString ((byte *)&send,send.len,usb_mode);
		res = ReadIRString (st,1,500,usb_mode);
		count++;
		if (res != 1 || *st != 'O') {
			if (res && *st == 'R') return (ERR_RESEND);
			if (usb_mode) FlushUSB ();
			else FlushCom ();
			msSleep (200);
			if (count > 2) msSleep (1000);
			res = 0;
		}
	} while (res == 0 && count < 5);

	if (count == 5) return (ERR_TIMEOUT);

	return (0);

}


void WriteIRString (byte pnt[],int len,byte usb_mode)
{
#ifdef WIN32
	if (usb_mode) WriteUSBString (pnt,len);
	else WriteSerialString (pnt,len);
#endif

#ifdef LINUX
	WriteSerialString (pnt,len);
#endif
}

int	ReadIRString (byte pnt[],int len,word timeout,byte usb_mode)
{
#ifdef WIN32
	if (usb_mode) return (ReadUSBString (pnt,len,timeout));
	else return (ReadSerialString (pnt,len,timeout));
#endif

#ifdef LINUX
	return (ReadSerialString (pnt,len,timeout));
#endif
}


#ifdef LINUX
void FlushUSB (void)
{
	FlushCom ();
}
#endif
/*
void FlushIoEx (DEVICEINFO *dev)
{
#ifdef WIN32
	if (dev->io.if_type == IF_USB) FlushUSBEx (dev->io.usbport);
	else FlushComEx (dev->io.comport);
#endif

#ifdef LINUX
	FlushComEx (dev->io.comport);
#endif
}
*/
