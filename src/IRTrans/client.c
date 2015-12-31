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

#else

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <errno.h>
#include <netdb.h>
#include <signal.h>
typedef int SOCKET;
typedef void* WSAEVENT;
#define closesocket close
#endif

#include <stdio.h>
#include "remote.h"
#include "network.h"
#include "errcode.h"


int		InitClientSocket (char host[],SOCKET *sock,unsigned long id);
void	PrintStatError (NETWORKSTATUS *stat);

void	ReceiveIR (void);
char	*trim (char st[],int i);
int		MainMenu (void);
void	BuildMainMenu ();
void	BuildSendMenu ();
void	BuildLearnMenu ();
int		SendRemoteCommand (char rem[],char com[],char mask[],char bus[]);
int		SendCommand (NETWORKCOMMAND *com,NETWORKSTATUS *stat);
int		NetworkCommand (int netcommand,char remote[],char command[],word timeout,int adr,NETWORKSTATUS *stat);
int		ShutdownServer (void);
int		SetSwitch (char sw[],char value[]);

#define TIMEOUT 30000

SOCKET serv;

char send_remote[100];
char learn_remote[100];

main (int argc,char *argv[])
{
	int res;
//	unsigned char st[256];
//	NETWORKSTATUS stat;
	
	if (argc != 2 && argc != 3 && argc != 4 && argc != 5 && argc != 6) {
 		fprintf(stderr, "usage   : %s <ip>\n", argv[0]);
		fprintf(stderr, "send    : %s <ip> <remote> <command>\n",argv[0]);
		fprintf(stderr, "send    : %s <ip> <remote> <command> <sendmask> <bus>\n",argv[0]);
 		fprintf(stderr, "shutdown: %s <ip> -shutdown\n", argv[0]);
 		fprintf(stderr, "receive : %s <ip> -receive\n", argv[0]);
 		fprintf(stderr, "switch  : %s <ip> -switch <switch> <value>\n", argv[0]);
		exit (-1);
	}

	res = InitClientSocket (argv[1],&serv,0);
	if (res) {
		fprintf (stderr,"Error connecting to host %s\n",argv[1]);
		exit (res);
	}

	if (argc == 3 && !strcmp (argv[2],"-shutdown")) return (ShutdownServer ());
	if (argc == 3 && !strcmp (argv[2],"-receive")) ReceiveIR ();
	if (argc == 5 && !strcmp (argv[2],"-switch")) return (SetSwitch (argv[3],argv[4]));
	if (argc == 4) return (SendRemoteCommand (argv[2],argv[3],NULL,NULL));
	if (argc == 5) return (SendRemoteCommand (argv[2],argv[3],argv[4],NULL));
	if (argc == 6) return (SendRemoteCommand (argv[2],argv[3],argv[4],argv[5]));


/*
//	NetworkCommand (COMMAND_MCE_CHARS,"",0,0,'L',&stat);
	
	strcpy (st,"      -123456"); // Error durch Scrolling, Error durch 0 Character !!!
//	strcpy (st,"-1234567890abcdefghijklmnop");
	*st = 1;
	st[1] = 2;
	st[2] = 3;
	st[3] = 4;
	st[4] = 5;
	st[5] = 0x94;
	st[6] = 1;
	st[7] = 1;
	st[8] = 6;
	st[9] = 6;
	st[10] = 0x9f;
	st[11] = 0x9e;
//	st[12] = 0;
	sprintf (st,"%-40s%-40s%-40s%-40s","12345678901234567","XP Mediacenter."," "," ");
//	strcpy (st,"123456789012               ");
	NetworkCommand (COMMAND_LCD,st,0,LCD_TEXT | LCD_BACKLIGHT,'L',&stat);
return;

	NetworkCommand (COMMAND_STARTCLOCK,"",0,0,'L',&stat);
	res = 0;
	while (1) {
		sprintf (st,"Test Nr %d",res++);
		NetworkCommand (COMMAND_LCD,st,0,LCD_TEXT | LCD_BACKLIGHT,'L',&stat);
		Sleep (500);
	}
*/

	while (MainMenu ());

	return (0);
}

