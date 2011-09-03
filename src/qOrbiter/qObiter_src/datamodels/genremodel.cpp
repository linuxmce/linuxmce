#include "genremodel.h"

#include <QDebug>

GenreModel::GenreModel(GenreItem* prototype, QObject *parent) :
    QAbstractListModel(parent), m_prototype(prototype)
{
  setRoleNames(m_prototype->roleNames());
   qRegisterMetaType<QModelIndex>("QModelIndex");
}

int GenreModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return m_list.size();
}

QVariant GenreModel::data(const QModelIndex &index, int role) const
{
  if(index.row() < 0 || index.row() >= m_list.size())
    return QVariant();
  return m_list.at(index.row())->data(role);
}

GenreModel::~GenreModel() {
  delete m_prototype;
  clear();
}

void GenreModel::appendRow(GenreItem *item)
{
  appendRows(QList<GenreItem*>() << item);
}

void GenreModel::appendRows(const QList<GenreItem *> &items)
{
  beginInsertRows(QModelIndex(), rowCount(), rowCount()+items.size()-1);
  foreach(GenreItem *item, items) {

   QObject::connect(item, SIGNAL(dataChanged()), this, SLOT(handleItemChange()));
    m_list.append(item);
  }

  endInsertRows();
  QModelIndex index = indexFromItem(m_list.last());
  QModelIndex index2 = indexFromItem(m_list.first());
  int currentRows= m_list.count() - 1;
  emit dataChanged(index2, index, currentRows);

}

void GenreModel::insertRow(int row, GenreItem *item)
{
  beginInsertRows(QModelIndex(), row, row);
  connect(item, SIGNAL(dataChanged()), this, SLOT(handleItemChange()));
  qDebug() << "Inserting at:" << row;
  m_list.insert(row, item);
  endInsertRows();
}

void GenreModel::handleItemChange()
{
  GenreItem* item = static_cast<GenreItem*>(sender());
  QModelIndex index = indexFromItem(item);
  qDebug() << "Handling item change for:" << index;
  if(index.isValid())
  {
    //emit dataChanged(index, index);
  }
}

GenreItem * GenreModel::find(const QString &id) const
{
  foreach(GenreItem* item, m_list) {
    if(item->id() == id) return item;
  }
  return 0;
}

QModelIndex GenreModel::indexFromItem(const GenreItem *item) const
{
  Q_ASSERT(item);
  for(int row=0; row<m_list.size(); ++row) {

    if(m_list.at(row) == item) return index(row);

  }

  return QModelIndex();
}

void GenreModel::clear()
{

  qDeleteAll(m_list);
  m_list.clear();
  this->reset();

}

bool GenreModel::removeRow(int row, const QModelIndex &parent)
{
  Q_UNUSED(parent);
  if(row < 0 || row >= m_list.size()) return false;
  beginRemoveRows(QModelIndex(), row, row);
  delete m_list.takeAt(row);
  endRemoveRows();
  return true;
}

bool GenreModel::removeRows(int row, int count, const QModelIndex &parent)
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

GenreItem * GenreModel::takeRow(int row)
{
  beginRemoveRows(QModelIndex(), row, row);
  GenreItem* item = m_list.takeAt(row);
  endRemoveRows();
  return item;
}

GenreItem * GenreModel::currentRow()
{
    GenreItem* item = m_list.at(0);
    return item;
}

bool GenreModel::setSelectionStatus(QString format)
{
    GenreItem* item = find(format);
    bool newStatus = item->updateSelection(false);
    qDebug() << "Setting State for:" << format;
    //return state;
    ReturnSelectedItems();
    return newStatus;
}

bool GenreModel::getSelectionStatus(QString format)
{
    qDebug() << "Looking for status for" << format;
    GenreItem* item = find(format);

    bool g = item->selectedStatus();
    return g;

}
void GenreModel::ReturnSelectedItems()
{
    QStringList t_selected_items;

    foreach(GenreItem* item, m_list) {
        if(item->selectedStatus() == true) t_selected_items.append(item->fileformat());
    }
    QString qs_sorting_string= t_selected_items.join(",");
    qDebug() << "File Format updated sorting filter" << qs_sorting_string;
    emit SetTypeSort(3, qs_sorting_string);
}

