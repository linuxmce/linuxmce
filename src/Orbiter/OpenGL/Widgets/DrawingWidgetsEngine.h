/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
/*
Orbiter

Copyright (C) 2006 Pluto, Inc., a Florida Corporation

www.plutohome.com

Phone: +1 (877) 758-8648

This program is distributed according to the terms of the Pluto Public License, available at:
http://plutohome.com/index.php?section=public_license

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

@author: "Ciprian Mustiata" ciprian.m at plutohome dot com, "Cristian Miron" chris.m at plutohome dot com 

*/


#ifndef DRAWINGWIDGETSENGINE_H
#define DRAWINGWIDGETSENGINE_H

#include "basewidget.h"
#include "../Mesh/MeshFrame.h"

#include <vector>

#define BASICWIDGET		1000
#define BASICWINDOW		(BASICWIDGET+1)
#define BEZIERWINDOW	(BASICWIDGET+2)
namespace GLEffect2D
{

class DrawingWidgetsEngine
{
	int Width, Height;
	std::vector<TBaseWidget*> Widgets;
	MeshFrame Context;


public:
	/**
	* That function add inthe drawing list the widgets
	* @param Widget 
	*/
	void AddWidget(TBaseWidget* Widget);

	/**
	 * Getters
	 */
	int GetWidth() {
		return Width;
	}
	
	int GetHeight() {
		return Height;
	}
	/**
	 *	Default constructor
	 */
	DrawingWidgetsEngine(int Width, int Height);

	/*
	 *	Default no-op destructor 
	 */
	~DrawingWidgetsEngine();
	///	Painting routines

	/** Paint all widgets registred
	*/
	void Paint();
    	
	/**
		Widget list management
	*/
	/**
	 *    Factory method
	 * @param WidgetType Type of window it wants to be created 
	 * @param Top The top of final window
	 * @param Left The left of final window
	 * @param Width The width of final window
	 * @param Height The height of final window
	 * @param Text The text of final window
	 * @return 
	 */
	TBaseWidget* CreateWidget(int WidgetType, int Top, int Left, int Width, int Height, char* Text);
	/**
	 * Delete specific widget from painting list
	 * @param Widget Widget to be removed from the list. 
	 */
	void DeleteWidget(TBaseWidget* Widget);
	/**
	 *    Clears all widgets from drawing list
	 */
	void RemoveAll();

	/**
	 *	Removes the widget from the list of drawing widgets without delete it as object
	 *	returns false in case that widget doesn't exist
	 */
	bool RemoveWidgetFromList(TBaseWidget* Widget);

	void Resize(int Width, int Height);
};

}

#endif	

