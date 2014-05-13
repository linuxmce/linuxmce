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
 OrbiterRenderer_SDL

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com

 Phone: +1 (877) 758-8648

 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#include "OrbiterRenderer_SDL.h"

#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "DCE/Logger.h"
#include "SerializeClass/ShapesColors.h"

#include "../../SDL_Helpers/SDL_Defs.h"
#include "../../SDL_Helpers/SDL_pixel.h"

#include "../RendererOCG.h"
#include "../ScreenHistory.h"
#include "../Orbiter.h"

#include "SDLRendererOCGHelper.h"
#include "pluto_main/Define_Effect.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_Text.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "DataGrid.h"
#include "SDLGraphic.h"
#include "Splitter/TextWrapper.h"

#ifdef WIN32
#include "../Win32/MainDialog.h"
#endif

#include "SDL_rotozoom.h"
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <sge.h>
#include <sge_surface.h>

#ifdef WIN32
	#include <SDL_syswm.h>
#else
	#include <SDL/SDL_syswm.h>
#endif

#if !defined(DIRECTFB) && !defined(WIN32) && !defined(BLUETOOTH_DONGLE) && !defined(PROXY_ORBITER) && !defined(MOXI_ORBITER)
	#include "utilities/linux/transparency/transparency.h"
	#include "utilities/linux/wrapper/wrapper_x11.h"
#endif

#if !defined(BLUETOOTH_DONGLE) && !defined(PROXY_ORBITER) && !defined(MOXI_ORBITER)
#define USE_ONLY_SCREEN_SURFACE
#endif

#ifdef WINCE
    #include "wince.h"
#endif

using namespace DCE;

bool g_bResettingVideoMode;
void *SetVideoModeWatchDogThread(void *p)
{
    //OrbiterRenderer_SDL *pOrbiterRenderer_SDL = (OrbiterRenderer_SDL *) p;
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"Inside WatchDogThread");
    Sleep(2000);
    if( g_bResettingVideoMode )
    {
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "SDL_SetVideoMode hangs on init. Xorg is not initialized yet? Restarting...");
#ifndef WIN32
        kill(getpid(), SIGKILL);
#endif
    }
    return NULL;
}
//-----------------------------------------------------------------------------------------------------
OrbiterRenderer_SDL::OrbiterRenderer_SDL(Orbiter *pOrbiter) : OrbiterRenderer(pOrbiter)
{
	m_bRelativeMode = false;
	m_pScreenImage = NULL;
}

OrbiterRenderer_SDL::~OrbiterRenderer_SDL()
{
	// We need to do this before freeing the surface.  It's a repeat of what's in Orbiter's destructor
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "about to free surface");

	#ifndef USE_ONLY_SCREEN_SURFACE
		SDL_FreeSurface(m_pScreenImage);
	#endif

	m_pScreenImage = NULL;
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "~OrbiterRenderer_SDL finished");
}

void OrbiterRenderer_SDL::Configure()
{
	/// SDL 2D code, creates the SDL window
	//initializing the engine...
	Uint32 uSDLInitFlags = SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE;

	if(m_bFullScreen)
		uSDLInitFlags |= SDL_FULLSCREEN;

    // Switch Nokia into fullscreen mode
    #if defined(MAEMO_NOKIA770) && !defined(DIRECTFB)
		uSDLInitFlags |= SDL_FULLSCREEN;
	   	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Added SDL_FULLSCREEN flag to the uSDLInitFlags");
	#endif
	
	if (SDL_Init(uSDLInitFlags) == -1)
	{
	#ifndef WINCE
			cerr << "Failed initializing SDL: " << SDL_GetError() << endl;
	#else
			printf("Failed to initialize SDL %s\n", SDL_GetError());
	#endif //WINCE

	#if !defined(WIN32) && !defined(DIRECTFB) //linux
            string sCmd = "/usr/pluto/bin/Start_X.sh";//; /usr/pluto/bin/Start_WM.sh";
            LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "X is not running! Starting X and the window manager: %s", sCmd.c_str());
            system(sCmd.c_str());
	#endif //linux
	
		exit(1);
	}

	if (TTF_Init() == -1)
	{
	#ifndef WINCE
		cout << "Failed to init SDL TTF: " << TTF_GetError() << "\nText won't be rendered" << endl;
	#endif
	}

