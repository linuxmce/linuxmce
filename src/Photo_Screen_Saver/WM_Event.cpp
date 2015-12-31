/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#include "WM_Event.h"

void WM_Event::Quit()
{
	this->Type = WMET_QUIT;
}

void WM_Event::LeftKey()
{
	this->Type = WMET_LEFT_KEY;
}

void WM_Event::RightKey()
{
	this->Type = WMET_RIGHT_KEY;
}

void WM_Event::UpKey()
{
	this->Type = WMET_UP_KEY;	
}
	
void WM_Event::DownKey()
{
	this->Type = WMET_DOWN_KEY;	
}		

void WM_Event::EnterKey()
{
	this->Type = WMET_ENTER_KEY;	
}

void WM_Event::EscapeKey()
{
	this->Type = WMET_ESCAPE_KEY;
}

void WM_Event::MinusKey()
{
	this->Type = WMET_MINUS_KEY;
}

void WM_Event::PlusKey()
{
	this->Type = WMET_PLUS_KEY;
}

void WM_Event::Save()
{
	this->Type = WMET_SAVE;
}


void WM_Event::ConvertFromSDLEvent(SDL_Event& Event)
{
	return; // Ignore all events
	switch(Event.type)
	{
		case SDL_QUIT:
			Quit();
			break;
		case SDL_KEYUP:
			switch(Event.key.keysym.sym)
			{
				case SDLK_UP:
					UpKey();
					break;
				case SDLK_DOWN:
					DownKey();
					break;
				case SDLK_LEFT:
					LeftKey();
					break;
				case SDLK_RIGHT:
					RightKey();
					break;
				case SDLK_ESCAPE:
					EscapeKey();
					break;
				case SDLK_KP_PLUS:
				case SDLK_PLUS:
					PlusKey();
					break;
				case SDLK_KP_MINUS:
				case SDLK_MINUS:
					MinusKey();
					break;
				case SDLK_KP_ENTER:
				case SDLK_RETURN:
					EnterKey();
					break;
				default:
					Type = 0;
			}
			break;
		case SDL_MOUSEMOTION:
			Quit();
			break;
		default:
			Type = 0;
	}
}
