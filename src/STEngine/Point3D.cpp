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
#include "Point3D.h"

void Point3D::ApplyTransform(Transform& pTransform)
{
	float X = this->X * pTransform.TransformMatrix[0 * 4 + 0] + 
		this->Y * pTransform.TransformMatrix[1 * 4 + 0] + 
		this->Z * pTransform.TransformMatrix[2 * 4 + 0] + 
		pTransform.TransformMatrix[3 * 4 + 0];
	float Y = this->X * pTransform.TransformMatrix[0 * 4 + 1] + 
		this->Y * pTransform.TransformMatrix[1 * 4 + 1] + 
		this->Z * pTransform.TransformMatrix[2 * 4 + 1] + 
		pTransform.TransformMatrix[3 * 4 + 1];
	float Z = this->X * pTransform.TransformMatrix[0 * 4 + 2] + 
		this->Y * pTransform.TransformMatrix[1 * 4 + 2] + 
		this->Z * pTransform.TransformMatrix[2 * 4 + 2] + 
		pTransform.TransformMatrix[3 * 4 + 2];
	this->X = X;
	this->Y = Y;
	this->Z = Z;
}
