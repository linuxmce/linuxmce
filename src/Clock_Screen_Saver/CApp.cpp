#include "CApp.h"
#include "Clock_Screen_Saver.h"

enum BKG_STYLE { BKG_CENTERED, BKG_TILED, BKG_HOME, BKG_FIT };
enum CLOCK_STYLE { HOUR12, HOUR24, HOURAMPM };

const CLOCK_STYLE	DEFAULT_CLOCK_STYLE = HOURAMPM;
const std::string	DEFAULT_FONT_FILE = "/usr/share/fonts/truetype/msttcorefonts/Verdana.ttf";
//const int		DEFAULT_FONT_SIZE = 180;
const SDL_Color		DEFAULT_FONT_COLOR = { 255, 255, 0, 255 };

const std::string	DEFAULT_BKG_FILE = "./background.png";
//const std::string	DEFAULT_BKG_FILE = "fd";
const BKG_STYLE		DEFAULT_BKG_STYLE = BKG_CENTERED;
const SDL_Color		DEFAULT_BKG_COLOR = { 0, 0, 255, 255 };

bool		CApp::Running = false;
bool		CApp::lmce_here = false;
SDL_Window	*CApp::window = nullptr;
SDL_Renderer	*CApp::renderer = nullptr;
SDL_Texture	*CApp::bkgTexture = nullptr;
SDL_Surface	*CApp::bkgSurface = nullptr;
SDL_Texture	*CApp::timeTexture = nullptr;
SDL_Surface	*CApp::timeSurface = nullptr;
TTF_Font	*CApp::timeFont = nullptr;
SDL_Color	*CApp::timeColor = nullptr;
time_t		CApp::timeCurrent;
int		CApp::windowWidth = 0;
int		CApp::windowHeight = 0;
DCE::Clock_Screen_Saver *CApp::CSSDevice = NULL;
std::string 	CApp::timeText = ":";
std::string 	CApp::timeLast = "";
bool		CApp::event = false;

SDL_Texture *CApp::renderTime(const std::string &message, SDL_Color color, SDL_Renderer *renderer)
{
	//We need to first render to a surface as that's what TTF_RenderText
	//returns, then load that surface into a texture
	SDL_Surface *surf = TTF_RenderText_Blended(timeFont, message.c_str(), color);
	if (surf == nullptr){
		error( std::cout, "=renderTime - TTF_RenderText |" );
		return nullptr;
	}

	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surf);
	if (texture == nullptr){
		error( std::cout, "=renderTime - CreateTexture |" );
	}

	//Clean up the surface and font
	SDL_FreeSurface(surf);

	return texture;
}

SDL_Texture *CApp::renderText(const std::string &message, const std::string &fontFile,
	SDL_Color color, int fontSize, SDL_Renderer *renderer)
{
	//Open the font
	TTF_Font *font = TTF_OpenFont(fontFile.c_str(), fontSize);
	if (font == nullptr){
		error( std::cout, "=renderText - TTF_OpenFont |" );
		return nullptr;
	}

	//We need to first render to a surface as that's what TTF_RenderText
	//returns, then load that surface into a texture
	SDL_Surface *surf = TTF_RenderText_Blended(font, message.c_str(), color);
	if (surf == nullptr){
		TTF_CloseFont(font);
		error( std::cout, "=renderText - TTF_RenderText |" );
		return nullptr;
	}

	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surf);
	if (texture == nullptr){
		error( std::cout, "=renderText - CreateTexture |" );
	}

	//Clean up the surface and font
	SDL_FreeSurface(surf);
	TTF_CloseFont(font);

	return texture;
}

void CApp::renderTexture(SDL_Texture *texture, SDL_Renderer *renderer, int x_pos, int y_pos, int width, int height){
	// Setup the destination rectangle to be at the position we want
	SDL_Rect dest;

	// Set the upper left/right pixel locations
	dest.x = x_pos;
	dest.y = y_pos;

	// Set the width and height
	dest.w = width;
	dest.h = height;

	SDL_RenderCopy(renderer, texture, NULL, &dest);
}

