#include <SDL/SDL_syswm.h>
#include <X11/extensions/XTest.h>
#include <X11/extensions/Xrender.h>
#include <X11/extensions/shape.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>

#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <glib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <fstream>
#include <string>
using namespace std;

#include "../CompositeHelper.h"

/* screen width, height, and bit depth */
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600
#define SCREEN_BPP     32

#define _NET_WM_STATE_REMOVE        0    /* remove/unset property */
#define _NET_WM_STATE_ADD           1    /* add/set property */
#define _NET_WM_STATE_TOGGLE        2    /* toggle property  */
#define _NET_WM_STATE_SET          10    /* shortcut: remove & add property */

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	const Uint32 rmask = 0xff000000;
	const Uint32 gmask = 0x00ff0000;
	const Uint32 bmask = 0x0000ff00;
	const Uint32 amask = 0x000000ff;
	
	const int rshift = 24, gshift = 16, bshift = 8, ashift = 0;
#else
	const Uint32 rmask = 0x000000ff;
	const Uint32 gmask = 0x0000ff00;
	const Uint32 bmask = 0x00ff0000;
	const Uint32 amask = 0xff000000;

	const int rshift = 0, gshift = 8, bshift = 16, ashift = 24;
#endif

/* This is our SDL surface */
SDL_Surface *surface;

GLfloat xrot; /* X Rotation ( NEW ) */
GLfloat yrot; /* Y Rotation ( NEW ) */
GLfloat zrot; /* Z Rotation ( NEW ) */

GLuint face_texture; 
GLuint text_texture;

int pid_player = 0;
int pid_xcompmgr = 0;

string sTextureName;
bool bUseComposite = false;
bool bUseMask = false;
bool bRemoveMask = false;
bool bFullScreen = false;

float MaxU = 1.0f;
float MaxV = 1.0f;

void WaitForChildren()
{
	int status = 0;

	printf("waiting for %d\n", pid_player);
	waitpid(pid_player, &status, 0);

	printf("killing %d\n", pid_xcompmgr);
	kill(pid_xcompmgr, SIGKILL);

	printf("waiting for %d\n", pid_xcompmgr);
	waitpid(pid_xcompmgr, &status, 0);
}

/* function to release/destroy our resources and restoring the old desktop */
void Quit( int returnCode )
{
	/* clean up the window */
	SDL_Quit( );

	WaitForChildren();

	/* and exit appropriately */
	exit( returnCode );
}

int MinPowerOf2(int Value)
{
	int Result = 1;
	while(Result<Value)
		Result*= 2;
	return Result;	
}

