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
#ifndef ACTIVEMEDIASTREAMS_H
#define ACTIVEMEDIASTREAMS_H

#include <QObject>
#include <QVariant>
#include <QImage>
#include <QMap>
#include "DCE/DataGrid.h"
#include "genericmodelitem.h"
using namespace DCE;

class ActiveMediaStreamItem : public GenericModelItem
{
    Q_OBJECT

public:
    enum Roles {
        StreamIDRole = Qt::UserRole+1,
        DescriptionRole =Qt::UserRole+2,
    };

    ActiveMediaStreamItem(DataGridCell *pCell);
    explicit ActiveMediaStreamItem( QObject *parent = NULL);
    void setFromDataGridCell(DataGridCell *pCell);

    //    QVariant data(int role);

private:
    QString m_StreamID;
    QString m_description;

signals:
    void dataChanged();

};

#endif // ACTIVEMEDIASTREAMS_H
