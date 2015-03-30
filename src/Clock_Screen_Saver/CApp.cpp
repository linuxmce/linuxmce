#include "CApp.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;


bool        CApp::Running = false;

SDL_Window      *CApp::window = nullptr;
SDL_Renderer    *CApp::renderer = nullptr;
SDL_Texture     *CApp::bkgTexture = nullptr;
SDL_Surface     *CApp::bkgSurface = nullptr;
SDL_Texture     *CApp::timeTexture = nullptr;
SDL_Surface     *CApp::timeSurface = nullptr;
TTF_Font        *CApp::timeFont = nullptr;
SDL_Color       *CApp::colorTime = nullptr;
time_t          CApp::timeCurrent;



SDL_Texture *CApp::renderTime(const std::string &message, SDL_Color color, SDL_Renderer *renderer)
{
	//We need to first render to a surface as that's what TTF_RenderText
	//returns, then load that surface into a texture
	SDL_Surface *surf = TTF_RenderText_Blended(timeFont, message.c_str(), color);
	if (surf == nullptr){
		error( std::cout, "TTF_RenderText" );
		return nullptr;
	}

	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surf);
	if (texture == nullptr){
		error( std::cout, "CreateTexture" );
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
		error( std::cout, "TTF_OpenFont" );
		return nullptr;
	}

	//We need to first render to a surface as that's what TTF_RenderText
	//returns, then load that surface into a texture
	SDL_Surface *surf = TTF_RenderText_Blended(font, message.c_str(), color);
	if (surf == nullptr){
		TTF_CloseFont(font);
		error( std::cout, "TTF_RenderText" );
		return nullptr;
	}

	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surf);
	if (texture == nullptr){
		error( std::cout, "CreateTexture" );
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
	SDL_Texture *texture = IMG_LoadTexture(renderer, filename.c_str());
	if ( texture == nullptr )
	{
		error ( std::cout, "loadTexture, IMG_LoadTexture" );
	}
	return texture;
}

CApp::CApp() {
	window = nullptr;
	renderer = nullptr;

	bkgSurface = nullptr;
	bkgTexture = nullptr;

	timeSurface = nullptr;
	timeTexture = nullptr;
	timeFont = nullptr;

	Running = true;
}

void CApp::error( std::ostream &out_stream, const std::string &err_msg)
{
	out_stream << err_msg << " ERR: " << SDL_GetError() << std::endl;
}

void *CApp::OnExecute(void *dev) {
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
	window = nullptr;
	renderer = nullptr;

	bkgSurface = nullptr;
	bkgTexture = nullptr;

	timeSurface = nullptr;
	timeTexture = nullptr;
	timeFont = nullptr;

	Running = true;

	if (SDL_Init(SDL_INIT_VIDEO) != 0 ) {
		return false;
	}

	if (TTF_Init() != 0) {
		SDL_Quit();
		return false;
	}

	// Create SDL window, eventually fullscreen
	window = SDL_CreateWindow("Clock_Screen_Saver", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
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
		SDL_Quit;
		return false;
	}

	// TODO: Check for selected image prior to loading?
	bkgTexture = loadTexture ( "./background.png", renderer );
	if (bkgTexture == nullptr){
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		error ( std::cout, "SDL_CreateTextureFromSurface" );
		TTF_Quit();
		SDL_Quit();
		return false;
	}

	// TODO: Check for font prior to opening?
	std::string fontFile = "./lazy.ttf";
	int fontSize = 180;
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
}

void CApp::OnLoop() {
}

void CApp::OnRender() {
	SDL_RenderClear(renderer);

	if ( bkgTexture != nullptr )
	{
/*
		// This will tile the bkgTexture
		int bkgWidth, bkgHeight;
		SDL_QueryTexture(bkgTexture, NULL, NULL, &bkgWidth, &bkgHeight);

		//Determine how many tiles we'll need to fill the screen
		int xTiles = SCREEN_WIDTH / bkgWidth;
		int yTiles = SCREEN_HEIGHT / bkgHeight;

		//Draw the tiles by calculating their positions
		for (int i = 0; i < xTiles * yTiles; ++i){
			int x = i % xTiles;
			int y = i / xTiles;
			renderTexture(bkgTexture, renderer, x * bkgWidth, y * bkgHeight, bkgWidth, bkgHeight);
		}
*/

		// This will place the bkgTexture centred on the screen, original size
		int bkgWidth, bkgHeight;
		SDL_QueryTexture(bkgTexture, NULL, NULL, &bkgWidth, &bkgHeight);
		int x = SCREEN_WIDTH / 2 - bkgWidth / 2;
		int y = SCREEN_HEIGHT / 2 - bkgHeight / 2;
		renderTexture(bkgTexture, renderer, x, y);

/*
		// This will place the bkgTexture at 0,0 & original size
		//renderTexture(bkgTexture, renderer, 0, 0);
*/
/*
		// This will place the bkgTexture strectched to fit the window
		//SDL_RenderCopy(renderer, bkgTexture, NULL, NULL);
*/
	}
	else
	{
		// Create bkground colour?
	}

	// Get the current time in a string
	timeCurrent = time(0);
	struct tm *timeinfo = localtime( &timeCurrent );
	char buffer[80];
	strftime(buffer, 80, "%I:%M", timeinfo);
	std::string timeText = buffer;

	// Render the time onscreen.
	// TODO: get this from data
	SDL_Color colorTime = { 255, 255, 255, 255 };
	//timeTexture = renderText(timeText, "./lazy.ttf", colorTime, 180, renderer);
	timeTexture = renderTime(timeText, colorTime, renderer);
	int timeWidth, timeHeight;
	SDL_QueryTexture(timeTexture, NULL, NULL, &timeWidth, &timeHeight);
	int x = SCREEN_WIDTH / 2 - timeWidth / 2;
	int y = SCREEN_HEIGHT / 2 - timeHeight / 2;
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
