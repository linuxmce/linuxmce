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
#ifndef MeshContainer_H_
#define MeshContainer_H_

#include "MeshVertex.h"
#include "MeshTriangle.h"
#include "../OpenGLGraphic.h"

#include "../GLMathUtils.h"

/**
 *	Geometry class container. It offers informations which will be processed for painting
 *	by a MeshPainter
 */
class MeshContainer
{
public:
	//Public variables for fast access
	MeshVertex* Vertexes;
	int NoVertexes;
	MeshTriangle* Triangles;
	int NoTriangles;
	bool Blended_;

	/**
	 *	Default constructor, generates an empty container
	 */
	MeshContainer();
	/**
	 *	Cleanup destructor, it frees the contained geometry
	 */
	virtual ~MeshContainer();
	/**
	 *	Setup alpha blending to all vertexes, as result makes the entire geometry to a current alpha
	 */
	virtual void SetAlpha(float Alpha);
	/**
	 *	Set for all triangles one specified texture
	 */
	virtual void SetTexture(OpenGLGraphic* Texture);
	/**
	 *	Setup the specified color for all vertexes that makes the geometry
	 */
	void SetColor(Point3D& Color);
	/**
	 *	Scales the texturing coordinates values to the specified paremeter sizes
	 */
	virtual void SetUVMapping(FloatRect Rect);
	/**
	*	Makes a list with all texture mapped on its triangles and release them
	*/
	void DisposeTextures(bool bDestroyGraphics);
};

#endif 
