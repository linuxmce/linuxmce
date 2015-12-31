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

  This class is intended to be an intelligent container class for the floorplans. Initially it will hold the images for the floorplans
  and the floorplan devices and their types. I hope expand it to internalize the data needed for placing on the floorplan the items at their
  appropriate places. None of the operations here can be heavy however, as this class needs to remain lightweight and fast as its connected
  to the gui thread. It is also responsible for sending signals to the DCE thread that events have occured, like page changes, and to send the
  new floorplan and fetch relevant data.
  */


#ifndef FLOORPLANMODEL_H
#define FLOORPLANMODEL_H

#include <QAbstractListModel>
#include <datamodels/floorplanimageitem.h>
#include <contextobjects/floorplandevice.h>
#include <QModelIndex>
#include <qorbitermanager.h>

class qorbiterManager;

/*!
 * \brief The FloorPlanModel class
 * \ingroup data_model
 */
class FloorPlanModel : public QAbstractListModel
{
    Q_PROPERTY (QString currentPage READ getCurrentPage WRITE setCurrentPage NOTIFY pageChanged)
    Q_PROPERTY (QImage currentImage READ getCurrentImage WRITE setImage NOTIFY floorPlanImageChanged)
    Q_PROPERTY (int iCurrentPage READ getCurrentIntPage WRITE setCurrentIntPage NOTIFY pageChanged)
    Q_PROPERTY (bool itemSelected READ getStatus WRITE setStatus NOTIFY selectedChanged )
    Q_PROPERTY (int selectedDevice  READ getSelectedDevice WRITE setSelectedDevice NOTIFY selectedDeviceChanged )
    Q_PROPERTY(int floorplanType READ floorplanType NOTIFY floorplanTypeChanged)
    Q_PROPERTY(QVariantMap selectedDevices READ getSelectedDevices NOTIFY selectedDevicesChanged)

    Q_OBJECT
public:
    explicit FloorPlanModel(FloorplanDevice *m_prototype, qorbiterManager *r, QObject *parent = 0);
    typedef QMap <int, QString> myMap;
#ifdef QT5
    QHash<int, QByteArray> roleNames() const;
#endif
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    void appendRow(FloorplanDevice *item);
    void appendRows(const QList<FloorplanDevice*> &items);
    void insertRow(int row, FloorplanDevice* item);

    bool removeRow(int row, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    FloorplanDevice* takeRow(int row);
    FloorplanDevice* find(const QString &id) const;

    QModelIndex indexFromItem( const FloorplanDevice* item) const;
    FloorplanDevice* currentRow();

    void sortModel(int column, Qt::SortOrder order);
    QString m_installation;
    QString currentPage;
    QImage currentImage;
    int currentFloorPlanType;
    int totalPages;
    int iCurrentPage;

    bool itemSelected;
    bool multiSelect;
    int selectedDevice;

    QString imageBasePath;

    QMap<int, QString*> floorplanPages;
    QVariantMap selectedDevices;

#if (QT5)
    QQmlComponent *fpProxy;
    QQmlEngine *proxyEngine;
#else
    QDeclarativeComponent *fpProxy;
    QDeclarativeEngine *proxyEngine;
#endif

    //QModelIndex index(int row, int column, const QModelIndex &parent) const ;
    //QModelIndex parent(const QModelIndex &child) const;
    //int columnCount(const QModelIndex &parent) const;

signals:

    void pageChanged(QString s);
    void floorPlanImageChanged();
    void floorPlanStatus(QString s);
    void requestNewFloorPlanData(QString p);
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void changePage(int p);
    void addFloorplanSprite(int x, int y, int device, bool status);
    void floorplanTypeChanged();
    void selectedChanged();
    void selectedDeviceChanged();
    void selectedDevicesChanged();
    void adjustLevel(int, myMap);
    void sendCommandMessage(QVariantMap cmdMsg);
    void deviceCommandsChanged();
    void deviceParamsChanged();
    void deviceChanged(int device);
    void createFloorplanDevice(int device, int deviceX, int deviceY, int deviceType );


public slots:
    FloorplanDevice* find(int device) const;
    FloorplanDevice *get(int idx);
    Q_INVOKABLE FloorplanDevice *getDevice(int device);
    Q_INVOKABLE int count() { return m_list.size();}
    void clearAllSelections();
    void clear();
    void handleItemChange();
    void handleStatusChange(int device);
    void updateDevice(int device);
    void setDeviceSelection(int devNo);
    bool getDeviceSelection(int devNo);
    void set(int level) {/*emit adjustLevel(level, selectedDevices);*/}
    void executeDeviceCommand(int deviceId, int cmdId);

    void setStatus(bool status) { itemSelected = status; emit selectedChanged();}
    bool getStatus() {return itemSelected;}

    void setSelectedDevice(int dev) {selectedDevice = dev; emit selectedDeviceChanged();}
    int getSelectedDevice() {return selectedDevice;}

    int getCurrentIntPage() {return iCurrentPage;}
    void setCurrentIntPage(int i) {iCurrentPage = i; emit changePage( iCurrentPage);}

    void updateStatus(QString status, QString state, int device);

    QImage getFloorPlanImage () {return currentImage;}
    QImage getCurrentImage() {return currentImage;}
    QImage getPageImage(QString &id);

    void setImageData(const uchar *data, int iData_size);
    void setImage(QImage fp) {  currentImage = fp; emit floorPlanImageChanged(); emit requestNewFloorPlanData(currentPage); }

    int getDeviceX(int device);
    int getDeviceY(int device);

    QString getCurrentImagePath();
    void populateSprites();
    void finishSprite();

    Q_INVOKABLE void setCurrentPage(QString currentPageId);
    QString getCurrentPage() {return currentPage;}

    int floorplanType(){return currentFloorPlanType;}

    void setCurrentFloorPlanType(int t) { currentFloorPlanType = t; emit floorplanTypeChanged(); }
    int getCurrentFloorPlanType() {return currentFloorPlanType; }

    void setDeviceParams(QVariantList p, int device);

    QVariantMap getSelectedDevices(){return selectedDevices;}

    QVariantMap getDeviceCommands(int d);

    QVariantList getCommandParams(int device);

    Q_INVOKABLE void updateDeviceData();
    int getColor(int device) const;
    QString getText(int device) const;
    QString getDeviceStatus(int device) const;
    QVariantMap getDeviceData(int device) const ;

private:
    FloorplanDevice* m_prototype;
    QList<FloorplanDevice*> m_list;
    qorbiterManager *uiRef;

};

#endif // FLOORPLANMODEL_H
