#include "devicemodel.h"
#include <QDebug>

DeviceModel::DeviceModel(AvDevice *prototype, QObject *parent) :
    QAbstractListModel(parent), m_prototype(prototype)
{
#ifndef QT5
    setRoleNames(m_prototype->roleNames());
#endif
    qRegisterMetaType<QModelIndex>("QModelIndex");
   totalDevices = 0;
   activeDevices = 0;
    loadingStatus = false;
    progress = 0;
    resetting = false;

}

int DeviceModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid()){
        return 0;
    }
    else{
        return m_list.size();
    }

}

QVariant DeviceModel::data(const QModelIndex &index, int role) const
{
    if(index.row() < 0 || index.row() >= m_list.size())
        return QVariant();
    return m_list.at(index.row())->data(role);
}

QHash<int, QByteArray> DeviceModel::roleNames() const
{
    return m_prototype->roleNames();
}


DeviceModel::~DeviceModel() {
    delete m_prototype;
    clear();
}

void DeviceModel::appendRow(AvDevice *item)
{
    setLoadingStatus(true);
    appendRows(QList<AvDevice*>() << new AvDevice(item->deviceNumber(), item->id(), item->controlled(), item->located(), item->device_Type(), item->activity(), this));
    item->deleteLater();
}

void DeviceModel::appendRows(const QList<AvDevice *> &items)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount()+items.size()-1);
    foreach(AvDevice *item, items) {
        m_list.append(item);
        QObject::connect(item, SIGNAL(dataChanged()), this , SLOT(handleItemChange()));
    }

    endInsertRows();
    QModelIndex index = indexFromItem(m_list.last());
    QModelIndex index2 = indexFromItem(m_list.first());
    int currentRows= m_list.count();
    //emit itemAdded(currentRows);
   // setCurrentCells(currentRows);
    //double p = (((double)m_list.size() / (double)seperator) * 100) ;
  //  setProgress(p);
    emit dataChanged(index2, index, currentRows);
    setLoadingStatus(false);


}

void DeviceModel::insertRow(int row, AvDevice *item)
{

    beginInsertRows(QModelIndex(), row, row);
    connect(item, SIGNAL(dataChanged()), this, SLOT(handleItemChange()));
    ////qdebug() << "Inserting at:" << row;
    m_list.insert(row, item);
    endInsertRows();

}

void DeviceModel::handleItemChange()
{
    AvDevice* item = static_cast<AvDevice*>(sender());
    QModelIndex index = indexFromItem(item);
    qDebug() << "Handling item change for:" << index;
    if(index.isValid())
    {
        emit dataChanged(index, index, index.row());
    }
}

void DeviceModel::reset()
{
    resetting = true;

    emit modelAboutToBeReset();
    beginResetModel();
    resetInternalData();
    setProgress(0.0);

    endResetModel();
    emit modelReset();

   resetting = false;
}

bool DeviceModel::resetInternalData()
{
    qDebug("Resetting DeviceModel data");
    int total = m_list.count();
    for(int i = 0; i <= m_list.count(); ++i){

        qDebug() <<"removing::" << m_list.count() << " of " << total;
        m_list.removeAt(i);
    }
    m_list.clear();
    qDebug() << "Items cleared. Count:: "<< m_list.count();
    return true;

}

AvDevice * DeviceModel::find(const QString &id) const
{
    foreach(AvDevice* item, m_list) {
        if(item->id() == id) return item;
    }
    return 0;
}

QVariant DeviceModel::get(int index, const QString &name) const
{
    if (index>=0 && index<m_list.size()) {
        AvDevice* myItem = m_list.at(index);

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

QModelIndex DeviceModel::indexFromItem(const AvDevice *item) const
{
    Q_ASSERT(item);
    for(int row=0; row<m_list.size(); ++row) {

        if(m_list.at(row) == item) return index(row);

    }

    return QModelIndex();
}

bool DeviceModel::clear()
{
    resetting = true;
    emit modelAboutToBeReset();
    beginResetModel();
    if(resetInternalData()){
    setProgress(0.0);
    endResetModel();
    emit modelReset();
    resetting = false;
    return true;
    }

}

bool DeviceModel::removeRow(int row, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    if(row < 0 || row >= m_list.size()) return false;
    beginRemoveRows(QModelIndex(), row, row);
    delete m_list.takeAt(row);
    endRemoveRows();
    return true;
}

bool DeviceModel::removeRows(int row, int count, const QModelIndex &parent)
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

AvDevice * DeviceModel::takeRow(int row)
{
    beginRemoveRows(QModelIndex(), row, row);
    AvDevice* item = m_list.takeAt(row);
    endRemoveRows();
    return item;
}

AvDevice * DeviceModel::currentRow()
{
    AvDevice* item = m_list.at(0);
    return item;
}

void DeviceModel::sortModel(int column, Qt::SortOrder order)
{
}

AvDevice* DeviceModel::getItem(int itmIndex)
{
    if(m_list.length() <= itmIndex){
        return m_list.at(itmIndex);
    }

    return new AvDevice();
}
