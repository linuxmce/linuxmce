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
#include "avcodegrid.h"


AvCodeGrid::AvCodeGrid(AvCommand* prototype, QObject *parent) :
    QAbstractListModel(parent), m_prototype(prototype)
{
#ifndef QT5
    setRoleNames(m_prototype->roleNames());
#endif

    qRegisterMetaType<QModelIndex>("QModelIndex");
}

#ifdef QT5
QHash<int, QByteArray> AvCodeGrid::roleNames() const
{
    return m_prototype->roleNames();
}
#endif

int AvCodeGrid::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_list.size();
}

QVariant AvCodeGrid::data(const QModelIndex &index, int role) const
{
    if(index.row() < 0 || index.row() >= m_list.size())
        return QVariant();
    return m_list.at(index.row())->data(role);
}

void AvCodeGrid::appendRow(AvCommand *item)
{
    appendRows(QList<AvCommand*>() << new AvCommand(item->command(), item->name(), item->learnState(), item->cmdParent()));
    item->deleteLater();
}

void AvCodeGrid::appendRows(const QList<AvCommand *> &items)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount()+items.size()-1);
    foreach(AvCommand *item, items) {
       // qDebug() << "Inserting at:" << rowCount() << item->cmdParent();
        QObject::connect(item, SIGNAL(dataChanged()), this, SLOT(handleItemChange()));
        m_list.append(item);
    }

    endInsertRows();
    QModelIndex index = indexFromItem(m_list.last());
    QModelIndex index2 = indexFromItem(m_list.first());
    int currentRows= m_list.count() - 1;
    emit dataChanged(index2, index, currentRows);
}

void AvCodeGrid::insertRow(int row, AvCommand *item)
{
    beginInsertRows(QModelIndex(), row, row);
    connect(item, SIGNAL(dataChanged()), this, SLOT(handleItemChange()));
    m_list.insert(row, item);
    endInsertRows();
}

void AvCodeGrid::handleItemChange()
{
    AvCommand* item = static_cast<AvCommand*>(sender());
    QModelIndex index = indexFromItem(item);
   // qDebug() << "Handling item change for:" << index;
    if(index.isValid())
    {
        emit dataChanged(index, index, 0);
        emit deviceAdded();
    }
}

AvCommand * AvCodeGrid::find(const QString &id) const
{
    foreach(AvCommand* item, m_list) {
        if(item->command() == id.toInt()) return item;            //note this line is different in that it keys on device number, which i have mapped to a field named device_number in the class AvCommand
    }
    return 0;
}

QModelIndex AvCodeGrid::indexFromItem(const AvCommand *item) const
{
    Q_ASSERT(item);
    for(int row=0; row<m_list.size(); ++row) {

        if(m_list.at(row) == item) return index(row,row,QModelIndex());

    }

    return QModelIndex();
}

void AvCodeGrid::clear()
{
    this->reset();
}

void AvCodeGrid::sortModel(int column, Qt::SortOrder order)
{

}

bool AvCodeGrid::removeRow(int row, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    if(row < 0 || row >= m_list.size()) return false;
    beginRemoveRows(QModelIndex(), row, row);
    delete m_list.takeAt(row);
    endRemoveRows();
    return true;
}

bool AvCodeGrid::removeRows(int row, int count, const QModelIndex &parent)
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

AvCommand * AvCodeGrid::takeRow(int row)
{
    beginRemoveRows(QModelIndex(), row, row);
    AvCommand* item = m_list.takeAt(row);
    endRemoveRows();
    return item;
}

AvCommand * AvCodeGrid::currentRow()
{
    AvCommand* item = m_list.at(0);
    return item;
}

void AvCodeGrid::reset()
{   
    emit modelAboutToBeReset();
    emit beginResetModel();
    if(resetInternalData()){
        emit endResetModel();
        emit modelReset();
    }
}

bool AvCodeGrid::resetInternalData()
{
    for(int i = 0; i <= m_list.count(); ++i){
        m_list.removeAt(i);
    }
    m_list.clear();
    return true;
}

