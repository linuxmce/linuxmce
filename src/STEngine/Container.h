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
#ifndef Container_H_
#define Container_H_

#include "Vertex.h"
#include "Triangle.h"
#include "GraphicImage.h"

class Container
{
public:
	Vertex* Vertexes;
	int NoVertexes;
	Triangle* Triangles;
	int NoTriangles;
	bool Blended_;

	Container();
	virtual ~Container();
	
	virtual void SetAlpha(float Alpha);
	virtual void SetTexture(GraphicImage* Texture);
	void SetColor(Point3D& Color);

	Container* Clone();
};

#endif 
