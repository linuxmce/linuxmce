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
#include "MeshContainer.h"

#include "../GLMathUtils.h"
#include "../OpenGLGraphic.h"

#include <map>

MeshContainer::MeshContainer(void) : Vertexes(NULL), NoVertexes(0), Triangles(NULL), NoTriangles(0), Blended_(false)
{
}

MeshContainer::~MeshContainer(void)
{
	delete [] Vertexes;
	delete [] Triangles;
}

void MeshContainer::DisposeTextures(bool bDestroyGraphics)
{
	map<OpenGLGraphic *, bool> mapTextures;

	for(int i = 0; i < NoTriangles; i++)
	{
		if(NULL != Triangles[i].Texture)
		{
			mapTextures[Triangles[i].Texture] = true;
			Triangles[i].Texture = NULL;
		}
	}

	for(map<OpenGLGraphic *, bool>::iterator it = mapTextures.begin(); it != mapTextures.end(); ++it)
	{
		OpenGLGraphic *pGraphic = it->first;
		
		if(bDestroyGraphics)
			RELEASE_GRAPHIC(pGraphic)
		else if(TextureManager::Instance()->IsSafeToReleaseTextures())
			pGraphic->Clear();
	}

	mapTextures.clear();
}

void MeshContainer::SetAlpha(float Alpha)
{
	for(int i = 0; i < NoVertexes; i++)
		Vertexes[i].Alpha = Alpha;
}

void MeshContainer::SetTexture(OpenGLGraphic* Texture)
{
	for(int i = 0; i < NoTriangles; i++)
		Triangles[i].Texture = 	Texture;
}

/*virtual*/ void MeshContainer::SetUVMapping(FloatRect Rect)
{
	for(int i = 0; i < NoVertexes; i++)
	{
		Vertexes[i].UVW.X = MathUtils::InterpolateValues(Rect.Left, Rect.Left + Rect.Width, 
			Vertexes[i].UVW.X);
		Vertexes[i].UVW.Y = MathUtils::InterpolateValues(Rect.Top, Rect.Top + Rect.Height, 
			Vertexes[i].UVW.Y);
	}
}

void MeshContainer::SetColor(Point3D& Color)
{
	for(int i = 0; i < NoVertexes; i++)
	{
		Vertexes[i].Color = Color;
	}

}

