#include "genericflatlistmodel.h"
#include <QDebug>
/*!
 *\class GenericFlatListModel
 *\brief This class is a generic flat C++ listmodel for use with qml
 *
 *\ingroup qt_controllers
 *
 *The GenericFlatListModel is based on QAbstractListmodel. Its purpose is to provide a non-abstract listmodel class that is easiliy
 *availible for use with the various lists and models that are part of LinuxMCE.
 *
 *\sa QAbstractListmodel
 *
 */

GenericFlatListModel::GenericFlatListModel(GenericModelItem *prototypeItem, QObject *parent) :
    QAbstractListModel(parent), m_prototype(prototypeItem)
{
#ifndef QT5
    setRoleNames(m_prototype->roleNames());
#endif
    qRegisterMetaType<QModelIndex>("QModelIndex");
    totalcells = 0;
    seperator = 16;
    loadingStatus = false;
    progress = 0;
    clearing = false;
    clear();
    setTotalPages(0);
}

int GenericFlatListModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid()){
        return 0;
    }
    else{
        return m_list.size();
    }

}


QVariant GenericFlatListModel::data(const QModelIndex &index, int role) const
{
    if(index.row() < 0 || index.row() >= m_list.size())
        return QVariant();
    return m_list.at(index.row())->data(role);
}

QHash<int, QByteArray> GenericFlatListModel::roleNames() const
{
    return m_prototype->roleNames();
}

GenericFlatListModel::~GenericFlatListModel() {
    delete m_prototype;
    clear();
}

void GenericFlatListModel::appendRow(GenericModelItem *item)
{
    setLoadingStatus(true);
    appendRows(QList<GenericModelItem*>() << item);
  //  qDebug() << m_list.count();
}

void GenericFlatListModel::appendRows(const QList<GenericModelItem *> &items)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount()+items.size()-1);
    foreach(GenericModelItem *item, items) {
        m_list.append(item);
        QObject::connect(item, SIGNAL(dataChanged()), this , SLOT(handleItemChange()));

    }

    endInsertRows();
    QModelIndex index = indexFromItem(m_list.last());
    QModelIndex index2 = indexFromItem(m_list.first());
    int currentRows= m_list.count();
    emit itemAdded(currentRows);
    setCurrentCells(currentRows);
    double p = (((double)m_list.size() / (double)seperator) * 100) ;
    setProgress(p);
    emit dataChanged(index2, index, currentRows);
    setLoadingStatus(false);
    QApplication::processEvents(QEventLoop::AllEvents);

}

void GenericFlatListModel::insertRow(int row, GenericModelItem *item)
{

    beginInsertRows(QModelIndex(), row, row);
    connect(item, SIGNAL(dataChanged()), this, SLOT(handleItemChange()));
    ////qdebug() << "Inserting at:" << row;
    m_list.insert(row, item);
    endInsertRows();

}

void GenericFlatListModel::handleItemChange()
{
    GenericModelItem* item = static_cast<GenericModelItem*>(sender());
    QModelIndex index = indexFromItem(item);
    qDebug() << "Handling item change for:" << index;
    if(index.isValid())
    {
        emit dataChanged(index, index, index.row() );
    }
}

void GenericFlatListModel::reset()
{
    clearing = true;
    QApplication::processEvents(QEventLoop::AllEvents);
    emit modelAboutToBeReset();
    beginResetModel();
    resetInternalData();
    setProgress(0.0);
    QApplication::processEvents(QEventLoop::AllEvents);
    endResetModel();
    emit modelReset();
    QApplication::processEvents(QEventLoop::AllEvents);
   clearing = false;
}

bool GenericFlatListModel::resetInternalData()
{
    qDebug("Resetting GenericFlatListModel data");
    int total = m_list.count();
    for(int i = 0; i <= m_list.count(); ++i){

      //  qDebug() <<"removing::" << m_list.count() << " of " << total;
        m_list.removeAt(i);
    }
    m_list.clear();
  //  qDebug() << "Items cleared. Count:: "<< m_list.count();
    return true;

}

GenericModelItem* GenericFlatListModel::find(const QString &id) const
{
    foreach(GenericModelItem* item, m_list) {
        if(item->id() == id) return item;
    }
    return 0;
}

