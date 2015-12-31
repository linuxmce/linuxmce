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
typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef unsigned short uint16_t;

typedef char int8_t;
typedef short int16_t;
typedef int int32_t;
#endif


typedef uint8_t byte;
typedef uint16_t word;





#ifndef AVR
#pragma pack(1)
#define CODE_LEN	176							// 2 Byte mehr für String-Ende 0
#else
#ifdef REMOTECONTROL
#define CODE_LEN	20
#else
#define CODE_LEN	174
#endif
#endif


  


//
#define TIME_LEN	8
//#define TIME_LEN	6
#define RAW_EXTRA	TIME_LEN * 4 + 2
#define CODE_LENRAW	(CODE_LEN + RAW_EXTRA - 2)
#define OLD_LENRAW	(CODE_LEN + (6 * 4 + 2) - 2)
#define MAXLCDLEN 170



typedef struct {
	byte len;
	byte checksumme;
	byte command;

	byte address;
	word target_mask;

	byte ir_length;
	byte transmit_freq;
	byte mode;

	short pause_len[TIME_LEN];
	short pulse_len[TIME_LEN];
	byte time_cnt;
	byte ir_repeat;
	byte repeat_pause;

	byte data[CODE_LEN];
} IRDATA;

typedef struct {
	byte len;
	byte checksumme;
	byte command;

	byte address;
	word target_mask;

	byte ir_length;
	byte transmit_freq;
	byte mode;

	word pause_len[6];
	word pulse_len[6];
	byte time_cnt;
	byte ir_repeat;
	byte repeat_pause;

	byte data[CODE_LEN];
} IRDATA3;

typedef struct {
	byte len;
	byte checksumme;
	byte command;

	byte address;
	word target_mask;

	byte ir_length;
	byte transmit_freq;
	byte mode;

	byte data[CODE_LENRAW];
} IRRAW;

typedef struct {
	byte len;
	byte checksumme;
	byte command;

	byte address;
	word target_mask;

	byte ir_length;
	byte transmit_freq;
	byte mode;

	byte data[MAXLCDLEN];
} IRRAWLCD;


// Mode Flags
#define DEVMODE_PC			0
#define DEVMODE_SEND		1
#define DEVMODE_IR			2
#define DEVMODE_SBUS		4
#define DEVMODE_IRCODE		8
#define DEVMODE_SBUSCODE	16
#define DEVMODE_RAW			32
#define DEVMODE_RAWFAST		64
#define DEVMODE_REPEAT		128

// Extended Mode Flags
#define INTERNAL_LEDS		1
#define EXTERNAL_LEDS		2
#define STANDARD_RCV		4
#define BO_RCV				8
#define BO_MODE				16
#define SELF_REPEAT			32
#define	INTERNAL_REPEAT		64
#define SBUS_UART			128			// Set: SBUS läuft über UART

// Extended Mode2 Flags
#define LRN_TIMEOUT_MASK	7			// Bit 0-2
#define SBUS_BAUD_MASK		24			// Bit 3-4


#define LOCAL_MODE			16
#define	RECEIVE_ALL			17
#define	QUICK_MODE			32
#define RAW_MODE			64
#define REPEAT_MODE			128
#define IR_MODE				256
#define SBUS_MODE			512
#define	INSTANT_MODE		1024



#define SBUS_REPEAT			1
#define HOST_VERSION		2
#define HOST_NETWORK_STATUS	3

#define SBUS_SEND			4
#define SBUS_RESEND			5
#define HOST_SEND			6
#define HOST_RESEND			7


#define SBUS_LEARN			16
#define HOST_LEARNIR		18

#define SBUS_QUICKPARM		48
#define HOST_LEARNIRQUICK	50

#define SBUS_RAWMODE		80
#define HOST_LEARNIRRAW		82

#define SBUS_REPEATMODE		144
#define HOST_LEARNIRREPEAT	146
#define SBUS_RAWREPEATMODE		208
#define HOST_LEARNIRRAWREPEAT	210

