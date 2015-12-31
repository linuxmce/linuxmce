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
#ifndef WizardWidgetPageH
#define WizardWidgetPageH
//---------------------------------------------------------------------------
#include "WizardWidgetBase.h"
//---------------------------------------------------------------------------
#include <string>
//---------------------------------------------------------------------------
#define SURFACE_MAIN				0
#define SURFACE_LEFT				1
#define SURFACE_RIGHT				2
#define SURFACE_HIGH_MAIN			3
#define SURFACE_HIGH_LEFT			4
#define SURFACE_HIGH_RIGHT			5
#define SURFACE_ARROW_LEFT			6
#define SURFACE_ARROW_RIGHT			7
#define SURFACE_HIGH_ARROW_LEFT		8
#define SURFACE_HIGH_ARROW_RIGHT	9
//---------------------------------------------------------------------------

/**
 *	WizardWidgetPage: class that implements one page of the wizard
 *	It knows to give to children messages that doesn't belongs to it
 */
class WizardWidgetPage : public WizardWidgetBase
{
    std::string Caption;

	/**
	 *	Surfaces that defines the button's skin
	 */
	SDL_Surface* ButtonSurfaces[10];

	/**
	 *	Default font (filename, style and size) for actual page	
	 */
	int FontHeight, Style;
	std::string Font_Name;
	TColorDesc *Font_Color;
	/**
	 *	Set the surface sides for buttons
	 */
	void SetSurface(std::string FileName, int IndexSurface);
public:
	/**
	 *	Getters
	 */
	/**
	 *	Get the surfaces for button
	 */
	SDL_Surface* GetSurface(int IndexSurface);
	/**
	 *	Get the height of the font
	 */
	int GetFontHeight();

	TColorDesc *GetFontColor();

	std::string GetFontName();

	/**
	 *	Setup the current font as default's page
	 */
	virtual void SetDefaultFont();

	/**
	 *	Default constructor
	 */
	WizardWidgetPage(GenericBackEnd* FrontEnd, std::string Name);
    /**
     *	Destructor for page
     */
	virtual ~WizardWidgetPage();

	/**
	 *	Setup override method
	 */
	virtual void SetUpAttribute(std::string Attribute, std::string DataValue);

	/**
	 *	Paint the children controls of the current page
	 */
	virtual void Paint();

};
//---------------------------------------------------------------------------
#endif
