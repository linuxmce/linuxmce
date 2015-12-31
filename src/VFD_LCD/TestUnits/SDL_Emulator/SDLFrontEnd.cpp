#include "SDLFrontEnd.h"
//--------------------------------------------------------------------------------------------------------
#include <SDL_image.h>
//--------------------------------------------------------------------------------------------------------
#define CURRENT_ITEM_POSITION_X 50
#define CURRENT_ITEM_POSITION_Y 150
#define ITEM_WIDTH	300
#define ITEM_HEIGHT 14
//--------------------------------------------------------------------------------------------------------
SDLFrontEnd::SDLFrontEnd(IInputProcessor *pInputProcessor, int w, int h) : 
	IRenderer(), IInputProvider(pInputProcessor),
	m_pSurface(NULL), m_pFont(NULL), m_nWidth(w), m_nHeight(h),
	m_ScreenMutex("screen rendering")
{
	m_ScreenMutex.Init(NULL);
}
//--------------------------------------------------------------------------------------------------------
SDLFrontEnd::~SDLFrontEnd()
{
	TTF_Quit();
	SDL_Quit();

	pthread_mutex_destroy(&m_ScreenMutex.mutex);
}
//--------------------------------------------------------------------------------------------------------
bool SDLFrontEnd::Init()
{
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE) < 0)
	{
		fprintf(stderr, "Video initialization failed: %s\n", SDL_GetError());
		return false;
	}	

	const SDL_VideoInfo *videoInfo = SDL_GetVideoInfo();

	if (!videoInfo)
	{
		fprintf(stderr, "Video query failed: %s\n", SDL_GetError());
		return false;
	}

	int videoFlags = SDL_HWPALETTE;

	if(videoInfo->hw_available)
		videoFlags |= SDL_HWSURFACE;
	else
		videoFlags |= SDL_SWSURFACE;

	if(videoInfo->blit_hw)
		videoFlags |= SDL_HWACCEL;

	m_pSurface = SDL_SetVideoMode(m_nWidth, m_nHeight, 32, videoFlags);

	if (!m_pSurface)
	{
		fprintf(stderr,  "Video mode set failed: %s\n", SDL_GetError());
		return false;
	}

	Clear();

	if(TTF_Init() != -1)
	{
#ifdef WIN32		
		m_pFont = TTF_OpenFont("/windows/fonts/arial.ttf", 12);
#else
		m_pFont = TTF_OpenFont("/usr/share/fonts/truetype/msttcorefonts/arial.ttf", 12);
#endif
	}

	return true;
}
//--------------------------------------------------------------------------------------------------------
bool SDLFrontEnd::EventLoop()
{
	SDL_Event event;

	while(true)
	{
		while (SDL_PollEvent(&event))
		{
			switch( event.type )
			{
				case SDL_KEYDOWN:
					{
						Input input;
						input.type = itKeyboardInput;
						input.keyboardinput = kiNone;

						switch(event.key.keysym.sym)
						{
							case SDLK_UP:
								input.keyboardinput = kiUp;	
								break;
							case SDLK_DOWN:
								input.keyboardinput = kiDown;
								break;
							case SDLK_LEFT:
								input.keyboardinput = kiLeft;
								break;
							case SDLK_RIGHT:
								input.keyboardinput = kiRight;
								break;
							default:
								break;
						}

						if(kiNone != input.keyboardinput)
							HandleInput(input);
					}
					break;

				case SDL_QUIT:
					printf("about to quit!\n");
					return true;
					break;

				default:
					break;
			}
		}
	}
}
//--------------------------------------------------------------------------------------------------------
void SDLFrontEnd::Render(const DisplayState &display_state)
{
	PLUTO_SAFETY_LOCK(sm, m_ScreenMutex);

	Clear();

	SDL_Color color = {255, 0, 0, 128};

	vector<string>::const_iterator it;

	//prior options
	int i = 1;
	for(it = display_state.m_vectPriorOptions.begin(); it != display_state.m_vectPriorOptions.end(); ++it, ++i)
		RenderText(CURRENT_ITEM_POSITION_X, CURRENT_ITEM_POSITION_Y - i * ITEM_HEIGHT, *it, color);

	//current option
	RenderText(CURRENT_ITEM_POSITION_X - 10, CURRENT_ITEM_POSITION_Y, "o", color);
	RenderText(CURRENT_ITEM_POSITION_X, CURRENT_ITEM_POSITION_Y, display_state.m_sHeader, color);

	//next options
	i = 1;
	for(it = display_state.m_vectNextOptions.begin(); it != display_state.m_vectNextOptions.end(); ++it, ++i)
		RenderText(CURRENT_ITEM_POSITION_X, CURRENT_ITEM_POSITION_Y + i * ITEM_HEIGHT, *it, color);

	//can go up/down/left/right
	if(display_state.m_bCanGoLeft)
		RenderText(0, (m_nHeight + ITEM_HEIGHT) / 2, "<", color);
	if(display_state.m_bCanGoRight)
		RenderText(m_nWidth - 10, (m_nHeight + ITEM_HEIGHT) / 2, ">", color);
	if(display_state.m_bCanGoUp)
		RenderText(m_nWidth / 2, 0, "^", color);
	if(display_state.m_bCanGoDown)
		RenderText(m_nWidth / 2, m_nHeight - 10, "V", color);


	SDL_Color blue = {0,0,255,128};

	//status
	RenderText(10, 30, display_state.m_sStatusMessage, blue);
	//progress
	RenderText(10, 40, "Progress : " + display_state.m_sProgress, blue);

	SDL_UpdateRect(m_pSurface, 0, 0, 0, 0);
}
//--------------------------------------------------------------------------------------------------------
void SDLFrontEnd::Clear()
{
	//background
	SDL_Rect Rectangle;
	Rectangle.x = 0; Rectangle.y = 0; Rectangle.w = m_nWidth; Rectangle.h = m_nHeight;
	SDL_FillRect(m_pSurface, &Rectangle, SDL_MapRGBA(m_pSurface->format, 0xe0, 0xff, 0xe0, 0xff));

	//options
	Rectangle.x = CURRENT_ITEM_POSITION_X;
	Rectangle.w = ITEM_WIDTH;
	Rectangle.h = ITEM_HEIGHT - 1;

	for(int i = - ADIACENT_OPTIONS_NUMBER; i <= ADIACENT_OPTIONS_NUMBER; i++)
	{
		Rectangle.y = CURRENT_ITEM_POSITION_Y + i * ITEM_HEIGHT + 2;
		SDL_FillRect(m_pSurface, &Rectangle, SDL_MapRGBA(m_pSurface->format, 0x40, 0xcf, 0xc0, 0xff));
	}
}
//--------------------------------------------------------------------------------------------------------
void SDLFrontEnd::RenderText(int x, int y, string sText, SDL_Color color)
{
	SDL_Surface *pTextSurface = TTF_RenderUTF8_Blended(m_pFont, sText.c_str(), color);

	if(NULL != pTextSurface)
	{
		SDL_Rect dest_rect;
		dest_rect.x = x;
		dest_rect.y = y;
		dest_rect.w = 0;
		dest_rect.h = 0;

		SDL_BlitSurface(pTextSurface, NULL, m_pSurface, &dest_rect);
		SDL_FreeSurface(pTextSurface);
	}
}
//--------------------------------------------------------------------------------------------------------

