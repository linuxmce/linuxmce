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
#include "GL2DEffectLayersCompose.h"

#include <iostream>

#include "../OpenGLGraphic.h"
#include "../Mesh/MeshPainter.h"

#include "../ExtensionManager.h"

namespace GLEffect2D 
{

LayersCompose* LayersCompose::Instance_ = NULL;

LayersCompose::LayersCompose() : 
	TextureMan(NULL),
	CurrentLayer(NULL), 
	OldLayer(NULL),
	NeedUpdateLayers(true),
	OldScreen(NULL),
	NewScreen(NULL)
{
}

/*virtual*/ LayersCompose::~LayersCompose(void)
{
	delete OldScreen;
	OldScreen = NULL;
	delete NewScreen;
	NewScreen = NULL;

	RemoveLayer(3);
	RemoveLayer(2);
}

/*static*/ LayersCompose* LayersCompose::Instance()
{
	if(Instance_ == NULL)
		Instance_ = new LayersCompose();
	return Instance_;
}

void LayersCompose::Setup(int Width, int Height)
{
	this->Width = Width;
	this->Height = Height;
	TextureMan = TextureManager::Instance();
	CreateLayer(2);
	CreateLayer(3);
	NewScreen = new GLRenderScreenToGraphic(Width, Height);
	OldScreen = new GLRenderScreenToGraphic(Width, Height);
}	

void LayersCompose::CleanUp()
{
	delete Instance_;
	Instance_ = NULL;
}

bool LayersCompose::ExistsLayerLevel(int LayerLevel)
{
	return (LayerList.find(LayerLevel) != LayerList.end());
}

Layer* LayersCompose::CreateLayer(int LayerLevel)
{
	Layer* Result = new Layer(Width, Height);
	LayerList[LayerLevel] = Result;
	return Result;
}

bool LayersCompose::RemoveLayer(int LayerLevel)
{
	if(!ExistsLayerLevel(LayerLevel))
		return false;

	std::map <int, Layer*>::iterator Item = LayerList.begin();
	while(Item != LayerList.end())
	{
		if(Item->first == LayerLevel)
		{
			delete (Item->second); 
			LayerList.erase(Item);
			return true;
		}
		else
			++Item;
	}

	return false;
}

FloatRect LayersCompose::GetDefaultFullScreenUVMapping()
{
	FloatRect Result;
	return Result;
}

void LayersCompose::Paint()
{
	bool bHasEffects = HasEffects();
	if(!bHasEffects)
	{
		NeedUpdateLayers = true;
		this->PaintScreen3D();
		//ShowAnimationTextures();

		return;
	}

	if(NeedUpdateLayers && bHasEffects)
	{
		this->PaintScreen3D();
		NeedUpdateLayers = false;
		TakeNewScreenSnapshot();
		LayerList[2]->RenderGraphic = OldScreen->GetRenderGraphic();
		LayerList[3]->RenderGraphic = NewScreen->GetRenderGraphic();
	}

//	if(bHasEffects)
//		PaintOldScreen3D();
	
	ExtensionManager Extension;
	Extension.ClearScreen(true, false);

	std::map <int, Layer*>::iterator Item;
	for(Item = LayerList.begin(); Item != LayerList.end(); ++Item)
	{
		if ((Item->second)->HasEffects())
			Item->second->Paint();
	}

	if(!HasEffects())
		PaintScreen3D();

	//ShowAnimationTextures();
}

bool LayersCompose::HasEffects()
{
	std::map <int, Layer*>::iterator Item;
	for(Item = LayerList.begin(); Item != LayerList.end(); ++Item)
	{
		if ((Item->second)->HasEffects())
			return true;
	}	
	return false;
}

Effect* LayersCompose::CreateEffect(int LayerLevel, int IDEffect, 
	int StartAfter, int TimeForComplete)
{
	return LayerList[LayerLevel]->CreateEffect(IDEffect, StartAfter, TimeForComplete);
}

TBaseWidget* LayersCompose::CreateWidget(int LayerLevel, int WidgetType, int Top, 
	int Left, int Width, int Height, char* Text)
{
	return LayerList[LayerLevel]->CreateWidget(WidgetType, Top, Left, Width, 
		Height, Text);
}

void LayersCompose::PaintScreen3D()
{
	if(CurrentLayer)
	{
		MeshTransform Transform;
		Transform.ApplyTranslate(-Width/2.0f, -Height/2.0f,Height/2.0f);
		CurrentLayer->Paint(Transform);
	}
}

void LayersCompose::PaintOldScreen3D()
{
 	OpenGLTexture Texture = OldScreen->GetRenderGraphic()->Texture;

	//glBindTexture(GL_TEXTURE_2D, Texture);
	TextureMan->SetupTexture(Texture);
	glEnd();

	float MaxU = float(Width)/MathUtils::MinPowerOf2(Width);
	float MaxV = float(Height)/MathUtils::MinPowerOf2(Height);
	glPushMatrix();
	glTranslatef(float(-Width/2), float(Height/2), float(Height/2));
	glScalef(1, -1, 1);
	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(1, 1, 1);
	glTexCoord2f(0.0, 0.0);
	glVertex3d(0, 0, 0);
	glTexCoord2f(MaxU, 0.0);
	glVertex3d(Width, 0, 0);

	glTexCoord2f(0.0, MaxV);
	glVertex3d(0, Height, 0);

	glTexCoord2f(MaxU, MaxV);
	glVertex3d(Width, Height, 0);

	glColor3f(1, 1, 1);
	glEnd();

	glPopMatrix();

	//if(OldLayer)
	//{
	//	MeshTransform Transform;
	//	Transform.ApplyTranslate(-Width/2.0f, -Height/2.0f,Height/2.0f);
	//	OldLayer->Paint(Transform);
	//}
}

void LayersCompose::Resize(int Width, int Height)
{
	std::map <int, Layer*>::iterator Item;
	for(Item = LayerList.begin(); Item != LayerList.end(); ++Item)
		Item->second->Resize(Width, Height);
}

void LayersCompose::UpdateLayers(MeshFrame* CurrentLayer, MeshFrame* OldLayer)
{
	this->CurrentLayer = CurrentLayer;
	this->OldLayer = OldLayer;
}

MeshFrame* LayersCompose::GetCurrentLayer()
{
	return CurrentLayer;
}

MeshFrame* LayersCompose::GetOldLayer()
{
	return OldLayer;
}

void LayersCompose::TakeNewScreenSnapshot()
{
	OpenGLTexture Texture = NewScreen->GetRenderGraphic()->Texture;
	NewScreen->GetRenderGraphic()->Texture = OldScreen->GetRenderGraphic()->Texture;
	OldScreen->GetRenderGraphic()->Texture = Texture;
	NewScreen->RenderFrameToGraphic();	
}


void LayersCompose::ShowAnimationTextures()
{
	OpenGLTexture Texture = NewScreen->GetRenderGraphic()->Texture;

	TextureMan->SetupTexture(Texture);
	glEnd();
	//glBindTexture(GL_TEXTURE_2D, Texture);
	glTranslatef(-400, -300, 300);
	glScalef(1, -1, 1);
	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(1, 0, 0);
	glTexCoord2f(0.0, 0.0);
	glVertex3d(0, 0, 120);
	glTexCoord2f(1.0, 0.0);
	glVertex3d(100, 0, 120);
	glTexCoord2f(0.0, 1.0);
	glVertex3d(0, 100, 120);
	glTexCoord2f(1.0, 1.0);
	glVertex3d(100, 100, 120);

	glColor3f(1, 1, 1);
	glEnd();

	glScalef(1, -1, 1);
	glTranslatef(400, 300, -300);

	Texture = OldScreen->GetRenderGraphic()->Texture;

	
	//glBindTexture(GL_TEXTURE_2D, Texture);
	TextureMan->SetupTexture(Texture);
	glEnd();

	glTranslatef(-400, -300, 300);
	glScalef(1, -1, 1);
	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(1, 1, 0);

	glTexCoord2f(0.0, 0.0);
	glVertex3d(-100, 0, 120);

	glTexCoord2f(1.0, 0.0);
	glVertex3d(0, 0, 120);
	
	glTexCoord2f(0.0, 1.0);
	glVertex3d(-100, 100, 120);
	
	glTexCoord2f(1.0, 1.0);
	glVertex3d(0, 100, 120);

	glColor3f(1, 1, 1);
	glEnd();

	glScalef(1, -1, 1);
	glTranslatef(400, 300, -300);
}


}