void CApp::renderTexture(SDL_Texture *texture, SDL_Renderer *renderer, int x_pos, int y_pos)
{
	int width, height;

	// get width and height from the texture itself
	SDL_QueryTexture(texture, NULL, NULL, &width, &height);

	renderTexture(texture, renderer, x_pos, y_pos, width, height);
}

SDL_Texture *CApp::loadTexture( const std::string &filename, SDL_Renderer *renderer)
{
	SDL_Texture *texture = nullptr;
	// FIXME: need to check if file is valid prior to opening?
	if ( filename != "" )
	{
		texture = IMG_LoadTexture(renderer, filename.c_str());
		if ( texture == nullptr )
		{
			error ( std::cout, "=loadTexture - IMG_LoadTexture |" );
		}
	}
	return texture;
}

CApp::CApp() {
}

void CApp::error( std::ostream &out_stream, const std::string &err_msg)
{
	out_stream << " ERR: " << err_msg << SDL_GetError() << std::endl;
}

void *CApp::OnExecute(void *device)
{
	if( device == NULL )
	{
		// not running under lmce, ignore all lmce associated stuffenz.
		lmce_here = false;
	}

	if ( lmce_here )
	{
		CSSDevice = (Clock_Screen_Saver *)device;
		if( CSSDevice->m_pData == NULL )
		{
			// error
			lmce_here = false;
		}
	}

	if (OnInit() == false) {
		return NULL;
	}

	SDL_Event Event;

	while (Running) {
		while (SDL_PollEvent(&Event)) {
			OnEvent(&Event);
		}
		OnLoop();
		OnRender();

		SDL_Delay(1000);
	}

	OnCleanup();

	return NULL;
}

bool CApp::OnInit() {
	Running = true;

	if (SDL_Init(SDL_INIT_VIDEO) != 0 ) {
		return false;
	}

	SDL_DisplayMode display_mode;
	if (SDL_GetDesktopDisplayMode(0, &display_mode) != 0) {
		error ( std::cout, "SDL_GetDesktopDisplayMode");
		SDL_Quit();
		return false;
	}

	if (TTF_Init() != 0) {
		SDL_Quit();
		return false;
	}

	windowWidth = display_mode.w;
	windowHeight = display_mode.h;

	// Create SDL window, eventually fullscreen
	window = SDL_CreateWindow("Clock_Screen_Saver", 0, 0, windowWidth, windowHeight, SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_BORDERLESS);
	if (window == nullptr) {
		error ( std::cout, "SDL_CreateWindow" );
		TTF_Quit();
		SDL_Quit();
		return false;
	}

	// Create software renderer, perhaps try HW before SW?
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE );
	if ( renderer == nullptr ) {
		SDL_DestroyWindow(window);
		error ( std::cout, "SDL_CreateRenderer" );
		TTF_Quit();
		SDL_Quit();
		return false;
	}

	// TODO: Check for selected image prior to loading?
	bkgTexture = loadTexture ( DEFAULT_BKG_FILE, renderer );
/*
	if (bkgTexture == nullptr){
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		error ( std::cout, "OnInit - SDL_CreateTextureFromSurface" );
		TTF_Quit();
		SDL_Quit();
		return false;
	}
*/

	int fontSize = 0;
	// TODO: Check for font prior to opening?
	std::string fontFile = DEFAULT_FONT_FILE;
//	fontSize = DEFAULT_FONT_SIZE;

	switch (DEFAULT_CLOCK_STYLE) {
		case HOURAMPM:
			fontSize = windowWidth / 5;
			break;
		case HOUR12:
		case HOUR24:
		default:
			fontSize = windowWidth / 3.5;
	}

	error ( std::cout, to_string(fontSize));

	timeFont = TTF_OpenFont(fontFile.c_str(), fontSize);
	if (timeFont == nullptr){
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		error( std::cout, "TTF_OpenFont" );
		IMG_Quit();
		TTF_Quit();
		SDL_Quit();
		return false;
	}

	return true;
}

