/* libvfd demo applicadtion
* Copyright (C) 2006, Advanced Micro Devices, Inc.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*/

/* This is an example application that shows the current time on
* the VFD screen
*/

#include <stdio.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
//#include <linux/hiddev.h>
#include <linux/input.h>
#include <unistd.h>
#include <errno.h>

#include "../pluto_main/Define_Button.h"

//#include <libvfd.h>
#define test_bit(bit, array) (array[bit/8] & (1<<(bit%8)))

void (*g_ButtonCallBackFn)(int PK_Button)=NULL;
int g_QuitButtonThread=0;

void* KeyboardLoop(void* param)
{
	char *pDev = (char *) param;
//	Media_Live_LCDButtons *pMedia_Live_LCDButtons = (Media_Live_LCDButtons *) param;
	int ret,i;
	extern int errno;
	int fd=0;
	char name[256]="Unknown";
	FILE *fp;
	int version;
	struct input_id device_info;
	struct input_event event[64]; /* the events (up to 64 at once) */
	unsigned int evtype_bitmask[EV_MAX/8+1];
	int yalv;
	unsigned int key_bitmask[KEY_MAX/8+1];
	int read_bytes;

	// The /dev/event can jump all around and we have
	// to hunt for it each time.  Start with the value we get
	// from the database, then try replacing the end with a '0' - '9'
	int iRetry;
	for(iRetry=-1;iRetry<=9;++iRetry)
	{
		// The first time we'll leave the device alone
		if( iRetry!=-1 )
			pDev[ strlen(pDev)-1 ] = '0' + iRetry;

		printf("Looking for keyboard on %s\n", pDev);

		// Find the keyboard device
		fd = open(pDev,O_RDWR);
		if(fd==0)
		{
			printf("Couldn't open the VFD device on %s",pDev);
			return NULL;
		}

		ioctl(fd, EVIOCGVERSION, &version);
		printf("evdev driver version is %d.%d.%d", version >>16, (version>>8)&0xff,version &0xff);


		ioctl(fd, EVIOCGID, &device_info);
		printf("vendor 0x%04hx product 0x%04hx version 0x%04hx is on",
			device_info.vendor, device_info.product, device_info.version);
		switch(device_info.bustype)
		{
		case BUS_USB: printf(" a Universal Serial Bus\n"); break;
		default: printf(" an unknown bus type: 0x%04hx\n", device_info.bustype);
		}

		memset(name,0,sizeof(name));  
		if(ioctl(fd, EVIOCGNAME(sizeof(name)),name)<0) 
		{
			perror("evdev ioctl");
		}

		if( strstr(name,"DM-140GINK")==NULL )
		{
			printf("No keyboard -- The device on %s says it's name is %s",pDev,name);
			close(fd);
			continue;
		}
		else
			break;
	}

	printf("Using keyboard on %s", pDev);

	memset(evtype_bitmask,0,sizeof(evtype_bitmask));  
	if(ioctl(fd, EVIOCGBIT(0, sizeof(evtype_bitmask)), evtype_bitmask) < 0)
	{
		perror("evdev ioctl");
	}
	printf("Supported event types:\n");
	for(yalv=0; yalv< EV_MAX; yalv++)
	{
		/* This means that the bit is set in the event types list */
		if(test_bit(yalv,evtype_bitmask))
		{
			printf(" Event type 0x%02x ",yalv);
			switch(yalv)
			{
			case EV_KEY : printf(" (Keys or Buttons)\n"); break;
			case EV_ABS : printf(" (Absolute Axes)\n"); break;
			case EV_LED : printf(" (LEDs)\n"); break;
			case EV_REP : printf(" (Repeat)\n"); break;
			default : printf(" (Unknown event type: 0x%04hx)\n",yalv); break;
			}
		}
	}

	memset(key_bitmask,0,sizeof(key_bitmask));
	if(ioctl(fd,EVIOCGBIT(EV_KEY, sizeof(key_bitmask)), key_bitmask)<0)
	{
		perror("evdev ioctl");
	}
	for(yalv=0;yalv<KEY_MAX;yalv++)
	{
		if(test_bit(yalv, key_bitmask))
		{
			printf("  KEY 0x%02x ", yalv);
			switch(yalv)
			{
			case KEY_BACKSLASH: printf(" (Backslash)\n"); break;
			case KEY_Z: printf(" (Z)\n"); break;
			case KEY_X: printf(" (X)\n"); break;
			case KEY_C: printf(" (C)\n"); break;
			case KEY_F6: printf(" (F6)\n"); break;
			case KEY_F7: printf(" (F7)\n"); break;
			case KEY_F8: printf(" (F8)\n"); break;
			case KEY_F9: printf(" (F9)\n"); break;
			case KEY_F10: printf(" (F10)\n"); break;
			default: printf("Unknown KEY type 0x%04hx\n",yalv); break;
			}
		}
	}

	while(!g_QuitButtonThread)
	{
		read_bytes = read(fd,&event,sizeof(struct input_event)*64);

		if( read_bytes < (int)sizeof(struct input_event) || g_QuitButtonThread )
		{
			printf("evtest: short read");
			return NULL;
		}
		for(yalv=0; yalv < (int)(read_bytes/sizeof(struct input_event)); yalv++)
		{
			if(event[yalv].type)
			{
				printf("Event: time %ld.%06ld, type %d, code %d, value %s = \n",
					event[yalv].time.tv_sec, event[yalv].time.tv_usec, event[yalv].type, 
					event[yalv].code, (event[yalv].value)?"down":"up");

				int PK_Button=0;
				switch(event[yalv].code)
				{
				case KEY_PLAYPAUSE	: PK_Button=BUTTON_Pause_CONST; break;
				case KEY_STOPCD	: PK_Button=BUTTON_Stop_CONST; break;
				case KEY_PREVIOUSSONG	: PK_Button=BUTTON_Skip_Prior_CONST; break;
				case KEY_NEXTSONG	: PK_Button=BUTTON_Skip_Next_CONST; break;
				case KEY_FASTFORWARD	: PK_Button=BUTTON_Fast_Forward_CONST; break;
				case KEY_REWIND	: PK_Button=BUTTON_Rewind_CONST; break;
				default:		printf("Unknown button"); break;
				}
				if( PK_Button && event[yalv].value )  // Only the down
				{
					printf("Got button %d\n",PK_Button);
					if( g_ButtonCallBackFn )
						(*g_ButtonCallBackFn)(PK_Button);
				}
			}
		}
	}
	printf("Closing button thread\n");
	close(fd);
	return NULL;
}
