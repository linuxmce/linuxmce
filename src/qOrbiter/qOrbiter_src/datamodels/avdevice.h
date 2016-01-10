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
#include <QHash>
#include <QVariant>
/*!
 * \brief The AvDevice class is the item class that makes up the objects inside a DeviceModel.
 * \rel DeviceModel
 */
class AvDevice : public QObject
{
    Q_OBJECT
    enum Roles {
        NameRole = Qt::UserRole+1,
        ControllerRole =Qt::UserRole+2,
        DeviceNumberRole= Qt::DisplayRole+3,
        ActiveRole = Qt::DisplayRole+4,
        LocationRole = Qt::UserRole+5,
        TypeRole = Qt::UserRole+6
    };
public:
    AvDevice() {}
    explicit AvDevice(int inc_deviceNo, QString device_name, QString qs_controller, QString loc, int type, bool state, QObject *parent = 0);
    QVariant data(int role) const;
    QHash<int, QByteArray> roleNames() const;

    inline QString id() const {  return qs_device; }
    inline QString controlled() const { return qs_controlledBy; }
    inline QString located() const { return location; }
    inline int deviceNumber() const {return i_deviceNo;}
    inline bool activity() const {return active;}
    inline int device_Type() const {return deviceType;}
    
signals: 
    void dataChanged();
    
private:
    int i_deviceNo;
    QString qs_controlledBy;
    QString qs_device;
    QString location;
    bool active;
    int deviceType;
    
};

#endif // AVDEVICE_H
