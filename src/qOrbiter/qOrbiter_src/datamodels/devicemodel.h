#ifndef DEVICEMODEL_H
#define DEVICEMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QVariant>
#include <datamodels/avdevice.h>
/*!
 * \brief The DeviceModel class. This class is the model container class for AvDevice class objects representing
 * hardware or software devices in the a linuxmce system.
 * \rel AvDevice
 */
class DeviceModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int activeDevices READ getActiveDevice WRITE setActiveDevices NOTIFY activeDevicesChanged)
    Q_PROPERTY(int totalDevices READ getDeviceCount WRITE setDeviceCount NOTIFY deviceCountChanged )
    Q_PROPERTY (double progress READ getProgress WRITE setProgress NOTIFY progressChanged )
    Q_PROPERTY(bool loadingStatus READ getLoadingStatus WRITE setLoadingStatus NOTIFY loadingStateChanged )

public:
    explicit DeviceModel(AvDevice *prototype, QObject *parent = 0);
    ~DeviceModel();
    int totalDevices;
    int activeDevices;
    double progress;
    bool loadingStatus;
    bool resetting;


    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;


 QHash<int, QByteArray> roleNames() const;


 /*QAbstractListModel functions*/

    void appendRows(const QList<AvDevice*> &items);
  Q_INVOKABLE  void appendRow(AvDevice* item);
   // bool insertRows(int row, int count, AvDevice *item);
    void insertRow(int row, AvDevice* item);
    bool removeRow(int row, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    AvDevice* takeRow(int row);
    AvDevice* find(const QString &id) const;
    Q_INVOKABLE QVariant get(int index, const QString &name) const;
    QModelIndex indexFromItem( const AvDevice* item) const;
    AvDevice* currentRow();
    void sortModel(int column, Qt::SortOrder order);
    AvDevice* getItem(int itmIndex);

    /*Added functions */

    void setProgress(double iProgress) {progress = iProgress; emit progressChanged(); }
    double getProgress() {return progress;}

    void setLoadingStatus(bool s) {loadingStatus = s; emit loadingStateChanged();}
    bool getLoadingStatus() {return loadingStatus;}

    void setDeviceCount(int c) {totalDevices= c; emit deviceCountChanged();}
    int getDeviceCount() {return totalDevices;}

    void setActiveDevices(int a) {activeDevices = a; emit activeDevicesChanged();}
    int getActiveDevice() {return activeDevices;}

signals:
    void loadingStateChanged();
    void progressChanged();
    void deviceCountChanged();
    void activeDevicesChanged();

     void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const int &sRow);
     void modelAboutToBeReset();
     void modelReset();
    
public slots:
    bool clear();

private slots:
    void handleItemChange();
    void reset();

private:
    bool resetInternalData();
    AvDevice* m_prototype;
    QList<AvDevice*> m_list;
    
};

#endif // DEVICEMODEL_H
