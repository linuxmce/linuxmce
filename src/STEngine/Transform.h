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
#ifndef Transform_H_
#define Transform_H_

class Transform
{
public:
	/**
	*	Linear 3D transform matrix (4 x 4 size)
	*/
	float TransformMatrix[16];

	Transform(void);
	virtual ~Transform(void);
	void SetIdentity();
	
	void Scale(float X, float Y, float Z);
	void ApplyScale(float X, float Y, float Z);
	
	void Translate(float X, float Y, float Z);
	void ApplyTranslate(float X, float Y, float Z);

	void RotateX(float Angle);
	void ApplyRotateX(float Angle);
	void RotateY(float Angle);
	void ApplyRotateY(float Angle);
	void RotateZ(float Angle);
	void ApplyRotateZ(float Angle);
	void ApplyTransform(Transform& Transform);

	void CopyFrom(const Transform& Transform);

	void operator= (const Transform& Transform);
};

#endif
