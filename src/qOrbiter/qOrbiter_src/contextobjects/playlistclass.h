#ifndef PLAYLISTCLASS_H
#define PLAYLISTCLASS_H
/*
  This class acts as a controller for managing playlist items currently. It will be expanded to encompass the
  existing orbiter playlist management functions like saving to public and private users, reordering, etc.
  */
#include <QAbstractListModel>
#include <QList>
#include <QVariant>
#include <contextobjects/playlistitemclass.h>

class PlaylistClass: public QAbstractListModel
{
public:
  explicit PlaylistClass(PlaylistItemClass* prototype, QObject* parent = 0);
  ~PlaylistClass();
  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  void appendRow(PlaylistItemClass* item);
  void appendRows(const QList<PlaylistItemClass*> &items);
  void insertRow(int row, PlaylistItemClass* item);
  bool removeRow(int row, const QModelIndex &parent = QModelIndex());
  bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
  PlaylistItemClass* takeRow(int row);
  PlaylistItemClass* find(const QString &id) const;
  QModelIndex indexFromItem( const PlaylistItemClass* item) const;
  PlaylistItemClass* currentRow();
  bool checkDupe(QString name, QString position);
  void setItemStatus(int b);
  void clear();

signals:
  void ItemAdded();
  void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const int &sRow);

private slots:
  void handleItemChange();

private:
  PlaylistItemClass* m_prototype;
  QList<PlaylistItemClass*> m_list;
};

#endif // PLAYLISTCLASS_H
