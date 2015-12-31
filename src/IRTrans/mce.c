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

#define _WIN32_WINNT 0x501

#include <windows.h>
#include <winuser.h>
#include <io.h>
#include <direct.h>
#include <stdio.h>
#include <sys/timeb.h>


#include "remote.h"
#include "global.h"
#include "lowlevel.h"
#include "dbstruct.h"

#endif

#ifdef MEDIACENTER

extern DEVICEINFO IRDevices[MAX_IR_DEVICES];
extern APP app_pnt[30];
extern long app_cnt;

void	SendMediaCenterAction (int app,int com);
void	IRTransSendInput (int key,int flags);
void	SendKey (APP *app,APPCOMMAND *appcom,byte flag);
void	ConvertLcase (char *pnt,int len);
void	SendMediacenterEvent (int eventcode);
void	SendAppcommand (APP *app,APPCOMMAND *appcom);
void	SendWMChar (APP *app,APPCOMMAND *appcom);


#define CONTROL		1
#define	SHIFT		2
#define NO_KEYUP	4
#define ALT			8
#define LWINKEY		16

#define MULTIKEY_TIMEOUT 1000


void PostWindowsMessage (long rem,long com)
{
	int i,j;
	APPCOMMAND *appcom;


	for (i=0;i < app_cnt;i++) if (app_pnt[i].remnum == rem) {
		for (j=0;j < app_pnt[i].com_cnt;j++) if (app_pnt[i].com[j].comnum == com) {
			appcom = &(app_pnt[i].com[j]);
			if (appcom->type[0] == TYPE_MCE) SendMediaCenterAction (i,j);
			if (appcom->type[0] == TYPE_KEY) SendKey (app_pnt + i,appcom,0);
			if (appcom->type[0] == TYPE_KEYF) SendKey (app_pnt + i,appcom,1);
			if (appcom->type[0] == TYPE_APPCOM) SendAppcommand (app_pnt + i,appcom);
			if (appcom->type[0] == TYPE_CHR) SendWMChar (app_pnt + i,appcom);
		}
	}
}

void SendWMChar (APP *app,APPCOMMAND *appcom)
{
	HWND win;

	win = FindWindow (app->classname,NULL);
	if (!win) return;

	PostMessage (win,WM_KEYDOWN,(WPARAM)appcom->function[0],(LPARAM)0);
//	PostMessage (win,WM_KEYUP,(WPARAM)appcom->function[0],(LPARAM)0xC0000001);
}

void SendAppcommand (APP *app,APPCOMMAND *appcom)
{
	HWND win;

	win = FindWindow (app->classname,NULL);
	if (!win) return;

	PostMessage (win,WM_APPCOMMAND,(WPARAM)1,(LPARAM)(appcom->function[0] << 16));
}

void SendKey (APP *app,APPCOMMAND *appcom,byte flag)
{
	HWND win;
	DWORD thr;
	byte mf = 0;
	struct _timeb tb;
	unsigned long tv;

	static byte cindex;
	static APPCOMMAND *lastcom;
	static unsigned long lasttime;

	if (app && app->classname[0]) {
		win = FindWindow (app->classname,NULL);
		if (!win) return;
		if (flag) {
			thr = GetWindowThreadProcessId (win,NULL);
			AttachThreadInput (GetCurrentThreadId (),thr,TRUE);

			SetFocus (win);

			AttachThreadInput (GetCurrentThreadId (),thr,FALSE);
		}
	}

	_ftime (&tb);
	tv = (tb.time & 0x7fffff) * 1000 + tb.millitm;

	if (appcom != lastcom) cindex = 0;
	else {
		if ((tv - lasttime) < MULTIKEY_TIMEOUT) mf = ++cindex;
		else cindex = 0;
	}

	if (appcom->type[cindex] != TYPE_KEY) {
		if (cindex == 1) mf = 0;
		cindex = 0;
	}

	if (mf)	IRTransSendInput (VK_BACK,0);

	IRTransSendInput (appcom->function[cindex],0);
	
	lastcom = appcom;
	lasttime = tv;
}

