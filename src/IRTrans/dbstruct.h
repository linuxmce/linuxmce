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
#pragma pack(1)


#define COMMAND_MODE_FN		1
#define COMMAND_MODE_KEY	2

typedef struct {
	char name[80];
	word target_mask;
} ROUTING;

typedef struct {
	char name[80];
	byte addr;
} ROOMS;

typedef struct {
	char name[80];
	long number;
	word target_mask;
	word source_mask;
	long timing_start;
	long timing_end;
	long command_start;
	long command_end;
	long toggle_pos;
	byte transmitter;
	byte rcv_len;
} IRREMOTE;


typedef struct {
	long remote;
	byte ir_length;
	byte transmit_freq;
	byte mode;
	word pause_len[TIME_LEN];
	word pulse_len[TIME_LEN];
	byte time_cnt;
	byte ir_repeat;
	byte repeat_pause;

} IRTIMING;

typedef struct {
	char name[20];
	long remote;
	long timing;
	long command_length;
	byte toggle_seq;
	byte toggle_pos;
	byte mode;
	byte ir_length;
	byte data[CODE_LENRAW];
} IRCOMMAND;

typedef struct {
	char mac_remote[80];
	char mac_command[20];
	long pause;
} MACROCOMMAND;

typedef struct {
	char name[20];
	long remote;
	long timing;
	long command_length;
	byte toggle_seq;
	byte toggle_pos;
	byte mode;
	byte ir_length;
	long macro_num;
	long macro_len;
} IRMACRO;

typedef struct {
	word id;
	word num;
	word mode;
	char remote[80];
	char command[20];
} SWITCH;

typedef struct {
	long comnum;
	byte type[8];
	int function[8];
} APPCOMMAND;

typedef struct {
	char name[20];
	char classname[50];
	char appname[100];
	char remote[80];
	long remnum;
	byte type;
	byte com_cnt;
	byte active;
	byte align;
	APPCOMMAND com[50];
} APP;

#pragma pack(8)

