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
#ifndef GLMathUtils_H_
#define GLMathUtils_H_

#include "../../SerializeClass/ShapesColors.h"

/**
 *	OpenGL based color description, the values are 0.0f to 1.0f
 */
struct FloatColor 
{
	float Red, Green, Blue, Alpha;

	FloatColor(float ValueRed, float ValueGreen, float ValueBlue, 
		float ValueAlpha)
		: Red(ValueRed),
		Green(ValueGreen),
		Blue(ValueBlue),
		Alpha(ValueAlpha)
	{

	}
};

typedef struct FloatColor ColorRGB;  

/**
 * That class defines one rectangle structure which is defined as top,left coordinate
 * and it's width and height
 * That structure is used where Rect is not suitable for actual needs (because works with 
 * integer based types
 */
class FloatRect
{
public:
	float Top;
	float Left;
	float Width;
	float Height;

	/**
	* Default constructor
	*/
	FloatRect();


	/**
	* Constructor with setup the variables
	*/
	FloatRect(float , float , float , float );

	/**
	* Copy onstructor
	*/
	FloatRect(PlutoRectangle Rectangle);

	/**
	* That function say ain intermediar rectangle from source rectangle
	* and one another dest point using a given value 
	* for Value = 0 the result is the source rectangle
	* for Value = 1 the result is destination rectangle
	* for Value = 0.5 the result is the middle of both rectangles
	*/
	FloatRect Interpolate(FloatRect DestRect, float Value);	

};
/**
* That class defines one point structure which is defined as X, Y coordinate
* That structure is used where Point is not suitable for actual needs (because works with 
* integer based types)
*/
class FloatPoint
{
public:
	float X;
	float Y;
	/**
	* Default constructor
	*/
	FloatPoint() 
		: X(0),
		Y(0)
	{
	}

	FloatPoint(float ValueX, float ValueY) 
		: X(ValueX),
		Y(ValueY)
	{
	}

	/**
	* That function say ain intermediar point from source point
	* and one another dest point using a given value 
	* for Value = 0 the result is the source point
	* for Value = 1 the result is destination point
	* for Value = 0.5 the result is the middle of both points
	*/

	FloatPoint Interpolate(FloatPoint DestPoint, float Value)
	{
		FloatPoint Result;

		Result.X = X*(1-Value)+DestPoint.X*Value;
		Result.Y = Y*(1-Value)+DestPoint.Y*Value;

		return Result;
	}
};

class T3DMatrix;

/**
* Class that implements the 3D coordinate system. Is not used for actual implementation
* but became functional when the widgets will be fully 3D
*/
class T3DVertex {
public:
	/**
	* 3D coordinate of one vertex
	*/	 
	float X, Y, Z;
public:
	/**
	*        Moves a vector with a specified size
	* @param ValueX The X axis size
	* @param ValueY The Y axis size
	* @param ValueZ The Z axis size
	*/
	void Translate(float ValueX, float ValueY, float ValueZ);

	/**
	*        Scales (over the origin) one 3D Point
	* @param ValueX The X axis size
	* @param ValueY The X axis size
	* @param ValueZ The X axis size
	*/
	void Scale(float ValueX, float ValueY, float ValueZ);
	friend class T3DMatrix;

	/**
	*        Apply a custom transform (given by a transform matrix) to a point
	* @param Matrix 
	* @return 
	*/
	T3DVertex Transform(T3DMatrix& Matrix);
};

/**
* Structure for a 3-dimensional point 
*/
typedef struct point_3d {						
	float x, y, z;
} POINT_3D;


class MathUtils
{
public:
	/**
	* That function the minimum value greater or equal than given value which is power of two
	* @param Value 
	* @return 
	*/
	static int MinPowerOf2(int Value);
	static float InterpolateValues(float Source, float Dest, float Theta);
	static float InterpolateSinus(float Source, float Dest, float Theta);
	static const float _PI;

	static POINT_3D pointAdd(POINT_3D p, POINT_3D q);
	static POINT_3D pointTimes(float c, POINT_3D p);
	static POINT_3D makePoint(float a, float b, float c);

	static POINT_3D Bernstein(float u, POINT_3D *p);

};

class GLMathUtils
{
public:
	static const float _PI;

	GLMathUtils(void);
	~GLMathUtils(void);
	static float RadianToDegree(float Angle);
	static float DegreeToRadian(float Angle);
	
	static bool IsPowerOf2(int Value);
	static int MinPowerOf2(int Value);
};

#endif
