#include "floorplanmodel.h"
#ifdef debug
#include <QDebug>
#endif

FloorPlanModel::FloorPlanModel(FloorplanDevice* prototype, qorbiterManager *r, QObject *parent) :
    QAbstractListModel(parent), m_prototype(prototype), uiRef(r)
{
#ifndef QT5
    setRoleNames(m_prototype->roleNames());
#endif
    qRegisterMetaType<QModelIndex>("QModelIndex");
    qRegisterMetaType<QMap<int, QString*> >("QMap");
    setStatus(false);
    multiSelect = true;

   // connect(this, &FloorPlanModel::requestNewFloorPlanData, this, &FloorPlanModel::populateSprites);
}
#ifdef QT5
QHash<int, QByteArray> FloorPlanModel::roleNames() const
{
    return m_prototype->roleNames();
}
#endif

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

void FloorPlanModel::appendRow( FloorplanDevice *item)
{
    qDebug() << Q_FUNC_INFO << item->floorplanType();
    appendRows(QList<FloorplanDevice*>() << item);

}

void FloorPlanModel::appendRows(const QList<FloorplanDevice *> &items)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount()+items.size()-1);
    foreach(FloorplanDevice *item, items) {

        QObject::connect(item, SIGNAL(dataChanged()), this, SLOT(handleItemChange()));
        QObject::connect(this, SIGNAL(changePage(int)), item, SLOT(setCurrentPage(int)));
        QObject::connect(item, SIGNAL(deviceCommandsChanged()), this, SIGNAL(deviceCommandsChanged()));
        QObject::connect(item, SIGNAL(commandParamsChanged()), this, SIGNAL(deviceParamsChanged()));
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

void FloorPlanModel::setDeviceParams(QVariantList p, int device)
{
    FloorplanDevice *d = find(device);
    if(d){
        d->setParams(p);
    }
}

QVariantMap FloorPlanModel::getDeviceCommands(int d)
{
    FloorplanDevice * t = find(d);
    if(t!=0){
        return t->getDeviceCommands();
    }
}

QVariantList FloorPlanModel::getCommandParams(int device)
{
    FloorplanDevice * t = find(device);
    if(t!=0){
        return t->getParams();
    }
}

void FloorPlanModel::setDeviceSelection(int devNo)
{
    // qDebug() << "Handling status change for:" << devNo;
    for (int i =0; i < m_list.count(); i++){
        if (m_list.at(i)->deviceNumber() == devNo){
            if(m_list.at(i)->selected() == false){
                selectedDevices.insert(QString::number(devNo), m_list.at(i)->id() );
            }
            else
            {
                handleStatusChange(devNo);
            }

            m_list.at(i)->setSelected(!m_list.at(i)->selected());
        }
    }

    if(selectedDevices.count() > 0){
        setStatus(true);
    }
    else
    {
        setStatus(false);
    }
    qDebug()<<" Devices selected:" <<selectedDevices.count();
    emit selectedDevicesChanged();
    emit selectedDeviceChanged();
}

bool FloorPlanModel::getDeviceSelection(int devNo)
{
#ifdef QT_DEBUG
    // qDebug() << "Handling status request for:" << devNo;
#endif
    for (int i =0; i < m_list.count(); i++){
        if (m_list.at(i)->deviceNum() == devNo){

            return m_list.at(i)->selected();

        }
    }
}

void FloorPlanModel::executeDeviceCommand(int deviceId, int cmdId)
{
    QVariantMap outGoingCommandMsg;

    QVariantList params;
    int deviceTo;
    int deviceFrom;

    FloorplanDevice *bellWeather = find(deviceId);
    if(bellWeather){
      //  qDebug() << bellWeather->getDeviceCommands();
        QVariantMap t=bellWeather->getDeviceCommands();
      //  qDebug() << t;
    }



}

void FloorPlanModel::updateStatus(QString status, QString state, int device)
{
    FloorplanDevice *d = find(device);
    if(d && d->floorplanType() == currentFloorPlanType){
        QStringList data;

        switch (currentFloorPlanType) {
        case 2:

            data= state.split("/");

            if(data.length() > 1){

                d->setDeviceState(data.at(0));
                d->setDeviceLevel(data.at(1).toInt());
            }


            break;
        default:
            d->setDeviceState(state);
            d->setDeviceStatus(status);
            break;
        }

    }
}

void FloorPlanModel::handleItemChange()
{
    FloorplanDevice* item = static_cast<FloorplanDevice*>(sender());
    QModelIndex index = indexFromItem(item);
    if(index.isValid())
    {
        emit dataChanged(index, index);
        emit deviceChanged(item->deviceNumber());
    }
}

void FloorPlanModel::handleStatusChange(int device)
{
    selectedDevices.remove(QString::number(device));  
    //    myMap::iterator dev = selectedDevices.begin();
    //    while (dev != selectedDevices.end()) {
    //        if (dev.key() == device)
    //            dev = selectedDevices.erase(dev);
    //        else
    //            ++dev;
    //    }
    emit selectedDevicesChanged();

}

void FloorPlanModel::updateDevice(int device)
{
#ifdef QT_DEBUG
    qDebug("Updating Sprites");
#endif
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
            return item;
        }
        else
        {
            return new FloorplanDevice();
        }
    }
    return 0;
}

