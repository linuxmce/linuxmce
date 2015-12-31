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
#ifndef MeshTransform_H_
#define MeshTransform_H_

class MeshTransform
{
public:
	/**
	 *	Linear 3D transform matrix (4 x 4 size)
	 */
	float TransformMatrix[16];
	/**
	 *	Default constructor, creates an Identity transform (no transform)
	 */
	MeshTransform(void);
	/**
	 *	Default destructor
	 */
	virtual ~MeshTransform(void);
	/**
	 *	Reset all transforms (setup as Identity transform)
	 */
	void SetIdentity();
	
	/**
	 *	Define a scale operation (removes all before transforms)
	 */
	void Scale(float X, float Y, float Z);
	/**
	 *	Define a scale operation (it adds over the old transforms)
	 */
	void ApplyScale(float X, float Y, float Z);
	
	/**
	 *	Define a translate operation (removes all before transforms)
	 */
	void Translate(float X, float Y, float Z);
	/**
	 *	Define a translate operation (it adds over the old transforms)
	 */
	void ApplyTranslate(float X, float Y, float Z);

	/**
	 *	Define a rotate operation over OX axis (removes all before transforms)
	 */
	void RotateX(float Angle);
	/**
	 *	Define a rotate operation over OX axis (it adds over the old transforms)
	 */
	void ApplyRotateX(float Angle);
	/**
	 *	Define a rotate operation over OY axis (removes all before transforms)
	 */
	void RotateY(float Angle);
	/**
	 *	Define a rotate operation over OY axis (it adds over the old transforms)
	 */
	void ApplyRotateY(float Angle);
	/**
	 *	Define a rotate operation over OZ axis (removes all before transforms)
	 */
	void RotateZ(float Angle);
	/**
	 *	Define a rotate operation over OZ axis (it adds over the old transforms)
	 */
	void ApplyRotateZ(float Angle);

	/**
	 *	Combine the current transform with another. 
	 *	@param Transform That parameter will add it's transformations to the end of
	 *	current transforms
	 */
	void ApplyTransform(const MeshTransform& Transform);

	/**
	 *	Copy all transformations from another MeshTransform
	 */
	void CopyFrom(const MeshTransform& Transform);

	/*
	 *	Operator that combine the current transform with another. 
	 *	@param Transform That parameter will add it's transformations to the end of
	 *	current transforms
	 */
	void operator*= (const MeshTransform& Transform);
	/**
	 *	Operator that copy all transformations from another MeshTransform
	 */
	void operator= (const MeshTransform& Transform);
};

#endif
