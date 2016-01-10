/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
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
/* $Id: gears.c,v 1.2 1999/10/21 16:39:06 brianp Exp $ */

/*
 * 3-D gear wheels.  This program is in the public domain.
 *
 * Command line options:
 *    -info      print GL implementation information
 *
 *
 * Brian Paul
 */

/* Conversion to GLUT by Mark J. Kilgard */

/*
 * $Log: gears.c,v $
 * Revision 1.2  1999/10/21 16:39:06  brianp
 * added -info command line option
 *
 * Revision 1.1.1.1  1999/08/19 00:55:40  jtg
 * Imported sources
 *
 * Revision 3.2  1999/06/03 17:07:36  brianp
 * an extra quad was being drawn in front and back faces
 *
 * Revision 3.1  1998/11/03 02:49:10  brianp
 * added fps output
 *
 * Revision 3.0  1998/02/14 18:42:29  brianp
 * initial rev
 *
 */


#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <GL/gl.h>
#include <GL/glx.h>

#include <SDL.h>
#include <SDL/SDL_syswm.h>


#include <X11/Xutil.h>
#include <X11/extensions/shape.h>
#include <X11/Xlib.h>

#include <unistd.h>

#ifndef M_PI
#define M_PI 3.14159265
#endif

static GLint T0 = 0;
static GLint Frames = 0;

static Display * pDisplay = NULL;
Window mainWindow;

/**

  Draw a gear wheel.  You'll probably want to call this function when
  building a display list since we do a lot of trig here.
 
  Input:  inner_radius - radius of hole at center
          outer_radius - radius at center of teeth
          width - width of gear
          teeth - number of teeth
          tooth_depth - depth of tooth

 **/

static void
gear(GLfloat inner_radius, GLfloat outer_radius, GLfloat width,
  GLint teeth, GLfloat tooth_depth)
{
  GLint i;
  GLfloat r0, r1, r2;
  GLfloat angle, da;
  GLfloat u, v, len;

  r0 = inner_radius;
  r1 = outer_radius - tooth_depth / 2.0;
  r2 = outer_radius + tooth_depth / 2.0;

  da = 2.0 * M_PI / teeth / 4.0;

  glShadeModel(GL_FLAT);

  glNormal3f(0.0, 0.0, 1.0);

  /* draw front face */
  glBegin(GL_QUAD_STRIP);
  for (i = 0; i <= teeth; i++) {
    angle = i * 2.0 * M_PI / teeth;
    glVertex3f(r0 * cos(angle), r0 * sin(angle), width * 0.5);
    glVertex3f(r1 * cos(angle), r1 * sin(angle), width * 0.5);
    if (i < teeth) {
      glVertex3f(r0 * cos(angle), r0 * sin(angle), width * 0.5);
      glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), width * 0.5);
    }
  }
  glEnd();

  /* draw front sides of teeth */
  glBegin(GL_QUADS);
  da = 2.0 * M_PI / teeth / 4.0;
  for (i = 0; i < teeth; i++) {
    angle = i * 2.0 * M_PI / teeth;

    glVertex3f(r1 * cos(angle), r1 * sin(angle), width * 0.5);
    glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), width * 0.5);
    glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), width * 0.5);
    glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), width * 0.5);
  }
  glEnd();

  glNormal3f(0.0, 0.0, -1.0);

  /* draw back face */
  glBegin(GL_QUAD_STRIP);
  for (i = 0; i <= teeth; i++) {
    angle = i * 2.0 * M_PI / teeth;
    glVertex3f(r1 * cos(angle), r1 * sin(angle), -width * 0.5);
    glVertex3f(r0 * cos(angle), r0 * sin(angle), -width * 0.5);
    if (i < teeth) {
      glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), -width * 0.5);
      glVertex3f(r0 * cos(angle), r0 * sin(angle), -width * 0.5);
    }
  }
  glEnd();

  /* draw back sides of teeth */
  glBegin(GL_QUADS);
  da = 2.0 * M_PI / teeth / 4.0;
  for (i = 0; i < teeth; i++) {
    angle = i * 2.0 * M_PI / teeth;

    glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), -width * 0.5);
    glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), -width * 0.5);
    glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), -width * 0.5);
    glVertex3f(r1 * cos(angle), r1 * sin(angle), -width * 0.5);
  }
  glEnd();

  /* draw outward faces of teeth */
  glBegin(GL_QUAD_STRIP);
  for (i = 0; i < teeth; i++) {
    angle = i * 2.0 * M_PI / teeth;

    glVertex3f(r1 * cos(angle), r1 * sin(angle), width * 0.5);
    glVertex3f(r1 * cos(angle), r1 * sin(angle), -width * 0.5);
    u = r2 * cos(angle + da) - r1 * cos(angle);
    v = r2 * sin(angle + da) - r1 * sin(angle);
    len = sqrt(u * u + v * v);
    u /= len;
    v /= len;
    glNormal3f(v, -u, 0.0);
    glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), width * 0.5);
    glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), -width * 0.5);
    glNormal3f(cos(angle), sin(angle), 0.0);
    glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), width * 0.5);
    glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), -width * 0.5);
    u = r1 * cos(angle + 3 * da) - r2 * cos(angle + 2 * da);
    v = r1 * sin(angle + 3 * da) - r2 * sin(angle + 2 * da);
    glNormal3f(v, -u, 0.0);
    glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), width * 0.5);
    glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), -width * 0.5);
    glNormal3f(cos(angle), sin(angle), 0.0);
  }

  glVertex3f(r1 * cos(0), r1 * sin(0), width * 0.5);
  glVertex3f(r1 * cos(0), r1 * sin(0), -width * 0.5);

  glEnd();

  glShadeModel(GL_SMOOTH);

  /* draw inside radius cylinder */
  glBegin(GL_QUAD_STRIP);
  for (i = 0; i <= teeth; i++) {
    angle = i * 2.0 * M_PI / teeth;
    glNormal3f(-cos(angle), -sin(angle), 0.0);
    glVertex3f(r0 * cos(angle), r0 * sin(angle), -width * 0.5);
    glVertex3f(r0 * cos(angle), r0 * sin(angle), width * 0.5);
  }
  glEnd();

}

