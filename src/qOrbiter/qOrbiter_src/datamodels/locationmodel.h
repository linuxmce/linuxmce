#ifndef LOCATIONMODEL_H
#define LOCATIONMODEL_H

#include <QAbstractListModel>
#include <datamodels/locationitem.h>
#include <QList>
#include <QVariant>
#include "../contextobjects/entertainareatimecode.h"
/*!
 * \brief The LocationModel class.
 * QAbstractListmodel based class for storing the various items relating to locations in the home and the current users location.
 * \ingroup room_list
 */
class LocationModel : public QAbstractListModel
{

    Q_PROPERTY(QString currentRoom READ getStringRoom NOTIFY roomChanged)/*!< The current location, string format */
    Q_PROPERTY(QString currentEA READ getStringEa NOTIFY eaChanged)/*!< The current entertain area, string format */
    Q_PROPERTY(int currentItem READ getCurrentItem WRITE setCurrentItem NOTIFY locationChanged)
    Q_PROPERTY(EntertainAreaTimeCode* currentEaTimecode READ currentEaTimecode  NOTIFY currentEaTimecodeChanged)

    Q_OBJECT

public:
    explicit LocationModel(LocationItem* prototype, QObject* parent = 0);
    ~LocationModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    void appendRow(LocationItem* item);
    void appendRows(const QList<LocationItem*> &items);
    void insertRow(int row, LocationItem* item);
    bool removeRow(int row, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    LocationItem* takeRow(int row);
    LocationItem* find(const QString &id) const;
    void sortModel(int column, Qt::SortOrder order);
    bool check(int room);

#ifdef QT5
 QHash<int, QByteArray> roleNames() const;
#endif


    QModelIndex indexFromItem( const LocationItem* item) const;
    void clear();

    int rdefault_Ea;
    int idefault_Ea;
    QString sdefault_Ea;

    QString currentRoom;
    QString currentEA;   
    QString currentUser;

    int i_currentUser;
    int i_currentEA;
    int i_currentRoom;

    int currentItem;

    EntertainAreaTimeCode *currentEaTimecode() const;
    void setCurrentEaTimecode(EntertainAreaTimeCode *currentEaTimecode);

public slots:
    void setEaTimeCode(QString ea, QMap<long, std::string> data);
    void setEaTimeCode(int room, QMap<long, std::string> data);
    bool addTimeCodeTrack(QString ea, int intEa, int room);
    Q_INVOKABLE QList<EntertainAreaTimeCode *> allTimeCode();

    void setCurrentItem(int i) {currentItem=i;emit locationChanged();}
    int getCurrentItem() {return currentItem;}

    void setLocation(int ea, int room);

    void setCurrentEA(QString ea) { currentEA = ea;  emit eaChanged(); }
    QString getStringEa() {return currentEA;}
    int getIntEa() {return i_currentEA;}

    void setCurrentRoom (QString room) {currentRoom = room; emit roomChanged();}
    QString getStringRoom () { return currentRoom;}
    int getIntRoom () { return i_currentRoom;}

    void setCurrentUser (int user) {currentUser = QString::number(user); emit userChanged(); }
    QString getStringUser () {return currentUser;}
    int getIntUser() {return i_currentUser;}

    Q_INVOKABLE QString getRoomName(int r);

signals:
    void eaChanged();
    void roomChanged();
    void userChanged();
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void currentEaTimecodeChanged();

    void locationChanged();
    void seekToTime(QString t);

private slots:
    void handleItemChange();

private:
    LocationItem* m_prototype;
    QList<LocationItem*> m_list;

    EntertainAreaTimeCode *m_currentEaTimecode;
    QVector <EntertainAreaTimeCode*> eaTimecodeItems;

};

#endif // LOCATIONMODEL_H
