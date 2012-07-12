#include "mediatypemodel.h"
#ifdef debug
#include <QDebug>
#endif

MediaSubTypeModel::MediaSubTypeModel(MediaSubTypeItem* prototype, QObject *parent) :
    QAbstractListModel(parent), m_prototype(prototype)
{
  setRoleNames(m_prototype->roleNames());
   qRegisterMetaType<QModelIndex>("QModelIndex");

}

int MediaSubTypeModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return m_list.size();
}

QVariant MediaSubTypeModel::data(const QModelIndex &index, int role) const
{
  if(index.row() < 0 || index.row() >= m_list.size())
    return QVariant();
  return m_list.at(index.row())->data(role);
}

MediaSubTypeModel::~MediaSubTypeModel() {
  delete m_prototype;
  clear();
}

void MediaSubTypeModel::appendRow(MediaSubTypeItem *item)
{
  appendRows(QList<MediaSubTypeItem*>() << item);
}

void MediaSubTypeModel::appendRows(const QList<MediaSubTypeItem *> &items)
{
  beginInsertRows(QModelIndex(), rowCount(), rowCount()+items.size()-1);
  foreach(MediaSubTypeItem *item, items) {

      QObject::connect(item, SIGNAL(filterChanged()), this, SLOT(handleItemChange()));
    m_list.append(item);
  }

  endInsertRows();
  //QModelIndex index = indexFromItem(m_list.last());
  //QModelIndex index2 = indexFromItem(m_list.first());
  //int currentRows= m_list.count() - 1;
  //emit dataChanged(index2, index, currentRows);

}

void MediaSubTypeModel::insertRow(int row, MediaSubTypeItem *item)
{
  beginInsertRows(QModelIndex(), row, row);
  connect(item, SIGNAL(dataChanged()), this, SLOT(handleItemChange()));
  //qDebug() << "Inserting at:" << row;
  m_list.insert(row, item);
  endInsertRows();
}

void MediaSubTypeModel::handleItemChange()
{
  MediaSubTypeItem* item = static_cast<MediaSubTypeItem*>(sender());
  QModelIndex index = indexFromItem(item);
 // qDebug() << "Handling item change for:" << index;
  if(index.isValid())
  {
     QModelIndex lastrow;
     ident = item->mediatype();
     lastrow = index;
    emit dataChanged(index, index ,item->selectedStatus());
  }

}

MediaSubTypeItem * MediaSubTypeModel::find(const QString &id) const
{
  foreach(MediaSubTypeItem* item, m_list) {
    if(item->id() == id) return item;
  }
  return 0;
}

QModelIndex MediaSubTypeModel::indexFromItem(const MediaSubTypeItem *item) const
{
  Q_ASSERT(item);
  for(int row=0; row<m_list.size(); ++row) {

    if(m_list.at(row) == item) return index(row);

  }

  return QModelIndex();
}

void MediaSubTypeModel::clear()
{

  qDeleteAll(m_list);
  m_list.clear();
  this->reset();

}

bool MediaSubTypeModel::removeRow(int row, const QModelIndex &parent)
{
  Q_UNUSED(parent);
  if(row < 0 || row >= m_list.size()) return false;
  beginRemoveRows(QModelIndex(), row, row);
  delete m_list.takeAt(row);
  endRemoveRows();
  return true;
}

bool MediaSubTypeModel::removeRows(int row, int count, const QModelIndex &parent)
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

MediaSubTypeItem * MediaSubTypeModel::takeRow(int row)
{
  beginRemoveRows(QModelIndex(), row, row);
  MediaSubTypeItem* item = m_list.takeAt(row);
  endRemoveRows();
  return item;
}

MediaSubTypeItem * MediaSubTypeModel::currentRow()
{
    MediaSubTypeItem* item = m_list.at(0);
    return item;
}

void MediaSubTypeModel::sortModel(int column, Qt::SortOrder order)
{
}

void MediaSubTypeModel::setSelectionStatus(QString format)
{
    MediaSubTypeItem* item = find(format);
    item->updateSelection(false);
   // qDebug() << "Setting State for:" << format;
    //return state;
    ReturnSelectedItems();
}

bool MediaSubTypeModel::getSelectionStatus()
{
    //qDebug() << "Looking for status for" << format;
    //MediaTypeItem* item = find(format);

    bool g;//  = item->selectedStatus();
    return g;

}

void MediaSubTypeModel::ReturnSelectedItems()
{
    QStringList t_selected_items;

    foreach(MediaSubTypeItem* item, m_list)
    {
        if(item->selectedStatus() == true)
        {
            t_selected_items.append(item->mediatype());
            //qDebug() << "MediaSubType updated sorting filter" << item->mediatype() << "::" << item->name();
        }
    }
    QString qs_sorting_string= t_selected_items.join(",");

    emit SetTypeSort(1, qs_sorting_string);
}


