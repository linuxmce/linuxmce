
/* Bring up a window and play with it */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//#define NOTICE(X)	printf("%s", X);

#include "SDL.h"


void DrawPict(SDL_Surface *screen, 	int speedy, int flip, int nofade)
{
	SDL_Surface *picture;
	SDL_Rect dest, update;
	int i, centered;
	int ncolors;
	SDL_Color *colors, *cmap;

	char *bmpfile = "sample.bmp";		// Sample image 
	//fprintf(stderr, "Loading picture: %s\n", bmpfile);
	picture = SDL_LoadBMP(bmpfile);
	if ( picture == NULL ) {
		//fprintf(stderr, "Couldn't load %s: %s\n", bmpfile, SDL_GetError());
		return;
	}

	// Set the display colors -- on a hicolor display this is a no-op 
	if ( picture->format->palette ) {
		ncolors = picture->format->palette->ncolors;
		colors  = (SDL_Color *)malloc(ncolors*sizeof(SDL_Color));
		cmap    = (SDL_Color *)malloc(ncolors*sizeof(SDL_Color));
		memcpy(colors, picture->format->palette->colors,
						ncolors*sizeof(SDL_Color));
	} else {
		int       r, g, b;

		// Allocate 256 color palette 
		ncolors = 256;
		colors  = (SDL_Color *)malloc(ncolors*sizeof(SDL_Color));
		cmap    = (SDL_Color *)malloc(ncolors*sizeof(SDL_Color));

		// Set a 3,3,2 color cube 
		for ( r=0; r<8; ++r ) {
			for ( g=0; g<8; ++g ) {
				for ( b=0; b<4; ++b ) {
					i = ((r<<5)|(g<<2)|b);
					colors[i].r = r<<5;
					colors[i].g = g<<5;
					colors[i].b = b<<6;
				}
			}
		}
	}
//NOTICE("testwin: setting colors\n");
	if ( ! SDL_SetColors(screen, colors, 0, ncolors) &&
				(screen->format->palette != NULL) ) {
		//fprintf(stderr, 
//"Warning: Couldn't set all of the colors, but SDL will map the image\n"
//"         (colormap fading will suffer - try the -warp option)\n"
//		);
	}

	// Set the screen to black (not really necessary) 
	if ( SDL_LockSurface(screen) == 0 ) {
		Uint32 black;
		Uint8 *pixels;

		black = SDL_MapRGB(screen->format, 0, 0, 0);
		pixels = (Uint8 *)screen->pixels;
		for ( i=0; i<screen->h; ++i ) {
			memset(pixels, black,
				screen->w*screen->format->BytesPerPixel);
			pixels += screen->pitch;
		}
		SDL_UnlockSurface(screen);
		SDL_UpdateRect(screen, 0, 0, 0, 0);
	}
	
	// Display the picture 
	if ( speedy ) {
		SDL_Surface *displayfmt;

//fprintf(stderr, "Converting picture\n");
		displayfmt = SDL_DisplayFormat(picture);
		if ( displayfmt == NULL ) {
			//fprintf(stderr,
//				"Couldn't convert image: %s\n", SDL_GetError());
			goto done;
		}
		SDL_FreeSurface(picture);
		picture = displayfmt;
	}
	/*printf("(image surface located in %s memory)\n", 
			(picture->flags&SDL_HWSURFACE) ? "video" : "system");*/
	centered = (screen->w - picture->w)/2;
	if ( centered < 0 ) {
		centered = 0;
	}
	dest.y = (screen->h - picture->h)/2;
	dest.w = picture->w;
	dest.h = picture->h;
//NOTICE("testwin: moving image\n");
	for ( i=0; i<=centered; ++i ) {
		dest.x = i;
		update = dest;
		if ( SDL_BlitSurface(picture, NULL, screen, &update) < 0 ) {
			//fprintf(stderr, "Blit failed: %s\n", SDL_GetError());
			break;
		}
		if ( flip ) {
			SDL_Flip(screen);
		} else {
			SDL_UpdateRects(screen, 1, &update);
		}
	}

#ifdef SCREENSHOT
/*	if ( SDL_SaveBMP(screen, "screen.bmp") < 0 )
		printf("Couldn't save screen: %s\n", SDL_GetError());
		*/
#endif

#ifndef BENCHMARK_SDL
	// Let it sit there for a while 
	SDL_Delay(5*1000);
#endif
	// Fade the colormap 
	if ( ! nofade ) {
		int maxstep;
		SDL_Color final;
		SDL_Color palcolors[256];
		struct {
			Sint16 r, g, b;
		} cdist[256];

//NOTICE("testwin: fading out...\n");
		memcpy(cmap, colors, ncolors*sizeof(SDL_Color));
		maxstep = 32-1;
		final.r = 0xFF;
		final.g = 0x00;
		final.b = 0x00;
		memcpy(palcolors, colors, ncolors*sizeof(SDL_Color));
		for ( i=0; i<ncolors; ++i ) {
			cdist[i].r = final.r-palcolors[i].r;
			cdist[i].g = final.g-palcolors[i].g;
			cdist[i].b = final.b-palcolors[i].b;
		}
		for ( i=0; i<=maxstep/2; ++i ) {	// halfway fade 
			int c;
			for ( c=0; c<ncolors; ++c ) {
				colors[c].r =
					palcolors[c].r+((cdist[c].r*i))/maxstep;
				colors[c].g =
					palcolors[c].g+((cdist[c].g*i))/maxstep;
				colors[c].b =
					palcolors[c].b+((cdist[c].b*i))/maxstep;
			}
			SDL_SetColors(screen, colors, 0, ncolors);
			SDL_Delay(1);
		}
		final.r = 0x00;
		final.g = 0x00;
		final.b = 0x00;
		memcpy(palcolors, colors, ncolors*sizeof(SDL_Color));
		for ( i=0; i<ncolors; ++i ) {
			cdist[i].r = final.r-palcolors[i].r;
			cdist[i].g = final.g-palcolors[i].g;
			cdist[i].b = final.b-palcolors[i].b;
		}
		maxstep /= 2;
		for ( i=0; i<=maxstep; ++i ) {		// finish fade out 
			int c;
			for ( c=0; c<ncolors; ++c ) {
				colors[c].r =
					palcolors[c].r+((cdist[c].r*i))/maxstep;
				colors[c].g =
					palcolors[c].g+((cdist[c].g*i))/maxstep;
				colors[c].b =
					palcolors[c].b+((cdist[c].b*i))/maxstep;
			}
			SDL_SetColors(screen, colors, 0, ncolors);
			SDL_Delay(1);
		}
		for ( i=0; i<ncolors; ++i ) {
			colors[i].r = final.r;
			colors[i].g = final.g;
			colors[i].b = final.b;
		}
		SDL_SetColors(screen, colors, 0, ncolors);
//NOTICE("testwin: fading in...\n");
		memcpy(palcolors, colors, ncolors*sizeof(SDL_Color));
		for ( i=0; i<ncolors; ++i ) {
			cdist[i].r = cmap[i].r-palcolors[i].r;
			cdist[i].g = cmap[i].g-palcolors[i].g;
			cdist[i].b = cmap[i].b-palcolors[i].b;
		}
		for ( i=0; i<=maxstep; ++i ) {	// 32 step fade in 
			int c;
			for ( c=0; c<ncolors; ++c ) {
				colors[c].r =
					palcolors[c].r+((cdist[c].r*i))/maxstep;
				colors[c].g =
					palcolors[c].g+((cdist[c].g*i))/maxstep;
				colors[c].b =
					palcolors[c].b+((cdist[c].b*i))/maxstep;
			}
			SDL_SetColors(screen, colors, 0, ncolors);
			SDL_Delay(1);
		}
//NOTICE("testwin: fading over\n");
	}
	
done:
	// Free the picture and return 
	SDL_FreeSurface(picture);
	free(colors); free(cmap);
	return;
}