Uint32 getpixel(SDL_Surface *pSDL_Surface,int x, int y)
{
    // all pixels outside the pSDL_Surface are black
    if (x < 0 || x >= pSDL_Surface->w || y < 0 || y >= pSDL_Surface->h)
        return SDL_MapRGB(pSDL_Surface->format, 0, 0, 0);

    int bpp = pSDL_Surface->format->BytesPerPixel;
    Uint8 * pixel = (Uint8 *) pSDL_Surface->pixels + y * pSDL_Surface->pitch + x * bpp;

    switch(bpp)
    {
    case 1:
        return * pixel;

    case 2:
        return * (Uint16 *) pixel;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return pixel[0] << 16 | pixel[1] << 8 | pixel[2];
        else
            return pixel[0] | pixel[1] << 8 | pixel[2] << 16;

    case 4:
        return * (Uint32 *) pixel;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}
//-----------------------------------------------------------------------------------------------------
void putpixel(SDL_Surface *pSDL_Surface,int x, int y, Uint32 pixel_color)
{
    // don't try to put a pixel outside the pSDL_Surface
    if (x < 0 || x >= pSDL_Surface->w || y < 0 || y >= pSDL_Surface->h)
        return;

    int bpp = pSDL_Surface->format->BytesPerPixel;
    Uint8 * pixel = (Uint8 *) pSDL_Surface->pixels + y * pSDL_Surface->pitch + x * bpp;

    switch(bpp)
    {
    case 1:
        * pixel = pixel_color;
        break;

    case 2:
        * (Uint16 *) pixel = pixel_color;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
        {
            pixel[0] = (pixel_color >> 16) & 0xff;
            pixel[1] = (pixel_color >> 8) & 0xff;
            pixel[2] = pixel_color & 0xff;
        }
        else
        {
            pixel[0] = pixel_color & 0xff;
            pixel[1] = (pixel_color >> 8) & 0xff;
            pixel[2] = (pixel_color >> 16) & 0xff;
        }
        break;

    case 4:
        * (Uint32 *) pixel = pixel_color | 0xFF000000; //opaque
        break;
    }
}


/* function to load in bitmap as a GL texture */
int LoadGLTextures( )
{
	/* Create storage space for the texture */
	SDL_Surface *pTextureImage; 

	/* Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit */
	if ( ( pTextureImage = IMG_Load(sTextureName.c_str())))
	{
		int Width = MinPowerOf2(pTextureImage->w);
		int Height = MinPowerOf2(pTextureImage->h);

		if(Width != pTextureImage->w || Height != pTextureImage->h)
		{
			Uint32 saved_flags = pTextureImage->flags&(SDL_SRCALPHA|SDL_RLEACCELOK);
			Uint8  saved_alpha = pTextureImage->format->alpha;

			if ((saved_flags & SDL_SRCALPHA) == SDL_SRCALPHA) 
				SDL_SetAlpha(pTextureImage, 0, 0);

			SDL_Surface *LocalSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, Width, Height, 
				pTextureImage->format->BitsPerPixel, 
				pTextureImage->format->Rmask, pTextureImage->format->Gmask, pTextureImage->format->Bmask,
				pTextureImage->format->Amask);

			SDL_BlitSurface(pTextureImage, NULL, LocalSurface, NULL);

			if ((saved_flags & SDL_SRCALPHA) == SDL_SRCALPHA) 
				SDL_SetAlpha(LocalSurface, saved_flags, saved_alpha);

			SDL_FreeSurface(pTextureImage); 
			pTextureImage = LocalSurface;
		}

		/* Create The Texture */
		glGenTextures( 1, &face_texture );

		/* Typical Texture Generation Using Data From The Bitmap */
		glBindTexture( GL_TEXTURE_2D, face_texture );

		if(pTextureImage->format->BytesPerPixel == 4)
		{
			/* Generate The Texture */
			glTexImage2D( GL_TEXTURE_2D, 0, 4, pTextureImage->w,
				pTextureImage->h, 0, GL_RGBA,
				GL_UNSIGNED_BYTE, pTextureImage->pixels );
		}
		else
		{
			/* Generate The Texture */
			glTexImage2D( GL_TEXTURE_2D, 0, 3, pTextureImage->w,
				pTextureImage->h, 0, GL_RGB,
				GL_UNSIGNED_BYTE, pTextureImage->pixels );
		}

		/* Linear Filtering */
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	}
	else
		return false;

	/* Free up any memory we may have used */
	if ( pTextureImage )
		SDL_FreeSurface(pTextureImage);

	return true;
}

/* function to load in bitmap as a GL texture */
void PreRenderInstructions()
{
	if(TTF_Init() != -1)
	{
		TTF_Font *pFont =  TTF_OpenFont("/usr/share/fonts/truetype/msttcorefonts/arial.ttf", 12);

		if(NULL == pFont)
		{
			pFont = TTF_OpenFont("/usr/share/fonts/truetype/ttf-bitstream-vera/Vera.ttf", 12);
		}

		SDL_Color color;
		color.r = 0;
		color.g = 0;
		color.b = 0;
		color.unused = 0;

		if(NULL != pFont)
		{
			SDL_Surface *pSurfaceTextLines[5];

			pSurfaceTextLines[0] = TTF_RenderUTF8_Blended(pFont, "Keys:", color);
			pSurfaceTextLines[1] = TTF_RenderUTF8_Blended(pFont, "c - toggle composite", color);
			pSurfaceTextLines[2] = TTF_RenderUTF8_Blended(pFont, "m - toggle masking", color);
			pSurfaceTextLines[3] = TTF_RenderUTF8_Blended(pFont, "F1 - toggle fullscreen", color);
			pSurfaceTextLines[4] = TTF_RenderUTF8_Blended(pFont, "Escape - quit", color);

			SDL_Surface *pSurfaceText = SDL_CreateRGBSurface(SDL_SWSURFACE, 128, 64, 32, rmask, gmask, bmask, amask);
			
			if ( NULL != pSurfaceText)
			{
				SDL_FillRect(pSurfaceText, NULL, SDL_MapRGBA(pSurfaceText->format, 0xff, 0xff, 0xff, 128));

				for(int i = 0; i < 5; i++)
				{
					if(NULL != pSurfaceTextLines[i])
					{
						SDL_Rect dest_rect;
						memset(&dest_rect, 0, sizeof(SDL_Rect));

						dest_rect.y = i * 12;
						SDL_BlitSurface(pSurfaceTextLines[i], NULL, pSurfaceText, &dest_rect);

						SDL_FreeSurface(pSurfaceTextLines[i]);
					}
				}

				/* Create The Texture */
				glGenTextures( 1, &text_texture );

				/* Typical Texture Generation Using Data From The Bitmap */
				glBindTexture( GL_TEXTURE_2D, text_texture );

				/* Generate The Texture */
				glTexImage2D( GL_TEXTURE_2D, 0, 4, pSurfaceText->w,
					pSurfaceText->h, 0, GL_RGBA,
					GL_UNSIGNED_BYTE, pSurfaceText->pixels );

				/* Linear Filtering */
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
				SDL_FreeSurface(pSurfaceText);
			}

			TTF_CloseFont(pFont);
		}

		TTF_Quit(); 
	}
}

