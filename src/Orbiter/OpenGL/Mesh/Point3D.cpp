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
#include "Point3D.h"

Point3D::Point3D(float X, float Y, float Z)
{
	this->X = X;
	this->Y = Y;
	this->Z = Z;
}

void Point3D::ApplyTransform(MeshTransform& Transform)
{
	float X = this->X * Transform.TransformMatrix[0 * 4 + 0] + 
		this->Y * Transform.TransformMatrix[1 * 4 + 0] + 
		this->Z * Transform.TransformMatrix[2 * 4 + 0] + 
		Transform.TransformMatrix[3 * 4 + 0];
	float Y = this->X * Transform.TransformMatrix[0 * 4 + 1] + 
		this->Y * Transform.TransformMatrix[1 * 4 + 1] + 
		this->Z * Transform.TransformMatrix[2 * 4 + 1] + 
		Transform.TransformMatrix[3 * 4 + 1];
	float Z = this->X * Transform.TransformMatrix[0 * 4 + 2] + 
		this->Y * Transform.TransformMatrix[1 * 4 + 2] + 
		this->Z * Transform.TransformMatrix[2 * 4 + 2] + 
		Transform.TransformMatrix[3 * 4 + 2];
	this->X = X;
	this->Y = Y;
	this->Z = Z;
}
