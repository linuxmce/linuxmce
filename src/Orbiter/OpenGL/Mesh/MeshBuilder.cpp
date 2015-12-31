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
#include "MeshBuilder.h"

#include "../GLMathUtils.h"

MeshBuilder::MeshBuilder(void) : Blended_(true/*false*/)
{
	this->GlobalRed = 1.0f;
	this->GlobalGreen = 1.0f;
	this->GlobalBlue = 1.0f;
	this->GlobalAlpha = 1.0f;
	this->GlobalU = 1.0f;
	this->GlobalV = 1.0f;
	this->GlobalMaterial = NULL;
}

MeshBuilder::~MeshBuilder(void)
{
	CleanUp();
}

void MeshBuilder::CleanUp()
{
	Vertexes.clear();
	Triangles.clear();	
}

void MeshBuilder::Begin(unsigned char BuildMode)
{
	CleanUp();
	this->BuildMode = BuildMode;
}

/*virtual*/ MeshContainer* MeshBuilder::End()
{
	int Count;
	
	MeshContainer* Result = new MeshContainer();
	Result->NoVertexes = int(Vertexes.size());
	Result->Vertexes = new MeshVertex[Result->NoVertexes];
	
	for(Count = 0; Count < Result->NoVertexes; Count++)
		Result->Vertexes[Count] = Vertexes[Count];

	Result->NoTriangles = int(Triangles.size());
	Result->Triangles = new MeshTriangle[Result->NoTriangles];
	for(Count = 0; Count < Result->NoTriangles; Count++)
		Result->Triangles[Count] = Triangles[Count];
			
	Result->Blended_ = Blended_;

	CleanUp();
	return Result;
}

/*virtual*/ void MeshBuilder::AddVertexFloat(float X, float Y, float Z)
{
	MeshVertex Vertex;
	Vertex.X = X; Vertex.Y = Y; Vertex.Z = Z; 
	Vertex.UVW.X = GlobalU; Vertex.UVW.Y = GlobalV; 
	Vertex.Alpha = GlobalAlpha;
	Vertex.Color.X = GlobalRed; Vertex.Color.Y = GlobalGreen; Vertex.Color.Z = GlobalBlue;
	Vertexes.push_back(Vertex);
	BuildTriangle();
}

/*virtual*/ void MeshBuilder::AddVertexFloatUV(float X, float Y, float Z, float U, float V)
{
	MeshVertex Vertex;
	Vertex.X = X; Vertex.Y = Y; Vertex.Z = Z; 
	Vertex.UVW.X = U; Vertex.UVW.Y = V; 
	Vertex.Alpha = GlobalAlpha;
	Vertex.Color.X = GlobalRed; Vertex.Color.Y = GlobalGreen; Vertex.Color.Z = GlobalBlue;
	
	Vertexes.push_back(Vertex);
	BuildTriangle();
}

/*virtual*/ void MeshBuilder::BuildTriangle()
{
	int NoVertexes = int(Vertexes.size());
	switch(BuildMode)
	{
	case MBMODE_TRIANGLES:
		if(NoVertexes % 3 == 0)
			AddTriangle(
				NoVertexes-3,		
				NoVertexes-2, 
				NoVertexes-1);	
		
	break;
	case MBMODE_TRIANGLE_STRIP:
		if(NoVertexes >= 3)
			if(NoVertexes % 2 == 0)
			AddTriangle(
				NoVertexes-3,
				NoVertexes-2, 
				NoVertexes-1);
			else
				AddTriangle(
				NoVertexes-3,
				NoVertexes-1,
				NoVertexes-2
				);

	break;
	}
}

/*virtual*/ void MeshBuilder::AddTriangle(int Vertex1, int Vertex2, int Vertex3)
{
	MeshTriangle Triangle;
	Triangle.Vertex1 = Vertex1;
	Triangle.Vertex2 = Vertex2;
	Triangle.Vertex3 = Vertex3;
	Triangle.Texture = GlobalMaterial;
	Triangles.push_back(Triangle);
}

/*virtual*/ void MeshBuilder::SetTexture2D(float U, float V)
{
	GlobalU = U;
	GlobalV = V;
}
/*virtual*/ void MeshBuilder::SetTexture(OpenGLGraphic*Texture)
{
	GlobalMaterial = Texture;	
}

/*virtual*/ void MeshBuilder::SetColor(float Red, float Green, float Blue)
{
	this->GlobalRed = Red;	
	this->GlobalGreen = Green;
	this->GlobalBlue = Blue;
}

/*virtual*/ void MeshBuilder::SetAlpha(float Alpha)
{
	this->GlobalAlpha = Alpha;
	SetBlended(true);
}

/*virtual*/ void MeshBuilder::SetBlended(bool Blended)
{
	Blended_ = Blended;
}


