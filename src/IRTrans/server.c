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

extern int hCom;

SOCKET local_socket;

#define LIRCD			"/dev/lircd"
#define PERMISSIONS		0666


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

#ifdef WIN32

#include "winusbio.h"
#include "winio.h"

#endif

int		InitServerSocket (SOCKET *,SOCKET *, SOCKET *, SOCKET*);
int		ConfigSocket (SOCKET *sock,unsigned short port);
void	ExecuteNetCommand (SOCKET sockfd);
void	DoExecuteNetCommand (int client,NETWORKCOMMAND *com,STATUSBUFFER *stat);
void	PutNetworkStatus (int res,char msg[],STATUSBUFFER *stat);
void	PutDeviceStatus (NETWORKMODE *mode);
void	PutDeviceStatusEx (NETWORKMODEEX *mode);
int		CheckIdx (char ip[],int idx,NETWORKSTATUS *stat);
int		GetNetworkClient (SOCKET sockfd);
int		ReadIRDatabase (void);
int		ExecuteReceivedCommand (byte command[],int len,int bus);
SOCKET	register_remote_client (SOCKET fd,int mode);
void	clear_existing_socket (SOCKET fd);
void	process_lirc_command (SOCKET fd);
void	process_udp_command (SOCKET fd);
void	lirc_list_command (SOCKET fd,char rem[],char key[],char msg[]);
void	lirc_send_success (SOCKET fd,char msg[]);
void	lirc_send_error (SOCKET fd,char msg[],char err[]);
void	CloseIRSocket (int client);
void	ResultStoreTiming (IRDATA *ird,NETWORKTIMING *stat);
int		GetHotcode (char rem[],char com[],byte data[]);
void	ReformatHotcode (byte data[],int len);
void	display_usage (void);
void	compress_lcdbuffer (LCDCOMMAND *lcd,char *buf,int bus);
int		count_difference (LCDCOMMAND *lcd);
void	compress_char (char ln[],char src,char tar);



#define SELECT_TRANS	1
#define	SELECT_SERVER	2
#define	SELECT_LIRC		3
#define SELECT_LOCAL	4
#define	SELECT_COMMAND	5
#define SELECT_REOPEN	6
#define SELECT_WEB		7
#define SELECT_UDP		8
#define SELECT_XAP		9
#define SELECT_CLIENT	10

#define COMMAND_SERVER	102
#define COMMAND_LIRC	103
#define COMMAND_LOCAL	104
#define COMMAND_REOPEN	106


#ifdef WIN32

int build_event_table (HANDLE events[],byte event_type[],byte event_num[],OVERLAPPED OvCom[],int *ser_event);
int get_selected_event (int eventnum,HANDLE events[],byte event_type[],byte event_num[],SOCKET *sockfd);

WSAEVENT SockEvent;
WSAEVENT LircEvent;
WSAEVENT UDPEvent;
WSAEVENT WebEvent;
WSAEVENT xAPEvent;

#endif

byte IRDataBaseRead;
byte lcd_init = 1;
STATUS_BUFFER remote_statusex[MAX_IR_DEVICES];
byte status_changed = 1;
unsigned int status_cache_timeout = 86400;
unsigned long last_status_read;
byte last_adress,resend_flag;
SOCKET server_socket;
SOCKET lirc_socket;
SOCKET udp_socket;
SOCKET web_socket;
SOCKET udp_relay_socket;

char udp_relay_host[100];
int udp_relay_port = 0;
char udp_relay_format[255];

unsigned short webport = 0;
unsigned long seq_client = 10;
unsigned long seq_call;
unsigned long netmask[32];
unsigned long netip[32];
byte netcount = 0;

NETWORKCLIENT sockinfo[CLIENT_COUNT];

void (*CallBackFn)(const char *pRemote,const char *pCommand)=NULL;

extern IRREMOTE *rem_pnt;
extern long rem_cnt;
extern IRCOMMAND *cmd_pnt;
extern long cmd_cnt;
extern char irtrans_usage[];

unsigned long mode_flag = 0;
char logfile[256];
char hexfile[256];
FILE *hexfp;
byte hexflag;
char irserver_version[20];
char irtrans_version[100];
FILE *logfp;
byte time_len;
byte new_lcd_flag;
byte display_bus;




//	System:
//	LOCALE für Fehlermeldungen
//	Timeout bevor Repeat beginnt
//	Fehlermeldungen mit Parametern erweitern.
//	Neuer Parameter für remotes Verzeichnis


//	Diverses:
//	Export / Import

//	Adresse für LCD übergeben



//	1.0.0	Erstes Final Release								MM		20.04.03
//	2.0.0	Neues Network Format (Rückgabe)						MM		25.04.03
//	2.0.1	Fehlermeldung bei nicht definiertem Hotcode überm.	MM		25.04.03
//	2.0.2	Erkennung von globalen Wiederholungscodes			MM		26.04.03
//	2.0.3	Erweiterungen des Loggings							MM		01.05.03
//	2.0.4	Erweiterungen des Loggings							MM		02.05.03
//	2.1.0	Neues Network Command "Reload" (Girder Client)		MM		06.05.03
//	2.1.1	Kein Bus Scan beim Start des Systems				MM		07.05.03
//	2.2.0	LCD Support											MM		11.05.03
//	2.2.1	Ausblenden von Togglebits auf dem Server			MM		15.05.03
//	2.2.2	LIRC Socket abschaltbar								MM		19.05.03
//	2.3.1	LCD Support erweitert								MM		09.06.03
//	2.3.2	LCD Komprimierung implementiert						MM		10.06.03
//	2.3.3	Lernen jetzt auch mit alten Timings möglich			MM		02.07.03
//	2.3.4	Byteordererkennung und Korrektur (PowerPC)			MM		05.07.03
//	2.3.5	Fixed LCD Display init Bug							MM		17.07.03
//	2.3.6	Changed Receive Code								MM		24.07.03	
//	2.3.7	Last Command Korrektur bei nicht bekannten Befehlen	MM		24.07.03	
//	2.3.8	B&O Anpassungen										MM		02.08.03
//	2.3.9	B&O Anpassungen										MM		04.08.03
//	2.3.10	B&O Carrier											MM		07.08.03
//	2.3.11	Bug nach Client Connect unter LINUX beseitigt		MM		11.09.03
//	2.3.12	Command Nummer für MyHTPC Client ergänzt			MM		15.09.03
//	2.3.13	Anpassung an neue FW Versionen						MM		17.09.03
//	2.3.14	Bug in learnok behoben								MM		23.09.03
//	2.3.15	Small corrections									MM		24.09.03
//	2.3.16	Check for duplicate Commands						MM		24.09.03
//	2.3.17	Send all instances of duplicate Commands			MM		27.09.03
//	2.3.18	Support for temperature sensors						MM		28.09.03
//	2.3.19	Adresse des Empfängers wird an Client übergeben		MM		12.10.03
//	2.3.20	Send Routing										MM		12.10.03
//	2.3.21	Globales Toggle Bit supported						MM		17.10.03
//	2.3.22	Set ID supported									MM		03.11.03
//	2.3.23	UDP Support											FB		08.11.03
//	2.3.24	PowerOn Command wird gespeichert					MM		09.11.03
//	2.3.25	Fehler bei Short Repeat beseitigt					MM		11.11.03
//	2.3.26	Serial Number + no_reset als Default				MM		12.11.03
//	2.3.27	Bugfixes											MM		21.12.03
//	3.0.0	Webserver integriert								MM		24.12.03
//	3.0.1	Bugfix Device Status								MM		05.01.04
//	3.0.2	Neue Option -reset_eeprom							MM		06.01.04
//	3.1.0	Protocol Version im Netzwerk						MM		07.01.04
//	3.1.1	Liste der FBs + Commands							MM		17.01.04
//	3.1.2	Translator Support erweitert						MM		21.01.04
//	3.1.3	Altes Commandformat wird parallel unterstützt		MM		03.02.04
//	3.1.4	Translator erweitert								MM		08.02.04
//	3.1.5	Code Korrektur von RCMM Codes						MM		01.03.04
//	3.2.0	Support for CCF Codes								MM		05.03.04
//	3.2.2	Parallele Unterstützung von altem & neuem Format	MM		17.03.04
//	3.2.3	Fehler bei Timingneuerkennung beseitigt				MM		01.04.04
//	3.2.4	Pronto Codes: Remotes bei Repeat Codes ergänzt		MM		02.04.04
//	3.2.5	CCF Bugfix											MM		03.04.04
//	3.2.6	Test ob Command vorhanden							MM		09.04.04
//	3.2.7	RAW HF Codes werden richtig gesendet				MM		12.04.04
//	3.2.8	Korrektur von LIRC Send Command						MM		13.04.04
//	3.2.9	Returnstruct von Testcommand verkürzt				MM		25.04.04
//	3.2.10	Empfang langer Pakete korrigiert					MM		28.04.04
//	3.2.11	RAW CCF Bugfix										MM		29.04.04
//	3.2.12	CCF Support für RC5 & RC6							MM		30.04.04
//	3.2.13	Lernen von Codes wenn nur Timing da ist				MM		30.04.04
//	3.2.14	Erweiterung des RAW Supports						MM		01.05.04
//	3.2.15	Variable Flashwordsize								MM		01.05.04
//	3.2.16	LONGSEND Support									MM		02.05.04
//	3.2.17	Server Shutdown via Client							MM		07.05.04
//	3.2.18	Translator Support: Gruppen							MM		10.05.04
//	3.2.19	RC6A												MM		11.05.04
//	3.2.20	Mixed fixes											MM		13.05.04
//	3.2.21	Korrektur Receive kurzer RAW Codes					MM		15.05.04
//	3.2.22	Senderauswahl via Adress							MM		17.05.04
//	3.2.23	Capabilities in Statusabfrage						MM		02.06.04
//	3.2.24	Flash Translator via SBUS							MM		03.06.04
//	3.2.25	Neuer Debug Mode -codedump							MM		19.06.04
//	3.2.26	Crash bei großen Hexdumps beseitigt					MM		20.06.04
//			Sendmask für Translatorbefehle
//	3.2.27	CCF RAW Bugs beseitigt								MM		21.06.04
//	3.2.28	Old Commandbuffer Bug fixed							MM		11.07.04
//	3.2.29	Berechtigung über IP Adressen						MM		03.08.04
//	3.3.0	Macro Support										MM		09.08.04
//	3.3.1	Support für Macro Wait Translator					MM		16.08.04
//	3.3.2	Bug behoben: Multisend an Clients					MM		21.08.04
//	3.3.3	Support der -learned_only Funtion					MM		24.08.04
//	3.3.4	CCF Präzision verbessert							MM		24.08.04
//	3.3.5	Neue usage Meldung beim Start						MM		25.08.04
//	3.3.6	UDP Relay											MM		04.09.04
//	3.3.7	Keine Netmask auf Local Socket						MM		04.09.04
//	3.3.8	CCF Präzision weiter verbessert						MM		04.09.04
//	4.1.1	Support für TIME_LEN = 8							MM		11.09.04
//	4.1.2	Fehler bei Togglecodes behoben						MM		12.09.04
//	4.1.3	Commandlist Togglecodes								MM		15.09.04
//	4.1.4	Fehler in Swap_Irdata beseitigt						MM		19.09.04
//	4.1.5	Uneed Version										MM		21.09.04
//	4.2.1	CCF RAW Codes mit Repeat							MM		26.09.04
//			CCF Rundung von Timingwerten optimiert				MM		26.09.04
//	4.2.2	CCF Mode 7000 Codes									MM		27.09.04
//	4.3.1	Support for extended IR Carrier						MM		29.09.04
//	4.3.2	64 Bit Support for AMD64 on LINUX					MM		05.10.04
//	5.1.1	Multiple Device support								MM		10.10.04
//	5.1.2	Multiple Device extensions							MM		16.10.04
//	5.1.3	Multiple Switch devices								MM		17.10.04
//	5.1.4	Extended Carriermode für mehrere Devices			MM		21.10.04
//	5.1.5	Support für Reset Display Option					MM		22.10.04
//	5.1.6	Mediacenter Support									MM		24.10.04
//	5.1.7	Mediacenter Remote beschleunigt						MM		25.10.04
//	5.1.8	Fix für Translator Flash Problem					MM		28.10.04
//	5.1.9	Erweiterung Multibus Send							MM		31.10.04
//	5.1.10	V5 für LINUX										MM		02.11.04
//	5.1.11	LCD Init bei Display Reset							MM		05.11.04
//	5.1.12	CCF Tolerance erweitert								MM		08.11.04
//	5.1.13	Capability Fix für alte Translator					MM		08.11.04
//	5.1.14	MCE Remote Erweiterung								MM		15.11.04
//	5.1.15	LINUX USB I/O										MM		16.11.04
//	5.1.16	Diag Message removed								MM		04.12.04
//	5.1.17	Mediacenter IR support Standard						MM		07.12.04
//	5.1.18	Support für neue Statusflags (Receive Timeout)		MM		10.12.04
//	5.1.19	Receive while LCD Update							MM		12.12.04
//	5.1.20	Receive while LCD Update LINUX						MM		13.12.04
//	5.1.21	Set Status Webserver korrigiert						MM		13.12.04
//	5.1.22	Charset Konvertierung								MM		14.12.04
//	5.1.23	Numeric Codes für B&O angepaßt						MM		14.12.04
//	5.1.24	RC6 Send Fix										MM		15.12.04
//	5.1.25	Charset Konvertierung								MM		15.12.04
//	5.1.26	RC6 + RAW Repeat									MM		15.12.04
//	5.1.27	Tastenentprellung									MM		17.12.04
//	5.1.28	Get Device Command									MM		19.12.04
//	5.2.1	MCE Steuerung konfigurierbar						MM		20.12.04
//	5.2.2	I/O für langsame Systeme weiter verbessert			MM		24.12.04
//	5.2.3	Realtime Clock Support								MM		27.12.04
//	5.2.4	Realtime Clock on Shutdown							MM		28.12.04
//	5.2.5	Realtime Clock set on Shutdown						MM		29.12.04
//	5.2.6	Genauere Zeit für die Realtime Clock				MM		31.12.04
//	5.2.7	Support für einstellbaren Learn Timeout				MM		02.01.05
//	5.2.8	Device Status über den Bus							MM		03.01.05
//	5.2.9	Erweiterte Keyboard Steuerung						MM		06.01.05
//	5.2.10	Korrektur Mediacenter Funktionen					MM		17.01.05
//	5.2.11	Support für Character Sets							MM		18.01.05
//	5.2.12	MCE Volume Fix										MM		19.01.05
//	5.2.13	MCE Volume Fix										MM		19.01.05
//	5.2.14	Special Character Support							MM		20.01.05
//	5.2.15	Support für Uneed Mega32 Modul						MM		20.01.05
//			Leeren Translator flashen							MM		20.01.05
//			0 Chars auf LCD Display								MM		20.01.05
//	5.2.16	Neues LCD Protokoll									MM		21.01.05
//	5.2.17	VFD Level Support									MM		22.01.05
//	5.2.18	LCD Mode fix										MM		24.01.05
//	5.2.19	Korrektur Init EEPROM (Extended Mode 2)				MM		27.01.05
//	5.2.20	Korrektur Resend über mehrere Busse					MM		27.01.05
//	5.2.21	Fix for Send with very old devices					MM		01.02.05
//	5.2.22	Update für Multikeys Translator						MM		04.02.05
//	5.2.23	MCE Commands via Events								MM		08.02.05
//	5.2.24	Support für 1MHz Carrier							MM		16.02.05
//	5.2.25	Support für LCD Module V 2.0						MM		20.02.05
//	5.2.26	Support für UNEED V2								MM		22.02.05
//	5.2.27	Displaytyp am Bus einstellbar						MM		22.02.05
//	5.2.28	CH+ / CH- wieder über Keyboard an Mediacenter		MM		24.02.05
//	5.3.01	Events beliebig konfigurierbar						MM		27.02.05
//	5.3.02	Event Steuerung für externe Apps erweitert			MM		02.03.05
//	5.3.03	Support für RC5/RC6 Notoggle Flag					MM		08.03.05
//	5.3.04	Bug für Sendkey ohne Window behoben					MM		09.03.05
//	5.3.05	Support für Suspend / Resume						MM		24.03.05
//	5.3.06	Reihenfolge beim Exit geändert						MM		24.03.05
//	5.3.07	Zeichenumsetzung erweitert (FR)						MM		29.03.05
//	5.3.08	Sommerzeit Fix										MM		02.04.05
//	5.3.09	Fix für alte LCDs									MM		06.04.05
//	5.3.10	Fix für alte Uneed LCDs								MM		08.04.05
//	5.3.11	Neues Networkcommand DELETE_COMMAND					MM		11.04.05
//			Nach Delete keine Leerzeichen mehr am Fileende		MM		11.04.05
//	5.3.12	Kleine Erweiterungen für schnellen SBUS				MM		17.04.05
//	5.3.13	Erweiterungen für SBUS Flash Translator (Fast SBUS)	MM		19.04.05
//	5.3.14	COMMAND_EMPTY für PHP								MM		01.05.05
//	5.3.15	Mehrer IRTrans + DisplayIRTrans						MM		02.05.05
//	5.3.16	Timeout für Status Cache							MM		04.05.05
//	5.3.17	Statusproblem LINUX mit großen Buffern				MM		05.05.05
//	5.3.18	Update für Translator Flash via SBUS (Classic)		MM		09.05.05
//	5.3.19	Neue Option -hexfile								MM		09.05.05
//	5.3.20	Serielles IF Handling geändert						MM		11.05.05
//	5.3.21	xAP Support											MM		16.05.05
//	5.3.22	xAP Pronto Support									MM		17.05.05
//	5.3.23	Längenangabe [RCV-LEN] im .rem File					MM		18.05.05
//	5.3.24	IRDA Support										MM		20.05.05


