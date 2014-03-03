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
#include "DataModelItems/sleepingalarm.h"

using namespace DCE;

class DataGridHandler
{

public:
    enum Roles {
        DescriptionRole = Qt::UserRole+1,
        ValueRole =Qt::UserRole+2,
    };

  static GenericModelItem* GetModelItemType(int PK_DataGrid, QObject* parent = 0) {
	if (PK_DataGrid == DATAGRID_Floorplan_Media_Streams_CONST) {
	    return new ActiveMediaStreamItem(parent);
	} else if (PK_DataGrid == DATAGRID_Alarms_In_Room_CONST) {
	    return new SleepingAlarm(parent);
	} else {
	    // Default roles
	    GenericModelItem* pItem = new GenericModelItem(parent);
	    QHash<int, QByteArray> names;
	    names[DescriptionRole] = "description";
	    names[ValueRole] = "value";
	    pItem->setRoleNames(names);
	    return pItem;
	}
  }

  static GenericModelItem* GetModelItemForCell(int PK_DataGrid, DataGridTable* pTable, int row, QObject * parent = 0)
  {
    GenericModelItem* pItem = GetModelItemType(PK_DataGrid, parent);
    LoadDataFromTable(PK_DataGrid, pItem, pTable, row);
    return pItem;
  }

  static void LoadDataFromTable(int PK_DataGrid, GenericModelItem* pItem, DataGridTable* pTable, int row)
  {
    int col = 0;
    DataGridCell *pCell = pTable->GetData(col, row);
    if(!pCell){
      return;
    }
    if (PK_DataGrid == DATAGRID_Floorplan_Media_Streams_CONST) {
        (static_cast<ActiveMediaStreamItem*>(pItem))->setFromDataGridCell(pCell);
    } else if (PK_DataGrid == DATAGRID_Alarms_In_Room_CONST) {
        QString name;
	QString days;
	QString timeleft;
	QString alarmtime;
	int eventgrp;
	bool state = QString::fromStdString(pCell->GetText()).contains("ON");
        //getting the cell to the right
        DataGridCell *pCell2 = pTable->GetData(1, row);
	eventgrp = atoi(pCell2->GetValue());
	QString data = pCell2->GetText();
	QStringList breakerbreaker = data.split(QRegExp("\n"), QString::KeepEmptyParts );
	name = breakerbreaker.at(0);
	days=breakerbreaker.at(2);
	days.remove("Day of Week");
	if(!breakerbreaker.at(1).split(QRegExp("\n")).isEmpty())
	{
	    alarmtime=breakerbreaker.at(1).split(QRegExp(" "),QString::SkipEmptyParts).first();
	    timeleft=breakerbreaker.at(1).split(QRegExp(" "),QString::SkipEmptyParts).last();
	} else {
	    alarmtime=breakerbreaker.at(1);
	    timeleft = "";
	}
	(static_cast<SleepingAlarm*>(pItem))->setAlarmData(eventgrp, name, alarmtime, state, timeleft, days);
    } else {
        // Default, get one cell use text and value
        pItem->setData(ValueRole, pCell->GetValue());
	pItem->setData(DescriptionRole, pCell->GetText());
    }
}

};

#endif // DATAGRIDHANDLER_H
