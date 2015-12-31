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
#ifndef MeshFrame_H_
#define MeshFrame_H_

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
#include "MeshContainer.h"
#include "MeshTransform.h"

#include <vector>

using namespace std;

class MeshFrame
{
	MeshContainer* m_pMeshContainer;
	MeshFrame* m_pParent;

	string m_sName;

	bool m_bVisible;
	bool m_bVolatile;
	bool m_bDontReleaseTexture;

public:
	//MOVE ME
	/**
	 *	Defines all transforms are for current geometry and to all childrens
	 */
	MeshTransform Transform;
	/**
	 *	Defines transforms over texturing coordinates
	 */
	MeshTransform TextureTransform;
	/**
	 *	Defines the list of childrens
	 */
	vector<MeshFrame*> Children;

	/**
	 *	Default constructor
	 */
	MeshFrame(string Name = "unknown", MeshContainer* Mesh = NULL);
	/**
	 *	Virtual destructor (warning: does not cleanup more than itself, for 
	 *	cleanup all you should use CleanUp method)
	 */
	virtual ~MeshFrame(void);

	/**
	 *	Clean from memory all geometry to itself and all childrens
	 */
	virtual void CleanUp(bool VolatilesOnly = false);
	/**
	 *	Setter for Mesh geometry that is contained to current MeshFrame
	 */
	void SetMeshContainer(MeshContainer* Mesh);
	/**
	 *	Setter for Mesh geometry that is contained to current MeshFrame
	 */
	MeshContainer* GetMeshContainer();

	/**
	 *	Add a child to the current object
	 */
	void AddChild(MeshFrame* Frame);
	/**
	 *	Search in it's childs and if finds it it removes it
	 *  @bDetachOnly - if true, the frame won't be deleted, if false, the frame will be deleted
	 */
	void RemoveChild(MeshFrame* Frame, bool bDetachOnly);
	/**
	 *	Replaces in it's childrens one frame with another
	 */
	MeshFrame* ReplaceChild(MeshFrame* OldFrame, MeshFrame* NewFrame);

	/**
	 *	Helper method using that paints the geometry to itself and to children
	 */
	virtual void Paint(MeshTransform ChildTransform);

	/**
	 *	Set current transform with a specified transform list
	 */
	virtual void SetTransform(MeshTransform& Transform);
	/**
	 *	Add to current transform a specified transform list
	 */
	virtual void ApplyTransform(MeshTransform& Transform);
	/**
	 *	Set current transform for UV coordinates
	 */
	virtual void SetTextureTransform(MeshTransform& TextureTransform);

	/**
	 *	Set the visible status for itself and it's childrens
	 */
	virtual void SetVisible(bool Visible);

	/**
	 *	Helper method using that paints the geometry to itself and to children
	 */
	virtual void Paint();

	/**
	 *	Set transparency level to itself and childrens that doesn't match the ExcludePattern
	 */
	virtual void SetAlpha(float Alpha, string ExcludePattern = "");
	/**
	 *	Set color recursively to itself and childrens
	 */
	virtual void SetColor(Point3D& Color);

	/**
	 *	Creates a clone of itself and childrens
	 */
	MeshFrame *Clone();
	/**
	 *	Change names for protect from deletion
	 */
	void Stealth();
	/**
	 *	Getter for Parent frame (if is one)
	 */
	MeshFrame *GetParent() { return m_pParent; }
	/**
	 *	Make current frame orphan
	 */
	void ResetParent() { m_pParent = NULL; }
	/**
	 *	Search a child from it's name
	 */
	MeshFrame *FindChild(string Name);

	/**
	 *	Displays on log it's frame and children structure
	 */
	void Print(string sIndent);
	/**
	 *	Getter of frame's name
	 */
	string Name() { return m_sName; }

	/**
	*	Check if operations performs correctly
	*/
	bool CheckIntegrity(MeshFrame *Frame);

	/**
	 *	Set volatile flag, means that should be deleted in special cleanup case
	 */
	void MarkAsVolatile();
	void MarkAsVolatileRecursively();
	/**
	 *	Check volatile flag
	 */
	bool IsVolatile() { return m_bVolatile; }
	/**
	 *    Is this mesh valid ?
	 */
	bool MeshIsValid();
};

#endif
