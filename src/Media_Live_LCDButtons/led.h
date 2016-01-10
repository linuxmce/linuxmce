/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#ifndef led_h
#define led_h

extern "C" 
{
	
extern unsigned int gIconMask;
extern unsigned char gLayout;
extern unsigned char gScrollTime;
extern unsigned char gFlags;
extern int gDisplayMode;

static int OpenHID(const char *pszVendor, const char *pszProduct);
static void FindReports(int fd);
static int SendReport(int fd, const unsigned char *buf, size_t size);
int VFDShowIcons(int fd, unsigned int mask);
int VFDIconSet(int fd, int icon, int state);
int VFDIconOn(int fd, int icon);
int VFDIconOff(int fd, int icon);
int VFDTurnOffIcons(int fd);
int VFDSetVolume(int fd, int level);
int VFDSetString(int fd, int region, int offset, unsigned char *buffer);
int VFDClearString(int fd, int region);
int VFDSetDisplay(int fd, int mode, unsigned char layout, unsigned char time, unsigned char flags);
int VFDUpdateDisplay(int fd, int mode, unsigned char layout, unsigned char time, unsigned char flags);
int VFDGlobalUpdateDisplay(int fd);
int VFDSetScrollRegion(int fd, int region);
int VFDSetScrollTime(int fd, int time);
int VFDEnableDisplay(int fd, int mode, unsigned char layout, unsigned char time, unsigned char flags);
int VFDDisableDisplay(int fd);
int VFDEnableString(int fd, unsigned char ucLayout);
static void _set_pixel(int page, int x, int y, int color);
int VFDGraphicsClearBuffer(int page);
int VFDGraphicsCopyPage(int page, unsigned char *buffer);
int VFDSetGraphics(int fd, unsigned char region, unsigned char *buf);
int VFDGraphicsSendPage(int fd, int page);
int VFDGraphicsShowPage(int fd, int page);
int VFDGraphicsRect(int page, unsigned char color, int srcx, int srcy, int width, int height);
void* KeyboardLoop(void* param);
int StartLCD();
}

#endif
