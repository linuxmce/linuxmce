#include "climatescenariomodel.h"

ClimateScenarioModel::ClimateScenarioModel(ClimateScenarioItem* prototype, QObject* parent): QAbstractListModel(parent), m_prototype(prototype)
{
   #ifndef QT5
    setRoleNames(m_prototype->roleNames());
#endif
}

#ifdef QT5
QHash<int, QByteArray> ClimateScenarioModel::roleNames() const
{
     return m_prototype->roleNames();
}
#endif


int ClimateScenarioModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return m_list.size();
}

QVariant ClimateScenarioModel::data(const QModelIndex &index, int role) const
{
  if(index.row() < 0 || index.row() >= m_list.size())
    return QVariant();
  return m_list.at(index.row())->data(role);
}

ClimateScenarioModel::~ClimateScenarioModel() {
  delete m_prototype;
  clear();
}

void ClimateScenarioModel::appendRow(ClimateScenarioItem *item)
{
  appendRows(QList<ClimateScenarioItem*>() << item);
}

void ClimateScenarioModel::appendRows(const QList<ClimateScenarioItem *> &items)
{
  beginInsertRows(QModelIndex(), rowCount(), rowCount()+items.size()-1);
  foreach(ClimateScenarioItem *item, items) {
    //connect(item, SIGNAL(dataChanged()), SLOT(handleItemChange()));
    m_list.append(item);
  }
  endInsertRows();
}

void ClimateScenarioModel::insertRow(int row, ClimateScenarioItem *item)
{
  beginInsertRows(QModelIndex(), row, row);
//  connect(item, SIGNAL(dataChanged()), SLOT(handleItemChange()));
  m_list.insert(row, item);
  endInsertRows();
}

void ClimateScenarioModel::handleItemChange()
{
  ClimateScenarioItem* item = static_cast<ClimateScenarioItem*>(sender());
  QModelIndex index = indexFromItem(item);
  if(index.isValid())
    emit dataChanged(index, index);
}

ClimateScenarioItem * ClimateScenarioModel::find(const QString &id) const
{
  foreach(ClimateScenarioItem* item, m_list) {
    if(item->id() == id) return item;
  }
  return 0;
}

QModelIndex ClimateScenarioModel::indexFromItem(const ClimateScenarioItem *item) const
{
  Q_ASSERT(item);
  for(int row=0; row<m_list.size(); ++row) {
    if(m_list.at(row) == item) return index(row);
  }
  return QModelIndex();
}

void ClimateScenarioModel::sortModel(int column, Qt::SortOrder order)
{
}

void ClimateScenarioModel::clear()
{
    if(!m_list.empty()){
        qDeleteAll(m_list);
        m_list.clear();
    }
}

bool ClimateScenarioModel::removeRow(int row, const QModelIndex &parent)
{
  Q_UNUSED(parent);
  if(row < 0 || row >= m_list.size()) return false;
  beginRemoveRows(QModelIndex(), row, row);
  delete m_list.takeAt(row);
  endRemoveRows();
  return true;
}

bool ClimateScenarioModel::removeRows(int row, int count, const QModelIndex &parent)
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

ClimateScenarioItem * ClimateScenarioModel::takeRow(int row)
{
  beginRemoveRows(QModelIndex(), row, row);
  ClimateScenarioItem* item = m_list.takeAt(row);
  endRemoveRows();
  return item;
}



