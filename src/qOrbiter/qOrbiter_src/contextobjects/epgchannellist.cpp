#include "epgchannellist.h"



EPGChannelList::EPGChannelList(EPGItemClass* prototype) :
    m_prototype(prototype)
{
   #ifndef QT5
    setRoleNames(m_prototype->roleNames());
#endif
    qRegisterMetaType<QModelIndex>("QModelIndex");
    isActive = false;

}

int EPGChannelList::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_list.size();
}

QVariant EPGChannelList::data(const QModelIndex &index, int role) const
{
    if(index.row() < 0 || index.row() >= m_list.size())
        return QVariant();
    return m_list.at(index.row())->data(role);
}

EPGChannelList::~EPGChannelList() {
    delete m_prototype;
    clear();
}

void EPGChannelList::appendRow( EPGItemClass *item)
{
    //appendRows(QList<EPGItemClass*>() << new EPGItemClass(item->name(), item->channel(), item->name(), item->program(), item->id(), item->channelImage(), item->programImage(), this ) );

}

void EPGChannelList::appendRows(const QList<EPGItemClass *> &items)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount()+items.size()-1);
    foreach(EPGItemClass *item, items)
    {
        connect(item, SIGNAL(dataChanged()), this , SLOT(handleItemChange()));
        connect(item, SIGNAL(destroyed()), this, SLOT(itemDeleted()));
        m_list.append(item);
    }
    endInsertRows();
    QModelIndex index = indexFromItem(m_list.last());
    QModelIndex index2 = indexFromItem(m_list.first());
    int currentRows= m_list.count() - 1;
    emit dataChanged(index2, index, currentRows);
    isActive = true;
}

void EPGChannelList::insertRow(int row, EPGItemClass *item)
{
    beginInsertRows(QModelIndex(), row, row);
    connect(item, SIGNAL(dataChanged()), this, SLOT(handleItemChange()));
    qDebug() << "Inserting at:" << row;
    m_list.insert(row, item);
    endInsertRows();
}

void EPGChannelList::handleItemChange()
{
    EPGItemClass* item = static_cast<EPGItemClass*>(sender());
    QModelIndex index = indexFromItem(item);
    //qDebug() << "Handling item change for:" << index;
    if(index.isValid())
    {
         emit dataChanged(index, index, 0);
    }
}

void EPGChannelList::resetInternalData()
{
    qDebug("Clearing Television Channel Listmodel data.");
     int counter=0;

    QList<EPGItemClass*>::iterator i;
    for( i = m_list.begin(); i !=m_list.end(); ++i){
        EPGItemClass* pItem = m_list.takeFirst();

        counter++;
    }
    qDebug("Television Channel Listmodel data Cleared.");

}

EPGItemClass * EPGChannelList::find(const QString &id) const
{
    foreach(EPGItemClass* item, m_list)
    {
//        if(item->id() == id.toInt() )
//        {

//            return item;
//        }
    }
    return 0;
}

QModelIndex EPGChannelList::indexFromItem(const EPGItemClass *item) const
{
    Q_ASSERT(item);
    for(int row=0; row<m_list.size(); ++row) {

        if(m_list.at(row) == item) return index(row);

    }

    return QModelIndex();
}

void EPGChannelList::clear()
{

    qDeleteAll(m_list.begin(), m_list.end());
    m_list.clear();


}

bool EPGChannelList::removeRow(int row, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    if(row < 0 || row >= m_list.size()) return false;
    beginRemoveRows(QModelIndex(), row, row);
    delete m_list.takeAt(row);
    endRemoveRows();
    return true;
}

bool EPGChannelList::removeRows(int row, int count, const QModelIndex &parent)
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

EPGItemClass * EPGChannelList::takeRow(int row)
{
    beginRemoveRows(QModelIndex(), row, row);
    EPGItemClass* item = m_list.takeAt(row);
    endRemoveRows();
    return item;
}

EPGItemClass * EPGChannelList::currentRow()
{
    EPGItemClass* item = m_list.at(0);
    return item;
}

void EPGChannelList::setItemStatus(int pos)
{
    EPGItemClass * item = find(QString::number(pos));

}

