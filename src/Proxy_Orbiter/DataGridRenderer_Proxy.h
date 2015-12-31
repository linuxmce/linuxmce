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
#ifndef __DATAGRID_RENDERER_PROXY_H__
#define __DATAGRID_RENDERER_PROXY_H__

#include "../Orbiter/DataGridRenderer.h"

class DataGridRenderer_Proxy : public DataGridRenderer
{
public:
	DataGridRenderer_Proxy(DesignObj_Orbiter *pOwner);
	~DataGridRenderer_Proxy();

	bool RenderCell(DataGridTable *pT, DataGridCell *pCell, int j, int i, int iGraphicToDisplay, PlutoPoint point = PlutoPoint(0, 0));
};

#endif //__DATAGRID_RENDERER_PROXY_H__