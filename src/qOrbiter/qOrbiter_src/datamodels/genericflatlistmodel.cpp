#include "genericflatlistmodel.h"
#include "DCE/Logger.h"
#include "../qorbitermanager.h"

using namespace DCE;

/*!
 *\class GenericFlatListModel
 *\brief This class is a generic flat C++ listmodel that wraps a LinuxMCE datagrid, for use with qml
 *
 *\ingroup qt_controllers
 *
 *The GenericFlatListModel is based on QAbstractListmodel. Its purpose is to provide a non-abstract listmodel class that is easiliy
 *availible for use with the various datagrids that are part of LinuxMCE.
 *
 *This class allows transparently for lazy loading and a window of loaded items. It also supports seeking.
 *
 *\sa QAbstractListmodel
 *
 */

GenericFlatListModel::GenericFlatListModel(QObject *parent) :
  QAbstractListModel(parent), m_prototype(NULL)
{
    qRegisterMetaType<QModelIndex>("QModelIndex");
    loaded = false;
    m_windowSize = 100;
    totalRows = 0;
    m_totalCols = 0;
    m_seek = false;
    resetWindow();
    mediaType=-1;
}
GenericFlatListModel::GenericFlatListModel(GenericModelItem *prototypeItem, QObject *parent) :
    QAbstractListModel(parent), m_prototype(prototypeItem)
{
#ifndef QT5
    setRoleNames(m_prototype->roleNames());
#endif
    qRegisterMetaType<QModelIndex>("QModelIndex");
    qRegisterMetaType<QMap<int, QString*> >("QMap");
    loaded = false;
    m_windowSize = 100;
    totalRows = 0;
    m_totalCols = 0;
    m_seek = false;
    resetWindow();

    seperator = 16;
    loadingStatus = false;
    progress = 0;
    clearing = false;

    clear();
}

void GenericFlatListModel::resetWindow() {
    m_list.clear();
    m_windowStart = 0;
    m_iLastRow = 0;
    m_requestStart = -1;
    m_requestEnd = -1;
    m_bForward = true;
}

void GenericFlatListModel::setPrototype(GenericModelItem* pItem)
{
  m_prototype = pItem;
#ifndef QT5
    setRoleNames(m_prototype->roleNames());
#endif
}
int GenericFlatListModel::rowCount(const QModelIndex &parent) const
{
  //    Q_UNUSED(parent);
    if (!parent.isValid()) { // invalid parent == root parent
      //        LoggerWrapper::GetInstance()->Write(LV_STATUS, "GenericFlatListModel.rowCount = %d", totalRows);
        return totalRows;
    } else {
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "GenericFlatListModel.rowCount invalid parent index");
        return 0;
    }
}


// not const per se, but from the outside this object contains all data at any time, so lets roll with that
QVariant GenericFlatListModel::data(const QModelIndex &index, int role) const
{
    //LoggerWrapper::GetInstance()->Write(LV_STATUS, "GenericFlatListModel.data row = %d (0->%d)[%d->%d]", index.row(), totalRows, m_windowStart, m_windowStart+m_list.size());
    // index outside the range of the datagrid ?
    if(index.row() < 0 || index.row() >= totalRows)
        return QVariant();
    
    // index outside of the loaded items ?
    if (index.row() < m_windowStart || index.row() >= m_windowStart+m_list.size()) {
        // This method must be const to satisfy the superclass, but even though we are actually changing the state of the
        // object here, the change is only internal, externally we always fake it and say we have the total number of rows
        const_cast<GenericFlatListModel*>(this)->requestMoreData(index.row(), index.row()-m_iLastRow);
    const_cast<GenericFlatListModel*>(this)->m_iLastRow = index.row();
    // Only return Loading... for the first role
    if (role == Qt::UserRole+1) {
     //  qDebug() << "not loaded";
        return QString("Loading...");
    } else {
        return QVariant("");
    }
    }
    return m_list.at(index.row()-m_windowStart)[0]->data(role);
}

bool GenericFlatListModel::setData(const int index, const QString roleName, const QVariant & value)
{
 // LoggerWrapper::GetInstance()->Write(LV_DEBUG, "GenericFlatListModel.setData, index = %d", index);
    
    if(index < 0 || index >= totalRows)
        return false;

    // index outside of the loaded items ?
    if (index < m_windowStart || index >= m_windowStart+m_list.size())
        return false;

    int role = m_prototype->roleNames().key(roleName.toUtf8());

    if (role > 0)
        return m_list.at(index-m_windowStart)[0]->setData(role, value);
    else
        return false;
}

