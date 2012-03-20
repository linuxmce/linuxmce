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

    This is a small class designed to specifically deal with the current devices in as part of the 'Resend AV codes'
    mechanism. This class hold the 'top' level devices so to speak in the tree of current availible devices for control. Note,
    this class should govern the visibility of the objects and limit them to the relevant now playing objects. Otherwise, it would
    be a large theoretical list of devices in a large home. With the availibility of this in qml, it can be displayed in many different
    fashions.
*/

#ifndef AVDEVICE_H
#define AVDEVICE_H

#include <QObject>

class AvDevice : public QObject
{
    Q_OBJECT
    Q_PROPERTY (QString qs_device READ getDeviceName WRITE setDeviceName NOTIFY deviceNameSet )
    Q_PROPERTY (QString qs_controlledBy READ getController WRITE setController NOTIFY controllerSet)
    Q_PROPERTY (int i_deviceNo READ getDeviceNo WRITE setDeviceNo NOTIFY deviceSet)
public:
    explicit AvDevice(int inc_deviceNo, QString device_name, QString qs_controller, QObject *parent = 0);
    int i_deviceNo;
    QString qs_controlledBy;
    QString qs_device;
    
signals:
    void deviceSet();
    void controllerSet();
    void deviceNameSet();
    
public slots:
    void setDeviceNo(int d);
    int getDeviceNo();

    void setController(QString controller);
    QString getController();

    void setDeviceName(QString d);
    QString getDeviceName();
    
};

#endif // AVDEVICE_H
