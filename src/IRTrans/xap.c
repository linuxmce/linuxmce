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

#include <winsock2.h>
#include <windows.h>
#include <winuser.h>
#include <io.h>
#include <direct.h>
#include <sys/types.h>
#include <time.h>
#include <sys/timeb.h>
#include <process.h>

#define MSG_NOSIGNAL	0

#endif

#include <stdio.h>
#include <stdlib.h>

#ifdef LINUX
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <netdb.h>
#include <stdint.h>

typedef int SOCKET;
typedef int DWORD;
typedef int WSAEVENT;

#define closesocket		close


#endif

#include "remote.h"
#include "dbstruct.h"
#include "network.h"
#include "errcode.h"
#include "fileio.h"
#include "lowlevel.h"
#include "server.h"
#include "global.h"
#include "webserver.h"
#include "flash.h"
#include "xap.h"
#include "ccf.h"

int xAP_last_hbeat;
int xAP_rcv_port;
SOCKET xAP_rcv;
SOCKET xAP_send;


// target=IRTrans.irserver.<bus>:<ID>.<LED>
// Internal LEDs: target=IRTrans.irserver.<bus>:<ID>:int
// External LEDs: target=IRTrans.irserver.<bus>:<ID>:ext


void xAP_SendHeartbeat (void)
{
	int i;
	char st[1500];

	for (i=0;i<device_cnt;i++) {
		sprintf (st,"xap-hbeat\n{\nv=%d\nhop=1\nuid=%s00\nclass=xap-hbeat.alive\nsource=%s.%d\ninterval=%d\nport=%d\npid=%d\n}\n",
			     XAP_VERSION,XAP_UID,XAP_ADR,i,XAP_HBEAT,xAP_rcv_port,getpid ());
		xAP_SendData (st);
	}
	xAP_last_hbeat = time (0);
}



void xAP_SendIREvent (char remote[],char command[],int bus,int adr)
{
	char st[1500];

	sprintf (st,"xap-header\n{\nv=%d\nhop=1\nuid=%s%02x\nclass=ir.receive\nsource=%s.%d:%d\n}\nIR.Signal\n{\ndevice=%s\nsignal=%s\n}\n",
			     XAP_VERSION,XAP_UID,adr+1,XAP_ADR,bus,adr,remote,command);

	xAP_SendData (st);
}


void xAP_EventReceived (void)
{
	int res;
	char data[1500];

	res = recv(xAP_rcv,data,1500,MSG_NOSIGNAL);
	if (res <= 0) return;

	data[res] = 0;

	xAP_ProcessHeader (data);
}

void xAP_SendData (char data[])
{
	send (xAP_send,data,strlen (data),MSG_NOSIGNAL);

}


void xAP_ProcessHeader (char data[])
{
	IRDATA ird;
	long cmd_num;
	unsigned long adr;
	int p,q,pos,res,bus,mask,led;
	char line[256],err[256],txt[256];

	char remote[1500];
	char command[512];

	if (strncmp (data,"xap-header",10)) return;

	if (xAP_GetItem (data,"class=",line) <= 0 || strcmp (line,"ir.transmit")) return;

	adr = 0;
	bus = 0;
	mask = 0xffff;
	led = 0;

	if (xAP_GetItem (data,"target=",line) > 0) {
		if (xAP_GetItem (data,"target=irtrans.irserver.",line) <= 0) return;
		
		pos = 0;
		while (line[pos] && line[pos] != ':') pos++;
		
		p = line[pos];
		line[pos] = 0;

		if (line[0] == '*') {
			adr = 0x40000000;
			bus = 255;
		}
		else {
			adr = atoi (line) << 20;
			bus = atoi (line);
		}

		if (p) {
			p = ++pos;
			while (line[pos] && line[pos] != '.') pos++;

			q = line[pos];
			line[pos] = 0;

			if (line[p] == '*') {
				adr |= 0x1ffff;
				mask = 0xffff;
			}
			else {
				adr |= 0x10000 | (1 << atoi (line+p));
				mask = (1 << atoi (line+p));
			}
		}

		if (q) {
			pos++;
			if (line[pos] == '*') {
				adr |= 0x60000;
				led = 3;
			}
			else if (line[pos] == 'i') {
				adr |= 0x20000;
				led = 1;
			}
			else if (line[pos] == 'e') {
				adr |= 0x40000;
				led = 2;
			}
		}
	}

	if ((pos = xAP_GetItem (data,"ir.signal",line)) > 0) {

		if (xAP_GetItem (data+pos,"device=",remote) <= 0) return;
		if (xAP_GetItem (data+pos,"signal=",command) <= 0) return;

		res = DBFindRemoteCommand (remote,command,&cmd_num,NULL);
		if (!res) res = SendIR (cmd_num,adr);
		if (res) {
			GetError (res, txt);
			switch(res) {
				case ERR_REMOTENOTFOUND:
					sprintf (err, txt, remote);
					break;
				case ERR_COMMANDNOTFOUND:
					sprintf (err, txt, command);
					break;
				case ERR_WRONGBUS:
					sprintf (err,txt,(adr >> 20) & (MAX_IR_DEVICES - 1));
					break;
				default:
					sprintf (err, txt);
					break;
			}
			log_print (err, LOG_ERROR);
			return;
		}
		sprintf (txt,"xAP send: %s-%s [%x]\n", remote,command,adr);
		log_print (txt,LOG_DEBUG);
	}


	if ((pos = xAP_GetItem (data,"ir.pronto",line)) > 0) {

		if (xAP_GetItem (data+pos,"ir=",remote) <= 0) return;
	
		res = DecodeCCF (remote,&ird,START);
		if (res <= 0) {
			sprintf (err,"Illegal xAP Pronto command\n");
			log_print (err, LOG_ERROR);
			return;
		}

		ird.address = led;
		ird.target_mask = mask;

		DoSendIR (&ird,0,bus);
	}
}


int xAP_GetItem (char data[],char item[],char value[])
{
	int i = 0;
	char line[256];

	do {
		i = xAP_GetLine (data,line,i);
		ConvertLcase (line,strlen (line));
		if (!memcmp (line,item,strlen (item))) {
			strcpy (value,line + strlen (item));
			return (i);
		}
	} while (i != -1);

	return (0);
}


int xAP_GetLine (char data[],char line[],int pos)
{
	int p;
	
	line[0] = 0;
	while (data[pos] == 10 || data[pos] == 13) pos++;

	if (!data[pos]) return (-1);

	p = pos;
	while (data[pos] && data[pos] != 10 && data[pos] != 13) pos++;


	memcpy (line,data+p,pos-p);
	line[pos-p] = 0;

	return (pos);
}