int SetSwitch (char sw[],char value[])
{
	char st[10];
	NETWORKSTATUS stat;

	st[0] = atoi (sw);
	st[1] = atoi (value);
	NetworkCommand (COMMAND_SETSWITCH,st,st+1,0,'L',&stat);

	return (0);
}

void ReceiveIR ()
{
	int res;
	NETWORKSTATUS stat;
	NETWORKRECV *rcv;

	printf ("Waiting for IR commands ...\n");
	memset (&stat,0,sizeof (NETWORKSTATUS));
	rcv = (NETWORKRECV *)&stat;
	for (;;) {
		res = recv (serv,(char *)&stat,8,0);
		if (stat.statuslen > 8) {
			res = recv (serv,((char *)&stat) + 8,stat.statuslen-8,0);
			if (stat.statustype == STATUS_RECEIVE) {
				printf ("RCV: %s.%s [%d: %s]\n",trim (rcv->remote,80),trim (rcv->command,20),rcv->clientid,trim (rcv->data,200));
			}
		}
	} 
}


char *trim (char st[],int i)
{
	i--;
	while (i && st[i] == ' ') i--;
	st[i+1] = 0;
	return (st);
}


int SendRemoteCommand (char rem[],char com[],char mask[],char bus[])
{
	int adr = 0,res,wait = 0,ibus = 0;
	NETWORKSTATUS stat;
#ifdef WIN32
	
	if (!strcmp (rem,"wait") || !strcmp (rem,"Wait") || !strcmp (rem,"WAIT")) {
		adr = atoi (com);
		Sleep (adr);
		return (0);
	}

#endif
	if (mask) {
		if (!strcmp (mask,"wait") || !strcmp (mask,"Wait") || !strcmp (mask,"WAIT")) wait = atoi (mask+4);
		else if (!strcmp (mask,"int") || !strcmp (mask,"Int") || !strcmp (mask,"INT")) adr |= 1 << 17;
		else if (!strcmp (mask,"ext") || !strcmp (mask,"Ext") || !strcmp (mask,"EXT")) adr |= 2 << 17;
		else if (!strcmp (mask,"all") || !strcmp (mask,"All") || !strcmp (mask,"ALL")) adr |= 3 << 17;
		else {
			if (mask[0] == '0' && mask[1] == 'x') sscanf (mask,"%x",&adr);
			else sscanf (mask,"%d",&adr);
			if (adr) adr = (adr & 0xffff) | 0x10000;
		}
	}
	if (bus) {
		ibus = atoi (bus);
		if (ibus == 255) adr |= 0x40000000;
		else {
			ibus = abs (ibus) & (MAX_IR_DEVICES - 1);
			adr |= ibus << 20;
		}
	}
	res = NetworkCommand (COMMAND_SEND,rem,com,0,adr,&stat);
#ifdef WIN32
	Sleep (wait);
#endif
	return (res);
}


int ShutdownServer (void)
{
	NETWORKCOMMAND com;
	NETWORKSTATUS stat;

	com.netcommand = COMMAND_SHUTDOWN;
	strcpy (com.remote,"XXXshutdownXXX");

	SendCommand (&com,&stat);

#ifdef WIN32
	Sleep (8000);
#endif

	return (0);
}

