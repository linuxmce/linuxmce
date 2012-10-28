#ifndef MEDIATYPEMODEL_H
#define MEDIATYPEMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QVariant>
#include <datamodels/DataModelItems/mediatypeitem.h>
#include <QStringList>

class MediaSubTypeItem;

class MediaSubTypeModel: public QAbstractListModel
{
  Q_OBJECT

public:
  explicit MediaSubTypeModel(MediaSubTypeItem* prototype, QObject* parent = 0);
  ~MediaSubTypeModel();
#ifdef QT5
   QHash<int, QByteArray> roleNames() const;
#endif
  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  void appendRow(MediaSubTypeItem* item);
  void appendRows(const QList<MediaSubTypeItem*> &items);
  void insertRow(int row, MediaSubTypeItem* item);
  bool removeRow(int row, const QModelIndex &parent = QModelIndex());
  bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
  MediaSubTypeItem* takeRow(int row);
  MediaSubTypeItem* find(const QString &id) const;
  QModelIndex indexFromItem( const MediaSubTypeItem* item) const;
  MediaSubTypeItem* currentRow();
  void sortModel(int column, Qt::SortOrder order);


  void clear();

  QModelIndex lastrow;
  QString ident;

  Q_INVOKABLE void setSelectionStatus(QString format);
  Q_INVOKABLE bool getSelectionStatus();
  void ReturnSelectedItems();

signals:
  void SetTypeSort(int pos, QString qs_sorting_string);
  void ItemAdded();
  void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const bool &state);

private slots:
  void handleItemChange();

private:
  MediaSubTypeItem* m_prototype;
  QList<MediaSubTypeItem*> m_list;
};


#endif // MEDIATYPEMODEL_H
