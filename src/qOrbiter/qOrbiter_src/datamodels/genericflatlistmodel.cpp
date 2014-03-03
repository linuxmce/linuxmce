#include "genericflatlistmodel.h"
#include "DCE/Logger.h"
#include "../qorbitermanager.h"

using namespace DCE;

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

GenericFlatListModel::GenericFlatListModel(QObject *parent) :
  QAbstractListModel(parent), m_prototype(NULL)
{
    qRegisterMetaType<QModelIndex>("QModelIndex");
    m_windowSize = 100;
    resetWindowVariables();
}
GenericFlatListModel::GenericFlatListModel(GenericModelItem *prototypeItem, QObject *parent) :
    QAbstractListModel(parent), m_prototype(prototypeItem)
{
#ifndef QT5
    setRoleNames(m_prototype->roleNames());
#endif
    qRegisterMetaType<QModelIndex>("QModelIndex");
    qRegisterMetaType<QMap<int, QString*> >("QMap");
    m_windowSize = 100;
    resetWindowVariables();
    seperator = 16;
    loadingStatus = false;
    progress = 0;
    clearing = false;
    clear();
}

void GenericFlatListModel::resetWindowVariables() {
    m_windowStart = 0;
    totalRows = 0;
    m_totalCols = 0;
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
    Q_UNUSED(parent);
    return totalRows;
}


// not const per se, but from the outside this object contains all data at any time, so lets roll with that
QVariant GenericFlatListModel::data(const QModelIndex &index, int role) const
{
    LoggerWrapper::GetInstance()->Write(LV_WARNING, "GenericFlatListModel.data row = %d", index.row());
    // index outside the range of the datagrid ?
    if(index.row() < 0 || index.row() >= totalRows)
        return QVariant();
    
    LoggerWrapper::GetInstance()->Write(LV_WARNING, "GenericFlatListModel.data between 0 and totalRows");
    LoggerWrapper::GetInstance()->Write(LV_WARNING, "GenericFlatListModel.data windowStart = %d, windowEnd = %d", m_windowStart, m_windowStart+m_list.size());
    // index outside of the loaded items ?
    if (index.row() < m_windowStart || index.row() >= m_windowStart+m_list.size()) {
        // This method must be const to satisfy the superclass, but even though we are actually changing the state of the
        // object here, the change is only internal, externally we always fake it and say we have the total number of rows
        const_cast<GenericFlatListModel*>(this)->requestMoreData(index.row(), index.row()-m_iLastRow);
	const_cast<GenericFlatListModel*>(this)->m_iLastRow = index.row();
        return QString("Loading...");
    }
    LoggerWrapper::GetInstance()->Write(LV_WARNING, "GenericFlatListModel.data between m_windowStart and indexEnd");
    return m_list.at(index.row()-m_windowStart)->data(role);
}

bool GenericFlatListModel::setData(const int index, const QString roleName, const QVariant & value)
{
    LoggerWrapper::GetInstance()->Write(LV_DEBUG, "GenericFlatListModel.setData");
    
    if(index < 0 || index > totalRows)
        return false;

    // index outside of the loaded items ?
    if (index < m_windowStart || index > m_windowStart+m_list.size())
        return false;

    int role = m_prototype->roleNames().key(roleName.toUtf8());
    if (role > 0)
        return m_list.at(index-m_windowStart)->setData(role, value);
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
    LoggerWrapper::GetInstance()->Write(LV_DEBUG, "GenericFlatListModel.appendRows start");
    beginInsertRows(QModelIndex(), rowCount(), rowCount()+items.size()-1);
    foreach(GenericModelItem *item, items) {
        item->setParent(this);
        QObject::connect(item, SIGNAL(dataChanged()), this , SLOT(handleItemChange()));
        m_list.append(item);
    }
    endInsertRows();
    
    QModelIndex index = indexFromItem(m_list.last());
    QModelIndex index2 = indexFromItem(m_list.first());
    int currentRows= m_list.count();
    //    emit itemAdded(currentRows);
    // setCurrentCells(currentRows);
    // double p = (((double)m_list.size() / (double)seperator) * 100) ;
    //setProgress(p);
    emit dataChanged(index2, index);
    /* setLoadingStatus(false);
    QApplication::processEvents(QEventLoop::AllEvents);*/

    LoggerWrapper::GetInstance()->Write(LV_DEBUG, "GenericFlatListModel.appendRows end");
}

