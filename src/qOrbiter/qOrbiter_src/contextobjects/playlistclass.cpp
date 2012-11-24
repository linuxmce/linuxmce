#include "playlistclass.h"
#include <QDebug>


PlaylistClass::PlaylistClass(PlaylistItemClass* prototype, QObject *parent) :
    QAbstractListModel(parent), m_prototype(prototype)
{
   #ifndef QT5
    setRoleNames(m_prototype->roleNames());
#endif
    qRegisterMetaType<QModelIndex>("QModelIndex");

}

int PlaylistClass::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid()){
        return 0;
    }
    else{
        return m_list.size();
    }
}

QVariant PlaylistClass::data(const QModelIndex &index, int role) const
{
    if(index.row() < 0 || index.row() >= m_list.size())
        return QVariant();
    return m_list.at(index.row())->data(role);
}

PlaylistClass::~PlaylistClass() {
    delete m_prototype;
    clear();
}
#ifdef QT5
QHash<int, QByteArray> PlaylistClass::roleNames() const
{
    return m_prototype->roleNames();
}
#endif
void PlaylistClass::appendRow(PlaylistItemClass *item)
{
    appendRows(QList<PlaylistItemClass*>() << item);
}
//the purpose of this function is to first clear the existing playlist data out, and add the new data in due to the way the dce router send the updated playlist to us
void PlaylistClass::populate()
{
    qDebug("Playlist Clearing");
    emit modelAboutToBeReset();
    beginResetModel();
    if(resetInternalData()){
    endResetModel();
    emit modelReset();
   emit playlistReady();
    }


}

void PlaylistClass::appendRows(const QList<PlaylistItemClass *> &items)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount()+items.size()-1);
    foreach(PlaylistItemClass *item, items) {

        if (checkDupe(item->id(), item->index()) == false)
        {
            m_list.append(item);
            QObject::connect(item, SIGNAL(dataChanged()), this, SLOT(handleItemChange()));

        }

    }

    endInsertRows();
    QModelIndex index = indexFromItem(m_list.last());
    QModelIndex index2 = indexFromItem(m_list.first());
    int currentRows= m_list.count();
    emit dataChanged(index2, index, currentRows);

}

void PlaylistClass::insertRow(int row, PlaylistItemClass *item)
{
    beginInsertRows(QModelIndex(), row, row);
    connect(item, SIGNAL(dataChanged()), this, SLOT(handleItemChange()));    
    m_list.insert(row, item);
    endInsertRows();
}

void PlaylistClass::handleItemChange()
{
    PlaylistItemClass* item = static_cast<PlaylistItemClass*>(sender());
    QModelIndex index = indexFromItem(item);   
    if(index.isValid())
    {
        emit dataChanged(index, index, 0);
    }
}

PlaylistItemClass * PlaylistClass::find(const QString &id) const
{
    foreach(PlaylistItemClass* item, m_list) {
        if(item->id().compare(id))
        {
            return item;
        }
    }

    return 0;
}

QModelIndex PlaylistClass::indexFromItem(const PlaylistItemClass *item) const
{
    Q_ASSERT(item);
    for(int row=0; row<m_list.size(); ++row) {

        if(m_list.at(row) == item) return index(row);

    }

    return QModelIndex();
}

void PlaylistClass::clear()
{
    emit modelAboutToBeReset();
    beginResetModel();
    if(resetInternalData()){
    endResetModel();
    emit modelReset();
    }
}

bool PlaylistClass::removeRow(int row, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    if(row < 0 || row >= m_list.size()) return false;
    beginRemoveRows(QModelIndex(), row, row);
    delete m_list.takeAt(row);
    endRemoveRows();
    return true;
}

bool PlaylistClass::removeRows(int row, int count, const QModelIndex &parent)
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

PlaylistItemClass * PlaylistClass::takeRow(int row)
{
    beginRemoveRows(QModelIndex(), row, row);
    PlaylistItemClass* item = m_list.takeAt(row);
    endRemoveRows();
    return item;
}

PlaylistItemClass * PlaylistClass::currentRow()
{
    PlaylistItemClass* item = m_list.at(0);
    return item;
}

void PlaylistClass::sortModel(int column, Qt::SortOrder order)
{
}

void PlaylistClass::setItemStatus(int pos)
{
    PlaylistItemClass * item = find(QString::number(pos));
    item->setActive(true);
}

void PlaylistClass::setCurrentIndex(int i)
{
    currentIndex = i;

  if(currentIndex > totalSize){
      qDebug() << "Oops, missing a track." << currentIndex << "::" << totalSize;
      populate();
  }
  else{
      emit activeItemChanged();
  }

}

int PlaylistClass::getCurrentIndex()
{
    return currentIndex;

}

bool PlaylistClass::checkDupe(QString name, int position)
{
    //qDebug() << "Checking dupe for:" << name << " at " << position ;

    if (PlaylistItemClass *item = find(name))
    { //  qDebug() << "Found " << name;

        if (item->index() == position )
        {
            //  qDebug() << "Dupe position " << position ;
            return true;
        }
        else
        {
            // qDebug() << item->index();
            // qDebug() << "Did not find item at postion " << position;
            return  false;
        }

    }
    else
    {
        // qDebug() << name << " at " << position-1 << " Not in playlist";
        return false;
    }

    return true;
}


bool PlaylistClass::resetInternalData()
{  
    int total = m_list.count();
    for(int i = 0; i < m_list.count(); i++){
        m_list.removeAt(i);
    }
    m_list.clear();
    return true;
}
