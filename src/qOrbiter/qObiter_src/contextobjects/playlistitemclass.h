#ifndef PLAYLISTITEMCLASS_H
#define PLAYLISTITEMCLASS_H

/* The purpose of this class is to keep a reference to all items in the current playlist, to notify the ui of playlist changes,
  and to eventually allow the user to change the order of the playlist on the fly. role names subject to change
  */

#include <QAbstractListModel>
#include <QVariant>
#include <QImage>
#include <QMap>

class PlaylistItemClass: public QObject
{
 Q_OBJECT


public:
  enum Roles {
    NameRole = Qt::UserRole+1,
    IndexRole =Qt::UserRole+2,
    PathRole= Qt::DisplayRole+3,

    AttributeRole = Qt::UserRole+5,
    FKRole = Qt::DisplayRole+6

  };

public:
  PlaylistItemClass(QObject *parent = 0) {}
  explicit PlaylistItemClass( QString &ident, QString &name,  QString &path,  int &index,   QObject *parent = 0);
  QVariant data(int role) const;
  QHash<int, QByteArray> roleNames() const;

  inline QString id() const {  return m_fk_file.toUtf8(); }
  inline QString name() const { return m_name.toLatin1(); }
  inline int index() const { return m_index; }
  inline QString path() const { return m_path; }

  inline QMap <QString*, int> attributes() const {return m_mapAttrib;}


private:
  QString m_name;
  int m_index;
  QString m_path;

  QString m_fk_file;
  QMap <QString*, int> m_mapAttrib;

signals:
  void imageChanged();
  void dataChanged();
};

#endif // PLAYLISTITEMCLASS_H
