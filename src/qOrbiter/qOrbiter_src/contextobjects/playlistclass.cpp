#include "playlistclass.h"


PlaylistClass::PlaylistClass(PlaylistItemClass* prototype, QObject *parent) :
    QAbstractListModel(parent), m_prototype(prototype)
{
    setRoleNames(m_prototype->roleNames());
    qRegisterMetaType<QModelIndex>("QModelIndex");

}

int PlaylistClass::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_list.size();
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

void PlaylistClass::appendRow(PlaylistItemClass *item)
{
    appendRows(QList<PlaylistItemClass*>() << item);
}
//the purpose of this function is to first clear the existing playlist data out, and add the new data in due to the way the dce router send the updated playlist to us
void PlaylistClass::populate()
{

    emit modelAboutToBeReset();
    beginResetModel();
    resetInternalData();

    if(removeRows(0, m_list.count()-1))
    {
        //  qDebug() << "List Cleared";
        m_list.clear();

    }
    endResetModel();
    emit modelReset();
    emit playlistReady();

}

void PlaylistClass::appendRows(const QList<PlaylistItemClass *> &items)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount()+items.size()-1);
    foreach(PlaylistItemClass *item, items) {

        if (checkDupe(item->id(), item->index()) == false)
        {
            QObject::connect(item, SIGNAL(dataChanged()), this, SLOT(handleItemChange()));
            m_list.append(item);
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
    //qDebug() << "Inserting at:" << row;
    m_list.insert(row, item);
    endInsertRows();
}

void PlaylistClass::handleItemChange()
{
    PlaylistItemClass* item = static_cast<PlaylistItemClass*>(sender());
    QModelIndex index = indexFromItem(item);
    //  qDebug() << "Handling item change for:" << index;
    if(index.isValid())
    {
        emit dataChanged(index, index, index.row());
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
    resetInternalData();
    endResetModel();
    emit modelReset();


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

void PlaylistClass::setItemStatus(int pos)
{
    PlaylistItemClass * item = find(QString::number(pos));
    item->setActive(true);
}

void PlaylistClass::setCurrentIndex(int i)
{
    currentIndex = i;
}

int PlaylistClass::getCurrentIndex()
{
    return currentIndex;
    emit activeItemChanged();
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

void PlaylistClass::beginResetModel()
{
}

void PlaylistClass::endResetModel()
{
}

void PlaylistClass::resetInternalData()
{
    m_list.clear();
}