static GLfloat view_rotx = 20.0, view_roty = 30.0, view_rotz = 0.0;
static GLint gear1, gear2, gear3;
static GLfloat angle = 0.0;
static GLubyte* rgbaBuffer = NULL;

static void
draw(void)
{
  static int eug = 1;
  static time_t change = 0;
  time_t now = time(NULL);
  
  unsigned int width_return = 0, height_return = 0;
  Pixmap pixmap_return;
  int x_hot_return = 0, y_hot_return = 0;
  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glPushMatrix();
  glRotatef(view_rotx, 1.0, 0.0, 0.0);
  glRotatef(view_roty, 0.0, 1.0, 0.0);
  glRotatef(view_rotz, 0.0, 0.0, 1.0);

  glPushMatrix();
  glTranslatef(-3.0, -2.0, 0.0);
  glRotatef(angle, 0.0, 0.0, 1.0);
  glCallList(gear1);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(3.1, -2.0, 0.0);
  glRotatef(-2.0 * angle - 9.0, 0.0, 0.0, 1.0);
  glCallList(gear2);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(-3.1, 4.2, 0.0);
  glRotatef(-2.0 * angle - 25.0, 0.0, 0.0, 1.0);
  glCallList(gear3);
  glPopMatrix();

  glPopMatrix();

{
	static int bufsize = 800 * 800 * 4;
	int i=0;
	GLubyte R_comp = 0;
	GLubyte G_comp = 0;
	GLubyte B_comp = 0;
	GLubyte A_comp = 0;
//	GLXContext glxCtx = glXCreateContext(pDisplay, fVisInfo, None, True);
/*	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glPixelStorei(GL_PACK_ALIGNMENT, 4);*/
/*    glPixelStorei(GL_PACK_ALIGNMENT, ALIGNMENT);
	glReadBuffer(GL_BACK);
	glReadPixels(0, 0, 800, 800, GL_RGBA, GL_UNSIGNED_BYTE, (GLubyte*)rgbaBuffer);*/
	
/*	for (i = 0; i < bufsize; i+=4)
	{
		R_comp = i;
		G_comp = i + 1;
		B_comp = i + 2;
		A_comp = i + 3;
		
		if( rgbaBuffer [A_comp] > 0 )
		{
			rgbaBuffer [A_comp] = (GLubyte)255;
			rgbaBuffer [R_comp] = 0;
			rgbaBuffer [G_comp] = 0;
			rgbaBuffer [B_comp] = 0;
		}
	}*/
	
//	glDrawPixels(800, 800, GL_RGBA, GL_UNSIGNED_BYTE, rgbaBuffer);
	
/*	if (!ctx.fPixmapGC)
		ctx.fPixmapGC = XCreateGC(fPimpl->fDpy, ctx.fX11Pixmap, 0, 0);
	
	if (ctx.fPixmapGC)
	{
		// GL buffer read operation gives bottom-up order of pixels, but XImage
		// require top-down. So, change RGB lines first.
		char *dest = ctx.fXImage->data;
		const UChar_t *src = &ctx.fBUBuffer[ctx.fW * 4 * (ctx.fH - 1)];
		for (UInt_t i = 0, e = ctx.fH; i < e; ++i) {
		std::memcpy(dest, src, ctx.fW * 4);
		dest += ctx.fW * 4;
		src -= ctx.fW * 4;
	}*/
	
}

/*{
	XRectangle rects[4] =
	{
		{0, 0, 0, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0}
	};
	
	rects[0].width = 200 * eug;
	rects[0].height = 200 * eug;
	
	eug %= 3;
	eug++;
	
	XShapeCombineRectangles (
		pDisplay, mainWindow,
		ShapeBounding,
		0, 0,
		rects,
		4, ShapeSet, Unsorted);
}*/


//	XReadBitmapFile(pDisplay, mainWindow, "", &width_return, &height_return, &pixmap_return, &x_hot_return, &y_hot_return);
	

  SDL_GL_SwapBuffers();

  Frames++;
  {
     GLint t = SDL_GetTicks();
     if (t - T0 >= 5000) {
        GLfloat seconds = (t - T0) / 1000.0;
        GLfloat fps = Frames / seconds;
        printf("%d frames in %g seconds = %g FPS\n", Frames, seconds, fps);
        T0 = t;
        Frames = 0;
     }
  }
  
  
//	XSync(pDisplay, False);
  	
	if( !change )
	{
		change = now;
	}
	if( now - change > 5 )
	{
		change = time(NULL);
		
//		usleep(100000);
		
		GC black_gc, white_gc;
		XGCValues values;
		
		Pixmap pix = XCreatePixmap(pDisplay, mainWindow, 900, 900, 1);
		
		values.foreground = BlackPixel (pDisplay, DefaultScreen (pDisplay));
		black_gc = XCreateGC (pDisplay, pix, GCForeground, &values);
		
		values.foreground = WhitePixel (pDisplay, DefaultScreen (pDisplay));
		white_gc = XCreateGC (pDisplay, pix, GCForeground, &values);
		
		eug %= 3;
		eug++;

		XPoint points[3];
		points[0].x = 450;
		points[0].y = 450 - eug * 100;
		points[1].x = 450 + eug * 100;
		points[1].y = 450;
		points[2].x = 450 - eug * 100;
		points[2].y = 450;
		
		XFillRectangle (pDisplay, pix, white_gc, 0, 0, 899, 899);
		//XFillRectangle (pDisplay, pix, black_gc, 0, 0, eug * 100, eug * 100);
		XFillPolygon(pDisplay, pix, black_gc, points, 3, Convex, CoordModeOrigin);
		
		XShapeCombineMask(pDisplay, mainWindow, ShapeBounding, 0, 0, pix, 0);
		
//		usleep(100000);
		
//		XSync(pDisplay, False);
		
//		usleep(100000);
		
		XFreePixmap(pDisplay, pix);
	}
}


