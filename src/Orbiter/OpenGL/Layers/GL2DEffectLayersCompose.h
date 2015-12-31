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
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#ifndef GL2DEffectLayersCompose_H_
#define GL2DEffectLayersCompose_H_

#include "../Effects/gl2deffectfactory.h"
#include "../Texture/TextureManager.h"
#include <map>

#include "GL2DEffectLayer.h"

#include "../Mesh/MeshFrame.h"
#include "GLRenderScreenToGraphic.h"


namespace GLEffect2D 
{

enum GL2DEffectDefaultLayers {
	DesktopOffScreen = 2,
	DesktopOnScreen = 3
};

/**
 *	Singleton class that defines a layered based rendering
 */
class LayersCompose
{
	std::map <int, Layer*> LayerList;

	TextureManager* TextureMan;
	MeshFrame* CurrentLayer;
	MeshFrame* OldLayer;

	static LayersCompose* Instance_;
	LayersCompose();

	bool NeedUpdateLayers;

public:
	int Width, Height;

	GLRenderScreenToGraphic *OldScreen;
	GLRenderScreenToGraphic *NewScreen;

	static LayersCompose* Instance();
	virtual ~LayersCompose(void);

	void Setup(int Width, int Height);
	void CleanUp();

	void UpdateLayers(MeshFrame* CurrentLayer, MeshFrame* OldLayer);

	FloatRect GetDefaultFullScreenUVMapping();

	/**
	 *	Creates a custom layer that match the level. 
	 *	If the layer level already exits, it returns the old layer
	 */
	Layer* CreateLayer(int LayerLevel);

	/**
	 *	Returns if exist one layer at choosen layer level
	 */
	bool ExistsLayerLevel(int LayerLevel);

	/**
	 *	Remove a layer from drawing
	 *	returns false if layer previously doesn't exist;
	 */
	bool RemoveLayer(int LayerLevel);

	/**
	 *	Paint all layers
	 */
	void Paint();

	/*
	 *	Test if there are pending effects in any of layers
	 */
	bool HasEffects();

	/**
	 *	Creates a layer inside the chosed layer
	 */
	Effect* CreateEffect(int LayerLevel, int IDEffect, int StartAfter, int TimeForComplete);

	/**
	 *	Creates a widget inside the chosed layer
	 */
	TBaseWidget* CreateWidget(int LayerLevel, int WidgetType, int Top, 
		int Left, int Width, int Height, char* Text);

	void PaintScreen3D();
	void PaintOldScreen3D();

	/**
	 *	TODO: For Ciprian, to treat the cases of window resize
	 *	Support for resize screen 
	 */
	void Resize(int Width, int Height);

	/**
	 * Layer management	
	 */
	MeshFrame* GetCurrentLayer();
	MeshFrame* GetOldLayer();

	void TakeNewScreenSnapshot();
	void ShowAnimationTextures();
};

}

#endif
