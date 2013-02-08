#include "orbitermodel.h"

OrbiterModel::OrbiterModel(OrbiterRoomModel* prototype, QObject *parent) :
    QAbstractListModel(parent), m_prototype(prototype)
{
 #ifndef QT5
    setRoleNames(m_prototype->roleNames());
#endif
}

int OrbiterModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return m_list.size();
}

QVariant OrbiterModel::data(const QModelIndex &index, int role) const
{
  if(index.row() < 0 || index.row() >= m_list.size())
    return QVariant();
  return m_list.at(index.row())->data(role);
}

OrbiterModel::~OrbiterModel() {
  delete m_prototype;
  clear();
}

void OrbiterModel::appendRow(OrbiterRoomModel *item)
{
  appendRows(QList<OrbiterRoomModel*>() << item);
}

void OrbiterModel::appendRows(const QList<OrbiterRoomModel *> &items)
{
  beginInsertRows(QModelIndex(), rowCount(), rowCount()+items.size()-1);
  foreach(OrbiterRoomModel *item, items) {
    connect(item, SIGNAL(dataChanged()), SLOT(handleItemChange()));
    m_list.append(item);
  }
  endInsertRows();
}

void OrbiterModel::insertRow(int row, OrbiterRoomModel *item)
{
  beginInsertRows(QModelIndex(), row, row);
  connect(item, SIGNAL(dataChanged()), SLOT(handleItemChange()));
  m_list.insert(row, item);
  endInsertRows();
}

void OrbiterModel::handleItemChange()
{
  OrbiterRoomModel* item = static_cast<OrbiterRoomModel*>(sender());
  QModelIndex index = indexFromItem(item);
  if(index.isValid())
    emit dataChanged(index, index);
}

OrbiterRoomModel *OrbiterModel ::find(const QString &id) const
{
  foreach(OrbiterRoomModel* item, m_list) {
    if(item->id() == id) return item;
  }
  return 0;
}

QModelIndex OrbiterModel::indexFromItem(const OrbiterRoomModel *item) const
{
  Q_ASSERT(item);
  for(int row=0; row<m_list.size(); ++row) {
    if(m_list.at(row) == item) return index(row);
  }
  return QModelIndex();
}

void OrbiterModel::clear()
{
  qDeleteAll(m_list);
  m_list.clear();
}

bool OrbiterModel::removeRow(int row, const QModelIndex &parent)
{
  Q_UNUSED(parent);
  if(row < 0 || row >= m_list.size()) return false;
  beginRemoveRows(QModelIndex(), row, row);
  delete m_list.takeAt(row);
  endRemoveRows();
  return true;
}

bool OrbiterModel::removeRows(int row, int count, const QModelIndex &parent)
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

OrbiterRoomModel *OrbiterModel ::takeRow(int row)
{
  beginRemoveRows(QModelIndex(), row, row);
  OrbiterListItem* item = m_list.takeAt(row);
  endRemoveRows();
  return item;
}