#define SBUS_RESET			192
#define SBUS_PING			193
#define SBUS_PONG			194
#define SBUS_PARAMETER		196
#define HOST_SETMODE		197
#define SET_ID				199

#define SET_TRANSLATE_DATA	200
#define SBUS_TRANSLATE_DATA	201
#define READ_TRANSLATE_DATA	202
#define START_FLASH_MODE	203
#define	TRANSFER_FLASH		204

#define	SEND				SBUS_SEND
#define LEARN				SBUS_LEARN
#define	SETMODE				SBUS_PARAMETER

#define ADRESS_MASK			15
#define ADRESS_LOCAL		16
#define ADRESS_ALL			32


#define	START_BIT			1
#define REPEAT_START		2
#define START_MASK			3
#define RC5_DATA			4
#define	RC6_DATA			8
#define IRDA_DATA			12
#define	RAW_DATA			16
#define NO_TOGGLE			2
#define MACRO_DATA			32


#define LCD_BACKLIGHT		1
#define LCD_TEXT			2


#define LCD_INIT			4
#define LCD_SETTIME			8
#define LCD_DISPLAYTIME		16
#define LCD_DEFINECHAR		12
#define LCD_BRIGHTNESS		20
#define LCD_REFRESHDATE		24

#define	LCD_DATA			32

#define	TEMP_GET			1
#define TEMP_RESET			2
#define TEMP_DATA			64
#define NON_IRMODE			224

#define SWITCH_DATA			96

#define IR_CORRECT_455		1				// Entspricht 8 µs Korrektur

#ifdef B_O
#define IR_CORRECT			IR_CORRECT_455
#else
#define IR_CORRECT			8				// Entspricht 64 µs Korrektur
#endif

#define IR_TOLERANCE_BO		50				// Entspricht 280 µs Toleranz
#define IR_TOLERANCE		15				// Entspricht 120 µs Toleranz
#define RCX_TOLERANCE		19				// Entspricht 152 µs Toleranz
#define MAX_IR_REPEAT		5
#define RAW_TOLERANCE		20

#ifndef AVR
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
#define FN_FLASH32	4096
#define FN_FLASH128	8192
#define FN_DUALSND	16384
#endif

#define START			1
#define REPEAT			2


typedef struct {
	byte sbus_len;
	byte sbus_checksumme;
	byte sbus_command;
	byte sbus_address;
	byte mode;
	word target_mask;
	byte hotcode_len;
	byte hotcode[CODE_LENRAW];
} MODE_BUFFER;


typedef struct {
	byte sbus_len;
	byte sbus_checksumme;
	byte sbus_command;
	byte sbus_address;
	byte device_mode;
	word send_mask;
	byte version[10];
	byte extended_mode;
	unsigned long capabilities;
	byte extended_mode2;
} STATUS_LINE;

typedef struct {
	byte sbus_len;
	byte sbus_checksumme;
	byte sbus_command;
	byte sbus_address;
	byte device_mode;
	word send_mask;
	byte version[10];
	byte extended_mode;
	word capabilities;
} STATUS_LINE_3;

typedef struct {
	byte sbus_len;
	byte sbus_checksumme;
	byte sbus_command;
	byte sbus_address;
	byte device_mode;
	word send_mask;
	byte version[10];
	byte extended_mode;
} STATUS_LINE_2;

typedef struct {
	byte sbus_len;
	byte sbus_checksumme;
	byte sbus_command;
	byte sbus_address;
	byte device_mode;
	word send_mask;
	byte version[10];
} STATUS_LINE_1;

typedef struct {
	byte my_adress;
	STATUS_LINE stat[16];
} STATUS_BUFFER;

typedef struct {
	byte my_adress;
	STATUS_LINE_1 stat[16];
} STATUS_BUFFER_1;

typedef struct {
	byte my_adress;
	STATUS_LINE_2 stat[16];
} STATUS_BUFFER_2;

typedef struct {
	byte my_adress;
	STATUS_LINE_3 stat[16];
} STATUS_BUFFER_3;