int WinMain(int argc, char *argv[])
{
	SDL_Surface *screen;
	int speedy, flip, nofade;
	int delay;
	int w, h;
	int desired_bpp;
	Uint32 video_flags;

	// Set default options and check command-line 
	speedy = 0;
	flip = 0;
	nofade = 0;
	delay = 10;
	w = 640;
	h = 480;
	desired_bpp = 0;
	video_flags = 0;

	if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		//fprintf(stderr,
//			"Couldn't initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}
	atexit(SDL_Quit);			//Clean up on exit

// Initialize the display 
	screen = SDL_SetVideoMode(w, h, desired_bpp, video_flags);
	if ( screen == NULL ) {
		//fprintf(stderr, "Couldn't set %dx%dx%d video mode: %s\n",
//					w, h, desired_bpp, SDL_GetError());
		exit(1);
	}
	/*printf("Set%s %dx%dx%d mode\n",
			screen->flags & SDL_FULLSCREEN ? " fullscreen" : "",
			screen->w, screen->h, screen->format->BitsPerPixel);
	printf("(video surface located in %s memory)\n",
			(screen->flags&SDL_HWSURFACE) ? "video" : "system");*/
	if ( screen->flags & SDL_DOUBLEBUF ) {
		//printf("Double-buffering enabled\n");
		flip = 1;
	}

	// Set the window manager title bar 
	SDL_WM_SetCaption("SDL test window", "testwin");

	// Do all the drawing work 
	DrawPict(screen, speedy, flip, nofade);

	SDL_Delay(delay*1000);

	return(0);
}