void CApp::OnEvent(SDL_Event* Event) {
	if (Event->type == SDL_QUIT) {
		Running = false;
	}
	event = true;
}

void CApp::OnLoop() {
	// Get the current time in a string
	timeCurrent = time(0);
	struct tm *timeinfo = localtime( &timeCurrent );
	char buffer[80];

	switch (DEFAULT_CLOCK_STYLE) {
		case HOUR24:
			// 24 Hour
			strftime(buffer, 80, "%H:%M", timeinfo);
			break;
		case HOURAMPM:
			// 12 Hour with AM/PM
			strftime(buffer, 80, "%I:%M %p", timeinfo);
			break;
		case HOUR12:
		default:
			// 12 Hour
			strftime(buffer, 80, "%I:%M", timeinfo);
	}

	// Strip leading "0" from 12 Hour Time
	timeText = buffer;
	if ( timeText[0] == '0' )
	{
		timeText.erase(0, 1);
	}

}

void CApp::OnRender() {
	if ( !event && (timeText == timeLast) )
		return;
	timeLast = timeText;
	event = false;

	// Set render color to black ( background will be rendered in this color )
	SDL_SetRenderDrawColor( renderer, DEFAULT_BKG_COLOR.r, DEFAULT_BKG_COLOR.g, DEFAULT_BKG_COLOR.b, DEFAULT_BKG_COLOR.a );

	SDL_RenderClear(renderer);

	if ( bkgTexture != nullptr )
	{
		switch (DEFAULT_BKG_STYLE)
		{
			case BKG_CENTERED:
				{
				// This will place the bkgTexture centred on the screen, original size
				int bkgWidth, bkgHeight;
				SDL_QueryTexture(bkgTexture, NULL, NULL, &bkgWidth, &bkgHeight);
				int x = windowWidth / 2 - bkgWidth / 2;
				int y = windowHeight / 2 - bkgHeight / 2;
				renderTexture(bkgTexture, renderer, x, y);
				}
				break;
			case BKG_TILED:
				{
				// This will tile the bkgTexture
				int bkgWidth, bkgHeight;
				SDL_QueryTexture(bkgTexture, NULL, NULL, &bkgWidth, &bkgHeight);

				//Determine how many tiles we'll need to fill the screen
				int xTiles = (windowWidth / bkgWidth) + 1;
				int yTiles = (windowHeight / bkgHeight) + 1;

				//Draw the tiles by calculating their positions
				for (int i = 0; i < xTiles * yTiles; ++i){
					int x = i % xTiles;
					int y = i / xTiles;
					renderTexture(bkgTexture, renderer, x * bkgWidth, y * bkgHeight, bkgWidth, bkgHeight);
				}
				}
				break;
			case BKG_HOME:
				// This will place the bkgTexture at 0,0 & original size
				renderTexture(bkgTexture, renderer, 0, 0);
				break;
			case BKG_FIT:
			default:
				// This will place the bkgTexture strectched to fit the window
				SDL_RenderCopy(renderer, bkgTexture, NULL, NULL);
				break;
		}
	}

	// Render the time onscreen.
	// TODO: get this from data
	SDL_Color colorTime = DEFAULT_FONT_COLOR; // { 255, 255, 255, 255 };
	timeTexture = renderTime(timeText, colorTime, renderer);
	int timeWidth, timeHeight;
	SDL_QueryTexture(timeTexture, NULL, NULL, &timeWidth, &timeHeight);
	int x = windowWidth / 2 - timeWidth / 2;
	int y = windowHeight / 2 - timeHeight / 2;
	renderTexture(timeTexture, renderer, x, y);
	SDL_DestroyTexture(timeTexture);

	SDL_RenderPresent(renderer);
}

void CApp::OnCleanup() {
	TTF_CloseFont(timeFont);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_Quit();
	SDL_Quit();
}


/*

int main(int argc, char* argv[]) {
	CApp theApp;

	return theApp.OnExecute();
}

*/
