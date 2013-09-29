#include "sleepingalarmmodel.h"
#include <QDebug>

SleepingAlarmModel::SleepingAlarmModel(QObject *parent) :
    QAbstractListModel(parent)
{
#ifndef QT5
   setRoleNames(m_prototype->roleNames());
#endif
}


int SleepingAlarmModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return m_list.size();
}

QVariant SleepingAlarmModel::data(const QModelIndex &index, int role) const
{
  if(index.row() < 0 || index.row() >= m_list.size())
    return QVariant();
  return m_list.at(index.row())->data(role);
}

SleepingAlarmModel::~SleepingAlarmModel() {

     m_prototype = NULL;
}

void SleepingAlarmModel::appendRow(SleepingAlarm *item)
{
  appendRows(QList<SleepingAlarm*>() << item);
}

void SleepingAlarmModel::appendRows(const QList<SleepingAlarm *> &items)
{
  beginInsertRows(QModelIndex(), rowCount(), rowCount()+items.size()-1);
  foreach(SleepingAlarm *item, items) {
    connect(item, SIGNAL(dataChanged()), SLOT(handleItemChange()));
    m_list.append(item);
  }
  endInsertRows();
}

void SleepingAlarmModel::insertRow(int row, SleepingAlarm *item)
{
  beginInsertRows(QModelIndex(), row, row);
  connect(item, SIGNAL(dataChanged()), SLOT(handleItemChange()));
 // m_list.insert(row, item);
  endInsertRows();
}

void SleepingAlarmModel::handleItemChange()
{
  SleepingAlarm* item = static_cast<SleepingAlarm*>(sender());
  QModelIndex index = indexFromItem(item);
  if(index.isValid())
      emit dataChanged(index, index);

  qDebug()<< "Sleeping alarms model changed";
}

bool SleepingAlarmModel::resetInternalData()
{
    qDebug("Resetting Alarms data");
//    for(int i = 0; i <= m_list.count(); ++i){
//        m_list.removeAt(i);
//    }
    m_list.erase(m_list.begin(), m_list.end());
    if(m_list.empty())
        return true;
    else
        return false;
}

SleepingAlarm * SleepingAlarmModel::find(const QString &id) const
{
  foreach(SleepingAlarm* item, m_list) {
    if(item->id() == id) return item;
  }
  return 0;
}

void SleepingAlarmModel::sortModel(int column, Qt::SortOrder order)
{
}

#ifdef QT5
QHash<int, QByteArray> SleepingAlarmModel::roleNames() const
{
    return m_prototype->roleNames();
}
#endif

QModelIndex SleepingAlarmModel::indexFromItem(const SleepingAlarm *item) const
{
  Q_ASSERT(item);
  for(int row=0; row<m_list.size(); ++row) {
    if(m_list.at(row) == item) return index(row);
  }
  return QModelIndex();
}

void SleepingAlarmModel::clear()
{
    qDebug() << "Clearing alarms";
    emit modelAboutToBeReset();
    beginResetModel();
    if(resetInternalData()){

    }
    else
    {
        qDebug() << "Could clear alarm model! -- " << m_list.size() << " items.";
    }
    endResetModel();
    emit modelReset();

}



bool SleepingAlarmModel::removeRow(int row, const QModelIndex &parent)
{
  Q_UNUSED(parent);
  if(row < 0 || row >= m_list.size()) return false;
  beginRemoveRows(QModelIndex(), row, row);
  delete m_list.takeAt(row);
  endRemoveRows();
  return true;
}

bool SleepingAlarmModel::removeRows(int row, int count, const QModelIndex &parent)
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

SleepingAlarm * SleepingAlarmModel::takeRow(int row)
{
  beginRemoveRows(QModelIndex(), row, row);
  SleepingAlarm* item = m_list.takeAt(row);
  endRemoveRows();
  return item;
}