#define VERSION	"5.3.24"

#if defined LINUX || defined _CONSOLE

#ifdef IRTRANS_AS_LIB
	int libmain (int argc,char *argv[])
#else
	main (int argc,char *argv[])
#endif
{
	int res,i,adr;
	char *pnt;
	int device_id = -1;
	int eeprom_id = -1;
	char st[255];

	new_lcd_flag = 0;
	mode_flag |= NO_RESET;
	mode_flag |= LOG_FATAL;
	strcpy (irserver_version,VERSION);

	if (argc == 2 && !strcmp (argv[1],"-version")) {
		printf ("IRServer Version %s\nMinimun IRTrans Version %s\n",VERSION,MINIMUM_SW_VERSION);
		exit (0);
		}

	if (argc == 2 && !strcmp (argv[1],"-deviceinfo")) {
		mode_flag = (mode_flag & ~LOG_MASK) | 4;
#ifdef WIN32
		strcpy (st,"usb;com1;com2;com3;com4");
#else
		strcpy (st,"usb;/dev/ttyS0;/dev/ttyS1;/dev/ttyS2;/dev/ttyS3");
#endif
		return (get_devices (st,1));
		}
	argc++;
	for (;--argc > 2;argv++) {											// Process all Command Line Arguments

		if (!strcmp (argv[1],"-no_reconnect")) {
			mode_flag |= NO_RECONNECT;
			continue;
		}
		if (!strcmp (argv[1],"-start_clock")) {
			mode_flag |= CLOCK_STARTUP;
			continue;
		}
		if (!strcmp (argv[1],"-no_lirc")) {
			mode_flag |= NO_LIRC;
			continue;
		}
		if (!strcmp (argv[1],"-debug_code")) {
			mode_flag |= DEBUG_CODE;
			continue;
		}
		if (!strcmp (argv[1],"-hexdump")) {
			mode_flag |= HEXDUMP;
			continue;
		}
		if (!strcmp (argv[1],"-codedump")) {
			mode_flag |= CODEDUMP;
			continue;
		}
		if (!strcmp (argv[1],"-xap")) {
			mode_flag |= XAP;
			continue;
		}
		if (!strcmp (argv[1],"-learned_only")) {
			mode_flag |= LEARNED_ONLY;
			continue;
		}
		if (!strcmp (argv[1],"-logfile")) {
			argc--;
			argv++;
			strcpy (logfile,argv[1]);
			continue;
		}
		if (!strcmp (argv[1],"-hexfile")) {
			argc--;
			argv++;
			strcpy (hexfile,argv[1]);
			continue;
		}
		if (!strcmp (argv[1],"-udp_relay")) {
			argc--;
			argv++;
			strcpy (udp_relay_host,argv[1]);
			argc--;
			argv++;
			udp_relay_port = atoi (argv[1]);
			argc--;
			argv++;
			strcpy (udp_relay_format,argv[1]);
			continue;
		}
		if (!strcmp (argv[1],"-loglevel")) {
			argc--;
			argv++;
			mode_flag = (mode_flag & ~LOG_MASK) | (atoi (argv[1]) & LOG_MASK);
			continue;
		}
		if (!strcmp (argv[1],"-set_id")) {
			argc--;
			argv++;
			device_id = atoi (argv[1]) & 0xf;
			continue;
		}
		if (!strcmp (argv[1],"-stat_timeout")) {
			argc--;
			argv++;
			status_cache_timeout = atoi (argv[1]);
			continue;
		}
		if (!strcmp (argv[1],"-lcd")) {
			argc--;
			argv++;
			new_lcd_flag = atoi (argv[1]);
			continue;
		}
		if (!strcmp (argv[1],"-netmask")) {
			argc--;
			argv++;
			if (netcount < 32) {
				netip[netcount] = ntohl (inet_addr (strtok (argv[1],"/")));
				pnt = strtok (NULL,"/");
				if (pnt) {
					adr = 0;
					res = atoi (pnt);
					for (i=0;i < res;i++) adr |= 1 << (31-i);
					netmask[netcount++] = adr;
				}

			}
			continue;
		}
		if (!strcmp (argv[1],"-reset_eeprom")) {
			argc--;
			argv++;
			eeprom_id = atoi (argv[1]) & 0xf;
			continue;
		}
		if (!strcmp (argv[1],"-http_port")) {
			argc--;
			argv++;
			webport = atoi (argv[1]);
			continue;
		}
		if (!strcmp (argv[1],"-no_web")) {
			mode_flag |= NO_WEB;
			continue;
		}
#ifdef LINUX
		if (!strcmp (argv[1],"-daemon")) {
			if (*logfile == 0) strcpy (logfile,"irserver.log");
			mode_flag |= DAEMON_MODE;
			continue;
		}
#endif
		fprintf (stderr,"Unknown option %s\n",argv[1]);
		display_usage ();
	}

	if (argc == 1 || argv[1][0] == '-') display_usage ();

	res = InitServer (argv[1]);

	if (res) {
		GetError (res,st);
		log_print (st,LOG_FATAL);
#ifdef IRTRANS_AS_LIB
		return res;
#else
		exit (res);
#endif
	}

	if (device_id != -1) {  // Bus !!!
		res = SetTransceiverIDEx (0,(byte)device_id);
#ifdef IRTRANS_AS_LIB
		return res;
#else
		exit (res);
#endif
	}

	if (eeprom_id != -1) {	// Bus !!!
		res = SetTransceiverModusEx (0,159,0xffff,0,"",0,0xf,3);
#ifdef IRTRANS_AS_LIB
		return res;
#else
		exit (res);
#endif
	}

	res = RunServer ();

	if (res) {
		GetError (res,st);
		log_print (st,LOG_FATAL);
#ifdef IRTRANS_AS_LIB
		return res;
#else
		exit (res);
#endif
	}
	return (0);
}


void display_usage (void)
{
	fprintf (stderr,"%s",irtrans_usage);
	exit (1);
}

#endif

int InitServer (char dev[])
{
	int res,i;
	char msg[256];

#ifdef LINUX
	FILE *fp;
#endif


	res = InitCommunicationEx (dev);
	if (res) return (res);
	
	if (*logfile) {
		logfp = fopen (logfile,"w");
		if (!logfp) {
			fprintf (stderr,"Unable to open Logfile %s\n",logfile);
			exit (-1);
		}
	}

	if (*hexfile) {
		hexfp = fopen (hexfile,"w");
		if (!hexfp) {
			fprintf (stderr,"Unable to open Hexfile %s\n",hexfile);
			exit (-1);
		}
	}

#ifdef LINUX
	if (mode_flag & DAEMON_MODE) {
		if (fork () == 0) {
			fclose (stdin);
			fclose (stdout);
			fclose (stderr);
			setsid ();
			fp = fopen ("/tmp/.irserver.pid","w");
			if (fp) {
				fprintf (fp,"%d\n",getpid ());
				fclose (fp);
			}
		}
		else exit (0);
	}
#endif

	sprintf (msg,"IRServer Version %s\n",VERSION);
	log_print (msg,LOG_FATAL);

	if (!device_cnt) {
		sprintf (msg,"No IRTrans Devices found.\nAborting ...\n\n");
		log_print (msg,LOG_FATAL);
		exit (-1);
	}
	for (i=0;i < device_cnt;i++) {
		if (IRDevices[i].fw_serno) sprintf (msg,"[%2d]: %-20s %-12s SN: %u\n",i,IRDevices[i].name,IRDevices[i].version,IRDevices[i].fw_serno);
		else sprintf (msg,"[%2d]: %-20s %-12s\n",i,IRDevices[i].name,IRDevices[i].version);
		log_print (msg,LOG_FATAL);
	}

	if (udp_relay_port) {
		sprintf (msg,"Relaying to UDP %s:%d using Format %s\n",udp_relay_host,udp_relay_port,udp_relay_format);
		log_print (msg,LOG_INFO);
	}


	res = ReadIRDatabase ();
	if (res) return (res);

	InitConversionTables ();
	return (0);
}