void SendMediaCenterAction (int app,int com)
{
	int res;
	HWND mcewin;
	char *sysdir,prog[256];

	mcewin = FindWindow (app_pnt[app].classname,NULL);
	if (mcewin == NULL) {
		if (app_pnt[app].com[com].type[0] == TYPE_RUN || app_pnt[app].com[com].type[1] == TYPE_RUN) {
			sysdir = getenv ("SystemRoot");
			if (sysdir) {
				sprintf (prog,"%s\\ehome\\ehshell.exe",sysdir);
				res = WinExec (prog,SW_SHOWMAXIMIZED);
			}
		}
	}
	else {
		switch (app_pnt[app].com[com].function[0]) {
		case 12:										// Clear
			IRTransSendInput (VK_ESCAPE,0);
			return;
		case 17:										// Mute
//			IRTransSendInput (119,0);
			SendMediacenterEvent (APPCOMMAND_VOLUME_MUTE);
			return;
		case 18:										// Vol-
//			IRTransSendInput (120,0);
			SendMediacenterEvent (APPCOMMAND_VOLUME_DOWN);
			return;
		case 19:										// Vol+
//			IRTransSendInput (121,0);
			SendMediacenterEvent (APPCOMMAND_VOLUME_UP);
			return;
		case 20:										// Play
//			IRTransSendInput (80,SHIFT | CONTROL);
			SendMediacenterEvent (APPCOMMAND_MEDIA_PLAY);
			return;
		case 21:										// Stop
//			IRTransSendInput (83,SHIFT | CONTROL);
			SendMediacenterEvent (APPCOMMAND_MEDIA_STOP);
			return;
		case 22:										// Next
//			IRTransSendInput (70,CONTROL);
			SendMediacenterEvent (APPCOMMAND_MEDIA_NEXTTRACK);
			return;
		case 23:										// Prev
//			IRTransSendInput (66,CONTROL);
			SendMediacenterEvent (APPCOMMAND_MEDIA_PREVIOUSTRACK);
			return;
		case 24:										// REC
//			IRTransSendInput (82,CONTROL);
			SendMediacenterEvent (APPCOMMAND_MEDIA_RECORD);
			return;
		case 25:										// Pause
//			IRTransSendInput (80,CONTROL);
			SendMediacenterEvent (APPCOMMAND_MEDIA_PAUSE);
			return;
		case 26:										// REW
//			IRTransSendInput (66,SHIFT | CONTROL);
			SendMediacenterEvent (APPCOMMAND_MEDIA_REWIND);
			return;
		case 27:										// FWD
//			IRTransSendInput (70,SHIFT | CONTROL);
			SendMediacenterEvent (APPCOMMAND_MEDIA_FAST_FORWARD);
			return;
		case 28:										// Ch+
			IRTransSendInput (187,CONTROL);
//			SendMediacenterEvent (APPCOMMAND_MEDIA_CHANNEL_UP);
			return;
		case 29:										// Ch-
			IRTransSendInput (189,CONTROL);
//			SendMediacenterEvent (APPCOMMAND_MEDIA_CHANNEL_DOWN);
			return;
		case 31:										// DVDMenu
			IRTransSendInput (77,SHIFT | CONTROL);
			return;
		case 32:										// DVDAudio
			IRTransSendInput (65,SHIFT | CONTROL);
			return;
		case 33:										// DVDSubtitle
			IRTransSendInput (85,SHIFT | CONTROL);
			return;
		case 34:										// EPG
			IRTransSendInput (71,CONTROL);
			return;
		case 35:										// Music
			IRTransSendInput (77,CONTROL);
			return;
		case 36:										// Pictures
			IRTransSendInput (73,CONTROL);
			return;
		case 37:										// RecTV
			IRTransSendInput (79,CONTROL);
			return;
		case 38:										// TV
			IRTransSendInput (84,CONTROL | SHIFT);
			return;
		case 39:										// LiveTV
			IRTransSendInput (84,CONTROL);
			return;
		case 40:										// Video
			IRTransSendInput (69,CONTROL);
			return;
		case 41:										// Info
			IRTransSendInput (68,CONTROL);
			return;
		case 42:										// Ehome
			IRTransSendInput (VK_RETURN,ALT | LWINKEY);
			return;
//			APPCOMMAND_BROWSER_BACKWARD
		}
	}
}


void SendMediacenterEvent (int eventcode)
{
	HWND mcewin;

	mcewin = FindWindow ("eHome Render Window",NULL);
	if (!mcewin) return;
	PostMessage (mcewin,WM_APPCOMMAND,(WPARAM)1,(LPARAM)(eventcode << 16));
}


int GetKeyCode (char *com)
{
	if (*com == '\\') {
		com++;
		if (!strcmp (com,"space")) return ' ';
		if (!strcmp (com,"enter")) return VK_RETURN;
		if (!strcmp (com,"up")) return VK_UP;
		if (!strcmp (com,"down")) return VK_DOWN;
		if (!strcmp (com,"right")) return VK_RIGHT;
		if (!strcmp (com,"left")) return VK_LEFT;
		if (!strcmp (com,"backspace")) return VK_BACK;
		if (!strcmp (com,"end")) return VK_END;
		if (!strcmp (com,"home")) return VK_HOME;
		if (!strcmp (com,"pgup")) return VK_PRIOR;
		if (!strcmp (com,"pgdown")) return VK_NEXT;
		return (0);
	}
	if (*com >= 'a' && *com <= 'z') return (*com - ('a' - 'A'));
	else return (*com);
}

