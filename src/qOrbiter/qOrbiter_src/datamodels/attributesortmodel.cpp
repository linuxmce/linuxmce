#include "attributesortmodel.h"
#ifdef debug
#include <QDebug>
#endif

AttributeSortModel::AttributeSortModel(AttributeSortItem* prototype, QObject *parent) :
    QAbstractListModel(parent), m_prototype(prototype)
{
  setRoleNames(m_prototype->roleNames());
   qRegisterMetaType<QModelIndex>("QModelIndex");
}

int AttributeSortModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return m_list.size();
}

QVariant AttributeSortModel::data(const QModelIndex &index, int role) const
{
  if(index.row() < 0 || index.row() >= m_list.size())
    return QVariant();
  return m_list.at(index.row())->data(role);
}

AttributeSortModel::~AttributeSortModel() {
  delete m_prototype;
  clear();
}

void AttributeSortModel::appendRow(AttributeSortItem *item)
{
  appendRows(QList<AttributeSortItem*>() << item);
}

void AttributeSortModel::appendRows(const QList<AttributeSortItem *> &items)
{
  beginInsertRows(QModelIndex(), rowCount(), rowCount()+items.size()-1);
  foreach(AttributeSortItem *item, items) {

      QObject::connect(item, SIGNAL(filterChanged()), this, SLOT(handleItemChange()));
    m_list.append(item);
  }

  endInsertRows();
  //QModelIndex index = indexFromItem(m_list.last());
  //QModelIndex index2 = indexFromItem(m_list.first());
  //int currentRows= m_list.count() - 1;
  //emit dataChanged(index2, index, currentRows);

}

void AttributeSortModel::insertRow(int row, AttributeSortItem *item)
{
  beginInsertRows(QModelIndex(), row, row);
  connect(item, SIGNAL(dataChanged()), this, SLOT(handleItemChange()));
  //qDebug() << "Inserting at:" << row;
  m_list.insert(row, item);
  endInsertRows();
}

void AttributeSortModel::handleItemChange()
{
  AttributeSortItem* item = static_cast<AttributeSortItem*>(sender());
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

AttributeSortItem * AttributeSortModel::find(const QString &id) const
{
  foreach(AttributeSortItem* item, m_list) {
    if(item->id() == id) return item;
  }
  return 0;
}

QModelIndex AttributeSortModel::indexFromItem(const AttributeSortItem *item) const
{
  Q_ASSERT(item);
  for(int row=0; row<m_list.size(); ++row) {

    if(m_list.at(row) == item) return index(row);

  }

  return QModelIndex();
}

void AttributeSortModel::clear()
{

  qDeleteAll(m_list);
  m_list.clear();
  this->reset();

}

bool AttributeSortModel::removeRow(int row, const QModelIndex &parent)
{
  Q_UNUSED(parent);
  if(row < 0 || row >= m_list.size()) return false;
  beginRemoveRows(QModelIndex(), row, row);
  delete m_list.takeAt(row);
  endRemoveRows();
  return true;
}

bool AttributeSortModel::removeRows(int row, int count, const QModelIndex &parent)
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

AttributeSortItem * AttributeSortModel::takeRow(int row)
{
  beginRemoveRows(QModelIndex(), row, row);
  AttributeSortItem* item = m_list.takeAt(row);
  endRemoveRows();
  return item;
}

AttributeSortItem * AttributeSortModel::currentRow()
{
    AttributeSortItem* item = m_list.at(0);
    return item;
}

void AttributeSortModel::sortModel(int column, Qt::SortOrder order)
{
}

bool AttributeSortModel::setSelectionStatus(QString format)
{
    AttributeSortItem* item = find(format);
    bool newStatus = item->updateSelection(false);
    //qDebug() << "Setting State for:" << format;
    //return state;

    ReturnSelectedItems();
    return newStatus;
}

bool AttributeSortModel::getSelectionStatus(QString format)
{
    //qDebug() << "Looking for status for" << format;
    AttributeSortItem* item = find(format);

    bool g = item->selectedStatus();
    return g;

}

void AttributeSortModel::ReturnSelectedItems()
{
    QStringList t_selected_items;
    foreach(AttributeSortItem* item, m_list) {
        if(item->selectedStatus() == true) t_selected_items.append(item->fileformat());
    }
    QString qs_sorting_string= t_selected_items.join(",");
    //qDebug() << "Attribute Sort updated sorting filter" << qs_sorting_string;
    emit SetTypeSort(6, qs_sorting_string);
}

void AttributeSortModel::resetStates()
{
    foreach(AttributeSortItem* item, m_list) {
        if(item->setStatus(false));
    }
}


