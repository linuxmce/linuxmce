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
#include "DCE/DataGrid.h"
#include "DCE/Logger.h"

#include "genericmodelitem.h"
#include "ActiveMediaStreams.h"
#include "DataModelItems/sleepingalarm.h"
#include "playlists/epgitemclass.h"
#include "gridItem.h"

using namespace DCE;

class DataGridHandler
{

public:
    enum Roles {
        DescriptionRole = Qt::UserRole+1,
        ValueRole = Qt::UserRole+2,
        OtherRole = Qt::UserRole+3
    };

    static QHash<int, QByteArray> getRoleNames(int PK_DataGrid) {
        QHash<int, QByteArray> names;

        switch (PK_DataGrid) {

        case DATAGRID_Media_Browser_CONST: /*!< Case specific role for media browser */

            names[gridItem::NameRole] = "name";
            names[gridItem::IndexRole] = "dceindex";
            names[gridItem::PathRole] = "path";
            names[gridItem::FKRole]= "id";
            break;

//        case DATAGRID_EPG_All_Shows_CONST:
//            names[EPGItemClass::NameRole] = "name";                 /** Maps to channel name */
//            names[EPGItemClass::ProgramRole] = "program";           /** Maps to program on channel */
//            names[EPGItemClass::ChannelRole] = "channel";           /** Numeric channel number, user facing */
//            names[EPGItemClass::ChannelIdRole] = "channelid";       /** Numeric number, internal representation. It has the source number prepended to the actual string of digits. Ie. the last three are the tunable digits */
//            names[EPGItemClass::TimeSlotRole] = "timeslot";         /** String representation of the time slot data */
//            names[EPGItemClass::ProgramIdRole] = "programid";       /** Experimental */
//            names[EPGItemClass::SeriesIdRole] = "seriesid";         /** Experimental */
//            names[EPGItemClass::BroadcastSourceRole] = "sourceid";  /** The current source being used */
//            names[EPGItemClass::BroadcastSourceNameRole] = "source";/** The Source name */
//            names[EPGItemClass::IdRole] = "id";
//            break;

        default: /*!< Generic Roles */
            names[DescriptionRole] = "description";
            names[ValueRole] = "value";
            names[OtherRole] = "other";
            break;
        }

        return names;
    }

    static GenericModelItem* GetModelItemType(int PK_DataGrid, QObject* parent = 0) {
        if (PK_DataGrid == DATAGRID_Floorplan_Media_Streams_CONST) {
            return new ActiveMediaStreamItem(parent);
        } else if (PK_DataGrid == DATAGRID_Alarms_In_Room_CONST) {
            return new SleepingAlarm(parent);
        } else if (PK_DataGrid  == DATAGRID_EPG_All_Shows_CONST ||
                   PK_DataGrid  == DATAGRID_EPG_Grid_CONST){
         return new EPGItemClass(parent);
        } else {
            // uses generic model item
            GenericModelItem* pItem = new GenericModelItem(parent);
            pItem->setRoleNames(getRoleNames(PK_DataGrid));

            return pItem;
        }
    }

    static GenericModelItem* GetModelItemForCell(int PK_DataGrid, DataGridTable* pTable, int row, int col, QObject * parent = 0)
    {
        GenericModelItem* pItem = GetModelItemType(PK_DataGrid, parent);
        LoadDataFromTable(PK_DataGrid, pItem, pTable, row, col);
        return pItem;
    }

    static void LoadDataFromTable(int PK_DataGrid, GenericModelItem* pItem, DataGridTable* pTable, int row, int col)
    {

        DataGridCell *pCell = pTable->GetData(col, row);
        if(!pCell) {
            LoggerWrapper::GetInstance()->Write(LV_WARNING, "LoadDataFromTable() pCell == NULL ! row = %d, col = %d", row, col);
            return;
        }
        for (map<string,string>::iterator i= pCell->m_mapAttributes.begin(); i!= pCell->m_mapAttributes.end(); ++i){
            qDebug() << i->first.c_str() <<"::"<< i->second.c_str();
        }
        if (PK_DataGrid == DATAGRID_Media_Browser_CONST) {
            const char *pPath = pCell->GetImagePath();
            QString fk_file;
            QString cellTitle;
            QString filePath;
            filePath = QString::fromUtf8(pPath).remove(".tnj");
            fk_file = pCell->GetValue();
            cellTitle = QString::fromUtf8(pCell->m_Text);
            pItem->setData(gridItem::FKRole, fk_file);
            pItem->setData(gridItem::NameRole, cellTitle);
            pItem->setData(gridItem::PathRole, filePath.remove("/home/mediapics/"));

        } else if (PK_DataGrid == DATAGRID_Floorplan_Media_Streams_CONST) {
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
            if (pCell2 != NULL) {
                eventgrp = atoi(pCell2->GetValue());
                QString data = QString::fromUtf8(pCell2->GetText());
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
            }
            (static_cast<SleepingAlarm*>(pItem))->setAlarmData(eventgrp, name, alarmtime, state, timeleft, days);
        } else if(PK_DataGrid == DATAGRID_EPG_Grid_CONST || PK_DataGrid == DATAGRID_EPG_All_Shows_CONST) {



            QString channelIndex = QString::fromStdString(pCell->m_mapAttributes_Find("Number"));
            QString timeSlot = QString::fromStdString(pCell->m_mapAttributes_Find("Time"));
            QString startTime = QString::fromStdString(pCell->m_mapAttributes_Find("StartTime"));
            QString endTime = QString::fromStdString(pCell->m_mapAttributes_Find("EndTime"));
            QString progId = QString::fromStdString(pCell->m_mapAttributes_Find("Program"));
            QString sId = QString::fromStdString(pCell->m_mapAttributes_Find("Series"));
            QString src = QString::fromStdString(pCell->m_mapAttributes_Find("Source"));
            QStringList srcInfo = src.split(" ");
            QString sourceId, sourceName;

            sourceId = srcInfo.at(0);
            if (srcInfo.length() > 1)
                sourceName = srcInfo.at(1);

            QString program;
            int index = 0;

            QString tmp = QString::fromStdString(pCell->m_mapAttributes_Find("Name").c_str());
            QString channelName;
            int channelNumber = -1;
            if (tmp.indexOf(" ") > 0) {
                 channelNumber = tmp.left(tmp.indexOf(" ")).toInt();
                 channelName = tmp.right(tmp.length()-tmp.indexOf(" "));
            }
//            qDebug() << pCell->GetText();
//            qDebug() << pCell->GetValue();
            channelIndex = pCell->GetValue();
            program = QString::fromStdString(pCell->m_mapAttributes_Find("Info"));

            (static_cast<EPGItemClass*>(pItem))->setEpgItemData(
                        channelName,
                        program,
                        QString::number(channelNumber),
                        channelIndex,
                        timeSlot, startTime.toInt(), endTime.toInt(),
                        progId,
                        sId,
                        sourceId,
                        sourceName
                        );
        } else if (PK_DataGrid == DATAGRID_Active_Users_On_Channel_CONST) {
            DataGridCell *pCell2 = pTable->GetData(1, row);
            pItem->setData(ValueRole, pCell2->GetValue());
            pItem->setData(DescriptionRole, QString::fromUtf8(pCell2->GetText()));
        } else {

            // Default, get one cell use text and value
            pItem->setData(ValueRole, pCell->GetValue());
            pItem->setData(DescriptionRole, QString::fromUtf8(pCell->GetText()));
        }
    }

};

#endif // DATAGRIDHANDLER_H
