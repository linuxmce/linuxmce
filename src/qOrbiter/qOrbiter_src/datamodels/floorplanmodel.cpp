#include "floorplanmodel.h"
#include <QDebug>
FloorPlanModel::FloorPlanModel(FloorPlanItem* prototype, QObject *parent) :
    QAbstractListModel(parent), m_prototype(prototype)
{
    setRoleNames(m_prototype->roleNames());
     qRegisterMetaType<QModelIndex>("QModelIndex");
}

int FloorPlanModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return m_list.size();
}

QVariant FloorPlanModel::data(const QModelIndex &index, int role) const
{

  if(index.row() < 0 || index.row() >= m_list.size())
    return QVariant();
  return m_list.at(index.row())->data(role);
}

/*
DataGridItemModelClass::~DataGridItemModelClass() {
  delete m_prototype;
  clear();
}
*/

void FloorPlanModel::appendRow(FloorPlanItem *item)
{
  appendRows(QList<FloorPlanItem*>() << item);
}

void FloorPlanModel::appendRows(const QList<FloorPlanItem *> &items)
{
  beginInsertRows(QModelIndex(), rowCount(), rowCount()+items.size()-1);
  foreach(FloorPlanItem *item, items) {

   QObject::connect(item, SIGNAL(dataChanged()), this, SLOT(handleItemChange()));
    m_list.append(item);
  }

  endInsertRows();
  QModelIndex index = indexFromItem(m_list.last());
  QModelIndex index2 = indexFromItem(m_list.first());
  int currentRows= m_list.count() - 1;
  emit dataChanged(index2, index);

}

void FloorPlanModel::insertRow(int row, FloorPlanItem *item)
{
  beginInsertRows(QModelIndex(), row, row);
  connect(item, SIGNAL(dataChanged()), this, SLOT(handleItemChange()));
  //qDebug() << "Inserting at:" << row;
  m_list.insert(row, item);
  endInsertRows();
}

void FloorPlanModel::handleItemChange()
{
  FloorPlanItem* item = static_cast<FloorPlanItem*>(sender());
  QModelIndex index = indexFromItem(item);
  //qDebug() << "Handling item change for:" << index;
  if(index.isValid())
  {
    emit dataChanged(index, index);
  }
}

FloorPlanItem * FloorPlanModel::find(const QString &id) const
{
  foreach(FloorPlanItem* item, m_list) {

    if(item->id() == id) qDebug() << item->id();return item;
  }
  return 0;
}

QModelIndex FloorPlanModel::indexFromItem(const FloorPlanItem *item) const
{
  Q_ASSERT(item);
  for(int row=0; row<m_list.size(); ++row) {
 //qDebug() << "Row:" << row << "::" << m_list.at(row)->id();
      if(m_list.at(row) == item) {

          return index(row,row,QModelIndex());
      }

  }
 qDebug("fail!");
  return QModelIndex();
}

void FloorPlanModel::clear()
{

  qDeleteAll(m_list);
  m_list.clear();
  this->reset();

}

bool FloorPlanModel::removeRow(int row, const QModelIndex &parent)
{
  Q_UNUSED(parent);
  if(row < 0 || row >= m_list.size()) return false;
  beginRemoveRows(QModelIndex(), row, row);
  delete m_list.takeAt(row);
  endRemoveRows();
  return true;
}

bool FloorPlanModel::removeRows(int row, int count, const QModelIndex &parent)
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

FloorPlanItem * FloorPlanModel::takeRow(int row)
{
  beginRemoveRows(QModelIndex(), row, row);
  FloorPlanItem* item = m_list.takeAt(row);
  endRemoveRows();
  return item;
}

FloorPlanItem * FloorPlanModel::currentRow()
{
    FloorPlanItem* item = m_list.at(0);
    return item;
}

QImage FloorPlanModel::getPageImage(QString &id)
{
    qDebug() << "Seeking page id:" << id;
    FloorPlanItem *myItem = find(id);
    if (!myItem)
    { QImage fail;
        return fail;
    }
    qDebug() << "Found it!";

     QImage fpImage= myItem->floorplanImage();
     qDebug () << fpImage.isNull();
    return fpImage;
}



void FloorPlanModel::setCurrentPage(QString currentPageId)
{
 currentPage = currentPageId;
 qDebug() << "image set to" << currentPageId;
emit pageChanged();
}

/*
QModelIndex FloorPlanModel::index(int row, int column, const QModelIndex &parent) const
{
}

QModelIndex FloorPlanModel::parent(const QModelIndex &child) const
{
}

int FloorPlanModel::columnCount(const QModelIndex &parent) const
{

}
*/
