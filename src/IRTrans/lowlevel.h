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
int		SetTransceiverIDEx (byte bus,byte id);
int		GetBusInfo (STATUS_BUFFER *sb);
int		GetBusInfoEx (STATUS_BUFFER *sb,byte bus);
int		GetBusInfoDetail (STATUS_BUFFER *sb,byte bus);
int		SetTransceiverModusEx (byte bus,byte mode,word send_mask,byte addr,char *hotcode,int hotlen,byte extended_mode,byte extended_mode2);
int		TransferFlashdataEx (byte bus,word data[],int adr,int len,byte active,long iradr);
int		SendIR (long cmd_num,long address);
int		DoSendIR (IRDATA *ir_data,long rpt_len,int bus);
int		SendIRDataEx (IRDATA *ir_data,long address);
int		SendLCD (IRRAW *ir_data,long address);
int		AdvancedLCD (byte mode,byte data[],int len);
void	LCDBrightness (int val);
int		ResendIREx (byte bus,IRDATA *ir_data);
byte	Convert2OldCarrier (byte carrier);
int		ResetTransceiverEx (byte bus);

int		ReadIR (byte data[]);
int		LearnIREx (IRDATA *ir_data,word addr,word timeout,word ir_timeout);
int		LearnNextIREx (IRDATA *ir_data,word addr,word timeout,word ir_timeout);
int		LearnRawIREx (IRRAW *ir_data,word addr,word timeout,word ir_timeout);
int		LearnRawIRRepeatEx (IRRAW *ir_data,word addr,word timeout,word ir_timeout);
int		LearnRepeatIREx (IRDATA *ir_data,word addr,word timeout,word ir_timeout);
void	ResetComLines (void);

void	PrintPulseData (IRDATA *ir_data);
void	PrintCommand (IRDATA *ir_data);
void	PrintRawData (IRRAW *ir_data);

int		WriteTransceiverCommand (byte pnt);
int		WriteTransceiver (IRDATA *src,byte usb_mode);
byte	get_checksumme (IRDATA *ir);
void	ConvertToIRTRANS3 (IRDATA *ird);
void	ConvertToIRTRANS4 (IRDATA3 *ird);

int GetTransceiverVersion (char version [],unsigned int *cap,unsigned long *serno,byte usbmode);
int		ResetTransceiver (void);
int		InitCommunication (char device[],char version[]);
int		InitCommunicationEx (char devicesel[]);
void	InitConversionTables (void);
void	ConvertLCDCharset (byte *pnt);
void	LCDTimeCommand (byte mode);
void	SetSpecialChars (byte dat[]);

void	FlushUSB (void);
void	FlushCom (void);
void	msSleep (long time);
int		ReadIRString (byte pnt[],int len,word timeout,byte usb_mode);
void	WriteIRString (byte pnt[],int len,byte usb_mode);
void	GetError (int res,char st[]);
void	log_print (char msg[],int level);
void	Hexdump_File (IRDATA *ird);

void	swap_irdata (IRDATA *src,IRDATA *tar);
void	swap_word (word *pnt);
void	swap_long (int32_t *pnt);
int		GetByteorder (void);
void	SwapStatusbuffer (STATUS_BUFFER *sb);
unsigned long GetMsTime (void);
int		get_devices (char sel[],byte testmode);
int		get_detail_deviceinfo (char serno[],char devnode[],byte if_type);
void	sort_ir_devices (char selstring[]);


extern byte byteorder;

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
	byte if_type;					// 0 = RS232    1 = USB
	byte time_len;
	byte raw_repeat;
	byte ext_carrier;
	byte inst_receive_mode;
	byte advanced_lcd;
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
	byte my_addr;
	IOINFO io;
} DEVICEINFO;


int		WriteIRStringEx (DEVICEINFO *dev,byte pnt[],int len);
int		ReadIRStringEx (DEVICEINFO *dev,byte pnt[],int len,word timeout);
int		WriteTransceiverEx (DEVICEINFO *dev,IRDATA *src);
void	FlushIoEx (DEVICEINFO *dev);
int		GetTransceiverVersionEx (DEVICEINFO *dev);
void	FlushComEx(HANDLE fp);
void	CancelLearnEx (DEVICEINFO *dev);
int		ReadInstantReceive (DEVICEINFO *dev,byte pnt[],int len);
int		GetAvailableDataEx (DEVICEINFO *dev);


extern	DEVICEINFO IRDevices[MAX_IR_DEVICES];
extern	int device_cnt;

extern	char hexfile[256];
extern	FILE *hexfp;
extern	byte hexflag;


#define TABLE_CNT	1

extern byte DispConvTable[TABLE_CNT][256];
