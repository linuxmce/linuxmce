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
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#ifndef BASEWIDGET_H
#define BASEWIDGET_H

/**
That class is an abstract class that should be overwriten for basic purposes. It describes an OpenGL Window

@author ciplogic
*/

#include <vector>
#include <string>

#include "../GLMathUtils.h"
#include "../Mesh/MeshFrame.h"
#include "../OpenGLGraphic.h"
#include "../GLMathUtils.h"

class TBaseWidget{
public:
	TBaseWidget();
	
	virtual ~TBaseWidget();
	virtual void Paint();
	
	/**
	* That class defines one GL Widget
	* You may asociate size of it as top-left point and size (in pixels)
	* @param ParamLeft - left coordinate (of top-left most point)
	* @param ParamTop - top coordinate (of top-left most point of the widget)
	* @param ParamWidth - width of the widget
	* @param ParamHeight - height of the widget
	* @param ParamText - the asociated text of the widget
	*/
	TBaseWidget(MeshFrame* ParentContext,
		int Left, 
		int Top, 
		int Width, 
		int Height, 
		std::string Text);

	/**
	 *	Method that gives the rectangle of the textures coordinates
	 */
	FloatRect GetTextureCoordinates();

	// Getters
	/**
	 * Get the texure ID
	 */
	OpenGLGraphic* GetTexture() {
		return BackgroundTex;
	} 
	
	/**
	 * get the visibility window status
	 */
	bool GetVisible();

	virtual std::string GetName();
	
	/**
	 * Get the parent window
	 * if Result = NULL, that window is a parent window
	 */
	 
	TBaseWidget* GetParent();
	
	/**
	 * Return the number of children
	 */  
	int GetNoChild();
	
	/**
	 * return a specified children 
	 */
	TBaseWidget* GetChild(unsigned int NoChild);

	MeshFrame* GetContext();
		
	// Setters
	/**
	 * Set the windows visible status
	 */ 
	void SetVisible(bool VisibleValue) {
		Visible = VisibleValue;
	}
	
	/**
	 * set the texture (OpenGL ID for texture)
	 */
	void SetTexture(OpenGLGraphic* ValueBackgroundTex) {
		BackgroundTex = ValueBackgroundTex;
	}
	
	
protected:
	/**
	 * Texture Wrapper apply the texturing coordinates system over the
	 * window. For instance the (0.0, 0.0, 1.0, 0.5) means the top-half
	 * side of the texture set up for that window
	 */
	FloatRect TextureWrapper2D;	
	/**
	 * attached texture to the widget, if are more, you should create another widget
	 * or add in the derived class
	 * if BackgroundTex = 0 then the texture is disabled
	 */
	OpenGLGraphic* BackgroundTex;
	/**
	 * Background: color blending with current window texture, if doesn't exist, 
	 * that is the color that apply over the window rectangle	 
	 */
	ColorRGB Background;

	/**
	 * Size of the current widget on screen. They are relative to the widgets, but painting operation may
	 * overflow that dimensions
	 */
	int Left, Top, Width, Height;
	/**
	 * Children widgets to the current widget
	 */	
	std::vector<TBaseWidget*> Childs;
	/**
	 * Set up the parent of the window
	 */
	TBaseWidget* Parent;
	std::string Text;
	/**
	 * True if the window will be displayed
	 */
	bool Visible; 

	MeshFrame* Context;
	MeshFrame* Frame;
	MeshContainer* Container;
};
#endif