/* function to reset our viewport after a window resize */
int resizeWindow( int width, int height )
{
	/* Height / width ration */
	GLfloat ratio;

	/* Protect against a divide by zero */
	if ( height == 0 )
		height = 1;

	ratio = ( GLfloat )width / ( GLfloat )height;

	/* Setup our viewport. */
	glViewport( 0, 0, ( GLint )width, ( GLint )height );

	/*
	* change to the projection matrix and set
	* our viewing volume.
	*/
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );

	/* Set our perspective */
	gluPerspective( 45.0f, ratio, 0.1f, 100.0f );

	/* Make sure we're chaning the model view and not the projection */
	glMatrixMode( GL_MODELVIEW );

	/* Reset The View */
	glLoadIdentity( );

	glShadeModel(GL_SMOOTH);
	//Realy Nice perspective calculations
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glClearDepth(1.0);                       // Depth Buffer Setup
	//glEnable(GL_DEPTH_TEST);                 // Enable Depth Buffer
	glDepthFunc(GL_LESS);	

	glEnable(GL_TEXTURE_2D);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return( TRUE );
}

/* function to handle key press events */
bool handleKeyPress( SDL_keysym *keysym )
{
	switch ( keysym->sym )
	{
		case SDLK_F1:
			/* F1 key was pressed
			* this toggles fullscreen mode
			*/
			bFullScreen = !bFullScreen;
			SDL_WM_ToggleFullScreen( surface );
			break;

		case SDLK_m:
			bUseMask = !bUseMask;

			if(!bUseMask)
				bRemoveMask = true;

			return false;

		case SDLK_c:
			bUseComposite = !bUseComposite;
			return true;

		case SDLK_ESCAPE:
			Quit( 0 );
			break;

		default:
			break;
	}

	return false;
}

/* general OpenGL initialization function */
int initGL()
{
	/* Load in the texture */
	if(!LoadGLTextures())
		return FALSE;

	/* Prerender texture with instructions */
	PreRenderInstructions();
		
	/* Enable Texture Mapping ( NEW ) */
	glEnable( GL_TEXTURE_2D );

	/* Enable smooth shading */
	glShadeModel( GL_SMOOTH );

	/* Set the background black */
	glClearColor( 0.0f, 0.0f, 0.0f, 0.5f );

	/* Depth buffer setup */
	glClearDepth( 1.0f );

	/* Enables Depth Testing */
	glEnable( GL_DEPTH_TEST );

	/* The Type Of Depth Test To Do */
	glDepthFunc( GL_LEQUAL );

	/* Really Nice Perspective Calculations */
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

	return( TRUE );
}

/* Here goes our drawing code */
int drawGLScene()
{
	/* These are to calculate our fps */
	static GLint T0     = 0;
	static GLint T1mask = 0;
	static GLint Frames = 0;

	/* Clear The Screen And The Depth Buffer */
	glClearColor(0.5f, 0.0f, 0.0f, 0.0f);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	/* Move Into The Screen 5 Units */
	glLoadIdentity( );
	glTranslatef( 0.0f, 0.0f, -5.0f );

//////////////////////////////

	//render the texture with instructions
	glRotatef( 0.0f, 1.0f, 0.0f, 0.0f); /* Rotate On The X Axis */
	glRotatef( 0.0f, 0.0f, 1.0f, 0.0f); /* Rotate On The Y Axis */
	glRotatef( 0.0f, 0.0f, 0.0f, 1.0f); /* Rotate On The Z Axis */

	glBindTexture( GL_TEXTURE_2D, text_texture );

	glBegin(GL_QUADS);

	glTexCoord2f(0.0f, 1.0f); glVertex3f(-2.7f, -2.0f,  0.0f);	// Bottom Left Of The Texture and Quad
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.2f, -2.0f,  0.0f);	// Bottom Right Of The Texture and Quad
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.2f, -1.0f,  0.0f);	// Top Right Of The Texture and Quad
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-2.7f, -1.0f,  0.0f);	// Top Left Of The Texture and Quad

	glEnd();

