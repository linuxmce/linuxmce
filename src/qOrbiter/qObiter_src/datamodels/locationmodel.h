#ifndef LOCATIONMODEL_H
#define LOCATIONMODEL_H

#include <QAbstractListModel>
#include <datamodels/locationitem.h>
#include <QList>
#include <QVariant>



class LocationModel : public QAbstractListModel
{
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
  QModelIndex indexFromItem( const LocationItem* item) const;
  void clear();

  QString default_Ea;

private slots:
  void handleItemChange();

private:
  LocationItem* m_prototype;
  QList<LocationItem*> m_list;

};

#endif // LOCATIONMODEL_H
