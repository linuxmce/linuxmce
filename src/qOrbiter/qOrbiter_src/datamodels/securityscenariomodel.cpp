#include "securityscenariomodel.h"
SecurityScenarioModel::SecurityScenarioModel(SecurityScenarioItem* prototype, QObject* parent): QAbstractListModel(parent), m_prototype(prototype)
{
   #ifndef QT5
    setRoleNames(m_prototype->roleNames());
#endif
}

#ifdef QT5
QHash<int, QByteArray> SecurityScenarioModel::roleNames() const
{
     return m_prototype->roleNames();
}
#endif

int SecurityScenarioModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return m_list.size();
}

QVariant SecurityScenarioModel::data(const QModelIndex &index, int role) const
{
  if(index.row() < 0 || index.row() >= m_list.size())
    return QVariant();
  return m_list.at(index.row())->data(role);
}

SecurityScenarioModel::~SecurityScenarioModel() {
  delete m_prototype;
  clear();
}

void SecurityScenarioModel::appendRow(SecurityScenarioItem *item)
{
  appendRows(QList<SecurityScenarioItem*>() << item);
}

void SecurityScenarioModel::appendRows(const QList<SecurityScenarioItem *> &items)
{
  beginInsertRows(QModelIndex(), rowCount(), rowCount()+items.size()-1);
  foreach(SecurityScenarioItem *item, items) {
    //connect(item, SIGNAL(dataChanged()), SLOT(handleItemChange()));
    m_list.append(item);
  }
  endInsertRows();
}

void SecurityScenarioModel::insertRow(int row, SecurityScenarioItem *item)
{
  beginInsertRows(QModelIndex(), row, row);
 // connect(item, SIGNAL(dataChanged()), SLOT(handleItemChange()));
  m_list.insert(row, item);
  endInsertRows();
}

void SecurityScenarioModel::handleItemChange()
{
  SecurityScenarioItem* item = static_cast<SecurityScenarioItem*>(sender());
  QModelIndex index = indexFromItem(item);
  if(index.isValid())
    emit dataChanged(index, index);
}

SecurityScenarioItem * SecurityScenarioModel::find(const QString &id) const
{
  foreach(SecurityScenarioItem* item, m_list) {
    if(item->id() == id) return item;
  }
  return 0;
}

QModelIndex SecurityScenarioModel::indexFromItem(const SecurityScenarioItem *item) const
{
  Q_ASSERT(item);
  for(int row=0; row<m_list.size(); ++row) {
    if(m_list.at(row) == item) return index(row);
  }
  return QModelIndex();
}

void SecurityScenarioModel::sortModel(int column, Qt::SortOrder order)
{
}

void SecurityScenarioModel::clear()
{
    if(!m_list.empty()){
        qDeleteAll(m_list);
        m_list.clear();
    }
}

bool SecurityScenarioModel::removeRow(int row, const QModelIndex &parent)
{
  Q_UNUSED(parent);
  if(row < 0 || row >= m_list.size()) return false;
  beginRemoveRows(QModelIndex(), row, row);
  delete m_list.takeAt(row);
  endRemoveRows();
  return true;
}

bool SecurityScenarioModel::removeRows(int row, int count, const QModelIndex &parent)
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

SecurityScenarioItem * SecurityScenarioModel::takeRow(int row)
{
  beginRemoveRows(QModelIndex(), row, row);
  SecurityScenarioItem* item = m_list.takeAt(row);
  endRemoveRows();
  return item;
}