static void
idle(void)
{
  angle += 2.0;
}

/* new window size or exposure */
static void
reshape(int width, int height)
{
  GLfloat h = (GLfloat) height / (GLfloat) width;

  glViewport(0, 0, (GLint) width, (GLint) height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-1.0, 1.0, -h, h, 5.0, 60.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.0, 0.0, -40.0);
}

static void
init(int argc, char *argv[])
{
  static GLfloat pos[4] =
  {5.0, 5.0, 10.0, 0.0};
  static GLfloat red[4] =
  {0.8, 0.1, 0.0, 1.0};
  static GLfloat green[4] =
  {0.0, 0.8, 0.2, 1.0};
  static GLfloat blue[4] =
  {0.2, 0.2, 1.0, 1.0};

  glLightfv(GL_LIGHT0, GL_POSITION, pos);
  glEnable(GL_CULL_FACE);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_DEPTH_TEST);

  /* make the gears */
  gear1 = glGenLists(1);
  glNewList(gear1, GL_COMPILE);
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red);
  gear(1.0, 5.0, 1.0, 20, 0.7);
  glEndList();

  gear2 = glGenLists(1);
  glNewList(gear2, GL_COMPILE);
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green);
  gear(0.5, 4.0, 2.0, 10, 0.7);
  glEndList();

  gear3 = glGenLists(1);
  glNewList(gear3, GL_COMPILE);
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue);
  gear(1.3, 4.0, 0.5, 10, 0.7);
  glEndList();

  glEnable(GL_NORMALIZE);

  if (argc > 1 && strcmp(argv[1], "-info")==0) {
     printf("GL_RENDERER   = %s\n", (char *) glGetString(GL_RENDERER));
     printf("GL_VERSION    = %s\n", (char *) glGetString(GL_VERSION));
     printf("GL_VENDOR     = %s\n", (char *) glGetString(GL_VENDOR));
     printf("GL_EXTENSIONS = %s\n", (char *) glGetString(GL_EXTENSIONS));
  }
}