#ifdef MAEMO_NOKIA770
    SDL_WM_SetCaption("PlutoNokia Orbiter", "OrbiterRenderer_SDL");
#else
    SDL_WM_SetCaption("OrbiterRenderer_SDL", "OrbiterRenderer_SDL");
#endif

	atexit(SDL_Quit);
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Initialized SDL");

#ifdef USE_ONLY_SCREEN_SURFACE
	Uint32 uVideoModeFlags = SDL_SWSURFACE | SDL_RESIZABLE;

#if !defined(WIN32) || defined(WINCE)
	if(m_bFullScreen)
		uVideoModeFlags |= SDL_FULLSCREEN;
#endif


#if defined(MAEMO_NOKIA770) && !defined(DIRECTFB)
    uVideoModeFlags |= SDL_FULLSCREEN;
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Added SDL_FULLSCREEN flag to the uVideoModeFlags");
#endif

	g_bResettingVideoMode=true;
	pthread_t hackthread;
	pthread_create(&hackthread, NULL, SetVideoModeWatchDogThread, (void*)this);

    Screen = SDL_SetVideoMode(OrbiterLogic()->m_iImageWidth, OrbiterLogic()->m_iImageHeight, 0, uVideoModeFlags);
    if (Screen == NULL)
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Failed to set video mode (%d x %d): %s", OrbiterLogic()->m_iImageWidth, OrbiterLogic()->m_iImageHeight, SDL_GetError());
		exit(1);
	}

#if !defined(WIN32) && defined(KDE_LMCE)
    SDL_SysWMinfo info;

    SDL_VERSION(&info.version);
    if ( SDL_GetWMInfo(&info) )
    {
            SDL_bool set;
            Atom WM_HINTS;

            /* We haven't modified the window manager hints yet */
            set = SDL_FALSE;

            Window WMwindow = info.info.x11.wmwindow;

            /* First try to set MWM hints */
            WM_HINTS = XInternAtom(info.info.x11.display, "_MOTIF_WM_HINTS", True);
            if ( WM_HINTS != None ) {
                    /* Hints used by Motif compliant window managers */
                    struct {
                            unsigned long flags;
                            unsigned long functions;
                            unsigned long decorations;
                            long input_mode;
                            unsigned long status;
                    } MWMHints = { (1L << 1), 0, 0, 0, 0 };

                    XChangeProperty(info.info.x11.display, WMwindow,
                                    WM_HINTS, WM_HINTS, 32,
                                    PropModeReplace,
                                    (unsigned char *)&MWMHints,
                                    sizeof(MWMHints)/sizeof(long));
                    set = SDL_TRUE;
            }
            /* Now try to set KWM hints */
            WM_HINTS = XInternAtom(info.info.x11.display, "KWM_WIN_DECORATION", True);
            if ( WM_HINTS != None ) {
                    long KWMHints = 0;

                    XChangeProperty(info.info.x11.display, WMwindow,
                                    WM_HINTS, WM_HINTS, 32,
                                    PropModeReplace,
                                    (unsigned char *)&KWMHints,
                                    sizeof(KWMHints)/sizeof(long));
                    set = SDL_TRUE;
            }
            /* Now try to set GNOME hints */
            WM_HINTS = XInternAtom(info.info.x11.display, "_WIN_HINTS", True);
            if ( WM_HINTS != None ) {
                    long GNOMEHints = 0;

                    XChangeProperty(info.info.x11.display, WMwindow,
                                    WM_HINTS, WM_HINTS, 32,
                                    PropModeReplace,
                                    (unsigned char *)&GNOMEHints,
                                    sizeof(GNOMEHints)/sizeof(long));
                    set = SDL_TRUE;
            }
	}
#endif 

	g_bResettingVideoMode=false;
    InitializeAfterSetVideoMode();
	SetupWindow();
#endif

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Set video mode to %d x %d Window.", OrbiterLogic()->m_iImageWidth, OrbiterLogic()->m_iImageHeight);

