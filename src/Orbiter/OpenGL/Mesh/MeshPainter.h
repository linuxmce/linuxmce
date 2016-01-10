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
#ifndef MESHPAINTER_H_
#define MESHPAINTER_H_

#include "MeshContainer.h"
#include "MeshTransform.h"

#include <pthread.h>

class ExtensionManager;

class MeshPainter
{
	pthread_mutex_t SafetyPaintMutex;
	static MeshPainter* Instance_;
	ExtensionManager *ExtensionManager_;
	
	MeshPainter();
	
public:
	void PaintContainer(MeshContainer& Container, MeshTransform& Transform,
		MeshTransform& TextureTransform);
	
	static MeshPainter* Instance();

	void Setup(ExtensionManager *ExtensionManager);
	void CleanUp();
};

#endif /*MESHPAINTER_H_*/