int RunServer ()
{
	int res,wait,evnt,i,j;
	SOCKET sockfd;
	DWORD cnt;
	byte rdcom[1000],dummy;
	unsigned long lasttimesync = 0;


#ifdef LINUX
	fd_set events;
	int maxfd,ionum;
	int socktype;
	struct timeval tv;
	byte usbflag;
#endif

#ifdef WIN32
	int numevents;
	int waittime;
	int ser_event;
	HANDLE events[CLIENT_COUNT + MAX_IR_DEVICES + 4];
	OVERLAPPED OvCom[MAX_IR_DEVICES];
	byte event_type[CLIENT_COUNT + MAX_IR_DEVICES + 4];
	byte event_num[CLIENT_COUNT + MAX_IR_DEVICES + 4];
#endif

#ifndef IRTRANS_AS_LIB
	res = InitServerSocket (&server_socket,&lirc_socket,&udp_socket,&web_socket);
	if (res) return (res);
#endif	

	if (webport) {
		sprintf (rdcom,"IRTrans Webserver started on Port %d.\n",webport);
		if (webport) log_print (rdcom,LOG_FATAL);
	}

	if (mode_flag & CLOCK_STARTUP) LCDTimeCommand (LCD_DISPLAYTIME);
	else LCDTimeCommand (LCD_SETTIME);
	LCDBrightness (5);
	lasttimesync = time (0);

#ifdef WIN32
	SockEvent = WSACreateEvent ();
	WSAEventSelect (server_socket,SockEvent,FD_ACCEPT);

	UDPEvent = WSACreateEvent ();
	WSAEventSelect (udp_socket, UDPEvent,FD_READ);

	xAPEvent = WSACreateEvent ();
	WSAEventSelect (xAP_rcv, xAPEvent,FD_READ);

	if (mode_flag & NO_LIRC) LircEvent = NULL;
	else {
		LircEvent = WSACreateEvent ();
		WSAEventSelect (lirc_socket,LircEvent,FD_ACCEPT);
	}

	if (mode_flag & NO_WEB) WebEvent = NULL;
	else {
		WebEvent = WSACreateEvent ();
		WSAEventSelect (web_socket,WebEvent,FD_ACCEPT);
	}

	waittime = INFINITE;
	for (i=0;i < device_cnt;i++) if ((!(mode_flag & NO_RECONNECT) && IRDevices[i].io.if_type == IF_USB) || mode_flag & XAP) waittime = 10000;

	while (1) {
		for (i=0;i < device_cnt;i++) {
			for (j=0;j < IRDevices[i].io.receive_buffer_cnt;j++) {
				if (IRDevices[i].io.receive_cnt[j] > 2) ExecuteReceivedCommand (IRDevices[i].io.receive_buffer[j],IRDevices[i].io.receive_cnt[j],i);
				IRDevices[i].io.receive_cnt[j] = 0;
			}
			IRDevices[i].io.receive_buffer_cnt = 0;
		}
		for (i=0;i < device_cnt;i++) while (GetAvailableDataEx (IRDevices+i)) {
			cnt = ReadInstantReceive (&IRDevices[i],rdcom,1000);
			if (cnt > 2) ExecuteReceivedCommand (rdcom,cnt,i);
		}
		numevents = build_event_table (events,event_type,event_num,OvCom,&ser_event);

		wait = WaitForMultipleObjects (numevents,events,FALSE,waittime);

		if ((time (0) - lasttimesync) > 3600) {
			LCDTimeCommand (LCD_SETTIME);
			lasttimesync = time (0);
		}

		if ((time (0) - xAP_last_hbeat) > XAP_HBEAT) xAP_SendHeartbeat ();

		if (wait == WAIT_TIMEOUT) {
			dummy = 1;
			for (i=0;i < device_cnt;i++) if (IRDevices[i].io.if_type == IF_USB) WriteUSBStringEx (IRDevices + i,&dummy,1);
		}
		else {
			evnt = get_selected_event (wait,events,event_type,event_num,&sockfd);
			switch (evnt) {
				case SELECT_TRANS:
					cnt = ReadInstantReceive (&IRDevices[event_num[wait]],rdcom,1000);
					if (cnt > 2) ExecuteReceivedCommand (rdcom,cnt,event_num[wait]);
					break;
				case SELECT_LIRC:
				case SELECT_LOCAL:
				case SELECT_SERVER:
					register_remote_client (sockfd,evnt);
					break;
				case SELECT_WEB:
					ProcessWebRequest (sockfd);
					break;
				case SELECT_UDP:
					process_udp_command (sockfd);
					break;
				case SELECT_XAP:
					xAP_EventReceived ();
					break;
				case COMMAND_LIRC:
				case COMMAND_LOCAL:
					process_lirc_command (sockfd);
					break;
				case COMMAND_SERVER:
				case COMMAND_REOPEN:
					ExecuteNetCommand (sockfd);
					break;
			}
		}
	}
#endif

#ifdef LINUX
	while (1) {

		maxfd = build_select_table (&events);

		tv.tv_sec = 10;
		tv.tv_usec = 0;

		usbflag = 0;
		for (i=0;i < device_cnt;i++) if (!(mode_flag & NO_RECONNECT) && IRDevices[i].io.if_type == IF_USB) usbflag = 1;

		if ((usbflag && !(mode_flag & NO_RECONNECT)) || mode_flag & XAP) wait = select (maxfd,&events,NULL,NULL,&tv);
		else wait = select (maxfd,&events,NULL,NULL,NULL);


		if ((time (0) - lasttimesync) > 3600) {
			LCDTimeCommand (LCD_SETTIME);
			lasttimesync = time (0);
		}

		if ((time (0) - xAP_last_hbeat) > XAP_HBEAT) xAP_SendHeartbeat ();

		if (!wait) {
			dummy = 1;
			for (i=0;i < device_cnt;i++) if (IRDevices[i].io.if_type == IF_USB) WriteIRStringEx (IRDevices + i,&dummy,1);
		}
		while (wait) {
			evnt = get_selected_fd (&events,&sockfd,&ionum);
			switch (evnt) {
				case SELECT_TRANS:
					cnt = ReadInstantReceive (IRDevices + ionum,rdcom,1000);
					if (cnt > 2) ExecuteReceivedCommand (rdcom,cnt,ionum);
					break;
				case SELECT_LIRC:
				case SELECT_LOCAL:
				case SELECT_SERVER:
					register_remote_client (sockfd,evnt);
					break;
				case SELECT_WEB:
					ProcessWebRequest (sockfd);
					break;
				case COMMAND_LIRC:
				case COMMAND_LOCAL:
					process_lirc_command (sockfd);
					break;
				case SELECT_UDP:
					process_udp_command (sockfd);
					break;
				case COMMAND_SERVER:
				case COMMAND_REOPEN:
					ExecuteNetCommand (sockfd);
					break;
			}
			for (i=0;i < device_cnt;i++) {
				for (j=0;j < IRDevices[i].io.receive_buffer_cnt;j++) {
					if (IRDevices[i].io.receive_cnt[j] > 2) ExecuteReceivedCommand (IRDevices[i].io.receive_buffer[j],IRDevices[i].io.receive_cnt[j],i);
					IRDevices[i].io.receive_cnt[j] = 0;
				}
				IRDevices[i].io.receive_buffer_cnt = 0;
			}
			wait--;
		}
	}

#endif

}


#ifdef WIN32

int build_event_table (HANDLE events[],byte event_type[],byte event_num[],OVERLAPPED OvCom[],int *ser_event)
{
	int i,num = 0;

	for (i=0;i < device_cnt;i++) {
		if (IRDevices[i].io.if_type == IF_USB) SetUSBEventEx (&IRDevices[i],FT_EVENT_RXCHAR);
		else {
			memset (&OvCom[i],0,sizeof (OVERLAPPED));
			OvCom[i].hEvent = IRDevices[i].io.event;
			SetCommMask (IRDevices[i].io.comport,EV_RXCHAR);
			WaitCommEvent (IRDevices[i].io.comport,ser_event,&OvCom[i]);
		}
		event_type[num] = SELECT_TRANS;
		event_num[num] = i;
		events[num++] = IRDevices[i].io.event;
	}

	event_type[num] = SELECT_SERVER;
	event_num[num] = 0;
	events[num++] = SockEvent;

	if (LircEvent) {
		event_type[num] = SELECT_LIRC;
		event_num[num] = 0;
		events[num++] = LircEvent;
	}

	event_type[num] = SELECT_UDP;
	event_num[num] = 0;
	events[num++] = UDPEvent;
	
	if (WebEvent) {
		event_type[num] = SELECT_WEB;
		event_num[num] = 0;
		events[num++] = WebEvent;
	}

	if (mode_flag & XAP) {
		event_type[num] = SELECT_XAP;
		event_num[num] = 0;
		events[num++] = xAPEvent;
	}

	for (i=0;i < CLIENT_COUNT;i++) {
		if (sockinfo[i].fd && sockinfo[i].event) {
			event_type[num] = SELECT_CLIENT;
			event_num[num] = i;
			events[num++] = sockinfo[i].event;
		}
	}

	return (num);
}

int get_selected_event (int eventnum,HANDLE events[],byte event_type[],byte event_num[],SOCKET *sockfd)
{

	int fds = 0,i = 0;

	if (event_type[eventnum] == SELECT_TRANS) ResetEvent (events[eventnum]);
	else WSAResetEvent (events[eventnum]);

	if (event_type[eventnum] == SELECT_SERVER) *sockfd = server_socket;
	else if (event_type[eventnum] == SELECT_LIRC) *sockfd = lirc_socket;
	else if (event_type[eventnum] == SELECT_UDP) *sockfd = udp_socket;
	else if (event_type[eventnum] == SELECT_WEB) *sockfd = web_socket;
	else if (event_type[eventnum] == SELECT_XAP) *sockfd = xAP_rcv;

	fds = event_type[eventnum];

	if (event_type[eventnum] == SELECT_CLIENT) {
		*sockfd = sockinfo[event_num[eventnum]].fd;
		fds = sockinfo[event_num[eventnum]].type + 100;
	}

	for (i=0;i < device_cnt;i++) {
		if (IRDevices[i].io.if_type == IF_USB) SetUSBEventEx (&IRDevices[i],0);
	else SetCommMask (IRDevices[i].io.comport,0);
	}

	return (fds);
}

#endif

#ifdef LINUX

int get_selected_fd (fd_set *events,SOCKET *sockfd,int *ionum)
{
	int fds = 0,i = 0;

	for (i=0;i < device_cnt && !fds;i++) {
		if (FD_ISSET (IRDevices[i].io.comport,events)) {
			fds = SELECT_TRANS;
			*sockfd = IRDevices[i].io.comport;
			*ionum = i;
		}
	}
#ifndef IRTRANS_AS_LIB	
	if (!fds && FD_ISSET (server_socket,events)) {
		fds = SELECT_SERVER;
		*sockfd = server_socket;
	}
#endif	
	if (!(mode_flag & NO_LIRC) && !fds && FD_ISSET (lirc_socket,events)) {
		fds = SELECT_LIRC;
		*sockfd = lirc_socket;
	}
#ifndef IRTRANS_AS_LIB
	if (!fds && FD_ISSET (local_socket,events)) {
		fds = SELECT_LOCAL;
		*sockfd = local_socket;
	}
#endif
	if (!(mode_flag & NO_WEB) && !fds && FD_ISSET (web_socket,events)) {
		fds = SELECT_WEB;
		*sockfd = web_socket;
	}
#ifndef IRTRANS_AS_LIB
	if (!fds && FD_ISSET (udp_socket,events)) {
		fds = SELECT_UDP;
		*sockfd = udp_socket;
	}
#endif	
	if (!fds && FD_ISSET (xAP_rcv,events)) {
		fds = SELECT_XAP;
		*sockfd = xAP_rcv;
	}

	i = 0;
	while (!fds && i < CLIENT_COUNT) {
		if (sockinfo[i].type && FD_ISSET (sockinfo[i].fd,events)) {
			fds = sockinfo[i].type + 100;
			*sockfd = sockinfo[i].fd;
		}
		i++;
	}

	if (fds) FD_CLR (*sockfd,events);

	return (fds);
}


int build_select_table (fd_set *events)
{
	int i,max;
	
	FD_ZERO (events);

	FD_SET (server_socket,events);
	max = server_socket;

	for (i=0;i < device_cnt;i++) {
		FD_SET (IRDevices[i].io.comport,events);
		if (IRDevices[i].io.comport > max) max = IRDevices[i].io.comport;
	}

	if (!(mode_flag & NO_LIRC)) {
		FD_SET (lirc_socket,events);
		if (lirc_socket > max) max = lirc_socket;
	}

	FD_SET (local_socket,events);
	if (local_socket > max) max = local_socket;

	FD_SET (udp_socket,events);
	if (udp_socket > max) max = udp_socket;

	if (!(mode_flag & NO_WEB)) {
		FD_SET (web_socket,events);
		if (web_socket > max) max = web_socket;
	}

	if (mode_flag & XAP) {
		FD_SET (xAP_rcv,events);
		if (xAP_rcv > max) max = xAP_rcv;
	}

	for (i=0;i < CLIENT_COUNT;i++) if (sockinfo[i].fd) {
		FD_SET (sockinfo[i].fd,events);
		if (sockinfo[i].fd > max) max = sockinfo[i].fd;
	}

	return (max + 1);
}

#endif




SOCKET register_remote_client (SOCKET fd,int mode)
{
	int res,num;
	unsigned long adr;
	SOCKET call;
	int clilen;
	char rdcom[1024];
	uint32_t clientid;
	struct sockaddr_in cli_addr;

	if (mode == SELECT_LIRC) sprintf (rdcom,"LIRC TCP/IP Socket connection request\n");
	if (mode == SELECT_LOCAL) sprintf (rdcom,"Local Socket connection request\n");
	if (mode == SELECT_SERVER) sprintf (rdcom,"IRTRANS TCP/IP Socket connection request\n");

	log_print (rdcom,LOG_DEBUG);
	clilen = sizeof (cli_addr);

	call = accept (fd,(struct sockaddr *)&cli_addr,&clilen);
	if (call < 0) {
		sprintf (rdcom,"Accept error %d\n",errno);
		log_print (rdcom,LOG_ERROR);
	}
	else {
#ifdef WIN32
		adr = ntohl (cli_addr.sin_addr.S_un.S_addr);
#else
		adr = ntohl (cli_addr.sin_addr.s_addr);
#endif
		if (adr != 0x7f000001 && mode != SELECT_LOCAL) {
			for (res=0;res < netcount;res++) {
				if ((netip[res] & netmask[res]) == (adr & netmask[res])) break;
			}
			if (netcount && res == netcount) {
				sprintf (rdcom,"Error: IP Address %s not allowed (Access rights).\n",inet_ntoa (cli_addr.sin_addr));
				log_print (rdcom,LOG_ERROR);
				shutdown (call,2);
				closesocket (call);
				return (0);
			}
		}

		res = 0;
		if (mode == SELECT_SERVER) {
#ifdef WIN32
			WSAEventSelect (call,SockEvent,0);
			ioctlsocket (call,FIONBIO,&res);
#endif
			res = recv (call,(char *)&clientid,4,MSG_NOSIGNAL);
			if (res != 4) {
				shutdown (call,2);
				closesocket (call);
				return (0);
			}
#ifdef LINUX
			fcntl (call,F_SETFL,O_NONBLOCK);
#endif
			swap_long (&clientid);
			res = 0;
			if (clientid > 1) {
				while (res < CLIENT_COUNT) {
					if (sockinfo[res].type == SELECT_REOPEN && 
						clientid == sockinfo[res].clientid) {
						if (sockinfo[res].fd) {
#ifdef WIN32
							WSACloseEvent (sockinfo[res].event);
							sockinfo[res].event = NULL;
#endif
							shutdown (sockinfo[res].fd,2);
							closesocket (sockinfo[res].fd);
						}
						sockinfo[res].fd = call;
#ifdef WIN32
						sockinfo[res].event = WSACreateEvent ();
						WSAEventSelect (call,sockinfo[res].event,FD_READ | FD_CLOSE);
#endif
						return (call);
					}
					res++;
				}
				// Send Status illegal ID
			}
			if (clientid == 1) mode = SELECT_REOPEN;
		}
		while (res < CLIENT_COUNT) {							// Leeren Eintrag suchen
			if (sockinfo[res].type == 0) {
				sockinfo[res].fd = call;
				sockinfo[res].type = mode;
				if (mode == SELECT_REOPEN) sockinfo[res].clientid = seq_client++;
				strcpy (sockinfo[res].ip,inet_ntoa (cli_addr.sin_addr));
#ifdef WIN32
				sockinfo[res].event = WSACreateEvent ();
				WSAEventSelect (call,sockinfo[res].event,FD_READ | FD_CLOSE);
#endif
				if (mode == SELECT_LIRC) sprintf (rdcom,"LIRC TCP/IP Client %d accepted from %s\n",res,inet_ntoa (cli_addr.sin_addr));
				if (mode == SELECT_LOCAL) sprintf (rdcom,"Local Client %d accepted on %d\n",res,call);
				if (mode == SELECT_SERVER) sprintf (rdcom,"IRTRANS TCP/IP Client %d accepted from %s\n",res,inet_ntoa (cli_addr.sin_addr));
				if (mode == SELECT_REOPEN) sprintf (rdcom,"IRTRANS [R] TCP/IP Client %d accepted from %s\n",res,inet_ntoa (cli_addr.sin_addr));
				log_print (rdcom,LOG_INFO);
				break;
			}
			res++;
		}
		if (res == CLIENT_COUNT) {
			res = 0;
			num = -1;
			clientid = 0xffffffff;
			while (res < CLIENT_COUNT) {						// Ältesten Eintrag suchen
				if (sockinfo[res].type == SELECT_REOPEN && 
					sockinfo[res].callno < clientid && sockinfo[res].fd == 0) {
					num = res;
					clientid = sockinfo[res].callno;
				}
				res++;
			}

			res = num;
			if (res >= 0) {
				if (sockinfo[res].fp) fclose ((sockinfo[res].fp));
				sockinfo[res].fp = NULL;

				sockinfo[res].fd = call;
				sockinfo[res].type = mode;
				if (mode == SELECT_REOPEN) sockinfo[res].clientid = seq_client++;
				strcpy (sockinfo[res].ip,inet_ntoa (cli_addr.sin_addr));
	#ifdef WIN32
				sockinfo[res].event = WSACreateEvent ();
				WSAEventSelect (call,sockinfo[res].event,FD_READ | FD_CLOSE);
	#endif
				if (mode == SELECT_LIRC) sprintf (rdcom,"LIRC TCP/IP Client %d accepted from %s\n",res,inet_ntoa (cli_addr.sin_addr));
				if (mode == SELECT_LOCAL) sprintf (rdcom,"Local Client %d accepted on %d\n",res,call);
				if (mode == SELECT_SERVER) sprintf (rdcom,"IRTRANS TCP/IP Client %d accepted from %s\n",res,inet_ntoa (cli_addr.sin_addr));
				log_print (rdcom,LOG_INFO);
			}
		}

		if (res == CLIENT_COUNT) {
			sprintf (rdcom,"No more socket client (max=%d)\n",res);
			log_print (rdcom,LOG_ERROR);
			shutdown (call,2);
			closesocket (call);
		}
	}

	return (call);
}