Qt::ItemFlags GenericFlatListModel::flags ( const QModelIndex & index ) const 
{
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

QHash<int, QByteArray> GenericFlatListModel::roleNames() const
{
    return m_prototype->roleNames();
}

GenericFlatListModel::~GenericFlatListModel() {
    if (m_prototype != NULL)
        delete m_prototype;
    clear();
}

void GenericFlatListModel::appendRow(GenericModelItem *item)
{
    //    setLoadingStatus(true);
    appendRows(QList<GenericModelItem*>() << item);
}

void GenericFlatListModel::appendRows(const QList<GenericModelItem *> &items)
{
  //  LoggerWrapper::GetInstance()->Write(LV_DEBUG, "GenericFlatListModel.appendRows start");
    beginInsertRows(QModelIndex(), rowCount(), rowCount()+items.size()-1);
    foreach(GenericModelItem *item, items) {
        QApplication::processEvents(QEventLoop::AllEvents);
        item->setParent(this);
        QObject::connect(item, SIGNAL(dataChanged()), this , SLOT(handleItemChange()));
        m_list.append(QVector<GenericModelItem*>(m_totalCols, item));
    }
    endInsertRows();
    
    QModelIndex index = indexFromItem(m_list.last()[0]);
    QModelIndex index2 = indexFromItem(m_list.first()[0]);
    int currentRows= m_list.count();
    //    emit itemAdded(currentRows);
    // setCurrentCells(currentRows);
    // double p = (((double)m_list.size() / (double)seperator) * 100) ;
    //setProgress(p);
    emit dataChanged(index2, index);
    /* setLoadingStatus(false);
    QApplication::processEvents(QEventLoop::AllEvents);*/

  //  LoggerWrapper::GetInstance()->Write(LV_DEBUG, "GenericFlatListModel.appendRows end");
}

// Not to be confused with the QAbstractItemModel::insertRow
void GenericFlatListModel::insertRow(int row, QVector<GenericModelItem*> items)
{
 //   LoggerWrapper::GetInstance()->Write(LV_DEBUG, "GenericFlatListModel.insertRow row = %d", row);
    // Is received row within the expected request range
    if (m_requestStart >= 0 && row >= m_requestStart && row <= m_requestEnd) {
      bool okToInsert = false;
      if (m_bForward) {
    if (row == m_requestStart) {
    //  LoggerWrapper::GetInstance()->Write(LV_DEBUG, "GenericFlatListModel.insertRow forward request ok");
      m_requestStart++;
      okToInsert = true;
    }
      } else {
    if (row == m_requestEnd) {
//	  LoggerWrapper::GetInstance()->Write(LV_DEBUG, "GenericFlatListModel.insertRow backward request ok");
      m_requestEnd--;
      okToInsert = true;
    }
      }
      if (okToInsert) {
        m_list.insert(row-m_windowStart, items);
        for (int i = 0; i < m_totalCols && i < items.size(); i++) {
            items[i]->setParent(this);
            connect(items[i], SIGNAL(dataChanged()), this, SLOT(handleItemChange()));
        }
    if (!m_bForward)
        m_windowStart--;
    //	LoggerWrapper::GetInstance()->Write(LV_DEBUG, "GenericFlatListModel.insertRow emit dataChanged");
    emit dataChanged(index(row), index(row));
      }

      if (m_requestStart > m_requestEnd) {
   // LoggerWrapper::GetInstance()->Write(LV_DEBUG, "GenericFlatListModel.insertRow request complete");
    // we are done
    m_requestStart = -1;
    m_requestEnd = -1;
    beginResetModel();
    endResetModel();
        emit loadComplete();
      }
    }
    //    LoggerWrapper::GetInstance()->Write(LV_DEBUG, "GenericFlatListModel.insertRow end");
}

void GenericFlatListModel::handleItemChange()
{
    GenericModelItem* item = static_cast<GenericModelItem*>(sender());
    QModelIndex index = indexFromItem(item);
  //  LoggerWrapper::GetInstance()->Write(LV_DEBUG, "GenericFlatListModel.handleItemChange row = %d", index.row());
    if(index.isValid())
    {
   //     LoggerWrapper::GetInstance()->Write(LV_STATUS, "GenericFlatListModel.handleItemChange emit dataChanged");
        emit dataChanged(index, index);
    }
}

void GenericFlatListModel::reset()
{
    clearing = true;
    QApplication::processEvents(QEventLoop::AllEvents);
    //    emit modelAboutToBeReset();
    beginResetModel();
    resetInternalData();
    setProgress(0.0);
    endResetModel();
    //    emit modelReset();
    QApplication::processEvents(QEventLoop::AllEvents);
   clearing = false;
}

bool GenericFlatListModel::resetInternalData()
{
    qDebug("Resetting GenericFlatListModel data");
    return true;

}

GenericModelItem* GenericFlatListModel::find(const QString &id) const
{
    foreach(QVector<GenericModelItem*> item, m_list) {
        if(item[0]->id() == id) return item[0];
    }
    return 0;
}

void GenericFlatListModel::loadData()
{
    requestMoreData(0, 1);
    m_iLastRow = 0;
}

QVariant GenericFlatListModel::get(int index, const QString &name) const
{
    return get(index, 0, name);
}

QVariant GenericFlatListModel::get(int index, int col, const QString &name) const
{
    if (index>=0 && index<totalRows) {
      if (index < m_windowStart || index >= m_windowStart+m_list.size()) {
        // This method must be const to satisfy the superclass, but even though we are actually changing the state of the
        // object here, the change is only internal, externally we always fake it and say we have the total number of rows
        const_cast<GenericFlatListModel*>(this)->requestMoreData(index, index-m_iLastRow);
        const_cast<GenericFlatListModel*>(this)->m_iLastRow = index;
      } else {
          GenericModelItem* myItem = m_list.at(index-m_windowStart)[col];

      QHash<int, QByteArray> myHash = myItem->roleNames();
      QHash<int, QByteArray>::const_iterator i = myHash.constBegin();

      while (i != myHash.constEnd()) {
              if (i.value() == name) {
              return myItem->data(i.key());
          }
          ++i;
      }
      }
    } else {
        return "";
    }
    return "";
}

QModelIndex GenericFlatListModel::indexFromItem(const GenericModelItem *item) const
{
    Q_ASSERT(item);
    for(int row=0; row<m_list.size(); ++row) {

        if(m_list.at(row)[0] == item) return index(row+m_windowStart, 0);

    }
  //  LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "GenericFlatListModel.indexFromItem invalid item (or outside current window)");
    //TODO - how to deal with items outside our window
    return QModelIndex();
}

void GenericFlatListModel::updateItemData(int row, int role, QVariant value)
{
//    LoggerWrapper::GetInstance()->Write(LV_DEBUG, "GenericFlatListModel.updateItemData start");
    // TODO: should make sure model data is identical to when update request was sent
    if (row < m_windowStart || row > m_windowStart+m_list.size()) {
    //  LoggerWrapper::GetInstance()->Write(LV_WARNING, "GenericFlatListModel.updateItemData row outside loaded window");
    return;
    }
    GenericModelItem* pItem = m_list.at(row-m_windowStart)[0];
    pItem->updateData(role, value);
    QModelIndex index = indexFromItem(pItem);
    emit dataChanged(index, index);
    //    LoggerWrapper::GetInstance()->Write(LV_DEBUG, "GenericFlatListModel.updateItemData end");
}

void GenericFlatListModel::clear()
{
  //  LoggerWrapper::GetInstance()->Write(LV_WARNING, "GenericFlatListModel.clear start");
    clearing = true;
    loaded = false;
    if (totalRows > 0)
    {
        QModelIndex index;
        beginRemoveRows(index, 0, totalRows-1);
    totalRows = 0;
    //	m_totalCols = 0;
    resetWindow();
  //  LoggerWrapper::GetInstance()->Write(LV_DEBUG, "GenericFlatListModel.clear m_list cleared");
    endRemoveRows();
    beginResetModel();
    endResetModel();
    }
    clearing = false;
 //   LoggerWrapper::GetInstance()->Write(LV_DEBUG, "GenericFlatListModel.clear end");

}

bool GenericFlatListModel::removeRow(int row, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    if(row < m_windowStart || row >= m_windowStart+m_list.size()) return false;
    beginRemoveRows(QModelIndex(), row, row);
    foreach (GenericModelItem* pItem, m_list.takeAt(row))
        delete pItem;
    endRemoveRows();
    return true;
}

bool GenericFlatListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    if(row < m_windowStart || (row+count) > (m_windowStart+m_list.size())) return false;
    beginRemoveRows(QModelIndex(), row, row+count-1);
    for(int i=0; i<count; ++i) {
        QApplication::processEvents(QEventLoop::AllEvents);
        foreach (GenericModelItem* pItem, m_list.takeAt(row))
            delete pItem;
    }
    endRemoveRows();
    return true;
}

