#include "attributesortmodel.h"

#include <QDebug>


AttributeSortModel::AttributeSortModel(AttributeSortItem* prototype, int filterNumber, bool exclusive, QObject *parent) :
    QAbstractListModel(parent), m_prototype(prototype), filterLevel(filterNumber), m_allowMulti(exclusive)
{
#ifndef QT5
    setRoleNames(m_prototype->roleNames());
#endif
    qRegisterMetaType<QModelIndex>("QModelIndex");
}

int AttributeSortModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_list.size();
}

QVariant AttributeSortModel::data(const QModelIndex &index, int role) const
{
    if(index.row() < 0 || index.row() >= m_list.size())
        return QVariant();
    return m_list.at(index.row())->data(role);
}

AttributeSortModel::~AttributeSortModel() {
    delete m_prototype;
    clear();
}

void AttributeSortModel::appendRow(AttributeSortItem *item)
{
    appendRows(QList<AttributeSortItem*>() << new AttributeSortItem(item->name(), item->fileformat(), item->cellImage(), item->selectedStatus(), this));
    item->destruct();
}

void AttributeSortModel::appendRows(const QList<AttributeSortItem *> &items)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount()+items.size()-1);
    foreach(AttributeSortItem *item, items) {
        QObject::connect(item, SIGNAL(dataChanged()), this, SLOT(handleItemChange()));
        QObject::connect(item, SIGNAL(destroyed()), this, SLOT(itemDeletion()));
        m_list.append(item);
    }

    endInsertRows();
    //QModelIndex index = indexFromItem(m_list.last());
    //QModelIndex index2 = indexFromItem(m_list.first());
    //int currentRows= m_list.count() - 1;
    //emit dataChanged(index2, index, currentRows);

}

void AttributeSortModel::insertRow(int row, AttributeSortItem *item)
{
    beginInsertRows(QModelIndex(), row, row);
    connect(item, SIGNAL(dataChanged()), this, SLOT(handleItemChange()));
  //  qDebug() << "Inserting at:" << row;
    m_list.insert(row, item);
    endInsertRows();
}

void AttributeSortModel::handleItemChange()
{
    AttributeSortItem* item = static_cast<AttributeSortItem*>(sender());
    QModelIndex index = indexFromItem(item);
  //  qDebug() << "Handling item change for:" << index << item->name();
    if(index.isValid())
    {
        emit dataChanged(index, index ,index.row());
    } else {
        qDebug() << " invalid model index" << index;
    }
}

void AttributeSortModel::reset()
{

    emit modelAboutToBeReset();
    beginResetModel();
    if(resetInternalData()){
        endResetModel();
    }
}

bool AttributeSortModel::resetInternalData()
{

    qDeleteAll(m_list);
    m_list.clear();
   qDebug("Attribute Sort Model Cleared.");
    return true;
}
bool AttributeSortModel::allowMulti() const
{
    return m_allowMulti;
}

void AttributeSortModel::setAllowMulti(bool allowMulti)
{
    m_allowMulti = allowMulti;
}


AttributeSortItem * AttributeSortModel::find(const QString &id) const
{
    foreach(AttributeSortItem* item, m_list) {
        if(item->id() == id) return item;
    }
    return 0;
}

QModelIndex AttributeSortModel::indexFromItem(const AttributeSortItem *item) const
{

    Q_ASSERT(item);

    for(int row=0; row<m_list.size(); ++row) {

        if(m_list.at(row) == item) return index(row);

    }

    return QModelIndex();
}

void AttributeSortModel::clear()
{

    this->reset();

}

bool AttributeSortModel::removeRow(int row, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    if(row < 0 || row >= m_list.size()) return false;
    beginRemoveRows(QModelIndex(), row, row);
    delete m_list.takeAt(row);
    endRemoveRows();
    return true;
}

bool AttributeSortModel::removeRows(int row, int count, const QModelIndex &parent)
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

AttributeSortItem * AttributeSortModel::takeRow(int row)
{
    beginRemoveRows(QModelIndex(), row, row);
    AttributeSortItem* item = m_list.takeAt(row);
    endRemoveRows();
    return item;
}

AttributeSortItem * AttributeSortModel::currentRow()
{
    AttributeSortItem* item = m_list.at(0);
    return item;
}

void AttributeSortModel::sortModel(int column, Qt::SortOrder order)
{

}

void AttributeSortModel::setSelectionStatus(QString format)
{

    AttributeSortItem* item = find(format);
    if(!item){
        qWarning() << format << " was not found";
        return;
    }
    item->updateSelection(!item->selectedStatus());
    if(!m_allowMulti){
        qDebug() << "exclusive sort, unchecking other items!";
        foreach(AttributeSortItem* uitem, m_list) {
            if(uitem->id()!= item->id()) uitem->m_isSelected = false;
        }
    }
  //  qDebug() << "Set State for:" << format << "to " << item->selectedStatus();

    //return state;
    ReturnSelectedItems();
}

bool AttributeSortModel::getSelectionStatus(QString format)
{
    //qDebug() << "Looking for status for" << format;
    AttributeSortItem* item = find(format);
    bool g = item->selectedStatus();
    return g;
}

bool AttributeSortModel::clearSelections()
{

}

void AttributeSortModel::ReturnSelectedItems()
{
    QStringList t_selected_items;
    foreach(AttributeSortItem* item, m_list) {
        if(item->selectedStatus() == true) t_selected_items.append(item->fileformat());
    }
    QString qs_sorting_string= t_selected_items.join(",");
  //  qDebug() << "Attribute Sort updated sorting filter" << qs_sorting_string;
    emit SetTypeSort(filterLevel, qs_sorting_string);
}

void AttributeSortModel::resetStates()
{
    foreach(AttributeSortItem* item, m_list) {
        item->setStatus(false);
    }
    return ReturnSelectedItems();
}

#ifdef QT5
QHash<int, QByteArray> AttributeSortModel::roleNames() const
{
    return m_prototype->roleNames();
}
#endif



