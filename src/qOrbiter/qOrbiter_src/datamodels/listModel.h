/*
 * Author: Christophe Dumez <dchris@gmail.com>
 * License: Public domain (No attribution required)
 * Website: http://cdumez.blogspot.com/
 * Version: 1.0
 */

#ifndef LISTMODEL_H
#define LISTMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QVariant>
#include <datamodels/gridItem.h>


class ListModel : public QAbstractListModel
{
  Q_OBJECT

public:
  explicit ListModel(gridItem* prototype, QObject* parent = 0);
  ~ListModel();
  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  void appendRow(gridItem* item);
  void appendRows(const QList<gridItem*> &items);
  void insertRow(int row, gridItem* item);
  bool removeRow(int row, const QModelIndex &parent = QModelIndex());
  bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
  gridItem* takeRow(int row);
  gridItem* find(const QString &id) const;
  Q_INVOKABLE QVariant get(int index, const QString &name) const;
  QModelIndex indexFromItem( const gridItem* item) const;
  gridItem* currentRow();
  void clear();

signals:
  void ItemAdded();
  void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const int &sRow);

private slots:
  void handleItemChange();

private:
  gridItem* m_prototype;
  QList<gridItem*> m_list;
};

#endif // LISTMODEL_H
