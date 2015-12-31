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
#include <QVariantMap>
#include <QDebug>
/*!
 * \brief The FloorplanDevice class encapsulates a singular floorplan object.
 * It contains the normal properties and methods of a custom QML element
 *
 */

class FloorplanDevice : public QObject
{
    Q_PROPERTY (int currentFloorplanX READ getCurrentX WRITE setCurrentX NOTIFY floorplanXChanged)
    Q_PROPERTY (int currentFloorplanY READ getCurrentY WRITE setCurrentY NOTIFY floorplanYChanged)
    Q_PROPERTY(QString deviceStatus READ deviceStatus NOTIFY statusChanged)
    Q_PROPERTY(QString deviceState READ deviceState NOTIFY deviceStateChanged)
    Q_PROPERTY(QString text READ text  NOTIFY dataChanged)
    Q_PROPERTY(QString deviceName READ deviceName NOTIFY deviceNameChanged)
    Q_PROPERTY(int deviceNumber READ deviceNumber NOTIFY deviceNumberChanged)
    Q_PROPERTY(bool selected READ selected WRITE setSelected NOTIFY selectedChanged)

    // Q_PROPERTY(QVariantMap deviceCommands READ getDeviceCommands WRITE setDeviceCommand NOTIFY deviceCommandsChanged)


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
        StatusRole = Qt::UserRole+9,
        CommandRole= Qt::UserRole+10,
        ColorRole = Qt::UserRole+11,
        TextRole = Qt::UserRole+12,
        ParamRole= Qt::EditRole+11,
        SelectedRole= Qt::EditRole+12,
        RoomRole = Qt::EditRole+13
    };

public:
    explicit FloorplanDevice(QString name,
                             int deviceNo,
                             int floorplan_device_type,
                             int i_flooplanType,
                             QString position,
                             int room,
                             QObject *parent = 0);
    FloorplanDevice(QObject*parent=0) {}
    QVariant data(int role) ;
    QHash<int, QByteArray> roleNames() const;

    /*!
     * \brief id
     * \return
     */
    inline QString id() const {  return mQS_name; }                         //returns name

    /*!
     * \brief deviceNum
     * \return
     */
    inline int deviceNum() const { return mI_deviceNo; }                    //returns device no

    /*!
     * \brief deviceType
     * \return
     */
    inline int deviceType() const { return mI_floorplan_device_type; }      // returns device type

    /*!
     * \brief floorplanType
     * \return
     */
    inline int floorplanType() const { return mI_floorplanType; }

    /*!
     * \brief pagePosition
     * \return
     */
    inline QString pagePosition() const { return mQS_position; }            // returns page position raw, will be converted to return current page position
    /*!
     * \brief iconPath
     * \return
     */
    inline QString iconPath() const { return mQS_iconpath; }
    /*!
     * \brief deviceImage
     * \return
     */
    inline QImage deviceImage() const {  return mIM_icon; }
    /*!
     * \brief getCurrentX
     * \return
     */
    inline int getCurrentX () const {return currentFloorplanX; }
    /*!
     * \brief getCurrentY
     * \return
     */
    inline int getCurrentY () const {return currentFloorplanY;}


    inline int getColor() const {return m_iColor;}
    void setColor(int color) { m_iColor = color; }

    QString getText() const {return m_sText;}
    void setText(QString text) { m_sText = text; emit dataChanged(); }

    inline QString getDeviceStatus() const {return m_sDeviceStatus;}

    void setDeviceStatus(QString status) { m_sDeviceStatus = status; emit statusChanged();}


    QString text() {return m_sText;}


    /*! The Selected Status of this floorplan object item for the GUI */


    QVariantMap deviceCommands;
    QVariantList commandParams;


    //newer api format

    int deviceNumber(){return mI_deviceNo;}
    QString deviceName(){return mQS_name;}
    QString deviceStatus() {return m_sDeviceStatus;}
    QString deviceState() {return m_deviceState;}
    QString alertStatus() {return m_alertStatus;}

    int deviceLevel() {return mi_deviceLevel;}
    int xPosition(){return currentFloorplanX;}
    int yPosition(){return currentFloorplanY;}
    /*!
     * \brief selected() returns internal m_selected as property
     * \return
     */
    inline bool selected() const {return m_selected;}


    QString getRoom() const;
    void setRoom(const QString &room);

signals:
    void roomChanged();
    void statusChanged();
    void dataChanged();
    void floorplanXChanged();
    void floorplanYChanged();
    void deviceCommandsChanged();
    void commandParamsChanged();

    void deviceNumberChanged();
    void deviceNameChanged();
    void alertStatusChanged();
    void deviceStateChanged();
    void deviceLevelChanged();
    void selectedChanged();

public slots:
    QVariantMap objectData();
    void setDeviceState(QString s){if(m_deviceState==s)return; m_deviceState=s; emit deviceStateChanged(); }
    void setDeviceLevel(int i){if(mi_deviceLevel==i)return; mi_deviceLevel=i;  emit deviceLevelChanged();}
    void getPagePosition(int page);
    void setCurrentPage(int page);
    void setCurrentX(int x) {currentFloorplanX = x; emit floorplanXChanged(); }
    void setCurrentY( int y) {currentFloorplanY = y; emit floorplanYChanged();}
    void setSelected( bool isSelected) {if(isSelected==m_selected)return; m_selected=isSelected; emit selectedChanged(); }
    /*!
     * \brief setDeviceCommand
     * \param p
     * Sets an a formatted device command to the QVariantMap that will be returned to the qml caller.
     */
    void setDeviceCommand(QVariantMap p){
        deviceCommands= p;
        emit deviceCommandsChanged();
        emit dataChanged();
    }

    /*!
     * \brief getDeviceCommands
     * \return
     * The return value is a QVariantMap which should be compatible with parsing as a Javascript Object.
     */
    Q_INVOKABLE inline QVariantMap getDeviceCommands () const {return deviceCommands;}
    Q_INVOKABLE inline void setParams(QVariantList p) {commandParams = p; emit commandParamsChanged(); getCommandParameters(); emit dataChanged(); }
    Q_INVOKABLE inline QVariantList getParams() const {return commandParams;}

private slots:
    void setDeviceName(QString n){if(mQS_name==n)return; mQS_name=n; emit deviceNameChanged();}
    void setDeviceNumber(int d){if(mI_deviceNo==d)return; mI_deviceNo=d; emit deviceNumberChanged();}


private:
    void setupFloorplanPositions();
    void getCommandParameters();

    QString mQS_name;                       //device name
    int mI_deviceNo;                        //device number
    int mI_floorplan_device_type;           //floorplan device type
    int mI_floorplanType;                   //type of floorplan its associated with
    QString mQS_position;                   //postion, raw in string form
    QImage mIM_icon;                        //raw image data
    QStringList mm_currentPosition;     //map of positions on pages

    QString mQS_iconpath;                   //icon path to be used in the future with skins
    int currentFloorplanX;
    int currentFloorplanY;
    int m_iColor;   // color to use in icon
    QString m_sText;   // text to display next to icon
    QString m_sDeviceStatus;   // The status device data field
    int mi_deviceLevel;
    QString m_deviceState;
    QString m_alertStatus;
    bool m_selected;
    QString m_room;

};
Q_DECLARE_METATYPE(FloorplanDevice*)
#endif // FLOORPLANDEVICE_H