////////////////////////////

	glRotatef( xrot, 1.0f, 0.0f, 0.0f); /* Rotate On The X Axis */
	glRotatef( yrot, 0.0f, 1.0f, 0.0f); /* Rotate On The Y Axis */
	glRotatef( zrot, 0.0f, 0.0f, 1.0f); /* Rotate On The Z Axis */

	/* Select Our Texture */
	glBindTexture( GL_TEXTURE_2D, face_texture );

	/* NOTE:
	*   The x coordinates of the glTexCoord2f function need to inverted
	* for SDL because of the way SDL_LoadBmp loads the data. So where
	* in the tutorial it has glTexCoord2f( 1.0f, 0.0f ); it should
	* now read glTexCoord2f( 0.0f, 0.0f );
	*/

	glBegin(GL_QUADS);
	// Front Face
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	// Bottom Left Of The Texture and Quad
	glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
	glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	// Top Right Of The Texture and Quad
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	// Top Left Of The Texture and Quad
	// Back Face
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	// Bottom Right Of The Texture and Quad
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	// Top Right Of The Texture and Quad
	glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	// Top Left Of The Texture and Quad
	glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	// Bottom Left Of The Texture and Quad
	// Top Face
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	// Top Left Of The Texture and Quad
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	// Bottom Left Of The Texture and Quad
	glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
	glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	// Top Right Of The Texture and Quad
	// Bottom Face
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	// Top Right Of The Texture and Quad
	glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	// Top Left Of The Texture and Quad
	glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	// Bottom Left Of The Texture and Quad
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
	// Right face
	glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	// Bottom Right Of The Texture and Quad
	glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	// Top Right Of The Texture and Quad
	glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	// Top Left Of The Texture and Quad
	glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	// Bottom Left Of The Texture and Quad
	// Left Face
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	// Bottom Left Of The Texture and Quad
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	// Top Right Of The Texture and Quad
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	// Top Left Of The Texture and Quad
	glEnd();

	/* Draw it to the screen */
	SDL_GL_SwapBuffers( );

	GLint t = SDL_GetTicks();
	
	if((bUseMask || bRemoveMask) && t - T1mask >= 500)
	{
		SDL_SysWMinfo info;
		SDL_VERSION(&info.version); 
		if(SDL_GetWMInfo(&info))
		{
			if(bUseMask)
			{
				XRectangle rects[2] = 
				{
					{0, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2},
					{SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT}
				};

				XShapeCombineRectangles (
					info.info.x11.display,
					info.info.x11.wmwindow,
					ShapeBounding,
					0, 0,
					rects,
					2, ShapeSet, Unsorted);

				printf("Applied mask\n");
			}
			else
			{
				//remove mask
				XRectangle rects[1] = 
				{
					{0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}
				};

				XShapeCombineRectangles (
					info.info.x11.display,
					info.info.x11.wmwindow,
					ShapeBounding,
					0, 0,
					rects,
					1, ShapeSet, Unsorted);

				printf("Removed mask\n");
				bRemoveMask = false;
			}

			T1mask = t;
		}
		else
		{
			Quit(1);
		}
	}

	Frames++;

	/* Gather our frames per second */
	if (t - T0 >= 5000) 
	{
		GLfloat seconds = (t - T0) / 1000.0;
		GLfloat fps = Frames / seconds;
		//todo
		printf("%d frames in %g seconds = %g FPS\n", Frames, seconds, fps);
		T0 = t;
		Frames = 0;
	}

//#ifndef WIN32
//	usleep(40000);
//#endif
	xrot += 0.3f; /* X Axis Rotation */
	yrot += 0.2f; /* Y Axis Rotation */
	zrot += 0.4f; /* Z Axis Rotation */

	return( TRUE );
}

