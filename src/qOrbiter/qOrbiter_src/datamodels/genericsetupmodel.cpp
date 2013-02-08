#include "genericsetupmodel.h"


GenericSetupModel::GenericSetupModel(GenericSetupItem* prototype, qorbiterManager *ref) :
    m_prototype(prototype), manager_ref(ref)
{
 #ifndef QT5
    setRoleNames(m_prototype->roleNames());
#endif
   qRegisterMetaType<QModelIndex>("QModelIndex");
   totalcells = 0;
}

int GenericSetupModel::rowCount(const QModelIndex &parent) const
{
  //Q_UNUSED(parent);
  return m_list.size();
}

QVariant GenericSetupModel::data(const QModelIndex &index, int role) const
{
  if(index.row() < 0 || index.row() >= m_list.size())
    return QVariant();
  return m_list.at(index.row())->data(role);
}

GenericSetupModel::~GenericSetupModel() {
  delete m_prototype;
  clear();
}

void GenericSetupModel::appendRow(GenericSetupItem *item)
{
  appendRows(QList<GenericSetupItem*>() << item);
}

void GenericSetupModel::appendRows(const QList<GenericSetupItem *> &items)
{
  beginInsertRows(QModelIndex(), rowCount(), rowCount()+items.size()-1);
  foreach(GenericSetupItem *item, items) {

   connect(item, SIGNAL(dataChanged()), this , SLOT(handleItemChange()));
    m_list.append(item);
  }

  endInsertRows();
  QModelIndex index = indexFromItem(m_list.last());
  QModelIndex index2 = indexFromItem(m_list.first());
  int currentRows= m_list.count() - 1;
  emit dataChanged(index2, index, currentRows);

}

void GenericSetupModel::insertRow(int row, GenericSetupItem *item)
{
  beginInsertRows(QModelIndex(), row, row);
  connect(item, SIGNAL(dataChanged()), this, SLOT(handleItemChange()));
  //qDebug() << "Inserting at:" << row;
  m_list.insert(row, item);
  endInsertRows();
}

void GenericSetupModel::handleItemChange()
{
  GenericSetupItem* item = static_cast<GenericSetupItem*>(sender());
  QModelIndex index = indexFromItem(item);
  //qDebug() << "Handling item change for:" << index;
  if(index.isValid())
  {
    emit dataChanged(index, index, 0);
  }
}

GenericSetupItem * GenericSetupModel::find(const QString &id) const
{
  foreach(GenericSetupItem* item, m_list) {
    if(item->id() == id) return item;
  }
  return 0;
}

QVariant GenericSetupModel::get(int index, const QString &name) const
{
  if (index>=0 && index<m_list.size()) {
    GenericSetupItem* myItem = m_list.at(index);

    QHash<int, QByteArray> myHash = myItem->roleNames();
    QHash<int, QByteArray>::const_iterator i = myHash.constBegin();

    while (i != myHash.constEnd()) {
         if (i.value() == name) {
         return myItem->data(i.key());
         }
         ++i;
     }
  } else {
      return "";
  }
}

QModelIndex GenericSetupModel::indexFromItem(const GenericSetupItem *item) const
{
  Q_ASSERT(item);
  for(int row=0; row<m_list.size(); ++row) {

    if(m_list.at(row) == item) return index(row);

  }

  return QModelIndex();
}

void GenericSetupModel::clear()
{

  qDeleteAll(m_list);
  m_list.clear();
  #ifndef QT5
  this->reset();
#endif

}

void GenericSetupModel::sortModel(int column, Qt::SortOrder order)
{
}

bool GenericSetupModel::removeRow(int row, const QModelIndex &parent)
{
  Q_UNUSED(parent);
  if(row < 0 || row >= m_list.size()) return false;
  beginRemoveRows(QModelIndex(), row, row);
  delete m_list.takeAt(row);
  endRemoveRows();
  return true;
}

bool GenericSetupModel::removeRows(int row, int count, const QModelIndex &parent)
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

GenericSetupItem * GenericSetupModel::takeRow(int row)
{
  beginRemoveRows(QModelIndex(), row, row);
  GenericSetupItem* item = m_list.takeAt(row);
  endRemoveRows();
  return item;
}

GenericSetupItem * GenericSetupModel::currentRow()
{
    GenericSetupItem* item = m_list.at(0);
    return item;
}

void GenericSetupModel::checkForMore()
{
    if (totalcells > m_list.count())
    {
        //manager_ref->pqOrbiter->populateAdditionalMedia();
    }
}

#ifdef QT5
QHash<int, QByteArray> GenericSetupModel::roleNames() const
{
     return m_prototype->roleNames();
}
#endif
