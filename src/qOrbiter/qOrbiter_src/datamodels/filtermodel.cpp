#include "filtermodel.h"


FilterModel::FilterModel(FilterModelItem* prototype, QObject *parent) :
    QAbstractListModel(parent), m_prototype(prototype)
{
 #ifndef QT5
    setRoleNames(m_prototype->roleNames());
#endif
   qRegisterMetaType<QModelIndex>("QModelIndex");
}

int FilterModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return m_list.size();
}

QVariant FilterModel::data(const QModelIndex &index, int role) const
{
  if(index.row() < 0 || index.row() >= m_list.size())
    return QVariant();
  return m_list.at(index.row())->data(role);
}

FilterModel::~FilterModel() {
  delete m_prototype;
  clear();
}

void FilterModel::appendRow(FilterModelItem *item)
{
  appendRows(QList<FilterModelItem*>() << item);
}

void FilterModel::appendRows(const QList<FilterModelItem *> &items)
{
  beginInsertRows(QModelIndex(), rowCount(), rowCount()+items.size()-1);
  foreach(FilterModelItem *item, items) {

      QObject::connect(item, SIGNAL(filterChanged()), this, SLOT(handleItemChange()));
    m_list.append(item);
  }

  endInsertRows();
  //QModelIndex index = indexFromItem(m_list.last());
  //QModelIndex index2 = indexFromItem(m_list.first());
  //int currentRows= m_list.count() - 1;
  //emit dataChanged(index2, index, currentRows);

}

void FilterModel::insertRow(int row, FilterModelItem *item)
{
  beginInsertRows(QModelIndex(), row, row);
  connect(item, SIGNAL(dataChanged()), this, SLOT(handleItemChange()));
  //qDebug() << "Inserting at:" << row;
  m_list.insert(row, item);
  endInsertRows();
}

void FilterModel::handleItemChange()
{
  FilterModelItem* item = static_cast<FilterModelItem*>(sender());
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

FilterModelItem * FilterModel::find(const QString &id) const
{
  foreach(FilterModelItem* item, m_list) {
    if(item->id() == id) return item;
  }
  return 0;
}

QModelIndex FilterModel::indexFromItem(const FilterModelItem *item) const
{
  Q_ASSERT(item);
  for(int row=0; row<m_list.size(); ++row) {

    if(m_list.at(row) == item) return index(row);

  }

  return QModelIndex();
}

void FilterModel::clear()
{

  qDeleteAll(m_list);
  m_list.clear();

  #ifndef QT5
  this->reset();
#endif


}

bool FilterModel::removeRow(int row, const QModelIndex &parent)
{
  Q_UNUSED(parent);
  if(row < 0 || row >= m_list.size()) return false;
  beginRemoveRows(QModelIndex(), row, row);
  delete m_list.takeAt(row);
  endRemoveRows();
  return true;
}

bool FilterModel::removeRows(int row, int count, const QModelIndex &parent)
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

FilterModelItem * FilterModel::takeRow(int row)
{
  beginRemoveRows(QModelIndex(), row, row);
  FilterModelItem* item = m_list.takeAt(row);
  endRemoveRows();
  return item;
}

FilterModelItem * FilterModel::currentRow()
{
    FilterModelItem* item = m_list.at(0);
    return item;
}

void FilterModel::sortModel(int column, Qt::SortOrder order)
{
}

bool FilterModel::setSelectionStatus(QString format)
{
    FilterModelItem* item = find(format);
    bool newStatus = item->updateSelection(false);
    //qDebug() << "Setting State for:" << format;
    //return state;
    ReturnSelectedItems();
    return newStatus;
}

bool FilterModel::getSelectionStatus(QString format)
{
    //qDebug() << "Looking for status for" << format;
    FilterModelItem* item = find(format);
    bool g=item->selectedStatus();
    return g;
}

void FilterModel::ReturnSelectedItems()
{
    QStringList t_selected_items;

    foreach(FilterModelItem* item, m_list) {
        if(item->selectedStatus() == true) t_selected_items.append(item->fileformat());
    }
    QString qs_sorting_string= t_selected_items.join(",");
    //qDebug() << "File Format updated sorting filter" << qs_sorting_string;
    emit SetTypeSort(2, qs_sorting_string);
}

#ifdef QT5
QHash<int, QByteArray> FilterModel::roleNames() const
{
     return m_prototype->roleNames();
}
#endif

