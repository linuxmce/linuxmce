#ifndef FLOORPLANDEVICE_H
#define FLOORPLANDEVICE_H

/*
    This file is part of QOrbiter for use with the LinuxMCE project found at http://www.linuxmce.org
   Langston Ball  golgoj4@gmail.com
    QOrbiter is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QOrbiter is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QOrbiter.  If not, see <http://www.gnu.org/licenses/>.

  the purpose of this class is to enable a representation of floorplan devices for qml
  it will allow an abstraction of all aspects of the device and allow for flexibility
  in both the display and use of floorplan devices.
  */

#include <QObject>
#include <QVariant>
#include <QImage>
#include <QMap>
#include <QStringList>

class FloorplanDevice : public QObject
{
    Q_PROPERTY (int currentFloorplanX READ getCurrentX WRITE setCurrentX NOTIFY floorplanXChanged)
    Q_PROPERTY (int currentFloorplanY READ getCurrentY WRITE setCurrentY NOTIFY floorplanYChanged)

    Q_OBJECT

    enum Roles {
        NameRole = Qt::UserRole+1,
        DeviceNoRole =Qt::UserRole+2,
        DeviceTypeRole= Qt::UserRole+3,
        PositionRole = Qt::UserRole+4,
        IconImageRole = Qt::UserRole+5,
        FloorPlanTypeRole = Qt::UserRole+6,
        XRole = Qt::UserRole+7,
        YRole = Qt::UserRole+8,
        StatusRole = Qt::UserRole+9
    };

public:
    explicit FloorplanDevice(QString &name, int &deviceNo, int &floorplan_device_type, int i_flooplanType, QString &position, QImage &icon, QObject *parent = 0);
    FloorplanDevice(QObject * parent=0) {}
    QVariant data(int role) const;
    QHash<int, QByteArray> roleNames() const;

    inline QString id() const {  return mQS_name; }                         //returns name
    inline int deviceNum() const { return mI_deviceNo; }                    //returns device no
    inline int deviceType() const { return mI_floorplan_device_type; }      // returns device type
    inline int floorplanType() const { return mI_floorplanType; }
    inline QString pagePosition() const { return mQS_position; }            // returns page position raw, will be converted to return current page position
    inline QString iconPath() const { return mQS_iconpath; }
    inline QImage deviceImage() const {  return mIM_icon; }
    inline int getCurrentX () const {return currentFloorplanX; }
    inline int getCurrentY () const {return currentFloorplanY;}
    inline bool getStatus() const {return status;}

    QString mQS_name;                       //device name
    int mI_deviceNo;                        //device number
    int mI_floorplan_device_type;           //floorplan device type
    int mI_floorplanType;                   //type of floorplan its associated with
    QString mQS_position;                   //postion, raw in string form
    QImage mIM_icon;                        //raw image data
    QStringList mm_currentPosition;     //map of positions on pages
    QMap <QString, QString> mm_commands;    //command map with command and corresponding ??
    QString mQS_iconpath;                   //icon path to be used in the future with skins
    int currentFloorplanX;
    int currentFloorplanY;
    bool status;


signals:
    void statusChanged();
    void dataChanged();
    void floorplanXChanged();
    void floorplanYChanged();

public slots:
    void getPagePosition(int page);
    void setCurrentPage(int page);

    void setCurrentX(int x) {currentFloorplanX = x; emit floorplanXChanged(); }
    void setCurrentY( int y) {currentFloorplanY = y; emit floorplanYChanged();}
    void setStatus( bool state) {status = state; }


private:
    void setupFloorplanPositions();

};

#endif // FLOORPLANDEVICE_H
