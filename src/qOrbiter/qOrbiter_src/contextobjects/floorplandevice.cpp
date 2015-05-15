#include "floorplandevice.h"
#include <QStringList>
#ifdef debug
#include <QDebug>
#endif

FloorplanDevice::FloorplanDevice(QString name, int deviceNo, int floorplan_device_type, int i_flooplanType, QString position, QObject *parent) :
    mQS_name(name),
    mI_deviceNo(deviceNo),
    mI_floorplan_device_type(floorplan_device_type),
    mI_floorplanType(i_flooplanType),
    mQS_position(position)
{
    setCurrentX(0);
    setCurrentY(0);
    setStatus(false);
    setupFloorplanPositions();
    setText(name);
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
    return names;
}

QVariant FloorplanDevice::data(int role) const
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
        return getStatus();
    case ColorRole:
        return getColor();
    case TextRole:
        return getText();
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
    //  QString address = "http://"+manager.m_ipAddress+"/lmce-admin/qOrbiterGenerator.php?c=";

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
