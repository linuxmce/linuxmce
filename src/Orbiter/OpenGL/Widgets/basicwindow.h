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
#ifndef BASICWINDOW_H
#define BASICWINDOW_H

#include "basewidget.h"

#include "../Mesh/MeshContainer.h"
/**
@author ciplogic
*/

class TBasicWindow : public TBaseWidget
{	
public:
	TBasicWindow(MeshFrame* Context,
		int Left, 
		int Top, 
		int Width, 
		int Height, 
		std::string Text);
	virtual ~TBasicWindow();

	virtual void Paint();

	/**
	 *        Set up a background texture to one Window
	 * @param Background That surface must be in size as power of two (e.g. 256 x 512)
	 *      Default texture mapping will fill completly the window. 
	 *	Future implementation should add texture mapping wrappers
	 *  
	 * @return it returns true if the texture was corectly set
	 */
	bool SetBackgroundImage(OpenGLGraphic* Background);

public:
	/**
	  Getters
	*/
	
	float GetAlpha() const {
		return Background.Alpha;	
	}
	
	int GetLeft() const {
		return Left;
	}

	int GetTop() const {
		return Top;
	}
	
	int GetWidth() const {
		return Width;
	}

	int GetHeight() const {
		return Height;
	}
	
	/**
	  Setters
	*/
	void SetAlpha(float Alpha) {
		if(Alpha>1.0f)
			Background.Alpha = 1.0f;
		else
			Background.Alpha = Alpha;
	}
	
	void SetLeft(int Left) {
		this->Left = Left;
	}

	void SetTop(int Top) {
		this->Top = Top;
	}

	void SetWidth(int Width) {
		this->Width = Width;
	}

	void SetHeight(int Height) {
		this->Height = Height;
	}

	void SetBackgroundColor(float Red, float Green, float Blue, float Alpha) {
		Background.Red = Red;
		Background.Green = Green;
		Background.Blue = Blue;
		Background.Alpha = Alpha;
	}
	
	void SetTextureWraping(float LeftCoordinate, float TopCoordinate, 
		float WidthCoordinate, float HeightCoordinate);
	
	void SetTextureWraping(FloatRect UVRect);

	/**
	 * Utility function: given one FloatRect it set the coordinates of the window
	 * using the float based coordinates
	 */
	void SetRectCoordinates(FloatRect Coordinates);
			
	void SetAbsoluteUV(FloatRect UVRect);

};

#endif
