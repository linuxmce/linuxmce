/*
    This file is part of QOrbiter for use with the LinuxMCE project found at http://www.linuxmce.org
   Langston Ball  golgoj4@gmail.com
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
*/

#ifndef FLOORPLANMODEL_H
#define FLOORPLANMODEL_H
/*
  This class is intended to be an intelligent container class for the floorplans. Initially it will hold the images for the floorplans
  and the floorplan devices and their types. I hope expand it to internalize the data needed for placing on the floorplan the items at their
  appropriate places. None of the operations here can be heavy however, as this class needs to remain lightweight and fast as its connected
  to the gui thread.
  */
#include <QAbstractListModel>
#include <datamodels/floorplanimageitem.h>
#include <QModelIndex>

class FloorPlanModel : public QAbstractListModel
{
    Q_PROPERTY (QString currentPage READ getCurrentPage WRITE setCurrentPage NOTIFY pageChanged)
    Q_PROPERTY (QImage currentImage READ getCurrentImage WRITE setImage NOTIFY floorPlanImageChanged)
    Q_OBJECT
public:
    explicit FloorPlanModel(FloorPlanItem *m_prototype, QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    void appendRow(FloorPlanItem* item);
    void appendRows(const QList<FloorPlanItem*> &items);
    void insertRow(int row, FloorPlanItem* item);
    bool removeRow(int row, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    FloorPlanItem* takeRow(int row);
    FloorPlanItem* find(const QString &id) const;
    QModelIndex indexFromItem( const FloorPlanItem* item) const;
    FloorPlanItem* currentRow();
    QString currentPage;
    QImage currentImage;
    int currentFloorPlanType;

    // QModelIndex index(int row, int column, const QModelIndex &parent) const ;
    //  QModelIndex parent(const QModelIndex &child) const;
    //  int columnCount(const QModelIndex &parent) const;

signals:
    void pageChanged(QString s);
    void floorPlanImageChanged();
    void floorPlanStatus(QString s);
    void requestNewFloorPlanData(QString p);
public slots:
    void clear();
    void handleItemChange();

    QImage getPageImage(QString &id);
    QString getCurrentImagePath();

    QImage getCurrentImage() {return currentImage;}


    Q_INVOKABLE void setCurrentPage(QString currentPageId);
    QString getCurrentPage() {return currentPage;}

    void setImageData(const uchar *data, int iData_size);
    void setImage(QImage fp) { currentImage = fp; emit floorPlanImageChanged(); emit requestNewFloorPlanData(currentPage);}

    QImage getFloorPlanImage () {return currentImage;}

private:
    FloorPlanItem* m_prototype;
    QList<FloorPlanItem*> m_list;
};

#endif // FLOORPLANMODEL_H
