#include "CApp.h"
#include "Clock_Screen_Saver.h"
#include "utilities/linux/window_manager/WMController/WMController.h"

const CLOCK_STYLE	DEFAULT_CLOCK_STYLE = HOUR24;
const std::string	DEFAULT_FONT_FILE = "/usr/share/fonts/truetype/msttcorefonts/Verdana.ttf";
const SDL_Color		DEFAULT_FONT_COLOR = { 255, 255, 255, 255 };

//const std::string	DEFAULT_BKG_FILE = "./background.png";
const std::string	DEFAULT_BKG_FILE = "";
const BKG_STYLE		DEFAULT_BKG_STYLE = BKG_CENTERED;
const SDL_Color		DEFAULT_BKG_COLOR = { 0, 0, 0, 255 };

bool		CApp::Running = false;
bool		CApp::lmce_here = false;
SDL_Window	*CApp::window = nullptr;
SDL_Renderer	*CApp::renderer = nullptr;
SDL_Texture	*CApp::bkgTexture = nullptr;
SDL_Surface	*CApp::bkgSurface = nullptr;
SDL_Texture	*CApp::timeTexture = nullptr;
SDL_Surface	*CApp::timeSurface = nullptr;
TTF_Font	*CApp::timeFont = nullptr;
time_t		CApp::timeCurrent;
int		CApp::windowWidth = 0;
int		CApp::windowHeight = 0;
DCE::Clock_Screen_Saver *CApp::CSSDevice = NULL;
std::string 	CApp::timeText = ":";
std::string 	CApp::timeLast = "";
bool		CApp::event = false;
bool		CApp::Active = false;

CLOCK_STYLE	CApp::clockStyle = DEFAULT_CLOCK_STYLE;
std::string	CApp::fontFilename = DEFAULT_FONT_FILE;
SDL_Color	CApp::timeColor = DEFAULT_FONT_COLOR;
int		CApp::fontSize = 0;
std::string	CApp::bkgFilename = DEFAULT_BKG_FILE;
BKG_STYLE	CApp::bkgStyle = DEFAULT_BKG_STYLE;
SDL_Color	CApp::bkgColor = DEFAULT_BKG_COLOR;

bool CApp::Minimize()
{
  WMControllerImpl *pWMController = new WMControllerImpl();

  if (!pWMController)
    return false;

  return pWMController->SetVisible("Clock_Screen_Saver.Clock_Screen_Saver",false);
}

bool CApp::Maximize()
{
  WMControllerImpl *pWMController = new WMControllerImpl();

  if (!pWMController)
    return false;

  return pWMController->SetVisible("Clock_Screen_Saver.Clock_Screen_Saver",true);
}

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

