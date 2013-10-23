#include "usermodel.h"


UserModel::UserModel(UserItem* prototype, QObject *parent) :
    QAbstractListModel(parent), m_prototype(prototype)
{
 #ifndef QT5
    setRoleNames(m_prototype->roleNames());
#endif
setCurrentPrivateUser(-1);
}

#ifdef QT5
QHash<int, QByteArray> UserModel::roleNames() const
{
     return m_prototype->roleNames();
}
#endif

int UserModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return m_list.size();
}

QVariant UserModel::data(const QModelIndex &index, int role) const
{
  if(index.row() < 0 || index.row() >= m_list.size())
    return QVariant();
  return m_list.at(index.row())->data(role);
}

UserModel::~UserModel() {
  delete m_prototype;
  clear();
}

void UserModel::appendRow(UserItem *item)
{
  appendRows(QList<UserItem*>() << item);
}

void UserModel::appendRows(const QList<UserItem *> &items)
{
  beginInsertRows(QModelIndex(), rowCount(), rowCount()+items.size()-1);
  foreach(UserItem *item, items) {
    //connect(item, SIGNAL(dataChanged()), SLOT(handleItemChange()));
    m_list.append(item);
  }
  endInsertRows();
}

void UserModel::insertRow(int row, UserItem *item)
{
  beginInsertRows(QModelIndex(), row, row);
  //connect(item, SIGNAL(dataChanged()), SLOT(handleItemChange()));
  m_list.insert(row, item);
  endInsertRows();
}

void UserModel::handleItemChange()
{
  UserItem* item = static_cast<UserItem*>(sender());
  QModelIndex index = indexFromItem(item);
  if(index.isValid())
    emit dataChanged(index, index);
}

UserItem * UserModel::find(const QString &id) const
{
  foreach(UserItem* item, m_list) {

    if(item->pk_user_get() == id.toInt())
    {
        return item;
    }
  }
  return new UserItem();
}

QModelIndex UserModel::indexFromItem(const UserItem *item) const
{
  Q_ASSERT(item);
  for(int row=0; row<m_list.size(); ++row) {
    if(m_list.at(row) == item) return index(row);
  }
  return QModelIndex();
}

void UserModel::sortModel(int column, Qt::SortOrder order)
{
}

void UserModel::clear()
{
  qDeleteAll(m_list);
  m_list.clear();
}

bool UserModel::removeRow(int row, const QModelIndex &parent)
{
  Q_UNUSED(parent);
  if(row < 0 || row >= m_list.size()) return false;
  beginRemoveRows(QModelIndex(), row, row);
  delete m_list.takeAt(row);
  endRemoveRows();
  return true;
}

bool UserModel::removeRows(int row, int count, const QModelIndex &parent)
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

UserItem * UserModel::takeRow(int row)
{
  beginRemoveRows(QModelIndex(), row, row);
  UserItem* item = m_list.takeAt(row);
  endRemoveRows();
  return item;
}

void UserModel::findDefault( )
{
    defaultUser = m_list.at(0)->default_user();

    for (int cnt = 0; cnt < m_list.length(); cnt++)
    {

        if (m_list.at(cnt)->pk_user_get() == defaultUser)
        {
         defaultUzer = m_list.at(cnt)->id();
        }
    }
}
