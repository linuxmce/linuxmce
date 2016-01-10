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
#include "DrawingWidgetsEngine.h"

/* Standard OpenGL/GLX header files */
#include <GL/gl.h>

#include <stdlib.h>

#include "basicwindow.h"
#include "bezierwindow.h"

#include "DCE/Logger.h"

namespace GLEffect2D
{

DrawingWidgetsEngine::DrawingWidgetsEngine(int Width, int Height)
{
	this->Width = Width;
	this->Height = Height;
} 

DrawingWidgetsEngine::~DrawingWidgetsEngine()
{
}

void DrawingWidgetsEngine::Paint()
{

	vector<TBaseWidget*>::iterator Widget;
	for(Widget = Widgets.begin(); Widget < Widgets.end(); Widget++)
	{
		TBaseWidget* WidgetPtr = *Widget;
		if(WidgetPtr->GetParent() == NULL)
		{
			WidgetPtr->Paint();
			string WidgetName = "Widget: " + WidgetPtr->GetName();

			DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL, WidgetName.c_str());
		}
	}
	MeshTransform Transform;
	Transform.ApplyTranslate( (float)- Width / 2, (float)- Height / 2, (float)Height / 2);

	Context.Paint(Transform);
	//DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "EndFrame");
}

TBaseWidget* DrawingWidgetsEngine::CreateWidget(int WidgetType, int Top, int Left, int Width, int Height, char* Text)
{
	TBaseWidget * Result = NULL;
	switch(WidgetType) {
		case BASICWINDOW:
			Result = new TBasicWindow(&Context, Top, Left, Width, Height, Text);
			break;
		case BEZIERWINDOW:
			Result = new TBezierWindow(&Context,Top, Left, Width, Height, Text);
 			break;
	}
	
	AddWidget(Result);
	
	return Result;
}

void DrawingWidgetsEngine::AddWidget(TBaseWidget * Widget) {
	Widgets.push_back(Widget);
}

void DrawingWidgetsEngine::DeleteWidget(TBaseWidget* Widget) {
	vector<TBaseWidget*>::iterator FindItem;
	for(FindItem = Widgets.begin(); FindItem < Widgets.end(); FindItem++)
		if((*FindItem) == Widget) {
			delete Widget;
			Widgets.erase(FindItem);
			return;
		}
}

void DrawingWidgetsEngine::RemoveAll()
{
	vector<TBaseWidget*>::iterator FindItem;
	for(FindItem = Widgets.begin(); FindItem < Widgets.end(); FindItem++)
		delete *FindItem;
	Widgets.clear();
}

bool DrawingWidgetsEngine::RemoveWidgetFromList(TBaseWidget* Widget)
{
	vector<TBaseWidget*>::iterator FindItem;
	for(FindItem = Widgets.begin(); FindItem < Widgets.end(); FindItem++)
		if((*FindItem) == Widget) {
			FindItem = Widgets.erase(FindItem);
			return true;
		}
	return false;
}

void DrawingWidgetsEngine::Resize(int Width, int Height)
{
	this->Width = Width;
	this->Height = Height;
}

}
