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
#endif

#ifdef LINUX
typedef int SOCKET;
typedef int DWORD;
typedef int WSAEVENT;
#define closesocket		close
extern int hCom;
#include <stdint.h>
#endif

#include <stdio.h>

#include "remote.h"
#include "errcode.h"
#include "dbstruct.h"
#include "network.h"
#include "lowlevel.h"
#include "global.h"
#include "fileio.h"

#ifdef WIN32
#include "winio.h"
#include "winusbio.h"
#endif


#include "flash.h"


void set_entry (char entry[],int pos,FLASH_CONTENT *content,byte type,byte remote,byte group,byte shift,byte setup,word sourcemask,word acc_timeout,word acc_repeat,char *version);
void ReadTraReceive (char tp[]);
void ReadTraConfig ();


word flash_data[65536];
FILE *fptrans;

TRANSLATECOMMAND trans[10000];
int trans_num;
extern STATUS_BUFFER remote_statusex[MAX_IR_DEVICES];



void StoreTransItem (TRANSLATECOMMAND *tr)
{
	trans_num = tr->number;
	memcpy (&trans[trans_num],tr,sizeof (TRANSLATECOMMAND));

}


int FileTransData (char nm[])
{
	int i;
	char st[255];

	strcpy (st,nm);
	strcat (st,".tra");
	fptrans = fopen (st,"w");
	if (fptrans == NULL) return ERR_OPENTRANS;

	for (i=0;i <= trans_num;i++) {
		if (trans[i].type == F_CONFIG) {
			fprintf (fptrans,"[CONFIG]\n  [NAME]%s\n",trans[i].remote);

			fprintf (fptrans,"  [SETTINGS]0x%x\n",trans[i].setup);
			fprintf (fptrans,"  [SOURCEMASK]0x%x\n",trans[i].source_mask);
			fprintf (fptrans,"  [TARGETMASK]0x%x\n",trans[i].target_mask);
			fprintf (fptrans,"[END]\n");
		}
		if ((trans[i].type >= F_COMMAND && trans[i].type <= F_VOLUMEMACROD) || trans[i].type >= F_MACRO) {
			if (trans[i].type == F_COMMAND) fprintf (fptrans,"[RCVCOMMAND]\n");
			if (trans[i].type == F_VOLUMEMACRO) fprintf (fptrans,"[VOLUMEMACRO]\n");
			if (trans[i].type == F_VOLUMEMACROD) fprintf (fptrans,"[VOLUMEMACROD]\n");
			if (trans[i].type >= F_MACRO) fprintf (fptrans,"[MACRO %d]\n",trans[i].type - F_MACRO);

			fprintf (fptrans,"  [REMOTE]%s\n",trans[i].remote);
			fprintf (fptrans,"  [COMMAND]%s\n",trans[i].command);
			fprintf (fptrans,"  [SETTINGS]0x%x\n",trans[i].setup);
			fprintf (fptrans,"  [SOURCEMASK]0x%x\n",trans[i].source_mask);
			fprintf (fptrans,"  [ACC_TIMEOUT]%d\n",trans[i].accelerator_timeout);
			fprintf (fptrans,"  [ACC_SETUP]0x%x\n",trans[i].accelerator_repeat);
			fprintf (fptrans,"  [REMOTE_NUM]%d\n",trans[i].remote_num);
			fprintf (fptrans,"  [GROUP_NUM]%d\n",trans[i].group_num);
			fprintf (fptrans,"  [SHIFT_NUM]%d\n",trans[i].multi_num);
			fprintf (fptrans,"[END]\n");
		}
		if (trans[i].type == F_SEND) {
			fprintf (fptrans,"[SEND]\n");

			fprintf (fptrans,"  [REMOTE]%s\n",trans[i].remote);
			fprintf (fptrans,"  [COMMAND]%s\n",trans[i].command);
			fprintf (fptrans,"  [TARGETMASK]0x%x\n",trans[i].target_mask);
			fprintf (fptrans,"  [TIMEOUT]%d\n",trans[i].wait_timeout);
			fprintf (fptrans,"  [REMOTE_NUM]%d\n",trans[i].remote_num);
			fprintf (fptrans,"  [GROUP_NUM]%d\n",trans[i].group_num);
			fprintf (fptrans,"  [SHIFT_NUM]%d\n",trans[i].multi_num);
			fprintf (fptrans,"[END]\n");
		}
		if (trans[i].type == F_REMOTE) {
			fprintf (fptrans,"[REMOTE]\n  [NAME]%s\n",trans[i].remote);
			fprintf (fptrans,"  [SETTINGS]0x%x\n",trans[i].setup);
			fprintf (fptrans,"  [SOURCEMASK]0x%x\n",trans[i].source_mask);
			fprintf (fptrans,"  [TARGETMASK]0x%x\n",trans[i].target_mask);
			fprintf (fptrans,"  [REMOTE_NUM]%d\n",trans[i].remote_num);
			fprintf (fptrans,"[END]\n");
		}
		if (trans[i].type == F_ENABLEGROUP) {
			fprintf (fptrans,"[GROUP]\n");
			fprintf (fptrans,"  [REMOTE]%s\n",trans[i].remote);
			fprintf (fptrans,"  [COMMAND]%s\n",trans[i].command);
			fprintf (fptrans,"  [SETTINGS]0x%x\n",trans[i].setup);
			fprintf (fptrans,"  [SOURCEMASK]0x%x\n",trans[i].source_mask);
			fprintf (fptrans,"  [REMOTE_NUM]%d\n",trans[i].remote_num);
			fprintf (fptrans,"  [GROUP_NUM]%d\n",trans[i].group_num);
			fprintf (fptrans,"[END]\n");
		}
		if (trans[i].type == F_PREKEY) {
			fprintf (fptrans,"[SHIFT]\n");
			fprintf (fptrans,"  [REMOTE]%s\n",trans[i].remote);
			fprintf (fptrans,"  [COMMAND]%s\n",trans[i].command);
			fprintf (fptrans,"  [SETTINGS]0x%x\n",trans[i].setup);
			fprintf (fptrans,"  [SOURCEMASK]0x%x\n",trans[i].source_mask);
			fprintf (fptrans,"  [REMOTE_NUM]%d\n",trans[i].remote_num);
			fprintf (fptrans,"  [GROUP_NUM]%d\n",trans[i].group_num);
			fprintf (fptrans,"  [SHIFT_NUM]%d\n",trans[i].multi_num);
			fprintf (fptrans,"[END]\n");
		}
	}

	fclose (fptrans);
	return (0);
}


