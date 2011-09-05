#ifndef ORBITERMODEL_H
#define ORBITERMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QVariant>
#include <datamodels/orbiterroommodel.h>


/*
  this class is on of the destination of the qOrbitergen data. its purpose its is to model the data required to
  operate an orbiter. Instead of pre-rendered objects, this should allow a more dynamic user interface as
  we can present differnt views of this data as well as have it update our ui in real time.
  */

class OrbiterListItem: public QAbstractListModel{
  Q_OBJECT

public:
  OrbiterListItem(QObject* parent = 0);
  virtual ~OrbiterListItem() {}
  virtual QString id() const = 0;
  virtual QVariant data(int role) const = 0;
  virtual QHash<int, QByteArray> roleNames() const = 0;

signals:
  void dataChanged();
};

class OrbiterModel : public QAbstractListModel
{
    Q_OBJECT
public:
  explicit OrbiterModel(OrbiterRoomModel* prototype, QObject* parent = 0);
  ~OrbiterModel();
  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  void appendRow(OrbiterRoomModel* item);
  void appendRows(const QList<OrbiterRoomModel*> &items);
  void insertRow(int row, OrbiterRoomModel* item);
  bool removeRow(int row, const QModelIndex &parent = QModelIndex());
  bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
  OrbiterRoomModel* takeRow(int row);
  OrbiterRoomModel* find(const QString &id) const;
  QModelIndex indexFromItem( const OrbiterRoomModel* item) const;
  QMap<int,QString> installation_users;
  QMap<int,QString> installation_rooms;
  QMap<int, int> intallation_eas_rooms;
  void clear();

private slots:
  void handleItemChange();

private:
  OrbiterRoomModel* m_prototype;
  QList<OrbiterRoomModel*> m_list;



};

#endif // ORBITERMODEL_H
