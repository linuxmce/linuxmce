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
/**
 *	Created by CipLogic < ciprian dot m at plutohome dot com >
 */
#ifndef WM_EVENT_H_
#define WM_EVENT_H_

#include <SDL.h>

/**
 * WMET = Window Message Event Type
 * Types that may generate one window, is used 
 * for future kind of events and maybe for multiple devices
 */
 
#define WMET_APPLICATION		1000
#define WMET_QUIT				1001

#define WMET_KEYS				2000
#define WMET_LEFT_KEY 			2001
#define WMET_RIGHT_KEY			2002
#define WMET_UP_KEY				2003
#define WMET_DOWN_KEY			2004

#define WMET_ENTER_KEY			2010
#define WMET_ESCAPE_KEY			2030

#define WMET_PLUS_KEY			2021
#define WMET_MINUS_KEY			2022

#define WMET_SAVE				3010

/**
 *	Helper class that converts SDL events 
 *	to generic WM_Event
 */
class WM_Event
{
public:
	int Type;

	/**
	 *	Set the current event as Quit Event
	 */
	void Quit();
	
	/**
	 *	Set the current event as left key event
	 */
	void LeftKey();

	/**
	 *	Set the current event as right key event
	 */
	void RightKey();	

	/**
	 *	Set the current event as Up key event
	 */
	void UpKey();	

	/**
	 *	Set the current event as Down key event
	 */
	void DownKey();	

	/**
	 *	Set the current event as Enter key event
	 */
	void EnterKey();	

	/**
	 *	Set the current event as Escape key event
	 */
	void EscapeKey();	

	/**
	 *	Set the current event as Plus key event
	 */
	void PlusKey();	

	/**
	 *	Set the current event as Minus key event
	 */
	void MinusKey();

	/**
	 *	
	 */
	void Save();	

	/**
	 *	Get the event information based on SDL_Event
	 *	given as parameter
	 */
	void ConvertFromSDLEvent(SDL_Event& Event);	
};

#endif /*WM_EVENT_H_*/
