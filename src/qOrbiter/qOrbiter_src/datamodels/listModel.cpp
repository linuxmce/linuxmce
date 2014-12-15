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

DceListModel::DceListModel(gridItem* prototype, QObject* parent) :
    QAbstractListModel(parent),  m_prototype(prototype)
{
#ifndef QT5
    setRoleNames(m_prototype->roleNames());
#endif
    qRegisterMetaType<QModelIndex>("QModelIndex");
    currentItemIndex=0;
    totalcells = 0;
    seperator = 16;
    loadingStatus = false;
    progress = 0;
    clearing = false;
    gridType=-1;
    clear();
    setTotalPages(0);
}



int DceListModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid()){
        return 0;
    }
    else{
        return m_list.size();
    }

}

QVariant DceListModel::data(const QModelIndex &index, int role) const
{
    if(index.row() < 0 || index.row() >= m_list.size())
        return QVariant();
    return m_list.at(index.row())->data(role);
}

QHash<int, QByteArray> DceListModel::roleNames() const
{
    return m_prototype->roleNames();
}



void DceListModel::appendRow(gridItem *item)
{
    setLoadingStatus(true);   
    appendRows(QList<gridItem*>() << new gridItem(item->id(), item->name(), item->path(), item->index(), this ));

    //  item->destruct();
}

void DceListModel::appendRows(const QList<gridItem *> &items)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount()+items.size()-1);
    foreach(gridItem *item, items) {
     // qWarning() << "DceListModel Active Item::" << item->index();
        //setCurrentItemIndex(item->index());
        QObject::connect(item, SIGNAL(destroyed()), this, SLOT(itemDeleted()),Qt::QueuedConnection);
        QObject::connect(item, SIGNAL(dataChanged()), this , SLOT(handleItemChange()));
        m_list.append(item);
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

void DceListModel::insertRow(int row, gridItem *item)
{

    beginInsertRows(QModelIndex(), row, row);
    connect(item, SIGNAL(dataChanged()), this, SLOT(handleItemChange()));
    ////qdebug() << "Inserting at:" << row;
    m_list.insert(row, item);
    endInsertRows();

}

void DceListModel::handleItemChange()
{
    gridItem* item = static_cast<gridItem*>(sender());
    QModelIndex index = indexFromItem(item);
    qDebug() << "Handling item change for:" << index;
    if(index.isValid())
    {
        emit dataChanged(index, index, index.row() );
    }
}

void DceListModel::reset()
{
    clearing = true;

    emit modelAboutToBeReset();
    beginResetModel();
    if(resetInternalData()){
        setProgress(0.0);
        endResetModel();
        emit modelReset();
        clearing = false;
    }
    QApplication::processEvents(QEventLoop::AllEvents);
}

bool DceListModel::resetInternalData()
{
    qDebug("Resetting Media Listmodel data");
    qDebug() <<  "::Items to be cleared:: "<< m_list.size();
    m_list.erase(m_list.begin(), m_list.end());
    if(m_list.empty()){
        qDebug() << "Cleaned list," << m_list.size() << "remain.";
    }

    return true;
}

gridItem * DceListModel::find(const QString &id) const
{
    foreach(gridItem* item, m_list) {
        if(item->id() == id) return item;
    }
    return 0;
}

QVariant DceListModel::get(int index, const QString &name) const
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

QModelIndex DceListModel::indexFromItem(const gridItem *item) const
{
    Q_ASSERT(item);
    for(int row=0; row<m_list.size(); ++row) {
        if(m_list.at(row) == item) return index(row);
    }
    return QModelIndex();
}

void DceListModel::clear()
{
    clearing = true;
    QApplication::processEvents(QEventLoop::AllEvents);
    emit modelAboutToBeReset();
    beginResetModel();
    if(resetInternalData()){
        setProgress(0.0);
        setTotalCells(0);
        setLoadingStatus(false);
        progress = 0;
        setTotalPages(0);
        QApplication::processEvents(QEventLoop::AllEvents);
        endResetModel();
        emit modelReset();
        QApplication::processEvents(QEventLoop::AllEvents);
        clearing = false;
    }

}

bool DceListModel::removeRow(int row, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    if(row < 0 || row >= m_list.size()) return false;
    beginRemoveRows(QModelIndex(), row, row);
    delete m_list.takeAt(row);
    endRemoveRows();
    return true;
}

bool DceListModel::removeRows(int row, int count, const QModelIndex &parent)
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

gridItem * DceListModel::takeRow(int row)
{
    beginRemoveRows(QModelIndex(), row, row);
    gridItem* item = m_list.takeAt(row);
    endRemoveRows();
    return item;
}

gridItem * DceListModel::currentRow()
{
    gridItem* item = m_list.at(0);
    return item;
}

void DceListModel::sortModel(int column, Qt::SortOrder order)
{

}

void DceListModel::checkForMore()
{


}

void DceListModel::populateGrid(int mediaType)
{
    //manager_ref->pqOrbiter->prepareFileList(mediaType);
}

void DceListModel::setTotalCells(int cells)
{
    totalcells = cells;
    emit statusMessage("Size Changed to" + QString::number(totalcells));
    if ( m_list.size() < totalcells)
    {
        setLoadingStatus(true);
    }
    else
    {
        setLoadingStatus(false);

    }
    emit sizeChanged(cells);

}

int DceListModel::getTotalCells()
{
    return totalcells;
}

void DceListModel::setGridType(int type)
{
    gridType = type;
    //emit gridTypeChanged(gridType);
    ////qdebug("Grid Type Set");

}

int DceListModel::getGridType()
{
    return gridType;
}

void DceListModel::setLoadingStatus(bool b)
{
    loadingStatus = b;
    emit loadingStatusChanged(loadingStatus);
}

bool DceListModel::getLoadingStatus()
{
    return loadingStatus;
}

void DceListModel::setProgress(double n_progress)
{
    progress = n_progress;
    emit progressChanged( progress );
    // qDebug() << "Loading Progress:" << progress;
    // qDebug() << QString::number(m_list.count()) +" cells in model, out of " + QString::number(totalcells);

}

double DceListModel::getProgress()
{
    return progress;
}

void DceListModel::attributeSort()
{


}

void DceListModel::clearAndRequest(int type)
{
    clearing = true;
    gridType = type;
    QApplication::processEvents(QEventLoop::AllEvents);
    emit modelAboutToBeReset();
    beginResetModel();
    if( resetInternalData()){
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

void DceListModel::clearForPaging()
{
    if(m_list.size() > 0){


        if(resetInternalData()){
            emit modelAboutToBeReset();
            beginResetModel();
            setProgress(0.0);
            endResetModel();
            emit modelReset();
        }
    }
    emit pagingCleared();
}



