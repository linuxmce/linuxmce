#ifndef MEDIATYPEMODEL_H
#define MEDIATYPEMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QVariant>
#include <datamodels/DataModelItems/mediatypeitem.h>

class MediaTypeItem;

class MediaTypeModel: public QAbstractListModel
{
  Q_OBJECT

public:
  explicit MediaTypeModel(MediaTypeItem* prototype, QObject* parent = 0);
  ~MediaTypeModel();
  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  void appendRow(MediaTypeItem* item);
  void appendRows(const QList<MediaTypeItem*> &items);
  void insertRow(int row, MediaTypeItem* item);
  bool removeRow(int row, const QModelIndex &parent = QModelIndex());
  bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
  MediaTypeItem* takeRow(int row);
  MediaTypeItem* find(const QString &id) const;
  QModelIndex indexFromItem( const MediaTypeItem* item) const;
  MediaTypeItem* currentRow();


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
  MediaTypeItem* m_prototype;
  QList<MediaTypeItem*> m_list;
};


#endif // MEDIATYPEMODEL_H
