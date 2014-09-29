#include "existingorbitermodel.h"

ExistingOrbiterModel::ExistingOrbiterModel(ExistingOrbiter *prototype, QObject *parent) :
    QAbstractListModel(parent), m_prototype(prototype)
{
#ifndef QT5
 setRoleNames(m_prototype->roleNames());
#endif
 qRegisterMetaType<QModelIndex>("QModelIndex");
}

#ifdef QT5
QHash<int, QByteArray> ExistingOrbiterModel::roleNames() const
{
     return m_prototype->roleNames();
}
#endif

void ExistingOrbiterModel::appendRow(ExistingOrbiter *item)
{
    appendRows(QList<ExistingOrbiter*>() << item);
}

void ExistingOrbiterModel::appendRows(const QList<ExistingOrbiter *> &items)
{
    beginInsertRows(QModelIndex(), rowCount(QModelIndex()), rowCount(QModelIndex())+items.size()-1);
    foreach(ExistingOrbiter *item, items) {

        if(m_list.contains(item)){
            return;
        }
        QObject::connect(item, SIGNAL(dataChanged()), this, SLOT(handleItemChange()));
        m_list.append(item);
    }

    endInsertRows();
    QModelIndex index = indexFromItem(m_list.last());
    QModelIndex index2 = indexFromItem(m_list.first());
    emit dataChanged(index2, index);
}


QVariant ExistingOrbiterModel::data(const QModelIndex &index, int role) const
{

        if(index.row() < 0 || index.row() >= m_list.size())
            return QVariant();
        return m_list.at(index.row())->data(role);

}

int ExistingOrbiterModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid()){
        return 0;
    }
    else{
        return m_list.size();
    }
}


QModelIndex ExistingOrbiterModel::indexFromItem(const ExistingOrbiter *item) const
{
    Q_ASSERT(item);
    for(int row=0; row<m_list.count(); ++row) {
        //  qDebug() << "item:" << item->id() << "::" << m_list.at(row)->id();
        if(m_list.at(row)->id() == item->id()) {

            return index(row,0);
        }
        else
        {
            //  qDebug("item not Found");
        }

    }

    return QModelIndex();
}

void ExistingOrbiterModel::handleItemChange()
{
    ExistingOrbiter* item = static_cast<ExistingOrbiter*>(sender());
    QModelIndex index = indexFromItem(item);
    if(index.isValid())
    {
        emit dataChanged(index, index);
    }
}