#ifdef USE_ONLY_SCREEN_SURFACE
	m_pScreenImage = Screen;
#else
    m_pScreenImage = SDL_CreateRGBSurface(SDL_SWSURFACE, OrbiterLogic()->m_iImageWidth, OrbiterLogic()->m_iImageHeight, BIT_PER_PIXEL, rmask, gmask, bmask, amask);
	if (m_pScreenImage == NULL)
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "SDL_CreateRGBSurface failed! %s",SDL_GetError());
	}
#endif
	OrbiterLogic()->m_bWeCanRepeat = true;

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Created back screen surface!");

    // we will use the per-pixel transparency
	//SetOrbiterWindowTransparency(0.5);

	if (FileUtils::FileExists("/etc/pluto/orbiter.disable.mouse"))
		SDL_ShowCursor(0);
}
//----------------------------------------------------------------------------------------------------
void OrbiterRenderer_SDL::SetOrbiterWindowTransparency(double TransparencyLevel)
{
    // FIXME: we shold go until we find the root window
    // FIXME: use GetMainWindow(), to avoid duplicate code
#if !defined(DIRECTFB) && !defined(WIN32) && !defined(BLUETOOTH_DONGLE) && !defined(PROXY_ORBITER) && !defined(MOXI_ORBITER)
	X11_Locker_NewDisplay locker_NewDisplay;
	SDL_SysWMinfo info;
	SDL_VERSION(&info.version); // this is important!
	bool bResult = SDL_GetWMInfo(&info);
	if (bResult == false)
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "OrbiterRenderer_SDL(): error in SDL_GetWMInfo()");
	}
	Window parent = GetParentWnd( info.info.x11.display, info.info.x11.window );
	Window grandparent = GetParentWnd( info.info.x11.display, parent );
	Window ggrandparent = GetParentWnd( info.info.x11.display, grandparent );

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterRenderer_SDL(): setting transparency");
	SetWindowTransparency(locker_NewDisplay.GetDisplay(), info.info.x11.window, TransparencyLevel);
	SetWindowTransparency(locker_NewDisplay.GetDisplay(), parent, TransparencyLevel);
	SetWindowTransparency(locker_NewDisplay.GetDisplay(), grandparent, TransparencyLevel);
	SetWindowTransparency(locker_NewDisplay.GetDisplay(), ggrandparent, TransparencyLevel);
