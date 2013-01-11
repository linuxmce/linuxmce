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

#ifdef debug
#include <QDebug>
#endif
#ifdef QT5
#include <QtWidgets/QApplication>

#else
#include <QApplication>
#endif


#include "datamodels/listModel.h"


ListModel::ListModel(gridItem* prototype, QObject* parent) :
    QAbstractListModel(parent),  m_prototype(prototype)
{
#ifndef RPI
    setRoleNames(m_prototype->roleNames());
#endif
    qRegisterMetaType<QModelIndex>("QModelIndex");
    totalcells = 0;
    seperator = 16;
    loadingStatus = false;
    progress = 0;
    clearing = false;
    clear();
    setTotalPages(0);
}

int ListModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid()){
        return 0;
    }
    else{
        return m_list.size();
    }

}

QVariant ListModel::data(const QModelIndex &index, int role) const
{
    if(index.row() < 0 || index.row() >= m_list.size())
        return QVariant();
    return m_list.at(index.row())->data(role);
}

QHash<int, QByteArray> ListModel::roleNames() const
{
    return m_prototype->roleNames();
}

ListModel::~ListModel() {
    delete m_prototype;
    clear();
}

void ListModel::appendRow(gridItem *item)
{
    setLoadingStatus(true);
    appendRows(QList<gridItem*>() << item);
  //  qDebug() << m_list.count();
}

void ListModel::appendRows(const QList<gridItem *> &items)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount()+items.size()-1);
    foreach(gridItem *item, items) {
        m_list.append(item);
        QObject::connect(item, SIGNAL(dataChanged()), this , SLOT(handleItemChange()));

    }

    endInsertRows();
    QModelIndex index = indexFromItem(m_list.last());
    QModelIndex index2 = indexFromItem(m_list.first());
    int currentRows= m_list.count();
    emit itemAdded(currentRows);
    setCurrentCells(currentRows);
    double p = (((double)m_list.size() / (double)seperator) * 100) ;
    setProgress(p);
    emit dataChanged(index2, index, currentRows);
    setLoadingStatus(false);
    QApplication::processEvents(QEventLoop::AllEvents);

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
    qDebug() << "Handling item change for:" << index;
    if(index.isValid())
    {
        emit dataChanged(index, index, index.row() );
    }
}

void ListModel::reset()
{
    clearing = true;
    QApplication::processEvents(QEventLoop::AllEvents);
    emit modelAboutToBeReset();
    beginResetModel();
    resetInternalData();
    setProgress(0.0);
    QApplication::processEvents(QEventLoop::AllEvents);
    endResetModel();
    emit modelReset();
    QApplication::processEvents(QEventLoop::AllEvents);
   clearing = false;
}

bool ListModel::resetInternalData()
{
    qDebug("Resetting listmodel data");
    int total = m_list.count();
    for(int i = 0; i < m_list.count(); ++i){

      //  qDebug() <<"removing::" << m_list.count() << " of " << total;
        gridItem* pItem = m_list.takeAt(i);
        if (pItem)
                delete pItem;
    }
    m_list.clear();
  //  qDebug() << "Items cleared. Count:: "<< m_list.count();
    return true;

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
    clearing = true;
    QApplication::processEvents(QEventLoop::AllEvents);
    emit modelAboutToBeReset();
    beginResetModel();
    if(resetInternalData()){
    setProgress(0.0);
    QApplication::processEvents(QEventLoop::AllEvents);
    endResetModel();
    emit modelReset();
    QApplication::processEvents(QEventLoop::AllEvents);
    clearing = false;
    }

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
    if(row < 0 || (row+count) > m_list.size()) return false;
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

void ListModel::sortModel(int column, Qt::SortOrder order)
{
}

void ListModel::checkForMore()
{


}

void ListModel::populateGrid(int mediaType)
{
    //manager_ref->pqOrbiter->prepareFileList(mediaType);
}

void ListModel::setTotalCells(int cells)
{
    totalcells = cells;
    emit statusMessage("Size Changed" + QString::number(totalcells));
    if ( m_list.size() < totalcells)
    {



    }
    else
    {


    }
    emit sizeChanged(cells);

}

int ListModel::getTotalCells()
{
    return totalcells;
}

void ListModel::setGridType(int type)
{
    gridType = type;
    //emit gridTypeChanged(gridType);
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


}

void ListModel::clearAndRequest(int type)
{
    clearing = true;
    gridType = type;
    QApplication::processEvents(QEventLoop::AllEvents);
    emit modelAboutToBeReset();
    beginResetModel();
    if(    resetInternalData()){
        setProgress(0.0);
        QApplication::processEvents(QEventLoop::AllEvents);
        endResetModel();
        emit modelReset();

        QApplication::processEvents(QEventLoop::AllEvents);
        clearing = false;
        emit ready(gridType);
    }
    else
    {

    }
}

void ListModel::clearForPaging()
{

    /*
    if(removeRows(0, m_list.count(), QModelIndex()))
        emit pagingCleared();
        */
    QApplication::processEvents(QEventLoop::AllEvents);
    emit modelAboutToBeReset();
    beginResetModel();
    if(resetInternalData()){
        setProgress(0.0);
        QApplication::processEvents(QEventLoop::AllEvents);
        endResetModel();
        emit modelReset();
        emit pagingCleared();
        QApplication::processEvents(QEventLoop::AllEvents);
    }

}

