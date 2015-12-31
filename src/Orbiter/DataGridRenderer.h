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
#ifndef __DATAGRID_RENDERER_H__
#define __DATAGRID_RENDERER_H__

#include "Orbiter.h"
#include "ObjectRenderer.h"
#include "DesignObj_Orbiter.h"

class TextStyle;

namespace DCE
{
	class DataGridCell;
};

class DataGridRenderer : public ObjectRenderer
{
	friend class DesignObj_DataGrid;  // This handles a special case caching with 2 grids that must stay sync'd
protected:
	DesignObj_DataGrid *m_pObj_Owner_DataGrid;
	int StartAnimation;
	int iPK_Direction;

public:
	DataGridRenderer(DesignObj_Orbiter *pOwner);
	virtual ~DataGridRenderer(void);

	virtual void RenderObject(DesignObj_Orbiter *pObj_Screen, PlutoPoint point = PlutoPoint(0, 0));
	virtual void GetGridCellDimensions(int Colspan,  int Rowspan,  int Column,  int Row,  int &x,  int &y,  int &w,  int &h );
	virtual bool RenderCell(DataGridTable *pT, DataGridCell *pCell, int j, int i, int iGraphicToDisplay, PlutoPoint point = PlutoPoint(0, 0));
};

#endif //__DATAGRID_RENDERER_H__
