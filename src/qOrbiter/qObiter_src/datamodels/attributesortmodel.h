#ifndef ATTRIBUTESORTMODEL_H
#define ATTRIBUTESORTMODEL_H


#include <QAbstractListModel>
#include <QList>
#include <QVariant>
#include <datamodels/DataModelItems/attributesortitem.h>
#include <QStringList>

class AttributeSortItem;

class AttributeSortModel: public QAbstractListModel
{
  Q_OBJECT

public:
  explicit AttributeSortModel(AttributeSortItem* prototype, QObject* parent = 0);
  ~AttributeSortModel();
  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  void appendRow(AttributeSortItem* item);
  void appendRows(const QList<AttributeSortItem*> &items);
  void insertRow(int row, AttributeSortItem* item);
  bool removeRow(int row, const QModelIndex &parent = QModelIndex());
  bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
  AttributeSortItem* takeRow(int row);
  AttributeSortItem* find(const QString &id) const;
  QModelIndex indexFromItem( const AttributeSortItem* item) const;
  AttributeSortItem* currentRow();

  void ReturnSelectedItems();
  void clear();

  QModelIndex lastrow;
  QString ident;

  Q_INVOKABLE void setSelectionStatus(QString format);
  Q_INVOKABLE bool getSelectionStatus();

signals:
  void ItemAdded();
  void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const bool &state);
   void SetTypeSort(int pos, QString type);
private slots:
  void handleItemChange();

private:
  AttributeSortItem* m_prototype;
  QList<AttributeSortItem*> m_list;
};

#endif // ATTRIBUTESORTMODEL_H
