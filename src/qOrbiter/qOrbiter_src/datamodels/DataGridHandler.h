/*
    This file is part of QOrbiter for use with the LinuxMCE project found at http://www.linuxmce.org
    QOrbiter is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QOrbiter is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QOrbiter.  If not, see <http://www.gnu.org/licenses/>.

    This class is another specific model/item pair with the parent being parent being floorplan model.
    it is not however implmented directly as a model, but a QList<QOBject*>. This class holds very simple data pertaining to the
    images and descriptions of particular pages.

*/
#ifndef DATAGRIDHANDLER_H
#define DATAGRIDHANDLER_H

#include "pluto_main/Define_DataGrid.h"
#include "../DCE/DataGrid.h"

#include "genericmodelitem.h"
#include "ActiveMediaStreams.h"

using namespace DCE;

class DataGridHandler
{

public:
  static GenericModelItem* GetModelItemType(int PK_DataGrid) {
	if (PK_DataGrid == DATAGRID_Floorplan_Media_Streams_CONST) {
	    return new ActiveMediaStreamItem();
	} else {
	    return new GenericModelItem();
	}
  }

  static GenericModelItem* GetModelItemForCell(int PK_DataGrid, DataGridCell* pCell)
  {
    GenericModelItem* pItem = GetModelItemType(PK_DataGrid);
    LoadDataFromCell(PK_DataGrid, pItem, pCell);
    return pItem;
  }

  static void LoadDataFromCell(int PK_DataGrid, GenericModelItem* pItem, DataGridCell* pCell)
  {
    if (PK_DataGrid == DATAGRID_Floorplan_Media_Streams_CONST) {
      (static_cast<ActiveMediaStreamItem*>(pItem))->setFromDataGridCell(pCell);
    }


  }

};

#endif // DATAGRIDHANDLER_H
