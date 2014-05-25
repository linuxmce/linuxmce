#include "mediascenariomodel.h"

MediaScenarioModel::MediaScenarioModel(MediaScenarioItem* prototype, QObject* parent): QAbstractListModel(parent), m_prototype(prototype)
{
   #ifndef QT5
    setRoleNames(m_prototype->roleNames());
#endif
     QList<MediaScenarioItem*> *m_list = new QList<MediaScenarioItem*>;
}


int MediaScenarioModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return m_list.size();
}

QVariant MediaScenarioModel::data(const QModelIndex &index, int role) const
{
  if(index.row() < 0 || index.row() >= m_list.size())
    return QVariant();
  return m_list.at(index.row())->data(role);
}

MediaScenarioModel::~MediaScenarioModel() {
  delete m_prototype;
    clear();
}
#ifdef QT5
QHash<int, QByteArray> MediaScenarioModel::roleNames() const
{
     return m_prototype->roleNames();
}
#endif

void MediaScenarioModel::appendRow(MediaScenarioItem *item)
{
  appendRows(QList<MediaScenarioItem*>() << item);
}

void MediaScenarioModel::appendRows(const QList<MediaScenarioItem *> &items)
{
  beginInsertRows(QModelIndex(), rowCount(), rowCount()+items.size()-1);
  foreach(MediaScenarioItem *item, items) {
  //  connect(item, SIGNAL(dataChanged()), SLOT(handleItemChange()));
    m_list.append(item);
  }
  endInsertRows();
}

void MediaScenarioModel::insertRow(int row, MediaScenarioItem *item)
{
  beginInsertRows(QModelIndex(), row, row);
//  connect(item, SIGNAL(dataChanged()), SLOT(handleItemChange()));
  m_list.insert(row, item);
  endInsertRows();
}

void MediaScenarioModel::handleItemChange()
{
  MediaScenarioItem* item = static_cast<MediaScenarioItem*>(sender());
  QModelIndex index = indexFromItem(item);
  if(index.isValid())
    emit DataChanged(index, index);
}

MediaScenarioItem * MediaScenarioModel::find(const QString &id) const
{
  foreach(MediaScenarioItem* item, m_list) {
    if(item->id() == id) return item;
  }
  return 0;
}

QModelIndex MediaScenarioModel::indexFromItem(const MediaScenarioItem *item) const
{
  Q_ASSERT(item);
  for(int row=0; row<m_list.size(); ++row) {
    if(m_list.at(row) == item) return index(row);
  }
  return QModelIndex();
}

void MediaScenarioModel::sortModel(int column, Qt::SortOrder order)
{
}

void MediaScenarioModel::clear()
{
   if(!m_list.empty()){
       m_list.clear();
   }
}

bool MediaScenarioModel::removeRow(int row, const QModelIndex &parent)
{
  Q_UNUSED(parent);
  if(row < 0 || row >= m_list.size()) return false;
  beginRemoveRows(QModelIndex(), row, row);
  delete m_list.takeAt(row);
  endRemoveRows();
  return true;
}

bool MediaScenarioModel::removeRows(int row, int count, const QModelIndex &parent)
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

MediaScenarioItem * MediaScenarioModel::takeRow(int row)
{
  beginRemoveRows(QModelIndex(), row, row);
  MediaScenarioItem* item = m_list.takeAt(row);
  endRemoveRows();
  return item;
}