// Not to be confused with the QAbstractItemModel::insertRow
void GenericFlatListModel::insertRow(int row, GenericModelItem *item)
{
    LoggerWrapper::GetInstance()->Write(LV_DEBUG, "GenericFlatListModel.insertRow row = %d", row);
    // Is received row within the expected request range
    if (m_requestStart >= 0 && row >= m_requestStart && row < m_requestEnd) {
      bool okToInsert = false;
      if (m_bForward) {
	if (row == m_requestStart) {
	  LoggerWrapper::GetInstance()->Write(LV_DEBUG, "GenericFlatListModel.insertRow forward request ok");
	  m_requestStart++;
	  okToInsert = true;
	}
      } else {
	if (row == m_requestEnd-1) {
	  LoggerWrapper::GetInstance()->Write(LV_DEBUG, "GenericFlatListModel.insertRow backward request ok");
	  m_requestEnd--;
	  okToInsert = true;
	}
      }	
      if (okToInsert) {
	item->setParent(this);
	connect(item, SIGNAL(dataChanged()), this, SLOT(handleItemChange()));
	m_list.insert(row-m_windowStart, item);
	if (!m_bForward)
	    m_windowStart--;
	emit dataChanged(index(row), index(row));
      }

      if (m_requestStart == m_requestEnd) {
	// we are done
	m_requestStart = -1;
	m_requestEnd = -1;
      }
    }
    LoggerWrapper::GetInstance()->Write(LV_DEBUG, "GenericFlatListModel.insertRow end");
}

void GenericFlatListModel::handleItemChange()
{
    GenericModelItem* item = static_cast<GenericModelItem*>(sender());
    QModelIndex index = indexFromItem(item);
    qDebug() << "Handling item change for:" << index;
    if(index.isValid())
    {
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "GenericFlatListModel.handleItemChange emit dataChanged");
        emit dataChanged(index, index);
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
    if (index>=0 && index<totalRows) {
      if (index < m_windowStart || index > m_windowStart+m_list.size()) {
        // This method must be const to satisfy the superclass, but even though we are actually changing the state of the
        // object here, the change is only internal, externally we always fake it and say we have the total number of rows
	const_cast<GenericFlatListModel*>(this)->requestMoreData(index, index-m_iLastRow);
	const_cast<GenericFlatListModel*>(this)->m_iLastRow = index;
      } else {
          GenericModelItem* myItem = m_list.at(index-m_windowStart);

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
}

QModelIndex GenericFlatListModel::indexFromItem(const GenericModelItem *item) const
{
    Q_ASSERT(item);
    for(int row=0; row<m_list.size(); ++row) {

        if(m_list.at(row) == item) return index(row);

    }

    return QModelIndex();
}

void GenericFlatListModel::updateItemData(int row, int role, QVariant value)
{
    LoggerWrapper::GetInstance()->Write(LV_DEBUG, "GenericFlatListModel.updateItemData start");
    // TODO: should make sure model data is identical to when update request was sent
    if (row < m_windowStart || row > m_windowStart+m_list.size()) {
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "GenericFlatListModel.updateItemData row outside loaded window");
	return;
    }      
    GenericModelItem* pItem = m_list.at(row-m_windowStart);
    pItem->updateData(role, value);
    QModelIndex index = indexFromItem(pItem);
    emit dataChanged(index, index);
    LoggerWrapper::GetInstance()->Write(LV_DEBUG, "GenericFlatListModel.updateItemData end");
}

void GenericFlatListModel::clear()
{
    LoggerWrapper::GetInstance()->Write(LV_WARNING, "GenericFlatListModel.clear start");
    clearing = true;
    QModelIndex index;
    beginRemoveRows(index, m_windowStart, m_windowStart+m_list.size());
    m_list.clear();
    LoggerWrapper::GetInstance()->Write(LV_DEBUG, "GenericFlatListModel.clear m_list cleared");

    resetWindowVariables();
    endRemoveRows();
    clearing = false;
    LoggerWrapper::GetInstance()->Write(LV_WARNING, "GenericFlatListModel.clear end");

}

bool GenericFlatListModel::removeRow(int row, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    if(row < m_windowStart || row >= m_windowStart+m_list.size()) return false;
    beginRemoveRows(QModelIndex(), row, row);
    delete m_list.takeAt(row);
    endRemoveRows();
    return true;
}

bool GenericFlatListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    if(row < m_windowStart || (row+count) > (m_windowStart+m_list.size())) return false;
    beginRemoveRows(QModelIndex(), row, row+count-1);
    for(int i=0; i<count; ++i) {
        delete m_list.takeAt(row);
    }
    endRemoveRows();
    return true;
}

