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
#ifndef MeshBuilder_H_
#define MeshBuilder_H_

#include "MeshContainer.h"

/**
 * MB_BMODE = Mesh Builder BuildMode
 * MB_BMODE_TRIANGLES = classic mode, similar with glBegin(GL_Triangles)
 * 
 */ 
#define MBMODE_TRIANGLES			1
#define MBMODE_TRIANGLE_STRIP 		2

#include <vector>
#include "../Texture/TextureManager.h"
#include "../OpenGLGraphic.h"

/**
 *	Helper class that creates a MeshContainer based by a drawing command list.
 *	Calling the End() method will output the current pending geometry
 */
class MeshBuilder
{
	unsigned char BuildMode;
	bool Blended_;

	vector<MeshVertex> Vertexes;
	vector<MeshTriangle>Triangles;
	
	float GlobalU, GlobalV, GlobalAlpha;
	float GlobalRed, GlobalGreen, GlobalBlue;
	OpenGLGraphic* GlobalMaterial;
	
	virtual void BuildTriangle();
	virtual void AddTriangle(int Vertex1, int Vertex2, int Vertex3);
	
public:
	/**
	 *	Default constructor, setup the default color, etc.
	 */
	MeshBuilder();
	/**
	 *	Default destructor, cleanups the pending drawings, etc.
	 */
	virtual ~MeshBuilder(void);

	/**
	 *	CleanUp does clean the all pending lists	
	 */	
	virtual void CleanUp();
	
	/**
	 *	Starts an empty list in a specified BuildMode:
	 *	Triangles and TriangleStrip
	 *	MBMODE_TRIANGLES creates a triangle at every 3 points: 3n-2, 3n-1, 3n
	 *	MBMODE_TRIANGLE_STRIP creates a triangle at every new point: n-2, n-1, n, after first three points
	 */
	virtual void Begin(unsigned char BuildMode);
	
	/**
	 * At the end of building of the primitive as result will be a MeshContainer
	 */ 
	virtual MeshContainer* End();
	
	/**
	 *	Add a vertex to the drawing list, it will use current UV and color
	 */
	virtual void AddVertexFloat(float X, float Y, float Z);
	/**
	 *	Add a vertex with UV to the drawing list, it will use current color
	 */
	virtual void AddVertexFloatUV(float X, float Y, float Z, float U, float V);
	/**
	 *	Setup default UV (texturing coordinate) for drawing
	 */
	virtual void SetTexture2D(float U, float V);
	/**
	 *	Set default texture for drawing
	 */
	virtual void SetTexture(OpenGLGraphic* Texture);
	/**
	 *	Set default color for drawing
	 */
	virtual void SetColor(float Red, float Green, float Blue);
	
	/**
	 *	Set current alpha blending level 
	 */
	virtual void SetAlpha(float Alpha);
	/**
	 *	Set transparency level
	 */
	virtual void SetBlended(bool Blended);

	/**
	 *	Helper class to create a rectangle/graphic. If Graphic is NULL, there will be a white rectangle	
	 */
	static MeshContainer* BuildRectangle(PlutoRectangle& Rectangle, 
		OpenGLGraphic* Graphic);

	/**
	 *	Helper class to create an arrow
	 */
	static MeshContainer* BuildArrow(PlutoPoint p1, PlutoPoint p2, PlutoSize sizeArrow,
		PlutoPoint& arrow_p1, PlutoPoint& arrow_p2);
};

#endif
