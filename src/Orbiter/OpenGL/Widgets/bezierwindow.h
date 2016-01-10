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
#ifndef BEZIERWINDOW_H
#define BEZIERWINDOW_H

/**
  @author Ciprian Mustiata (ciprian.m@plutohome.com)
*/

#include "basewidget.h"

/**
 * Bezier code - based on NEHE tutorial
 * http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=28
 * 
 */

/** Structure For A 3rd Degree Bezier Patch 
*/
typedef struct bpatch {
	// 4x4 Grid Of Anchor Points
	POINT_3D	anchors[4][4];				
	// Display List For Bezier Patch
	GLuint		dlBPatch;					
	// Texture For The Patch
	GLuint		texture;	
	//  Number of patch divisions
	int Divisions;
} BEZIER_PATCH;

/** That class describe a bezier patch. That is defined as a 16 points 
*/
class TBezierWindow : public TBaseWidget
{
public:
	/**
	* Creates a bezier window with with default color and settigs
	* @param ParentContext Object hierarchy that match the drawing
	* @param Left Left coordinate of the window (in pixels) 
	* @param Top Top coordinate of the window (in pixels)
	* @param Width Width size of the window (in pixels)
	* @param Height Height size of the window (in pixels)
	* @param Text Attached text of the window (for now, not used), maybe for future implementations
	*/
	TBezierWindow(MeshFrame* ParentContext,
		int Left, 
		int Top, 
		int Width, 
		int Height, 
		std::string Text);
	virtual ~TBezierWindow();

	/**
	* Paint the window using defaults
	*/ 
	virtual void Paint();

	/**  Set up a background texture to one Window
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
	
	/*
	  Setters
	*/
	/** Set the level of transparency (0.= completly transparent
	 */
	void SetAlpha(float Alpha) {
		if(Alpha>1.0f)
			Background.Alpha = 1.0f;
		else
			Background.Alpha = Alpha;
	}
	
	/** Set the left coordinate of the window
	 */
	void SetLeft(int Left) {
		this->Left = Left;
	}

	/** Set the top coordinate of the window
	 */
	void SetTop(int Top) {
		this->Top = Top;
	}

	/** Set the width (in pixels) of the window
	 */
	void SetWidth(int Width) {
		this->Width = Width;
	}

	/** Set the height (in pixels) of the window
	 */
	void SetHeight(int Height) {
		this->Height = Height;
	}

	/** Set the RGBA color of the window. That color will mix with the curent texture if exists
	 */
	void SetBackgroundColor(float Red, float Green, float Blue, float Alpha) {
		Background.Red = Red;
		Background.Green = Green;
		Background.Blue = Blue;
		Background.Alpha = Alpha;
	}
	
	void SetTextureWraping(float LeftCoordinate, float TopCoordinate, 
		float WidthCoordinate, float HeightCoordinate);	

	/** Utility function: given one FloatRect it set the coordinates of the window
	 * using the float based coordinates
	 */
	void SetRectCoordinates(FloatRect Coordinates);
	
	//Test:
	BEZIER_PATCH BezierDefinition;
};

#endif

