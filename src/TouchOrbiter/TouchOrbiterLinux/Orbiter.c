// switches

// standard includes

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <signal.h>


// Graphics includes
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "SDL_mouse.h"

// application includes

#include "ProxyOrbiter.h"
#include "IniFile.h"

// macros

// function prototypes

void proccesMessage (int message);
void *PollSDLEventFunc(void *data);
void DisplayImage(SDL_Surface *picture);
void GetImage(int Socket);
void setError(int err);

// a few global variables

static const char *ServerIP, *ServerPortStr;
static unsigned int ServerPort;

static const char *DisplayWidthStr, *DisplayHeightStr;
static unsigned int DisplayWidth;
static unsigned int DisplayHeight;
//static int CommSocket = 0 ;

static struct IniStore IniStore;

//init SDL stuff
SDL_Surface *screen;
SDL_Surface *picture;
SDL_Surface *sdl_text;
SDL_Color text_color = {255, 10, 10,0};
SDL_Rect pictureLocation;
SDL_Rect textLocation;
SDL_Rect cursorLocation;
TTF_Font *font;

void *RedrawThreadFunc(void *data);
pthread_mutex_t redraw_mutex;
pthread_cond_t redraw_cond;
char RedrawDone = FALSE;

const SDL_VideoInfo* videoinfo;
char runMain = TRUE;
char error = 0;

static const char *reconnectTimeout;
int reconnectTimer = 10;

pthread_mutex_t comm_mutex;

struct MessageQueue hwnd;



// for some reason after QuickReload router the program exits
// after tracing i found out the SIGPIPE brings the program down
// we must catch this signal and throw away

void signal_handler(sig)
int sig;
{
	switch(sig) {
	case SIGHUP:
		fprintf ( stdout,"Hangup Signal received\n");
		break;
	case SIGTERM:
		fprintf ( stdout,"Terminate Signal received\n" );
		break;
	case SIGUSR1:
		fprintf ( stdout,"USR1 Signal received\n" );
		break;
	case SIGPIPE:
		fprintf ( stdout,"PIPE Signal received\n" );
		break;
	default:
		fprintf ( stdout,"Unknown Signal received\n" );
		break;
	}
}



void init_signal_catch(void)
{
//	signal(SIGHUP,signal_handler); /* catch hangup signal */
//	signal(SIGTERM,signal_handler); /* catch kill signal */
//	signal(SIGUSR1,signal_handler); /* catch kill signal */
	signal(SIGPIPE,signal_handler); /* catch kill signal */		
}



