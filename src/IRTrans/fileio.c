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
#include <io.h>
#include <direct.h>

#define MSG_NOSIGNAL	0
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

typedef int SOCKET;
typedef int DWORD;
typedef int WSAEVENT;
#define closesocket		close
#endif

#include "errcode.h"
#include "remote.h"
#include "network.h"
#include "dbstruct.h"
#include "fileio.h"
#include "global.h"
#include "lowlevel.h"
#include "ccf.h"

#define dbpath "."

#ifdef WIN32
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif



void GetNumericCode (char command[],char numeric[],char rem[],char com[])
{
	int b,q,z,val;
	
	if (*command < '0') {
		q = 0;
		while (rem[q] && q < 4) {
			sprintf (numeric + q * 2,"%02x",rem[q]);
			q++;
		}
		b = 0;
		while (com[b] && b < 4) {
			sprintf (numeric + q * 2,"%02x",com[b]);
			b++;
			q++;
		}
		q *= 2;
		while (q < 16) numeric[q++] = '0';
		numeric[16] = 0;
		return;
	}
	val = b = q = z = 0;
	if (strlen (command) == 22 && !strncmp (command,"004011",6)) {
		q = 6;
		while (command[q]) {
			val |= (command[q] - '0') << b;
			b+= 2;
			if (b == 4) {
				val &= 0xf;
				if (val < 10) val += '0';
				else val += 'a' - 10;
				numeric[z++] = val;
				val = 0;
				b = 0;
			}
			q++;
		}
		if (b) {
			val &= 0xf;
			if (val < 10) val += '0';
			else val += 'a' - 10;
			numeric[z++] = val;
		}
		while (z < 16) numeric[z++] = '0';
		numeric[16] = 0;
		return;
	}
	if (command[q] == 'S') q++;
	while (command[q]) {
		val |= (command[q] - '0') << b;
		b++;
		if (b == 4) {
			val &= 0xf;
			if (val < 10) val += '0';
			else val += 'a' - 10;
			numeric[z++] = val;
			val = 0;
			b = 0;
		}
		q++;
	}
	if (b) {
		val &= 0xf;
		if (val < 10) val += '0';
		else val += 'a' - 10;
		numeric[z++] = val;
	}
	while (z < 16) numeric[z++] = '0';
	numeric[16] = 0;
}

FILE *ASCIIOpenRemote (char name[],NETWORKCLIENT *client)
{
	FILE *fp;
	char nm[256];

	strcpy (nm,name);
	if (strcmp (nm + strlen (nm) - 4,".rem")) strcat (nm,".rem");
	fp = DBOpenFile (nm,"r");
	memset (&client->ird,0,sizeof (IRDATA));
	client->learnstatus.received[0] = 0;
	client->learnstatus.adress = 0;
	client->learnstatus.statustype = STATUS_LEARN;
	client->learnstatus.statuslen = sizeof (NETWORKLEARNSTAT);
	memset (client->learnstatus.remote,' ',80);
	memset (client->learnstatus.received,' ',CODE_LEN);
	client->learnstatus.num_timings = 0;
	client->learnstatus.num_commands = 0;
	client->learnstatus.learnok = 0;

	if (fp)	{
		ASCIITimingSample (fp,client);
		fclose (fp);
		fp = DBOpenFile (nm,"r+");
		memcpy (client->learnstatus.remote,name,strlen (name));
		strcpy (client->filename,nm);
		return (fp);
	}

	fp = DBOpenFile (nm,"w+");
	if (!fp) return (NULL);

	fprintf (fp,"[REMOTE]\n");
	fprintf (fp,"  [NAME]%s\n\n",name);
	fprintf (fp,"[TIMING]\n");

	fflush (fp);
	
	memcpy (client->learnstatus.remote,name,strlen (name));
	strcpy (client->filename,nm);

	return (fp);
}


void ASCIITimingSample (FILE *fp,NETWORKCLIENT *client)
{
	char ln[1000];
	IRTIMING irt;
	int i;
	char *data;
	char *rp;
	
	
	data = DBFindSection (fp,"TIMING",NULL,NULL,NULL);
	if (!data) return;

	data = DBFindSection (fp,"0",ln,"[COMMANDS]",NULL);

	if (data) {
		StoreIRTiming (&irt,ln);
		client->learnstatus.num_timings++;

		while (data && *data == '[' && data[1] != 'C') {
			data = DBReadString (ln,fp,NULL);
			if (data && *data == '[' && data[1] != 'C') client->learnstatus.num_timings++;
		}

		memcpy (&client->ird.ir_length,&irt.ir_length,sizeof (IRTIMING) - 4);
		client->ird.ir_length = 1;
	}

	rewind (fp);

	data = DBFindSection (fp,"COMMANDS",NULL,NULL,NULL);
	if (!data) return;
	
	data = DBReadString (ln,fp,NULL);
	if (!data || *data != '[') return;

	i = strlen (data);
	while (i && data[i] != ']') i--;
	if (!i) return;
	i++;

    if ((rp = strstr(data, "[RAW]")))
    {
        int jj;
        rp += 5;
        sscanf(rp, "%d", &client->ird.ir_length);
		rp = data+i;
        for (jj = 0;jj < client->ird.ir_length; jj++)
        {
            int     val;
            int len;
            sscanf(rp, "%d%n", &val, &len);
            client->ird.data[jj] = val/8;
            rp += len;
        }
    }
    else
    {
        strcpy (client->ird.data,data+i);
        client->ird.ir_length = strlen (data+i);
    }


	client->learnstatus.num_commands++;

	while (data && *data == '[') {
		data = DBReadString (ln,fp,NULL);
		if (data && *data == '[') client->learnstatus.num_commands++;
	}

}

// Sucht nach existierenden Toggle - Commands
int ASCIIFindToggleSeq (FILE *fp,IRDATA *ird,char name[])
{
	
	int i,p = 0;
	int a;
	char ln[2048],*data;

	rewind (fp);
	data = DBFindSection (fp,"COMMANDS",NULL,NULL,NULL);

	if (!data) return (-1);
	while (data) {
		data = DBReadString (ln,fp,NULL);
		if (data && *data == '[') {
			data++;
			i = 0;
			while (data[i] && data[i] != '#' && data[i] != ']') i++;
			if (data[i] && !memcmp (name,data,i)) {
				if (data[i] == ']') a = 1;
				else a = atoi (data + i + 1) + 1;
				while (data[i] != 'D') i++;
				i += 2;
				if (!strcmp (ird->data,data+i)) {
					fseek (fp,0,SEEK_END);
					return (-a);
				}
				p = a;
			}
		}
	}

	fseek (fp,0,SEEK_END);

	return (p);
}


