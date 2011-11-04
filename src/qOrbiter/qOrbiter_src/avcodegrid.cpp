#include "avcodegrid.h"

AvCodeGrid::AvCodeGrid(AvItem* prototype, QObject *parent) :
    QAbstractItemModel(parent), m_prototype(prototype)
{
    setRoleNames(m_prototype->roleNames());
     qRegisterMetaType<QModelIndex>("QModelIndex");
}

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

/*
AvCodeGrid::~AvCodeGrid() {
  delete m_prototype;
  clear();
}
*/

void AvCodeGrid::appendRow(AvItem *item)
{
  appendRows(QList<AvItem*>() << item);
}

void AvCodeGrid::appendRows(const QList<AvItem *> &items)
{
  beginInsertRows(QModelIndex(), rowCount(), rowCount()+items.size()-1);
  foreach(AvItem *item, items) {

   QObject::connect(item, SIGNAL(dataChanged()), this, SLOT(handleItemChange()));
    m_list.append(item);
  }

  endInsertRows();
  QModelIndex index = indexFromItem(m_list.last());
  QModelIndex index2 = indexFromItem(m_list.first());
  int currentRows= m_list.count() - 1;
  //emit dataChanged(index2, index, currentRows);

}

void AvCodeGrid::insertRow(int row, AvItem *item)
{
  beginInsertRows(QModelIndex(), row, row);
  connect(item, SIGNAL(dataChanged()), this, SLOT(handleItemChange()));
  //qDebug() << "Inserting at:" << row;
  m_list.insert(row, item);
  endInsertRows();
}

void AvCodeGrid::handleItemChange()
{
  AvItem* item = static_cast<AvItem*>(sender());
  QModelIndex index = indexFromItem(item);
  //qDebug() << "Handling item change for:" << index;
  if(index.isValid())
  {
    emit dataChanged(index, index);
  }
}

AvItem * AvCodeGrid::find(const QString &id) const
{
  foreach(AvItem* item, m_list) {
    if(item->id() == id) return item;
  }
  return 0;
}

QModelIndex AvCodeGrid::indexFromItem(const AvItem *item) const
{
  Q_ASSERT(item);
  for(int row=0; row<m_list.size(); ++row) {

      if(m_list.at(row) == item) return index(row,row,QModelIndex());

  }

  return QModelIndex();
}

void AvCodeGrid::clear()
{

  qDeleteAll(m_list);
  m_list.clear();
  this->reset();

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

AvItem * AvCodeGrid::takeRow(int row)
{
  beginRemoveRows(QModelIndex(), row, row);
  AvItem* item = m_list.takeAt(row);
  endRemoveRows();
  return item;
}

AvItem * AvCodeGrid::currentRow()
{
    AvItem* item = m_list.at(0);
    return item;
}