#endif
}
//----------------------------------------------------------------------------------------------------
void OrbiterRenderer_SDL::RenderScreen( bool bRenderGraphicsOnly )
{
	#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"$$$ RENDER SCREEN $$$ %s",
			(OrbiterLogic()->m_pScreenHistory_Current ? OrbiterLogic()->m_pScreenHistory_Current->GetObj()->m_ObjectID.c_str() : " NO SCREEN"));
	#endif
	
	if (OrbiterLogic()->m_pScreenHistory_Current)
	{
		PLUTO_SAFETY_LOCK(cm, OrbiterLogic()->m_ScreenMutex);
        LockDisplay();
        SDL_FillRect(m_pScreenImage, NULL, SDL_MapRGBA(m_pScreenImage->format, 0, 0, 0, 255));
        UnlockDisplay();
    }

	OrbiterRenderer::RenderScreen(bRenderGraphicsOnly);
	DisplayImageOnScreen(m_pScreenImage);
}
//----------------------------------------------------------------------------------------------------
void OrbiterRenderer_SDL::DisplayImageOnScreen(SDL_Surface *m_pScreenImage)
{
	if(OrbiterLogic()->m_bQuit_get())
		return;

	PLUTO_SAFETY_LOCK(cm,OrbiterLogic()->m_ScreenMutex);
    LockDisplay();

#ifndef USE_ONLY_SCREEN_SURFACE
    SDL_BlitSurface(m_pScreenImage, NULL, Screen, NULL);
#endif

	SDL_UpdateRect(Screen, 0, 0, 0, 0);
    UnlockDisplay();
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_SDL::RenderText(string &TextToDisplay,DesignObjText *Text,TextStyle *pTextStyle, 
									 PlutoPoint point, string ObjectID)
{
    PLUTO_SAFETY_LOCK(cm,OrbiterLogic()->m_ScreenMutex);

	SDL_Rect TextLocation;
	TextLocation.x = point.X + Text->m_rPosition.X;
	TextLocation.y = point.Y + Text->m_rPosition.Y;
	TextLocation.w = Text->m_rPosition.Width;
	TextLocation.h = Text->m_rPosition.Height;

#ifdef WIN32
#ifdef WINCE
	string BasePath = "C:\\Windows\\Fonts\\";
#else
	char pWindowsDirector[MAX_PATH];
	GetWindowsDirectory(pWindowsDirector, MAX_PATH);
	string BasePath = string(pWindowsDirector) + "\\Fonts\\";
#endif

#else
	string BasePath="/usr/share/fonts/truetype/msttcorefonts/";
#endif //win32

	WrapAndRenderText(m_pScreenImage, TextToDisplay, TextLocation.x, TextLocation.y, TextLocation.w, TextLocation.h, BasePath,
		pTextStyle,Text->m_iPK_HorizAlignment,Text->m_iPK_VertAlignment, &OrbiterLogic()->m_mapTextStyle);

}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_SDL::HollowRectangle(int X, int Y, int Width, int Height, PlutoColor color, string ParentObjectID/* = ""*/, string ObjectID/* = ""*/)
{
	ClipRectangle(X, Y, Width, Height);

#ifdef DEBUG
	
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "SDL::HollowRectangle: color:%u %u %u -> 0x%x",
													color.R(), color.G(), color.B(), color.m_Value);
	
#endif
	
	sge_Rect(m_pScreenImage,X,Y,Width + X,Height + Y,color.m_Value);
}

//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_SDL::SolidRectangle(int x, int y, int width, int height, PlutoColor color, 
										 string ParentObjectID/* = ""*/,
										 string ObjectID/* = ""*/)
{
	ClipRectangle(x, y, width, height);

	SDL_Rect Rectangle;
	Rectangle.x = x; Rectangle.y = y; Rectangle.w = width; Rectangle.h = height;

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "SDL::SolidRectangle: color=%u %u %u; x=%u, y=%u; w=%u; h=%u\n",
																color.R(), color.G(), color.B(), x, y, width, height);
#endif

	if(color.A() < 255)
	{
		SDL_Rect rectBar;
		rectBar.x = 0; rectBar.y = 0; rectBar.w = width; rectBar.h = height;
		SDL_Surface *pSDL_Bar = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA, rectBar.w, rectBar.h, BIT_PER_PIXEL, rmask, gmask, bmask, amask);
		SDL_FillRect(pSDL_Bar, &rectBar, SDL_MapRGBA(pSDL_Bar->format, color.R(), color.G(), color.B(), color.A()));

		LockDisplay();
		SDL_BlitSurface(pSDL_Bar, NULL, m_pScreenImage, &Rectangle);
		UnlockDisplay();
		SDL_FreeSurface(pSDL_Bar);	
	}
	else
	{
		LockDisplay();
		SDL_FillRect(m_pScreenImage, &Rectangle, SDL_MapRGBA(m_pScreenImage->format, color.R(), color.G(), color.B(), color.A()));
		UnlockDisplay();
	}
}