GenericModelItem* GenericFlatListModel::takeRow(int row)
{
    if(row < m_windowStart || row > (m_windowStart+m_list.size())) return new GenericModelItem();
    beginRemoveRows(QModelIndex(), row, row);
    QVector<GenericModelItem*> colVec = m_list.takeAt(row-m_windowStart);
    GenericModelItem* item = colVec[0];
    for (int i = 1; i < colVec.size(); i++)
        delete colVec[i];

    endRemoveRows();
    return item;
}

GenericModelItem* GenericFlatListModel::currentRow()
{
    QVector<GenericModelItem*> colVec = m_list.at(0);
    GenericModelItem* item = colVec[0];
    return item;
}

void GenericFlatListModel::sortModel(int column, Qt::SortOrder order)
{
}

void GenericFlatListModel::setTotalRows(int rows)
{

  //  LoggerWrapper::GetInstance()->Write(LV_WARNING, "GenericFlatListModel.setTotalRows rows = %d", rows);
    loaded = true; // tie into this method, as this is always called after a populate datagrid
    // ok, so the fakery starts here - once we got the number of rows, immediately say that we got them all..
    beginInsertRows(QModelIndex(), 0, (rows));
    totalRows = rows;

    // TODO ?? what are these for? standard list api?
    //    emit statusMessage("Size Changed" + QString::number(totalRows));
    //    emit sizeChanged(rows);
    endInsertRows();
    beginResetModel();
    endResetModel();

}