int main(int argc, char *argv[])
{
  SDL_Surface *screen;
  int done;
  Uint8 *keys;

  SDL_Init(SDL_INIT_VIDEO);

//  SDL_GL_SetAttribute(SDL_GL_RENDER_TYPE,   GLX_RGBA_BIT /*0x8011*/);
//  SDL_GL_SetAttribute(SDL_GL_DRAWABLE_TYPE, GLX_WINDOW_BIT /*0x8010*/);
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE,      8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,    8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,     8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,    8);
//  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,  1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,    0);

  screen = SDL_SetVideoMode(800, 800, 32, SDL_OPENGL|SDL_RESIZABLE);
  if ( ! screen ) {
    fprintf(stderr, "Couldn't set 300x300 GL video mode: %s\n", SDL_GetError());
    SDL_Quit();
    exit(2);
  }
  SDL_WM_SetCaption("Gears", "gears");

{
	rgbaBuffer = malloc( sizeof(GLubyte) * 800 * 800 * 4 );
	SDL_SysWMinfo info;
	SDL_VERSION(&info.version); // this is important!
	int bResult = SDL_GetWMInfo(&info);
	if ( !bResult )
	{
		printf("error in SDL_GetWMInfo\n");
	}
	
	// save the SDL display
	pDisplay = info.info.x11.display;
	mainWindow = info.info.x11.wmwindow;
}

  init(argc, argv);
  reshape(screen->w, screen->h);
  done = 0;
  while ( ! done ) {
    SDL_Event event;

    idle();
    while ( SDL_PollEvent(&event) ) {
      switch(event.type) {
        case SDL_VIDEORESIZE:
          screen = SDL_SetVideoMode(event.resize.w, event.resize.h, 16,
                                    SDL_OPENGL|SDL_RESIZABLE);
          if ( screen ) {
            reshape(screen->w, screen->h);
          } else {
            /* Uh oh, we couldn't set the new video mode?? */;
          }
          break;

        case SDL_QUIT:
          done = 1;
          break;
      }
    }
    keys = SDL_GetKeyState(NULL);

    if ( keys[SDLK_ESCAPE] ) {
      done = 1;
    }
    if ( keys[SDLK_UP] ) {
      view_rotx += 5.0;
    }
    if ( keys[SDLK_DOWN] ) {
      view_rotx -= 5.0;
    }
    if ( keys[SDLK_LEFT] ) {
      view_roty += 5.0;
    }
    if ( keys[SDLK_RIGHT] ) {
      view_roty -= 5.0;
    }
    if ( keys[SDLK_z] ) {
      if ( SDL_GetModState() & KMOD_SHIFT ) {
        view_rotz -= 5.0;
      } else {
        view_rotz += 5.0;
      }
    }

    draw();
    XSync(pDisplay, False);
  }
  SDL_Quit();
  free( rgbaBuffer );
  return 0;             /* ANSI C requires main to return int. */
}