FloorplanDevice *FloorPlanModel::find(int device) const
{
    foreach(FloorplanDevice* item, m_list) {

        if(item->deviceNum() == device)
        {
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

void FloorPlanModel::clear()
{
    qDeleteAll(m_list);
    m_list.clear();
#ifndef QT5
    this->reset();
#endif
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

FloorplanDevice *FloorPlanModel::get(int idx)
{

    FloorplanDevice* item = m_list.at(idx);
    if(item)
        return item;
    else
        return new FloorplanDevice();


}

FloorplanDevice *FloorPlanModel::getDevice(int device)
{
    return find(device);
}

void FloorPlanModel::sortModel(int column, Qt::SortOrder order)
{
}

void FloorPlanModel::clearAllSelections()
{
    foreach(FloorplanDevice* item, m_list) {
        item->setSelected(false);
    }
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
    QObject * page = NULL;
    page = view->findChild<QObject*>("floorplan_display");

    if(page==NULL){
        qDebug() << "Could not find floorplan object";
    } else {
        qDebug () << "Found floorplan object";
    }


    foreach(FloorplanDevice* item, m_list) {

        if(item->floorplanType() == currentFloorPlanType)
        {

            if(item->getCurrentX() != -1)
            {

                emit createFloorplanDevice(
                            item->deviceNum(),
                            item->xPosition(),
                            item->yPosition(),
                            item->deviceType()
                            );
                // qDebug() << "Need to draw" << item->id();
                //                QMetaObject::invokeMethod(
                //                            page,
                //                            "placeSprites",
                //                            Q_ARG(QVariant,item->getCurrentX()),
                //                            Q_ARG(QVariant,item->getCurrentY()),
                //                            Q_ARG(QVariant,item->deviceNum()),
                //                            Q_ARG(QVariant,false ),
                //                            Q_ARG(QVariant,
                //                                  item->deviceType()),

                //                            );
            }
        }
        else
        {

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

void FloorPlanModel::updateDeviceData()
{
    foreach(FloorplanDevice* item, m_list) {
        if(item->floorplanType()==currentFloorPlanType)
            emit uiRef->getDeviceStatus(item->deviceNum());
    }
}

int FloorPlanModel::getColor(int device) const
{
    FloorplanDevice *d = find(device);
    if(d){
        return d->getColor();
    } else {
        return 0;
    }
}

QString FloorPlanModel::getText(int device) const
{
    FloorplanDevice *d = find(device);
    if(d){
        return d->deviceName();
    } else {
        return QString(tr("Invalid Device"));
    }
}

QString FloorPlanModel::getDeviceStatus(int device) const
{
    FloorplanDevice *d = find(device);
    if(d){
        return d->deviceStatus();
    } else {
        return QString(tr("Invalid Device"));
    }
}

QVariantMap FloorPlanModel::getDeviceData(int device) const
{
    FloorplanDevice *d = find(device);
    if(d){
        return d->objectData();
    } else {
        return QVariantMap();
    }
}