int SendMenu (void)
{
	char st[255],l2[255];
	int choice = 0;
	int sw,md;
	NETWORKSTATUS stat;

	BuildSendMenu ();
	scanf ("%s",st);
	getchar ();
	choice = atoi (st);

	switch (choice) {
		case 1:
			printf ("\nEnter Remote Name: ");
			fflush (stdout);
			scanf ("%s",send_remote);
			getchar ();
			break;
		case 2:
			printf ("\nCommand Name: ");
			fflush (stdout);
			scanf ("%s",st);
			getchar ();
			NetworkCommand (COMMAND_SEND,send_remote,st,0,0,&stat);
			break;
		case 3:
			printf ("\nLCD Text: ");
			fflush (stdout);
			gets (st);
			NetworkCommand (COMMAND_LCD,st,0,LCD_TEXT | LCD_BACKLIGHT,'L',&stat);
			break;
		case 4:
			printf ("\nLCD Init Text Line 1: ");
			fflush (stdout);
			gets (st);
			printf ("\nLCD Init Text Line 2: ");
			fflush (stdout);
			gets (l2);
			strcat (st,"\n");
			strcat (st,l2);
			NetworkCommand (COMMAND_LCDINIT,st,0,LCD_TEXT,'L',&stat);
			break;
		case 5:
			printf ("\nSwitch Number: ");
			fflush (stdout);
			scanf ("%d",&sw);
			getchar ();
			printf ("\nSwitch Value: ");
			fflush (stdout);
			scanf ("%d",&md);
			getchar ();
			st[0] = sw;
			st[1] = md;
			NetworkCommand (COMMAND_SETSWITCH,st,st+1,0,'L',&stat);
			break;
		case 99:
			return (0);
			break;
	}

	return (1);
}

int LearnMenu (void)
{
	char st[255];
	int choice = 0;
	NETWORKSTATUS stat;

	BuildLearnMenu ();
	scanf ("%s",st);
	getchar ();
	choice = atoi (st);

	switch (choice) {
		case 1:
			if (*learn_remote) NetworkCommand (COMMAND_CLOSE,learn_remote,st,0,'L',&stat);
			printf ("\nEnter Remote Name: ");
			fflush (stdout);
			scanf ("%s",learn_remote);
			getchar ();
			NetworkCommand (COMMAND_LRNREM,learn_remote,st,0,'L',&stat);
			break;
		case 2:
			printf ("\nPress Remote Button to record timing ....");
			fflush (stdout);
			NetworkCommand (COMMAND_LRNTIM,learn_remote,st,TIMEOUT,'*',&stat);
			printf ("OK\n");
			fflush (stdout);
			break;
		case 3:
			printf ("\nCommand Name: ");
			fflush (stdout);
			scanf ("%s",st);
			getchar ();
			printf ("\nPress Remote Button to learn ....");
			fflush (stdout);
			NetworkCommand (COMMAND_LRNCOM,learn_remote,st,TIMEOUT,'*',&stat);
			printf ("OK\n");
			fflush (stdout);
			break;
		case 4:
			printf ("\nCommand Name: ");
			fflush (stdout);
			scanf ("%s",st);
			getchar ();
			printf ("\nPress Remote Button to learn ....");
			fflush (stdout);
			NetworkCommand (COMMAND_LRNLONG,learn_remote,st,TIMEOUT,'*',&stat);
			printf ("OK\n");
			fflush (stdout);
			break;
		case 5:
			printf ("\nCommand Name: ");
			fflush (stdout);
			scanf ("%s",st);
			getchar ();
			printf ("\nPress Remote Button to learn ....");
			fflush (stdout);
			NetworkCommand (COMMAND_LRNRAW,learn_remote,st,TIMEOUT,'*',&stat);
			printf ("OK\n");
			fflush (stdout);
			break;
		case 99:
			if (*learn_remote) NetworkCommand (COMMAND_CLOSE,learn_remote,st,0,'L',&stat);
			return (0);
			break;
	}

	return (1);
}