int LoadTranslation (TRANSLATEBUFFER *tb,char nm[],word offset)
{
	int i;
	char ln[255],*st;

	if (!offset) {
		strcpy (ln,nm);
		strcat (ln,".tra");
		fptrans = fopen (ln,"r");
		if (fptrans == NULL) return ERR_OPENTRANS;
		trans_num = 0;
		while (st = DBReadString (ln,fptrans,0)) {
			ReadTraReceive (st);
		}
		fclose (fptrans);
	}

	i = 0;
	memset (tb,0,sizeof (TRANSLATEBUFFER));
	tb->statustype = STATUS_TRANSLATE;
	tb->statuslen = sizeof (TRANSLATEBUFFER);
	tb->offset = offset;

	while (i < 30 && offset < trans_num) {
		memcpy (&tb->trdata[i],&trans[offset],sizeof (TRANSLATECOMMAND));
		i++;
		offset++;
	}

	tb->count_buffer = i;
	tb->count_total = trans_num;
	if (i == 30) tb->count_remaining = (short)(trans_num - offset);
	else tb->count_remaining = 0;
	return (0);
}


void ReadTraReceive (char tp[])
{
	char ln[255],*st;

	memset (&trans[trans_num],0,sizeof (TRANSLATECOMMAND));

	trans[trans_num].number = trans_num;
	if (!strcmp (tp,"[RCVCOMMAND]")) trans[trans_num].type = F_COMMAND;
	if (!strcmp (tp,"[VOLUMEMACRO]")) trans[trans_num].type = F_VOLUMEMACRO;
	if (!strcmp (tp,"[VOLUMEMACROD]")) trans[trans_num].type = F_VOLUMEMACROD;
	if (!strncmp (tp,"[MACRO ",7)) trans[trans_num].type = F_MACRO + (byte)strtoul (tp + 7,0,0);
	if (!strcmp (tp,"[SEND]")) trans[trans_num].type = F_SEND;
	if (!strcmp (tp,"[CONFIG]")) trans[trans_num].type = F_CONFIG;
	if (!strcmp (tp,"[REMOTE]")) trans[trans_num].type = F_REMOTE;
	if (!strcmp (tp,"[GROUP]")) trans[trans_num].type = F_ENABLEGROUP;
	if (!strcmp (tp,"[SHIFT]")) trans[trans_num].type = F_PREKEY;
	while ((st = DBReadString (ln,fptrans,0)) && strcmp (st,"[END]")) {
		if (!strncmp (st,"[NAME]",6)) {
			memset (trans[trans_num].remote,' ',80);
			memcpy (trans[trans_num].remote,st + 6,strlen (st + 6));
		}
		if (!strncmp (st,"[REMOTE]",8)) {
			memset (trans[trans_num].remote,' ',80);
			memcpy (trans[trans_num].remote,st + 8,strlen (st + 8));
		}
		if (!strncmp (st,"[COMMAND]",9)) {
			memset (trans[trans_num].command ,' ',20);
			memcpy (trans[trans_num].command ,st + 9,strlen (st + 9));
		}
		if (!strncmp (st,"[SETTINGS]",10)) trans[trans_num].setup = (byte)strtoul (st + 10,0,0);
		if (!strncmp (st,"[SOURCEMASK]",12)) trans[trans_num].source_mask = strtoul (st + 12,0,0);
		if (!strncmp (st,"[TARGETMASK]",12)) trans[trans_num].target_mask = strtoul (st + 12,0,0);
		if (!strncmp (st,"[REMOTE_NUM]",12)) trans[trans_num].remote_num = (byte)strtoul (st + 12,0,0);
		if (!strncmp (st,"[GROUP_NUM]",11)) trans[trans_num].group_num = (byte)strtoul (st + 11,0,0);
		if (!strncmp (st,"[SHIFT_NUM]",11)) trans[trans_num].multi_num = (byte)strtoul (st + 11,0,0);
		if (!strncmp (st,"[ACC_TIMEOUT]",13)) trans[trans_num].accelerator_timeout = (byte)strtoul (st + 13,0,0);
		if (!strncmp (st,"[ACC_SETUP]",11)) trans[trans_num].accelerator_repeat = (byte)strtoul (st + 11,0,0);
		if (!strncmp (st,"[TIMEOUT]",9)) trans[trans_num].wait_timeout = (word)strtoul (st + 9,0,0);
	}
	trans_num++;
}

