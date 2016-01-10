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
#include "Transform.h"

#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include "MathUtils.h"

Transform::Transform(void)
{
	this->SetIdentity();
}

Transform::~Transform(void)
{
}

void Transform::SetIdentity(void)
{
	int i;
	for(i = 0; i<16; i++)
		TransformMatrix[i] = 0.0f;
	for(i = 0; i<4; i++)
		TransformMatrix[i*5] = 1.0f;
}

void Transform::Translate(float X, float Y, float Z)
{
	SetIdentity();
	TransformMatrix[3 * 4 + 0] = X;
	TransformMatrix[3 * 4 + 1] = Y;
	TransformMatrix[3 * 4 + 2] = Z;
}

void Transform::ApplyTranslate(float X, float Y, float Z)
{
	Transform Transform;
	Transform.Translate(X, Y, Z);
	ApplyTransform(Transform);
}

void Transform::Scale(float X, float Y, float Z)
{
	SetIdentity();
	TransformMatrix[0 * 4 + 0] = X;
	TransformMatrix[1 * 4 + 1] = Y;
	TransformMatrix[2 * 4 + 2] = Z;
}

void Transform::ApplyScale(float X, float Y, float Z)
{
	Transform Transform;
	Transform.Scale(X, Y, Z);
	ApplyTransform(Transform);
}

void Transform::RotateX(float Angle)
{
	SetIdentity();
	float RadAngle = DegreeToRadian(Angle);
	float S, C;
	S = sin(RadAngle); 
	C = cos(RadAngle);
	TransformMatrix[1 * 4 + 1] = C;
	TransformMatrix[2 * 4 + 2] = C;
	TransformMatrix[1 * 4 + 2] = -S;
	TransformMatrix[2 * 4 + 1] = S; 
}

void Transform::ApplyRotateX(float Angle)
{
	Transform Transform;
	Transform.RotateX(Angle);
	ApplyTransform(Transform);
}

void Transform::RotateY(float Angle)
{
	SetIdentity();
	float RadAngle = DegreeToRadian(Angle);
	float S, C;
	S = sin(RadAngle); 
	C = cos(RadAngle);
	TransformMatrix[0 * 4 + 0] = C;
	TransformMatrix[2 * 4 + 2] = C;
	TransformMatrix[0 * 4 + 2] = -S;
	TransformMatrix[2 * 4 + 0] = S; 
}

void Transform::ApplyRotateY(float Angle)
{
	Transform Transform;
	Transform.RotateY(Angle);
	ApplyTransform(Transform);
}

void Transform::RotateZ(float Angle)
{
	SetIdentity();
	float RadAngle = DegreeToRadian(Angle);
	float S, C;
	S = sin(RadAngle); 
	C = cos(RadAngle);
	TransformMatrix[0 * 4 + 0] = C;
	TransformMatrix[1 * 4 + 1] = C;
	TransformMatrix[0 * 4 + 1] = -S;
	TransformMatrix[1 * 4 + 0] = S; 
}

void Transform::ApplyRotateZ(float Angle)
{
	Transform Transform;
	Transform.RotateZ(Angle);
	ApplyTransform(Transform);
}

void Transform::ApplyTransform(Transform& pTransform)
{
	int M, N, P;
	Transform Final;
	for(M = 0; M < 4; M++)
	{
		for(P = 0; P < 4; P++)
		{
			Final.TransformMatrix[M * 4 + P] = 0.0f;
			for(N = 0; N < 4; N++)
			{
				Final.TransformMatrix[M * 4 + P] += 
					TransformMatrix[M * 4 + N] * pTransform.TransformMatrix[N * 4 + P];
			}
		}
	}
	CopyFrom(Final);
}

void Transform::CopyFrom(const Transform& Transform)
{
	for(int i = 0; i<16; i++)
		this->TransformMatrix[i] = Transform.TransformMatrix[i];
}

void Transform::operator= (const Transform& Transform)
{
	CopyFrom(Transform);
}
