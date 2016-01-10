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
#include "Painter.h"

#include "TextureManager.h"
#include "GraphicImage.h"
#include "ExtensionManager.h"

#include <GL/gl.h>

#include <stdio.h>

Painter* Painter::Instance_ = NULL;

// #define DEBUG_LINES

/*static*/ Painter* Painter::Instance()
{
	if(Instance_ == NULL)
		Instance_ = new Painter();
	return Instance_;
}

void Painter::Destroy()
{
    delete Instance_;
	Instance_ = NULL;
}

Painter::Painter()
{
}

void Painter::Setup(ExtensionManager *ExtensionManager)
{
	ExtensionManager_ = ExtensionManager;
}

/*virtual*/ void Painter::PaintContainer(Container& Container, Transform& Transform)
{
	if(NULL == ExtensionManager_)
		throw "ExtensionManager_ not set!";

	ExtensionManager_->EnableBlended(true);

	int Count;
	Vertex* Vertexes = new Vertex[Container.NoVertexes];
	
	for(Count = 0; Count < Container.NoVertexes; Count++)
	{
		Vertexes[Count] = Container.Vertexes[Count];
		Vertexes[Count].ApplyTransform(Transform);
	}
#ifdef DEBUG
	//if(Container.NoTriangles)
		//DCE::LoggerWrapper::GetInstance()->Write(LV_WARNING, "GL_TRIANGLES");
#endif

	glBegin(GL_TRIANGLES);
	for(Count = 0; Count < Container.NoTriangles; Count++)
	{
		Triangle& Triangle = Container.Triangles[Count];

		TextureManager::Instance()->SetupTexture(Triangle.Texture ? Triangle.Texture->Texture : 0);

		if (Triangle.Texture)
		{
			float MaxU = Triangle.Texture->MaxU;
			float MaxV = Triangle.Texture->MaxV;

			Vertex& Vertex = Vertexes[Triangle.Vertex1];
			glTexCoord2f(Vertex.UVW.X*MaxU, Vertex.UVW.Y*MaxV);
			glColor4f(Vertex.Color.X, Vertex.Color.Y, Vertex.Color.Z, Vertex.Alpha);
			glVertex3f(Vertex.X, Vertex.Y, Vertex.Z);

			Vertex = Vertexes[Triangle.Vertex2];
			glTexCoord2f(Vertex.UVW.X*MaxU, Vertex.UVW.Y*MaxV);
			glColor4f(Vertex.Color.X, Vertex.Color.Y, Vertex.Color.Z, Vertex.Alpha);
			glVertex3f(Vertex.X, Vertex.Y, Vertex.Z);

			Vertex = Vertexes[Triangle.Vertex3];
			glTexCoord2f(Vertex.UVW.X*MaxU, Vertex.UVW.Y*MaxV);
			glColor4f(Vertex.Color.X, Vertex.Color.Y, Vertex.Color.Z, Vertex.Alpha);
			glVertex3f(Vertex.X, Vertex.Y, Vertex.Z);
		}
		else
		{
			Vertex& Vertex = Vertexes[Triangle.Vertex1];
			glColor4f(Vertex.Color.X, Vertex.Color.Y, Vertex.Color.Z, Vertex.Alpha);
			glVertex3f(Vertex.X, Vertex.Y, Vertex.Z);

			Vertex = Vertexes[Triangle.Vertex2];
			glColor4f(Vertex.Color.X, Vertex.Color.Y, Vertex.Color.Z, Vertex.Alpha);
			glVertex3f(Vertex.X, Vertex.Y, Vertex.Z);

			Vertex = Vertexes[Triangle.Vertex3];
			glColor4f(Vertex.Color.X, Vertex.Color.Y, Vertex.Color.Z, Vertex.Alpha);
			glVertex3f(Vertex.X, Vertex.Y, Vertex.Z);			
		}
	}
	glEnd();

#ifdef DEBUG_LINES
	for(Count = 0; Count < Container.NoTriangles; Count++) 
	{
		Triangle& Triangle = Container.Triangles[Count];

		glDisable(GL_TEXTURE_2D);
		glBegin(GL_LINE_STRIP);
		glColor3f(1, 1, 1);
		Vertex& Vertex = Vertexes[Triangle.Vertex1];
		glVertex3f(Vertex.X, Vertex.Y, Vertex.Z);
		Vertex = Vertexes[Triangle.Vertex2];
		glVertex3f(Vertex.X, Vertex.Y, Vertex.Z);

		Vertex = Vertexes[Triangle.Vertex3];
		glVertex3f(Vertex.X, Vertex.Y, Vertex.Z);

		Vertex = Vertexes[Triangle.Vertex1];
		glVertex3f(Vertex.X, Vertex.Y, Vertex.Z);

		glEnd();

	}
#endif

	delete Vertexes;	
}

void Painter::CleanUp()
{	
	delete Instance_;
	Instance_ = NULL;
}