void process_lirc_command (SOCKET fd)
{
    int res,i;
	long num;
    char com[1024],msg[1024],*pnt,*key,*rem,err[256];
	static char active_lirc_string[1024];
    char *mp;
    char *ep;

    res = recv (fd,com,1024,MSG_NOSIGNAL);
    if (res <= 0) {
         i = 0;
         while (i < CLIENT_COUNT) {
           if (sockinfo[i].fd == fd) CloseIRSocket (i);
           i++;
	}
		 *active_lirc_string = 0;
         return;
        }
        com[res] = 0;
		strcat (active_lirc_string,com);
		if (com[res-1] != 13 && com[res-1] != 10) return;
		strcpy (com,active_lirc_string);
		*active_lirc_string = 0;
        for (mp = com; *mp && (ep = strchr(mp, '\n')); mp = ep)
        {
	       *ep++ = '\0';
		    strcpy (msg,mp);
			strcat (msg, "\n");
	        pnt = strtok (mp," \t\n");
										// Unterstützung Start / Stop Repeat ??
		    if (!strcmp (mp,"SEND_ONCE")) {
				rem = strtok (NULL," \t\n\r");
				key = strtok (NULL," \t\n\r");
				if (rem == NULL || key == NULL) {
					sprintf (err,"IR Send Error: No Remote / Command specified\n");
					log_print (err,LOG_ERROR);
					lirc_send_error (fd,msg,err);
					continue;
				}
			sprintf (err,"LIRC SEND_ONCE  Rem: %s  Key: %s\n",rem,key);
			log_print (err,LOG_DEBUG);

			if (!memcmp (key,"+++wait",7) || !memcmp (key,"+++Wait",7) || !memcmp (key,"+++WAIT",7)) {
				msSleep (atol (key + 7));
			}
			else {
				res = DBFindRemoteCommand (rem,key,&num,NULL);
				if (res) {
					sprintf (err,"IR Send Error %d\n",res);
	                log_print (err,LOG_ERROR);
		            lirc_send_error (fd,msg,err);
			        continue;
				}
			SendIR (num,0x40000000);
         }
         resend_flag = 0;
         lirc_send_success (fd,msg);
         continue;
        }
        if (!strcmp (mp,"LIST")) {
         rem = strtok (NULL," \t\n");
         key = strtok (NULL," \t\n");
         lirc_list_command (fd,rem,key,msg);
         continue;
        }

        sprintf (err,"Unknown LIRC Command received: %s\n",pnt);
        log_print (err,LOG_ERROR);
        lirc_send_error (fd,msg,err);
        }
}


void lirc_list_command (SOCKET fd,char rem[],char key[],char msg[])
{
	long i,j;
	char st[1024];
	char remote[80],command[20];

	memset (remote,0,80);
	memset (command,0,20);

	if (rem == NULL && key == NULL) {
		sprintf (st,"BEGIN\n%sSUCCESS\nDATA\n%d\n",msg,rem_cnt+1);
		send (fd,st,strlen (st),MSG_NOSIGNAL);
		sprintf (st,"%d Remotes:\n",rem_cnt);
		send (fd,st,strlen (st),MSG_NOSIGNAL);
		for (i=0;i < rem_cnt;i++) {
			sprintf (st,"%-20s  %3d Commands\n",rem_pnt[i].name,rem_pnt[i].command_end - rem_pnt[i].command_start);
			send (fd,st,strlen (st),MSG_NOSIGNAL);
		}

		sprintf (st,"END\n");
		send (fd,st,strlen (st),MSG_NOSIGNAL);
		return;
	}

	if (key == NULL) {
		strcpy (remote,rem);
		i = DBFindRemote (remote);
		if (i == -1) {
			sprintf (st,"No Remote %s found",rem);
			lirc_send_error (fd,msg,st);
			return;
		}
		sprintf (st,"BEGIN\n%sSUCCESS\nDATA\n%d\n",msg,rem_pnt[i].command_end - rem_pnt[i].command_start + 1);
		send (fd,st,strlen (st),MSG_NOSIGNAL);
		sprintf (st,"%d Commands:\n",rem_pnt[i].command_end - rem_pnt[i].command_start);
		send (fd,st,strlen (st),MSG_NOSIGNAL);
		for (j=0;j < rem_pnt[i].command_end - rem_pnt[i].command_start;j++) {
			sprintf (st,"%s\n",cmd_pnt[rem_pnt[i].command_start + j].name);
			send (fd,st,strlen (st),MSG_NOSIGNAL);
		}

		sprintf (st,"END\n");
		send (fd,st,strlen (st),MSG_NOSIGNAL);
		return;
	}
	strcpy (remote,rem);
	i = DBFindRemote (remote);
	if (i == -1) {
		sprintf (st,"No Remote %s found",rem);
		lirc_send_error (fd,msg,st);
		return;
	}
	strcpy (command,key);
	j = DBFindCommand (command,i);
	if (j == -1) {
		sprintf (st,"No Remote/Command  %s/%s found",rem,key);
		lirc_send_error (fd,msg,st);
		return;
	}
	sprintf (st,"BEGIN\n%sSUCCESS\nDATA\n%d\n",msg,1);
	send (fd,st,strlen (st),MSG_NOSIGNAL);

	sprintf (st,"Data: %s\n",cmd_pnt[j].data);
	send (fd,st,strlen (st),MSG_NOSIGNAL);

	sprintf (st,"END\n");
	send (fd,st,strlen (st),MSG_NOSIGNAL);
}


void lirc_send_success (SOCKET fd,char msg[])
{
	char st[1024];

	sprintf (st,"BEGIN\n%sSUCCESS\nEND\n",msg);
	send (fd,st,strlen (st),MSG_NOSIGNAL);
}


void lirc_send_error (SOCKET fd,char msg[],char err[])
{
	char st[1024];

	sprintf (st,"BEGIN\n%sERROR\nDATA\n1\n%s\nEND\n",msg,err);
	send (fd,st,strlen (st),MSG_NOSIGNAL);
}
	

// Erkannte IR Codes an Clients schicken

int ExecuteReceivedCommand (byte command[],int len,int bus)
{
	int res,i = 0;
	int start_pos = 0;
	static long l_time;
	static char l_command[21];
	static char l_remote[81];
	static int l_repeat;
	static char l_addr;
	long com_num,rem_num;
	static unsigned long ltv;
	static int rcnt;
	unsigned long tv;

#ifdef WIN32
	struct _timeb tb;
#endif

#ifdef LINUX
	struct timeval tb;
#endif

	char rem[512],name[512],num[512],dat[1024],msg[256];
	NETWORKRECV nr;

	memset (&nr,0,sizeof (NETWORKRECV));
	memset (nr.data,' ',200);
	memset (nr.remote,' ',80);
	memset (nr.command,' ',20);


	if (!(IRDevices[bus].io.inst_receive_mode & 2)) {
#ifdef WIN32
		_ftime (&tb);
		tv = (tb.time & 0x7fffff) * 1000 + tb.millitm;
#endif

#ifdef LINUX
		gettimeofday (&tb,NULL);
		tv = (tb.tv_sec & 0x7fffff) * 1000 + tb.tv_usec / 1000;
#endif

		if ((!rcnt && (tv - ltv) < 200) || (tv - ltv) < 250) {
			rcnt++;
			ltv = tv;
			if (rcnt < 5) return (1);
		}
		else rcnt = 0;
		ltv = tv;
	}

	start_pos = DBFindCommandName (command + 1,rem,name,*command,&rem_num,&com_num,&nr.command_num,start_pos);
	if (start_pos) {
		while (start_pos) {
			if (time (0) != l_time || strcmp (l_command,name) || strcmp (l_remote,rem)) l_repeat = 0;
			
			if ((time (0) - l_time) < 2 && !strcmp (l_command,name) && !strcmp (l_remote,rem) && l_addr != (*command & 0xf)) return (1);
			GetNumericCode (command + 1,num,rem,name);
			if (mode_flag & DEBUG_CODE) {
				sprintf (msg,"%s[%d.%d] %s %s\n",num,bus,(*command & 15),name,rem);
				log_print (msg,LOG_DEBUG);
			}
			sprintf (dat,"%s %02d %s %s%c",num,l_repeat,name,rem,10);
			nr.clientid = 0;
			nr.statuslen = sizeof (NETWORKRECV);
			nr.statustype = STATUS_RECEIVE;
			memcpy (nr.remote,rem,strlen (rem));
			memcpy (nr.command,name,strlen (name));
			if (command[1] >= '0') memcpy (nr.data,command+1,strlen (command+1));
			else memcpy (nr.data,num,strlen (num));
			nr.adress = *command & 15 + bus * 16;

			SwapNetworkheader ((NETWORKSTATUS *)&nr);
			i = 0;
			while (i < CLIENT_COUNT) {
				if (sockinfo[i].type == SELECT_LIRC || sockinfo[i].type == SELECT_LOCAL) {
					res = send (sockinfo[i].fd,dat,strlen (dat),MSG_NOSIGNAL);
					if (res <= 0) CloseIRSocket (i);
				}
				if (sockinfo[i].type == SELECT_SERVER || sockinfo[i].type == SELECT_REOPEN) {
					res = send (sockinfo[i].fd,(char *)&nr,sizeof (NETWORKRECV),MSG_NOSIGNAL);
					if (res <= 0) CloseIRSocket (i);
				}
				i++;
			}
			if( CallBackFn ) (*CallBackFn)(rem,name);
				
			if (udp_relay_port) udp_relay (rem,name,*command & 15);

			if (mode_flag & XAP) xAP_SendIREvent (rem,name,bus,*command & 15);
#ifdef WIN32
			PostWindowsMessage (rem_num,com_num);
#endif
			l_repeat++;
			l_time = time (0);
			strcpy (l_command,name);
			strcpy (l_remote,rem);
			l_addr = *command & 0xf;
			start_pos = DBFindCommandName (command + 1,rem,name,*command,&rem_num,&com_num,&nr.command_num,start_pos);
		}
		return (0);
	}
	else if (!(((*command & 0xf0) >> 2) & RAW_DATA) && command[1] >= '0' && !(mode_flag & LEARNED_ONLY)) { 
		nr.clientid = 0;
		nr.statuslen = sizeof (NETWORKRECV);
		nr.statustype = STATUS_RECEIVE;
		memcpy (nr.data,command+1,strlen (command+1));
		nr.adress = *command & 15 + bus * 16;
		nr.command_num = 0;

		SwapNetworkheader ((NETWORKSTATUS *)&nr);
		i = 0;
		while (i < CLIENT_COUNT) {
			if (sockinfo[i].type == SELECT_SERVER || sockinfo[i].type == SELECT_REOPEN) {
				res = send (sockinfo[i].fd,(char *)&nr,sizeof (NETWORKRECV),MSG_NOSIGNAL);
				if (res <= 0) CloseIRSocket (i);
			}
			i++;
		}
		if (mode_flag & DEBUG_CODE) {
			sprintf (msg,"[%d.%d]: %s\n",bus,(*command & 15),command+1);
			log_print (msg,LOG_DEBUG);
		}
	}
	return (1);
}