void ShowSetStatus (int adr,NETWORKMODE *status)
{
	int i;
	status->stat[adr].version[8] = 0;

	printf ("Device [%d]: %s\n\n",adr,status->stat[adr].version);

	printf ("Repeat Mask: ");
	for (i=0;i<16;i++) {
		printf ("%c  ",(status->stat[adr].send_mask & (1 << i) ? 'x':'o'));
	}
	printf ("\n             0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15\n\n");


	printf ("            Device Mode Send [20]: %c\n",(status->stat[adr].device_mode & DEVMODE_SEND ? 'x':'o'));
	printf ("            IR               [21]: %c\n",(status->stat[adr].device_mode & DEVMODE_IR ? 'x':'o'));
	printf ("            SBUS             [22]: %c\n",(status->stat[adr].device_mode & DEVMODE_SBUS ? 'x':'o'));
	printf ("            IR RemoteCtrl    [23]: %c\n",(status->stat[adr].device_mode & DEVMODE_IRCODE ? 'x':'o'));
	printf ("            SBUS RemoteCtrl  [24]: %c\n",(status->stat[adr].device_mode & DEVMODE_SBUSCODE ? 'x':'o'));
	printf ("            RAW              [25]: %c\n",(status->stat[adr].device_mode & DEVMODE_RAW ? 'x':'o'));
	printf ("            Fast Mode        [26]: %c\n",(status->stat[adr].device_mode & DEVMODE_RAWFAST ? 'x':'o'));
	printf ("            SBUS send Repeat [27]: %c\n",(status->stat[adr].device_mode & DEVMODE_REPEAT ? 'x':'o'));

	if (status->stat[adr].features & FN_DUALSND) {
		printf ("\n            Internal LEDs    [30]: %c\n",(status->stat[adr].extended_mode & INTERNAL_LEDS ? 'x':'o'));
		printf ("            External LEDs    [31]: %c\n",(status->stat[adr].extended_mode & EXTERNAL_LEDS ? 'x':'o'));
	}
	if (status->stat[adr].features & FN_DUALRCV) {	
		printf ("\n            Standard Receivr.[33]: %c\n",(status->stat[adr].extended_mode & STANDARD_RCV ? 'x':'o'));
		printf ("            455 kHz Receiver [34]: %c\n",(status->stat[adr].extended_mode & BO_RCV ? 'x':'o'));
	}

	if (status->stat[adr].version[0] == 'V' || status->stat[adr].version[0] == 'T' || status->stat[adr].version[0] == 'C' || status->stat[adr].version[0] == 'X' ) {	// 455kHz
		printf ("\n            Receive B&O      [35]: %c\n",(status->stat[adr].extended_mode & BO_MODE ? 'x':'o'));
	}

	if (strcmp (status->stat[adr].version+1,"3.02.10") >= 0) {
		printf ("\n                 Self Repeat [36]: %c\n",(status->stat[adr].extended_mode & SELF_REPEAT ? 'x':'o'));
		if (status->stat[adr].features & FN_DUALSND) {
			printf ("           via Internal LEDs [32]: %c\n",(status->stat[adr].extended_mode & INTERNAL_REPEAT ? 'x':'o'));
		}
	}
	if (strcmp (status->stat[adr].version+1,"4.04.01") >= 0) {
		printf ("\n         Learn Timeout   5ms [37]: %c\n",(((status->stat[adr].extended_mode2 & LRN_TIMEOUT_MASK) == 0) ? 'x':'o'));
		printf ("         Learn Timeout  15ms [38]: %c\n",(((status->stat[adr].extended_mode2 & LRN_TIMEOUT_MASK) == 1) ? 'x':'o'));
		printf ("         Learn Timeout  30ms [39]: %c\n",(((status->stat[adr].extended_mode2 & LRN_TIMEOUT_MASK) == 2) ? 'x':'o'));
		printf ("         Learn Timeout  60ms [40]: %c\n",(((status->stat[adr].extended_mode2 & LRN_TIMEOUT_MASK) == 3) ? 'x':'o'));
		printf ("         Learn Timeout  90ms [41]: %c\n",(((status->stat[adr].extended_mode2 & LRN_TIMEOUT_MASK) == 4) ? 'x':'o'));
		printf ("         Learn Timeout 120ms [42]: %c\n",(((status->stat[adr].extended_mode2 & LRN_TIMEOUT_MASK) == 5) ? 'x':'o'));
		printf ("         Learn Timeout 150ms [43]: %c\n",(((status->stat[adr].extended_mode2 & LRN_TIMEOUT_MASK) == 6) ? 'x':'o'));
	}

	printf ("\nPowerOn Remote               [50]: %s\n",status->stat[adr].remote);
	printf ("PowerOn Command              [51]: %s\n",status->stat[adr].command);

	printf ("\nCancel                     [88]\n");
	printf ("Save & Exit                [99]\n");

	printf ("\nSelect command ");
	fflush (stdout);
}

