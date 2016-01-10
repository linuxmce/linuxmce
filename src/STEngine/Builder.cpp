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
#include "Builder.h"

#include "MathUtils.h"
#include "Frame.h"

Builder::Builder(void) : Blended_(true/*false*/)
{
	this->GlobalRed = 1.0f;
	this->GlobalGreen = 1.0f;
	this->GlobalBlue = 1.0f;
	this->GlobalAlpha = 1.0f;
	this->GlobalU = 1.0f;
	this->GlobalV = 1.0f;
	this->GlobalMaterial = NULL;
}

Builder::~Builder(void)
{
	CleanUp();
}

void Builder::CleanUp()
{
	Vertexes.clear();
	Triangles.clear();	
}

void Builder::Begin(unsigned char BuildMode)
{
	CleanUp();
	this->BuildMode = BuildMode;
}

/*virtual*/ Container* Builder::End()
{
	int Count;
	
	Container* Result = new Container();
	Result->NoVertexes = int(Vertexes.size());
	Result->Vertexes = new Vertex[Result->NoVertexes];
	
	for(Count = 0; Count < Result->NoVertexes; Count++)
		Result->Vertexes[Count] = Vertexes[Count];

	Result->NoTriangles = int(Triangles.size());
	Result->Triangles = new Triangle[Result->NoTriangles];
	for(Count = 0; Count < Result->NoTriangles; Count++)
		Result->Triangles[Count] = Triangles[Count];
			
	Result->Blended_ = Blended_;

	CleanUp();
	return Result;
}

/*virtual*/ void Builder::AddVertexFloat(float X, float Y, float Z)
{
	Vertex Vertex;
	Vertex.X = X; Vertex.Y = Y; Vertex.Z = Z; 
	Vertex.UVW.X = GlobalU; Vertex.UVW.Y = GlobalV; 
	Vertex.Alpha = GlobalAlpha;
	Vertex.Color.X = GlobalRed; Vertex.Color.Y = GlobalGreen; Vertex.Color.Z = GlobalBlue;
	Vertexes.push_back(Vertex);
	BuildTriangle();
}

/*virtual*/ void Builder::AddVertexFloatUV(float X, float Y, float Z, float U, float V)
{
	Vertex Vertex;
	Vertex.X = X; Vertex.Y = Y; Vertex.Z = Z; 
	Vertex.UVW.X = U; Vertex.UVW.Y = V; 
	Vertex.Alpha = GlobalAlpha;
	Vertex.Color.X = GlobalRed; Vertex.Color.Y = GlobalGreen; Vertex.Color.Z = GlobalBlue;
	
	Vertexes.push_back(Vertex);
	BuildTriangle();
}

/*virtual*/ void Builder::BuildTriangle()
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

/*virtual*/ void Builder::AddTriangle(int Vertex1, int Vertex2, int Vertex3)
{
	Triangle Triangle;
	Triangle.Vertex1 = Vertex1;
	Triangle.Vertex2 = Vertex2;
	Triangle.Vertex3 = Vertex3;
	Triangle.Texture = GlobalMaterial;
	Triangles.push_back(Triangle);
}

/*virtual*/ void Builder::SetTexture2D(float U, float V)
{
	GlobalU = U;
	GlobalV = V;
}
/*virtual*/ void Builder::SetTexture(GraphicImage*Texture)
{
	GlobalMaterial = Texture;	
}

/*virtual*/ void Builder::SetColor(float Red, float Green, float Blue)
{
	this->GlobalRed = Red;	
	this->GlobalGreen = Green;
	this->GlobalBlue = Blue;
}

/*virtual*/ void Builder::SetAlpha(float Alpha)
{
	this->GlobalAlpha = Alpha;
	SetBlended(true);
}

/*virtual*/ void Builder::SetBlended(bool Blended)
{
	Blended_ = Blended;
}

Frame* BuildRectangle(float X, float Y, float Width, float Heght, GraphicImage* Image)
{
	Builder MB;
	MB.Begin(MBMODE_TRIANGLE_STRIP);
	MB.SetColor(1.0f, 1.0f, 1.0f);

	MB.SetTexture(Image);

	MB.SetTexture2D(0, 0);
	MB.AddVertexFloat(X, Y, 0);

	MB.SetTexture2D(1, 0);
	MB.AddVertexFloat(X+Width, Y, 0);

	MB.SetTexture2D(0, 1);
	MB.AddVertexFloat(X, Y+Heght, 0);

	MB.SetTexture2D(1, 1);
	MB.AddVertexFloat(X+Width, Y+Heght, 0);

	Frame* Result = new Frame("picture", MB.End());
	return Result;
}
