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
int		SetTransceiverIDEx (ir_byte bus,ir_byte id);
int		GetBusInfo (STATUS_BUFFER *sb);
int		GetBusInfoEx (STATUS_BUFFER *sb,ir_byte bus);
int		GetBusInfoDetail (STATUS_BUFFER *sb,ir_byte bus);
int		SetTransceiverModusEx (ir_byte bus,ir_byte mode,word send_mask,ir_byte addr,char *hotcode,int hotlen,ir_byte extended_mode,ir_byte extended_mode2);
int		TransferFlashdataEx (ir_byte bus,word data[],int adr,int len,ir_byte active,long iradr);
int		SendIR (long cmd_num,long address);
int		DoSendIR (IRDATA *ir_data,long rpt_len,int bus);
int		SendIRDataEx (IRDATA *ir_data,long address);
int		SendLCD (IRRAW *ir_data,long address);
int		AdvancedLCD (ir_byte mode,ir_byte data[],int len);
void	LCDBrightness (int val);
int		ResendIREx (ir_byte bus,IRDATA *ir_data);
ir_byte	Convert2OldCarrier (ir_byte carrier);
int		ResetTransceiverEx (ir_byte bus);

int		ReadIR (ir_byte data[]);
int		LearnIREx (IRDATA *ir_data,word addr,word timeout,word ir_timeout);
int		LearnNextIREx (IRDATA *ir_data,word addr,word timeout,word ir_timeout);
int		LearnRawIREx (IRRAW *ir_data,word addr,word timeout,word ir_timeout);
int		LearnRawIRRepeatEx (IRRAW *ir_data,word addr,word timeout,word ir_timeout);
int		LearnRepeatIREx (IRDATA *ir_data,word addr,word timeout,word ir_timeout);
void	ResetComLines (void);

void	PrintPulseData (IRDATA *ir_data);
void	PrintCommand (IRDATA *ir_data);
void	PrintRawData (IRRAW *ir_data);

int		WriteTransceiverCommand (ir_byte pnt);
int		WriteTransceiver (IRDATA *src,ir_byte usb_mode);
ir_byte	get_checksumme (IRDATA *ir);
void	ConvertToIRTRANS3 (IRDATA *ird);
void	ConvertToIRTRANS4 (IRDATA3 *ird);

int GetTransceiverVersion (char version [],unsigned int *cap,unsigned long *serno,ir_byte usbmode);
int		ResetTransceiver (void);
int		InitCommunication (char device[],char version[]);
int		InitCommunicationEx (char devicesel[]);
void	InitConversionTables (void);
void	ConvertLCDCharset (ir_byte *pnt);
void	LCDTimeCommand (ir_byte mode);
void	SetSpecialChars (ir_byte dat[]);

void	FlushUSB (void);
void	FlushCom (void);
void	msSleep (long time);
int		ReadIRString (ir_byte pnt[],int len,word timeout,ir_byte usb_mode);
void	WriteIRString (ir_byte pnt[],int len,ir_byte usb_mode);
void	GetError (int res,char st[]);
void	log_print (char msg[],int level);
void	Hexdump_File (IRDATA *ird);

void	swap_irdata (IRDATA *src,IRDATA *tar);
void	swap_word (word *pnt);
void	swap_long (int32_t *pnt);
int		GetByteorder (void);
void	SwapStatusbuffer (STATUS_BUFFER *sb);
unsigned long GetMsTime (void);
int		get_devices (char sel[],ir_byte testmode);
int		get_detail_deviceinfo (char serno[],char devnode[],ir_byte if_type);
void	sort_ir_devices (char selstring[]);


extern ir_byte byteorder;

#define MINIMUM_SW_VERSION "2.18.04"

#ifdef LINUX

typedef int HANDLE;

#endif

#ifndef FTD2XX_H

typedef void* FT_HANDLE;

#endif


#define MAX_IR_DEVICES	16


#pragma pack(8)

#define IF_RS232	0
#define IF_USB		1

typedef struct {
	ir_byte if_type;					// 0 = RS232    1 = USB
	ir_byte time_len;
	ir_byte raw_repeat;
	ir_byte ext_carrier;
	ir_byte inst_receive_mode;
	ir_byte advanced_lcd;
	char node[20];
	FT_HANDLE usbport;
	HANDLE comport;
	HANDLE event;
	char receive_buffer[4][256];
	int	 receive_cnt[4];
	int  receive_buffer_cnt;
} IOINFO;

typedef struct {
	char name[40];
	char usb_serno[20];
	char device_node[40];
	char cap_string[80];
	char version[20];
	uint32_t fw_serno;
	uint32_t fw_capabilities;
	ir_byte my_addr;
	IOINFO io;
} DEVICEINFO;


int		WriteIRStringEx (DEVICEINFO *dev,ir_byte pnt[],int len);
int		ReadIRStringEx (DEVICEINFO *dev,ir_byte pnt[],int len,word timeout);
int		WriteTransceiverEx (DEVICEINFO *dev,IRDATA *src);
void	FlushIoEx (DEVICEINFO *dev);
int		GetTransceiverVersionEx (DEVICEINFO *dev);
void	FlushComEx(HANDLE fp);
void	CancelLearnEx (DEVICEINFO *dev);
int		ReadInstantReceive (DEVICEINFO *dev,ir_byte pnt[],int len);
int		GetAvailableDataEx (DEVICEINFO *dev);


extern	DEVICEINFO IRDevices[MAX_IR_DEVICES];
extern	int device_cnt;

extern	char hexfile[256];
extern	FILE *hexfp;
extern	ir_byte hexflag;


#define TABLE_CNT	1

extern ir_byte DispConvTable[TABLE_CNT][256];
