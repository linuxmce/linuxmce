#include "floorplanmodel.h"

FloorPlanModel::FloorPlanModel(FloorplanDevice* prototype, QObject *parent) :
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

void FloorPlanModel::appendRow(FloorplanDevice *item)
{
  appendRows(QList<FloorplanDevice*>() << item);
}

void FloorPlanModel::appendRows(const QList<FloorplanDevice *> &items)
{
  beginInsertRows(QModelIndex(), rowCount(), rowCount()+items.size()-1);
  foreach(FloorplanDevice *item, items) {
/*
   QObject::connect(item, SIGNAL(dataChanged()), this, SLOT(handleItemChange()));
    m_list.append(item);
    */
  }

  endInsertRows();
  /*
  QModelIndex index = indexFromItem(m_list.last());
  QModelIndex index2 = indexFromItem(m_list.first());
  int currentRows= m_list.count() - 1;
  emit dataChanged(index2, index);
*/
}

void FloorPlanModel::insertRow(int row, FloorplanDevice *item)
{
  beginInsertRows(QModelIndex(), row, row);
  connect(item, SIGNAL(dataChanged()), this, SLOT(handleItemChange()));
  //qDebug() << "Inserting at:" << row;
  m_list.insert(row, item);
  endInsertRows();
}

void FloorPlanModel::handleItemChange()
{
  FloorplanDevice* item = static_cast<FloorplanDevice*>(sender());
  QModelIndex index = indexFromItem(item);
  //qDebug() << "Handling item change for:" << index;
  if(index.isValid())
  {
    emit dataChanged(index, index);
  }
}

FloorplanDevice * FloorPlanModel::find(const QString &id) const
{
  foreach(FloorplanDevice* item, m_list) {

      if(item->id().contains(id))
      {
          //qDebug() << "Found Match of: " << item->id() << "to " << id;
          return item;
      }
      else
      {
        //  qDebug() << item->id();
      }
  }
  return 0;
}

QModelIndex FloorPlanModel::indexFromItem(const FloorplanDevice *item) const
{
  Q_ASSERT(item);
  for(int row=0; row<m_list.size(); ++row) {
 //qDebug() << "Row:" << row << "::" << m_list.at(row)->id();
      if(m_list.at(row) == item) {

          return index(row,row,QModelIndex());
      }

  }

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

FloorplanDevice * FloorPlanModel::takeRow(int row)
{
  beginRemoveRows(QModelIndex(), row, row);
  FloorplanDevice* item = m_list.takeAt(row);
  endRemoveRows();
  return item;
}

FloorplanDevice * FloorPlanModel::currentRow()
{
    FloorplanDevice* item = m_list.at(0);
    return item;
}

QImage FloorPlanModel::getPageImage(QString &id)
{
   return currentImage;
}

QString FloorPlanModel::getCurrentImagePath()
{
    return imageBasePath+currentPage+".png";
}



void FloorPlanModel::setCurrentPage(QString currentPageId)
{

 currentPage = currentPageId;
 QString s = getCurrentImagePath();
 emit pageChanged(s);
}

void FloorPlanModel::setImageData(const uchar *data, int iData_size)
{
        QImage t;
        if( t.loadFromData(data, iData_size))
        {
            setImage(t);
            emit floorPlanStatus("Converted Image");
        }
        else
        {
            emit floorPlanStatus("Update Object Image Conversion Failed:");
        }
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
