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
#ifndef Point3D_H_
#define Point3D_H_

#include "MeshTransform.h"

/**
 *	Class that defines a 3D coordinate
 */
class Point3D
{
public:
	/**
	 *	Coordinates that match the OX, OY, OZ value
	 */
	float X, Y, Z;
	/**
	 *	Applies a list of transformations over the 3D coordinate of one point 3D
	 */
	void ApplyTransform(MeshTransform& Transform);

	/**
	 *	Default constructor
	 */
	Point3D(float X = 0.0f, float Y = 0.0f, float Z = 0.0f);

};

#endif
