#include "mediatypemodel.h"
#include <QDebug>


MediaTypeModel::MediaTypeModel(MediaTypeItem* prototype, QObject *parent) :
    QAbstractListModel(parent), m_prototype(prototype)
{
  setRoleNames(m_prototype->roleNames());
   qRegisterMetaType<QModelIndex>("QModelIndex");
}

int MediaTypeModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return m_list.size();
}

QVariant MediaTypeModel::data(const QModelIndex &index, int role) const
{
  if(index.row() < 0 || index.row() >= m_list.size())
    return QVariant();
  return m_list.at(index.row())->data(role);
}

MediaTypeModel::~MediaTypeModel() {
  delete m_prototype;
  clear();
}

void MediaTypeModel::appendRow(MediaTypeItem *item)
{
  appendRows(QList<MediaTypeItem*>() << item);
}

void MediaTypeModel::appendRows(const QList<MediaTypeItem *> &items)
{
  beginInsertRows(QModelIndex(), rowCount(), rowCount()+items.size()-1);
  foreach(MediaTypeItem *item, items) {

      QObject::connect(item, SIGNAL(filterChanged()), this, SLOT(handleItemChange()));
    m_list.append(item);
  }

  endInsertRows();
  //QModelIndex index = indexFromItem(m_list.last());
  //QModelIndex index2 = indexFromItem(m_list.first());
  //int currentRows= m_list.count() - 1;
  //emit dataChanged(index2, index, currentRows);

}

void MediaTypeModel::insertRow(int row, MediaTypeItem *item)
{
  beginInsertRows(QModelIndex(), row, row);
  connect(item, SIGNAL(dataChanged()), this, SLOT(handleItemChange()));
  qDebug() << "Inserting at:" << row;
  m_list.insert(row, item);
  endInsertRows();
}

void MediaTypeModel::handleItemChange()
{
  MediaTypeItem* item = static_cast<MediaTypeItem*>(sender());
  QModelIndex index = indexFromItem(item);
 // qDebug() << "Handling item change for:" << index;
  if(index.isValid())
  {
     QModelIndex lastrow;
     ident = item->fileformat();
     lastrow = index;
    emit dataChanged(index, index ,item->selectedStatus());
  }
}

MediaTypeItem * MediaTypeModel::find(const QString &id) const
{
  foreach(MediaTypeItem* item, m_list) {
    if(item->id() == id) return item;
  }
  return 0;
}

QModelIndex MediaTypeModel::indexFromItem(const MediaTypeItem *item) const
{
  Q_ASSERT(item);
  for(int row=0; row<m_list.size(); ++row) {

    if(m_list.at(row) == item) return index(row);

  }

  return QModelIndex();
}

void MediaTypeModel::clear()
{

  qDeleteAll(m_list);
  m_list.clear();
  this->reset();

}

bool MediaTypeModel::removeRow(int row, const QModelIndex &parent)
{
  Q_UNUSED(parent);
  if(row < 0 || row >= m_list.size()) return false;
  beginRemoveRows(QModelIndex(), row, row);
  delete m_list.takeAt(row);
  endRemoveRows();
  return true;
}

bool MediaTypeModel::removeRows(int row, int count, const QModelIndex &parent)
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

MediaTypeItem * MediaTypeModel::takeRow(int row)
{
  beginRemoveRows(QModelIndex(), row, row);
  MediaTypeItem* item = m_list.takeAt(row);
  endRemoveRows();
  return item;
}

MediaTypeItem * MediaTypeModel::currentRow()
{
    MediaTypeItem* item = m_list.at(0);
    return item;
}

void MediaTypeModel::setSelectionStatus(QString format)
{
    MediaTypeItem* item = find(format);
    item->updateSelection(false);
    qDebug() << "Setting State for:" << format;
    //return state;
}

bool MediaTypeModel::getSelectionStatus()
{
    //qDebug() << "Looking for status for" << format;
    //MediaTypeItem* item = find(format);

    bool g;//  = item->selectedStatus();
    return g;

}