bool EPGChannelList::checkDupe(QString name, QString position)
{
//    //qDebug() << "Checking dupe for:" << name << " at " <<position;

//    if (EPGItemClass *item = find(name))
//    {   //qDebug() << "Found " << name;

//        if (QString::number(item->index()-1).compare(position))
//        {
//            // qDebug() << "Dupe position " << position.toInt() ;
//            return false;
//        }
//        else
//        {
//            // qDebug() << item->index();
//            // qDebug() << "Did not find item at postion " << position;
//            return  true;
//        }

//    }
//    else
//    {
//        //qDebug() << name << " at " << position << " Not in playlist";
//        return true;
//    }

}

QModelIndex EPGChannelList::getChannelIndex(const QString &name) const
{
//    // qDebug() << "Finding tv program index for " << name ;
//    if (m_list.size() > 0)
//    {

//        for (int l=0; m_list.size() > l; l++)
//        {
//            //qDebug() << m_list.at(l)->mythid() << "::" << name ;
//            if (m_list.at(l)->data(3).toInt() == name.toInt()||m_list.at(l)->mythid() == name)
//            {

//                QModelIndex index = indexFromItem(m_list.at(l));
//                //   qDebug() << index;
//                return index;
//            }
//        }
//    }
//    else
//    {
//        return QModelIndex();
//    }
}

void EPGChannelList::populate()
{
    emit modelAboutToBeReset();
    beginResetModel();
    resetInternalData();
    //qDeleteAll(m_list);
    endResetModel();
    emit modelReset();
    emit requestEpg();
}

void EPGChannelList::setProgram(QString qml_text_channel)
{
    id = qml_text_channel;
    //qDebug("Set liveTv channel");

}

void EPGChannelList::setCurrentIndex(QModelIndex index)
{
    activeIndex = index;
    emit activeIndexChanged();
}

void EPGChannelList::setMythProgram(QString channel)
{
    id = channel;
}

void EPGChannelList::updatePosition()
{
    QModelIndex epgLocation = getMythChannelIndex(id);
    if (epgLocation.isValid())
    {
        setCurrentIndex(epgLocation);
        setCurrentRow(epgLocation.row());
        emit networkChanged(data(epgLocation,1).toString());
        emit channelNumberChanged(data(epgLocation, 3).toString());
        emit programChanged(data(epgLocation,5).toString());

    }
    else
    {
        // qDebug("Cant find myth station");
    }
}

QModelIndex EPGChannelList::getCurrentIndex()
{
    return activeIndex;
}

QModelIndex EPGChannelList::getMythChannelIndex(QString m)
{
//    // qDebug() << "Finding mythTV program index for " << m ;
//    if (m_list.size() > 0)
//    {

//        for (int l=0; m_list.size() > l; l++)
//        {

//            if (m_list.at(l)->mythid() == m)
//            {
//                //  qDebug() << m_list.at(l)->name() << "::" << m ;
//                QModelIndex index = indexFromItem(m_list.at(l));
//                // qDebug() << index;
//                return index;
//            }

//        }
//    }
//    else
//    {
//        return QModelIndex();
//    }
}

void EPGChannelList::empty()
{

    QApplication::processEvents(QEventLoop::AllEvents);
    emit modelAboutToBeReset();
    beginResetModel();
    resetInternalData();
    setProgress(0.0);
    QApplication::processEvents(QEventLoop::AllEvents);
    endResetModel();
    emit modelReset();
    QApplication::processEvents(QEventLoop::AllEvents);

}

void EPGChannelList::sortModel(int column, Qt::SortOrder order)
{
}



void EPGChannelList::updateLivePosition()
{
    QModelIndex epgLocation = getChannelIndex(id);
    if (epgLocation.isValid())
    {
        setCurrentIndex(epgLocation);
        setCurrentRow(epgLocation.row());
        emit networkChanged(data(epgLocation,1).toString());
        emit channelNumberChanged(data(epgLocation, 3).toString());
        emit programChanged(data(epgLocation,5).toString());
    }
    else
    {
        //qDebug() << "Cant find live tv station" << id;
    }
}

#ifdef QT5
QHash<int, QByteArray> EPGChannelList::roleNames() const
{
    return m_prototype->roleNames();
}
#endif
