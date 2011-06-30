#ifndef ORBITERROOMMODEL_H
#define ORBITERROOMMODEL_H

#include <datamodels/listModel.h>
#include <datamodels/orbiterrowmodel.h>
#include <QList>
#include <QVariant>


class OrbiterRoomModel : public ListModel{

    Q_OBJECT

public:
  explicit OrbiterRoomModel(OrbiterRowModel* prototype, QObject* parent = 0);
  ~OrbiterRoomModel();
  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  void appendRow(OrbiterRowModel* item);
  void appendRows(const QList<OrbiterRowModel*> &items);
  void insertRow(int row, OrbiterRowModel* item);
  bool removeRow(int row, const QModelIndex &parent = QModelIndex());
  bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
  OrbiterRowModel* takeRow(int row);
  OrbiterRowModel* find(const QString &id) const;
  QModelIndex indexFromItem( const OrbiterRowModel* item) const;
  void clear();

private slots:
  void handleItemChange();

private:
  OrbiterRowModel* m_prototype;
  QList <OrbiterRowModel* > m_list;

};

#endif // ORBITERROOMMODEL_H
