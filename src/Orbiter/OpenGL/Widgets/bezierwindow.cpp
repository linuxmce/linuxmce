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
#include "bezierwindow.h"

#include <GL/gl.h>

#include "../Mesh/MeshBuilder.h"
#include "../Mesh/MeshFrame.h"

MeshContainer* genBezier(BEZIER_PATCH patch, FloatRect TextureWrapper2D, ColorRGB Background,
					OpenGLGraphic* BackgroundTexture) 
{
	MeshContainer* Container = NULL;
	int divs = patch.Divisions;
	int		u = 0, v;
	double		py, px, pyold; 
	POINT_3D	temp[4];
	POINT_3D	*last = (POINT_3D*)malloc(sizeof(POINT_3D)*(divs+1));


	float CoordU, CoordV;

	// Array Of Points To Mark The First Line Of Polys
	temp[0] = patch.anchors[0][3];					// The First Derived Curve (Along X-Axis)
	temp[1] = patch.anchors[1][3];
	temp[2] = patch.anchors[2][3];
	temp[3] = patch.anchors[3][3];

	for (v=0;v<=divs;v++) {						// Create The First Line Of Points
		px = ((double)v)/((double)divs);				// Percent Along Y-Axis
		// Use The 4 Points From The Derived Curve To Calculate The Points Along That Curve
		last[v] = MathUtils::Bernstein((float)px, temp);
	}



	MeshBuilder MB;
	MB.SetColor(1.0f, 1.0f, 1.0f);
	MB.SetAlpha(Background.Alpha);
	MB.SetTexture(BackgroundTexture);

	MB.Begin(MBMODE_TRIANGLE_STRIP);

	for (u=1;u<=divs;u++) {
		py    = ((double)u)/((double)divs);			// Percent Along Y-Axis
		pyold = ((double)u-1.0)/((double)divs);			// Percent Along Old Y Axis

		temp[0] = MathUtils::Bernstein((float)py, patch.anchors[0]);		// Calculate New Bezier Points
		temp[1] = MathUtils::Bernstein((float)py, patch.anchors[1]);
		temp[2] = MathUtils::Bernstein((float)py, patch.anchors[2]);
		temp[3] = MathUtils::Bernstein((float)py, patch.anchors[3]);

		for (v=0;v<=divs;v++) {
			px = ((float)v)/((float)divs);			// Percent Along The X-Axis

			CoordU = (float)MathUtils::InterpolateValues(TextureWrapper2D.Left, 
				TextureWrapper2D.Left+TextureWrapper2D.Width, 
				1.0f-(float)px);
			CoordV = (float)MathUtils::InterpolateValues(TextureWrapper2D.Top, 
				TextureWrapper2D.Top+TextureWrapper2D.Height, 
				1-(float)pyold);

			MB.AddVertexFloatUV(last[v].x, last[v].y, last[v].z, 
				CoordU,
				CoordV); 
			
			last[v] = MathUtils::Bernstein((float)px, temp);			// Generate New Point

			CoordU = (float)MathUtils::InterpolateValues(TextureWrapper2D.Left, 
				TextureWrapper2D.Left+TextureWrapper2D.Width, 
				1-(float)px);
			CoordV = (float)MathUtils::InterpolateValues(TextureWrapper2D.Top, 
				TextureWrapper2D.Top+TextureWrapper2D.Height, 
				1-(float)py);

			MB.AddVertexFloatUV(last[v].x, last[v].y, last[v].z, 
				CoordU, 
				CoordV); 
		}
	}
	free(last);							// Free The Old Vertices Array

	Container = MB.End();

	return Container;
}

TBezierWindow::TBezierWindow(MeshFrame* ParentContext,
						int Left, 
						int Top, 
						int Width, 
						int Height, 
						std::string Text)
: TBaseWidget(ParentContext, Left, Top, Width, Height, Text)
{
	BezierDefinition.Divisions = 7;
	Frame = new MeshFrame("bezier window");
	Frame->SetMeshContainer(Container);
	Context = ParentContext;
	Context->AddChild(Frame);	
}

TBezierWindow::~TBezierWindow() {
	Context->RemoveChild(Frame, true);
	Frame->CleanUp();
	delete Frame;
}


void TBezierWindow::Paint()
{
	if (!Visible)
		return;
		
	TBaseWidget::Paint();
	
	if(Container)
		delete Container;
	Container = genBezier(BezierDefinition, TextureWrapper2D, Background, 
		BackgroundTex);
	Frame->SetMeshContainer(Container);
} 

bool TBezierWindow::SetBackgroundImage(OpenGLGraphic* Background)
{
	// if wants to be set an invalid texture, the function will return false
	if(Background<0)
		return false;
		
	// set up the texture as texture background
	BackgroundTex = Background;
	
	return true;
}

void TBezierWindow::SetRectCoordinates(FloatRect Coordinates)
{
	SetLeft((int)Coordinates.Left);
	SetTop((int)Coordinates.Top);
	SetWidth((int)Coordinates.Width);
	SetHeight((int)Coordinates.Height);

	for(int x = 0; x<4; x++)
		for(int y = 0; y<4; y++)
		{
			BezierDefinition.anchors[x][y].x = MathUtils::InterpolateValues(float(Left), 
					float(Left + Width), x/3.0f);
			BezierDefinition.anchors[x][y].y = MathUtils::InterpolateValues(float(Top), 
				float(Top + Height), y/3.0f);
			BezierDefinition.anchors[x][y].z = 0;
		}
}

	/** Set the UV texture coordinates of the texture. That will make the texture to be streched without any 
	  geometry modification. Too is used to fix the non power of 2 texture size. 
	 */
void TBezierWindow::SetTextureWraping(float LeftCoordinate, float TopCoordinate, 
	float WidthCoordinate, float HeightCoordinate) {
	TextureWrapper2D.Left = LeftCoordinate;
	TextureWrapper2D.Top = 1-TopCoordinate;
	TextureWrapper2D.Width = WidthCoordinate;
	TextureWrapper2D.Height = -HeightCoordinate;
}

