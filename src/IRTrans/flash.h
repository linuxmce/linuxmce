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

#define REMOTE_CNT	11

#define F_ENABLEGROUP		0
#define F_COMMAND			1
#define F_VOLUMEMACRO		2
#define F_VOLUMEMACROD		3
#define F_CONFIG			4
#define F_REMOTE			5
#define F_SEND				6
#define F_PREKEY			7

#define F_ERROR				99
#define F_MACRO				100

#define F_MAGIC				0x3542


#define PC_MASK				3
#define PC_RECV				1
#define PC_TRANS			2
#define PC_OFF				3

#ifdef	DETOMA

#define TRANSM_MASK			124

#define TRANSM_EXT1			4
#define TRANSM_EXT2			8
#define TRANSM_EXT3			16
#define TRANSM_EXT4			32
#define TRANSM_INTERN		64

#define TRANSM_ALL			TRANSM_MASK

#else

#define SBUS_MASK			12
#define SBUS_RECV			4
#define SBUS_TRANS			8
#define SBUS_OFF			12

#define TRANSM_MASK			48
#define TRANSM_INTERN		16
#define TRANSM_EXTERN		32
#define TRANSM_BOTH			TRANSM_MASK

#endif

#define ACC_WAIT			0xf0
#define ACC_REPEAT			0xf

#ifndef AVR
#pragma pack(1)
#endif

// Remote = 0: Globale Informationen

typedef struct {
	word dir_cnt;
	word data_pnt;
	word end_pnt;
	word magic;
	word checksum;

	ir_byte trans_setup[REMOTE_CNT];
	ir_byte align;
	word target_mask[REMOTE_CNT];
	word source_mask[REMOTE_CNT];
	unsigned long group_flags[REMOTE_CNT];
} FLASH_CONTENT;

extern FLASH_CONTENT f_content;
extern ir_byte enable_translator;
extern ir_byte pre_key;
extern unsigned long prekey_timeout;


#define CONTENT_LEN ((sizeof (f_content) + 1) / 2)


typedef struct {
	ir_byte type;
	ir_byte len;
	ir_byte remote;
	ir_byte group;
	word flash_adr;
	word source_mask;
	ir_byte accelerator_timeout;
	ir_byte accelerator_repeat;
	ir_byte trans_setup;
	ir_byte pre_key;
	ir_byte data[1];
} FLASH_ENTRY;

typedef struct {
	ir_byte type;
	ir_byte len;
	ir_byte remote;
	ir_byte group;
	word flash_adr;
	word source_mask;
	ir_byte accelerator_timeout;
	ir_byte accelerator_repeat;
	ir_byte trans_setup;
	ir_byte cdata[1];
} FLASH_ENTRY_1;

typedef struct {
	word hashcode;
	word adr;
} HASH_ENTRY;


void flash_init (void);
void flash_ioinit (void);
void read_flash_directory (void);
ir_byte flash_exec (ir_byte adr,ir_byte data[],ir_byte len);
void load_flashcommand (word adr,IRDATA *ird);
void send_flashcommand (FLASH_ENTRY *fentry,ir_byte num);
word find_flashentry (ir_byte data[],ir_byte len,word hpnt,FLASH_ENTRY *fentry);
ir_byte compare_code (ir_byte data[],ir_byte len,word adr,FLASH_ENTRY *fentry);
word get_hashcode (ir_byte data[],ir_byte len);
void read_flashdata (ir_byte *pnt,word adr,word cnt);
void write_flashdata (ir_byte *pnt,word adr,word cnt);
void set_flashadr (word adr);
void read_nextflashdata (ir_byte *pnt,word cnt);
void signal_error (void);
void set_commanddata (int pos,IRDATA *irpnt);
void switch_relay (ir_byte val);
