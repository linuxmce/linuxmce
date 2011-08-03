#ifndef FILTERMODEL_H
#define FILTERMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QVariant>
#include <datamodels/DataModelItems/filtermodelitem.h>

class FilterModelItem;

class FilterModel: public QAbstractListModel
{
  Q_OBJECT

public:
  explicit FilterModel(FilterModelItem* prototype, QObject* parent = 0);
  ~FilterModel();
  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  void appendRow(FilterModelItem* item);
  void appendRows(const QList<FilterModelItem*> &items);
  void insertRow(int row, FilterModelItem* item);
  bool removeRow(int row, const QModelIndex &parent = QModelIndex());
  bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
  FilterModelItem* takeRow(int row);
  FilterModelItem* find(const QString &id) const;
  QModelIndex indexFromItem( const FilterModelItem* item) const;
  FilterModelItem* currentRow();


  void clear();

  QModelIndex lastrow;
  QString ident;  

  Q_INVOKABLE void setSelectionStatus(QString format);
  Q_INVOKABLE bool getSelectionStatus();

signals:
  void ItemAdded();
  void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const bool &state);

private slots:
  void handleItemChange();

private:
  FilterModelItem* m_prototype;
  QList<FilterModelItem*> m_list;
};

#endif // FILTERMODEL_H
