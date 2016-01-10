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
//---------------------------------------------------------------------------
#ifndef WizardWidgetBackground_H
#define WizardWidgetBackground_H
//---------------------------------------------------------------------------
#include "WizardWidgetBase.h"
#include "SDL.h"
//---------------------------------------------------------------------------
class WizardWidgetBackground : public WizardWidgetBase
{
	TColorDesc* Color;
	SDL_Surface* Surface;
	/**
	 *	Set up the surface from an existing file
	 */
	virtual void SetSurface(std::string FileName);
public:
	/**
	 *	Default constructor
	 */
	WizardWidgetBackground(GenericBackEnd* FrontEnd, std::string Name);
	/**
	 *	Default destructor
	 */
	virtual ~WizardWidgetBackground();
	/**
	 *	Paints the control and the children of the curent component
	 */
	virtual void Paint();

	/**
	 *	Set up the known attributes
	 */
	virtual void SetUpAttribute(std::string Attribute, std::string DataValue);

};
//---------------------------------------------------------------------------
#endif