int SetFlashdataEx (byte bus,long iradr)
{
	int adr,i,j,flashwordsize;
	word val;
	IRDATA ird;
	FLASH_CONTENT *pnt;
	int cmd_count = 0;
	int cmd_num = 0;

	for (i=0;i <= trans_num;i++) if ((trans[i].type >= F_COMMAND && trans[i].type <= F_VOLUMEMACROD) || trans[i].type >= F_MACRO || trans[i].type == F_ENABLEGROUP || trans[i].type == F_PREKEY) {
		if (DBFindRemoteCommandEx (trans[i].remote,trans[i].command,&ird)) trans[i].type = F_ERROR;
		else cmd_count++;
	}

	pnt = (FLASH_CONTENT *)flash_data;
	pnt->dir_cnt = cmd_count;
	pnt->magic = F_MAGIC;
	pnt->checksum = 0;
	for (i=0;i < REMOTE_CNT;i++) pnt->group_flags[i] = 1;
	pnt->trans_setup[0] = trans[0].setup;
	pnt->source_mask[0] = (word)trans[0].source_mask;
	pnt->target_mask[0] = (word)trans[0].target_mask;
	pnt->end_pnt = pnt->data_pnt = CONTENT_LEN;
	cmd_num = 0;

	for (i=0;i <= trans_num;i++) if ((trans[i].type >= F_COMMAND && trans[i].type <= F_VOLUMEMACROD) || trans[i].type >= F_MACRO || trans[i].type == F_ENABLEGROUP || trans[i].type == F_PREKEY) {
		DBFindRemoteCommandEx (trans[i].remote,trans[i].command,&ird);
		ird.data[ird.ir_length] = 0;
		set_entry (ird.data,cmd_num,pnt,trans[i].type,trans[i].remote_num,trans[i].group_num,trans[i].multi_num,trans[i].setup,
					(word)trans[i].source_mask,trans[i].accelerator_timeout,trans[i].accelerator_repeat,remote_statusex[bus].stat[iradr].version+1);
		cmd_num++;
	}

	cmd_num = 0;
	for (i=0;i <= trans_num;i++) {
		if (trans[i].type == F_REMOTE) {
			pnt->trans_setup[trans[i].remote_num] = trans[i].setup;
			pnt->target_mask[trans[i].remote_num] = (word)trans[i].target_mask;
			pnt->source_mask[trans[i].remote_num] = (word)trans[i].source_mask;
		}
		if (trans[i].type == F_ENABLEGROUP) cmd_num++;

		if (trans[i].type == F_PREKEY) cmd_num++;

		if (trans[i].type >= F_COMMAND && trans[i].type <= F_VOLUMEMACROD) {
			if (trans[i+1].type == F_SEND) {
				DBFindRemoteCommandEx (trans[i+1].remote,trans[i+1].command,&ird);
				ird.target_mask = (word)trans[i+1].target_mask;
				set_commanddata (cmd_num,&ird);
				if (trans[i].type == F_VOLUMEMACROD) {
					if (trans[i+2].type == F_SEND) {
						DBFindRemoteCommandEx (trans[i+2].remote,trans[i+2].command,&ird);
						ird.target_mask = (word)trans[i+2].target_mask;
						set_commanddata (-1,&ird);
					}
				}
			}
			cmd_num++;
		}
		if (trans[i].type >= F_MACRO) {
			for (j=0;j < trans[i].type - F_MACRO && trans[i+j+1].type == F_SEND;j++) {
				DBFindRemoteCommandEx (trans[i+j+1].remote,trans[i+j+1].command,&ird);
				ird.target_mask = (word)trans[i+j+1].target_mask;
				ird.address = trans[i+j+1].wait_timeout / 16;
				if (j) set_commanddata (-1,&ird);
				else set_commanddata (cmd_num,&ird);
			}
			cmd_num++;
		}
	}

	val = 0;
	adr = CONTENT_LEN;
	while (adr < pnt->end_pnt) val += flash_data[adr++];
	pnt->checksum = val;

//	cap = iradr >> 8;
//	if (!(cap & FN_TRANSL)) flashwordsize = 64;
//	else 
		
	flashwordsize = 128;

	adr = 0;
	while (adr < pnt->end_pnt) {
		if ((adr + flashwordsize /2) < pnt->end_pnt) TransferFlashdataEx (bus,flash_data + adr,adr,flashwordsize,0,iradr);
		else TransferFlashdataEx (bus,flash_data + adr,adr,flashwordsize,1,iradr);
		adr += flashwordsize / 2;
	}
	return (0);
}



