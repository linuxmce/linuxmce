#ifndef LOCATIONMODEL_H
#define LOCATIONMODEL_H

#include <QAbstractListModel>
#include <datamodels/locationitem.h>
#include <QList>
#include <QVariant>



class LocationModel : public QAbstractListModel
{
    Q_PROPERTY(QString currentRoom READ getStringRoom NOTIFY roomChanged)
    Q_PROPERTY(QString currentEA READ getStringEa NOTIFY eaChanged)
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

public slots:
    void setLocation(int ea, int room);

    void setCurrentEA(int ea) { currentEA = QString::number(ea); i_currentEA= ea; emit eaChanged(); }
    QString getStringEa() {return currentEA;}
    int getIntEa() {return i_currentEA;}

    void setCurrentRoom (int room) {currentRoom = QString::number(room); i_currentRoom = room; emit roomChanged();}
    QString getStringRoom () { return currentRoom;}
    int getIntRoom () { return i_currentRoom;}

    void setCurrentUser (int user) {currentUser = QString::number(user); emit userChanged(); }
    QString getStringUser () {return currentUser;}
    int getIntUser() {return i_currentUser;}


signals:
    void eaChanged();
    void roomChanged();
    void userChanged();

private slots:
    void handleItemChange();

private:
    LocationItem* m_prototype;
    QList<LocationItem*> m_list;

};

#endif // LOCATIONMODEL_H