void udp_relay (char rem[],char com[],int adr)
{
	int i,p;
	char frm[255],dat[255];
	char adrst[10];
	char *parm[3];

	parm[0] = parm[1] = parm[2] = 0;
	sprintf (adrst,"%02d",adr);
	strcpy (frm,udp_relay_format);

	p = 0;
	for (i=0;frm[i];i++) {
		if (frm[i] == '%') {
			if (p == 3) frm[i] = ' ';
			else {
				i++;
				if (frm[i] == 'r') {
					parm[p++] = rem;
					frm[i] = 's';
				}
				else if (frm[i] == 'c') {
					parm[p++] = com;
					frm[i] = 's';
				}
				else if (frm[i] == 'a') {
					parm[p++] = adrst;
					frm[i] = 's';
				}
				else frm[i-1] = ' ';
			}
		}
	}

	sprintf (dat,frm,parm[0],parm[1],parm[2]);
	send (udp_relay_socket,dat,strlen (dat),0);
}


void CloseIRSocket (int client)
{
#ifdef WIN32
	WSACloseEvent (sockinfo[client].event);
	sockinfo[client].event = NULL;
#endif
	if (sockinfo[client].fd) {
		shutdown (sockinfo[client].fd,2);
		closesocket (sockinfo[client].fd);
	}
	sockinfo[client].fd = 0;
	if (sockinfo[client].type == SELECT_REOPEN) return;

	sockinfo[client].type = 0;
	sockinfo[client].callno = 0;

	if (sockinfo[client].fp) fclose ((sockinfo[client].fp));
	sockinfo[client].fp = NULL;
}

#ifdef LINUX
#ifdef DBOX

int ReadIRDatabase (void)
{

        int res,fl;
        char st[1024],*pnt,msg[256];
        FILE *fd;
        char *home = getenv("HOME");
 
        if (home)
            snprintf(st, sizeof(st), "%s/.irtrans/remotes", home);
        if (IRDataBaseRead) FreeDatabaseMemory ();
    else if (
           chdir ("./remotes")
        && !(home && chdir (st) == 0)
        && chdir ("/usr/local/share/irtrans/remotes")
        && chdir ("/usr/share/irtrans/remotes")
    ) return (ERR_NODATABASE);

        ReadRoutingTable ();
        ReadSwitches ();

        fd = popen ("ls","r");

        pnt = fgets (st,1000,fd);
        while (pnt) {

         if (pnt[strlen (pnt) - 1] == 10) pnt[strlen (pnt) - 1] = 0;
         fl = strlen (pnt) - 4;
         if (fl >= 1 && !strcmp (pnt + fl,".rem")) {
           res = DBReadCommandFile (pnt);
           if (res) {
                sprintf (msg,"Error %d reading DB-File %s\n",res,pnt);
                log_print (msg,LOG_ERROR);
           }
         }
         pnt = fgets (st,1000,fd);
        }

        pclose (fd);

        DBShowStatus ();

		ReadAppConfig ();

        IRDataBaseRead = 1;
        return (0);
}

#else   // Keine DBOX, normales LINUX

int ReadIRDatabase (void)
{

        int fd,i,len,pos,res,fl;
        long off;
        char st[2048],msg[256];
        struct dirent *di;
        char *home = getenv("HOME");
 
        if (home)
            snprintf(st, sizeof(st), "%s/.irtrans/remotes", home);
        if (IRDataBaseRead) FreeDatabaseMemory ();
    else if (
           chdir ("./remotes")
        && !(home && chdir (st) == 0)
        && chdir ("/usr/local/share/irtrans/remotes")
        && chdir ("/usr/share/irtrans/remotes")
    ) return (ERR_NODATABASE);

        ReadRoutingTable ();
        ReadSwitches ();

        fd = open (".",0);

        do {
         len = getdirentries (fd,st,2048,&off);

         pos = 0;
         while (pos < len) {
           di = (struct dirent *)&st[pos];
           fl = strlen (di -> d_name) - 4;
           if (fl >= 1 && !strcmp (di->d_name + fl,".rem")) {
                res = DBReadCommandFile (di->d_name);
                if (res) {
                      sprintf (msg,"Error %d reading DB-File %s\n",res,di->d_name);
                      log_print (msg,LOG_ERROR);
                }
           }
           pos += di -> d_reclen;
         }
        } while (len);

        close (fd);

        DBShowStatus ();
		ReadAppConfig ();

        IRDataBaseRead = 1;
        return (0);
}

#endif

#endif



#ifdef WIN32

int ReadIRDatabase (void)
{
	int res;
    struct _finddata_t c_file;
    long hFile;
	char msg[256];

	if (IRDataBaseRead) FreeDatabaseMemory ();
    else if (_chdir ("remotes")) return (ERR_NODATABASE);

    ReadRoutingTable ();
	ReadSwitches ();

	if((hFile = _findfirst( "*.rem", &c_file )) != -1L) {
		do {
			res = DBReadCommandFile (c_file.name);
			if (res) {
				sprintf (msg,"Error %d reading DB-File %s\n",res,c_file.name);
				log_print (msg,LOG_ERROR);
			}
		} while( _findnext( hFile, &c_file ) == 0);
		_findclose( hFile );
	}

	DBShowStatus ();
	ReadAppConfig ();

	IRDataBaseRead = 1;
	return (0);
}

#endif


void ExecuteNetCommand (SOCKET sockfd)
{
	int client;
	int res,sz,len;
	char err[255];
	char buffer[sizeof (CCFCOMMAND)];
	char buffer1[sizeof (CCFCOMMAND)];

	STATUSBUFFER stat;
	NETWORKCOMMAND *com;


	client = GetNetworkClient (sockfd);
	if (client == -1) {
		// Error not found
		return;
	}

	sockinfo[client].callno = seq_call++;
	
	com = (NETWORKCOMMAND *)buffer;

	while (1) {
		if (sockinfo[client].restlen) {
			memcpy (buffer,sockinfo[client].restdata,sockinfo[client].restlen + 1);
			
			sz = sockinfo[client].restread - sockinfo[client].restlen;

			res = recv (sockfd,buffer + sockinfo[client].restlen + 1,sz,MSG_NOSIGNAL);
			if (res < 0) {
#ifdef WIN32
				if (WSAGetLastError () == WSAEWOULDBLOCK) return;
#else
				if (errno == EAGAIN) return;
#endif
			}

			sockinfo[client].restlen = 0;
		}

		else {
			res = recv (sockfd,buffer,1,MSG_NOSIGNAL);

			if (res != 1 || *buffer < 1 || *buffer > COMMAND_EMPTY) {
				if (res < 0) {
		#ifdef WIN32
					if (WSAGetLastError () == WSAEWOULDBLOCK) return;
		#else
					if (errno == EAGAIN) return;
		#endif
				}
				if (res <= 0) CloseIRSocket (client);
				if (res == 0) {
					sprintf (err,"Client disconnect\n");
					log_print (err,LOG_INFO);
				}
				else {
					if (res == 1) sprintf (err,"Illegal Network command\n");
					if (res < 0)  sprintf (err,"Network connection closed\n");
					log_print (err,LOG_ERROR);
				}
				return;
			}

			sz = sizeof (NETWORKCOMMAND) - 1;
			if (*buffer == COMMAND_LCD) sz = sizeof (LCDCOMMAND) - 1;
			if (*buffer == COMMAND_STORETRANS) sz = sizeof (TRANSLATECOMMAND) - 1;
			if (*buffer == COMMAND_SENDCCF) sz = sizeof (CCFCOMMAND) - 1;

			res = recv (sockfd,buffer + 1,sz,MSG_NOSIGNAL);
		}
			
		if (res == sz - 4) {														// Altes Commandformat
			sz = res;
			memcpy (buffer1,buffer,res);
			memcpy (buffer,buffer1,8);
			memcpy (buffer+12,buffer1+8,res-8);
		}
		
		else if (res > 12 && (com->protocol_version / 100) != (PROTOCOL_VERSION / 100)) {
			sprintf (err,"ExecuteNetCommand: Illegal Protocol Version %d.%d (should be %d.%d)\n",
					 com->protocol_version/100,com->protocol_version%100,PROTOCOL_VERSION/100,PROTOCOL_VERSION%100);
			log_print (err,LOG_FATAL);
			return;
		}

		if (res != sz) {
			if (res <= 0) CloseIRSocket (client);
			if (res) {
				memcpy (sockinfo[client].restdata,buffer,res + 1);
				sockinfo[client].restlen = res;
				sockinfo[client].restread = sz;
			}
			return;
		}

		SwapNetworkcommand (com);
		DoExecuteNetCommand (client,com,&stat);

		stat.clientid = sockinfo[client].clientid;
		len = stat.statuslen;
		SwapNetworkstatus (&stat);

		sz = 0;
		while (sz < len) {
			res = send (sockfd,((char *)&stat) + sz,len - sz,MSG_NOSIGNAL);
			if (res > 0) sz += res;
			if (res == -1) msSleep (100);
		}

		if (res <= 0) {
			CloseIRSocket (client);
			sprintf (err,"IP Connection lost\n");
			log_print (err,LOG_ERROR);
		}
	}

}


