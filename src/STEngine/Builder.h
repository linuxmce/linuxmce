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
#ifndef Builder_H_
#define Builder_H_

#include "Container.h"

/**
 * MB_BMODE =  Builder BuildMode
 * MB_BMODE_TRIANGLES = classic mode, similar with glBegin(GL_Triangles)
 * 
 */ 
#define MBMODE_TRIANGLES			1
#define MBMODE_TRIANGLE_STRIP 		2

#include <vector>
#include "TextureManager.h"
#include "GraphicImage.h"
#include "Frame.h"

class Builder
{
	unsigned char BuildMode;
	bool Blended_;

	vector<Vertex> Vertexes;
	vector<Triangle>Triangles;
	
	float GlobalU, GlobalV, GlobalAlpha;
	float GlobalRed, GlobalGreen, GlobalBlue;
	GraphicImage* GlobalMaterial;
	
	virtual void BuildTriangle();
	virtual void AddTriangle(int Vertex1, int Vertex2, int Vertex3);
	
public:
	Builder(void);
	virtual ~Builder(void);
	
	virtual void CleanUp();
	
	virtual void Begin(unsigned char BuildMode);
	
	/**
	 * At the end of building of the primitive as result will be a Container
	 */ 
	virtual Container* End();
	
	virtual void AddVertexFloat(float X, float Y, float Z);
	virtual void AddVertexFloatUV(float X, float Y, float Z, float U, float V);

	virtual void SetTexture2D(float U, float V);
	virtual void SetTexture(GraphicImage* Texture);
	virtual void SetColor(float Red, float Green, float Blue);
	
	virtual void SetAlpha(float Alpha);
	virtual void SetBlended(bool Blended);
};

Frame* BuildRectangle(float X, float Y, float Width, float Heght, GraphicImage* Image);

#endif
