#ifndef _CAPP_H_
#define _CAPP_H_

#include <string>
#include <iostream>
#include <ctime>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>


enum BKG_STYLE { BKG_CENTERED, BKG_TILED, BKG_HOME, BKG_FIT };
enum CLOCK_STYLE { HOUR12, HOUR24, HOURAMPM };

namespace DCE
{
	class Clock_Screen_Saver;
	class Message;
}

using namespace DCE;

class CApp {
	private:
		static	bool		Running;
		static	bool		lmce_here;
		static	SDL_Window	*window;
		static	SDL_Renderer	*renderer;

		static	SDL_Texture	*bkgTexture;
		static	SDL_Surface	*bkgSurface;;

		static	SDL_Texture	*timeTexture;
		static	SDL_Surface	*timeSurface;
		static	TTF_Font	*timeFont;

		static	int		windowWidth;
		static	int		windowHeight;

		static	time_t		timeCurrent;
		static std::string	timeText;
		static std::string	timeLast;
		static bool		event;
		static bool		Active;

		static DCE::Clock_Screen_Saver *CSSDevice;
		static CLOCK_STYLE	clockStyle;
		static std::string	fontFilename;
		static SDL_Color	timeColor;
		static int		fontSize;
		static std::string	bkgFilename;
		static BKG_STYLE	bkgStyle;
		static SDL_Color	bkgColor;

	public:
		CApp();

		static void 	*OnExecute(void *dev);

		static		void		ProcessCommand(std::string cmd);

	private:
		static		void		error( std::ostream &out_stream, const std::string &err_msg);

		static		SDL_Texture	*loadTexture( const std::string &filename, SDL_Renderer *renderer);

		static		void		renderTexture(SDL_Texture *texture, SDL_Renderer *renderer,
							int x_pos, int y_pos, int w, int h);

		static		void 		renderTexture(SDL_Texture *texture, SDL_Renderer *renderer,
							int x_pos, int y_pos);

		static		SDL_Texture 	*renderText(const std::string &message, const std::string &fontFile,
						        SDL_Color color, int fontSize, SDL_Renderer *renderer);

		static		SDL_Texture 	*renderTime(const std::string &message, SDL_Color color,
							SDL_Renderer *renderer);

		static		void 		ParseConfiguration(std::string config);

		static		bool		OnInit();
		static		void		OnEvent(SDL_Event* Event);
		static		void		OnLoop();
		static		void		OnRender();
		static		void		OnCleanup();

		static bool Minimize();
		static bool Maximize();
};

#endif
