#include "telecomscenariomodel.h"

TelecomScenarioModel::TelecomScenarioModel(TelecomScenarioItem* prototype, QObject* parent): QAbstractListModel(parent), m_prototype(prototype)
{
   #ifndef QT5
    setRoleNames(m_prototype->roleNames());
#endif
}


int TelecomScenarioModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return m_list.size();
}

QVariant TelecomScenarioModel::data(const QModelIndex &index, int role) const
{
  if(index.row() < 0 || index.row() >= m_list.size())
    return QVariant();
  return m_list.at(index.row())->data(role);
}

TelecomScenarioModel::~TelecomScenarioModel() {
  delete m_prototype;
  clear();
}

void TelecomScenarioModel::appendRow(TelecomScenarioItem *item)
{
  appendRows(QList<TelecomScenarioItem*>() << item);
}

#ifdef QT5
QHash<int, QByteArray> TelecomScenarioModel::roleNames() const
{
     return m_prototype->roleNames();
}
#endif

void TelecomScenarioModel::appendRows(const QList<TelecomScenarioItem *> &items)
{
  beginInsertRows(QModelIndex(), rowCount(), rowCount()+items.size()-1);
  foreach(TelecomScenarioItem *item, items) {
 //   connect(item, SIGNAL(dataChanged()), SLOT(handleItemChange()));
    m_list.append(item);
  }
  endInsertRows();
}

void TelecomScenarioModel::insertRow(int row, TelecomScenarioItem *item)
{
  beginInsertRows(QModelIndex(), row, row);
 // connect(item, SIGNAL(dataChanged()), SLOT(handleItemChange()));
  m_list.insert(row, item);
  endInsertRows();
}

void TelecomScenarioModel::handleItemChange()
{
  TelecomScenarioItem* item = static_cast<TelecomScenarioItem*>(sender());
  QModelIndex index = indexFromItem(item);
  if(index.isValid())
    emit dataChanged(index, index);
}

TelecomScenarioItem * TelecomScenarioModel::find(const QString &id) const
{
  foreach(TelecomScenarioItem* item, m_list) {
    if(item->id() == id) return item;
  }
  return 0;
}

QModelIndex TelecomScenarioModel::indexFromItem(const TelecomScenarioItem *item) const
{
  Q_ASSERT(item);
  for(int row=0; row<m_list.size(); ++row) {
    if(m_list.at(row) == item) return index(row);
  }
  return QModelIndex();
}

void TelecomScenarioModel::sortModel(int column, Qt::SortOrder order)
{
}

void TelecomScenarioModel::clear()
{
    if(!m_list.empty()){
        qDeleteAll(m_list);
        m_list.clear();
    }
}

bool TelecomScenarioModel::removeRow(int row, const QModelIndex &parent)
{
  Q_UNUSED(parent);
  if(row < 0 || row >= m_list.size()) return false;
  beginRemoveRows(QModelIndex(), row, row);
  delete m_list.takeAt(row);
  endRemoveRows();
  return true;
}

bool TelecomScenarioModel::removeRows(int row, int count, const QModelIndex &parent)
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

TelecomScenarioItem * TelecomScenarioModel::takeRow(int row)
{
  beginRemoveRows(QModelIndex(), row, row);
  TelecomScenarioItem* item = m_list.takeAt(row);
  endRemoveRows();
  return item;
}