void CApp::ParseConfiguration(std::string config)
{
#ifdef KDE_LMCE
	vector<string> vectLines;
	// Split into \n and then split each into values.
	StringUtils::Tokenize(config,"\n",vectLines);
	for (vector<string>::iterator it=vectLines.begin(); it!=vectLines.end(); ++it)
	{
		std::string sItem = *it;
		std::string::size_type pos=0;
		std::string sParameter = StringUtils::Tokenize(sItem,"|",pos);
		std::string sValue = StringUtils::Tokenize(sItem,"|",pos);

		error ( std::cout, string("Configuration - Line ") + sItem );
		if ( sValue != "" && sValue != "\r" )
		{
			error ( std::cout, string("Configuration - sParameter: ") + sParameter + ", sValue: '" + sValue + "'");
			/* check config parameters for valid values and set vars accordingly */
			if ( sParameter == "time_style" )
			{
				// enum CLOCK_STYLE { HOUR12, HOUR24, HOURAMPM };
				if ( sValue == "24" )
				{
					error ( std::cout, "Setting 24HR Display" );
					clockStyle = HOUR24;
				}
				else if ( sValue == "12" )
				{
					error ( std::cout, "Setting 12HR Display" );
					clockStyle = HOUR12;
				}
				else if ( sValue ==  "AMPM" )
				{
					error ( std::cout, "Setting AM/PM Display" );
					clockStyle = HOURAMPM;
				}
			}
			else if ( sParameter == "time_font" )
			{
				error ( std::cout, string("Checking fontFilename: ") + sValue );
				// get and validate the font
				if ( FileUtils::FileExists( sValue ) )
				{
					fontFilename = sValue;
					error ( std::cout, string("Setting fontFilename: ") + fontFilename );
				}
			}
			else if ( sParameter == "time_size" )
			{
				fontSize = atoi( sValue.c_str() );
				if ( fontSize < 0 )
					fontSize = 0;
				else
					error ( std::cout, string("Setting fontSize to: ") + sValue );

			}
			else if ( sParameter == "time_color" )
			{
				// tokenize the colour values
				std::string::size_type pos=0;
				std::string sR = StringUtils::Tokenize(sValue,",",pos);
				std::string sG = StringUtils::Tokenize(sValue,",",pos);
				std::string sB = StringUtils::Tokenize(sValue,",",pos);
				std::string sA = StringUtils::Tokenize(sValue,",",pos);

				timeColor = { (unsigned char) std::stoi(sR), (unsigned char) std::stoi(sG), 
						(unsigned char) std::stoi(sB), (unsigned char) std::stoi(sA) };
			}
			else if ( sParameter == "bkg_color" )
			{
				// tokenize the colour values
				std::string::size_type pos=0;
				std::string sR = StringUtils::Tokenize(sValue,",",pos);
				std::string sG = StringUtils::Tokenize(sValue,",",pos);
				std::string sB = StringUtils::Tokenize(sValue,",",pos);
				std::string sA = StringUtils::Tokenize(sValue,",",pos);

				bkgColor = { (unsigned char) std::stoi(sR), (unsigned char) std::stoi(sG),
						(unsigned char) std::stoi(sB), (unsigned char) std::stoi(sA) };
			}
			else if ( sParameter == "bkg_image" )
			{
				// get and validate the image
				if ( FileUtils::FileExists( sValue ) )
				{
					bkgFilename = sValue;
				}
			}
			else if ( sParameter == "bkg_location" )
			{
				if ( sValue == "centered" )
				{
					error ( std::cout, "Setting background Centered" );
					bkgStyle = BKG_CENTERED;
				}
				else if ( sValue == "home" )
				{
					error ( std::cout, "Setting background Home" );
					bkgStyle = BKG_HOME;
				}
				else if ( sValue == "tiled" )
				{
					error ( std::cout, "Setting background Tiled" );
					bkgStyle = BKG_TILED;
				}
				else if ( sValue == "fit" )
				{
					error ( std::cout, "Setting background Fit" );
					bkgStyle = BKG_FIT;
				}
			}
		}
	}
#endif
}

void CApp::ProcessCommand(std::string cmd)
{
	if (cmd == "ON")
	{
		error ( std::cout, "Turning ON" );
		CApp::Maximize();
		SDL_ShowWindow(window);
		SDL_RaiseWindow(window);
		SDL_ShowCursor(SDL_DISABLE);
		Active = true;
		event = true;
		timeLast = "";
	}
	else if (cmd == "OFF")
	{
		error ( std::cout, "Turning OFF" );
		CApp::Minimize();
		SDL_ShowCursor(SDL_ENABLE);
		SDL_HideWindow(window);
		Active = false;
	}
}