char *ReadFileIntoBuffer( string sFileName, size_t &Size )
{
    Size=0;
    if( sFileName.length()==0 )
        return NULL;

    FILE *pFile = fopen( sFileName.c_str(), "rb" );
    if ( pFile == NULL )
        return NULL;

    fseek( pFile, 0L, SEEK_END );
    Size = ftell( pFile );
    fseek( pFile, 0L, SEEK_SET );

    char *pChr = new char[Size+1];
    fread( pChr, Size, 1, pFile );
    pChr[Size] = 0;
    fclose( pFile );
    return pChr;
}

bool ReadTextFile(string sFileName, string& sData)
{
	size_t nSize = 0;
	char *pData = ReadFileIntoBuffer(sFileName, nSize);
	bool bResult = NULL != pData;

	if(pData)
		sData = string(pData);

	delete [] pData;
	pData = NULL;
	return bResult;
}

int client_msg(Display *disp, Window win, char *msg,
                       unsigned long data0, unsigned long data1,
                       unsigned long data2, unsigned long data3,
                       unsigned long data4)
{
    XEvent event;
    long mask = SubstructureRedirectMask | SubstructureNotifyMask;

    event.xclient.type = ClientMessage;
    event.xclient.serial = 0;
    event.xclient.send_event = True;
    event.xclient.message_type = XInternAtom(disp, msg, False);
    event.xclient.window = win;
    event.xclient.format = 32;
    event.xclient.data.l[0] = data0;
    event.xclient.data.l[1] = data1;
    event.xclient.data.l[2] = data2;
    event.xclient.data.l[3] = data3;
    event.xclient.data.l[4] = data4;

    if (XSendEvent(disp, DefaultRootWindow(disp), False, mask, &event))
    {
		fprintf(stdout, "Success!\n");
        return EXIT_SUCCESS;
    }
    else
    {
        fprintf(stderr, "Cannot send %s event.\n", msg);
        return EXIT_FAILURE;
    }
}

