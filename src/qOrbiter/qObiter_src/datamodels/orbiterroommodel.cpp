#include "orbiterroommodel.h"


OrbiterRoomModel::OrbiterRoomModel(OrbiterRowModel* prototype, QObject *parent) :
    QAbstractListModel(parent), m_prototype(prototype)
{
  setRoleNames(m_prototype->roleNames());
}

int OrbiterRoomModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return m_list.size();
}

QVariant OrbiterRoomModel::data(const QModelIndex &index, int role) const
{
  if(index.row() < 0 || index.row() >= m_list.size())
    return QVariant();
  return m_list.at(index.row())->data(role);
}

OrbiterRoomModel::~OrbiterRoomModel() {
  delete m_prototype;
  clear();
}

void OrbiterRoomModel::appendRow(OrbiterRowModel *item)
{
  appendRows(QList<OrbiterRowModel*>() << item);
}

void OrbiterRoomModel::appendRows(const QList<OrbiterRowModel *> &items)
{
  beginInsertRows(QModelIndex(), rowCount(), rowCount()+items.size()-1);
  foreach(OrbiterRowModel *item, items) {
    connect(item, SIGNAL(dataChanged()), SLOT(handleItemChange()));
    m_list.append(item);
  }
  endInsertRows();
}

void OrbiterRoomModel::insertRow(int row, OrbiterRowModel *item)
{
  beginInsertRows(QModelIndex(), row, row);
  connect(item, SIGNAL(dataChanged()), SLOT(handleItemChange()));
  m_list.insert(row, item);
  endInsertRows();
}

void OrbiterRoomModel::handleItemChange()
{
  OrbiterRowModel* item = static_cast<OrbiterRowModel*>(sender());
  QModelIndex index = indexFromItem(item);
  if(index.isValid())
    emit dataChanged(index, index);
}

OrbiterRowModel * OrbiterRoomModel::find(const QString &id) const
{
  foreach(OrbiterRowModel* item, m_list) {
    if(item->id() == id) return item;
  }
  return 0;
}

QModelIndex OrbiterRoomModel::indexFromItem(const OrbiterRowModel *item) const
{
  Q_ASSERT(item);
  for(int row=0; row<m_list.size(); ++row) {
    if(m_list.at(row) == item) return index(row);
  }
  return QModelIndex();
}

void OrbiterRoomModel::clear()
{
  qDeleteAll(m_list);
  m_list.clear();
}

bool OrbiterRoomModel::removeRow(int row, const QModelIndex &parent)
{
  Q_UNUSED(parent);
  if(row < 0 || row >= m_list.size()) return false;
  beginRemoveRows(QModelIndex(), row, row);
  delete m_list.takeAt(row);
  endRemoveRows();
  return true;
}

bool OrbiterRoomModel::removeRows(int row, int count, const QModelIndex &parent)
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

OrbiterRowModel * OrbiterRoomModel::takeRow(int row)
{
  beginRemoveRows(QModelIndex(), row, row);
  OrbiterRowModel* item = m_list.takeAt(row);
  endRemoveRows();
  return item;
}
