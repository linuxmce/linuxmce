#include "floorplanmodel.h"
#include <QDebug>

FloorPlanModel::FloorPlanModel(FloorplanDevice* prototype, qorbiterManager *r, QObject *parent) :
    QAbstractListModel(parent), m_prototype(prototype), uiRef(r)
{
    setRoleNames(m_prototype->roleNames());
    qRegisterMetaType<QModelIndex>("QModelIndex");

}

int FloorPlanModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_list.size();
}

QVariant FloorPlanModel::data(const QModelIndex &index, int role) const
{

    if(index.row() < 0 || index.row() >= m_list.size())
        return QVariant();
    return m_list.at(index.row())->data(role);
}


void FloorPlanModel::appendRow(FloorplanDevice *item)
{
    appendRows(QList<FloorplanDevice*>() << item);
}

void FloorPlanModel::appendRows(const QList<FloorplanDevice *> &items)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount()+items.size()-1);
    foreach(FloorplanDevice *item, items) {

        QObject::connect(item, SIGNAL(dataChanged()), this, SLOT(handleItemChange()));
        QObject::connect(this, SIGNAL(changePage(int)), item, SLOT(setCurrentPage(int)));
        m_list.append(item);

    }

    endInsertRows();

    QModelIndex index = indexFromItem(m_list.last());
    QModelIndex index2 = indexFromItem(m_list.first());
    int currentRows= m_list.count() - 1;
    emit dataChanged(index2, index);

}

void FloorPlanModel::insertRow(int row, FloorplanDevice *item)
{
    beginInsertRows(QModelIndex(), row, row);
    connect(item, SIGNAL(dataChanged()), this, SLOT(handleItemChange()));
    //qDebug() << "Inserting at:" << row;
    m_list.insert(row, item);
    endInsertRows();
}

void FloorPlanModel::handleItemChange()
{
    FloorplanDevice* item = static_cast<FloorplanDevice*>(sender());
    QModelIndex index = indexFromItem(item);
    // qDebug() << "Handling item change for:" << index;
    if(index.isValid())
    {
        emit dataChanged(index, index);
    }
}

void FloorPlanModel::updateDevice(int device)
{
    qDebug("Updating Sprites");
    QObject * view = uiRef->qorbiterUIwin->rootObject();
    QList<QObject*>  currentFloorplanDevices = view->findChildren<QObject*>("floorplan_sprite");

    foreach(QObject* item, currentFloorplanDevices) {


    }
}

FloorplanDevice * FloorPlanModel::find(const QString &id) const
{
    foreach(FloorplanDevice* item, m_list) {

        if(item->id().contains(id))
        {
            //qDebug() << "Found Match of: " << item->id() << "to " << id;
            return item;
        }
        else
        {
            //  qDebug() << item->id();
        }
    }
    return 0;
}

QModelIndex FloorPlanModel::indexFromItem(const FloorplanDevice *item) const
{
    Q_ASSERT(item);
    for(int row=0; row<m_list.size(); ++row) {
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

void FloorPlanModel::clear()
{

    qDeleteAll(m_list);
    m_list.clear();
    this->reset();

}

bool FloorPlanModel::removeRow(int row, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    if(row < 0 || row >= m_list.size()) return false;
    beginRemoveRows(QModelIndex(), row, row);
    delete m_list.takeAt(row);
    endRemoveRows();
    return true;
}

bool FloorPlanModel::removeRows(int row, int count, const QModelIndex &parent)
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

FloorplanDevice * FloorPlanModel::takeRow(int row)
{
    beginRemoveRows(QModelIndex(), row, row);
    FloorplanDevice* item = m_list.takeAt(row);
    endRemoveRows();
    return item;
}

FloorplanDevice * FloorPlanModel::currentRow()
{
    FloorplanDevice* item = m_list.at(0);
    return item;
}

QImage FloorPlanModel::getPageImage(QString &id)
{
    return currentImage;
}

QString FloorPlanModel::getCurrentImagePath()
{
    return imageBasePath+currentPage+".png";
}

/*
  This function is responsible for loading the floorplan 'sprites'. This is a slightly tricky as we need to
  1) get the skin path 2) get the path to the qml file named FpSprite.qml 3) load it via network or locally transparently.
  if this is a network component, then we finish loading the component in a second function
  */
void FloorPlanModel::populateSprites()
{
    qDebug("Populating Sprites");
    QObject * view = uiRef->qorbiterUIwin->rootObject();
    QObject * page = view->findChild<QObject*>("floorplan_display");
    qDebug() << page->parent();

    foreach(FloorplanDevice* item, m_list) {

        if(item->floorplanType() == currentFloorPlanType)
        {
            if(item->getCurrentX() != -1)
            {
                qDebug() << "Need to draw" << item->id();
                QMetaObject::invokeMethod(page, "placeSprites", Q_ARG(QVariant,item->getCurrentX()), Q_ARG(QVariant,item->getCurrentY()), Q_ARG(QVariant,item->deviceNum()), Q_ARG(QVariant,false ), Q_ARG(QVariant, item->deviceType()));
            }
        }
        else
        {
            //  qDebug() << item->id();
        }
    }
}

void FloorPlanModel::finishSprite()
{

}



void FloorPlanModel::setCurrentPage(QString currentPageId)
{

    currentPage = currentPageId;
    setCurrentIntPage( currentPage.toInt());
    QString s = getCurrentImagePath();
    emit pageChanged(s);
}

void FloorPlanModel::setImageData(const uchar *data, int iData_size)
{
    QImage t;
    if( t.loadFromData(data, iData_size))
    {
        setImage(t);
        emit floorPlanStatus("Converted Image");
    }
    else
    {
        emit floorPlanStatus("Update Object Image Conversion Failed:");
    }
}

int FloorPlanModel::getDeviceX(int device)
{
    foreach(FloorplanDevice* item, m_list) {

        if(item->deviceNum() == device)
        {
            //qDebug() << "Found Match of: " << item->id() << "to " << id;
            return item->getCurrentX();
        }
        else
        {
            //  qDebug() << item->id();
        }
    }
    return 0;
}

int FloorPlanModel::getDeviceY(int device)
{
    foreach(FloorplanDevice* item, m_list) {

        if(item->deviceNum() == device)
        {
            //qDebug() << "Found Match of: " << item->id() << "to " << id;
            return item->getCurrentY();
        }
        else
        {
            //  qDebug() << item->id();
        }
    }
    return 0;
}

/*
QModelIndex FloorPlanModel::index(int row, int column, const QModelIndex &parent) const
{
}

QModelIndex FloorPlanModel::parent(const QModelIndex &child) const
{
}

int FloorPlanModel::columnCount(const QModelIndex &parent) const
{

}
*/
