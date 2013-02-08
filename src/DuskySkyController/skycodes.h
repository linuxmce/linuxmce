#ifndef __SKYCODES_H
#define __SKYCODES_H
/*
 * See http://www.heenan.me.uk/control-sky-from-pc/sky-control-codes.html
 * for a full list of possible buttons, below are on a sky+ remote.
 */

#define PWR_BTN		0x000C
#define SKY_BTN         0x0080
//#define TV_BTN
#define GUIDE_BTN	0x00CC
#define SBO_BTN		0x007D
#define SERVICES_BTN    0x007E
#define IACTIVE_BTN	0x00F5

//#define MUTE_BTN
#define INFO		0x00CB
//#define VOL_UP
//#define VOL_DOWN
#define CH_UP		0x0020
#define CH_DOWN		0x0021
#define TEXT_BTN	0x003C	
#define BACKUP_BTN      0x0083
#define HELP		0x0081

#define UP		0x0058
#define DOWN		0x0059
#define LEFT		0x005A
#define RIGHT		0x005B
#define SELECT		0x005C

#define PLAY		0x003E
#define PAUSE		0x0024
#define FFWD		0x0028
#define RWD		0x003D
#define STOP		0x003F
#define RECORD		0x0040

#define RED		0x006D
#define GREEN		0x006E
#define YELLOW		0x006F
#define BLUE		0x0070

#define ZERO		0x0000
#define ONE		0x0001
#define TWO		0x0002
#define THREE		0x0003
#define FOUR		0x0004
#define FIVE		0x0005
#define SIX		0x0006
#define SEVEN		0x0007
#define EIGHT		0x0008
#define NINE		0x0009


#define SKYPLUS   0x0C00 /**< add this to turn a sky code into a sky+ one */
#define SKYPLUSHD 0x5C00 /**< add this to turn a sky code into a sky hd one */

#endif