QVariant GenericFlatListModel::get(int index, const QString &name) const
{
    if (index>=0 && index<m_list.size()) {
        GenericModelItem* myItem = m_list.at(index);

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

QModelIndex GenericFlatListModel::indexFromItem(const GenericModelItem *item) const
{
    Q_ASSERT(item);
    for(int row=0; row<m_list.size(); ++row) {

        if(m_list.at(row) == item) return index(row);

    }

    return QModelIndex();
}

void GenericFlatListModel::clear()
{
    clearing = true;
    QApplication::processEvents(QEventLoop::AllEvents);
    emit modelAboutToBeReset();
    beginResetModel();
    if(resetInternalData()){
    setProgress(0.0);
    QApplication::processEvents(QEventLoop::AllEvents);
    endResetModel();
    emit modelReset();
    QApplication::processEvents(QEventLoop::AllEvents);
    clearing = false;
    }

}

bool GenericFlatListModel::removeRow(int row, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    if(row < 0 || row >= m_list.size()) return false;
    beginRemoveRows(QModelIndex(), row, row);
    delete m_list.takeAt(row);
    endRemoveRows();
    return true;
}

bool GenericFlatListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    if(row < 0 || (row+count) > m_list.size()) return false;
    beginRemoveRows(QModelIndex(), row, row+count-1);
    for(int i=0; i<count; ++i) {
        delete m_list.takeAt(row);
    }
    endRemoveRows();
    return true;
}

GenericModelItem* GenericFlatListModel::takeRow(int row)
{
    beginRemoveRows(QModelIndex(), row, row);
    GenericModelItem* item = m_list.takeAt(row);
    endRemoveRows();
    return item;
}

GenericModelItem* GenericFlatListModel::currentRow()
{
    GenericModelItem* item = m_list.at(0);
    return item;
}

void GenericFlatListModel::sortModel(int column, Qt::SortOrder order)
{
}

void GenericFlatListModel::checkForMore()
{


}

void GenericFlatListModel::populateGrid(int mediaType)
{
    //manager_ref->pqOrbiter->prepareFileList(mediaType);
}

void GenericFlatListModel::setTotalCells(int cells)
{
    totalcells = cells;
    emit statusMessage("Size Changed" + QString::number(totalcells));
    if ( m_list.size() < totalcells)
    {



    }
    else
    {


    }
    emit sizeChanged(cells);

}

int GenericFlatListModel::getTotalCells()
{
    return totalcells;
}

void GenericFlatListModel::setGridType(int type)
{
    gridType = type;
    //emit gridTypeChanged(gridType);
    ////qdebug("Grid Type Set");

}

int GenericFlatListModel::getGridType()
{
    return gridType;
}

void GenericFlatListModel::setLoadingStatus(bool b)
{
    loadingStatus = b;
    emit loadingStatusChanged(loadingStatus);
}

bool GenericFlatListModel::getLoadingStatus()
{
    return loadingStatus;
}

void GenericFlatListModel::setProgress(double n_progress)
{
    progress = n_progress;
    emit progressChanged( progress );
    // qDebug() << "Loading Progress:" << progress;
    // qDebug() << QString::number(m_list.count()) +" cells in model, out of " + QString::number(totalcells);

}

double GenericFlatListModel::getProgress()
{
    return progress;
}

void GenericFlatListModel::attributeSort()
{


}

void GenericFlatListModel::clearAndRequest(int type)
{
    clearing = true;
    gridType = type;
    QApplication::processEvents(QEventLoop::AllEvents);
    emit modelAboutToBeReset();
    beginResetModel();
    if(    resetInternalData()){
        setProgress(0.0);
        QApplication::processEvents(QEventLoop::AllEvents);
        endResetModel();
        emit modelReset();

        QApplication::processEvents(QEventLoop::AllEvents);
        clearing = false;
        emit ready(gridType);
    }
    else
    {

    }
}

void GenericFlatListModel::clearForPaging()
{

    /*
    if(removeRows(0, m_list.count(), QModelIndex()))
        emit pagingCleared();
        */
    QApplication::processEvents(QEventLoop::AllEvents);
    emit modelAboutToBeReset();
    beginResetModel();
    if(resetInternalData()){
        setProgress(0.0);
        QApplication::processEvents(QEventLoop::AllEvents);
        endResetModel();
        emit modelReset();
        emit pagingCleared();
        QApplication::processEvents(QEventLoop::AllEvents);
    }

}
