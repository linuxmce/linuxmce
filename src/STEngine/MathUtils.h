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
#ifndef MathUtils_H_
#define MathUtils_H_

int MinPowerOf2(int Value);
float DegreeToRadian(float Angle);

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
	* That function say ain intermediar rectangle from source rectangle
	* and one another dest point using a given value 
	* for Value = 0 the result is the source rectangle
	* for Value = 1 the result is destination rectangle
	* for Value = 0.5 the result is the middle of both rectangles
	*/
	FloatRect Interpolate(FloatRect DestRect, float Value);	

};

float InterpolateValues(float Source, float Dest, float Theta);

int RandomInInterval(int BeginValue, int EndValue);

#endif
