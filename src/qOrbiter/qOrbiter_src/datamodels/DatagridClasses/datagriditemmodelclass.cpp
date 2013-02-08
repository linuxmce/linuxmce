#include "datagriditemmodelclass.h"

DataGridItemModelClass::DataGridItemModelClass(DataGridItem* prototype, QObject *parent) :
    QAbstractItemModel(parent), m_prototype(prototype)
{
   #ifndef QT5
    setRoleNames(m_prototype->roleNames());
#endif
     qRegisterMetaType<QModelIndex>("QModelIndex");
}

int DataGridItemModelClass::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return m_list.size();
}

QVariant DataGridItemModelClass::data(const QModelIndex &index, int role) const
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

void DataGridItemModelClass::appendRow(DataGridItem *item)
{
  appendRows(QList<DataGridItem*>() << item);
}

void DataGridItemModelClass::appendRows(const QList<DataGridItem *> &items)
{
  beginInsertRows(QModelIndex(), rowCount(), rowCount()+items.size()-1);
  foreach(DataGridItem *item, items) {

   QObject::connect(item, SIGNAL(dataChanged()), this, SLOT(handleItemChange()));
    m_list.append(item);
  }

  endInsertRows();
  QModelIndex index = indexFromItem(m_list.last());
  QModelIndex index2 = indexFromItem(m_list.first());
  int currentRows= m_list.count() - 1;
  //emit dataChanged(index2, index, currentRows);

}

void DataGridItemModelClass::insertRow(int row, DataGridItem *item)
{
  beginInsertRows(QModelIndex(), row, row);
  connect(item, SIGNAL(dataChanged()), this, SLOT(handleItemChange()));
  //qDebug() << "Inserting at:" << row;
  m_list.insert(row, item);
  endInsertRows();
}

void DataGridItemModelClass::handleItemChange()
{
  DataGridItem* item = static_cast<DataGridItem*>(sender());
  QModelIndex index = indexFromItem(item);
  //qDebug() << "Handling item change for:" << index;
  if(index.isValid())
  {
    emit dataChanged(index, index);
  }
}

DataGridItem * DataGridItemModelClass::find(const QString &id) const
{
  foreach(DataGridItem* item, m_list) {
    if(item->id() == id) return item;
  }
  return 0;
}

QModelIndex DataGridItemModelClass::indexFromItem(const DataGridItem *item) const
{
  Q_ASSERT(item);
  for(int row=0; row<m_list.size(); ++row) {

      if(m_list.at(row) == item) return index(row,row,QModelIndex());

  }

  return QModelIndex();
}

void DataGridItemModelClass::clear()
{

  qDeleteAll(m_list);
  m_list.clear();
  #ifndef QT5
  this->reset();
#endif

}

void DataGridItemModelClass::sortModel(int column, Qt::SortOrder order)
{
}

bool DataGridItemModelClass::removeRow(int row, const QModelIndex &parent)
{
  Q_UNUSED(parent);
  if(row < 0 || row >= m_list.size()) return false;
  beginRemoveRows(QModelIndex(), row, row);
  delete m_list.takeAt(row);
  endRemoveRows();
  return true;
}

bool DataGridItemModelClass::removeRows(int row, int count, const QModelIndex &parent)
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

DataGridItem * DataGridItemModelClass::takeRow(int row)
{
  beginRemoveRows(QModelIndex(), row, row);
  DataGridItem* item = m_list.takeAt(row);
  endRemoveRows();
  return item;
}

DataGridItem * DataGridItemModelClass::currentRow()
{
    DataGridItem* item = m_list.at(0);
    return item;
}