void SetDeviceStatus (int adr,NETWORKMODE *status)
{
	int res;
	char st[255];
	int choice = 0;
	NETWORKCOMMAND command;
	NETWORKSTATUS stat;

	while (choice < 50) {
		ShowSetStatus (adr,status);

		scanf ("%s",st);
		getchar ();
		choice = atoi (st);

		if (choice < 20) status->stat[adr].send_mask ^= 1 << choice;

		else if (choice < 30) status->stat[adr].device_mode ^= 1 << (choice - 20);
		else if (choice <= 43) {
			if (choice >= 37 && choice <= 43) status->stat[adr].extended_mode2 = choice - 37;
			else {
				if (choice == 32) choice = 36;
				else if (choice > 32) choice--;
				status->stat[adr].extended_mode ^= 1 << (choice - 30);
			}
		}

		if (choice == 50) {
			printf ("\nEnter PowerOn Remote: ");
			fflush (stdout);
			scanf ("%s",status->stat[adr].remote);
		}
			
		if (choice == 51) {
			printf ("\nEnter PowerOn Command: ");
			fflush (stdout);
			scanf ("%s",status->stat[adr].command);
		}

		if (choice == 99) {
			command.netcommand = COMMAND_SETSTAT;
			strcpy (command.command,status->stat[adr].command);
			strcpy (command.remote,status->stat[adr].remote);
			command.timeout = adr;
			command.mode = status->stat[adr].device_mode;
			command.adress = status->stat[adr].send_mask | (status->stat[adr].extended_mode  << 16);
			command.trasmit_freq = status->stat[adr].extended_mode2;
			memset (&status,0,sizeof (status));
			res = SendCommand (&command,&stat);

			if (res) return;

			if (stat.statustype == STATUS_MESSAGE) {
				PrintStatError (&stat);
				return;
			}
		}

	}
}



int StatusMenu (void)
{
	char st[255];
	int choice = 0;
	int res,i,cnt;
	NETWORKMODE status;
	NETWORKCOMMAND com;


	printf ("Getting Device Status ...");
	fflush (stdout);

	com.netcommand = COMMAND_STATUS;

	memset (&status,0,sizeof (status));
	res = SendCommand (&com,(NETWORKSTATUS *)&status);

	if (res) return (0);

	if (status.statustype != STATUS_DEVICEMODE) {
		PrintStatError ((NETWORKSTATUS *)&status);
		return (0);
	}

	cnt = 0;
	for (i=0;i < 16;i++) {
		if (status.stat[i].version[0]) cnt++;
		status.stat[i].version[8] = 0;
	}

	printf ("%c                                   \n%d Device(s) Found:\n\n",13,cnt);
	for (i=0;i < 16;i++) {
		if (status.stat[i].version[0]) printf ("%2d: - %s\n",i,status.stat[i].version);
	}

	printf ("\n99 - Exit\n");
	printf ("\nSelect command ");
	fflush (stdout);
	scanf ("%s",st);
	getchar ();
	choice = atoi (st);

	if (choice == 99) return (0);

	SetDeviceStatus (choice,&status);

	return (0);
}

int MainMenu (void)
{
	char st[255];
	int choice = 0;

	BuildMainMenu ();
	scanf ("%s",st);
	getchar ();
	choice = atoi (st);

	switch (choice) {
		case 1:
			while (SendMenu ());
			break;
		case 2:
			while (LearnMenu ());
			break;
		case 3:
			while (StatusMenu ());
			break;
		case 99:
			return (0);
			break;
	}

	return (1);
}


void BuildMainMenu ()
{
	printf ("\n\n  1 - Send\n");
	printf ("  2 - Learn\n");
	printf ("  3 - Status\n");
	printf ("\n 99 - Exit\n");
	printf ("\nSelect command ");
	fflush (stdout);
}


