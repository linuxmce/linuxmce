#ifndef GENERICSETUPMODEL_H
#define GENERICSETUPMODEL_H
#include <QObject>
#include <QAbstractListModel>
#include "genericsetupitem.h"
#include <qorbitermanager.h>


class qorbiterManager;

class GenericSetupModel : public QAbstractListModel
{
  Q_OBJECT

public:
    explicit GenericSetupModel(GenericSetupItem* prototype, qorbiterManager *ref);
  ~GenericSetupModel();
#ifdef QT5
   QHash<int, QByteArray> roleNames() const;
#endif
  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  void appendRow(GenericSetupItem* item);
  void appendRows(const QList<GenericSetupItem*> &items);
  void insertRow(int row, GenericSetupItem* item);
  bool removeRow(int row, const QModelIndex &parent = QModelIndex());
  bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
  GenericSetupItem* takeRow(int row);
  GenericSetupItem* find(const QString &id) const;
  Q_INVOKABLE QVariant get(int index, const QString &name) const;
  QModelIndex indexFromItem( const GenericSetupItem* item) const;
  GenericSetupItem* currentRow();
  void clear();
  int totalcells;
  void sortModel(int column, Qt::SortOrder order);



signals:
  void ItemAdded();
  void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const int &sRow);

public slots:
  void checkForMore();

private slots:
  void handleItemChange();

private:
  GenericSetupItem* m_prototype;
  QList<GenericSetupItem*> m_list;
  qorbiterManager *manager_ref;
};
#endif // GENERICSETUPMODEL_H
