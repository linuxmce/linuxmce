#include "floorplandevice.h"
#include <QStringList>
#ifdef debug
#include <QDebug>
#endif

FloorplanDevice::FloorplanDevice(QString &name, int &deviceNo, int &floorplan_device_type, int i_flooplanType, QString &position, QImage &icon,  QObject *parent) :
    mQS_name(name), mI_deviceNo(deviceNo), mI_floorplan_device_type(floorplan_device_type), mI_floorplanType(i_flooplanType), mQS_position(position), mIM_icon(icon)

{
    setCurrentX(0);
    setCurrentY(0);
    status = false;
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
    default:
        return QVariant();
    }
}

void FloorplanDevice::setupFloorplanPositions()
{
    QStringList positions = mQS_position.split(",");
    QStringList::const_iterator constIterator;
    // positions should have minimum 2 entries, one device with X+Y co-ords
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

void FloorplanDevice::getPagePosition(int page)
{

}

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
