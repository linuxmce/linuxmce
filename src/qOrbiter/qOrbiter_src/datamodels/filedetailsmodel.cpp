#include "filedetailsmodel.h"

FileDetailsModel::FileDetailsModel(FileDetailsItem* prototype, QObject *parent) :
    QAbstractListModel(parent), m_prototype(prototype)
{
 #ifndef QT5
    setRoleNames(m_prototype->roleNames());
#endif
   qRegisterMetaType<QModelIndex>("QModelIndex");
}

int FileDetailsModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return m_list.size();
}

QVariant FileDetailsModel::data(const QModelIndex &index, int role) const
{
  if(index.row() < 0 || index.row() >= m_list.size())
    return QVariant();
  return m_list.at(index.row())->data(role);
}

FileDetailsModel::~FileDetailsModel() {
  delete m_prototype;
  clear();
}

void FileDetailsModel::appendRow(FileDetailsItem *item)
{
  appendRows(QList<FileDetailsItem*>() << item);
}

void FileDetailsModel::appendRows(const QList<FileDetailsItem *> &items)
{
  beginInsertRows(QModelIndex(), rowCount(), rowCount()+items.size()-1);
  foreach(FileDetailsItem *item, items) {

      QObject::connect(item, SIGNAL(filterChanged()), this, SLOT(handleItemChange()));
    m_list.append(item);
  }

  endInsertRows();
  //QModelIndex index = indexFromItem(m_list.last());
  //QModelIndex index2 = indexFromItem(m_list.first());
  //int currentRows= m_list.count() - 1;
  //emit dataChanged(index2, index, currentRows);

}

void FileDetailsModel::insertRow(int row, FileDetailsItem *item)
{
  beginInsertRows(QModelIndex(), row, row);
  connect(item, SIGNAL(dataChanged()), this, SLOT(handleItemChange()));
  //qDebug() << "Inserting at:" << row;
  m_list.insert(row, item);
  endInsertRows();
}

void FileDetailsModel::handleItemChange()
{
  FileDetailsItem* item = static_cast<FileDetailsItem*>(sender());
  QModelIndex index = indexFromItem(item);
 // qDebug() << "Handling item change for:" << index;
  if(index.isValid())
  {
     QModelIndex lastrow;
     ident = item->id();
     lastrow = index;
    emit dataChanged(index, index ,item->selectedStatus());
  }
}

FileDetailsItem * FileDetailsModel::find(const QString &id) const
{
  foreach(FileDetailsItem* item, m_list) {
    if(item->id() == id) return item;
  }
  return 0;
}

QModelIndex FileDetailsModel::indexFromItem(const FileDetailsItem *item) const
{
  Q_ASSERT(item);
  for(int row=0; row<m_list.size(); ++row) {

    if(m_list.at(row) == item) return index(row);

  }

  return QModelIndex();
}

void FileDetailsModel::clear()
{

  qDeleteAll(m_list);
  m_list.clear();
  #ifndef QT5
  this->reset();
#endif

}

bool FileDetailsModel::removeRow(int row, const QModelIndex &parent)
{
  Q_UNUSED(parent);
  if(row < 0 || row >= m_list.size()) return false;
  beginRemoveRows(QModelIndex(), row, row);
  delete m_list.takeAt(row);
  endRemoveRows();
  return true;
}

bool FileDetailsModel::removeRows(int row, int count, const QModelIndex &parent)
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

FileDetailsItem * FileDetailsModel::takeRow(int row)
{
  beginRemoveRows(QModelIndex(), row, row);
  FileDetailsItem* item = m_list.takeAt(row);
  endRemoveRows();
  return item;
}

FileDetailsItem * FileDetailsModel::currentRow()
{
    FileDetailsItem* item = m_list.at(0);
    return item;
}

void FileDetailsModel::setSelectionStatus(QString format)
{
    FileDetailsItem* item = find(format);
    item->updateSelection(false);
   // qDebug() << "Setting State for:" << format;
    //return state;
      ReturnSelectedItems();
}

bool FileDetailsModel::getSelectionStatus()
{
    //qDebug() << "Looking for status for" << format;
    //FileDetailsItem* item = find(format);
    bool g;//  = item->selectedStatus();
    return g;
}

void FileDetailsModel::ReturnSelectedItems()
{
    QStringList t_selected_items;

    foreach(FileDetailsItem* item, m_list) {
        if(item->selectedStatus() == true) t_selected_items.append(item->id());
    }
    QString qs_sorting_string= t_selected_items.join(",");
    //qDebug() << "File Format updated sorting filter" << qs_sorting_string;
    emit SetTypeSort(2, qs_sorting_string);
}

void FileDetailsModel::sortModel(int column, Qt::SortOrder order)
{
}




