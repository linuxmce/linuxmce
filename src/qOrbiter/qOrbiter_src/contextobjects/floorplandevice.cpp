#include "floorplandevice.h"
#include <QStringList>
#include "QtGui/qguiapplication.h"
#ifdef debug
#include <QDebug>
#endif

FloorplanDevice::FloorplanDevice(QString name, int deviceNo, int floorplan_device_type, int i_flooplanType, QString position, int room, QObject *parent) :
    mQS_name(name),
    mI_deviceNo(deviceNo),
    mI_floorplan_device_type(floorplan_device_type),
    mI_floorplanType(i_flooplanType),
    mQS_position(position),
  m_room(QString::number(room))
{
    QObject::connect(this, &FloorplanDevice::selectedChanged, this, &FloorplanDevice::dataChanged);
    QObject::connect(this, SIGNAL(deviceStateChanged()), this, SIGNAL(dataChanged()));
    QObject::connect(this, SIGNAL(deviceLevelChanged()), this, SIGNAL(dataChanged()));
    QObject::connect(this, SIGNAL(statusChanged()), this, SIGNAL(dataChanged()));
    QObject::connect(this, SIGNAL(alertStatusChanged()), this, SIGNAL(dataChanged()));
    qRegisterMetaType<FloorplanDevice*>("FloorplanDevice*");
    setCurrentX(0);
    setCurrentY(0);
    setSelected(false);
    setText(name);
    setSelected(false);
    setDeviceStatus("UNKNOWN");
    setDeviceState("UNKNOWN");
    setupFloorplanPositions();
}

QHash<int, QByteArray> FloorplanDevice::roleNames() const
{

    QHash<int, QByteArray> names;
    names[NameRole] = "name";
    names[DeviceNoRole] = "deviceno";
    names[FloorPlanTypeRole] = "floorplantype";
    names[DeviceTypeRole] = "type";
    names[PositionRole] = "position";
    names[IconImageRole]= "iconimage";
    names[XRole]= "x";
    names[YRole]= "y";
    names[CommandRole]="commandlist";
    names[ColorRole]="color";
    names[TextRole]="text";
    names[ParamRole]="paramlist";
    names[SelectedRole]="selected";
    names[RoomRole]="room";
    return names;
}

QVariantMap FloorplanDevice::objectData()
{
    QVariantMap ret;
    ret.insert("deviceNumber", deviceNumber());
    ret.insert("deviceName", deviceName());
    ret.insert("deviceStatus", deviceStatus());
    ret.insert("deviceState", deviceState());
    ret.insert("deviceLevel", deviceLevel());
    ret.insert("alertStatus", alertStatus());
    ret.insert("selected", selected());
    ret.insert("xPos", xPosition());
    ret.insert("yPos", yPosition());
    ret.insert("commands", getDeviceCommands());
qDebug() << ret;
    return ret;

}

QVariant FloorplanDevice::data(int role)
{

    switch(role) {
    case NameRole:
        return id();
    case DeviceNoRole:
        return deviceNum();
    case DeviceTypeRole:
        return deviceType();
    case PositionRole:
        return pagePosition();
    case IconImageRole:
        return deviceImage();
    case FloorPlanTypeRole:
        return floorplanType();
    case XRole:
        return getCurrentX();
    case YRole:
        return getCurrentY();
    case CommandRole:
        return getDeviceCommands();
    case ParamRole:
        return getParams();
    case SelectedRole:
        return deviceStatus();
    case ColorRole:
        return getColor();
    case TextRole:
        return getText();
    case RoomRole:
        return getRoom();
    default:
        return QVariant();
    }
}

/*!
 * \brief FloorplanDevice::setupFloorplanPositions sets initial coordinates for devices across a floorplan.
 */
void FloorplanDevice::setupFloorplanPositions()
{
    QStringList positions = mQS_position.split(",");

    /*!positions should have minimum 2 entries, one device with X+Y co-ords*/

    if (positions.count() >= 2)
    {
        for (int i = 0; i < positions.count(); ++i)
        {
            // qDebug() << mQS_name << " Floorplan #" << positions.at(i);
            // qDebug() << "Coordinates" << positions.at(i+1) << "," << positions.at(i+2);
            mm_currentPosition.append(positions.at(i+1)+","+ positions.at(i+2));

            if(positions.count() <= i+2)
            { break; }
            else
            {
                i=i+2;
            }
        }
    }
}

void FloorplanDevice::getCommandParameters()
{
    //  QString address = "http://"+manager.currentRouter+"/lmce-admin/qOrbiterGenerator.php?c=";

}
QString FloorplanDevice::getRoom() const
{
    return m_room;
}

void FloorplanDevice::setRoom(const QString &room)
{
    if(m_room==room)return;
    m_room = room;
    emit roomChanged();
}

/*!
 * \brief FloorplanDevice::getPagePosition
 * \param page
 * \warn Depreciated
 */
void FloorplanDevice::getPagePosition(int page)
{

}

/*!
 * \brief FloorplanDevice::setCurrentPage
 * \param page The desired page from the array of uploaded floorplan images.
 */
void FloorplanDevice::setCurrentPage(int page)
{
    //qDebug() <<page;
    //qDebug() << mm_currentPosition.count();

    if(mm_currentPosition.count() > page-1)
    {
        QStringList splitter = mm_currentPosition.at(page-1).split(",");
        setCurrentX(splitter.at(0).toInt());
        setCurrentY(splitter.at(1).toInt());
        emit dataChanged();
    }
}