void BuildSendMenu ()
{
	printf ("\n\n**Selected Remote: [%s]\n\n",send_remote);
	printf ("  1 - Select remote\n");
	printf ("  2 - Send command\n");
	printf ("  3 - Send LCD\n");
	printf ("  4 - Set LCD Init String\n");
	printf ("  5 - Set Switch\n");
	printf ("\n 99 - Exit\n");
	printf ("\nSelect command ");
	fflush (stdout);
}

void BuildLearnMenu ()
{
	printf ("\n\n**Selected Remote: [%s]\n\n",learn_remote);
	printf ("  1 - Select remote\n");
	printf ("  2 - Learn Timing\n");
	printf ("  3 - Learn Command [Based on timing]\n");
	printf ("  4 - Learn Command [Command with timing]\n");
	printf ("  5 - Learn Command [RAW Data]\n");
	printf ("\n 99 - Exit\n");
	printf ("\nSelect command ");
	fflush (stdout);
}

void PrintStatError (NETWORKSTATUS *stat)
{
	printf ("%s\n",stat->message);
}


int NetworkCommand (int netcommand,char remote[],char command[],word timeout,int adr,NETWORKSTATUS *stat)
{
	int res;
	LCDCOMMAND lcd;
	NETWORKCOMMAND *com;

	memset (&lcd,0,sizeof (lcd));

	com = (NETWORKCOMMAND *)&lcd;
	com->adress = adr;
	com->netcommand = netcommand;
	if (netcommand == COMMAND_LCD || netcommand == COMMAND_LCDINIT) {
		strcpy (lcd.framebuffer,remote);
		lcd.hgt = 4;
		lcd.wid = 40;
		lcd.lcdcommand = (byte)timeout;
	}
	else if (netcommand == COMMAND_SETSWITCH) {
		com->remote[0] = remote[0];
		com->command[0] = command[0];
	}
	else {
		strcpy (com->remote,remote);
		if (command) strcpy (com->command,command);
		com->timeout = timeout;
	}
	res = SendCommand (com,stat);

	if (stat->netstatus) {
		PrintStatError (stat);
		if (stat -> statuslevel == FATAL) exit (res);
	}
	return (stat->netstatus);
}


int SendCommand (NETWORKCOMMAND *com,NETWORKSTATUS *stat)
{
	int res,size;

	com->protocol_version = 200;
	size = sizeof (NETWORKCOMMAND);
	if (com->netcommand == COMMAND_LCD) size = sizeof (LCDCOMMAND);

	res = send (serv,(char *)com,size,0);

	if (res != size) {
		closesocket (serv);
		return (ERR_SEND);
	}

	memset (stat,0,sizeof (NETWORKSTATUS));

	do {
		res = recv (serv,(char *)stat,8,0);
		if (stat->statuslen > 8) {
			res = recv (serv,((char *)stat) + 8,stat->statuslen-8,0);
		}
		else return (0);
	} while (stat->statustype == STATUS_RECEIVE);

	return (0);
}




int InitClientSocket (char host[],SOCKET *sock,unsigned long id)
{
	struct sockaddr_in serv_addr;
	unsigned long adr;
	struct hostent *he;
	struct in_addr addr;

#ifdef WIN32

	int err;
    WORD	wVersionRequired;
    WSADATA	wsaData;
    wVersionRequired = MAKEWORD(2,2);
    err = WSAStartup(wVersionRequired, &wsaData);
    if (err != 0) exit(1);

#endif

	adr = inet_addr (host);
	if (adr == INADDR_NONE) {
		he = (struct hostent *)gethostbyname (host);
		if (he == NULL) return (ERR_FINDHOST);
		memcpy(&addr, he->h_addr_list[0], sizeof(struct in_addr));
		adr = addr.s_addr;
	}



	*sock = socket (PF_INET,SOCK_STREAM,0);
	if (*sock < 0) return (ERR_OPENSOCKET);

	memset (&serv_addr,0,sizeof (serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = adr;
	serv_addr.sin_port = htons (TCP_PORT);

	if (connect (*sock,(struct sockaddr *)&serv_addr,sizeof (serv_addr)) < 0) return (ERR_CONNECT);
	
	send (*sock,(char *)&id,4,0);

	return (0);
}
