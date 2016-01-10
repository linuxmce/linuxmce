#ifndef __SDL_FRONT_END_H__
#define __SDL_FRONT_END_H__
//--------------------------------------------------------------------------------------------------------
#include <SDL.h>
#include <SDL_ttf.h>
//--------------------------------------------------------------------------------------------------------
#include "DCE/Logger.h"
//--------------------------------------------------------------------------------------------------------
#include "../../LCDLogic/IRenderer.h"
#include "../../LCDLogic/IInputProvider.h"
//--------------------------------------------------------------------------------------------------------
class SDLFrontEnd : public IRenderer, public IInputProvider
{
private:

	SDL_Surface *m_pSurface;
	TTF_Font *m_pFont;
	int m_nWidth, m_nHeight;
	
	pluto_pthread_mutex_t m_ScreenMutex;

public:

	SDLFrontEnd(IInputProcessor *pInputProcessor, int w, int h);
	~SDLFrontEnd();

	bool Init();
	bool EventLoop();

	void Render(const DisplayState &display_state);

private:

	void Clear();
	void RenderText(int x, int y, string sText, SDL_Color color);
};
//--------------------------------------------------------------------------------------------------------
#endif //__SDL_FRONT_END_H__