#ifdef WIN32
int _tmain(int argc, char * /*_TCHAR*/ argv[])
#else
int main( int argc, char **argv )
#endif
{
	/* Flags to pass to SDL_SetVideoMode */
	int videoFlags;
	/* main loop variable */
	int done = FALSE;
	/* used to collect events */
	SDL_Event event;
	/* this holds some info about our display */
	const SDL_VideoInfo *videoInfo;

	sTextureName = "logo.png";

	printf("Usage : ./UIdiag [composite|mask|both]\n");
	printf("Keys:\n");
	printf("	c - toggle composite\n");
	printf("	m - toggle masking\n");
	printf("	F1 - toggle fullscreen/windowed\n");
	printf("	any other key - quit\n");

	if(argc >= 2)
	{
		string sArg(argv[1]);

		bUseComposite = sArg == "composite";
		bUseMask = sArg == "mask";

		if(sArg == "both")
			bUseComposite = bUseMask = true;
	}

	printf("Texture to use: %s\n", sTextureName.c_str());
	printf("Composite: %s\n", bUseComposite ? "enabled" : "disabled");
	printf("Mask: %s\n", bUseMask ? "enabled" : "disabled");

	pid_player = fork();
	if(pid_player == 0)
	{
		string sCommand = "cat /etc/mailcap | grep \"^video/mpeg;\"  | head -1 | cut -d';' -f2 > /tmp/player-name";
		system(sCommand.c_str());

		string sPlayerCommandTemplate;
		char pPlayerCommand[1024];

		string sSampleFile = "/home/public/data/samples/UIdiag_sample.mpg";

		struct stat buf;
		if(stat("sample.mpg", &buf) == 0)
			sSampleFile = "sample.mpg";

		printf("Using '%s' sample media file", sSampleFile.c_str());

		if (access("/usr/bin/xine", F_OK) == 0) 
		{	
			sPlayerCommandTemplate="xine -l -g -V xv '%s'";
			sprintf(pPlayerCommand, sPlayerCommandTemplate.c_str(), sSampleFile.c_str());
	
			printf("Running player application %s\n", pPlayerCommand); 
			system(pPlayerCommand);
		} 
		else if(ReadTextFile("/tmp/player-name", sPlayerCommandTemplate)) 
		{
			sprintf(pPlayerCommand, sPlayerCommandTemplate.c_str(), sSampleFile.c_str());
	
			printf("Running player application %s\n", pPlayerCommand); 
			system(pPlayerCommand);
		}
		else
		{
			printf("Error : cannot find the default player application!\n");
		}

		exit(0);
	}

	pid_xcompmgr = fork();
	if(pid_xcompmgr == 0)
	{
		printf("Launching xcompmgr...\n");
		execvp("xcompmgr", NULL);
		exit(0);
	} 

	SDL_WM_SetCaption("UI diagnostics", "UI diagnostics");

	while(true)
	{
		done = FALSE;

#ifndef WIN32
	if(bUseComposite)
	{
		Display *dpy = CompositeHelper::GetInstance().GetDisplay();

		if(NULL == dpy)
		{
			printf("Failed to open display!\n");
			return false;
		}

		XSynchronize(dpy, 1);
		XVisualInfo *visinfo = CompositeHelper::GetInstance().GetVisualForComposite();

		if(NULL != visinfo && CompositeHelper::GetInstance().RegisterReplacementWindowForSDL(visinfo, SCREEN_WIDTH, SCREEN_HEIGHT))
			printf("Created replacement window for SDL!\n");
		
		XSynchronize(dpy, 1); 
	}
#endif

		/* initialize SDL */
		if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE ) < 0 )
		{
			fprintf( stderr, "Video initialization failed: %s\n",
				SDL_GetError( ) );
			Quit(1);
		}

		/* Fetch the video info */
		videoInfo = SDL_GetVideoInfo( );

		if ( !videoInfo )
		{
			fprintf( stderr, "Video query failed: %s\n", SDL_GetError( ) );
			Quit(1);
		}

		/* the flags to pass to SDL_SetVideoMode */
		videoFlags = SDL_OPENGL;          /* Enable OpenGL in SDL */
		videoFlags |= SDL_GL_DOUBLEBUFFER; /* Enable double buffering */
		videoFlags |= SDL_HWPALETTE;       /* Store the palette in hardware */

		/* This checks to see if surfaces can be stored in memory */
		if ( videoInfo->hw_available )
			videoFlags |= SDL_HWSURFACE;
		else
			videoFlags |= SDL_SWSURFACE;

		/* This checks if hardware blits can be done */
		if ( videoInfo->blit_hw )
			videoFlags |= SDL_HWACCEL;

		/* Sets up OpenGL double buffering */
		SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

		/* get a SDL surface */
		surface = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, videoFlags );

		/* Verify there is a surface */
		if ( !surface )
		{
			fprintf( stderr,  "Video mode set failed: %s\n", SDL_GetError( ) );
			Quit(1);
		}

		/* make it topmost */
		SDL_SysWMinfo info;
		SDL_VERSION(&info.version);
		if(SDL_GetWMInfo(&info))
		{
			fprintf(stdout, "About to make window above\n");

			char *p1 = "above";
	        gchar *tmp_prop1, *tmp1;
			tmp_prop1 = g_strdup_printf("_NET_WM_STATE_%s", tmp1 = g_ascii_strup(p1, -1));
			Atom prop1 = XInternAtom(info.info.x11.display, tmp_prop1, False);
			g_free(tmp1);
			g_free(tmp_prop1);

			client_msg(info.info.x11.display, info.info.x11.wmwindow, "_NET_WM_STATE", _NET_WM_STATE_ADD, (unsigned long)prop1, 0, 0, 0);
		}

		//are we using the right visual?
		XWindowAttributes a;
		XGetWindowAttributes(info.info.x11.display, info.info.x11.wmwindow, &a);
		printf("SDL's window visual: 0x%x and depth %d \n", XVisualIDFromVisual(a.visual), a.depth);

		/* initialize OpenGL */
		initGL( );

		/* resize the initial window */
		resizeWindow( SCREEN_WIDTH, SCREEN_HEIGHT );

		if(bFullScreen)
			SDL_WM_ToggleFullScreen( surface );

		/* wait for events */
		while ( !done )
		{
			/* handle the events in the queue */

			while ( !done && SDL_PollEvent( &event ) )
			{
				switch( event.type )
				{
				case SDL_KEYDOWN:
					/* handle key presses */
					printf("got a key %d!\n", event.key.keysym.sym);
					if(handleKeyPress( &event.key.keysym))
						done = TRUE;

					break;
				case SDL_QUIT:
					/* handle quit requests */
					{
						printf("about to quit!\n");
						Quit(0);
						return 0;
					}
				default:
					break;
				}
			}

			drawGLScene( );
		}

		/* clean up the window */
		SDL_Quit( );
	}

	/* Should never get here */
	return(0);
} 