int ASCIIFindCommand (FILE *fp,char name[],NETWORKCLIENT *client)
{
	
	HANDLE hfile;
	long pos,new,len,oldlen;
	int i;
	char ln[2048],*data;
	char com[256];

	rewind (fp);
	data = DBFindSection (fp,"COMMANDS",NULL,NULL,NULL);

	if (!data) {
		fseek (fp,0,SEEK_END);
		fprintf (fp,"\n\n[COMMANDS]\n");
		return (0);
	}
	strcpy (com,name);
	ConvertLcase (com,strlen (com));
	while (data) {
		pos = ftell (fp);
		data = DBReadString (ln,fp,NULL);
		if (data && *data == '[') {
			ConvertLcase (data,strlen (data));
			data++;
			i = 0;
			while (data[i] && data[i] != ']') i++;
			if (data[i] && !memcmp (com,data,i) && i == (int)strlen (com)) {
				new = ftell (fp);							// Debug Info über Update
				oldlen = new - pos;
				fseek (fp,0,SEEK_END);
				len = ftell (fp) - new;
				fseek (fp,new,SEEK_SET);
				data = malloc (len + 1);
				len = fread (data,1,len + 1,fp);
				fseek (fp,pos,SEEK_SET);
				fwrite (data,1,len,fp);
				pos = ftell (fp);
				free (data);
				data = malloc (oldlen);
				memset (data,' ',oldlen);
				fwrite (data,1,oldlen,fp);
				fseek (fp,pos,SEEK_SET);
				free (data);

				if (client) {
#ifdef WIN32
					hfile = CreateFile (client->filename,GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
					if (hfile) {
						SetFilePointer (hfile,pos,NULL,FILE_BEGIN);
						SetEndOfFile (hfile);
						CloseHandle (hfile);
					}
#endif
#ifdef LINUX
					truncate (client->filename,pos);
#endif
				}

				return (1);												
			}
		}
	}

	fseek (fp,0,SEEK_END);

	return (0);
}



int ASCIIStoreCommand (FILE *fp,IRDATA *ird,char name[],int timing,int seq_number)
{
	if (seq_number) fprintf (fp,"  [%s#%02d][T]%d",name,seq_number,timing);
	else fprintf (fp,"  [%s][T]%d",name,timing);

	fprintf (fp,"[D]%s\n",ird -> data);
	fflush (fp);
	return (0);
}

int ASCIIStoreRAW (FILE *fp,IRRAW *ird,char name[])
{
	int i;
	if (ird->transmit_freq == 255)
		fprintf (fp,"  [%s][RAW]%d[FREQ]455[D]",name,ird->ir_length);
	else
		fprintf (fp,"  [%s][RAW]%d[FREQ]%d[D]",name,ird->ir_length,ird->transmit_freq);
	for (i=0;i < ird->ir_length;i++) fprintf (fp,"%d ",ird->data[i] * 8);
	fprintf (fp,"\n");
	fflush (fp);
	return (0);
}

int TestTimeValue (int val,int sollk,int sollg)
{
	if (val < sollk || val > sollg) return (0);
	return (1);
}


void CheckRCMMCode (IRDATA *ird)
{
	int i;
	if (!ird->time_cnt) return;
	for (i=0;i<ird->time_cnt;i++) {
		if (!TestTimeValue (ird->pulse_len[i],15,22) && !TestTimeValue (ird->pulse_len[i],48,55)) return;
		if (!TestTimeValue (ird->pause_len[i],32,40) && !TestTimeValue (ird->pause_len[i],53,60) &&
		    !TestTimeValue (ird->pause_len[i],72,80) && !TestTimeValue (ird->pause_len[i],93,101)) return;
	}
	for (i=0;i<ird->time_cnt;i++) {
		if (TestTimeValue (ird->pulse_len[i],15,22)) ird->pulse_len[i] = 21;
		if (TestTimeValue (ird->pulse_len[i],48,55)) ird->pulse_len[i] = 52;
		if (TestTimeValue (ird->pause_len[i],32,40)) ird->pause_len[i] = 35;
		if (TestTimeValue (ird->pause_len[i],53,60)) ird->pause_len[i] = 55;
		if (TestTimeValue (ird->pause_len[i],72,80)) ird->pause_len[i] = 76;
		if (TestTimeValue (ird->pause_len[i],93,101)) ird->pause_len[i] = 97;
	}
	ird->repeat_pause = 87;
	ird->ir_repeat = 2;
	ird->transmit_freq = 36;
}



int ASCIIStoreTiming (FILE *fp,IRDATA *ird,NETWORKLEARNSTAT *stat)
{
	int pos;
	long fpos,size;
	char *mem;
	char st[100];


	CheckRCMMCode (ird);

	pos = ASCIIFindTiming (fp,ird);

	if (pos == -1) return (-ERR_TIMINGNOTFOUND);

	if (pos) {
		if (pos >= 100) {
			fseek (fp,0,SEEK_END);
			return (pos - 100);
		}
		else {
			rewind (fp);
			DBFindSection (fp,"TIMING",NULL,NULL,NULL);
			sprintf (st,"%d",pos-1);
			DBFindSection (fp,st,NULL,NULL,NULL);
			fpos = ftell (fp);
			fseek (fp,0,SEEK_END);
			size = 1024 + ftell (fp) - fpos;
			mem = malloc (size);
			fseek (fp,fpos,SEEK_SET);
			size = fread (mem,1,size,fp);
			fseek (fp,fpos,SEEK_SET);
			ASCIIStoreTimingParam (fp,ird,pos);
			fprintf (fp,"\n");
			fwrite (mem,1,size,fp);
			free (mem);
			fflush (fp);
			stat->num_timings++;
			stat->learnok = 1;
			return (pos);
		}
	}

	fseek (fp,0,SEEK_END);
	ASCIIStoreTimingParam (fp,ird,pos);
	fprintf (fp,"\n\n[COMMANDS]\n");
	
	stat->num_timings++;
	stat->learnok = 1;

	fflush (fp);
	return (0);
}


void ASCIIStoreTimingParam (FILE *fp,IRDATA *ird,int timing)
{
	int i;

	fprintf (fp,"  [%d][N]%d",timing,ird->time_cnt);
	for (i=1;i <= ird->time_cnt;i++) {
		fprintf (fp,"[%d]%d %d",i,ird->pulse_len[i-1] * 8,ird->pause_len[i-1] * 8);
	}
	fprintf (fp,"[RC]%d[RP]%d",ird->ir_repeat,ird->repeat_pause);
	if (ird->transmit_freq == 255) 
		fprintf (fp,"[FREQ]455");
	else
		fprintf (fp,"[FREQ]%d",ird->transmit_freq);
	if (ird->mode & START_BIT) fprintf (fp,"[SB]");
	if (ird->mode & REPEAT_START) fprintf (fp,"[RS]");
	if (ird->mode & RC5_DATA) fprintf (fp,"[RC5]");
	if (ird->mode & RC6_DATA) fprintf (fp,"[RC6]");
}


int ASCIIFindTiming (FILE *fp,IRDATA *ird)
{
	int i,flag;
	char ln[256],*data;
	char st[255];
	IRTIMING irt;

	
	rewind (fp);
	data = DBFindSection (fp,"TIMING",NULL,NULL,NULL);

	if (!data) return (-1);

	flag = i = 0;
	while (data) {
		sprintf (st,"%d",i);
		data = DBFindSection (fp,st,ln,"[COMMANDS]",NULL);
		if (data) {
			flag++;
			StoreIRTiming (&irt,ln);
			if (CompareTiming (ird,&irt)) return (i + 100);
		}
		i++;
	}


	if (!flag) fseek (fp,0,SEEK_END);
	return (flag);
}


int CompareTiming (IRDATA *ird,IRTIMING *irt)
{
	int i;
	if (ird->mode != irt->mode || ird->time_cnt != irt->time_cnt || 
		ird->transmit_freq != ird->transmit_freq) return (0);
	for (i = 0;i < ird->time_cnt;i++) {
		if (ird->pause_len[i] < (irt->pause_len[i] - IR_TOLERANCE) || ird->pause_len[i] > (irt->pause_len[i] + IR_TOLERANCE) ||
			ird->pulse_len[i] < (irt->pulse_len[i] - IR_TOLERANCE) || ird->pulse_len[i] > (irt->pulse_len[i] + IR_TOLERANCE)) return (0);
	}
	return (1);

}


IRREMOTE *rem_pnt;
long rem_cnt;
IRCOMMAND *cmd_pnt;
long cmd_cnt;
IRTIMING *tim_pnt;
long tim_cnt;
MACROCOMMAND *mac_pnt;
long mac_cnt;
ROUTING *recv_routing;
long recv_routing_cnt;
ROUTING *send_routing;
long send_routing_cnt;
ROOMS *rooms;
long room_cnt;
SWITCH *switches;
long switch_cnt;
long ccf_raw;
long ccf_data;
long ccf_err;
APP app_pnt[30];
long app_cnt;


void GetRemoteDatabase (REMOTEBUFFER *buf,long offset)
{
	int i;
	memset (buf,0,sizeof (REMOTEBUFFER));
	buf->statustype = STATUS_REMOTELIST;
	buf->statuslen = sizeof (REMOTEBUFFER);
	buf->offset = (short)offset;

	i = 0;
	while (i < 40 && offset < rem_cnt) {
		memset (buf->remotes[i].name,' ',80);
		memcpy (buf->remotes[i].name,rem_pnt[offset].name,strlen (rem_pnt[offset].name));
		buf->remotes[i].source_mask = rem_pnt[offset].source_mask;
		buf->remotes[i].target_mask = rem_pnt[offset].target_mask;
		i++;
		offset++;
	}

	buf->count_buffer = i;
	buf->count_total = (word)rem_cnt;
	if (i == 40) buf->count_remaining = (short)(rem_cnt - offset);
	else buf->count_remaining = 0;
}

void GetCommandDatabase (COMMANDBUFFER *buf,char remote[],long offset)
{
	int i,nrem,start,tog = 0;
	char remcmp[100];

	memset (buf,0,sizeof (COMMANDBUFFER));
	buf->statustype = STATUS_COMMANDLIST;
	buf->statuslen = sizeof (COMMANDBUFFER);
	buf->offset = (short)offset;

	memset (remcmp,0,100);
	strcpy (remcmp,remote);
	ConvertLcase (remcmp,strlen (remcmp));


	nrem = DBFindRemote (remcmp);
	if (nrem == -1) return;

	start = rem_pnt[nrem].command_start;

	i = rem_pnt[nrem].command_start;
	while (i < rem_pnt[nrem].command_end) {
		if (cmd_pnt[i].toggle_seq > 1) tog++;
		i++;
	}

	
	i = 0;
	while (i < 200 && offset < rem_pnt[nrem].command_end-start) {
		if (cmd_pnt[offset+start].toggle_seq <= 1) {
			memset (buf->commands[i],' ',20);
			memcpy (buf->commands[i],cmd_pnt[offset+start].name,strlen (cmd_pnt[offset+start].name));
			i++;
		}
		offset++;
	}

	buf->count_buffer = i;
	buf->count_total = (word)rem_pnt[nrem].command_end - start - tog;
	if (i == 40) buf->count_remaining = (short)(rem_pnt[nrem].command_end - start - offset - tog);
	else buf->count_remaining = 0;
}

void FreeDatabaseMemory (void)
{
	if (rem_pnt) free (rem_pnt);
	rem_pnt = 0;
	rem_cnt = 0;
	if (cmd_pnt) free (cmd_pnt);
	cmd_pnt = 0;
	cmd_cnt = 0;
	if (tim_pnt) free (tim_pnt);
	tim_pnt = 0;
	tim_cnt = 0;
	if (mac_pnt) free (mac_pnt);
	mac_pnt = 0;
	mac_cnt = 0;
	if (recv_routing) free (recv_routing);
	recv_routing = 0;
	recv_routing_cnt = 0;
	if (send_routing) free (send_routing);
	send_routing = 0;
	send_routing_cnt = 0;
	if (rooms) free (rooms);
	rooms = 0;
	room_cnt = 0;
	ccf_data = ccf_raw = ccf_err = 0;
	app_cnt = 0;
	memset (app_pnt,0,sizeof (app_pnt));
}

int cmpRawData (byte *rcv,byte *mem,int len);
int getRawValue (byte *pnt,int pos,word *val);



int DBFindCommandName (byte command[],char remote[],char name[],byte address,long *remote_num,long *command_num,word *command_num_rel,int start)
{
	int len,i;
	byte *pnt;
	word mask;
	byte mode;

	static char last_name[50];
	static byte last_address;
	mask = 1 << (address & 15);
	mode = (address & 0xf0) >> 2;
	len = strlen (command);
	i = start;
	*command_num = *remote_num = 0;
	if ((mode & RAW_DATA) || *command < '0') {								// RAW Vergleich
		if (mode & RAW_DATA) pnt = (byte *)command;
		else pnt = (byte *)command + 1;
		while (i < cmd_cnt) {

			if (cmd_pnt[i].mode & RAW_DATA && (rem_pnt[cmd_pnt[i].remote].source_mask & mask)) {
				if (cmpRawData (pnt,cmd_pnt[i].data,cmd_pnt[i].ir_length)) {
					if (cmd_pnt[i].name[strlen (cmd_pnt[i].name) - 1] == '@') {
						if (!strncmp (last_name,cmd_pnt[i].name,strlen (cmd_pnt[i].name) - 1)) {
							strcpy (remote,rem_pnt[cmd_pnt[i].remote].name);
							strcpy (name,cmd_pnt[i].name);
							name[strlen (name) - 1] = 0;
							*command_num_rel = (word)(i - rem_pnt[cmd_pnt[i].remote].command_start);
							*command_num = i;
							*remote_num = cmd_pnt[i].remote;
							last_address = address;
							return (i + 1);
							}
					}
					else {
						strcpy (remote,rem_pnt[cmd_pnt[i].remote].name);
						strcpy (name,cmd_pnt[i].name);
						strcpy (last_name,name);
						*command_num_rel = (word)(i - rem_pnt[cmd_pnt[i].remote].command_start);
						*command_num = i;
						*remote_num = cmd_pnt[i].remote;
						last_address = address;
						return (i + 1);
					}
				}
			}
			i++;
		}
		if (!start) last_name[0] = 0;
		return (0);
	}

	if (mode & RC5_DATA) command[2] = '0';					// Toggle Bit maskieren
	if (mode & RC6_DATA) {
		if (len == 39) command[23] = '0';					// RC6 Mode 6A
		else command[5] = command[6] = '0';					// Toggle Bit maskieren
	}

	while (i < cmd_cnt) {
		if (!cmd_pnt[i].mode && (rem_pnt[cmd_pnt[i].remote].source_mask & mask) && 
			((rem_pnt[cmd_pnt[i].remote].rcv_len && len >= rem_pnt[cmd_pnt[i].remote].rcv_len && !memcmp (command,cmd_pnt[i].data,len)) ||
			(len == cmd_pnt[i].ir_length && !memcmp (command,cmd_pnt[i].data,len)))) {
			if (cmd_pnt[i].name[strlen (cmd_pnt[i].name) - 1] == '@') {
				if (!strncmp (last_name,cmd_pnt[i].name,strlen (cmd_pnt[i].name) - 1)) {
					strcpy (remote,rem_pnt[cmd_pnt[i].remote].name);
					strcpy (name,cmd_pnt[i].name);
					name[strlen (name) - 1] = 0;
					*command_num_rel = (word)(i - rem_pnt[cmd_pnt[i].remote].command_start);
					*command_num = i;
					*remote_num = cmd_pnt[i].remote;
					last_address = address;
					return (i + 1);
					}
			}
			else {
				strcpy (remote,rem_pnt[cmd_pnt[i].remote].name);
				strcpy (name,cmd_pnt[i].name);
				strcpy (last_name,name);
				*command_num_rel = (word)(i - rem_pnt[cmd_pnt[i].remote].command_start);
				*command_num = i;
				*remote_num = cmd_pnt[i].remote;
				last_address = address;
				return (i + 1);
			}
		}
		i++;
	}

	if (!start) last_name[0] = 0;
	return (0);
}


int cmpRawData (byte *rcv,byte *mem,int len)
{
	int pos = 0;
	word recvdata,memdata;
	getRawValue (rcv,pos,&recvdata);
	pos = getRawValue (mem,pos,&memdata);
	while (pos < len) {
		if (recvdata < (memdata - (RAW_TOLERANCE + (memdata >> 4))) || 
			recvdata > (memdata + (RAW_TOLERANCE + (memdata >> 4)))) return (0);
		getRawValue (rcv,pos,&recvdata);
		pos = getRawValue (mem,pos,&memdata);
	}
	return (1);
}


int getRawValue (byte *pnt,int pos,word *val)
{
	*val = 0;
	if (!pnt[pos]) {
		pos++;
		*val = pnt[pos++] << 8;
	}
	*val += pnt[pos++];
	return (pos);
}



int DBFindRemoteCommand (char remote[],char command[],long *cmd_num,long *rem_num)
{

	int ncmd,nrem;

	char remcmp[100];
	char cmdcmp[100];

	memset (remcmp,0,100);
	memset (cmdcmp,0,100);

	strcpy (remcmp,remote);
	strcpy (cmdcmp,command);
	ConvertLcase (remcmp,strlen (remcmp));
	ConvertLcase (cmdcmp,strlen (cmdcmp));


	nrem = DBFindRemote (remcmp);
	if (nrem == -1) return (ERR_REMOTENOTFOUND);
	if (rem_num) *rem_num = nrem;
	ncmd = DBFindCommand (cmdcmp,nrem);
	if (ncmd == -1) return (ERR_COMMANDNOTFOUND);
	*cmd_num = ncmd;
	return (0);
}

int DBGetIRCode (long cmd_num,IRDATA *ir,long idx,long *mac_len,long *mac_pause,long *rpt_len)
{
	int res;
	long nrem,ncmd,ntim,mcmd;

	IRRAW *rd;
	IRMACRO *m_pnt;

	m_pnt = (IRMACRO *)(cmd_pnt + cmd_num);

	if (cmd_pnt[cmd_num].mode == MACRO_DATA) {
		*mac_len = m_pnt->macro_len;
		res = DBFindRemoteCommand (mac_pnt[m_pnt->macro_num + idx].mac_remote,mac_pnt[m_pnt->macro_num + idx].mac_command,&mcmd,NULL);
		if (res) return (res);
		ncmd = mcmd;
		*mac_pause = mac_pnt[m_pnt->macro_num + idx].pause;
	}

	else {
		ncmd = cmd_num;
		*mac_len = 0;
		*mac_pause = 0;
	}

	nrem = cmd_pnt[ncmd].remote;

	if (cmd_pnt[ncmd].mode & RAW_DATA) {
		rd = (IRRAW *)ir;
		memset (rd,0,sizeof (IRRAW));
		rd->mode = cmd_pnt[ncmd].mode;
		rd->target_mask = rem_pnt[nrem].target_mask;
		if (rem_pnt[nrem].transmitter) rd->address = rem_pnt[nrem].transmitter;
//		if (extended_carrier) {
		if (cmd_pnt[ncmd].timing > 127) {
			if (cmd_pnt[ncmd].timing > 500) rd->transmit_freq = 255;
			else rd->transmit_freq = (cmd_pnt[ncmd].timing / 4) | 128;
		}
		else rd->transmit_freq = (byte)cmd_pnt[ncmd].timing;
/*		}
		else {
			if (cmd_pnt[ncmd].timing > 253) rd->transmit_freq = 255;
			else rd->transmit_freq = (byte)cmd_pnt[ncmd].timing;
		}
*/
		rd->ir_length = cmd_pnt[ncmd].ir_length;
		memcpy (rd->data,cmd_pnt[ncmd].data,CODE_LENRAW);
		if (rpt_len) *rpt_len = cmd_pnt[ncmd].command_length;
	}
	if (!cmd_pnt[ncmd].mode) {
		ntim = rem_pnt[nrem].timing_start + cmd_pnt[ncmd].timing;
		if (ntim >= rem_pnt[nrem].timing_end) return (ERR_TIMINGNOTFOUND); 

		memset (ir,0,sizeof (IRDATA));

		ir->target_mask = rem_pnt[nrem].target_mask;
		if (rem_pnt[nrem].transmitter) ir->address = rem_pnt[nrem].transmitter;
		memcpy (&ir->transmit_freq,&tim_pnt[ntim].transmit_freq,sizeof (IRTIMING));
		memcpy (ir->data,cmd_pnt[ncmd].data,CODE_LEN);
		ir->ir_length = cmd_pnt[ncmd].ir_length;
		if (rpt_len) *rpt_len = cmd_pnt[ncmd].command_length;
	}

	return (0);
}



int DBFindRemoteCommandEx(char remote[],char command[],IRDATA *ir)
{

	long nrem,ncmd,ntim;
	IRRAW *rd;

	char remcmp[100];
	char cmdcmp[100];

	memset (remcmp,0,100);
	memset (cmdcmp,0,100);

	strcpy (remcmp,remote);
	strcpy (cmdcmp,command);
	ConvertLcase (remcmp,strlen (remcmp));
	ConvertLcase (cmdcmp,strlen (cmdcmp));


	nrem = DBFindRemote (remcmp);
	if (nrem == -1) return (ERR_REMOTENOTFOUND);
	ncmd = DBFindCommand (cmdcmp,nrem);
	if (ncmd == -1) return (ERR_COMMANDNOTFOUND);

	if (cmd_pnt[ncmd].mode == MACRO_DATA) return (ERR_ISMACRO);

	if (cmd_pnt[ncmd].mode & RAW_DATA) {
		rd = (IRRAW *)ir;
		memset (rd,0,sizeof (IRRAW));
		rd->mode = cmd_pnt[ncmd].mode;
		rd->target_mask = rem_pnt[nrem].target_mask;
		if (rem_pnt[nrem].transmitter) rd->address = rem_pnt[nrem].transmitter;
//		if (extended_carrier) {
		if (cmd_pnt[ncmd].timing > 127) {
			if (cmd_pnt[ncmd].timing > 500) rd->transmit_freq = 255;
			else rd->transmit_freq = (cmd_pnt[ncmd].timing / 4) | 128;
		}
		else rd->transmit_freq = (byte)cmd_pnt[ncmd].timing;

		rd->ir_length = cmd_pnt[ncmd].ir_length;
		memcpy (rd->data,cmd_pnt[ncmd].data,CODE_LENRAW);
	}
	if (!cmd_pnt[ncmd].mode) {
		ntim = rem_pnt[nrem].timing_start + cmd_pnt[ncmd].timing;
		if (ntim >= rem_pnt[nrem].timing_end) return (ERR_TIMINGNOTFOUND); 

		memset (ir,0,sizeof (IRDATA));

		ir->target_mask = rem_pnt[nrem].target_mask;
		if (rem_pnt[nrem].transmitter) ir->address = rem_pnt[nrem].transmitter;
		memcpy (&ir->transmit_freq,&tim_pnt[ntim].transmit_freq,sizeof (IRTIMING));
		memcpy (ir->data,cmd_pnt[ncmd].data,CODE_LEN);
		ir->ir_length = cmd_pnt[ncmd].ir_length;
	}

	return (0);
}



int DBFindCommand (char command[],long remote)
{
	int p,s;
	int i = rem_pnt[remote].command_start;
	while (i < rem_pnt[remote].command_end) {
		if (!memcmp (command,cmd_pnt[i].name,20)) {
			if (rem_pnt[remote].toggle_pos) {
				s = rem_pnt[remote].toggle_pos;
				p = i;
				while ((cmd_pnt[p].toggle_seq != s || memcmp (command,cmd_pnt[p].name,20)) && p < rem_pnt[remote].command_end) p++;
				if (cmd_pnt[p].toggle_seq == s) {
					rem_pnt[remote].toggle_pos++;
					return (p);
				}
				else {
					rem_pnt[remote].toggle_pos = 2;
					return (i);
				}
			}

			else if (cmd_pnt[i].toggle_seq) {
				s = cmd_pnt[i].toggle_pos;
				p = i;
				while ((cmd_pnt[p].toggle_seq != s || memcmp (command,cmd_pnt[p].name,20)) && p < rem_pnt[remote].command_end) p++;
				if (cmd_pnt[p].toggle_seq == s) {
					cmd_pnt[i].toggle_pos++;
					return (p);
				}
				else {
					cmd_pnt[i].toggle_pos = 2;
					return (i);
				}
			}
			return (i);
		}
		i++;
	}
	return (-1);
}


int DBFindRemote (char remote[])
{
	int i = 0;
	while (i < rem_cnt) {
		if (!memcmp (remote,rem_pnt[i].name,80)) return (i);
		i++;
	}
	return (-1);
}



int	StoreSwitch (word id,word num,char *rem,char *com,word mode)
{
	int i = 0;
	while (i < switch_cnt) {
		if (switches[i].id == id && switches[i].num == num) {
			switches[i].mode = mode;
			strcpy (switches[i].remote,rem);
			strcpy (switches[i].command,com);
			return (1);
		}
		i++;
	}
	
	switches = realloc (switches,(switch_cnt + 1) * sizeof (SWITCH));
	switches[switch_cnt].id = id;
	switches[switch_cnt].num = num;
	switches[switch_cnt].mode = mode;
	strcpy (switches[switch_cnt].remote,rem);
	strcpy (switches[switch_cnt].command,com);
	switch_cnt++;

	return (0);
}


void WriteSwitches (void)
{
	int i = 0;
	char m;
	FILE *fp;

	if (switch_cnt == 0) return;

	fp = DBOpenFile ("switches.cfg","w");

	while (i < switch_cnt) {
		if (*switches[i].remote && *switches[i].command) {
			if (switches[i].mode == 1) m = 'T';
			if (switches[i].mode == 2) m = 'S';
			if (switches[i].mode == 4) m = '0';
			if (switches[i].mode == 8) m = '1';
			fprintf (fp,"[ID]%02d.%02d	[NR]%d	[%c]	%s	%s\n",switches[i].id >> 8,switches[i].id & 0xff,switches[i].num,m,switches[i].remote,switches[i].command);
		}
		i++;
	}

	fclose (fp);

}

int FindSwitch (word id,word num,char *rem,char *com,word *mode)
{
	int i = 0;
	*mode = 0;
	*rem = 0;
	*com = 0;
	while (i < switch_cnt) {
		if (switches[i].id == id && switches[i].num == num) {
			*mode = switches[i].mode;
			strcpy (rem,switches[i].remote);
			strcpy (com,switches[i].command);
			return (1);
		}
		i++;
	}
	return (0);
}

void ReadSwitches (void)
{
	FILE *fp;
	int i,j;
	char ln[2048],*data;
	SWITCH sw;

	fp = DBOpenFile ("switches.cfg","r");

	if (!fp) return;


	switch_cnt = 0;
	switches = 0;
	data = DBReadString (ln,fp,NULL);
	while (data) {
		i = 0;

		while (ln[i] && ln[i] != ']') i++;
		i++;
		if (ln[i+2] == '.') {
			ln[i+2] = 0;
			sw.id = atoi (ln + i) * 256 + atoi (ln + i + 3);
			i += 3;
		}
		else sw.id = atoi (ln + i);

		while (ln[i] && ln[i] != ']') i++;
		i++;
		sw.num = atoi (ln + i);
		sw.mode = 0;

		while (ln[i] && ln[i] != '[') i++;
		i++;
		if (ln[i] == 'T') sw.mode = 1;
		if (ln[i] == 'S') sw.mode = 2;
		if (ln[i] == '0') sw.mode = 4;
		if (ln[i] == '1') sw.mode = 8;

		i += 2;

		while (ln[i] == ' ' || ln[i] == '\t') i++;
		j = i;
		while (ln[i] != ' ' && ln[i] != '\t') i++;
		ln[i++] = 0;
		strcpy (sw.remote,ln + j);
		while (ln[i] == ' ' || ln[i] == '\t') i++;
		strcpy (sw.command,ln + i);

		switches = realloc (switches,(switch_cnt + 1) * sizeof (SWITCH));
		switches[switch_cnt++] = sw;

		data = DBReadString (ln,fp,NULL);
	}

	
	fclose (fp);
}

void ReadRoutingTable (void)
{
	FILE *fp;

	int res;

	fp = DBOpenFile ("routing","r");

	if (!fp) return;														// No routing table found

	res = DBStoreRooms (fp);												// Read Rooms
	if (!res) {
		DBStoreRouting (fp,"SEND-ROUTING",&send_routing,&send_routing_cnt);	// Read Recv Routing
		DBStoreRouting (fp,"RECV-ROUTING",&recv_routing,&recv_routing_cnt);	// Read Send Routing
	}

	fclose (fp);
	return;

}


void ReadAppConfig (void)
{

#ifdef WIN32
	int i,j,p,cf,res,cnum;
	FILE *fp;
	char ln[2048],*data,lchar;

	fp = DBOpenFile ("apps.cfg","r");

	if (!fp) return;														// No APP Config found

	cf = app_cnt = 0;
	rewind (fp);

	data = ln;

	while (data) {
		data = DBReadString (ln,fp,NULL);
		if (!data) {
			fclose (fp);
			return;
		}
		if (!strncmp (data,"[APP]",5)) {
			strcpy (app_pnt[app_cnt].name,data + 5);
			app_pnt[app_cnt].com_cnt = 0;
		}
		else if (!strncmp (data,"[CLASSNAME]",11)) {
			strcpy (app_pnt[app_cnt].classname,data + 11);
		}
		else if (!strncmp (data,"[APPNAME]",9)) {
			strcpy (app_pnt[app_cnt].appname,data + 9);
		}
		else if (!strncmp (data,"[ACTIVE]",8)) {
			app_pnt[app_cnt].active = 1;
		}
		else if (!strncmp (data,"[TYPE]",6)) {
			if (!strcmp (data+6,"MCE")) app_pnt[app_cnt].type = TYPE_MCE;
			if (!strcmp (data+6,"KEY")) app_pnt[app_cnt].type = TYPE_KEY;
			if (!strcmp (data+6,"APPCOM")) app_pnt[app_cnt].type = TYPE_APPCOM;
			if (!strcmp (data+6,"COM")) app_pnt[app_cnt].type = TYPE_COM;
		}
		else if (!strncmp (data,"[REMOTE]",8)) {
			strcpy (app_pnt[app_cnt].remote,data + 8);
			ConvertLcase (app_pnt[app_cnt].remote,strlen (app_pnt[app_cnt].remote));
		}
		else if (!strncmp (data,"[COMMANDS]",10)) {
			cf = 1;
		}
		else if (!strncmp (data,"[END-COMMANDS]",14)) {
			cf = 0;
		}
		else if (!strncmp (data,"[END-APP]",9)) {
			app_cnt++;
		}
		else if (cf) {
			i = 0;
			cnum = 0;
			while (data[i] && data[i] != ' ' && data[i] != '\t' && data[i] != '[') i++;
			if (data[i] == ' ' || data[i] == '\t') data[i++] = 0;
			ConvertLcase (data,strlen (data));
			while (data[i] && data[i] != '[') i++;
			if (!data[i]) continue;
			res = DBFindRemoteCommand (app_pnt[app_cnt].remote,data,&app_pnt[app_cnt].com[app_pnt[app_cnt].com_cnt].comnum,&app_pnt[app_cnt].remnum);
			if (res) continue;
			p = i;
			while (data[p]) {
				i = p + 5;
				j = i;
				while (data[j] && data[j] != ' ' && data[j] != '\t' && data[j] != '[') j++;
				lchar = data[j];
				data[j] = 0;
				ConvertLcase (data+i,strlen (data+i));
				if (!strncmp (data+p,"[FNC]",5)) {
					res = GetFunctionCode (app_pnt[app_cnt].type,data+i);
					if (!res) goto notfound;
					app_pnt[app_cnt].com[app_pnt[app_cnt].com_cnt].type[cnum] = app_pnt[app_cnt].type;
					app_pnt[app_cnt].com[app_pnt[app_cnt].com_cnt].function[cnum] = res;
					cnum++;
				}
				else if (!strncmp (data+p,"[KEY]",5)) {
					res = GetKeyCode (data+i);
					if (!res) goto notfound;
					app_pnt[app_cnt].com[app_pnt[app_cnt].com_cnt].type[cnum] = TYPE_KEY;
					app_pnt[app_cnt].com[app_pnt[app_cnt].com_cnt].function[cnum] = res;
					cnum++;
				}
				else if (!strncmp (data+p,"[KEF]",5)) {
					res = GetKeyCode (data+i);
					if (!res) goto notfound;
					app_pnt[app_cnt].com[app_pnt[app_cnt].com_cnt].type[cnum] = TYPE_KEYF;
					app_pnt[app_cnt].com[app_pnt[app_cnt].com_cnt].function[cnum] = res;
					cnum++;
				}
				else if (!strncmp (data+p,"[CHR]",5)) {
					res = GetKeyCode (data+i);
					if (!res) goto notfound;
					app_pnt[app_cnt].com[app_pnt[app_cnt].com_cnt].type[cnum] = TYPE_CHR;
					app_pnt[app_cnt].com[app_pnt[app_cnt].com_cnt].function[cnum] = res;
					cnum++;
				}
				else if (!strncmp (data+p,"[APP]",5)) {
					res = GetFunctionCode (TYPE_APPCOM,data+i);
					if (!res) goto notfound;
					app_pnt[app_cnt].com[app_pnt[app_cnt].com_cnt].type[cnum] = TYPE_APPCOM;
					app_pnt[app_cnt].com[app_pnt[app_cnt].com_cnt].function[cnum] = res;
					cnum++;
				}
				else if (!strncmp (data+p,"[RUN]",5)) {
					app_pnt[app_cnt].com[app_pnt[app_cnt].com_cnt].type[cnum] = TYPE_RUN;
					cnum++;
				}
notfound:
				data[j] = lchar;
				p = j;
			}
		if (cnum) app_pnt[app_cnt].com_cnt++;
		}
	}
	
#endif
}

void DBStoreRouting (FILE *fp,char section[],ROUTING **pnt,long *cnt)
{
	int i;
	char ln[2048],*data;

	*cnt = 0;
	rewind (fp);
	data = DBFindSection (fp,section,NULL,NULL,NULL);

	if (!data) return;

	while (data) {
		data = DBFindSection (fp,"REM",ln,"[END]",NULL);
		if (data) {
			i = 0;
			while (data[i] && data[i] != '[') i++;
			if (data[i] == '[') {
				data[i++] = 0;
				while (data[i] && data[i] != ']') i++;
				if (data[i] == ']') {
					i++;
					*pnt = realloc (*pnt,(*cnt + 1) * sizeof (ROUTING));
					memset (*pnt + *cnt,0,sizeof (ROUTING));
					if (strlen (data) > 80) data[80] = 0;
					ConvertLcase (data,strlen (data));
					((*pnt) + *cnt)->target_mask = (word)strtol (data + i,NULL,0);
					strncpy (((*pnt) + *cnt)->name,data,80);
					(*cnt)++;
				}
			}
		}
	}
}

int DBStoreRooms (FILE *fp)
{
	int i;
	char ln[2048],*data;

	
	room_cnt = 0;
	rewind (fp);
	data = DBFindSection (fp,"ADDRESS",NULL,NULL,NULL);

	if (!data) return (-1);
	while (data) {
		data = DBFindSection (fp,"NAME",ln,NULL,NULL);
		if (data) {
			i = 0;
			while (data[i] && data[i] != '[') i++;
			if (data[i] == '[') {
				data[i++] = 0;
				while (data[i] && data[i] != ']') i++;
				if (data[i] == ']') {
					i++;
					rooms = realloc (rooms,(room_cnt + 1) * sizeof (ROOMS));
					memset (rooms + room_cnt,0,sizeof (ROOMS));
					if (strlen (data) > 80) data[80] = 0;
					ConvertLcase (data,strlen (data));
					rooms[room_cnt].addr = atoi (data + i);
					strncpy (rooms[room_cnt].name,data,80);
					room_cnt++;
				}
			}
		}
	}
	return (0);
}



int DBReadCommandFile (char remote[])
{
	FILE *fp;
	char includeremote[100];
	char st[256];
	int res;

	fp = DBOpenFile (remote,"r");

	if (!fp) return (ERR_DBOPENINPUT);

	res = DBStoreRemote (fp,includeremote);

	if (*includeremote) {
		if (strcmp (includeremote + strlen (includeremote) - 4,".rem")) strcat (includeremote,".rem");
		fclose (fp);
		fp = DBOpenFile (includeremote,"r");

		if (!fp) return (ERR_DBOPENINCLUDE);
	}

	res = DBStoreTimings (fp);
	
	res = DBStoreCommands (fp);

	sprintf (st,"Remote %-20s compiled:",rem_pnt[rem_cnt].name);
	log_print (st,LOG_INFO);
	sprintf (st,"  %4d Timings - ",rem_pnt[rem_cnt].timing_end-rem_pnt[rem_cnt].timing_start);
	log_print (st,LOG_INFO);
	sprintf (st,"  %4d Commands\n",rem_pnt[rem_cnt].command_end-rem_pnt[rem_cnt].command_start);
	log_print (st,LOG_INFO);

	rem_cnt++;

	fclose (fp);
	return (0);
}


void FindDuplicateCommands (void)
{
	int i = 0,j;
	int cnt;
	char st[255];

	while (i < cmd_cnt) {
		j = i + 1;
		if (cmd_pnt[i].mode == 0 && cmd_pnt[i].name[strlen (cmd_pnt[i].name) - 1] != '@') {
			cnt = 0;
			while (j < cmd_cnt) {
				if (cmd_pnt[j].mode == 0 && cmd_pnt[j].name[strlen (cmd_pnt[j].name) - 1] != '@') {
					if (cmd_pnt[i].ir_length == cmd_pnt[j].ir_length && !memcmp (cmd_pnt[j].data,cmd_pnt[i].data,cmd_pnt[i].ir_length)) {
						if (!cnt) {
							sprintf (st,"Duplicate Commands for %s.%s: ",rem_pnt[cmd_pnt[i].remote].name,cmd_pnt[i].name);
							log_print (st,LOG_ERROR);
						}
						sprintf (st,"  %s.%s",rem_pnt[cmd_pnt[j].remote].name,cmd_pnt[j].name);
						log_print (st,LOG_ERROR);
						cnt++;
					}
				}
				j++;
			}
			if (cnt) {
				sprintf (st,"\n");
				log_print (st,LOG_ERROR);
			}
		}
		i++;
	}
}


void DBShowStatus (void)
{
	char st[256];
	sprintf (st,"Total: %3d Remotes  - %3d Timings - %3d Commands\n",rem_cnt,tim_cnt,cmd_cnt);
	FindDuplicateCommands ();
	log_print (st,LOG_INFO);
	sprintf (st,"       %3d CCF Data - %3d CCF RAW - %3d CCF Error\n",ccf_data,ccf_raw,ccf_err);
	log_print (st,LOG_INFO);
}

int DBStoreCommands (FILE *fp)
{
	int i,p,j,ccf_rpt,ccf_pause,cmd_length,n,res,maxlen;
	char ln[2048],*data,st[255];
	IRMACRO *m_pnt;

	rem_pnt[rem_cnt].command_start = cmd_cnt;
	rem_pnt[rem_cnt].command_end = cmd_cnt;
	rewind (fp);
	if (time_len == TIME_LEN) maxlen = CODE_LENRAW;
	else maxlen = OLD_LENRAW;
	data = DBFindSection (fp,"COMMANDS",NULL,NULL,NULL);

	if (!data) return (-1);
	while (data) {
		data = DBReadString (ln,fp,NULL);
		if (data && *data == '[') {
			ccf_rpt = ccf_pause = cmd_length = 0;
			data++;
			i = 0;
			while (data[i] && data[i] != ']') i++;
			if (data[i]) {
				cmd_pnt = realloc (cmd_pnt,(cmd_cnt + 1) * sizeof (IRCOMMAND));
				memset (cmd_pnt + cmd_cnt,0,sizeof (IRCOMMAND));
				data[i++] = 0;
				if (strlen (data) > 20) data[20] = 0;
				ConvertLcase (data,strlen (data));
				if (data[strlen (data) - 3] == '#') {
					data[strlen (data) - 3] = 0;
					memcpy (cmd_pnt[cmd_cnt].name,data,strlen (data));
					j = cmd_cnt - 1;
					if (atoi (data + strlen (data) + 1) == 1) while (j >= 0) {
						if (!memcmp (cmd_pnt[j].name,cmd_pnt[cmd_cnt].name,20) && !cmd_pnt[j].toggle_seq) {
							cmd_pnt[j].toggle_seq = 1;
							cmd_pnt[j].toggle_pos = 1;
							j = 0;
						}
						j--;
					}
					cmd_pnt[cmd_cnt].toggle_seq = atoi (data + strlen (data) + 1) + 1;
				}
				else {
					cmd_pnt[cmd_cnt].toggle_seq = 0;
					memcpy (cmd_pnt[cmd_cnt].name,data,strlen (data));
				}

				cmd_pnt[cmd_cnt].toggle_pos = 0;
				cmd_pnt[cmd_cnt].remote = rem_cnt;
				if (!memcmp (data + i,"[RL]",4)) {
					i += 4;
					cmd_length = atoi (data + i);
					while (data[i] && data[i] != '[') i++;
				}
				if (!memcmp (data + i,"[RC]",4)) {
					i += 4;
					ccf_rpt = atoi (data + i);
					ccf_pause = 0;
					while (data[i] && data[i] != '[') i++;
				}
				if (!memcmp (data + i,"[RP]",4)) {
					i += 4;
					ccf_pause = atoi (data + i);
					while (data[i] && data[i] != '[') i++;
				}
				if (!memcmp (data + i,"[RAW]",5)) {
					cmd_pnt[cmd_cnt].command_length = cmd_length;
					cmd_pnt[cmd_cnt].mode = RAW_DATA;
					i += 5;
					p = i;
					while (data[i] && data[i] != '[') i++;
					data[i++] = 0;
					cmd_pnt[cmd_cnt].ir_length = atoi (data+p);
					if (cmd_pnt[cmd_cnt].ir_length <= maxlen) {
						i += 5;
						p = i;
						while (data[i] && data[i] != '[') i++;
						data[i++] = 0;
						cmd_pnt[cmd_cnt].timing = atoi (data+p);
						i += 2;
						p = i;
						i = 0;
						while (i < cmd_pnt[cmd_cnt].ir_length) {
							cmd_pnt[cmd_cnt].data[i] = atoi (data + p) / 8;
							while (data[p] && data[p] != ' ') p++;
							p++;
							i++;
						}
						cmd_cnt++;
					}
				}
				else if (!memcmp (data + i,"[MACRO]",7)) {
					m_pnt = (IRMACRO *)&cmd_pnt[cmd_cnt];
					cmd_pnt[cmd_cnt].command_length = cmd_length;
					cmd_pnt[cmd_cnt].mode = MACRO_DATA;

					i += 7;
					p = i;
					while (data[i] && data[i] != '[') i++;
					data[i++] = 0;
					m_pnt->macro_len = atoi (data+p);
					m_pnt->macro_num = mac_cnt;

					mac_pnt = realloc (mac_pnt,(mac_cnt + m_pnt->macro_len) * sizeof (MACROCOMMAND));
					memset (mac_pnt[mac_cnt].mac_remote,0,m_pnt->macro_len * sizeof (MACROCOMMAND));
					for (n=0;n < m_pnt->macro_len && !strncmp (data+i,"IR]",3);n++) {
						i += 3;
						if (data[i++] != '[') break;
						p = i;
						while (data[i] && data[i] != ']') i++;
						if (data[i] != ']') break;
						data[i++] = 0;
						strcpy (mac_pnt[mac_cnt + n].mac_remote,data + p);
						if (data[i++] != '[') break;
						p = i;
						while (data[i] && data[i] != ']') i++;
						if (data[i] != ']') break;
						data[i++] = 0;
						strcpy (mac_pnt[mac_cnt + n].mac_command,data + p);
						if (data[i++] != '[') break;
						p = i;
						while (data[i] && data[i] != ']') i++;
						if (data[i] != ']') break;
						data[i++] = 0;
						mac_pnt[mac_cnt + n].pause = atoi (data + p);
						i++;

					}
					m_pnt->macro_len = n;
					mac_cnt += n;
					cmd_cnt++;

				}
				else if (!memcmp (data + i,"[CCF]",5)) {
					cmd_pnt[cmd_cnt].command_length = cmd_length;
					res = DBStoreCCFCode (data + i + 5);
					if (!res) {
						sprintf (st,"**** CCF Error: %s [See below for Remote name]\n",cmd_pnt[cmd_cnt].name);
						log_print (st,LOG_ERROR);
					}
					else cmd_cnt += res;
				}
				else {
					cmd_pnt[cmd_cnt].command_length = cmd_length;
					cmd_pnt[cmd_cnt].timing = atoi (data + i + FindLineSection (data+i,"T"));
					strcpy (cmd_pnt[cmd_cnt].data,data + i + FindLineSection (data+i,"D"));
					cmd_pnt[cmd_cnt].ir_length = strlen (cmd_pnt[cmd_cnt].data);
					cmd_pnt[cmd_cnt].mode = 0;
					if (tim_pnt[cmd_pnt[cmd_cnt].timing + rem_pnt[rem_cnt].timing_start].time_cnt <= time_len) cmd_cnt++;
				}

			}

		}
	}

	rem_pnt[rem_cnt].command_end = cmd_cnt;
	return (0);

}


int DBStoreCCFCode (char cd[])
{
	int res;
	IRDATA ird;
	IRRAW *irr;
	
	res = DecodeCCF (cd,&ird,START);

	if (res <= 0) {
		ccf_err++;
		return (0);
	}

	if (res & 4) {
		ccf_raw++;
		irr = (IRRAW *)&ird;

		cmd_pnt[cmd_cnt].mode = irr->mode;

		cmd_pnt[cmd_cnt].ir_length = irr->ir_length;
		cmd_pnt[cmd_cnt].timing = irr->transmit_freq;

		memcpy (cmd_pnt[cmd_cnt].data,irr->data,irr->ir_length);

		if (res & 1) {
			cmd_cnt++;

			cmd_pnt = realloc (cmd_pnt,(cmd_cnt + 1) * sizeof (IRCOMMAND));
			memset (cmd_pnt + cmd_cnt,0,sizeof (IRCOMMAND));
			cmd_pnt[cmd_cnt].toggle_seq = 0;
			memcpy (cmd_pnt[cmd_cnt].name,cmd_pnt[cmd_cnt-1].name,strlen (cmd_pnt[cmd_cnt-1].name));
			strcat (cmd_pnt[cmd_cnt].name,"@");

			DecodeCCF (cd,&ird,REPEAT);
			cmd_pnt[cmd_cnt].mode = irr->mode;

			cmd_pnt[cmd_cnt].ir_length = irr->ir_length;
			cmd_pnt[cmd_cnt].timing = irr->transmit_freq;

			memcpy (cmd_pnt[cmd_cnt].data,irr->data,irr->ir_length);
			cmd_pnt[cmd_cnt].remote = rem_cnt;
		}

		return (1);
	}

	if (res & 2) {
		ccf_data++;
		
		tim_pnt = realloc (tim_pnt,(tim_cnt + 1) * sizeof (IRTIMING));
		memset (&tim_pnt[tim_cnt],0,sizeof (IRTIMING));

		memcpy (&tim_pnt[tim_cnt].ir_length,&ird.ir_length,sizeof (IRTIMING) - 4);

		cmd_pnt[cmd_cnt].mode = 0;

		cmd_pnt[cmd_cnt].ir_length = ird.ir_length;
		cmd_pnt[cmd_cnt].timing = tim_cnt - rem_pnt[rem_cnt].timing_start;
		cmd_pnt[cmd_cnt].remote = rem_cnt;

		memcpy (cmd_pnt[cmd_cnt].data,ird.data,ird.ir_length);

		tim_cnt++;
		if (res & 1) {
			cmd_cnt++;

			cmd_pnt = realloc (cmd_pnt,(cmd_cnt + 1) * sizeof (IRCOMMAND));
			memset (cmd_pnt + cmd_cnt,0,sizeof (IRCOMMAND));
			cmd_pnt[cmd_cnt].toggle_seq = 0;
			memcpy (cmd_pnt[cmd_cnt].name,cmd_pnt[cmd_cnt-1].name,strlen (cmd_pnt[cmd_cnt-1].name));
			strcat (cmd_pnt[cmd_cnt].name,"@");

			DecodeCCF (cd,&ird,REPEAT);
			tim_pnt = realloc (tim_pnt,(tim_cnt + 1) * sizeof (IRTIMING));
			memset (&tim_pnt[tim_cnt],0,sizeof (IRTIMING));

			memcpy (&tim_pnt[tim_cnt].ir_length,&ird.ir_length,sizeof (IRTIMING) - 4);

			cmd_pnt[cmd_cnt].mode = 0;

			cmd_pnt[cmd_cnt].ir_length = ird.ir_length;
			cmd_pnt[cmd_cnt].timing = tim_cnt - rem_pnt[rem_cnt].timing_start;
			cmd_pnt[cmd_cnt].remote = rem_cnt;

			memcpy (cmd_pnt[cmd_cnt].data,ird.data,ird.ir_length);

			tim_cnt++;
		}


		rem_pnt[rem_cnt].timing_end = tim_cnt;
		return (1);
	}

	return (0);
}

int DBStoreTimings (FILE *fp)
{
	int i;
	char st[100];
	char ln[256],*data;

	
	rem_pnt[rem_cnt].timing_start = tim_cnt;
	rem_pnt[rem_cnt].timing_end = tim_cnt;

	rewind (fp);
	data = DBFindSection (fp,"TIMING",NULL,NULL,NULL);

	if (!data) return (-1);

	i = 0;
	while (data) {
		sprintf (st,"%d",i);
		data = DBFindSection (fp,st,ln,"[COMMANDS]",NULL);
		if (data) {
			tim_pnt = realloc (tim_pnt,(tim_cnt + 1) * sizeof (IRTIMING));
			StoreIRTiming (tim_pnt + tim_cnt,ln);
			tim_cnt++;
		}
		i++;
	}
	rem_pnt[rem_cnt].timing_end = tim_cnt;

	return (0);
}


int DBStoreRemote (FILE *fp,char newremote[])
{
	char name[100],*data,tra[100];

	newremote[0] = 0;

	rem_pnt = realloc (rem_pnt,(rem_cnt + 1) * sizeof (IRREMOTE));

	data = DBFindSection (fp,"REMOTE",NULL,NULL,NULL);

	if (!data) return (-1);

	data = DBFindSection (fp,"NAME",name,"[TIMING]",NULL);

	if (!data) return (-1);


	memset (&rem_pnt[rem_cnt],0,sizeof (IRREMOTE));
	ConvertLcase (data,strlen (data));
	strncpy (rem_pnt[rem_cnt].name,data,80);
	rem_pnt[rem_cnt].number = rem_cnt;
	GetRemoteAddressMask (rem_cnt);

	data = DBFindSection (fp,"GLOBAL-TOGGLE",NULL,NULL,NULL);
	if (data) rem_pnt[rem_cnt].toggle_pos = 1;

	rewind (fp);

	data = DBFindSection (fp,"TRANSMITTER",tra,"[TIMING]",NULL);
	if (data) {
		ConvertLcase (data,strlen (data));
		if (!strcmp (data,"extern") || !strcmp (data,"external")) rem_pnt[rem_cnt].transmitter = EXTERNAL_LEDS;
		if (!strcmp (data,"intern") || !strcmp (data,"internal")) rem_pnt[rem_cnt].transmitter = INTERNAL_LEDS;
		if (!strcmp (data,"both") || !strcmp (data,"beide") || !strcmp (data,"all") || !strcmp (data,"alle")) rem_pnt[rem_cnt].transmitter = INTERNAL_LEDS | EXTERNAL_LEDS;
	}

	rewind (fp);

	data = DBFindSection (fp,"RCV-LEN",tra,"[TIMING]",NULL);
	if (data) rem_pnt[rem_cnt].rcv_len = (byte)atoi (data);
	else rem_pnt[rem_cnt].rcv_len = 0;

	rewind (fp);

	data = DBFindSection (fp,"INCLUDE",newremote,NULL,NULL);

	return (0);
}

void GetRemoteAddressMask (int num)
{
	long i = 0;

	rem_pnt[num].target_mask = 0xffff;
	rem_pnt[num].source_mask = 0xffff;

	while (i < send_routing_cnt) {
		if (!memcmp (rem_pnt[num].name,send_routing[i].name,80)) {
			rem_pnt[num].target_mask = send_routing[i].target_mask;
		}
		i++;
	}

	i = 0;
	while (i < recv_routing_cnt) {
		if (!memcmp (rem_pnt[num].name,recv_routing[i].name,80)) {
			rem_pnt[num].source_mask = recv_routing[i].target_mask;
		}
		i++;
	}
}

void StoreIRTiming (IRTIMING *irp,char data[])
{
	int i = 0,p,flag = 0,rp;
	char cm[10],par[100];

	memset (irp,0,sizeof (IRTIMING));
	irp -> transmit_freq = 39;

	while (data[i]) {
		if (data[i] == '[') {
			i++;
			p = 0;
			while (data[i] && data[i] != ']') cm[p++] = data[i++];
			cm[p] = 0;
			p = i+1;
			while (data[i] && data[i] != '[') i++;
			strncpy (par,data+p,i-p);
			par[i-p] = 0;
			if (!strcmp (cm,"N")) irp -> time_cnt |= atoi (par);
			if (*cm >= '1' && *cm <= TIME_LEN + 48) StorePulseTiming (irp,cm,par);
			if (!strcmp (cm,"RC")) irp -> ir_repeat = atoi (par);
			if (!strcmp (cm,"RP")) irp -> repeat_pause = (rp = atoi (par));
			if (!strcmp (cm,"SB")) irp -> mode |= START_BIT;
			if (!strcmp (cm,"RS")) irp -> mode |= REPEAT_START;
			if (!strcmp (cm,"NOTOG")) flag = 1;
			if (!strcmp (cm,"FREQ")) {
				if (atoi (par) > 127) {
					if (atoi (par) > 500) irp -> transmit_freq = 255;
					else irp -> transmit_freq = (atoi (par) / 4) | 128;
				}
				else irp -> transmit_freq = atoi (par);
			}
			if (!strcmp (cm,"RC5")) irp -> mode |= RC5_DATA;
			if (!strcmp (cm,"RC6")) irp -> mode |= RC6_DATA;
			if (!strcmp (cm,"IRDA")) {
				irp -> mode |= IRDA_DATA;
				irp -> transmit_freq = 0;
				irp -> repeat_pause = rp / 100;
			}
		}
		else i++;
	}
	if (!((irp ->mode & IRDA_DATA) == IRDA_DATA) && irp -> mode & (RC5_DATA | RC6_DATA)) {
		irp -> mode &= ~START_MASK;
		if (flag) irp -> mode |= NO_TOGGLE;
	}
}

void StorePulseTiming (IRTIMING *irp,char cmd[],char data[])
{
	int i = 0;

	while (data[i] && data[i] >= '0') i++;
	
	if (!data[i]) return;
	data[i++] = 0;

	irp ->pulse_len[atoi (cmd)-1] = atoi (data) / 8;
	irp ->pause_len[atoi (cmd)-1] = atoi (data+i) / 8;
}

int FindLineSection (char ln[],char section[])
{
	unsigned int pnt,len;
	char cmp[256];

	sprintf (cmp,"[%s]",section);

	len = strlen (cmp);

	pnt = 0;

	while (len + pnt < strlen (ln)) {
		if (!memcmp (ln+pnt,cmp,len)) return (pnt+len);
		pnt++;
	}

	return (-1);
}



FILE *DBOpenFile (char remote[],char mode[])
{
	char nm[255];

	sprintf (nm,"%s%c%s",dbpath,PATH_SEPARATOR,remote);

	return (fopen (nm,mode));
}


char *DBFindSection (FILE *fp,char section[],char data[],char end[],long *fpos)
{
	int len;
	static char ln[2048];
	char *pnt;
	char cmp[256];

	sprintf (cmp,"[%s]",section);
	len = strlen (cmp);

	pnt = DBReadString (ln,fp,fpos);
	while (pnt) {
		if (end && !strcmp (ln,end)) return (0);
		if (!strncmp (pnt,cmp,len)) {
			if (data) strcpy (data,pnt+len);
			return (pnt+len);
		}
		pnt = DBReadString (ln,fp,fpos);
		}
	return (0);
}


char *DBReadString (char ln[],FILE *fp,long *fpos)
{
	int i;
	char *pnt;

	do {
		if (fpos) *fpos = ftell (fp);
		pnt = fgets (ln,2048,fp);
		if (!pnt) return (NULL);
		while (*pnt == ' ' || *pnt == '\t') pnt++;

		i = strlen (pnt) - 1;

		while (i && pnt[i-1] && pnt[i-1] <= ' ') i--;

		if (pnt[i] <= ' ') pnt[i] = 0;
	} while (*pnt == 0);

	return (pnt);
}

void ConvertLcase (char *pnt,int len)
{
	int i = 0;
	while (i < len) {
		pnt[i] = tolower (pnt[i]);
		i++;
	}
}

