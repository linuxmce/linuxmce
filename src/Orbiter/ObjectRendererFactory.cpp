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
#include "ObjectRendererFactory.h"
#include "ObjectRenderer.h"
#include "DataGridRenderer.h"
#include "../pluto_main/Define_DesignObjType.h"

#if defined(BLUETOOTH_DONGLE)
	#include "../Bluetooth_Dongle/DataGridRenderer_Bluetooth.h"
#elif defined(PROXY_ORBITER)
	#include "../Proxy_Orbiter/DataGridRenderer_Proxy.h"
#endif

#if defined(ORBITER_OPENGL)
	#include "OpenGL/ObjectRenderer_OpenGL.h"
	#include "OpenGL/DataGridRenderer_OpenGL.h"
#endif

ObjectRendererFactory::ObjectRendererFactory()
{
}

ObjectRendererFactory::~ObjectRendererFactory()
{
}

/*static*/ ObjectRenderer *ObjectRendererFactory::CreateObjectRenderer(DesignObj_Orbiter *pOwner, int nObjectType)
{
	switch(nObjectType)
	{
		case DESIGNOBJTYPE_Datagrid_CONST:

#if defined(BLUETOOTH_DONGLE)
			return new DataGridRenderer_Bluetooth(pOwner);
#elif defined(PROXY_ORBITER)
			return new DataGridRenderer_Proxy(pOwner);
#elif defined(ORBITER_OPENGL)
			return new DataGridRenderer_OpenGL(pOwner);
#else
			return new DataGridRenderer(pOwner);
#endif

	}

#if defined(ORBITER_OPENGL)
	return new ObjectRenderer_OpenGL(pOwner);
#else
	return new ObjectRenderer(pOwner);
#endif
}
