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
#ifndef __DATAGRID_RENDERER_BLUETOOTH_H__
#define __DATAGRID_RENDERER_BLUETOOTH_H__

#include "../Orbiter/DataGridRenderer.h"

class DataGridRenderer_Bluetooth : public DataGridRenderer
{
public:
	DataGridRenderer_Bluetooth(DesignObj_Orbiter *pOwner);
	~DataGridRenderer_Bluetooth();

	void RenderObject(DesignObj_Orbiter *pObj_Screen, PlutoPoint point = PlutoPoint(0, 0));
};

#endif //__DATAGRID_RENDERER_BLUETOOTH_H__