int GenericFlatListModel::getTotalRows()
{
    return totalRows;
}

int GenericFlatListModel::getTotalColumns()
{
    return m_totalCols;
}

void GenericFlatListModel::setLoadingStatus(bool b)
{
      loadingStatus = b;
  // emit loadingStatusChanged(loadingStatus);
}

bool GenericFlatListModel::getLoadingStatus()
{
    return loadingStatus;
}

void GenericFlatListModel::setProgress(double n_progress)
{
  progress = n_progress;
  // emit progressChanged( progress );
    // qDebug() << "Loading Progress:" << progress;
    // qDebug() << QString::number(m_list.count()) +" cells in model, out of " + QString::number(totalcells);

}

double GenericFlatListModel::getProgress()
{
    return progress;
}

void GenericFlatListModel::refreshData() {
 //   LoggerWrapper::GetInstance()->Write(LV_STATUS, "GenericFlatListModel.refreshData");
    QObject* p = QObject::parent();
    qorbiterManager* pManager = static_cast<qorbiterManager*>(p);
    // Clear all window items and request new datagrid data
    clear();
    if(pManager->currentIndex!=-1){
        setLastIndex(pManager->currentIndex);

     //  LoggerWrapper::GetInstance()->Write(LV_STATUS, "GenericFlatListModel.refreshData::found index to return to: %d", lastIndex);
    }

    pManager->refreshDataGrid(modelName, m_PK_DataGrid, m_option);
}

