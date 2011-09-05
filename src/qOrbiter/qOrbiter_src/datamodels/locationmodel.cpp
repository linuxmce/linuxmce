#include "locationmodel.h"

LocationModel::LocationModel(LocationItem* prototype, QObject *parent) :
    QAbstractListModel(parent), m_prototype(prototype)
{
  setRoleNames(m_prototype->roleNames());
}

int LocationModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return m_list.size();
}

QVariant LocationModel::data(const QModelIndex &index, int role) const
{
  if(index.row() < 0 || index.row() >= m_list.size())
    return QVariant();
  return m_list.at(index.row())->data(role);
}

LocationModel::~LocationModel() {
  delete m_prototype;
  clear();
}

void LocationModel::appendRow(LocationItem *item)
{
  appendRows(QList<LocationItem*>() << item);
}

void LocationModel::appendRows(const QList<LocationItem *> &items)
{
  beginInsertRows(QModelIndex(), rowCount(), rowCount()+items.size()-1);
  foreach(LocationItem *item, items) {
    //connect(item, SIGNAL(dataChanged()), SLOT(handleItemChange()));
    m_list.append(item);
  }
  endInsertRows();
}

void LocationModel::insertRow(int row, LocationItem *item)
{
  beginInsertRows(QModelIndex(), row, row);
  connect(item, SIGNAL(dataChanged()), SLOT(handleItemChange()));
 // m_list.insert(row, item);
  endInsertRows();
}

void LocationModel::handleItemChange()
{
  LocationItem* item = static_cast<LocationItem*>(sender());
  QModelIndex index = indexFromItem(item);
  if(index.isValid())
    emit dataChanged(index, index);
}

LocationItem * LocationModel::find(const QString &id) const
{
  foreach(LocationItem* item, m_list) {
    if(item->id() == id) return item;
  }
  return 0;
}

QModelIndex LocationModel::indexFromItem(const LocationItem *item) const
{
  Q_ASSERT(item);
  for(int row=0; row<m_list.size(); ++row) {
    if(m_list.at(row) == item) return index(row);
  }
  return QModelIndex();
}

void LocationModel::clear()
{
  qDeleteAll(m_list);
  m_list.clear();
}

bool LocationModel::removeRow(int row, const QModelIndex &parent)
{
  Q_UNUSED(parent);
  if(row < 0 || row >= m_list.size()) return false;
  beginRemoveRows(QModelIndex(), row, row);
  delete m_list.takeAt(row);
  endRemoveRows();
  return true;
}

bool LocationModel::removeRows(int row, int count, const QModelIndex &parent)
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

LocationItem * LocationModel::takeRow(int row)
{
  beginRemoveRows(QModelIndex(), row, row);
  LocationItem* item = m_list.takeAt(row);
  endRemoveRows();
  return item;
}