//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_SDL::RenderGraphic(class PlutoGraphic *pPlutoGraphic, 
	PlutoRectangle rectTotal, bool bDisableAspectRatio, PlutoPoint point/* = PlutoPoint(0, 0)*/,
	int nAlphaChannel/* = 255*/, string ParentObjectID/* = ""*/, string ObjectID/* = ""*/,
	string ObjectHash/* = ""*/)
{

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterRenderer_SDL::RenderGraphic ...\n");
#endif
	
	if(!pPlutoGraphic || pPlutoGraphic->GraphicType_get() != gtSDLGraphic)
		return;//nothing to render or not an sdl graphic

	if(pPlutoGraphic->IsEmpty())
		return;

    SDLGraphic *pSDLGraphic = (SDLGraphic *) pPlutoGraphic;
	SDL_Surface *pSDL_Surface = pSDLGraphic->m_pSDL_Surface;

	//render the sdl surface
	SDL_Rect Destination;
	Destination.x = point.X + rectTotal.X;
	Destination.y = point.Y + rectTotal.Y;
	Destination.w = rectTotal.Width;
	Destination.h = rectTotal.Height;

	if(pSDL_Surface->w == 0 || pSDL_Surface->h == 0) //nothing to render
		return; //malformated image?
	else
		if(pSDL_Surface->w != rectTotal.Width || pSDL_Surface->h != rectTotal.Height) //different size. we'll have to stretch the surface
		{
			double ZoomX = 1;
			double ZoomY = 1;

			SDL_Surface *rotozoom_picture;

			if(bDisableAspectRatio) //no aspect ratio kept
			{
				ZoomX = rectTotal.Width / double(pSDL_Surface->w);
				ZoomY = rectTotal.Height / double(pSDL_Surface->h);
			}
			else //we'll have to keep the aspect
			{
				ZoomX = ZoomY = min(rectTotal.Width / double(pSDL_Surface->w),
					rectTotal.Height / double(pSDL_Surface->h));
			}

			rotozoom_picture = zoomSurface(pSDL_Surface, ZoomX, ZoomY, SMOOTHING_ON);

            LockDisplay();
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterRenderer_SDL::RenderGraphic - Different size, stretching surface ...\n");
#endif
		
			SDL_BlitSurface(rotozoom_picture, NULL, m_pScreenImage, &Destination);
			SDL_FreeSurface(rotozoom_picture);
            UnlockDisplay();
		}
		else //same size ... just blit the surface
        {
            LockDisplay();
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "OrbiterRenderer_SDL::RenderGraphic - Blit Surface ...\n");
#endif

            SDL_BlitSurface(pSDL_Surface, NULL, m_pScreenImage, &Destination);
            UnlockDisplay();
        }
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_SDL::SaveBackgroundForDeselect(DesignObj_Orbiter *pObj, PlutoPoint point)
{
	SDL_Rect SourceRect;
	SourceRect.x = point.X + pObj->m_rPosition.Left(); SourceRect.y = point.Y + pObj->m_rPosition.Top();
	SourceRect.w = pObj->m_rPosition.Width; SourceRect.h = pObj->m_rPosition.Height;
	
    SDL_Surface *pSDL_Surface = SDL_CreateRGBSurface(SDL_SWSURFACE,
            pObj->m_rPosition.Width, pObj->m_rPosition.Height, BIT_PER_PIXEL, rmask, gmask, bmask, amask);
	
    LockDisplay();
	/*
	 * Damned if you do, damned if you don't.
	 * If it's a proxy orbiter, we want to do what GetBackground (below) does
	 * But if it's an OSD, we want the original behaviour.
	 * Why: with the original form of this function, the OSD is fine,
	 *      but the Proxy Orbiter ends up displaying black regions
	 *      with the new form of this function, the Proxy Orbiter is fine,
	 *      but the OSD renders a black frame that starts really faint and
	 *      fades in as you press a button more and more.
	 * I can't for the life of me understand where the OSD and Proxy Orbiter
	 * diverge, so here's to the good old Pluto coding spirit. Hear hear!
	 */
#ifdef PROXY_ORBITER
	/* New */
	SDL_SetAlpha(m_pScreenImage, 0, 0);
#endif
	SDL_BlitSurface(m_pScreenImage, &SourceRect, pSDL_Surface, NULL);
#ifndef PROXY_ORBITER
	/* Original */
	SDL_SetAlpha(pSDL_Surface, SDL_RLEACCEL , SDL_ALPHA_OPAQUE);
#endif
    UnlockDisplay();
    
	pObj->m_pGraphicToUndoSelect = new SDLGraphic(pSDL_Surface);
}
//-----------------------------------------------------------------------------------------------------
PlutoGraphic *OrbiterRenderer_SDL::GetBackground( PlutoRectangle &rect )
{
	ClipRectangle(rect);

    SDL_Surface *pSDL_Surface = SDL_CreateRGBSurface(SDL_SWSURFACE,
		rect.Width, rect.Height, BIT_PER_PIXEL, rmask, gmask, bmask, amask);

	SDL_Rect SourceRect;
	SourceRect.x = rect.Left(); SourceRect.y = rect.Top();
	SourceRect.w = rect.Width; SourceRect.h = rect.Height;

    LockDisplay();
	SDL_SetAlpha(m_pScreenImage, 0, 0);
	SDL_BlitSurface(m_pScreenImage, &SourceRect, pSDL_Surface, NULL);
    UnlockDisplay();
    
	return new SDLGraphic(pSDL_Surface);
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_SDL::ReplaceColorInRectangle(int x, int y, int width, int height, PlutoColor ColorToReplace, PlutoColor ReplacementColor, DesignObj_Orbiter *pObj/* = NULL*/)
{
    SDL_PixelFormat * PF = m_pScreenImage->format;
    Uint32 PlutoPixelSrc, Pixel;
#ifdef MAEMO_NOKIA770
	Uint16 PlutoPixelDest;
#else
	Uint32 PlutoPixelDest;
#endif
    Uint8 red, green, blue; 

#ifdef DEBUG
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "ReplaceColor: ColorToReplace --> %u %u %u : ReplacementColor --> %u %u %u",
        ColorToReplace.R(), ColorToReplace.G(), ColorToReplace.B(),
        ReplacementColor.R(), ReplacementColor.G(), ReplacementColor.B());
#endif

#ifdef MAEMO_NOKIA770
    PlutoPixelSrc = (ColorToReplace.R() << rshift) | (ColorToReplace.G() << gshift) | (ColorToReplace.B() << bshift); // | (ColorToReplace.A() << PF->Ashift);
#else
    PlutoPixelSrc = (ColorToReplace.R() << PF->Rshift) | (ColorToReplace.G() << PF->Gshift) | (ColorToReplace.B() << PF->Bshift); // | (ColorToReplace.A() << PF->Ashift);
#endif

    unsigned char *Source = (unsigned char *) &PlutoPixelSrc;
	
#ifdef MAEMO_NOKIA770

	PlutoPixelDest = ((ReplacementColor.R() >> PF->Rloss) << PF->Rshift) +
						((ReplacementColor.G() >> PF->Gloss) << PF->Gshift) +
						((ReplacementColor.B() >> PF->Bloss) << PF->Bshift);
#else
    PlutoPixelDest = (ReplacementColor.R() << PF->Rshift) | (ReplacementColor.G() << PF->Gshift) | (ReplacementColor.B() << PF->Bshift);//  TODO -- this should work | ReplacementColor.A() << PF->Ashift;
#endif

    const int max_diff = 3;

    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            // we may need locking on the surface
            Pixel = SDLGraphic::getpixel(m_pScreenImage, i + x, j + y);
            unsigned char *pPixel = (unsigned char *) &Pixel;
            SDL_GetRGB((Uint32)Pixel, m_pScreenImage->format, &red, &green, &blue);
            if (abs(Source[0]-red)<max_diff && abs(Source[1]-green)<max_diff && abs(Source[2]-blue)<max_diff /*&& abs(Source[3]-pPixel[3])<max_diff*/)
            {
                SDLGraphic::putpixel(m_pScreenImage,i + x, j + y, PlutoPixelDest);
            }
        }
    }


    PlutoRectangle rect(x, y, width, height);
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_SDL::BeginPaint()
{
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_SDL::EndPaint()
{
    //if we are using a buffer surface to blit images and text (Bluetooth_Dongle uses this)
    //will have to update the hole screen
    //if not, the user will call UpdateRect function for each rectangle he must invalidate
#ifndef USE_ONLY_SCREEN_SURFACE
    DisplayImageOnScreen(m_pScreenImage);
#endif
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_SDL::OnQuit()
{
    LoggerWrapper::GetInstance()->Write(LV_WARNING,"Got an on quit.  Pushing an event into SDL");
    SDL_Event *pEvent = new SDL_Event;
    pEvent->type = SDL_QUIT;
    SDL_PushEvent(pEvent);
    delete pEvent;
}
//-----------------------------------------------------------------------------------------------------
PlutoGraphic *OrbiterRenderer_SDL::CreateGraphic()
{
    return new SDLGraphic(this);
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_SDL::UpdateRect(PlutoRectangle rect, PlutoPoint point)
{
    //clipping the rectangle
    PlutoRectangle localrect = rect;
    localrect.X += point.X;
    localrect.Y += point.Y;

	ClipRectangle(localrect);

#ifdef USE_ONLY_SCREEN_SURFACE
	PLUTO_SAFETY_LOCK(cm,OrbiterLogic()->m_ScreenMutex);

    LockDisplay();
    SDL_UpdateRect(Screen, localrect.Left(), localrect.Top(), localrect.Width, localrect.Height);
    UnlockDisplay();
#endif
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_SDL::OnIdle()
{
    Sleep(5);
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_SDL::EventLoop()
{
	int SDL_Event_Pending = 0;

	SDL_Event Event;

   // For now I'll assume that shift + arrows scrolls a grid
    while (!OrbiterLogic()->m_bQuit_get()&& !OrbiterLogic()->m_bReload)
    {
		LockDisplay();
		SDL_Event_Pending = SDL_PollEvent(&Event);
		UnlockDisplay();

		if (SDL_Event_Pending)
		{
			Orbiter::Event orbiterEvent;
			orbiterEvent.type = Orbiter::Event::NOT_PROCESSED;

			if (Event.type == SDL_QUIT)
			{
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "Received sdl event SDL_QUIT");
				break;
			} 
			else if(Event.type == SDL_MOUSEMOTION)
			{
				if (m_bRelativeMode)
				{
					orbiterEvent.type = Orbiter::Event::MOUSE_RELATIVE_MOVE;
					orbiterEvent.data.region.m_iX = Event.motion.xrel;
					orbiterEvent.data.region.m_iY = Event.motion.yrel;
				}
				else
				{
					orbiterEvent.type = Orbiter::Event::MOUSE_MOVE;
					orbiterEvent.data.region.m_iX = Event.button.x;
					orbiterEvent.data.region.m_iY = Event.button.y;
				}
				orbiterEvent.data.region.m_iButton = Event.button.button;
				OrbiterLogic()->ProcessEvent(orbiterEvent);
			} 
			else if (Event.type == SDL_MOUSEBUTTONDOWN)
			{
				orbiterEvent.type = Orbiter::Event::REGION_DOWN;
				orbiterEvent.data.region.m_iX = Event.button.x;
				orbiterEvent.data.region.m_iY = Event.button.y;
				orbiterEvent.data.region.m_iButton = Event.button.button;
				OrbiterLogic()->ProcessEvent(orbiterEvent);

#if defined(WIN32) && !defined(DIRECTFB) && !defined(PROXY_ORBITER) && !defined(BLUETOOTH_DONGLE) && !defined(MOXI_ORBITER)
				RecordMouseAction(Event.button.x, Event.button.y);
#endif
			}
			else if (Event.type == SDL_MOUSEBUTTONUP)
			{
				orbiterEvent.type = Orbiter::Event::REGION_UP;
				orbiterEvent.data.region.m_iX = Event.button.x;
				orbiterEvent.data.region.m_iY = Event.button.y;
				orbiterEvent.data.region.m_iButton = Event.button.button;
				OrbiterLogic()->ProcessEvent(orbiterEvent);
			}
		}
		else
		{
			OnIdle();
		}
    }  // while
}

//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_SDL::CaptureRelativeMovements()
{
    // ERROR:
    //
    // this function does not return
    // it will causes the mouse to vanish (only if SDL_WM_GrabInput returns)
    // if SDL_WM_GrabInput returns, then usually SDL_ShowCursor does not return
    // the mouse will not appear again because this thread will be
    // blocked, so no one will release the mouse-movements
    //
    // Other notes:
    // SDL is not fair here
    // it opens a new display and probably new window
    // it also installs his error handler, overriding our handler
    // in this case X11 will use that bad handler
    // and will not stop the app

    if (0)
    {
        LoggerWrapper::GetInstance()->Write(LV_STATUS,"OrbiterRenderer_SDL::CaptureRelativeMovements()");
        m_bRelativeMode = true;

//#ifndef WIN32 // linux
//    // sample code
//    if (OrbiterLogic()->IsYieldScreen())
//    {
//        GrabPointer(false);
//        GrabKeyboard(false);
//    }
//    // sample code
//#endif
        // warning: incompatible with constrain-mouse in linux
        // both keyboard and mouse
        SDL_WM_GrabInput(SDL_GRAB_ON);
        SDL_ShowCursor(SDL_DISABLE);
        LoggerWrapper::GetInstance()->Write(LV_STATUS,"OrbiterRenderer_SDL::CaptureRelativeMovements() : done");
    }
}

//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_SDL::ReleaseRelativeMovements()
{
	//TODO : do we need this anymore? it's deactivated
    // CaptureRelativeMovements() was deactivated above
    if (0)
    {
        LoggerWrapper::GetInstance()->Write(LV_STATUS,"OrbiterRenderer_SDL::ReleaseRelativeMovements()");
        m_bRelativeMode = false;

        SDL_WM_GrabInput(SDL_GRAB_OFF);
        SDL_ShowCursor(SDL_ENABLE);

        LoggerWrapper::GetInstance()->Write(LV_STATUS,"OrbiterRenderer_SDL::ReleaseRelativeMovements() : done");
    }
}

//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_SDL::DrawLine(int x1, int y1, int x2, int y2, PlutoColor color)
{
	int x = x1;
	int y = y1;

#ifdef MAEMO_NOKIA770
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "SDL::DrawLine - color: %u, %u, %u",
																color.R(), color.G(), color.B());
#endif

	
    if(
		x1 < 0 || x1 > OrbiterLogic()->m_iImageWidth - 1 || x2 < 0 || x2 > OrbiterLogic()->m_iImageWidth - 1 || 
		y1 < 0 || y1 > OrbiterLogic()->m_iImageHeight - 1 || y2 < 0 || y2 > OrbiterLogic()->m_iImageHeight - 1
	) 
		return;
    
    int deltax = abs(x2 - x1); 
    int deltay = abs(y2 - y1); 
    int xinc1, xinc2, yinc1, yinc2, den, num, numadd, numpixels, curpixel;

    if (x2 >= x1) 
    {
        xinc1 = 1;
        xinc2 = 1;
    }
    else 
    {
        xinc1 = -1;
        xinc2 = -1;
    }

    if (y2 >= y1) 
    {
        yinc1 = 1;
        yinc2 = 1;
    }
    else 
    {
        yinc1 = -1;
        yinc2 = -1;
    }

    if (deltax >= deltay) 
    {
        xinc1 = 0; 
        yinc2 = 0; 
        den = deltax;
        num = deltax / 2;
        numadd = deltay;
        numpixels = deltax; 
    }
    else 
    {
        xinc2 = 0; 
        yinc1 = 0; 
        den = deltay;
        num = deltay / 2;
        numadd = deltax;
        numpixels = deltay; 
    }

    for (curpixel = 0; curpixel <= numpixels; curpixel++)
    {
		putpixel(m_pScreenImage, x % OrbiterLogic()->m_iImageWidth, y % OrbiterLogic()->m_iImageHeight, color.m_Value);
        num += numadd; 
        
		if (num >= den) 
        {
            num -= den; 
            x += xinc1; 
            y += yinc1; 
        }

        x += xinc2; 
        y += yinc2; 
    }
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_SDL::DrawArrow(PlutoPoint p1, PlutoPoint p2, PlutoSize sizeArrow, PlutoColor color,
	string ParentObjectID, string ObjectID) 
{
	OrbiterRenderer::DrawArrow(p1, p2, sizeArrow, color);

#ifdef MAEMO_NOKIA770
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "SDL::DrawArrow - color: %u, %u, %u",
																color.R(), color.G(), color.B());
#endif
	
	SDL_UpdateRect(Screen, 0, 0, 0, 0);
}
//-----------------------------------------------------------------------------------------------------