int main(void)
{
	char IniFile[MAX_PATH];
	memset(IniFile, 0, MAX_PATH * sizeof(char));
	snprintf(IniFile, MAX_PATH - 1, "Orbiter.ini");
	InitIniFile(&IniStore);
	if (!ReadIniFile(IniFile, &IniStore))
	{
		fprintf(stdout, "Failed to open configuration: %s\n", IniFile);
		return 0;
	}

	hwnd.CommSocket = 0;

	ServerIP = GetIniData(&IniStore, "Network.ServerIP");
	ServerPortStr = GetIniData(&IniStore, "Network.ServerPort");
	ServerPort = atoi(ServerPortStr);	
	reconnectTimeout = GetIniData(&IniStore, "Network.ReconnectTimeOut");
	reconnectTimer = atoi(reconnectTimeout);
	DisplayWidthStr = GetIniData(&IniStore, "Display.Width");
	DisplayWidth = atoi(DisplayWidthStr);
	DisplayHeightStr = GetIniData(&IniStore, "Display.Height");
	DisplayHeight = atoi(DisplayHeightStr);
	
	
	if (ServerIP == NULL || ServerPortStr == NULL)
	{
		fprintf(stdout, "Failed to parse configuration from: %s\n", IniFile);
		return 0;
	}
	
	atexit(SDL_Quit);
	
	init_signal_catch();

	printf("Connect to: %s:%s\n", ServerIP,ServerPortStr);			
	hwnd.CommSocket = ProxyOrbiter_Connect(ServerIP, ServerPort);

	if (hwnd.CommSocket == 0)
	{
		setError(1);
	}

	/* Initialize the SDL library */
	if( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		fprintf(stdout,	"Couldn't initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}

	/* 
		Setting the video mode according to the Orbiter.ini. For mini2440 it is should be 800x480
	
	*/ 
	screen = SDL_SetVideoMode(DisplayWidth, DisplayHeight, 32, SDL_DOUBLEBUF | SDL_HWSURFACE);
	if ( screen == NULL ) {
		fprintf(stdout, "Couldn't set %ux%ux8 video mode: %s\n",DisplayWidth,DisplayHeight,SDL_GetError());
		exit(1);
	}

	videoinfo = SDL_GetVideoInfo();

	printf("%i", videoinfo->blit_hw);

	// Set the title bar
	SDL_WM_SetCaption("TOUCH Orbiter", "TOUCH Orbiter");

	// Initialize SDL_ttf library
   	if (TTF_Init() != 0)
	{
		fprintf(stdout, "TTF_Init() Failed: %s\n", TTF_GetError());
		SDL_Quit();
		exit(1);
	}

	// Load a font
	font = TTF_OpenFont("Vera.ttf", 30);
	if (font == NULL)
	{
		fprintf(stdout, "TTF_OpenFont() Failed: %s\n", TTF_GetError());
		TTF_Quit();
		SDL_Quit();
		exit(1);
	}
	
	pictureLocation.x = 0;
	pictureLocation.y = 0;

	GetImage(hwnd.CommSocket);

	// create and start thread 
	pthread_t SDLEventHandle;
	if ( pthread_create( &SDLEventHandle, NULL, PollSDLEventFunc, NULL) ) 
	{
  		printf("Orbiter... error creating PollSDLEvent thread\n");
		exit(0);
	}


	pthread_mutex_init(&redraw_mutex,NULL);
	pthread_cond_init (&redraw_cond,NULL);
	pthread_t RedrawThreadHandle;
	if ( pthread_create( &RedrawThreadHandle, NULL, RedrawThreadFunc, NULL) ) 
	{
  		printf("Orbiter... error creating Screen Redraw thread\n");
		exit(0);
	}

	pthread_mutex_init(&hwnd.mu_queue,NULL);
	pthread_cond_init (&hwnd.cond,NULL);
	pthread_mutex_init(&comm_mutex,NULL);
	
	hwnd.msg_queue=0;

	ProxyOrbiter_PollNews(ServerIP, ServerPort, &hwnd);

	while (runMain==TRUE)
	{
		pthread_mutex_lock(&hwnd.mu_queue);
		pthread_cond_wait(&hwnd.cond, &hwnd.mu_queue);
		int msg = hwnd.msg_queue;
		hwnd.msg_queue=0;
		pthread_mutex_unlock(&hwnd.mu_queue);

		if(msg!=0)
		{
			proccesMessage(msg);
		}
	}

	pthread_join(SDLEventHandle,NULL);
	
	RedrawDone = TRUE;
	pthread_cond_signal(&redraw_cond);
	pthread_join(RedrawThreadHandle,NULL);
	pthread_mutex_destroy(&redraw_mutex);
	pthread_cond_destroy (&redraw_cond);

	pthread_mutex_destroy(&hwnd.mu_queue);
	pthread_cond_destroy (&hwnd.cond);
	pthread_mutex_destroy(&comm_mutex);

	fprintf(stdout,"Normal Exit\n");
	return 0;
}

void proccesMessage(int message)
{
	switch (message)
	{
		case 1:
			pthread_mutex_lock(&comm_mutex);
			GetImage(hwnd.CommSocket);
			pthread_mutex_unlock(&comm_mutex);
			pthread_cond_signal(&redraw_cond);
			return;

		case 2:
			if (reconnectTimer==0)
			{
				pthread_mutex_lock(&comm_mutex);
				hwnd.CommSocket = ProxyOrbiter_Connect(ServerIP, ServerPort);
				reconnectTimer = atoi(reconnectTimeout);
				if (hwnd.CommSocket!=0)
				{ 
					setError(0);
					GetImage(hwnd.CommSocket);
				}
				pthread_mutex_unlock(&comm_mutex);
				pthread_cond_signal(&redraw_cond);
			}else{
				reconnectTimer--;
			}
			return;
	}
}

void *PollSDLEventFunc(void *data)
{
	SDL_Event event;
	textLocation.x = 50;
	textLocation.y = 240;

	data = NULL;
	
	char done = FALSE;
	while (!done)
	{
		if( SDL_WaitEvent( &event ) ){
			/* We are only worried about SDL_KEYDOWN and SDL_KEYUP events */
			switch( event.type ){
				case SDL_KEYDOWN:
					switch(event.key.keysym.sym) {
						case SDLK_ESCAPE:
							done = TRUE;
						default:
							break;
					}
					break;

				case SDL_MOUSEBUTTONDOWN:
					switch(event.button.button) {
						case SDL_BUTTON_LEFT:
							// Tap and release
							pthread_mutex_lock(&comm_mutex);
							ProxyOrbiter_Touch(hwnd.CommSocket, event.button.x, event.button.y);
							GetImage(hwnd.CommSocket);
							pthread_mutex_unlock(&comm_mutex);
							pthread_cond_signal(&redraw_cond);
							break;

						default:
							break;
					}
					break;

				default:
					break;
			}
		}
	}
	
	ProxyOrbiter_Stop();
	runMain = FALSE;	
	pthread_cond_signal(&hwnd.cond);
	return 0;
}


void *RedrawThreadFunc(void *data)
{
	data = NULL;

	while (RedrawDone==FALSE)
	{
		SDL_FillRect(screen, NULL, RGB_COLOR_GREY);
		if (error!=0)
		{ 
			SDL_BlitSurface(sdl_text, NULL, screen, &textLocation);
		}else{
			SDL_BlitSurface(picture, NULL, screen, &pictureLocation);
		}
		SDL_Flip(screen);
		pthread_mutex_lock(&redraw_mutex);
		pthread_cond_wait(&redraw_cond, &redraw_mutex);
		pthread_mutex_unlock(&redraw_mutex);
	}
	return 0;
}


void setError(int err)
{
	char buffer[50];
	error=err;
	switch( err )
	{
		case 1:
			sprintf(buffer, "NOT CONNECTED");			
			sdl_text = TTF_RenderText_Solid(font, buffer, text_color);
			break;
		default:
			break;
	}
}


void GetImage(int Socket)
{
	unsigned long int length = 0;
	unsigned char *buffer = NULL;
	SDL_RWops *rw;
	char success = FALSE;

	if (Socket != 0)
	{
		success = ProxyOrbiter_GetImage(Socket, &buffer, &length);
	}

	if (success==FALSE)
	{
		setError(1);
		return;
	}
	
	rw = SDL_RWFromMem(buffer, length);
	
	picture = IMG_LoadTyped_RW(rw,1,"PNG");
	if (picture == NULL) {
		fprintf(stdout, "Couldn't load picture: %s\n", SDL_GetError());
		free(buffer);
		return;
	}
	free(buffer);
}
