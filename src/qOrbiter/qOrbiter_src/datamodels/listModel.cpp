/*
 * Author: Christophe Dumez <dchris@gmail.com>
 * License: Public domain (No attribution required)
 * Website: http://cdumez.blogspot.com/
 * Version: 1.0
 */

#include "datamodels/listModel.h"
#include <QDebug>

ListModel::ListModel(gridItem* prototype, QObject* parent) :
    QAbstractListModel(parent),  m_prototype(prototype)
{
    setRoleNames(m_prototype->roleNames());
    qRegisterMetaType<QModelIndex>("QModelIndex");
    totalcells = 0;
    loadingStatus = false;
    progress = 0;
}

int ListModel::rowCount(const QModelIndex &parent) const
{
    //Q_UNUSED(parent);
    return m_list.size();
}

QVariant ListModel::data(const QModelIndex &index, int role) const
{
    if(index.row() < 0 || index.row() >= m_list.size())
        return QVariant();
    return m_list.at(index.row())->data(role);
}

ListModel::~ListModel() {
    delete m_prototype;
    clear();
}

void ListModel::appendRow(gridItem *item)
{
    appendRows(QList<gridItem*>() << item);
}

void ListModel::appendRows(const QList<gridItem *> &items)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount()+items.size()-1);
    foreach(gridItem *item, items) {

        QObject::connect(item, SIGNAL(dataChanged()), this , SLOT(handleItemChange()));
        m_list.append(item);
    }

    endInsertRows();
    QModelIndex index = indexFromItem(m_list.last());
    QModelIndex index2 = indexFromItem(m_list.first());
    int currentRows= m_list.count() - 1;
    emit dataChanged(index2, index, currentRows);


}

void ListModel::insertRow(int row, gridItem *item)
{
    beginInsertRows(QModelIndex(), row, row);
    connect(item, SIGNAL(dataChanged()), this, SLOT(handleItemChange()));
    ////qdebug() << "Inserting at:" << row;
    m_list.insert(row, item);
    endInsertRows();
}

void ListModel::handleItemChange()
{
    gridItem* item = static_cast<gridItem*>(sender());
    QModelIndex index = indexFromItem(item);
    ////qdebug() << "Handling item change for:" << index;
    if(index.isValid())
    {
        emit dataChanged(index, index, 0);
    }
}

gridItem * ListModel::find(const QString &id) const
{
    foreach(gridItem* item, m_list) {
        if(item->id() == id) return item;
    }
    return 0;
}

QVariant ListModel::get(int index, const QString &name) const
{
    if (index>=0 && index<m_list.size()) {
        gridItem* myItem = m_list.at(index);

        QHash<int, QByteArray> myHash = myItem->roleNames();
        QHash<int, QByteArray>::const_iterator i = myHash.constBegin();

        while (i != myHash.constEnd()) {
            if (i.value() == name) {
                return myItem->data(i.key());
            }
            ++i;
        }
    } else {
        return "";
    }
}

QModelIndex ListModel::indexFromItem(const gridItem *item) const
{
    Q_ASSERT(item);
    for(int row=0; row<m_list.size(); ++row) {

        if(m_list.at(row) == item) return index(row);

    }

    return QModelIndex();
}

void ListModel::clear()
{
    //("Clearing List");

    qDeleteAll(m_list);
    m_list.clear();
   reset();
    totalcells = 0;

    qDebug() << "Total Rows:" << m_list.count();

}

bool ListModel::removeRow(int row, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    if(row < 0 || row >= m_list.size()) return false;
    beginRemoveRows(QModelIndex(), row, row);
    delete m_list.takeAt(row);
    endRemoveRows();
    return true;
}

bool ListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    if(row < 0 || (row+count) >= m_list.size()) return false;
    beginRemoveRows(QModelIndex(), row, row+count-1);
    for(int i=0; i<count; ++i) {
        delete m_list.takeAt(row);
    }
    endRemoveRows();
    return true;
}

gridItem * ListModel::takeRow(int row)
{
    beginRemoveRows(QModelIndex(), row, row);
    gridItem* item = m_list.takeAt(row);
    endRemoveRows();
    return item;
}

gridItem * ListModel::currentRow()
{
    gridItem* item = m_list.at(0);
    return item;
}

void ListModel::checkForMore()
{


    if (m_list.count() < totalcells && loadingStatus == true )
    {
        int l = m_list.count();
        double p = (((double)l / (double)totalcells) * 100) ;
        //qDebug()<< p;

        setProgress(p);
        setLoadingStatus(true);
        emit gimmieData(gridType);
    }
    else
    {
      //  qDebug() <<QString::number(m_list.count()) +" cells in model, out of " + QString::number(totalcells) <<" No Mas";
        setLoadingStatus(false);
    }

}

void ListModel::populateGrid(int mediaType)
{
    //manager_ref->pqOrbiter->prepareFileList(mediaType);
}

void ListModel::setTotalCells(int cells)
{
    totalcells = cells;

    if ( m_list.count() < totalcells)
    {

        setLoadingStatus(true);

    }
    else
    {
        //qdebug() <<QString::number(m_list.count()) +" cells in model, out of " + QString::number(totalcells);
        setLoadingStatus(false);
    }
   // emit sizeChanged(cells);

}

int ListModel::getTotalCells()
{
    return totalcells;
}

void ListModel::setGridType(int type)
{
    gridType = type;
    emit gridTypeChanged(gridType);
    ////qdebug("Grid Type Set");

}

int ListModel::getGridType()
{
    return gridType;
}

void ListModel::setLoadingStatus(bool b)
{
    loadingStatus = b;
    emit loadingStatusChanged(loadingStatus);
}

bool ListModel::getLoadingStatus()
{
    return loadingStatus;
}

void ListModel::setProgress(double n_progress)
{
    progress = n_progress;
    emit progressChanged( progress );
    // qDebug() << "Loading Progress:" << progress;
      // qDebug() << QString::number(m_list.count()) +" cells in model, out of " + QString::number(totalcells);

}

double ListModel::getProgress()
{
    return progress;
}

void ListModel::attributeSort()
{
   this->clear();
    emit gimmieData(gridType);

}

