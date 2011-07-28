#include "skindatamodel.h"
#include <QDebug>


SkinDataModel::SkinDataModel(SkinDataItem* prototype, QObject* parent): QAbstractListModel(parent), m_prototype(prototype)
{
    setRoleNames(m_prototype->roleNames());
}


int SkinDataModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return m_list.size();
}

QVariant SkinDataModel::data(const QModelIndex &index, int role) const
{
  if(index.row() < 0 || index.row() >= m_list.size())
    return QVariant();
  return m_list.at(index.row())->data(role);
}

SkinDataModel::~SkinDataModel() {
  delete m_prototype;
  clear();
}

void SkinDataModel::appendRow(SkinDataItem *item)
{
  appendRows(QList<SkinDataItem*>() << item);
}

void SkinDataModel::appendRows(const QList<SkinDataItem *> &items)
{
  beginInsertRows(QModelIndex(), rowCount(), rowCount()+items.size()-1);
  foreach(SkinDataItem *item, items) {
    //connect(item, SIGNAL(dataChanged()), SLOT(handleItemChange()));
    m_list.append(item);
  }
  endInsertRows();
}

void SkinDataModel::insertRow(int row, SkinDataItem *item)
{
  beginInsertRows(QModelIndex(), row, row);
 // connect(item, SIGNAL(dataChanged()), SLOT(handleItemChange()));
  m_list.insert(row, item);
  endInsertRows();
}

void SkinDataModel::handleItemChange()
{
  SkinDataItem* item = static_cast<SkinDataItem*>(sender());
  QModelIndex index = indexFromItem(item);
  if(index.isValid())
    emit dataChanged(index, index);
}

SkinDataItem * SkinDataModel::find(const QString &id) const
{
  foreach(SkinDataItem* item, m_list) {
    if(item->id() == id) return item;
  }
  return 0;
}

QModelIndex SkinDataModel::indexFromItem(const SkinDataItem *item) const
{
  Q_ASSERT(item);
  for(int row=0; row<m_list.size(); ++row) {
    if(m_list.at(row) == item) return index(row);
  }
  return QModelIndex();
}

void SkinDataModel::clear()
{
  qDeleteAll(m_list);
  m_list.clear();
}

bool SkinDataModel::removeRow(int row, const QModelIndex &parent)
{
  Q_UNUSED(parent);
  if(row < 0 || row >= m_list.size()) return false;
  beginRemoveRows(QModelIndex(), row, row);
  delete m_list.takeAt(row);
  endRemoveRows();
  return true;
}

bool SkinDataModel::removeRows(int row, int count, const QModelIndex &parent)
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

SkinDataItem * SkinDataModel::takeRow(int row)
{
  beginRemoveRows(QModelIndex(), row, row);
  SkinDataItem* item = m_list.takeAt(row);
  endRemoveRows();
  return item;
}


