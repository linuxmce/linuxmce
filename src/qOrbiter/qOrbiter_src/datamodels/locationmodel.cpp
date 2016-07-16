#include "locationmodel.h"


LocationModel::LocationModel(LocationItem* prototype, QObject *parent) :
    QAbstractListModel(parent), m_prototype(prototype)
{
    qRegisterMetaType<EntertainAreaTimeCode*>("EntertainAreaTimeCode*");
#ifndef QT5
    setRoleNames(m_prototype->roleNames());
#endif

}

int LocationModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_list.size();
}

QVariant LocationModel::data(const QModelIndex &index, int role) const
{
    if(index.row() < 0 || index.row() >= m_list.size())
        return QVariant();
    return m_list.at(index.row())->data(role);
}

LocationModel::~LocationModel() {
    delete m_prototype;
    clear();
}

void LocationModel::appendRow(LocationItem *item)
{
    appendRows(QList<LocationItem*>() << item);
}

void LocationModel::appendRows(const QList<LocationItem *> &items)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount()+items.size()-1);
    foreach(LocationItem *item, items) {
        connect(item, SIGNAL(dataChanged()), SLOT(handleItemChange()));
        m_list.append(item);
    }
    endInsertRows();
}

void LocationModel::insertRow(int row, LocationItem *item)
{
    beginInsertRows(QModelIndex(), row, row);
    connect(item, SIGNAL(dataChanged()), SLOT(handleItemChange()));
    // m_list.insert(row, item);
    endInsertRows();
}

void LocationModel::handleItemChange()
{
    LocationItem* item = static_cast<LocationItem*>(sender());
    QModelIndex index = indexFromItem(item);
    if(index.isValid())
        emit dataChanged(index, index);
}
EntertainAreaTimeCode *LocationModel::currentEaTimecode() const
{
    if(!m_currentEaTimecode)
        return new EntertainAreaTimeCode();
    else
        return m_currentEaTimecode;
}

void LocationModel::setCurrentEaTimecode(EntertainAreaTimeCode *currentEaTimecode)
{
    if(m_currentEaTimecode->eaId()==currentEaTimecode->eaId())return;
    m_currentEaTimecode = currentEaTimecode;
    emit currentEaTimecodeChanged();
}

void LocationModel::setEaTimeCode(QString ea, QMap<long, std::string> data)
{

    for (int i = 0; i < eaTimecodeItems.size(); ++i) {
        if(eaTimecodeItems.at(i)->eaName() ==ea){
            eaTimecodeItems.at(i)->setCurrentTimeCode(QString::fromStdString(data[74]) );
            eaTimecodeItems.at(i)->setTotalTimeCode( QString::fromStdString(data[57]));
            return;
        }
    }
    qDebug() << "Could not set ea " << ea;
}

void LocationModel::setEaTimeCode(int room, QMap<long, std::string> data)
{
    for (int i = 0; i < eaTimecodeItems.size(); ++i) {
        if(eaTimecodeItems.at(i)->getI_roomId() ==room){
            eaTimecodeItems.at(i)->setCurrentTimeCode(QString::fromStdString(data[74]) );
            eaTimecodeItems.at(i)->setTotalTimeCode( QString::fromStdString(data[57]));
            return;
        }
    }
    qDebug() << "Could not set ea  for room" << room;
}

bool LocationModel::addTimeCodeTrack(QString ea, int intEa, int room)
{
    EntertainAreaTimeCode *t  = new EntertainAreaTimeCode(intEa, ea, room);
    connect(t, &EntertainAreaTimeCode::seekToTime, this, &LocationModel::seekToTime);
    eaTimecodeItems.append(t);
    return true;
}

QList<EntertainAreaTimeCode *> LocationModel::allTimeCode()
{
    QList<EntertainAreaTimeCode*> ret;
    for (int i = 0; i < eaTimecodeItems.size(); ++i) {
        ret.append(eaTimecodeItems.at(i));

    }
    return ret;

}

QVariantList LocationModel::eaList()
{
    QVariantList t;

    foreach(LocationItem* item, m_list) {

        foreach(QVariant tmp, item->entertainAreas()){
           QVariantMap t1 = tmp.toMap();
          if(t1.value("ea_number")!=0){
              t.append(tmp);
          }
        }

    }
    qDebug() << t;
    return t;
}


LocationItem * LocationModel::find(const QString &id) const
{
    foreach(LocationItem* item, m_list) {
        if(item->id() == id) return item;
    }
    return 0;
}

void LocationModel::sortModel(int column, Qt::SortOrder order)
{

}

bool LocationModel::check(int room)
{
    foreach(LocationItem* item, m_list) {
        if(item->roomVal() == room) return true;
    }
    return false;
}



#ifdef QT5
QHash<int, QByteArray> LocationModel::roleNames() const
{
    return m_prototype->roleNames();
}
#endif

QModelIndex LocationModel::indexFromItem(const LocationItem *item) const
{
    Q_ASSERT(item);
    for(int row=0; row<m_list.size(); ++row) {
        if(m_list.at(row) == item) return index(row);
    }
    return QModelIndex();
}

void LocationModel::clear()
{
    qDeleteAll(m_list);
    m_list.clear();
}

void LocationModel::setLocation(int ea, int room)
{

    foreach(LocationItem* item, m_list) {
        if(item->roomVal()== room )
        {
            QVariantList l = item->entertainAreas();
            foreach(QVariant t, l){
                QVariantMap tl = t.toMap();
                if(tl["ea_number"].toInt()==ea){
                    i_currentEA = ea;
                    i_currentRoom= room;
                    if(tl["ea_number"].toInt()==0){
                         setCurrentEA(item->id());
                    } else {
                        setCurrentEA(tl["ea_name"].toString());
                    }
                    setCurrentRoom(item->id());
                    //setCurrentItem(indexFromItem(item).row());                          .
                }
            }

        }
    }

    if(eaTimecodeItems.isEmpty()) return;

    for (int i = 0; i < eaTimecodeItems.size(); ++i) {
        if(eaTimecodeItems.at(i)->eaId()==ea){
            m_currentEaTimecode = eaTimecodeItems.at(i);
            emit currentEaTimecodeChanged();
        }
    }
}

QString LocationModel::getRoomName(int r)
{

    foreach(LocationItem* item, m_list) {
        if(item->roomVal() == r) return item->data(item->NameRole).toString();
    }
    return false;
}

bool LocationModel::removeRow(int row, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    if(row < 0 || row >= m_list.size()) return false;
    beginRemoveRows(QModelIndex(), row, row);
    delete m_list.takeAt(row);
    endRemoveRows();
    return true;
}

bool LocationModel::removeRows(int row, int count, const QModelIndex &parent)
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

LocationItem * LocationModel::takeRow(int row)
{
    beginRemoveRows(QModelIndex(), row, row);
    LocationItem* item = m_list.takeAt(row);
    endRemoveRows();
    return item;
}