int GetFunctionCode (byte type,char *com)
{
	if (type == TYPE_APPCOM) {
		if (!strcmp (com,"appcommand_media_play")) return APPCOMMAND_MEDIA_PLAY;
		if (!strcmp (com,"appcommand_volume_mute")) return  APPCOMMAND_VOLUME_MUTE;
		if (!strcmp (com,"appcommand_volume_down")) return  APPCOMMAND_VOLUME_DOWN;
		if (!strcmp (com,"appcommand_volume_up")) return  APPCOMMAND_VOLUME_UP;
		if (!strcmp (com,"appcommand_media_stop")) return  APPCOMMAND_MEDIA_STOP;
		if (!strcmp (com,"appcommand_media_nexttrack")) return  APPCOMMAND_MEDIA_NEXTTRACK;
		if (!strcmp (com,"appcommand_media_previoustrack")) return  APPCOMMAND_MEDIA_PREVIOUSTRACK;
		if (!strcmp (com,"appcommand_media_record")) return  APPCOMMAND_MEDIA_RECORD;
		if (!strcmp (com,"appcommand_media_pause")) return  APPCOMMAND_MEDIA_PAUSE;
		if (!strcmp (com,"appcommand_media_rewind")) return  APPCOMMAND_MEDIA_REWIND;
		if (!strcmp (com,"appcommand_media_fast_forward")) return  APPCOMMAND_MEDIA_FAST_FORWARD;
		if (!strcmp (com,"appcommand_browser_backward")) return  APPCOMMAND_BROWSER_BACKWARD;
	}

	else if (type == TYPE_MCE) {
		if (!strcmp (com,"clear")) return 12;
		if (!strcmp (com,"mute")) return 17;
		if (!strcmp (com,"vol-")) return 18;
		if (!strcmp (com,"vol+")) return 19;
		if (!strcmp (com,"play")) return 20;
		if (!strcmp (com,"stop")) return 21;
		if (!strcmp (com,"next")) return 22;
		if (!strcmp (com,"prev")) return 23;
		if (!strcmp (com,"rec")) return 24;
		if (!strcmp (com,"pause")) return 25;
		if (!strcmp (com,"rew")) return 26;
		if (!strcmp (com,"fwd")) return 27;
		if (!strcmp (com,"ch+")) return 28;
		if (!strcmp (com,"ch-")) return 29;
		if (!strcmp (com,"dvdmenu")) return 31;
		if (!strcmp (com,"dvdaudio")) return 32;
		if (!strcmp (com,"dvdsubtitle")) return 33;
		if (!strcmp (com,"epg")) return 34;
		if (!strcmp (com,"music")) return 35;
		if (!strcmp (com,"pictures")) return 36;
		if (!strcmp (com,"rectv")) return 37;
		if (!strcmp (com,"tv")) return 38;
		if (!strcmp (com,"livetv")) return 39;
		if (!strcmp (com,"video")) return 40;
		if (!strcmp (com,"info")) return 41;
		if (!strcmp (com,"ehome")) return 42;
		return 0;
	}
	return 0;
}



void IRTransSendInput (int key,int flags)
{
	INPUT InpInfo[10];
	int p;

	memset (InpInfo,0,sizeof (InpInfo));

	p = 0;
	if (flags & CONTROL) {
		InpInfo[p].type = INPUT_KEYBOARD;
		InpInfo[p].ki.wVk = VK_CONTROL;
		p++;
	}
	if (flags & SHIFT) {
		InpInfo[p].type = INPUT_KEYBOARD;
		InpInfo[p].ki.wVk = VK_SHIFT;
		p++;
	}
	if (flags & ALT) {
		InpInfo[p].type = INPUT_KEYBOARD;
		InpInfo[p].ki.wVk = VK_MENU;
		p++;
	}
	if (flags & LWINKEY) {
		InpInfo[p].type = INPUT_KEYBOARD;
		InpInfo[p].ki.wVk = VK_LWIN;
		p++;
	}

	InpInfo[p].type = INPUT_KEYBOARD;
	InpInfo[p].ki.wVk = key;
	p++;
	
	if (!(flags & NO_KEYUP)) {
		InpInfo[p].type = INPUT_KEYBOARD;
		InpInfo[p].ki.dwFlags = KEYEVENTF_KEYUP;
		InpInfo[p].ki.wVk = key;
		p++;
	}

	if (flags & LWINKEY) {
		InpInfo[p].type = INPUT_KEYBOARD;
		InpInfo[p].ki.dwFlags = KEYEVENTF_KEYUP;
		InpInfo[p].ki.wVk = VK_LWIN;
		p++;
	}
	if (flags & ALT) {
		InpInfo[p].type = INPUT_KEYBOARD;
		InpInfo[p].ki.dwFlags = KEYEVENTF_KEYUP;
		InpInfo[p].ki.wVk = VK_MENU;
		p++;
	}
	if (flags & SHIFT) {
		InpInfo[p].type = INPUT_KEYBOARD;
		InpInfo[p].ki.wVk = VK_SHIFT;
		InpInfo[p].ki.dwFlags = KEYEVENTF_KEYUP;
		p++;
	}
	if (flags & CONTROL) {
		InpInfo[p].type = INPUT_KEYBOARD;
		InpInfo[p].ki.wVk = VK_CONTROL;
		InpInfo[p].ki.dwFlags = KEYEVENTF_KEYUP;
		p++;
	}
	SendInput (p,InpInfo,sizeof (INPUT));
}

#endif