void set_commanddata (int pos,IRDATA *irpnt)
{
	HASH_ENTRY *hash_table;
	FLASH_ENTRY_1 *fentry;
	FLASH_CONTENT *content;

	content = (FLASH_CONTENT *)flash_data;
	hash_table = (HASH_ENTRY *)(flash_data + CONTENT_LEN);
	fentry = (FLASH_ENTRY_1 *)(flash_data + hash_table[pos].adr);

	irpnt -> command = HOST_SEND;
	if (irpnt -> mode & RAW_DATA)
		irpnt -> len = (sizeof (IRDATA) - (CODE_LEN + (RAW_EXTRA))) + irpnt -> ir_length;
	else
		irpnt -> len = (sizeof (IRDATA) - CODE_LEN) + irpnt -> ir_length;

	if (time_len != TIME_LEN) {
		if (irpnt->mode & RAW_DATA) {
			if (irpnt->ir_length > OLD_LENRAW) return;
		}
		else {
			if (irpnt->time_cnt > 6) return;
			ConvertToIRTRANS3 (irpnt);
		}
	}


//	irpnt->target_mask = 0xffff;

	if (pos >= 0) fentry->flash_adr = content->end_pnt;

	memcpy ((void *)(flash_data + content->end_pnt),(void *)irpnt,irpnt->len);
	content->end_pnt += (irpnt->len + 1) / 2;
}


void set_entry (char entry[],int pos,FLASH_CONTENT *content,byte type,byte remote,byte group,byte shift,byte setup,word sourcemask,word acc_timeout,word acc_repeat,char *version)
{
	HASH_ENTRY *hash_table;
	FLASH_ENTRY_1 *fentry;
	
	hash_table = (HASH_ENTRY *)(flash_data + CONTENT_LEN);

	if (!pos) content->data_pnt = CONTENT_LEN + content->dir_cnt * (sizeof (HASH_ENTRY) / 2);

	hash_table[pos].hashcode = get_hashcode (entry,(byte)strlen (entry));
	hash_table[pos].adr = content->data_pnt;

	fentry = (FLASH_ENTRY_1 *)(flash_data + content->data_pnt);
	fentry->type = type;
	fentry->flash_adr = 0;
	if (group) fentry->group = group - 1;
	else fentry->group = 0;
	fentry->remote = remote;
	fentry->source_mask = sourcemask;
	fentry->accelerator_timeout = (byte)acc_timeout;
	fentry->accelerator_repeat = (byte)acc_repeat;
	fentry->trans_setup = setup;
	fentry->len = strlen (entry);

	if (strcmp (version,"4.04.27") >= 0) {
		fentry->cdata[0] = shift;
		strcpy (fentry->cdata+1,entry);
		content->end_pnt = content->data_pnt = (12 + fentry->len + 1) / 2 + hash_table[pos].adr;
	}
	else {
		strcpy (fentry->cdata,entry);
		content->end_pnt = content->data_pnt = (11 + fentry->len + 1) / 2 + hash_table[pos].adr;
	}
	
}



void read_flashdata (byte *pnt,word adr,word cnt)
{
	set_flashadr (adr);
	read_nextflashdata (pnt,cnt);
}

int last_adr;

void set_flashadr (word adr)
{
	last_adr = adr;	
}

void read_nextflashdata (byte *pnt,word cnt)
{
	memcpy (pnt,flash_data + last_adr,cnt);
	last_adr += cnt / 2;
}



word get_hashcode (byte data[],byte len)
{
	byte i;
	word h = 0;

	for (i=0;i < len;i++) h += (data[i] & 7) << ((i * 2) & 15);

	return (h);
}