void DoExecuteNetCommand (int client,NETWORKCOMMAND *com,STATUSBUFFER *stat)
{
	int res,i;
	byte bus;
	long cmd_num;
	IRDATA ird;
	IRRAW *irw;
	FILE *fp;
	HANDLE hfile;
	char st[255],err[255];
	byte dat[255];
	NETWORKSTATUS *ns;
	LCDCOMMAND *lcd;
	TRANSLATECOMMAND *tr;
	NETWORKLEARNSTAT *lstat;
	FUNCTIONBUFFER *fb;
	FUNCTIONBUFFEREX *fbex;
	NETWORKLCDSTAT *lcdb;
	unsigned long end_time;
	static byte suspend;


	memset (stat,0,sizeof (STATUSBUFFER));
	stat ->statuslen = 8;

	sprintf (st,"Netcommand: %d\n",com->netcommand);
	log_print (st,LOG_DEBUG);

	switch (com->netcommand) {

	case COMMAND_EMPTY:
		break;
	case COMMAND_MCE_CHARS:
		SetSpecialChars (dat);

		AdvancedLCD (LCD_DATA | LCD_DEFINECHAR,dat,dat[0] * 9 + 1);
		if (new_lcd_flag) msSleep (250);

		break;
	case COMMAND_LCDSTATUS:
		lcdb = (NETWORKLCDSTAT *)stat;
		memset (lcdb,0,sizeof (NETWORKLCDSTAT));
		lcdb->statustype = STATUS_LCDDATA;
		lcdb->statuslen = sizeof (NETWORKLCDSTAT);
		lcdb->numcol = 0;
		lcdb->numrows = 0;
		lcdb->clockflag = 0;

		if (new_lcd_flag == 1) {
			lcdb->clockflag = 3;
			lcdb->numcol = 20;
			lcdb->numrows = 4;
		}

		if (new_lcd_flag == 2) {
			lcdb->clockflag = 3;
			lcdb->numcol = 40;
			lcdb->numrows = 4;
		}

		if (new_lcd_flag) {
			display_bus = 255;
			return;
		}


		for (i=0;i < device_cnt;i++) if (IRDevices[i].version[0] == 'D' || (IRDevices[i].fw_capabilities & FN_DISPMASK)) {
			display_bus = i;
			break;
		}

		if (IRDevices[display_bus].version[0] == 'D') {
			lcdb->numcol = 16;
			lcdb->numrows = 2;
		}

		if ((IRDevices[display_bus].fw_capabilities & FN_DISPMASK) == FN_DISP1) {
			lcdb->numcol = 16;
			lcdb->numrows = 2;
		}
		if ((IRDevices[display_bus].fw_capabilities & FN_DISPMASK) == FN_DISP2) {
			lcdb->numcol = 20;
			lcdb->numrows = 4;
		}
		if (IRDevices[display_bus].fw_capabilities & FN_CLOCK) lcdb->clockflag |= 1;
		if (IRDevices[display_bus].io.advanced_lcd & 1) lcdb->clockflag |= 2;
		if (IRDevices[display_bus].io.advanced_lcd & 4) lcdb->clockflag |= 4;
		break;
	case COMMAND_SHUTDOWN:
		if (strcmp (com->remote,"XXXshutdownXXX")) {
			PutNetworkStatus (ERR_SHUTDOWN,NULL,stat);
			break;
		}
		LCDBrightness (4);
		LCDTimeCommand (LCD_DISPLAYTIME);
		sprintf (st,"IRTrans Server Shutdown via Client");
		log_print (st,LOG_FATAL);
		exit (0);
		break;
	case COMMAND_SUSPEND:
		suspend = 1;
		LCDTimeCommand (LCD_DISPLAYTIME);
		lcd_init = 1;
		LCDBrightness (4);
		break;
	case COMMAND_RESUME:
		suspend = 0;
		lcd_init = 1;
		LCDBrightness (5);
		break;
	case COMMAND_STORETRANS:
		tr = (TRANSLATECOMMAND *)com;
		StoreTransItem (tr);
		break;
	case COMMAND_SAVETRANS:
		res = FileTransData (com->remote);
		if (res) PutNetworkStatus (res,NULL,stat);
		break;
	case COMMAND_LOADTRANS:
		res = LoadTranslation ((TRANSLATEBUFFER *)stat,com->remote,(word)com->adress);
		if (res) PutNetworkStatus (res,NULL,stat);
		break;
	case COMMAND_FLASHTRANS:
		res = SetFlashdataEx ((byte)((com->adress) >> 8),(com->adress & 0xf));
		if (res) PutNetworkStatus (res,NULL,stat);
		break;
	case COMMAND_FUNCTIONS:
		fb = (FUNCTIONBUFFER *)stat;
		memset (fb,0,sizeof (FUNCTIONBUFFER));
		fb->statustype = STATUS_FUNCTION;
		fb->statuslen = sizeof (FUNCTIONBUFFER);
		fb->serno = IRDevices[com->adress].fw_serno;
		fb->functions = IRDevices[com->adress].fw_capabilities;
		break;
	case COMMAND_FUNCTIONEX:
		fbex = (FUNCTIONBUFFEREX *)stat;
		memset (fbex,0,sizeof (FUNCTIONBUFFEREX));
		fbex->statustype = STATUS_FUNCTIONEX;
		fbex->statuslen = sizeof (FUNCTIONBUFFEREX);
		fbex->serno = IRDevices[com->adress].fw_serno;
		fbex->functions = IRDevices[com->adress].fw_capabilities;
		memcpy (fbex->version,IRDevices[com->adress].version,8);
		break;
	case COMMAND_LONGSEND:
		end_time = GetMsTime () + com->timeout * 10;

		sprintf (st,"Longsend %s-%s for %d ms\n",com->remote,com->command,com->timeout * 10);
		log_print (st,LOG_DEBUG);
		res = DBFindRemoteCommandEx (com->remote,com->command,&ird);
		if (res) {
			ns = (NETWORKSTATUS *)stat;
			PutNetworkStatus (res,NULL,stat);
			strcpy (err,ns->message);
			if (res == ERR_REMOTENOTFOUND) sprintf (ns->message,err,com->remote);
			if (res == ERR_COMMANDNOTFOUND) sprintf (ns->message,err,com->command);
			log_print (ns->message,LOG_ERROR);
		}
		else SendIRDataEx (&ird,com->adress);

		resend_flag = 0;
		while (GetMsTime () < end_time) {
			if (!resend_flag) {											// 1. Resend; Command Laden
				strcat (com->command,"@");
				res = DBFindRemoteCommandEx (com->remote,com->command,&ird);
				if (res) {
					com->command[strlen (com->command) - 1] = 0;
					res = DBFindRemoteCommand (com->remote,com->command,&cmd_num,NULL);
					if (res) PutNetworkStatus (res,NULL,stat);
					else {
						bus = 0;
						if (com->adress & 0x10000) {
							ird.target_mask = (word)com->adress & 0xffff;
						}
						bus = (com->adress >> 20) & (MAX_IR_DEVICES - 1);
						if (com->adress & 0x40000000) bus = 255;
						if (com->adress & 0x60000) ird.address = (com->adress >> 17) & 0x3;
						ResendIREx (bus,&ird);
						resend_flag = 2;
					}
				}
				else {
					SendIRDataEx (&ird,com->adress);
					resend_flag = 1;
					com->command[strlen (com->command) - 1] = 0;
				}
				continue;
			}
			if (resend_flag == 1) strcat (com->command,"@");
			res = DBFindRemoteCommandEx (com->remote,com->command,&ird);
			if (res) PutNetworkStatus (res,NULL,stat);
			else {
				if (com->adress & 0x10000) {
					ird.target_mask = (word)com->adress & 0xffff;
				}
				bus = (com->adress >> 20) & (MAX_IR_DEVICES - 1);
				if (com->adress & 0x40000000) bus = 255;
				if (com->adress & 0x60000) ird.address = (com->adress >> 17) & 0x3;
				ResendIREx (bus,&ird);

			}
			if (resend_flag == 1) com->command[strlen (com->command) - 1] = 0;
		}
		break;
	case COMMAND_DELETECOM:
		sprintf (st,"Delete %s-%s\n",com->remote,com->command);
		log_print (st,LOG_DEBUG);
		strcpy (st,com->remote);
		if (com->adress != 1234) return;
		if (strcmp (st + strlen (st) - 4,".rem")) strcat (st,".rem");
		fp = DBOpenFile (st,"r+");
		if (!fp) {
			PutNetworkStatus (ERR_OPENASCII,com->remote,stat);
			return;
		}
		if (!ASCIIFindCommand (fp,com->command,NULL)) {
			ns = (NETWORKSTATUS *)stat;
			PutNetworkStatus (ERR_COMMANDNOTFOUND,NULL,stat);
			strcpy (err,ns->message);
			sprintf (ns->message,err,com->command);
			return;
		}
		res = ftell (fp);
		fclose (fp);
#ifdef WIN32
		hfile = CreateFile (st,GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		if (hfile) {
			SetFilePointer (hfile,res,NULL,FILE_BEGIN);
			SetEndOfFile (hfile);
			CloseHandle (hfile);
		}
#endif
#ifdef LINUX
		truncate (st,res);
#endif

		ReadIRDatabase ();
		break;
	case COMMAND_SEND:
		sprintf (st,"Send %s-%s [%x]\n",com->remote,com->command,com->adress);
		log_print (st,LOG_DEBUG);
		if (hexfp) {
			fprintf (hexfp,"%s-%s\n",com->remote,com->command);
			hexflag = 1;
		}

		res = DBFindRemoteCommand (com->remote,com->command,&cmd_num,NULL);
		if (!res) res = SendIR (cmd_num,com->adress);
		if (res) {
			ns = (NETWORKSTATUS *)stat;
			PutNetworkStatus (res,NULL,stat);
			strcpy (err,ns->message);
			if (res == ERR_REMOTENOTFOUND) sprintf (ns->message,err,com->remote);
			if (res == ERR_COMMANDNOTFOUND) sprintf (ns->message,err,com->command);
			if (res == ERR_WRONGBUS) sprintf (ns->message,err,(com->adress >> 20) & (MAX_IR_DEVICES - 1));
			log_print (ns->message,LOG_ERROR);
		}

		resend_flag = 0;
		break;
	case COMMAND_DEVICEDATA:
		sprintf (st,"Get Devicedata %s-%s\n",com->remote,com->command);
		log_print (st,LOG_DEBUG);
		res = DBFindRemoteCommand (com->remote,com->command,&cmd_num,NULL);
		if (!res) res = GetDeviceData (cmd_num,(DATABUFFER *)stat);
		if (res) {
			ns = (NETWORKSTATUS *)stat;
			PutNetworkStatus (res,NULL,stat);
			strcpy (err,ns->message);
			if (res == ERR_REMOTENOTFOUND) sprintf (ns->message,err,com->remote);
			if (res == ERR_COMMANDNOTFOUND) sprintf (ns->message,err,com->command);
			log_print (ns->message,LOG_ERROR);
		}
		break;
	case COMMAND_TESTCOM:
		res = DBFindRemoteCommand (com->remote,com->command,&cmd_num,NULL);
		if (res) {
			ns = (NETWORKSTATUS *)stat;
			PutNetworkStatus (ERR_TESTCOM,NULL,stat);
			ns->statuslevel = (word)com->adress;
			ns->statuslen -= 256;
		}
		break;
	case COMMAND_TEMP:
		sprintf (st,"Temperature %s-%d\n",com->remote,com->adress);
		log_print (st,LOG_DEBUG);
		memset (&ird,0,sizeof (IRDATA));
		ird.ir_length = 5;
		ird.target_mask = 1 << com->adress;
		if (!strcmp (com->remote,"get") || !strcmp (com->remote,"Get") || !strcmp (com->remote,"GET")) {
			ird.mode = TEMP_DATA | TEMP_GET;
		}
		if (!strcmp (com->remote,"reset") || !strcmp (com->remote,"Reset") || !strcmp (com->remote,"RESET")) {
			ird.mode = TEMP_DATA | TEMP_RESET;
		}
		if (ird.mode == 0) {
			ns = (NETWORKSTATUS *)stat;
			PutNetworkStatus (ERR_TEMPCOMMAND,NULL,stat);
			strcpy (err,ns->message);
			sprintf (ns->message,err,com->remote);
			log_print (ns->message,LOG_ERROR);
		}
		else SendIRDataEx (&ird,com->adress);
		break;
	case COMMAND_STARTCLOCK:
		LCDTimeCommand (LCD_DISPLAYTIME);
		lcd_init = 1;
		break;
	case COMMAND_LCD:
		lcd = (LCDCOMMAND *)com;
		irw = (IRRAW *)&ird;
		memset (irw,0,sizeof (IRRAW));
		if (lcd->lcdcommand & LCD_TEXT) {
			sprintf (st,"LCD: %s\n",lcd->framebuffer);
			log_print (st,LOG_DEBUG);
			compress_lcdbuffer (lcd,irw->data,display_bus);
			sprintf (st,"LCD-LEN: %d\n",strlen (irw->data));
			log_print (st,LOG_DEBUG);
		}

		if (suspend) return;
		irw->target_mask = (unsigned short)(lcd->adress & 0xffff);
		if (lcd->adress == 'L') irw->target_mask = 0xffff;
		irw->ir_length = 0;
		irw->mode = LCD_DATA | lcd->lcdcommand;
		irw->transmit_freq = lcd->timeout;
		SendLCD (irw,lcd->adress);
		resend_flag = 0;
		if (new_lcd_flag) msSleep (100);
//		printf ("<%s>\n",irw->data);
		//lcd_init = 1; // Debug für LCD Test
		break;
	case COMMAND_SETSWITCH:
		memset (&ird,0,sizeof (IRDATA));
		sprintf (st,"SWITCH: %d - %d\n",com->remote[0],com->command[0]);
		log_print (st,LOG_DEBUG);
		
		ird.target_mask = 0xffff;
		ird.ir_length = 0;
		ird.mode = SWITCH_DATA | com->command[0];
		ird.transmit_freq = com->remote[0];
		DoSendIR (&ird,0,0);						// !!!!!! Auf Multibus erweitern
		resend_flag = 0;
		break;
	case COMMAND_LCDINIT:
		lcd = (LCDCOMMAND *)com;
		irw = (IRRAW *)&ird;
		memset (irw,0,sizeof (IRRAW));
		memcpy (irw->data,lcd->framebuffer,40);
		sprintf (st,"LCDINIT: %s\n",lcd->framebuffer);
		log_print (st,LOG_DEBUG);

		irw->target_mask = (unsigned short)lcd->adress;
		if (lcd->adress == 'L') irw->target_mask = 0xffff;
		irw->ir_length = 0;
		irw->mode = LCD_DATA | LCD_INIT;
		irw->transmit_freq = lcd->timeout;
		SendLCD (irw,lcd->adress);
		resend_flag = 0;
		break;
	case COMMAND_RESEND:
		sprintf (st,"Resend %s-%s\n",com->remote,com->command);
		log_print (st,LOG_DEBUG);
		if (!resend_flag) {											// 1. Resend; Command Laden
			strcat (com->command,"@");
			res = DBFindRemoteCommandEx (com->remote,com->command,&ird);
			if (res) {
				com->command[strlen (com->command) - 1] = 0;
				res = DBFindRemoteCommandEx (com->remote,com->command,&ird);
				if (res) PutNetworkStatus (res,NULL,stat);
				else {
					if (com->adress & 0x10000) {
						ird.target_mask = (word)com->adress & 0xffff;
					}
					if (com->adress & 0x60000) ird.address = (com->adress >> 17) & 0x3;
					bus = (byte)((com->adress >> 20) & MAX_IR_DEVICES - 1);
					if (com->adress & 0x40000000) bus = 255;
					ResendIREx (bus,&ird);
					resend_flag = 2;
				}
			}
			else {
				SendIRDataEx (&ird,com->adress);
				resend_flag = 1;
			}
			break;
		}
		if (resend_flag == 1) strcat (com->command,"@");
		res = DBFindRemoteCommandEx (com->remote,com->command,&ird);
		if (res) PutNetworkStatus (res,NULL,stat);
		else {
			if (com->adress & 0x10000) {
				ird.target_mask = (word)com->adress & 0xffff;
			}
			if (com->adress & 0x60000) ird.address = (com->adress >> 17) & 0x3;
			bus = (byte)((com->adress >> 20) & MAX_IR_DEVICES - 1);
			if (com->adress & 0x40000000) bus = 255;
			ResendIREx (bus,&ird);
		}
		break;
	case COMMAND_LRNREM:
		if (sockinfo[client].fp != NULL) fclose (sockinfo[client].fp);
		sockinfo[client].fp = ASCIIOpenRemote (com->remote,&sockinfo[client]);
		if (sockinfo[client].learnstatus.num_timings > 0) sockinfo[client].learnstatus.learnok = 1;
		if (sockinfo[client].fp == NULL) PutNetworkStatus (ERR_OPENASCII,com->remote,stat);
		break;
	case COMMAND_LRNTIM:
		if (sockinfo[client].fp == NULL) {
			PutNetworkStatus (ERR_NOFILEOPEN,NULL,stat);
			break;
		}
		res = LearnIREx (&sockinfo[client].ird,(word)com->adress,com->timeout,(word)(com->adress >> 16));

		if (res) PutNetworkStatus (res,NULL,stat);
		else {
			if (com -> trasmit_freq && sockinfo[client].ird.transmit_freq != 255) sockinfo[client].ird.transmit_freq = com -> trasmit_freq;
			sockinfo[client].timing = ASCIIStoreTiming (sockinfo[client].fp,&sockinfo[client].ird,&sockinfo[client].learnstatus);
			last_adress = sockinfo[client].ird.address;
			ResultStoreTiming (&sockinfo[client].ird,(NETWORKTIMING *)stat);
			sockinfo[client].learnstatus.learnok = 1;
		}
		break;
	case COMMAND_LRNRAW:
		if (sockinfo[client].fp == NULL) {
			PutNetworkStatus (ERR_NOFILEOPEN,NULL,stat);
			break;
		}
		res = LearnRawIREx ((IRRAW *)&sockinfo[client].ird,(word)com->adress,com->timeout,(word)(com->adress >> 16));

		if (res) PutNetworkStatus (res,NULL,stat);
		else {
			if (ASCIIFindCommand (sockinfo[client].fp,com->command,sockinfo + client) == 0) sockinfo[client].learnstatus.num_commands++;
			if (com -> trasmit_freq && sockinfo[client].ird.transmit_freq != 255) sockinfo[client].ird.transmit_freq = com -> trasmit_freq;
			ASCIIStoreRAW (sockinfo[client].fp,(IRRAW *)&sockinfo[client].ird,com->command);
			last_adress = sockinfo[client].ird.address;
		}
		break;
	case COMMAND_LRNRAWRPT:
		if (sockinfo[client].fp == NULL) {
			PutNetworkStatus (ERR_NOFILEOPEN,NULL,stat);
			break;
		}
		res = LearnRawIRRepeatEx ((IRRAW *)&sockinfo[client].ird,(word)com->adress,com->timeout,(word)(com->adress >> 16));

		if (res) PutNetworkStatus (res,NULL,stat);
		else {
			if (com -> trasmit_freq && sockinfo[client].ird.transmit_freq != 255) sockinfo[client].ird.transmit_freq = com -> trasmit_freq;
			strcat (com->command,"@");
			if (ASCIIFindCommand (sockinfo[client].fp,com->command,sockinfo + client) == 0) sockinfo[client].learnstatus.num_commands++;
			ASCIIStoreRAW (sockinfo[client].fp,(IRRAW *)&sockinfo[client].ird,com->command);
			last_adress = sockinfo[client].ird.address;
		}
		break;
	case COMMAND_LRNCOM:
		if (sockinfo[client].fp == NULL) {
			PutNetworkStatus (ERR_NOFILEOPEN,NULL,stat);
			break;
		}
		if (sockinfo[client].ird.ir_length == 0) {
			PutNetworkStatus (ERR_NOTIMING,NULL,stat);
			break;
		}
		res = LearnNextIREx (&sockinfo[client].ird,(word)com->adress,com->timeout,(word)(com->adress >> 16));
		if (res) PutNetworkStatus (res,NULL,stat);
		else {
			if (ASCIIFindCommand (sockinfo[client].fp,com->command,sockinfo + client) == 0) sockinfo[client].learnstatus.num_commands++;
			res = ASCIIStoreCommand (sockinfo[client].fp,&sockinfo[client].ird,com->command,sockinfo[client].timing,0);
			memcpy (sockinfo[client].learnstatus.received,sockinfo[client].ird.data,sockinfo[client].ird.ir_length);
		}
		break;

	case COMMAND_LRNLONG:
		if (sockinfo[client].fp == NULL) {
			PutNetworkStatus (ERR_NOFILEOPEN,NULL,stat);
			break;
		}
		res = LearnIREx (&sockinfo[client].ird,(word)com->adress,com->timeout,(word)(com->adress >> 16));

		if (res) PutNetworkStatus (res,NULL,stat);
		else {
			if (com -> trasmit_freq && sockinfo[client].ird.transmit_freq != 255) sockinfo[client].ird.transmit_freq = com -> trasmit_freq;
			sockinfo[client].timing = ASCIIStoreTiming (sockinfo[client].fp,&sockinfo[client].ird,&sockinfo[client].learnstatus);
			if (ASCIIFindCommand (sockinfo[client].fp,com->command,sockinfo + client) == 0) sockinfo[client].learnstatus.num_commands++;
			ASCIIStoreCommand (sockinfo[client].fp,&sockinfo[client].ird,com->command,sockinfo[client].timing,0);
			last_adress = sockinfo[client].ird.address;
			memcpy (sockinfo[client].learnstatus.received,sockinfo[client].ird.data,sockinfo[client].ird.ir_length);
//			ResultStoreTiming (&sockinfo[client].ird,(NETWORKTIMING *)stat);
//			stat->adress = last_adress | ((com->adress,com >> 8) & (MAX_IR_DEVICES - 1));

		}
		break;

	case COMMAND_LRNTOG:
		if (sockinfo[client].fp == NULL) {
			PutNetworkStatus (ERR_NOFILEOPEN,NULL,stat);
			break;
		}
		res = LearnNextIREx (&sockinfo[client].ird,(word)com->adress,com->timeout,(word)(com->adress >> 16));
		if (res) PutNetworkStatus (res,NULL,stat);
		else {
			res = ASCIIFindToggleSeq (sockinfo[client].fp,&sockinfo[client].ird,com->command);
			if (res < 0) PutNetworkStatus (ERR_TOGGLE_DUP,NULL,stat);
			else {
				res = ASCIIStoreCommand (sockinfo[client].fp,&sockinfo[client].ird,com->command,sockinfo[client].timing,res);
				sockinfo[client].learnstatus.num_commands++;
				memcpy (sockinfo[client].learnstatus.received,sockinfo[client].ird.data,sockinfo[client].ird.ir_length);
			}
		}
		break;

	case COMMAND_LRNRPT:
		if (sockinfo[client].fp == NULL) {
			PutNetworkStatus (ERR_NOFILEOPEN,NULL,stat);
			break;
		}
		res = LearnRepeatIREx (&sockinfo[client].ird,(word)com->adress,com->timeout,(word)(com->adress >> 16));

		if (res) PutNetworkStatus (res,NULL,stat);
		else {
			if (com -> trasmit_freq && sockinfo[client].ird.transmit_freq != 255) sockinfo[client].ird.transmit_freq = com -> trasmit_freq;
			sockinfo[client].timing = ASCIIStoreTiming (sockinfo[client].fp,&sockinfo[client].ird,&sockinfo[client].learnstatus);
			strcat (com->command,"@");
			if (ASCIIFindCommand (sockinfo[client].fp,com->command,sockinfo + client) == 0) sockinfo[client].learnstatus.num_commands++;
			ASCIIStoreCommand (sockinfo[client].fp,&sockinfo[client].ird,com->command,sockinfo[client].timing,0);
			last_adress = sockinfo[client].ird.address;
			memcpy (sockinfo[client].learnstatus.received,sockinfo[client].ird.data,sockinfo[client].ird.ir_length);
		}
		break;

	case COMMAND_CLOSE:
		if (sockinfo[client].fp == NULL) {
			PutNetworkStatus (ERR_NOFILEOPEN,NULL,stat);
			break;
		}
		if (sockinfo[client].fp) fclose ((sockinfo[client].fp));
		sockinfo[client].fp = NULL;
		memset (sockinfo[client].learnstatus.remote,' ',80);
		memset (sockinfo[client].learnstatus.received,' ',80);
		ReadIRDatabase ();
		break;

	case COMMAND_RELOAD:
		if (sockinfo[client].fp) fflush ((sockinfo[client].fp));
		ReadIRDatabase ();
		break;
	case COMMAND_STATUS:
		if (status_changed || (time (0) - last_status_read) >= status_cache_timeout) {
			res = GetBusInfoEx (remote_statusex,0);
			if (res) {
				PutNetworkStatus (res,NULL,stat);
				break;
			}
			else {
				status_changed = 0;
				last_status_read = time (0);
			}
		}
		PutDeviceStatus ((NETWORKMODE *)stat);
		break;

	case COMMAND_STATUSEX:
		if (status_changed || (time (0) - last_status_read) >= status_cache_timeout) {
			res = GetBusInfoEx (remote_statusex,100);
			if (res) {
				PutNetworkStatus (res,NULL,stat);
				break;
			}
			else {
				status_changed = 0;
				last_status_read = time (0);
			}
		}
		PutDeviceStatusEx ((NETWORKMODEEX *)stat);
		break;

	case COMMAND_LEARNSTAT:
		if (sockinfo[client].fp == NULL) {
			memset (stat,0,sizeof (NETWORKLEARNSTAT));
			stat->statustype = STATUS_LEARN;
			stat->statuslen = sizeof (NETWORKLEARNSTAT);
			lstat = (NETWORKLEARNSTAT *)stat;
			memset (lstat->remote,' ',80);
			memset (lstat->received,' ',CODE_LEN);
		}
		else memcpy (stat,&sockinfo[client].learnstatus,sizeof (NETWORKLEARNSTAT));
		memset (sockinfo[client].learnstatus.received,' ',CODE_LEN);
		break;

	case COMMAND_RESET:
		lcd_init = 1;
		res = ResetTransceiverEx ((byte)((com->adress >> 8) & (MAX_IR_DEVICES-1)));
		if (res) PutNetworkStatus (res,NULL,stat);
		break;
	case COMMAND_SETSTAT:
		res = GetHotcode (com->remote,com->command,st);
		if (res == -1) {
			GetError (ERR_HOTCODE,st);
			sprintf (err,st,com->remote,com->command);
			log_print (err,LOG_ERROR);
			PutNetworkStatus (ERR_HOTCODE,err,stat);
		}
		else {
			StoreSwitch ((word)com->timeout,0,com->remote,com->command,1);
			WriteSwitches ();
			res = SetTransceiverModusEx ((byte)(com->timeout >> 8),com->mode,(word)com->adress,(byte)(com->timeout & 0xff),st,res,(byte)((com->adress >> 16) & 0xff),com->trasmit_freq);
			if (res) PutNetworkStatus (res,NULL,stat);
			msSleep (250);
			status_changed = 1;
			LCDTimeCommand (LCD_REFRESHDATE);
		}
		break;
	case COMMAND_GETREMOTES:
		GetRemoteDatabase ((REMOTEBUFFER *)stat,com->adress);
		break;
	case COMMAND_GETCOMMANDS:
		GetCommandDatabase ((COMMANDBUFFER *)stat,com->remote,com->adress);
		break;
	}

}

// Time with 1ms resolution

unsigned long GetMsTime (void)
{
#ifdef WIN32
	return (GetTickCount ());
#endif

#ifdef LINUX
	struct timeval tv;
	gettimeofday (&tv,0);

	return (tv.tv_sec * 1000 + tv.tv_usec);
#endif
}


char shadow_buf[256];

int GetDeviceStatus (STATUSBUFFER *buf)
{
	int res;

	if (status_changed || (time (0) - last_status_read) >= status_cache_timeout) {
		res = GetBusInfoEx (remote_statusex,0);
		if (res) {
			PutNetworkStatus (res,NULL,buf);
			return (res);
		}
		else {
			status_changed = 0;
			last_status_read = time (0);
		}
	}
	PutDeviceStatus ((NETWORKMODE *)buf);
	return (0);
}




void compress_lcdbuffer (LCDCOMMAND *lcd,char *buf,int bus)
{
	int i = 0;
	int pos = 0;
	char line[100];
	char lf[2];
	int x,y;


	byte LINEFEED = 0x1f;
	byte C_SPACE = 16;
	byte C_BLOCK = 17;
	byte C_DATA = 18;

	if (bus == 255) bus = 0;

	if ((IRDevices[bus].io.advanced_lcd & 2) || new_lcd_flag) {
		LINEFEED = 10;
		C_SPACE = 11;
		C_BLOCK = 12;
		C_DATA = 13;
		ConvertLCDCharset ((byte *)(lcd->framebuffer));
	}
	else if (IRDevices[bus].version[0] == 'D') ConvertLCDCharset ((byte *)(lcd->framebuffer));

	lf[0] = LINEFEED;
	lf[1] = 0;


	buf[0] = 0;
	if (count_difference (lcd) <= 24) {
		i = 0;
		buf[i++] = C_DATA;
		for (y = 0;y < lcd->hgt;y++) {
			for (x = 0;x < lcd->wid;x++) {
				if (shadow_buf[x + y * lcd->wid] != lcd->framebuffer[x + y * lcd->wid] && lcd->framebuffer[x + y * lcd->wid]) {
					buf[i++] = (x + 1) | y << 6;
					buf[i++] = lcd->framebuffer[x + y * lcd->wid];
				}
			}
		}
		buf[i++] = 0;
	}
	else for (i=0;i < lcd -> hgt;i++) {
		memset (line,0,100);
		memcpy (line,lcd->framebuffer + i * lcd->wid,lcd->wid);
		pos = strlen (line) - 1;
		while (line[pos] == ' ' && pos) pos--;
		if (line[pos + 1] == ' ') line[pos + 1] = 0;
		compress_char (line,' ',C_SPACE);
		compress_char (line,(char)255,C_BLOCK);
		strcat (line,lf);
		strcat (buf,line);
	}

	memcpy (shadow_buf,lcd->framebuffer,lcd->wid * lcd->hgt);

	if (!(IRDevices[bus].io.advanced_lcd & 2) && !new_lcd_flag && IRDevices[bus].version[0] != 'D') buf[strlen(buf)-1] = 0;
}


int count_difference (LCDCOMMAND *lcd)
{
	int i = 0;
	int diff = 0;
	if (lcd_init) {
		lcd_init = 0;
		return (99);
	}
	while (i < lcd->wid * lcd->hgt) {
		if (shadow_buf[i] != lcd->framebuffer[i]) diff++;
		i++;
	}
	return (diff);
}


void compress_char (char ln[],char src,char tar)
{
	char st[100];
	int i = 0;
	int pos;
	while (ln[i]) {
		if (ln[i] == src) {
			pos = i;
			while (ln[i] == src) i++;
			if ((i - pos) > 2) {
				strcpy (st,ln + i);
				ln[pos] = tar;
				ln[pos+1] = i - pos;
				strcpy (ln + pos + 2,st);
				i = pos + 2;
			}
		}
		i++;
	}
}


int GetHotcode (char rem[],char com[],byte data[])
{
	IRDATA ir;
	int i = 0;

	*data = 0;

	if (*rem == 0 || *com == 0) return (0);

	if (DBFindRemoteCommandEx (rem,com,&ir)) return (-1);
	if (ir.mode & RAW_DATA) {
		memcpy (data,ir.pause_len,ir.ir_length);
		ReformatHotcode (data,ir.ir_length);
		return (ir.ir_length);
	}
	else {
		memcpy (data,ir.data,ir.ir_length);
		return (ir.ir_length);
	}
}


void ReformatHotcode (byte data[],int len)
{
	int i = 0;
	word wert;
	while (i < len) {
		if (!data[i]) {
			i++;
			wert = (data[i] << 8) + data[i+1];
			wert -= (wert >> 4) + RAW_TOLERANCE;
			data[i] = wert >> 8;
			data[i+1] = wert & 0xff;
			i += 2;
		}
		else {
			data[i] -= (data[i] >> 4) + RAW_TOLERANCE;
			i++;
		}
	}

}

void PutDeviceStatus (NETWORKMODE *mode)
{
	int i;
	mode->adress = remote_statusex[0].my_adress;
	mode->statustype = STATUS_DEVICEMODE;
	mode->statuslen = sizeof (NETWORKMODE);

	for (i=0;i < 16;i++) {
		if (i == mode->adress) mode->stat[i].features = IRDevices[0].fw_capabilities;
		else mode->stat[i].features = remote_statusex[0].stat[i].capabilities;
		mode->stat[i].extended_mode = remote_statusex[0].stat[i].extended_mode;
		mode->stat[i].extended_mode2 = remote_statusex[0].stat[i].extended_mode2;
		mode->stat[i].device_mode = remote_statusex[0].stat[i].device_mode;
		mode->stat[i].send_mask = remote_statusex[0].stat[i].send_mask;
		memcpy (mode->stat[i].version,remote_statusex[0].stat[i].version,10);
		if (mode->stat[i].version[0]) {
			FindSwitch ((word)i,0,mode->stat[i].remote,mode->stat[i].command,&mode->stat[i].switch_mode);
		}

	}
}

void PutDeviceStatusEx (NETWORKMODEEX *mode)
{
	int i,bus;
	mode->statustype = STATUS_DEVICEMODEEX;
	mode->statuslen = sizeof (NETWORKMODEEX);
	mode->count = device_cnt;
	for (bus=0;bus < device_cnt;bus++) {
		mode->dev_adr[bus] = remote_statusex[bus].my_adress;
		for (i=0;i < 16;i++) {
			if (i == mode->dev_adr[bus]) mode->stat[bus][i].features = IRDevices[bus].fw_capabilities;
			else mode->stat[bus][i].features = remote_statusex[bus].stat[i].capabilities;
			mode->stat[bus][i].extended_mode = remote_statusex[bus].stat[i].extended_mode;
			mode->stat[bus][i].extended_mode2 = remote_statusex[bus].stat[i].extended_mode2;
			mode->stat[bus][i].device_mode = remote_statusex[bus].stat[i].device_mode;
			mode->stat[bus][i].send_mask = remote_statusex[bus].stat[i].send_mask;
			memcpy (mode->stat[bus][i].version,remote_statusex[bus].stat[i].version,10);
			if (mode->stat[bus][i].version[0]) {
				FindSwitch ((word)(i + bus * 16),0,mode->stat[bus][i].remote,mode->stat[bus][i].command,&mode->stat[bus][i].switch_mode);
			}

		}
	}

}

void PutNetworkStatus (int res,char msg[],STATUSBUFFER *buf)
{

	NETWORKSTATUS *stat;
	char txt[256];

	stat = (NETWORKSTATUS *)buf;

	stat->statustype = STATUS_MESSAGE;
	
	if (!res) {
		stat->statuslen = 8;
		return;
	}

	stat->netstatus = res;
	stat->statuslen = sizeof (NETWORKSTATUS);

	if (res == ERR_TIMEOUT) {
		stat->statuslevel = IRTIMEOUT;
		sprintf (stat->message,"Timeout");
		return;
	}
	
	if (res == ERR_OPENASCII) {
		stat->statuslevel = FATAL;
		sprintf (stat->message,"Error opening Remote %s",msg);
		return;
	}

	if (res == ERR_NOFILEOPEN) {
		stat->statuslevel = FATAL;
		sprintf (stat->message,"No Remote open");
		return;
	}

	stat->statuslevel = IR;

	if (msg) {
		strcpy (stat->message,msg);
		return;
	}
	
	GetError (res,txt);
	sprintf (stat->message,"IR Error: %s",txt);
}

void ResultStoreTiming (IRDATA *ird,NETWORKTIMING *stat)
{
	stat->statustype = STATUS_TIMING;
	stat->statuslen = sizeof (NETWORKTIMING);

	stat->timing.mode = ird->mode;
	stat->timing.time_cnt = ird->time_cnt;
	stat->timing.ir_repeat = ird->ir_repeat;
	stat->timing.repeat_pause = ird->repeat_pause;
	memcpy (stat->timing.pause_len,ird->pause_len,12);
	memcpy (stat->timing.pulse_len,ird->pulse_len,12);
	memcpy (stat->timing.data,ird->data,CODE_LEN);
	stat->adress = ird->address;
}


void SwapNetworkheader (NETWORKSTATUS *ns)
{
	if (!byteorder) return;

	swap_long (&ns->clientid);
	swap_word (&ns->statuslen);
	swap_word (&ns->statustype);
	swap_word (&ns->adress);
}


void SwapNetworkcommand (NETWORKCOMMAND *nc)
{
	if (!byteorder) return;

	swap_long (&nc->adress);
	swap_word (&nc->timeout);
}

void SwapNetworkstatus (void *pnt)
{
	int i;
	word type;
	NETWORKSTATUS *ns;
	NETWORKTIMING *nt;
	NETWORKMODE *nm;
	REMOTEBUFFER *rb;
	COMMANDBUFFER *cb;
	FUNCTIONBUFFER *fb;

	if (!byteorder) return;

	ns = (NETWORKSTATUS *)pnt;
	type = ns->statustype;

	SwapNetworkheader ((NETWORKSTATUS *)pnt);

	if (type == STATUS_MESSAGE) {
		swap_word (&ns->netstatus);
		swap_word (&ns->statuslevel);
		return;
	}
	if (type == STATUS_TIMING) {
		nt = (NETWORKTIMING *)pnt;
		for (i=0;i < TIME_LEN;i++) {
			swap_word (&nt->timing.pause_len[i]); 
			swap_word (&nt->timing.pulse_len[i]);
		}
		return;
	}
	if (type == STATUS_DEVICEMODE) {
		nm = (NETWORKMODE *)pnt;
		for (i=0;i<16;i++) swap_long (&nm->stat[i].send_mask);
		return;
	}
	if (type == STATUS_REMOTELIST) {
		rb = (REMOTEBUFFER *)pnt;
		swap_word (&rb->offset);
		swap_word (&rb->count_buffer);
		swap_word (&rb->count_remaining);
		swap_word (&rb->count_total);
		for (i=0;i<rb->count_buffer;i++) {
			swap_long (&rb->remotes[i].source_mask);
			swap_long (&rb->remotes[i].target_mask);
		}
		return;
	}
	if (type == STATUS_COMMANDLIST) {
		cb = (COMMANDBUFFER *)pnt;
		swap_word (&cb->offset);
		swap_word (&cb->count_buffer);
		swap_word (&cb->count_remaining);
		swap_word (&cb->count_total);
		return;
	}
	if (type == STATUS_FUNCTION) {
		fb = (FUNCTIONBUFFER *)pnt;
		swap_long (&fb->functions);
		swap_long (&fb->serno);
		return;
	}
}

int GetNetworkClient (SOCKET sockfd)
{

	int i = 0;
	while (i < CLIENT_COUNT) {
		if ((sockinfo[i].type == SELECT_SERVER || sockinfo[i].type == SELECT_REOPEN) 
			&& sockfd == sockinfo[i].fd) return (i);
		i++;
	}
	return (-1);
}

int InitServerSocket (SOCKET *sock,SOCKET *lirc, SOCKET *udp,SOCKET *web)
{
	char msg[256],hub[50];
	struct sockaddr_in serv_addr;

#ifdef LINUX
	int res,new = 1;
	struct stat s;
	struct sockaddr_un serv_addr_un;
#endif

#ifdef WIN32

	int err,res;
    WORD	wVersionRequired;
    WSADATA	wsaData;
    wVersionRequired = MAKEWORD(2,2);
    err = WSAStartup(wVersionRequired, &wsaData);
    if (err != 0) exit(1);

#endif
// ************************************************************* LIRC local Socket
#ifdef LINUX
	local_socket = socket(AF_UNIX,SOCK_STREAM,0);
	if (local_socket == -1) return (ERR_OPENSOCKET);

	res = stat (LIRCD,&s);
    if (res == -1 && errno != ENOENT) {
        close (local_socket);
		return (ERR_OPENSOCKET);
		}

	if(res != -1) {
		new = 0;
		res = unlink (LIRCD);
		if (res == -1 && errno != ENOENT) {
			close (local_socket);
			return (ERR_OPENSOCKET);
			}
        }

	serv_addr_un.sun_family = AF_UNIX;
	strcpy (serv_addr_un.sun_path,LIRCD);

	if (bind (local_socket,(struct sockaddr *)&serv_addr_un,sizeof(serv_addr)) == -1) {
		close(local_socket);
		return (ERR_OPENSOCKET);
		}

	if (new) chmod (LIRCD,PERMISSIONS);
	else {
		chmod(LIRCD,s.st_mode);
		chown(LIRCD,s.st_uid,s.st_gid);
	}

	listen(local_socket,3);

#endif

// ************************************************************* IRTrans Socket
	*sock = socket (PF_INET,SOCK_STREAM,0);
	if (*sock < 0) return (ERR_OPENSOCKET);

	if (res = ConfigSocket (sock,TCP_PORT)) return (res);

// ************************************************************* LIRC TCP/IP Socket
	*lirc = socket (PF_INET,SOCK_STREAM,0);
	if (*lirc < 0) return (ERR_OPENSOCKET);

	if (mode_flag & NO_LIRC) {
		*lirc = 0;
	}
	else if (res = ConfigSocket (lirc,LIRC_PORT)) return (res);
// ************************************************************* Web Socket
	*web = socket (PF_INET,SOCK_STREAM,0);
	if (*web < 0) return (ERR_OPENSOCKET);

	if (mode_flag & NO_WEB) {
		*web = 0;
	}
	else {
		if (webport) {
			if (res = ConfigSocket (web,webport)) return (ERR_BINDWEB);
		}

		else {
			if (res = ConfigSocket (web,webport = WEB_PORT)) {
				if (ConfigSocket (web,webport = ALTERNATE_WEB)) return (ERR_BINDWEB);
			}
		}
	}

// ************************************************************* UDP Socket
	*udp = socket (PF_INET,SOCK_DGRAM,0);
	if (*udp < 0) return (ERR_OPENSOCKET);

	memset (&serv_addr,0,sizeof (serv_addr));
	serv_addr.sin_family = AF_INET;
#ifdef DBOX
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = UDP_PORT;
#else
	serv_addr.sin_addr.s_addr = htonl (INADDR_ANY);
	serv_addr.sin_port = htons (UDP_PORT);
#endif

	if (bind (*udp,(struct sockaddr *)&serv_addr,sizeof (serv_addr)) < 0) return (ERR_BINDSOCKET);

// ************************************************************* UDP Relay Socket
	if (udp_relay_port) {
		udp_relay_socket = socket (PF_INET,SOCK_DGRAM,0);
		if (udp_relay_socket < 0) return (ERR_OPENSOCKET);

		memset (&serv_addr,0,sizeof (serv_addr));
		serv_addr.sin_family = AF_INET;

		serv_addr.sin_addr.s_addr = *((unsigned long *)gethostbyname (udp_relay_host)->h_addr);
		serv_addr.sin_port = htons ((word)udp_relay_port);

		if (connect (udp_relay_socket,(struct sockaddr *)&serv_addr,sizeof (serv_addr)) < 0) return (ERR_BINDSOCKET);
	}


	if (mode_flag & XAP) {
		xAP_rcv_port = XAP_PORT;
		xAP_rcv = socket (PF_INET,SOCK_DGRAM,0);
		if (xAP_rcv < 0) return (ERR_OPENSOCKET);

		memset (&serv_addr,0,sizeof (serv_addr));
		serv_addr.sin_family = AF_INET;

		serv_addr.sin_addr.s_addr = htonl (INADDR_ANY);
		serv_addr.sin_port = htons ((word)xAP_rcv_port);

		hub[0] = 0;

		if (bind (xAP_rcv,(struct sockaddr *)&serv_addr,sizeof (serv_addr)) < 0) {		// Hub aktiv
			sprintf (hub,"Hub active. ");
			for (xAP_rcv_port = XAP_PORT + 1;xAP_rcv_port < XAP_PORT + 100;xAP_rcv_port++) {
				memset (&serv_addr,0,sizeof (serv_addr));
				serv_addr.sin_family = AF_INET;

				serv_addr.sin_addr.s_addr = inet_addr ("127.0.0.1");
				serv_addr.sin_port = htons ((word)xAP_rcv_port);
				res = bind (xAP_rcv,(struct sockaddr *)&serv_addr,sizeof (serv_addr));
				if (!res) break;
			}
			if (res < 0) return (ERR_OPENSOCKET);
		}
		
		xAP_send = socket (PF_INET,SOCK_DGRAM,0);
		if (xAP_send < 0) return (ERR_OPENSOCKET);

		res = 1;
		setsockopt (xAP_send,SOL_SOCKET,SO_BROADCAST,(char *)&res,sizeof (int));

		memset (&serv_addr,0,sizeof (serv_addr));
		serv_addr.sin_family = AF_INET;

		serv_addr.sin_addr.s_addr = INADDR_BROADCAST;
		serv_addr.sin_port = htons ((word)XAP_PORT);

		if (res = connect (xAP_send,(struct sockaddr *)&serv_addr,sizeof (serv_addr)) < 0) {
			printf ("RES: %d   errno: %d\n",res,errno);
			return (ERR_BINDSOCKET);
		}

		if (mode_flag & XAP) {
			sprintf (msg,"xAP Interface enabled. %sListening on port %d\n",hub,xAP_rcv_port);
			log_print (msg,LOG_INFO);
		}
		
		xAP_SendHeartbeat ();
	}

	return (0);
}



int ConfigSocket (SOCKET *sock,unsigned short port)
{
	struct sockaddr_in serv_addr;
	memset (&serv_addr,0,sizeof (serv_addr));
	serv_addr.sin_family = AF_INET;

#ifdef DBOX
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = port;
#else
	serv_addr.sin_addr.s_addr = htonl (INADDR_ANY);
	serv_addr.sin_port = htons (port);
#endif
	if (bind (*sock,(struct sockaddr *)&serv_addr,sizeof (serv_addr)) < 0) return (ERR_BINDSOCKET);
		
	listen (*sock,5);
	return (0);
}

void process_udp_command (SOCKET fd)
{
	int res;
	long cmd_num;
	char rem[1024],*key, err[1024], txt[1024];


	res = recv(fd,rem,1024,MSG_NOSIGNAL);
	if (res <= 0) return;

	rem[res] = 0;

	sprintf (txt,"UDP send: %s\n", rem);
	log_print (txt,LOG_DEBUG);
	
	key=strchr(rem,',');
	if(key) {
		*key='\0';
		key++;
	}
	else {
		key=rem;
	}
	while(*key!=0 && *key==' ') {
		++key;
	}
	
	res = DBFindRemoteCommand (rem,key,&cmd_num,NULL);
	if (res) {
		GetError (res, txt);
		switch(res) {
			case ERR_REMOTENOTFOUND:
				sprintf (err, txt, rem);
				break;
			case ERR_COMMANDNOTFOUND:
				sprintf (err, txt, key);
				break;
			default:
				sprintf (err, txt);
				break;
		}
		log_print (err, LOG_ERROR);
		return;
	}
	SendIR (cmd_num,0);

}