void *CApp::OnExecute(void *device)
{
#ifdef KDE_LMCE
	lmce_here = true;
	if( device == NULL )
	{
		// not running under lmce, ignore all lmce associated stuffenz.
		lmce_here = false;
	}

	if ( lmce_here )
	{
		error ( std:: cout, "IT'S HERE!!!!!!" );
		CSSDevice = (Clock_Screen_Saver *)device;
	}

	if ( lmce_here )
	{
		std::string config = CSSDevice->DATA_Get_Configuration();
		error (std::cout, config );

		ParseConfiguration(config);
	}
#endif

	if ( OnInit() == false )
	{
		return NULL;
	}

	SDL_Event Event;

	while ( Running )
	{
		while (SDL_PollEvent(&Event))
		{
			OnEvent(&Event);
		}

		OnLoop();
		OnRender();

		SDL_Delay(100);
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
	window = SDL_CreateWindow("Clock_Screen_Saver", 0, 0, windowWidth, windowHeight, 
		SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_BORDERLESS | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_HIDDEN );
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
	bkgTexture = loadTexture ( bkgFilename, renderer );

	// TODO: Check for font prior to opening?
	if ( fontSize == 0 )
	{
		switch (clockStyle) {
			case HOURAMPM:
				fontSize = windowWidth / 5;
				break;
			case HOUR12:
			case HOUR24:
			default:
				fontSize = windowWidth / 3.5;
		}
	}

	error ( std::cout, to_string(fontSize));

	timeFont = TTF_OpenFont(fontFilename.c_str(), fontSize);
	if (timeFont == nullptr){
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		error( std::cout, "TTF_OpenFont" );
		IMG_Quit();
		TTF_Quit();
		SDL_Quit();
		return false;
	}

	ProcessCommand("OFF");

	return true;
}

void CApp::OnEvent(SDL_Event* Event) {
	switch (Event->type)
	{
		case SDL_QUIT:
			Running = false;
			break;
		case SDL_APP_WILLENTERFOREGROUND:
		case SDL_APP_DIDENTERFOREGROUND:
			error ( std::cout, "SDL_WINDOWEVENT_SHOWN" );
			timeLast = "";
			break;
	}
}

void CApp::OnLoop() {

	Uint32 flags = SDL_GetWindowFlags(window);
	if ( (flags & SDL_WINDOW_SHOWN) == SDL_WINDOW_SHOWN )
	{
		if (!Active)
		{
			std::cout << "S" << std::flush;
			SDL_ShowCursor(SDL_ENABLE);
			SDL_HideWindow(window);
		}
	}
	if ( (flags & SDL_WINDOW_HIDDEN) == SDL_WINDOW_HIDDEN )
	{
		if (Active)
		{
			std::cout << "H" << std::flush;
			SDL_ShowWindow(window);
			SDL_RaiseWindow(window);
			SDL_ShowCursor(SDL_DISABLE);
		}
	}

	// Get the current time in a string
	timeCurrent = time(0);
	struct tm *timeinfo = localtime( &timeCurrent );
	char buffer[80];

	switch (clockStyle) {
		case HOUR24:
			// 24 Hour
			strftime(buffer, 80, "%H:%M", timeinfo);
			break;
		case HOURAMPM:
			// 12 Hour with AM/PM
			strftime(buffer, 80, "%I:%M %p", timeinfo);
			break;
		case HOUR12:
			// 12 Hour
			strftime(buffer, 80, "%I:%M", timeinfo);
			break;
	}

	// Strip leading "0" from 12 Hour Time
	timeText = buffer;
	if ( timeText[0] == '0' )
	{
		timeText.erase(0, 1);
	}
}

void CApp::OnRender()
{
	static int i = 0;
	if (!Active)
		return;

	if ( (++i != 10) && !event && (timeText == timeLast) )
		return;
	timeLast = timeText;
	event = false;
	i = 0;

	std::cout << "R" << std::flush;

	// Set render color to bkgColor
	SDL_SetRenderDrawColor( renderer, bkgColor.r, bkgColor.g, bkgColor.b, bkgColor.a );

	SDL_RenderClear(renderer);

	if ( bkgTexture != nullptr )
	{
		switch (bkgStyle)
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
	SDL_Color colorTime = timeColor;
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
