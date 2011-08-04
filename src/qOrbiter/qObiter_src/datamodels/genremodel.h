#ifndef GENREMODEL_H
#define GENREMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QVariant>
#include <datamodels/DataModelItems/genreitem.h>
#include <QStringList>


class GenreModel : public QAbstractListModel
{
  Q_OBJECT

public:
  explicit GenreModel(GenreItem* prototype, QObject* parent = 0);
  ~GenreModel();
  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  void appendRow(GenreItem* item);
  void appendRows(const QList<GenreItem*> &items);
  void insertRow(int row, GenreItem* item);
  bool removeRow(int row, const QModelIndex &parent = QModelIndex());
  bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
  GenreItem* takeRow(int row);
  GenreItem* find(const QString &id) const;
  QModelIndex indexFromItem( const GenreItem* item) const;
  GenreItem* currentRow();
  void clear();
  void ReturnSelectedItems();
  QModelIndex lastrow;
  QString ident;
  Q_INVOKABLE void setSelectionStatus(QString format);
  Q_INVOKABLE bool getSelectionStatus();
signals:
  void ItemAdded();
  void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const int &sRow);
   void SetTypeSort(int pos, QString type);
private slots:
  void handleItemChange();

private:
  GenreItem* m_prototype;
  QList<GenreItem*> m_list;
};

#endif // GENREMODEL_H