/*static*/ MeshContainer* MeshBuilder::BuildRectangle(PlutoRectangle& Rectangle, 
OpenGLGraphic* Graphic)
{
	MeshBuilder MB;
	MB.Begin(MBMODE_TRIANGLE_STRIP);
	MB.SetColor(1.0f, 1.0f, 1.0f);

	MB.SetTexture(Graphic);

	MB.SetTexture2D(0, 0);
	MB.AddVertexFloat((float)Rectangle.Left(), (float)Rectangle.Top(), 0);

	MB.SetTexture2D(1, 0);
	MB.AddVertexFloat((float)Rectangle.Right(), (float)Rectangle.Top(), 0);

	MB.SetTexture2D(0, 1);
	MB.AddVertexFloat((float)Rectangle.Left(), (float)Rectangle.Bottom(), 0);

	MB.SetTexture2D(1, 1);
	MB.AddVertexFloat((float)Rectangle.Right(), (float)Rectangle.Bottom(), 0);

	return MB.End();

}

/*static*/ MeshContainer* MeshBuilder::BuildArrow(PlutoPoint p1, PlutoPoint p2, PlutoSize sizeArrow,
	PlutoPoint& arrow_p1, PlutoPoint& arrow_p2)
{
	MeshBuilder MB;
	MB.Begin(MBMODE_TRIANGLE_STRIP);
	MB.SetColor(1.0f, 1.0f, 1.0f);
	
    const float ratio = 0.15f; //the 1/2 ratio between arrow's tail width and head width
	PlutoPoint head_middle(static_cast<int>(arrow_p2.X - 0.5f * (arrow_p2.X - arrow_p1.X)), static_cast<int>(arrow_p2.Y - 0.5f * (arrow_p2.Y - arrow_p1.Y)));
	PlutoPoint head_p1(static_cast<int>(head_middle.X - ratio * (arrow_p2.X - arrow_p1.X)), static_cast<int>(head_middle.Y - ratio * (arrow_p2.Y - arrow_p1.Y)));
	PlutoPoint head_p2(static_cast<int>(head_middle.X + ratio * (arrow_p2.X - arrow_p1.X)), static_cast<int>(head_middle.Y + ratio * (arrow_p2.Y - arrow_p1.Y)));
	PlutoPoint tail_p1(static_cast<int>(p1.X - ratio * (arrow_p2.X - arrow_p1.X)), static_cast<int>(p1.Y - ratio * (arrow_p2.Y - arrow_p1.Y)));
	PlutoPoint tail_p2(static_cast<int>(p1.X + ratio * (arrow_p2.X - arrow_p1.X)), static_cast<int>(p1.Y + ratio * (arrow_p2.Y - arrow_p1.Y)));

	float max_x = float(max(max(p1.X, p2.X), max(arrow_p1.X, arrow_p2.X)));
	float max_y = float(max(max(p1.Y, p2.Y), max(arrow_p1.Y, arrow_p2.Y)));
	float min_x = float(min(min(p1.X, p2.X), min(arrow_p1.X, arrow_p2.X)));
	float min_y = float(min(min(p1.Y, p2.Y), min(arrow_p1.Y, arrow_p2.Y)));

	//the arrow head
	MB.SetTexture2D(arrow_p1.X / (max_x - min_x), arrow_p1.Y / (max_y - min_y));
	MB.AddVertexFloat((float)arrow_p1.X, (float)arrow_p1.Y, 0);

	MB.SetTexture2D(arrow_p2.X / (max_x - min_x), arrow_p2.Y / (max_y - min_y));
	MB.AddVertexFloat((float)arrow_p2.X, (float)arrow_p2.Y, 0);	

	MB.SetTexture2D(p2.X / (max_x - min_x), p2.Y / (max_y - min_y));
	MB.AddVertexFloat((float)p2.X, (float)p2.Y, 0);

	//extra dummy vertexe
	MB.SetTexture2D(head_p1.X / (max_x - min_x), head_p1.Y / (max_y - min_y));
	MB.AddVertexFloat((float)head_p1.X, (float)head_p1.Y, 0);

	//the arrow tail
	MB.SetTexture2D(head_p1.X / (max_x - min_x), head_p1.Y / (max_y - min_y));
	MB.AddVertexFloat((float)head_p1.X, (float)head_p1.Y, 0);

	MB.SetTexture2D(tail_p1.X / (max_x - min_x), tail_p1.Y / (max_y - min_y));
	MB.AddVertexFloat((float)tail_p1.X, (float)tail_p1.Y, 0);

	MB.SetTexture2D(head_p2.X / (max_x - min_x), head_p2.Y / (max_y - min_y));
	MB.AddVertexFloat((float)head_p2.X, (float)head_p2.Y, 0);

	MB.SetTexture2D((tail_p2.X) / (max_x - min_x), tail_p2.Y / (max_y - min_y));
	MB.AddVertexFloat((float)tail_p2.X, (float)tail_p2.Y, 0);

	MB.SetTexture2D(head_p1.X / (max_x - min_x), head_p1.Y / (max_y - min_y));
	MB.AddVertexFloat((float)head_p1.X, (float)head_p1.Y, 0);

	return MB.End();
}
