#include <datamodels/lightingscenariomodel.h>



LightingScenarioModel::LightingScenarioModel(LightingScenarioItem* prototype, QObject* parent): QAbstractListModel(parent), m_prototype(prototype)
{
   #ifndef QT5
    setRoleNames(m_prototype->roleNames());
#endif
}


int LightingScenarioModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return m_list.size();
}

QVariant LightingScenarioModel::data(const QModelIndex &index, int role) const
{
  if(index.row() < 0 || index.row() >= m_list.size())
    return QVariant();
  return m_list.at(index.row())->data(role);
}

LightingScenarioModel::~LightingScenarioModel() {
  delete m_prototype;
    clear();
}
#ifdef QT5
QHash<int, QByteArray> LightingScenarioModel::roleNames() const
{
      return m_prototype->roleNames();
}
#endif
void LightingScenarioModel::appendRow(LightingScenarioItem *item)
{
  appendRows(QList<LightingScenarioItem*>() << item);
}

void LightingScenarioModel::appendRows(const QList<LightingScenarioItem *> &items)
{
  beginInsertRows(QModelIndex(), rowCount(), rowCount()+items.size()-1);
  foreach(LightingScenarioItem *item, items) {
    //connect(item, SIGNAL(dataChanged()), SLOT(handleItemChange()));
    m_list.append(item);
  }
  endInsertRows();
}

void LightingScenarioModel::insertRow(int row, LightingScenarioItem *item)
{
  beginInsertRows(QModelIndex(), row, row);
 // connect(item, SIGNAL(dataChanged()), SLOT(handleItemChange()));
  m_list.insert(row, item);
  endInsertRows();
}

void LightingScenarioModel::handleItemChange()
{
  LightingScenarioItem* item = static_cast<LightingScenarioItem*>(sender());
  QModelIndex index = indexFromItem(item);
  if(index.isValid())
    emit dataChanged(index, index);
}

LightingScenarioItem * LightingScenarioModel::find(const QString &id) const
{
  foreach(LightingScenarioItem* item, m_list) {
    if(item->id() == id) return item;
  }
  return 0;
}

QModelIndex LightingScenarioModel::indexFromItem(const LightingScenarioItem *item) const
{
  Q_ASSERT(item);
  for(int row=0; row<m_list.size(); ++row) {
    if(m_list.at(row) == item) return index(row);
  }
  return QModelIndex();
}

void LightingScenarioModel::sortModel(int column, Qt::SortOrder order)
{
}

void LightingScenarioModel::clear()
{
    if(!m_list.empty()){
        qDeleteAll(m_list);
        m_list.clear();
    }
}

bool LightingScenarioModel::removeRow(int row, const QModelIndex &parent)
{
  Q_UNUSED(parent);
  if(row < 0 || row >= m_list.size()) return false;
  beginRemoveRows(QModelIndex(), row, row);
  delete m_list.takeAt(row);
  endRemoveRows();
  return true;
}

bool LightingScenarioModel::removeRows(int row, int count, const QModelIndex &parent)
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

LightingScenarioItem * LightingScenarioModel::takeRow(int row)
{
  beginRemoveRows(QModelIndex(), row, row);
  LightingScenarioItem* item = m_list.takeAt(row);
  endRemoveRows();
  return item;
}


