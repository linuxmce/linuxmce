/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#include <stdio.h>
#include <string.h>
#include <math.h>
//#include <sys/time.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <xine.h>
#include <xine/xineutils.h>

#include "XinePlayer.h"

static Display             *display;
static int                  screen;
static Window               window;
static int                  xpos, ypos, width, height;
static double               pixel_aspect;
//static 

#define INPUT_MOTION (ExposureMask | ButtonPressMask | KeyPressMask | \
                      ButtonMotionMask | StructureNotifyMask |        \
                      PropertyChangeMask | PointerMotionMask)
  
int main(int argc, char **argv)
{
	double            res_h, res_v;
	char             *vo_driver    = "auto";
	char             *ao_driver    = "auto";
	char             *mrl = NULL;
	int               i;

  printf("Test player Kiril + Radu \n");

	if(argc <= 1)
	{
		printf("specify one mrl\n");
		return 0;
	}
  
  if(argc > 1) {
    for(i = 1; i < argc; i++) {
      if(!strcmp(argv[i], "-vo")) {
	vo_driver = argv[++i];
      }
      else if(!strcmp(argv[i], "-ao")) {
	ao_driver = argv[++i];
      }
      else if((!strcmp(argv[i], "-h")) || (!strcmp(argv[i], "--help"))) {
	printf("Options:\n");
	printf("  -ao <ao name>           Audio output plugin name (default = alsa).\n");
	printf("  -vo <vo name>           Video output plugin name (default = Xv).\n");
	return 0;
      }      
      else {
	mrl = argv[i];
      }
    }
  }
  else
    mrl = argv[1];

	// Start 
	if( !XInitThreads() ) 
	{
		printf("XInitThreads() failed\n");
		return 0;
	}

	// XinePlayer
	XinePlayer *pPlayer;
	pPlayer = new XinePlayer();
 
	if( (display = XOpenDisplay(getenv("DISPLAY"))) == NULL )
	{
		printf("XOpenDisplay() failed.\n");
		return 0;
		}
    
	screen       = XDefaultScreen(display);
	xpos         = 0;
	ypos         = 0;
	width        = 640;
	height       = 480;

	XLockDisplay(display);
	window = XCreateSimpleWindow(display, XDefaultRootWindow(display),
			       xpos, ypos, width, height, 1, 0, 0);
	XSelectInput (display, window, INPUT_MOTION);
	XMapRaised(display, window);
  
	res_h = (DisplayWidth(display, screen) * 1000 / DisplayWidthMM(display, screen));
	res_v = (DisplayHeight(display, screen) * 1000 / DisplayHeightMM(display, screen));
	XSync(display, False);
	XUnlockDisplay(display);

	pPlayer->SetOutput( display, &window,screen );
	pixel_aspect          = res_v / res_h;

	if( fabs(pixel_aspect - 1.0) < 0.01)
		pixel_aspect = 1.0;
  
	pPlayer->Open( mrl );
	
	
	//pPlayer->GetInfo();
	pPlayer->LoadPostPlugin( "tvtime" );
	return 0;
	
	pPlayer->Play();
	delete pPlayer;
  
	XLockDisplay(display);
	XUnmapWindow(display,  window);
	XDestroyWindow(display, window);
	XUnlockDisplay(display);
	XCloseDisplay (display);

	return 1;
}

/*
 * Local variables:
 * compile-command: "gcc -Wall -O2 `xine-config --cflags` `xine-config --libs` -I/usr/X11R6/include -L/usr/X11R6/lib -lX11 -lm -o muxine muxine.c"
 * End:
 */