void GenericFlatListModel::requestMoreData(int row, int direction) {

   // LoggerWrapper::GetInstance()->Write(LV_WARNING, "GenericFlatListModel.requestMoreData row = %d", row);
    if (row >= m_requestStart && row <= m_requestEnd) {
      //  LoggerWrapper::GetInstance()->Write(LV_WARNING, "GenericFlatListModel.requestMoreData row already requested");
    return;
    }
    // TODO: need to examine threading/signals/slots to determine if we need to use a mutex
    // TODO: handle multiple requests, if a request for a new row outside the current request get here, we might get into trouble
    // the easiest way around this might be to add a request counter, and only accept rows inserted from the last request
    int newWindowStart;
    int numItems = m_windowSize;
   // LoggerWrapper::GetInstance()->Write(LV_WARNING, "GenericFlatListModel.requestMoreData m_windowSize = %d, numItems = %d", m_windowSize, numItems);
    int rowsToClear = m_list.size();
    if (direction >= 0) {
      //  LoggerWrapper::GetInstance()->Write(LV_WARNING, "GenericFlatListModel.requestMoreData scrolling down");
        // we are scrolling down the list, set new window start to 1/3 behind requested row
        newWindowStart = row - numItems/3;

    // if the new window end exeeds the total number of rows, adjust backwards
    if (newWindowStart + m_windowSize >= totalRows) {
        newWindowStart = totalRows - numItems;
    }
    if (newWindowStart <= 0)
      newWindowStart = 0;

    // new window and old overlaps, need to adjust request
    if (newWindowStart < m_windowStart+m_list.size()) {
        m_requestStart = m_windowStart+m_list.size();
        rowsToClear = newWindowStart - m_windowStart;
    } else {
        m_requestStart = newWindowStart;
    }

    // get the number of items to still have maxItems - need to get the same number as we clear
    numItems = rowsToClear;
    // when doing a request on an empty window, we get 0 items, so we just get a full window
    if (numItems <= 0 && m_list.size() == 0)
        numItems = m_windowSize > totalRows ? totalRows : m_windowSize;
    m_windowStart = newWindowStart;
    m_bForward = true;

    // clear data from top of list
  //  LoggerWrapper::GetInstance()->Write(LV_WARNING, "GenericFlatListModel.requestMoreData clearing data from top of list, rowsToClear = %d", rowsToClear);
    for (int i = 0; i < rowsToClear; i++) {
        m_list.removeAt(0);
    }

    } else {
    //    LoggerWrapper::GetInstance()->Write(LV_WARNING, "GenericFlatListModel.requestMoreData scrolling up");
    // we are scrolling up the list, window start 2/3 of numItems before row
        newWindowStart = row - numItems/3*2;

    if (newWindowStart < 0) {
        newWindowStart = 0;
    }

    // if new window end is inside old window, adjust rows to fetch
    if (newWindowStart + numItems > m_windowStart) {
        rowsToClear = m_windowStart - newWindowStart;
    }
    m_requestStart = newWindowStart;
    m_bForward = false;
    numItems = rowsToClear;

    // clear data from end of
   // LoggerWrapper::GetInstance()->Write(LV_WARNING, "GenericFlatListModel.requestMoreData clearing data from end of list: rowsToClear = %d", rowsToClear);
    for (int i = 0; i < rowsToClear; i++) {
        m_list.removeAt(m_list.size()-1);
    }
    }

    // request new data. Make sure that the request is valid (numItems > 0)
    if (numItems > 0) {
        m_requestEnd = m_requestStart + numItems - 1; // m_requestStart & -End are inclusive
   // LoggerWrapper::GetInstance()->Write(LV_WARNING, "GenericFlatListModel.requestMoreData start = %d, end = %d", m_requestStart, m_requestEnd);
    QObject* p = QObject::parent();
    qorbiterManager* pManager = static_cast<qorbiterManager*>(p);
    pManager->loadMoreData(modelName, m_dgName, m_PK_DataGrid, m_option, m_requestStart, numItems, m_totalCols, QString());
    } else {
  //  LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "GenericFlatListModel.requestMoreData numItems <= 0, aborting!");
    m_requestStart = -1;
    m_requestEnd = -1;
    }

}

void GenericFlatListModel::seek(QString seek)
{
    if (m_windowSize >= totalRows && m_windowStart == 0 && m_list.size() == totalRows) {
    } else {
        resetWindow();
    }
    m_seek = true;
  //  LoggerWrapper::GetInstance()->Write(LV_WARNING, "GenericFlatListModel.seek start");
    QObject* p = QObject::parent();
    qorbiterManager* pManager = static_cast<qorbiterManager*>(p);
    int numItems = 1; // hack: to get the exact row from the datagrid plugin, request only one row to get the row number. The normal loading code will then load a window of items around that row.
    if (numItems >= totalRows)
        numItems = totalRows;
    if (numItems > 0)
        pManager->loadMoreData(modelName, m_dgName, m_PK_DataGrid, m_option, 0, numItems, m_totalCols, seek);
    else
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "GenericFlatListModel.seek numItems = 0, aborting!");
}

void GenericFlatListModel::seekResult(int row)
{
    if (m_seek) {
    m_seek = false;
    // Scroll to position
    emit scrollToItem(row);
    }
}

void GenericFlatListModel::removeComplete() {
   //   LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "GenericFlatListModel.removeComplete");
}