GenericModelItem* GenericFlatListModel::takeRow(int row)
{
    if(row < m_windowStart || row > (m_windowStart+m_list.size())) return false;
    beginRemoveRows(QModelIndex(), row, row);
    GenericModelItem* item = m_list.takeAt(row-m_windowStart);
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

void GenericFlatListModel::setTotalRows(int rows)
{
    LoggerWrapper::GetInstance()->Write(LV_WARNING, "GenericFlatListModel.setTotalRows rows = %d", rows);
    // ok, so the fakery starts here - once we got the number of rows, immediately say that we got them all..
    beginInsertRows(QModelIndex(), 0, rows);
    totalRows = rows;

    // TODO ?? what are these for? standard list api?
    //    emit statusMessage("Size Changed" + QString::number(totalRows));
    //    emit sizeChanged(rows);
    endInsertRows();
    LoggerWrapper::GetInstance()->Write(LV_WARNING, "GenericFlatListModel.setTotalRows end");
}

int GenericFlatListModel::getTotalRows()
{
    return totalRows;
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

void GenericFlatListModel::refreshData() {
    QObject* p = QObject::parent();
    qorbiterManager* pManager = static_cast<qorbiterManager*>(p);
    // TODO: pass along indexStart
    pManager->refreshDataGrid(modelName, m_PK_DataGrid, m_option);
}

void GenericFlatListModel::requestMoreData(int row, int direction) {
    LoggerWrapper::GetInstance()->Write(LV_WARNING, "GenericFlatListModel.requestMoreData row = %d", row);
    if (row >= m_requestStart && row < m_requestEnd) {
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "GenericFlatListModel.requestMoreData row already requested");
	return;
    }
    // TODO: need to examine threading/signals/slots to determine if we need to use a mutex
    // TODO: handle multiple requests, if a request for a new row outside the current request get here, we might get into trouble
    // the easiest way around this might be to add a request counter, and only accept rows inserted from the last request
    int newWindowStart;
    int numItems = m_windowSize;
    LoggerWrapper::GetInstance()->Write(LV_WARNING, "GenericFlatListModel.requestMoreData m_windowSize = %d, numItems = %d", m_windowSize, numItems);
    int rowsToClear = m_list.size();
    if (direction >= 0) {
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "GenericFlatListModel.requestMoreData scrolling down");
        // we are scrolling down the list, set new window start to 1/3 behind requested row
        newWindowStart = row - numItems/3;

	// if the new window end exeeds the total number of rows, adjust backwards
	if (newWindowStart + m_windowSize > totalRows) {
	    newWindowStart = totalRows - numItems;
	}

	// new window and old overlaps, need to adjust request
	if (newWindowStart < m_windowStart+m_list.size()) {
	    m_requestStart = m_windowStart+m_list.size();
	    rowsToClear = newWindowStart - m_windowStart;
	} else {
	    m_requestStart = newWindowStart;
	}

	// get the number of items to still have maxItems - need to get the same number as we clear
	numItems = rowsToClear;
	m_windowStart = newWindowStart;
	m_bForward = true;

	// clear data from top of list
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "GenericFlatListModel.requestMoreData clearing data from top of list, rowsToClear = %d", rowsToClear);
	for (int i = 0; i < rowsToClear; i++) {
	    m_list.removeAt(0);
	}

    } else {
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "GenericFlatListModel.requestMoreData scrolling up");
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
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "GenericFlatListModel.requestMoreData clearing data from end of list: rowsToClear = %d", rowsToClear);
	for (int i = 0; i < rowsToClear; i++) {
	    m_list.removeAt(m_list.size()-1);
	}
    }

    // request new data. Make sure that the request is valid (numItems > 0)
    if (numItems > 0) {
        m_requestEnd = m_requestStart + numItems;
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "GenericFlatListModel.requestMoreData start = %d, end = %d", m_requestStart, m_requestEnd);
	QObject* p = QObject::parent();
	qorbiterManager* pManager = static_cast<qorbiterManager*>(p);
	pManager->loadMoreData(modelName, m_dgName, m_PK_DataGrid, m_option, m_requestStart, numItems, m_totalCols, QString());
    } else {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "GenericFlatListModel.requestMoreData numItems <= 0, aborting!");
	m_requestStart = -1;
	m_requestEnd = -1;
    }

}
