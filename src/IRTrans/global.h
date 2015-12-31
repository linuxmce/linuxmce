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
#define LOG_DEBUG		4
#define LOG_INFO		3
#define LOG_ERROR		2
#define LOG_FATAL		1
#define LOG_MASK		15

#define DEBUG_CODE		16
#define	HEXDUMP			32
#define OLDFORMAT		64
#define CODEDUMP		128
#define LEARNED_ONLY	256
#define	XAP				512

#define DAEMON_MODE		0x10000
#define NO_RECONNECT	0x20000
#define	NO_RESET		0x40000
#define NO_LIRC			0x80000
#define NO_WEB			0x100000
#define CLOCK_STARTUP	0x200000

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

#define FN_FLASH128	8192
#define FN_DUALSND	16384
#define FN_DISP1	32768
#define FN_DISP2	0x10000
#define FN_DISP3	0x20000
#define FN_CLOCK	0x40000
#define FN_DEBOUNCE	0x80000

#define FN_DISPMASK	0x38000

#define TYPE_MCE	1
#define TYPE_KEY	2
#define TYPE_RUN	3
#define TYPE_APPCOM	4
#define TYPE_COM	5
#define TYPE_CHR	6
#define TYPE_KEYF	7

extern unsigned long mode_flag;
extern char logfile[256];
extern FILE *logfp;
extern char irserver_version[20];
extern char irtrans_version[100];
//extern unsigned long serno;
extern byte last_adress,resend_flag;
extern unsigned short capabilities;
extern byte time_len;
extern byte raw_repeat;

#ifdef WIN32

void	PostWindowsMessage (long rem,long com);

/* cmd for HSHELL_APPCOMMAND and WM_APPCOMMAND */
#define APPCOMMAND_BROWSER_BACKWARD       1
#define APPCOMMAND_BROWSER_FORWARD        2
#define APPCOMMAND_BROWSER_REFRESH        3
#define APPCOMMAND_BROWSER_STOP           4
#define APPCOMMAND_BROWSER_SEARCH         5
#define APPCOMMAND_BROWSER_FAVORITES      6
#define APPCOMMAND_BROWSER_HOME           7
#define APPCOMMAND_VOLUME_MUTE            8
#define APPCOMMAND_VOLUME_DOWN            9
#define APPCOMMAND_VOLUME_UP              10
#define APPCOMMAND_MEDIA_NEXTTRACK        11
#define APPCOMMAND_MEDIA_PREVIOUSTRACK    12
#define APPCOMMAND_MEDIA_STOP             13
#define APPCOMMAND_MEDIA_PLAY_PAUSE       14
#define APPCOMMAND_LAUNCH_MAIL            15
#define APPCOMMAND_LAUNCH_MEDIA_SELECT    16
#define APPCOMMAND_LAUNCH_APP1            17
#define APPCOMMAND_LAUNCH_APP2            18
#define APPCOMMAND_BASS_DOWN              19
#define APPCOMMAND_BASS_BOOST             20
#define APPCOMMAND_BASS_UP                21
#define APPCOMMAND_TREBLE_DOWN            22
#define APPCOMMAND_TREBLE_UP              23
#define APPCOMMAND_MICROPHONE_VOLUME_MUTE 24
#define APPCOMMAND_MICROPHONE_VOLUME_DOWN 25
#define APPCOMMAND_MICROPHONE_VOLUME_UP   26
#define APPCOMMAND_HELP                   27
#define APPCOMMAND_FIND                   28
#define APPCOMMAND_NEW                    29
#define APPCOMMAND_OPEN                   30
#define APPCOMMAND_CLOSE                  31
#define APPCOMMAND_SAVE                   32
#define APPCOMMAND_PRINT                  33
#define APPCOMMAND_UNDO                   34
#define APPCOMMAND_REDO                   35
#define APPCOMMAND_COPY                   36
#define APPCOMMAND_CUT                    37
#define APPCOMMAND_PASTE                  38
#define APPCOMMAND_REPLY_TO_MAIL          39
#define APPCOMMAND_FORWARD_MAIL           40
#define APPCOMMAND_SEND_MAIL              41
#define APPCOMMAND_SPELL_CHECK            42
#define APPCOMMAND_DICTATE_OR_COMMAND_CONTROL_TOGGLE    43
#define APPCOMMAND_MIC_ON_OFF_TOGGLE      44
#define APPCOMMAND_CORRECTION_LIST        45
#define APPCOMMAND_MEDIA_PLAY             46
#define APPCOMMAND_MEDIA_PAUSE            47
#define APPCOMMAND_MEDIA_RECORD           48
#define APPCOMMAND_MEDIA_FAST_FORWARD     49
#define APPCOMMAND_MEDIA_REWIND           50
#define APPCOMMAND_MEDIA_CHANNEL_UP       51
#define APPCOMMAND_MEDIA_CHANNEL_DOWN     52

#define WM_APPCOMMAND                   0x0319

#endif
